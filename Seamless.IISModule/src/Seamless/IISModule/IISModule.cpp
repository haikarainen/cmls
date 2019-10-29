
#include "Seamless/IISModule/IISModule.hpp"

#include <Seamless/Application.hpp>

cmls::Application *cmls::IISModule::getApplication()
{
  return application;
}

void cmls::IISModule::deleteApplication()
{
  if (application)
  {
    delete application;
    application = nullptr;
  }
}

void cmls::IISModule::createApplication(std::string const &root, std::string const &site)
{
  if (!application)
  {
    application = new cmls::Application(root, site);
    application->initialize();
  }
}
