
#pragma once

#include <Seamless/Export.hpp>

namespace cmls
{
  class Application;

  class CMLS_API ConfigurationManager
  {
  public:
    ConfigurationManager(cmls::Application* app);
    virtual  ~ConfigurationManager();

    bool resolve();

  protected:

    cmls::Application* m_application = nullptr;
  };
}