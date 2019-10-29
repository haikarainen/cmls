
#include "Blog/Controllers/IndexController.hpp"
#include "Blog/Models/Entry.hpp"

#include <Seamless/Application.hpp>

blog::IndexController::IndexController(cmls::Application* const application)
  : cmls::Controller(application)
{
}

void blog::IndexController::handleRequest(cmls::Request const& request, cmls::Response& response)
{
  auto vm = application()->viewManager();

  blog::Entry entry {
    "This is a title",
    "This is example content"
  };

  vm->invokeView("sub/index.cmls", request, response, {
    {"entry", entry},
  });
}
