
#include "Seamless/IISModule/IISModuleHttp.hpp"

#include "Seamless/IISModule/IISModule.hpp"
#include "Seamless/Application.hpp"


#include <Seamless/Error.hpp>
#include <Seamless/Request.hpp>
#include <Seamless/Response.hpp>

REQUEST_NOTIFICATION_STATUS cmls::IISModuleHttp::OnBeginRequest(IHttpContext* pHttpContext, IHttpEventProvider* pProvider)
{
  IHttpRequest *iisRequest = pHttpContext->GetRequest();
  IHttpResponse *iisResponse = pHttpContext->GetResponse();
  
  if (!iisResponse)
  {
    LogError("No IIS response");
    return RQ_NOTIFICATION_FINISH_REQUEST;
  }

  auto application = cmls::IISModule::getApplication();
  if (!application)
  {
    LogError("No application");
    iisResponse->SetStatus(500, "No application");
    return RQ_NOTIFICATION_FINISH_REQUEST;
  }

  // Setup the cmls request
  cmls::Request cmlsRequest;
  cmlsRequest.headers(readHttpHeaders(iisRequest));
  cmlsRequest.method(readHttpMethod(iisRequest));
  cmlsRequest.path(readHttpPath(iisRequest));
  cmlsRequest.rawUrl(readHttpUrl(iisRequest));
  cmlsRequest.entity(readEntityBytes(iisRequest));
  

  // Create a cmls response and let the application generate it
  cmls::Response cmlsResponse;
  if (!application->handleRequest(cmlsRequest, cmlsResponse))
  {
    iisResponse->SetStatus(404, "Request not routed");
    return RQ_NOTIFICATION_FINISH_REQUEST;
  }

  // Prepare IIS response
  iisResponse->Clear();

  if(!writeHttpHeaders(cmlsResponse, iisResponse))
    LogWarning("writeHttpHeaders failed");

  if(!writeEntityBytes(cmlsResponse, iisResponse))
    LogWarning("writeEntityBytes failed");

  if(!writeHttpStatus(cmlsResponse, iisResponse))
    LogWarning("writeHttpStatus failed");

  return RQ_NOTIFICATION_FINISH_REQUEST;
}

cmls::Stream cmls::IISModuleHttp::readEntityBytes(IHttpRequest* request)
{
  cmls::Stream outputBuffer;
  char *readBuffer = new char[1024];
  DWORD readBufferBytes = 0;

  while (request->GetRemainingEntityBytes() > 0)
  {
    HRESULT result = request->ReadEntityBody(readBuffer, 1024, FALSE, &readBufferBytes);
    if (result != S_OK && (result & 0x0000FFFF) != ERROR_HANDLE_EOF)
    {
      LogError("ReadEntityBody failed");
      return outputBuffer;
    }

    outputBuffer.write(reinterpret_cast<uint8_t*>(readBuffer), readBufferBytes);
  }

  return outputBuffer;
}

cmls::HttpMethod cmls::IISModuleHttp::readHttpMethod(IHttpRequest* request)
{
  return cmls::httpMethod(request->GetHttpMethod());
}

cmls::HttpPath cmls::IISModuleHttp::readHttpPath(IHttpRequest* request)
{
  auto rawRequest = request->GetRawHttpRequest();
  if (!rawRequest)
  {
    LogError("No raw request available");
    return {};
  }

  // Read the abs. path into a wstring and convert it to utf-8
  std::wstring rawAbsString(rawRequest->CookedUrl.pAbsPath, rawRequest->CookedUrl.pAbsPath + (rawRequest->CookedUrl.AbsPathLength / sizeof(wchar_t)));
  auto pp = cmls::utf16to8(rawAbsString);

  // Remove any query
  auto pf = pp.find('?');
  if (pf != std::string::npos)
  {
    pp = cmls::substring(pp, 0, pf);
  }

  return pp;
}

cmls::HttpUrl cmls::IISModuleHttp::readHttpUrl(IHttpRequest* request)
{
  auto rawRequest = request->GetRawHttpRequest();
  if (!rawRequest)
  {
    LogError("No raw request available");
    return {};
  }

  // Read the full url into wstring and convert it to utf-8
  std::wstring r(rawRequest->CookedUrl.pFullUrl, rawRequest->CookedUrl.pFullUrl + (rawRequest->CookedUrl.FullUrlLength / sizeof(wchar_t)));
  auto pp = cmls::utf16to8(r);

  return pp;
}

