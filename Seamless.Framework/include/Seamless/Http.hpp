
#pragma once

#include <Seamless/Export.hpp>

#include <map>
#include <vector>
#include <string>
#include <cstdint>

namespace cmls 
{

  using HttpHeaderMap = std::map<std::string, std::string>;

  class CMLS_API HttpHeaderOwner
  {
  public:

    cmls::HttpHeaderMap const &headers() const;

    void headers(cmls::HttpHeaderMap const &newHeaders);

    void header(std::string const &name, std::string const &value);

    std::string header(std::string const &name) const;

    bool headerIsSet(std::string const &name) const;

  protected:
    cmls::HttpHeaderMap m_headers;
  };

  enum HttpScheme : uint8_t
  {
    HS_Unknown,
    HS_Http,
    HS_Https
  };

  CMLS_API HttpScheme httpScheme(std::string const &scheme);

  CMLS_API std::string toString(HttpScheme scheme);

  using HttpStatusCode = uint16_t;

  enum HttpMethod : uint16_t
  {
    HM_None     = 0b0000'0000'0000'0000,
    HM_All      = 0b1111'1111'1111'1111,

    HM_Get      = 0b0000'0000'0000'0001,
    HM_Head     = 0b0000'0000'0000'0010,
    HM_Post     = 0b0000'0000'0000'0100,
    HM_Put      = 0b0000'0000'0000'1000,
    HM_Delete   = 0b0000'0000'0001'0000,
    HM_Connect  = 0b0000'0000'0010'0000,
    HM_Options  = 0b0000'0000'0100'0000,
    HM_Trace    = 0b0000'0000'1000'0000,
    HM_Patch    = 0b0000'0001'0000'0000
  };

  CMLS_API HttpMethod httpMethod(std::string const &method);

  CMLS_API std::string toString(HttpMethod method);

  class CMLS_API HttpPath
  {
  public:
    HttpPath();

    HttpPath(std::string const &path);

    /** E.g. /users/foo/comments */
    std::string toString() const;

    /** E.g. {"users", "foo", "comments"} */
    std::vector<std::string> const &parts();

    /** Pushes an entry to the end of the path. If it ends with a */
    HttpPath &push(std::string const &part);

    /** Pops an entry from the end of the path */
    HttpPath &pop();

    /** Returns true if the path ends with / */
    bool isDirectory() const;

    /** Returns !isDirectory() */
    bool isFile() const;

  protected:
    bool m_isDirectory = false;
    std::vector<std::string> m_parts;
  };

  class CMLS_API HttpDomain
  {
  public:

    HttpDomain();

    /** Creates a new HttpDomain given a full domain name (e.g. docs.google.com) */
    HttpDomain(std::string const &domain);

    /** E.g. user.docs.google.com. */
    std::string toString() const;

    /** E.g. {"docs", "google", "com"} */
    std::vector<std::string> const &parts() const;

    /** E.g. "com" */
    std::string topLevel() const;

    /** E.g. "google" */
    std::string secondLevel() const;

    /** Returns subdomain at index, 0 being closest to topLevel */
    std::string subLevel(uint32_t index) const;

    /** Returns number of subdomain levels that exist. */
    uint32_t subLevels() const;

  protected:
    std::vector<std::string> m_parts;
  };
  
  using HttpPort = uint16_t;

  class CMLS_API HttpQuery
  {
  public:
    HttpQuery();
    HttpQuery(std::string const &ampersandDelimitedList);
    HttpQuery(std::map<std::string, std::string> const &values);

    std::string toString() const;

    /** Returns the value from the given variable, or empty if doesnt exist */
    std::string get(std::string const &name) const;

    /** Sets the given variable to value */
    HttpQuery &set(std::string const &name, std::string const &value);

    /** Removes the given variable if it exists*/
    HttpQuery &remove(std::string const &name);

    /** Returns true if the given variable exists */
    bool exists(std::string const &name) const;

    std::map<std::string, std::string> const &values() const;

  protected:
    std::map<std::string, std::string> m_values;
  };

  using HttpFragment = std::string;

  class CMLS_API HttpUrl
  {
  public:
    HttpUrl();
    HttpUrl(std::string const &fullUrl);

    /** E.g. http://docs.google.com:80/path/to/resource?foo=bar&baz=sand#foo */
    std::string toString() const;

    /** E.g. http */
    HttpScheme const &scheme() const;

    /** E.g. docs.google.com */
    HttpDomain const &domain() const;

    /** E.g. 80 */
    HttpPort const &port() const;

    /** E.g. /path/to/resource */
    HttpPath const &path() const;

    /** E.g. foo=bar&baz=sand */
    HttpQuery const &query() const;

    /** E.g. foo */
    HttpFragment const &fragment() const;

    /** E.g. http */
    HttpUrl &scheme(HttpScheme newScheme);

    /** E.g. docs.google.com */
    HttpUrl &domain(HttpDomain const &newDomain);

    /** E.g. 80 */
    HttpUrl &port(HttpPort newPort);

    /** E.g. /path/to/resource */
    HttpUrl &path(HttpPath const &newPath);

    /** E.g. foo=bar&baz=sand */
    HttpUrl &query(HttpQuery const &newQuery);

    /** E.g. foo */
    HttpUrl &fragment(HttpFragment const &newFragment);

  protected:
    HttpScheme m_scheme = HS_Http;
    HttpDomain m_domain;
    HttpPort   m_port = 80;
    HttpPath   m_path;
    HttpQuery  m_query;
    HttpFragment m_fragment;
  };


}