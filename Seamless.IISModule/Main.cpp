
#include <Seamless/Application.hpp>

#define _WINSOCKAPI_
#include <httpserv.h>
#include <comdef.h>

class SeamlessModule : public CGlobalModule
{
public:

  void Terminate()
  {
    if(m_application)
    {
      delete m_application;
    }
    m_application = nullptr;

    // Remove the class from memory.
    delete this;
  }

  void assertApplication()
  {
    if (!m_application)
    {
      m_application = new cmls::Application("C:\\Seamless\\");
    }
  }

  virtual GLOBAL_NOTIFICATION_STATUS OnGlobalApplicationStart(IHttpApplicationStartProvider *pProvider) override
  {
    assertApplication();
    return GL_NOTIFICATION_CONTINUE;
  }

  virtual GLOBAL_NOTIFICATION_STATUS OnGlobalApplicationStop(IHttpApplicationStopProvider *pProvider) override
  {

    return GL_NOTIFICATION_CONTINUE;
  }
  
  virtual GLOBAL_NOTIFICATION_STATUS OnGlobalPreBeginRequest(IPreBeginRequestProvider *pProvider) override
  {
    assertApplication();

    HRESULT hr = S_OK;
    IHttpContext* pHttpContext = pProvider->GetHttpContext();
    IHttpResponse * pHttpResponse = pHttpContext->GetResponse();

    if (!pHttpResponse)
    {
      return GL_NOTIFICATION_HANDLED;
    }

    if (!m_application)
    {
      pHttpResponse->SetStatus(404, "No application",0,hr);
      return GL_NOTIFICATION_HANDLED;
    }

    cmls::Request request;
    cmls::Response response;

    if (!m_application->handleRequest(request, response))
    {
      pHttpResponse->SetStatus(404, "Request not routed",0,hr);
      return GL_NOTIFICATION_HANDLED;
    }

    HTTP_DATA_CHUNK chunk;
    chunk.DataChunkType = HttpDataChunkFromMemory;
    auto buffer = response.data().c_str();
    chunk.FromMemory.BufferLength = (USHORT)strlen(buffer);
    chunk.FromMemory.pBuffer = (PVOID)buffer;

    DWORD sentData = 0;
    BOOL completionExpected = FALSE;

    // Clear the existing response.
    pHttpResponse->Clear();

    // Set the MIME type to plain text.
    auto mimeType = "text/html";
    pHttpResponse->SetHeader(HttpHeaderContentType, mimeType, (USHORT)strlen(mimeType), TRUE);

    hr = pHttpResponse->WriteEntityChunks(&chunk, 1, FALSE, TRUE, &sentData);
    if (FAILED(hr))
    {
      pHttpResponse->SetStatus(500, "Write entity chunks failed",0,hr);
    }

    return GL_NOTIFICATION_HANDLED;
  }

protected:
  cmls::Application *m_application = nullptr;
};

extern "C" __declspec(dllexport) HRESULT __stdcall RegisterModule(DWORD dwServerVersion, IHttpModuleRegistrationInfo *pModuleInfo, IHttpServer *pGlobalInfo)
{
  UNREFERENCED_PARAMETER(dwServerVersion);
  HRESULT result = S_OK;

  result = pModuleInfo->SetGlobalNotifications(new SeamlessModule, GL_APPLICATION_START | GL_APPLICATION_STOP | GL_PRE_BEGIN_REQUEST);
  if (FAILED(result))
  {
    return result;
  }

  return S_OK;
}