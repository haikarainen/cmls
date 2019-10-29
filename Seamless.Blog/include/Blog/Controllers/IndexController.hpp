
#pragma once 

#include <Blog/Export.hpp>
#include <Seamless/Application/Controller.hpp>

namespace blog
{
  class BLOG_API IndexController : public cmls::Controller
  {
  public:
    IndexController(cmls::Application *const application);

    // Inherited via Controller
    virtual void handleRequest(cmls::Request const& request, cmls::Response& response) override;
  };
}