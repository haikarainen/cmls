
#include "Seamless/View/ViewModule.hpp"
#include "Seamless/Error.hpp"

#define WIN32_MEAN_AND_LEAN
#include <Windows.h>

cmls::ViewModule::ViewModule(std::string const& path)
{
  m_path = path;
}

cmls::ViewModule::~ViewModule()
{
  unload();
}

bool cmls::ViewModule::load()
{
  if (loaded())
  {
    return true;
  }

  m_moduleHandle = LoadLibraryA(m_path.c_str());
  if (!m_moduleHandle)
  {
    LogError("LoadLibrary failed");
    return false;
  }

  m_entryPoint = (cmls::ViewModuleEntryPoint)GetProcAddress((HMODULE)m_moduleHandle, "cmls_main");
  if (!m_entryPoint)
  {
    LogError("GetProcAddress(cmls_main) failed");
    FreeLibrary((HMODULE)m_moduleHandle);
    return false;
  }

  return true;
}

void cmls::ViewModule::unload()
{
  if (!loaded())
  {
    return;
  }

  std::scoped_lock lock(m_moduleMutex);

  if (m_moduleHandle)
  {
    FreeLibrary((HMODULE)m_moduleHandle);
  }

  m_moduleHandle = nullptr;
  m_entryPoint = nullptr;
}

bool cmls::ViewModule::loaded() const
{
  std::scoped_lock lock(m_moduleMutex);
  return m_moduleHandle && m_entryPoint;
}

void cmls::ViewModule::invoke(cmls::Request const& request, cmls::Response& response, cmls::ViewParameterList const& parameterList)
{
  std::scoped_lock lock(m_moduleMutex);

  if (!loaded())
  {
    LogWarning("Attempted to invoke unloaded viewmodule. Ignoring..");
    return;
  }

  m_entryPoint(request, response, parameterList);
}

std::string cmls::ViewModule::errors()
{
  return m_errors;
}

void cmls::ViewModule::errors(std::string const& newErrors)
{
  m_errors = newErrors;
}