cmls::HttpHeaderMap cmls::IISModuleHttp::readHttpHeaders(IHttpRequest* request)
{
  static const std::map<uint16_t, std::string> headerNames = {
    {HttpHeaderCacheControl      , "Cache-Control"},
    {HttpHeaderConnection        , "Connection"},
    {HttpHeaderDate              , "Date"},
    {HttpHeaderKeepAlive         , "Keep-Alive"},
    {HttpHeaderPragma            , "Pragma"},
    {HttpHeaderTrailer           , "Trailer"},
    {HttpHeaderTransferEncoding  , "Transfer-Encoding"},
    {HttpHeaderUpgrade           , "Upgrade"},
    {HttpHeaderVia               , "Via"},
    {HttpHeaderWarning           , "Warning"},
    {HttpHeaderAllow             , "Allow"},
    {HttpHeaderContentLength     , "Content-Length"},
    {HttpHeaderContentType       , "Content-Type"},
    {HttpHeaderContentEncoding   , "Content-Encoding"},
    {HttpHeaderContentLanguage   , "Content-Language"},
    {HttpHeaderContentLocation   , "Content-Location"},
    {HttpHeaderContentMd5        , "Content-MD5"},
    {HttpHeaderContentRange      , "Content-Range"},
    {HttpHeaderExpires           , "Expires"},
    {HttpHeaderLastModified      , "Last-Modified"},
    {HttpHeaderAccept            , "Accept"},
    {HttpHeaderAcceptCharset     , "Accept-Charset"},
    {HttpHeaderAcceptEncoding    , "Accept-Encoding"},
    {HttpHeaderAcceptLanguage    , "Accept-Language"},
    {HttpHeaderAuthorization     , "Authorization"},
    {HttpHeaderCookie            , "Cookie"},
    {HttpHeaderExpect            , "Expect"},
    {HttpHeaderFrom              , "From"},
    {HttpHeaderHost              , "Host"},
    {HttpHeaderIfMatch           , "If-Match"},
    {HttpHeaderIfModifiedSince   , "If-Modified-Since"},
    {HttpHeaderIfNoneMatch       , "If-None-Match"},
    {HttpHeaderIfRange           , "If-Range"},
    {HttpHeaderIfUnmodifiedSince , "If-Unmodified-Since"},
    {HttpHeaderMaxForwards       , "Max-Forwards"},
    {HttpHeaderProxyAuthorization, "Proxy-Authorization"},
    {HttpHeaderReferer           , "Referer"},
    {HttpHeaderRange             , "Range"},
    {HttpHeaderTe                , "TE"},
    {HttpHeaderTranslate         , "Translate"},
    {HttpHeaderUserAgent         , "User-Agent"}
  };
  
  auto rawRequest = request->GetRawHttpRequest();
  if (!rawRequest)
  {
    LogError("No raw request available");
    return {};
  }

  cmls::HttpHeaderMap returner;

  for (uint16_t i = 0; i < HttpHeaderRequestMaximum; i++)
  {
    HTTP_KNOWN_HEADER *knownHeader = &rawRequest->Headers.KnownHeaders[i];

    auto finder = headerNames.find(i);
    if (finder != headerNames.end())
    {
      std::string h(knownHeader->pRawValue, knownHeader->pRawValue + knownHeader->RawValueLength);

      returner[finder->second] = h;
    }
    else
    {
      LogError("Known header %i does not have a name", i);
    }
  }

  for (uint16_t i = 0; i < rawRequest->Headers.UnknownHeaderCount; i++)
  {
    std::string hn(rawRequest->Headers.pUnknownHeaders[i].pName, rawRequest->Headers.pUnknownHeaders[i].NameLength);
    std::string hv(rawRequest->Headers.pUnknownHeaders[i].pRawValue, rawRequest->Headers.pUnknownHeaders[i].pRawValue + rawRequest->Headers.pUnknownHeaders[i].RawValueLength);
    returner[hn] = hv;
  }

  return returner;

}

bool cmls::IISModuleHttp::writeEntityBytes(cmls::Response const& cmlsResponse, IHttpResponse* iisResponse)
{
  // Setup the IIS response
  uint64_t responseSize = cmlsResponse.data().size();
  uint64_t chunkSize = 65535Ui64;
  uint64_t chunkRemainder = responseSize % chunkSize;

  uint64_t numChunks = responseSize / chunkSize + (chunkRemainder != 0);
  if (numChunks > 65535)
  {
    LogWarning("Chunk overflow, cutting from %i", numChunks);
    numChunks = 65535;
  }

  std::vector<HTTP_DATA_CHUNK> chunks;
  chunks.reserve(numChunks);

  for (uint16_t i = 0; i < numChunks; i++)
  {
    HTTP_DATA_CHUNK chunk;

    uint16_t currChunkSize = i == numChunks - 1 ? chunkRemainder : chunkSize;

    chunk.DataChunkType = HttpDataChunkFromMemory;
    chunk.FromMemory.BufferLength = currChunkSize;
    chunk.FromMemory.pBuffer = const_cast<uint8_t*>(cmlsResponse.data().data() + (i * chunkSize));

    chunks.push_back(chunk);
  }

  DWORD sentData = 0;
  BOOL completionExpected = FALSE;

  auto result = iisResponse->WriteEntityChunks(chunks.data(), chunks.size(), FALSE, FALSE, &sentData);
  if (FAILED(result))
  {
    iisResponse->SetStatus(500, "Write entity chunks failed", 0, result);
    return false;
  }

  return true;
}

bool cmls::IISModuleHttp::writeHttpHeaders(cmls::Response const& cmlsResponse, IHttpResponse* iisResponse)
{
  for(auto &h : cmlsResponse.headers())
  {
    iisResponse->SetHeader(h.first.c_str(), h.second.c_str(), (USHORT)h.second.size(), TRUE);
  }

  return true;
}

bool cmls::IISModuleHttp::writeHttpStatus(cmls::Response const& cmlsResponse, IHttpResponse* iisResponse)
{
  iisResponse->SetStatus(cmlsResponse.code(), cmlsResponse.reason().c_str());
  return true;
}
