
#include "Seamless/SharedIdentity.hpp"

#include "Seamless/Error.hpp"

#include <set>
#include <map>
#include <mutex>

namespace
{

  uint64_t nextAvailableId = 0;

  std::set<uint64_t> recycledIds;
  std::map<uint64_t, uint64_t> idUsage;

  std::recursive_mutex mutex;

  void decreaseIdUsage(uint64_t id)
  {
    std::scoped_lock lock(mutex);
    auto finder = ::idUsage.find(id);
    if (finder != ::idUsage.end())
    {
      if (finder->second > 0)
      {
        finder->second--;
      }

      if (finder->second == 0)
      {
        ::idUsage.erase(id);
        ::recycledIds.insert(id);
      }
    }
    else
    {
      // We shouldn't ever be in this position, but if we ever find ourselves in it, this will mitigate the issue quite effectively
      ::recycledIds.insert(id);
      LogWarning("Attempted to decrease id usage that wasn't used, will recycle it in hopes for the best (%ull)", id);
    }
  }

  void increaseIdUsage(uint64_t id)
  {
    std::scoped_lock lock(mutex);
    auto finder = ::idUsage.find(id);
    if (finder != ::idUsage.end())
    {
      finder->second++;
    }
    else
    {
      ::idUsage[id] = 1;
    }
  }

  uint64_t newId()
  {
    if (!::recycledIds.empty())
    {
      uint64_t id = *::recycledIds.begin();
      ::recycledIds.erase(id);
      increaseIdUsage(id);
      return id;
    }

    std::scoped_lock lock(mutex);
    uint64_t id = nextAvailableId++;
    increaseIdUsage(id);
    return id;
  }
}

SharedIdentity & SharedIdentity::operator=(SharedIdentity const & other)
{
  if (m_id != other.m_id)
  {
    ::decreaseIdUsage(m_id);
    m_id = other.m_id;
    ::increaseIdUsage(m_id);
  }

  return *this;
}

SharedIdentity::SharedIdentity(SharedIdentity const & copy)
{
  m_id = copy.m_id;
  ::increaseIdUsage(m_id);
}

SharedIdentity::SharedIdentity()
{
  m_id = ::newId();
}

SharedIdentity::~SharedIdentity()
{
  ::decreaseIdUsage(m_id);
}
