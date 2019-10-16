
#include "Seamless/Application/ConfigurationManager.hpp"

cmls::ConfigurationManager::ConfigurationManager(cmls::Application* app)
{
  m_application = app;
}

cmls::ConfigurationManager::~ConfigurationManager()
{
}

bool cmls::ConfigurationManager::resolve()
{
  return false;
}
