
#pragma once 

#include <Seamless/Export.hpp>
#include <Seamless/Event.hpp>
#include <Seamless/Timer.hpp>

#include <cstdint>
#include <memory>
#include <thread>
#include <mutex>
#include <queue>
#include <atomic>

namespace cmls
{

  enum AsyncTaskStatus : uint8_t
  {
    AS_Fresh = 0,
    AS_Queued,
    AS_Preparing,
    AS_Prepared,
    AS_Executing,
    AS_Executed,
    AS_Finalizing,
    AS_Finalized
  };

  enum AsyncProcessStatus : uint8_t
  {
    PS_Fresh = 0,
    PS_Running,
    PS_Success,
    PS_Error
  };

  class CMLS_API AsyncTask
  {
  public:
    Event<> onCompleted;

    AsyncTaskStatus status();

    void status(AsyncTaskStatus newStatus);

    void runSynchronously();

    virtual void prepare();
    virtual void execute();
    virtual void finalize();

  private:
    std::atomic<AsyncTaskStatus> m_status { AS_Fresh };
  };

  using AsyncTaskPtr = std::shared_ptr<AsyncTask>;
  using AsyncTaskWPtr = std::weak_ptr<AsyncTask>;

  class CMLS_API AsyncProcess : public AsyncTask
  {
  public:
    static std::shared_ptr<AsyncProcess> create(std::string const& commandline, std::vector<std::string> env = {});

    AsyncProcess(std::string const & commandline, std::vector<std::string> env = {});
    virtual ~AsyncProcess();

    virtual void execute() override;

    AsyncProcessStatus processStatus();

    inline int32_t returnCode()
    {
      return m_returnCode;
    }

    std::string const& output() const;

    Event<> onExecute;

  protected:

    // input 
    std::string m_commandline;
    std::vector<std::string> m_environment;
    std::string m_workingDirectory;

    // runtime
    int32_t m_pid = 0;

    // output
    int32_t m_returnCode = 0;
    std::string m_output;

    std::atomic<AsyncProcessStatus> m_processStatus { PS_Fresh };
  };

  typedef std::shared_ptr<AsyncProcess> AsyncProcessPtr;
  typedef std::weak_ptr<AsyncProcess> AsyncProcessWPtr;

  class CMLS_API AsyncThread
  {
  public:
    AsyncThread();

    virtual ~AsyncThread();

    void kill();

    void queueTaskExecution(AsyncTaskPtr task);

    template <typename IterType>
    void queueTaskExecution(IterType begin, IterType end)
    {
      std::scoped_lock lock(m_incomingMutex);

      while (begin != end)
      {
        m_incomingTasks.push(*begin);
        ++begin;
      }
    }

    std::queue<AsyncTaskPtr> fetchExecutedTasks();

  protected:
    std::thread m_thread;
    std::mutex m_incomingMutex;
    std::mutex m_outgoingMutex;

    std::atomic_bool m_shouldKill{ false };

    std::queue<AsyncTaskPtr> m_incomingTasks;
    std::queue<AsyncTaskPtr> m_outgoingTasks;

  };

  // Manages async operations from the main thread
  class CMLS_API AsyncContext
  {
  public:

    AsyncContext(uint32_t numThreads = 4);
    virtual ~AsyncContext();

    void queueTask(AsyncTaskPtr newTask);

    void tick();

  protected:
    std::vector<AsyncThread*> m_threadPool;
    std::queue<AsyncTaskPtr> m_outgoingTasks;
    std::queue<AsyncTaskPtr> m_incomingTasks;
    std::mutex m_outgoingMutex;
    std::mutex m_incomingMutex;

    uint32_t m_nextThread = 0;

    AsyncThread *nextThread();

  };
}