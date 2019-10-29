
#include "Seamless/Request.hpp"

#include "Seamless/Error.hpp"

cmls::Request::Request()
{

}

cmls::Request::~Request()
{
}

cmls::HttpPath const& cmls::Request::path() const
{
  return m_path;
}

cmls::Request& cmls::Request::path(cmls::HttpPath const& newPath)
{
  m_path = newPath;
  return *this;
}

cmls::HttpQuery const& cmls::Request::query() const
{
  return m_query;
}

cmls::Request& cmls::Request::query(cmls::HttpQuery const& newQuery)
{
  m_query = newQuery;
  return *this;
}

cmls::HttpMethod cmls::Request::method() const
{
  return m_method;
}

cmls::Request &cmls::Request::method(cmls::HttpMethod method)
{
  m_method = method;
  return *this;
}



cmls::Request& cmls::Request::rawUrl(cmls::HttpUrl const& newUrl)
{
  m_rawUrl = newUrl;
  return *this;
}

cmls::HttpUrl const& cmls::Request::rawUrl() const
{
  return m_rawUrl;
}


cmls::Stream const &cmls::Request::entity() const
{
  return m_entity;
}

void cmls::Request::entity(cmls::Stream const &newStream)
{
  m_entity = newStream;
}
