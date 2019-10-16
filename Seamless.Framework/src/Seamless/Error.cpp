
#include "Seamless/Error.hpp"

#include <mutex>
#include <iostream>
#include <set>

namespace
{
  std::recursive_mutex handlerMutex;
  std::recursive_mutex logMutex;
  std::set<cmls::LogHandler*> handlers;
  bool wasInitialized = false;
  void assertInitialized()
  {
    if (!wasInitialized)
    {
      wasInitialized = true;
    }
  }
}

void cmls::Log::registerHandler(LogHandler* handler)
{
  std::scoped_lock lock(::handlerMutex);
  ::handlers.insert(handler);
}

void cmls::Log::unregisterHandler(LogHandler* handler)
{
  std::scoped_lock lock(::handlerMutex);
  ::handlers.erase(handler);
}

void cmls::Log::writeLine(std::string const & line)
{
  assertInitialized();

  std::scoped_lock lock(::logMutex);

  std::cout << line.c_str() << std::endl;
  std::cout.flush();

  std::scoped_lock hlock(::handlerMutex);
  for (auto h : ::handlers)
  {
    h->writeLine(line);
  }
}

void cmls::Log::close()
{

}
