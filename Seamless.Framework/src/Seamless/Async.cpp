
#include "Seamless/Async.hpp"

#include "Seamless/Error.hpp"
#include "Seamless/String.hpp"
#include "Seamless/Utility.hpp"
#include "Seamless/Stream.hpp"

#define WIN32_MEAN_AND_LEAN
#include <Windows.h>

std::shared_ptr<cmls::AsyncProcess> cmls::AsyncProcess::create(std::string const& commandline, std::vector<std::string> env)
{
  return std::make_shared<AsyncProcess>(commandline, env);
}

cmls::AsyncProcess::AsyncProcess(std::string const & commandline, std::vector<std::string> env)
{
  m_commandline = commandline;
  m_environment = env;
  m_processStatus.store(PS_Fresh);
}

cmls::AsyncProcessStatus cmls::AsyncProcess::processStatus()
{
  return (AsyncProcessStatus)m_processStatus;
}

std::string const& cmls::AsyncProcess::output() const
{
  return m_output;
}

cmls::AsyncProcess::~AsyncProcess()
{
}

void cmls::AsyncProcess::execute()
{
  m_processStatus.store(PS_Running);
  onExecute();

  SECURITY_ATTRIBUTES stdOutAttribs;
  stdOutAttribs.nLength = sizeof(SECURITY_ATTRIBUTES);
  stdOutAttribs.bInheritHandle = TRUE;
  stdOutAttribs.lpSecurityDescriptor = NULL;

  HANDLE stdOutWrite = NULL;
  HANDLE stdOutRead = NULL;
  if (!CreatePipe(&stdOutRead, &stdOutWrite, &stdOutAttribs, 0))
  {
    LogError("CreatePipe failed (%d).\n", GetLastError());
    m_processStatus.store(PS_Error);
    return;
  }

  if (!SetHandleInformation(stdOutRead, HANDLE_FLAG_INHERIT, 0))
  {
    LogError("SetHandleInformation failed (%d).\n", GetLastError());
    m_processStatus.store(PS_Error);
    CloseHandle(stdOutRead);
    CloseHandle(stdOutWrite);
    return;
  }
  
  STARTUPINFOA si;
  ZeroMemory(&si, sizeof(si));
  si.cb = sizeof(si);
  si.dwFlags |= STARTF_USESTDHANDLES;
  si.hStdOutput = stdOutWrite;
  
  PROCESS_INFORMATION pi;
  ZeroMemory(&pi, sizeof(pi));

  char *cmd = new char[8192];
  snprintf(cmd, 8192, "%s", m_commandline.c_str());

  cmls::Stream envBlock;
  for (auto envVar : m_environment)
  {
    envVar = cmls::trim(envVar);
    envBlock.write(reinterpret_cast<uint8_t const*>(envVar.c_str()), envVar.size());
    envBlock.write(reinterpret_cast <uint8_t const*>("\0"), 1);
  }
  envBlock.write(reinterpret_cast <uint8_t const*>("\0\0"), 2);

  LPVOID envBlockPtr = m_environment.size() > 0 ? const_cast<uint8_t*>(envBlock.begin()) : nullptr;

  // Start the child process. 
  if (!CreateProcessA(NULL,   // No module name (use command line)
    cmd,        // Command line
    NULL,           // Process handle not inheritable
    NULL,           // Thread handle not inheritable
    TRUE,          // Set handle inheritance to TRUE
    0,              // No creation flags
    envBlockPtr,           // Use parent's environment block
    NULL,           // Use parent's starting directory 
    &si,            // Pointer to STARTUPINFO structure
    &pi)           // Pointer to PROCESS_INFORMATION structure
    )
  {
    LogError("CreateProcess failed (%d).\n", GetLastError());
    m_processStatus.store(PS_Error);
    CloseHandle(stdOutRead);
    CloseHandle(stdOutWrite);
    return;
  }

  bool processExited = false;
  while (!processExited)
  {
    processExited = WaitForSingleObject(pi.hProcess, 10) == WAIT_OBJECT_0;

    while (true)
    {
      char buf[1024];
      DWORD dwRead = 0;
      DWORD dwAvail = 0;

      if (!PeekNamedPipe(stdOutRead, NULL, 0, NULL, &dwAvail, NULL))
      {
        break;
      }

      if (!dwAvail)
      {
        break;
      }

      if (!ReadFile(stdOutRead, buf, min(sizeof(buf) - 1, dwAvail), &dwRead, NULL) || !dwRead)
      {
        break;
      } 

      buf[dwRead] = 0;
      m_output += buf;
    }
  }

  DWORD ret = 0;
  if (!GetExitCodeProcess(pi.hProcess, &ret))
  {
    LogWarning("Failed to get exit code for process");
  }
  else
  {
    m_returnCode = ret;
  }

  CloseHandle(stdOutRead);
  CloseHandle(stdOutWrite);
  CloseHandle(pi.hProcess);
  CloseHandle(pi.hThread);

  m_processStatus.store(PS_Success);
}



