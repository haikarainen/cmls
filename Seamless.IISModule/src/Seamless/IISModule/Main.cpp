
#include "Seamless/IISModule/IISModuleGlobal.hpp"
#include "Seamless/IISModule/IISModuleFactoryHttp.hpp"

#define _WINSOCKAPI_
#include <httpserv.h>
#include <comdef.h>

extern "C" __declspec(dllexport) HRESULT __stdcall RegisterModule(DWORD dwServerVersion, IHttpModuleRegistrationInfo *pModuleInfo, IHttpServer *pGlobalInfo)
{
  UNREFERENCED_PARAMETER(dwServerVersion);
  HRESULT result = S_OK;

  result = pModuleInfo->SetGlobalNotifications(new cmls::IISModuleGlobal, GL_APPLICATION_START | GL_APPLICATION_STOP);
  if (FAILED(result))
  {
    return result;
  }


  result = pModuleInfo->SetRequestNotifications(new cmls::IISModuleFactoryHttp, RQ_BEGIN_REQUEST, 0);
  if (FAILED(result))
  {
    return result;
  }

  return S_OK;
}