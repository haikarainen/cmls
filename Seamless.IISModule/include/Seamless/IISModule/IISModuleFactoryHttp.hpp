
#pragma once 

#define _WINSOCKAPI_
#include <httpserv.h>

namespace cmls 
{
  class IISModuleFactoryHttp : public IHttpModuleFactory
  {
    virtual HRESULT GetHttpModule(CHttpModule **ppModule, IModuleAllocator *pAllocator) override;

    virtual VOID Terminate(VOID) override;
  };

}