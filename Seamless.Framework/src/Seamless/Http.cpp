
#include "Seamless/Http.hpp"

#include "Seamless/String.hpp"
#include "Seamless/Error.hpp"

#include <string>
#include <regex>

cmls::HttpScheme cmls::httpScheme(std::string const& scheme)
{
  if (cmls::strToLower(scheme) == "http")
  {
    return HS_Http;
  }
  
  if (cmls::strToLower(scheme) == "https")
  {
    return HS_Https;
  }

  return HS_Unknown;
}

std::string cmls::toString(cmls::HttpScheme scheme)
{
  if(scheme == HS_Http)
  {
    return "http";
  }

  if(scheme == HS_Https)
  {
    return "https";
  }

  return "invalid";
}

cmls::HttpMethod cmls::httpMethod(std::string const& method)
{
  std::map<std::string, cmls::HttpMethod> const static methodMap = {
    {"GET", HM_Get},
    {"HEAD", HM_Head},
    {"POST", HM_Post},
    {"PUT", HM_Put},
    {"DELETE", HM_Delete},
    {"CONNECT", HM_Connect},
    {"OPTIONS", HM_Options},
    {"TRACE", HM_Trace},
    {"PATCH", HM_Patch},
  };

  auto ml = cmls::strToUpper(method);
  auto f = methodMap.find(ml);
  if(f != methodMap.end())
  {
    return f->second;
  }

  return HM_None;
}

std::string cmls::toString(cmls::HttpMethod method)
{
  switch (method)
  {
    case HM_Get:
      return "GET";
      break;
    case HM_Head:
      return "HEAD";
      break;
    case HM_Post:
      return "POST";
      break;
    case HM_Put:
      return "PUT";
      break;
    case HM_Delete:
      return "DELETE";
      break;
    case HM_Connect:
      return "CONNECT";
      break;
    case HM_Options:
      return "OPTIONS";
      break;
    case HM_Trace:
      return "TRACE";
      break;
    case HM_Patch:
      return "PATCH";
      break;
  }


  return "";
}

cmls::HttpPath::HttpPath()
{
}

cmls::HttpPath::HttpPath(std::string const& path)
{
  m_parts.clear();
  m_isDirectory = false;

  push(path);
}

std::string cmls::HttpPath::toString() const
{
  std::string returner = "/";
  returner += cmls::join(m_parts, "/");

  if (m_isDirectory)
  {
    returner += "/";
  }

  return returner;
}

std::vector<std::string> const& cmls::HttpPath::parts()
{
  return m_parts;
}

cmls::HttpPath& cmls::HttpPath::push(std::string const& part)
{
  if (part.size() == 0)
  {
    return *this;
  }

  m_isDirectory = part[part.size()-1] == '/';

  auto parts = cmls::split(part, {'/'});
  for(auto p : parts)
  {
    if (p == ".")
    {
      continue;
    }

    if (p == "..")
    {
      if(!m_parts.empty())
      {
        m_parts.pop_back();
      }
      continue;
    }

    m_parts.push_back(p);
  }

  return *this;
}

cmls::HttpPath& cmls::HttpPath::pop()
{
  m_parts.pop_back();

  return *this;
}

bool cmls::HttpPath::isDirectory() const
{
  return m_isDirectory;
}

bool cmls::HttpPath::isFile() const
{
  return !m_isDirectory;
}

cmls::HttpDomain::HttpDomain()
{
}

cmls::HttpDomain::HttpDomain(std::string const& domain)
{
  m_parts = cmls::split(domain, {'.'});
}

std::string cmls::HttpDomain::toString() const
{
  return cmls::join(m_parts, ".");
}

std::vector<std::string> const& cmls::HttpDomain::parts() const
{
  return m_parts;
}

std::string cmls::HttpDomain::topLevel() const
{
  if (m_parts.size() > 0)
  {
    return m_parts[m_parts.size() - 1];
  }

  LogWarning("out of bounds");
  return "";
}

std::string cmls::HttpDomain::secondLevel() const
{
  if (m_parts.size() > 1)
  {
    return m_parts[m_parts.size() - 2];
  }

  LogWarning("out of bounds");
  return "";
}

std::string cmls::HttpDomain::subLevel(uint32_t index) const
{
  int32_t idx = int32_t(m_parts.size()) - (int32_t(index) + 2);

  if (idx >= 0 && idx < m_parts.size())
  {
    return m_parts[idx];
  }

  LogWarning("out of bounds");
  return "";
}

uint32_t cmls::HttpDomain::subLevels() const
{
  auto numParts = m_parts.size();
  return  numParts > 2 ? numParts - 2 : 0;
}

cmls::HttpQuery::HttpQuery()
{
}

cmls::HttpQuery::HttpQuery(std::string const& ampersandDelimitedList)
{
  // @todo url decoding
  auto vars = cmls::split(ampersandDelimitedList, {'&'});
  for (auto v : vars)
  {
    auto parts = cmls::split(v, {'='});
    if (parts.size() != 2)
    {
      continue;
    }

    m_values[parts[0]] = parts[1];
  }
}

cmls::HttpQuery::HttpQuery(std::map<std::string, std::string> const& values)
{
  m_values = values;
}

std::string cmls::HttpQuery::toString() const
{
  std::string returner;
  bool first = true;
  for (auto v : m_values)
  {
    if (first)
    {
      first = false;
    }
    else
    {
      returner += "&";
    }

    // @todo url encoding
    returner += v.first + "=" + v.second;
  }

  return returner;
}

