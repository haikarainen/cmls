
#pragma once 

#include <Seamless/Stream.hpp>
#include <Seamless/Http.hpp>
#include <Seamless/Response.hpp>

#define _WINSOCKAPI_
#include <httpserv.h>

namespace cmls
{

  class IISModuleHttp : public CHttpModule
  {
  public:
    virtual REQUEST_NOTIFICATION_STATUS OnBeginRequest(IHttpContext* pHttpContext, IHttpEventProvider* pProvider) override;

  protected:


    cmls::Stream readEntityBytes(IHttpRequest *request);

    cmls::HttpMethod readHttpMethod(IHttpRequest *request);

    cmls::HttpPath readHttpPath(IHttpRequest *request);

    cmls::HttpUrl readHttpUrl(IHttpRequest *request);

    cmls::HttpHeaderMap readHttpHeaders(IHttpRequest *request);

    bool writeEntityBytes(cmls::Response const &cmlsResponse, IHttpResponse *iisResponse);

    bool writeHttpHeaders(cmls::Response const &cmlsResponse, IHttpResponse *iisResponse);

    bool writeHttpStatus(cmls::Response const &cmlsResponse, IHttpResponse *iisResponse);

  };

}