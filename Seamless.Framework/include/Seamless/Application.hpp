
#pragma once

#include <Seamless/Export.hpp>

#include <Seamless/Application/ViewManager.hpp>
#include <Seamless/Error.hpp>
#include <string>

namespace cmls
{

  class CMLS_API Application : public cmls::LogHandler
  {
  public:
    Application(std::string const &root = ".", std::string const &site = "default");
    virtual ~Application();

    bool initialize();

    bool handleRequest(cmls::Request const &request, cmls::Response &response);

    std::string const &root() const;

    std::string const &site() const;
    std::string sitePath() const;

    cmls::ViewManager *viewManager() const;

    virtual void writeLine(std::string const &line) override;
    std::string const &log() const;

  protected:

    bool loadConfiguration();

    std::ofstream m_logHandle;
    std::string m_log;

    std::string m_root;
    std::string m_site;
    cmls::ViewManager *m_viewManager = nullptr;


  };
}