std::string cmls::HttpQuery::get(std::string const& name) const
{
  auto f = m_values.find(name);
  if (f != m_values.end())
  {
    return f->second;
  }

  LogWarning("Attempted to get invalid parameter, returning empty string");

  return "";
}

cmls::HttpQuery &cmls::HttpQuery::set(std::string const& name, std::string const& value)
{
  m_values[name] = value;

  return *this;
}

cmls::HttpQuery &cmls::HttpQuery::remove(std::string const& name)
{
  m_values.erase(name);

  return *this;
}

bool cmls::HttpQuery::exists(std::string const& name) const
{
  return m_values.find(name) == m_values.end();
}

std::map<std::string, std::string> const& cmls::HttpQuery::values() const
{
  return m_values;
}

cmls::HttpUrl::HttpUrl()
{
}

cmls::HttpUrl::HttpUrl(std::string const& fullUrl)
{
  auto rfcRegexString = "^(([^:/?#]+):)?(//([^/?#]*))?([^?#]*)(\\?([^#]*))?(#(.*))?";
  std::regex rfcRegex(rfcRegexString);
  std::smatch rfcMatch;
  if (std::regex_search(fullUrl, rfcMatch, rfcRegex))
  {
    auto schemeStr = rfcMatch[2].str();
    if (!schemeStr.empty())
    {
      m_scheme = cmls::httpScheme(schemeStr);
    }
    else
    {
      m_scheme = HS_Http;
    }

    auto ds = cmls::split(rfcMatch[4].str(), {':'});
    if (ds.size() > 0)
    {
      m_domain = cmls::HttpDomain(ds[0]);
    }
    if (ds.size() > 1)
    {
      m_port = std::stoi(ds[1]);
    }
    else
    {
      m_port = 80;
    }

    m_path = cmls::HttpPath(rfcMatch[5].str());

    m_query = cmls::HttpQuery(rfcMatch[7].str());
    m_fragment = rfcMatch[9].str();
    return;
  }
  else
  {
    LogWarning("Invalid URL");
  }

  m_domain = cmls::HttpDomain("localhost");
  m_fragment = cmls::HttpFragment("");
  m_path = cmls::HttpPath();
  m_port = cmls::HttpPort(80);
  m_query = cmls::HttpQuery();
  m_scheme = cmls::HttpScheme(HS_Http);
}

std::string cmls::HttpUrl::toString() const
{
  // Scheme and domain are always relevant
  std::string url = cmls::toString(scheme()) + "://" + domain().toString();

  // Port is only relevant if the scheme and port differs
  if (!( (scheme() == HS_Http && port() == 80) || (scheme() == HS_Https && port() == 443)))
  {
    url += ":" + std::to_string(port());
  }

  // Path is always relevant
  url += path().toString();

  // Query is only relevant if it has values
  auto queryString = query().toString();
  if (queryString.size() > 0)
  {
    url += "?" + queryString;
  }

  // Fragment is only relevant if its set
  auto frag = fragment();
  if (frag.size() > 0)
  {
    url += "#" + frag;
  }

  return url;
}

cmls::HttpScheme const &cmls::HttpUrl::scheme() const
{
  return m_scheme;
}

cmls::HttpDomain const &cmls::HttpUrl::domain() const
{
  return m_domain;
}

cmls::HttpPort const &cmls::HttpUrl::port() const
{
  return m_port;
}

cmls::HttpPath const &cmls::HttpUrl::path() const
{ 
  return m_path;
}

cmls::HttpQuery const &cmls::HttpUrl::query() const
{
  return m_query;
}

cmls::HttpFragment const &cmls::HttpUrl::fragment() const
{
  return m_fragment;
}

cmls::HttpUrl &cmls::HttpUrl::scheme(HttpScheme newScheme)
{
  m_scheme = newScheme;
  return *this;
}

cmls::HttpUrl &cmls::HttpUrl::domain(HttpDomain const& newDomain)
{
  m_domain = newDomain;
  return *this;
}

cmls::HttpUrl &cmls::HttpUrl::port(HttpPort newPort)
{
  m_port = newPort;
  return *this;
}

cmls::HttpUrl &cmls::HttpUrl::path(HttpPath const& newPath)
{
  m_path = newPath;
  return *this;
}

cmls::HttpUrl &cmls::HttpUrl::query(HttpQuery const& newQuery)
{
  m_query = newQuery;
  return *this;
}

cmls::HttpUrl &cmls::HttpUrl::fragment(HttpFragment const& newFragment)
{
  m_fragment = newFragment;
  return *this;
}

cmls::HttpHeaderMap const& cmls::HttpHeaderOwner::headers() const
{
  return m_headers;
}

void cmls::HttpHeaderOwner::headers(cmls::HttpHeaderMap const& newHeaders)
{
  m_headers = newHeaders;
}


void cmls::HttpHeaderOwner::header(std::string const& name, std::string const& value)
{
  m_headers[name] = value;
}

std::string cmls::HttpHeaderOwner::header(std::string const& name) const
{
  auto finder = m_headers.find(name);
  if (finder != m_headers.end())
  {
    return finder->second;
  }

  LogWarning("Non-existent header");

  return "";
}

bool cmls::HttpHeaderOwner::headerIsSet(std::string const& name) const
{
  auto finder = m_headers.find(name);
  return (finder != m_headers.end());
}