
#pragma once 

#define _WINSOCKAPI_
#include <httpserv.h>

namespace cmls
{

  class Application;

  class IISModuleGlobal : public CGlobalModule
  {
  public:

    void Terminate();

    virtual GLOBAL_NOTIFICATION_STATUS OnGlobalApplicationStart(IHttpApplicationStartProvider* pProvider) override;

    virtual GLOBAL_NOTIFICATION_STATUS OnGlobalApplicationStop(IHttpApplicationStopProvider* pProvider) override;

  protected:
    cmls::Application *m_application = nullptr;
  };

}