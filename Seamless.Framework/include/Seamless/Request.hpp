
#pragma once

#include <Seamless/Export.hpp>
#include <Seamless/Http.hpp>
#include <Seamless/Stream.hpp>


#include <map>
#include <string>

namespace cmls
{
  
  class CMLS_API Request : public cmls::HttpHeaderOwner
  {
  public:
    Request();
    virtual ~Request();

    cmls::HttpMethod method() const;

    cmls::HttpPath const &path() const;

    cmls::HttpQuery const &query() const;

    cmls::HttpUrl const &rawUrl() const;

    Request &path(cmls::HttpPath const &newPath);

    Request &query(cmls::HttpQuery const &newQuery);



    Request &method(cmls::HttpMethod method);

    Request &rawUrl(cmls::HttpUrl const &newUrl);

    cmls::Stream const &entity() const;

    void entity(cmls::Stream const &newStream);

  protected:
    cmls::HttpUrl m_rawUrl;

    cmls::HttpPath m_path;
    cmls::HttpQuery m_query;
    cmls::HttpMethod m_method = HM_Get;
    cmls::Stream m_entity;

    // @todo; localAddress, remoteAddress
  };
}