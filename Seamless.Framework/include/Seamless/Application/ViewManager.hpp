
#pragma once

#include <Seamless/Export.hpp>
#include <Seamless/View/ViewModule.hpp>
#include <Seamless/View/ViewParameter.hpp>
#include <Seamless/Response.hpp>
#include <Seamless/Request.hpp>

#include <string>
#include <map>

namespace cmls
{

  class Application;

  class CMLS_API ViewManager
  {
  public:
    ViewManager(cmls::Application *application);
    virtual ~ViewManager();

    /** Invokes the view at the given root-relative .cmls path, returns true on success, false otherwise. */
    bool invokeView(std::string const& cmlsPath, cmls::Request const &request, cmls::Response &response, cmls::ViewParameters &&parameters);

    /** Retrieve the full path to the cmls file given a root-relative .cmls path */
    std::string fullViewPath(std::string const &cmlsPath);

    /** Retrieve the full path to the intermediate .cpp file given a root-relative .cmls path */
    std::string fullIntermediatePath(std::string const &cmlsPath);

    /** Retrieve the full path to the module for the given root-relative .cmls path */
    std::string fullModulePath(std::string const &cmlsPath);

    /** Returns true if the module for the given root-relative .cmls path is outdated. */
    bool isViewOutdated(std::string const &cmlsPath);

    /** Attempt to build the view at the given root-relative .cmls path, returns true on success */
    bool buildView(std::string const& cmlsPath);

    bool loadView(std::string const &cmlsPath);

    cmls::ViewModule *viewModule(std::string const &cmlsPath);

    cmls::ViewModule *resolveView(std::string const &cmlsPath);

    cmls::Application *application();

  protected:

    void errorResponse(std::string const &error, cmls::Response &response);

    cmls::Application *m_application = nullptr;
    std::map<std::string, cmls::ViewModule*> m_viewModules;
  };
}