#pragma once

#include <Seamless/Export.hpp>
#include <Seamless/String.hpp>

#include <string>
#include <fstream>

#define LogError(x, ...) cmls::Log::writeLine( cmls::formatString( "%50sError: %s", cmls::substring(cmls::formatString("%s:%u: ",__FUNCTION__, __LINE__ ), -50).c_str(), cmls::formatString(x, __VA_ARGS__).c_str()))
#define LogWarning(x, ...) cmls::Log::writeLine( cmls::formatString("%50sWarning: %s", cmls::substring(cmls::formatString("%s:%u: ",__FUNCTION__, __LINE__ ), -50).c_str(), cmls::formatString(x, __VA_ARGS__).c_str()))
#define LogNotice(x, ...) cmls::Log::writeLine( cmls::formatString("%50s%s", cmls::substring(cmls::formatString("%s:%u: ",__FUNCTION__, __LINE__ ), -50).c_str(), cmls::formatString(x, __VA_ARGS__).c_str()))
#define Log(x, ...) cmls::Log::writeLine( cmls::formatString( "%s", cmls::formatString(x, __VA_ARGS__).c_str()))


namespace cmls
{
  class CMLS_API LogHandler
  {
  public:
    virtual void writeLine(std::string const &line)=0;
  };

  class CMLS_API Log
  {
  public:
    static void registerHandler(LogHandler *handler);
    static void unregisterHandler(LogHandler *handler);

    static void writeLine(std::string const & line);
    static void close();
  };
}