
#pragma once

#include <Seamless/Export.hpp>

#include <Seamless/Request.hpp>
#include <Seamless/Response.hpp>

namespace cmls
{

  class Application;

  class CMLS_API Controller
  {
  public:

    Controller(cmls::Application * const application);

    virtual void handleRequest(cmls::Request const &request, cmls::Response &response)=0;

    cmls::Application *application() const;

  protected:
     cmls::Application * const m_application = nullptr;
  };

}