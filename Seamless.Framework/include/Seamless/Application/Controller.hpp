
#pragma once

#include <Seamless/Export.hpp>

#include <Seamless/Request.hpp>
#include <Seamless/Response.hpp>

namespace cmls
{

  class CMLS_API Controller
  {
  public:
    virtual void handleRequest(cmls::Request const &request, cmls::Response &response)=0;
  };

}