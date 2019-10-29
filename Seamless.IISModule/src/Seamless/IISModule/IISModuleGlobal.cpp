
#include "Seamless/IISModule/IISModuleGlobal.hpp"

#include "Seamless/IISModule/IISModule.hpp"

void cmls::IISModuleGlobal::Terminate()
{
  cmls::IISModule::deleteApplication();
  delete this;
}

GLOBAL_NOTIFICATION_STATUS cmls::IISModuleGlobal::OnGlobalApplicationStart(IHttpApplicationStartProvider* pProvider)
{
  // @todo IIS configuration instead of hardcoded variables
  cmls::IISModule::createApplication("C:\\Seamless\\", "default");

  return GL_NOTIFICATION_CONTINUE;
}

GLOBAL_NOTIFICATION_STATUS cmls::IISModuleGlobal::OnGlobalApplicationStop(IHttpApplicationStopProvider* pProvider)
{
  cmls::IISModule::deleteApplication();

  return GL_NOTIFICATION_CONTINUE;
}