cmls::AsyncThread::AsyncThread()
{
  m_thread = std::thread([this]() {
    while (true)
    {
      if (m_shouldKill.load())
      {
        {
          std::scoped_lock ilock(m_incomingMutex);
          std::scoped_lock olock(m_outgoingMutex);
          while (!m_incomingTasks.empty()) m_incomingTasks.pop();
          while (!m_outgoingTasks.empty()) m_outgoingTasks.pop();
        }
        return;
      }

      // Fetch a task from the incoming queue
      AsyncTaskPtr nextTask;
      {
        std::scoped_lock lock(m_incomingMutex);
        if (!m_incomingTasks.empty())
        {
          nextTask = m_incomingTasks.front();
          m_incomingTasks.pop();
        }
      }

      if (!nextTask)
      {
        //@todo
        //std::this_thread::sleep_for(std::chrono::milliseconds(50));
        continue;
      }

      nextTask->status(AS_Executing);
      nextTask->execute();
      nextTask->status(AS_Executed);

      // Insert the copied set into the executed task set
      {
        std::scoped_lock lock(m_outgoingMutex);
        m_outgoingTasks.push(nextTask);
      }
      // @todo
      //std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
  });
}

cmls::AsyncThread::~AsyncThread()
{
  kill();
}

cmls::AsyncContext::AsyncContext(uint32_t numThreads)
{
  for (uint32_t i = 0; i < numThreads; i++)
  {
    m_threadPool.push_back(new AsyncThread());
  }
}

cmls::AsyncContext::~AsyncContext()
{
  std::scoped_lock ilock(m_incomingMutex);
  std::scoped_lock olock(m_outgoingMutex);

  while (!m_incomingTasks.empty()) m_incomingTasks.pop();
  while (!m_outgoingTasks.empty()) m_outgoingTasks.pop();

  for (auto thr : m_threadPool)
  {
    delete thr;
  }

  m_threadPool.clear();
}

void cmls::AsyncTask::execute()
{
}

void cmls::AsyncTask::finalize()
{
}

void cmls::AsyncTask::prepare()
{
}

void cmls::AsyncTask::runSynchronously()
{
  status(AS_Preparing);
  prepare();

  status(AS_Executing);
  execute();

  status(AS_Finalizing);
  finalize();

  status(AS_Finalized);
}

void cmls::AsyncTask::status(AsyncTaskStatus newStatus)
{
  m_status.store(newStatus);
}

cmls::AsyncTaskStatus cmls::AsyncTask::status()
{
  return m_status.load();
}


void cmls::AsyncThread::kill()
{
  if (m_shouldKill)
  {
    return;
  }

  m_shouldKill = true;
  m_thread.join();
}

void cmls::AsyncThread::queueTaskExecution(cmls::AsyncTaskPtr task)
{
  std::scoped_lock lock(m_incomingMutex);
  m_incomingTasks.push(task);
}

std::queue< cmls::AsyncTaskPtr > cmls::AsyncThread::fetchExecutedTasks()
{
  std::scoped_lock lock(m_outgoingMutex);
  std::queue<cmls::AsyncTaskPtr> returner;
  m_outgoingTasks.swap(returner);
  return returner;
}

void cmls::AsyncContext::queueTask(cmls::AsyncTaskPtr newTask)
{
  {
    std::scoped_lock lock(m_outgoingMutex);
    m_outgoingTasks.push(newTask);
  }

  newTask->status(AS_Queued);
}

void cmls::AsyncContext::tick()
{
  // Load and copy the outgoing queue 
  std::queue<cmls::AsyncTaskPtr> tempQueue;
  {
    std::scoped_lock lock(m_outgoingMutex);
    tempQueue.swap(m_outgoingTasks);
  }

  // Prepare and add the tasks to a workload
  std::vector<cmls::AsyncTaskPtr> workload;
  while (!tempQueue.empty())
  {
    cmls::AsyncTaskPtr task = tempQueue.front();
    tempQueue.pop();

    task->status(AS_Preparing);
    task->prepare();
    task->status(AS_Prepared);

    workload.push_back(task);
  }

  // Euclidean-divide the workload amongst the threads
  auto dividedWork = cmls::euclideanDivide(workload, m_threadPool.size());

  // Distribute the divided work to the threads
  for (auto work : dividedWork)
  {
    auto currThread = nextThread();
    currThread->queueTaskExecution(work.begin(), work.end());
  }

  // Fetch the executed tasks from each thread and finalize them
  for (auto thr : m_threadPool)
  {
    auto tasks = thr->fetchExecutedTasks();
    while (!tasks.empty())
    {
      auto task = tasks.front();
      tasks.pop();

      task->status(AS_Finalizing);
      task->finalize();
      task->status(AS_Finalized);
      task->onCompleted();
    }
  }
}

cmls::AsyncThread *cmls::AsyncContext::nextThread()
{
  if (m_threadPool.empty())
  {
    return nullptr;
  }

  auto returner = m_threadPool[m_nextThread];

  m_nextThread++;
  if (m_nextThread >= m_threadPool.size())
  {
    m_nextThread = 0;
  }

  return returner;
}
