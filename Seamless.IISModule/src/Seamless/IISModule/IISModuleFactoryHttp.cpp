
#include "Seamless/IISModule/IISModuleFactoryHttp.hpp"

#include "Seamless/IISModule/IISModuleHttp.hpp"

HRESULT cmls::IISModuleFactoryHttp::GetHttpModule(CHttpModule **ppModule, IModuleAllocator *pAllocator)
{
  *ppModule = new cmls::IISModuleHttp();
  return S_OK;
}

VOID cmls::IISModuleFactoryHttp::Terminate(VOID)
{
  delete this;
}
