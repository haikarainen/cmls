
#pragma once

#include <Seamless/Export.hpp>

#include <Seamless/View/ViewParameter.hpp>
#include <Seamless/Response.hpp>
#include <Seamless/Request.hpp>

#include <string>
#include <mutex>

namespace cmls
{
  using ViewModuleEntryPoint = void(*)(cmls::Request const &, cmls::Response &, cmls::ViewParameterList const &);

  class CMLS_API ViewModule
  {
  public:
    ViewModule(std::string const & path);
    virtual ~ViewModule();

    bool load();
    void unload();
    bool loaded() const;

    void invoke(cmls::Request const &request, cmls::Response &response, cmls::ViewParameterList const &parameterList);

    std::string errors();
    void errors(std::string const &newErrors);

  protected:
    mutable std::recursive_mutex m_moduleMutex;
    std::string m_path;
    std::string m_errors;
    void *m_moduleHandle = nullptr;
    ViewModuleEntryPoint m_entryPoint = nullptr;
  };
}