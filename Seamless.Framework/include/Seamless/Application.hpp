
#pragma once

#include <Seamless/Export.hpp>

#include <Seamless/Application/ViewManager.hpp>
#include <Seamless/Application/ConfigurationManager.hpp>
#include <Seamless/Error.hpp>
#include <Seamless/String.hpp>

#include <set>
#include <string>

namespace cmls
{
  class Controller;

  struct Route
  {
    cmls::Pattern pattern;
    cmls::HttpMethod method = HM_Get;
    cmls::Controller *controller = nullptr;
  };


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

    void route(cmls::HttpMethod method, std::string const &patternText, cmls::Controller *controller);

    cmls::Controller *controller(std::string const &classId);

    void controller(std::string const &classId, cmls::Controller *controllerInstance);
    
    void staticRoute(std::vector<std::string> const &exts, std::string const &mimeType);

  protected:

    bool loadConfiguration();

    std::ofstream m_logHandle;
    std::string m_log;

    std::string m_root;
    std::string m_site;
    cmls::ViewManager *m_viewManager = nullptr;
    cmls::ConfigurationManager *m_configurationManager = nullptr;

    std::vector<cmls::Route> m_routes;
    std::map<std::string, cmls::Controller*> m_controllers;

    /** Maps file extensions (html, jpeg) to mime-types */
    std::map<std::string, std::string> m_staticServes;

  };
}