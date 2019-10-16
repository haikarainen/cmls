
#include "Seamless/Application/ViewManager.hpp"

#include "Seamless/Application.hpp"

#include "Seamless/TemplateCompiler/TemplateCompiler.hpp"
#include "Seamless/CppCompiler/CppCompiler.hpp"

#include "Seamless/Filesystem.hpp"
#include "Seamless/Error.hpp"



cmls::ViewManager::ViewManager(cmls::Application *app)
{
  m_application = app;
}

cmls::ViewManager::~ViewManager()
{
}

bool cmls::ViewManager::invokeView(std::string const& cmlsPath, cmls::Request const &request, cmls::Response &response, cmls::ViewParameters && parameters)
{
  auto view = resolveView(cmlsPath);
  if (!view)
  {
    errorResponse(cmls::formatString("Failed to invoke view, view could not be resolved (%s)", cmlsPath.c_str()), response);

    // @todo, clean this up!
    auto mod = viewModule(cmlsPath);
    if (mod)
    {
      errorResponse("<pre><code>" + mod->errors() + "</code></pre>", response);
    }


    return false;
  }

  if (!view->loaded())
  {
    errorResponse(cmls::formatString("Failed to invoke view, view wasn't loaded (%s)", cmlsPath.c_str()), response);
    return false;
  }

  cmls::ViewParameterList parameterList(parameters);
  view->invoke(request, response, parameterList);

  return true;
}

std::string cmls::ViewManager::fullViewPath(std::string const& cmlsPath)
{
   return cmls::File(application()->root() + "\\source\\" + cmlsPath).fullPath();
}

std::string cmls::ViewManager::fullIntermediatePath(std::string const& cmlsPath)
{
  return cmls::File(application()->root() + "\\intermediate\\" + cmlsPath).extension(".cpp").fullPath();
}

std::string cmls::ViewManager::fullModulePath(std::string const& cmlsPath)
{
  return cmls::File(application()->root() + "\\views\\" + cmlsPath).extension(".dll").fullPath();
}

bool cmls::ViewManager::isViewOutdated(std::string const& cmlsPath)
{
  auto viewFile = cmls::File(fullViewPath(cmlsPath));
  auto moduleFile = cmls::File(fullModulePath(cmlsPath));

  if (!viewFile.exist())
  {
    LogWarning("View source doesn't exist (%s)", viewFile.fullPath().c_str());
    return false;
  }

  if (!moduleFile.exist())
  {
    return true;
  }

  return viewFile.lastWriteTime() > moduleFile.lastWriteTime();
}

bool cmls::ViewManager::buildView(std::string const& cmlsPath)
{
  auto existing = viewModule(cmlsPath);
  existing->unload();

  auto source = cmls::File(fullViewPath(cmlsPath));
  if (!source.exist())
  {
    LogError("Source file does not exist");
    return false;
  }

  auto intermediate = cmls::File(fullIntermediatePath(cmlsPath));
  auto output = cmls::File(fullModulePath(cmlsPath));
  auto outputDir = cmls::Directory(output.path());
  if (!outputDir.exist())
  {
    if (!outputDir.create())
    {
      LogError("Failed to create output directory");
      return false;
    }
  }

  cmls::TemplateCompiler cmlsCompiler(application()->root());

  if (!cmlsCompiler.compile(source.fullPath(), intermediate.fullPath()))
  {
    LogError("Failed to compile intermediate source");
    return false;
  }
  cmls::CppCompiler cppCompiler;
  cmls::CppProject cppProject;
  
  cppProject.targetName = source.basename();
  cppProject.targetType = TT_DynamicLibrary;
  cppProject.targetPath = outputDir.fullPath();
  cppProject.intermediatePath = intermediate.path();
  cppProject.modules = cmlsCompiler.importedModules();
  cppProject.sources = {intermediate.fullPath()};

  if (!cppCompiler.compile(cppProject))
  {
    LogError("Failed to build view module");
    existing->errors(cppCompiler.output());
    return false;
  }

  return true;
}

bool cmls::ViewManager::loadView(std::string const& cmlsPath)
{
  auto module = viewModule(cmlsPath);
  
  if (!module->load())
  {
    LogError("Failed to load view module");
    return false;
  }

  return true;
}

cmls::ViewModule *cmls::ViewManager::viewModule(std::string const& cmlsPath)
{
  auto v = m_viewModules.find(fullModulePath(cmlsPath));
  if (v != m_viewModules.end())
  {
    return v->second;
  }

  auto modulePath = fullModulePath(cmlsPath);
  auto module = new ViewModule(modulePath);
  m_viewModules[modulePath] = module;

  return module;
}

cmls::ViewModule *cmls::ViewManager::resolveView(std::string const& cmlsPath)
{
  if (isViewOutdated(cmlsPath))
  {
    if (!buildView(cmlsPath))
    {
      LogError("Failed to resolve view, outdated and build failed.");
      return nullptr;
    }
  }

  if (!loadView(cmlsPath))
  {
    LogError("Failed to resolve view, load failed.");
    return nullptr;
  }

  return viewModule(cmlsPath);
}

cmls::Application* cmls::ViewManager::application()
{
  return m_application;
}

void cmls::ViewManager::errorResponse(std::string const& error, cmls::Response &response)
{
  LogError("%s", error.c_str());
  response << "<html><head><title>Seamless Error</title></head><body><h2>Error</h2><p>"  << error << "</p></body></html>";

}
