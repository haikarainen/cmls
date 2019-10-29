
#pragma once 

#include <string>

namespace cmls
{

  class Application;

  // Helper for tracking CMLS Application inside IIS Module
  namespace IISModule
  {
    namespace 
    {
      cmls::Application *application = nullptr;
    }

    cmls::Application *getApplication();

    void deleteApplication();

    void createApplication(std::string const &root, std::string const &site);
  }

}