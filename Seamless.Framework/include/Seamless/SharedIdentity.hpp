
#pragma once

#include <Seamless/Export.hpp>

#include <cstdint>

/** A threadsafe baseclass that maintains a shared, copyable identity */
class CMLS_API SharedIdentity
{
public:
  SharedIdentity();

  virtual ~SharedIdentity();

  SharedIdentity(SharedIdentity const & copy);

  SharedIdentity & operator=(SharedIdentity const & other);

  inline bool operator==(SharedIdentity const & other) const
  {
    return m_id == other.m_id;
  }

  inline bool operator<(SharedIdentity const & other) const
  {
    return m_id < other.m_id;
  }

  inline uint64_t const & getId() const
  {
    return m_id;
  }

protected:
  uint64_t m_id = 0;
};