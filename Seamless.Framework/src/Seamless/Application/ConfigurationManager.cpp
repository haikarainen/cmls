
#include "Seamless/Application/ConfigurationManager.hpp"
#include "Seamless/Application.hpp"
#include "Seamless/CppCompiler/CppCompiler.hpp"

#include "Seamless/Filesystem.hpp"


#define WIN32_MEAN_AND_LEAN
#include <Windows.h>

cmls::ConfigurationManager::ConfigurationManager(cmls::Application* app)
{
  m_application = app;
  SetDllDirectoryA(m_application->root().c_str());
}

cmls::ConfigurationManager::~ConfigurationManager()
{
  if(m_moduleHandle)
  {
    FreeLibrary((HMODULE)m_moduleHandle);
    m_moduleHandle = nullptr;
  }

  m_entryPoint = nullptr;
}

bool cmls::ConfigurationManager::resolve()
{
  /** ---- @todo refactor, see ViewManager after it's cleaned up **/

  // Unload module if already loaded
  if(m_moduleHandle)
  {
    FreeLibrary((HMODULE)m_moduleHandle);
    m_moduleHandle = nullptr;
  }

  m_entryPoint = nullptr;

  // Verify that site configuration exist
  cmls::File conf(m_application->sitePath() + "\\site.conf");
  if (!conf.exist())
  {
    LogError("Configuration file does not exist for site");
    return false;
  }

  // Read configuration data to string
  std::string data;
  if (!conf.readString(data))
  {
    LogError("Configuration file could not be read");
    return false;
  }

  // Intermediate type representing a serialized route
  struct Route
  {
    std::string patternText;
    HttpMethod method;
    std::string controllerName;
  };

  // Intermediate type representing a serialized static route
  struct Static
  {
    std::vector<std::string> extensions;
    std::string mimeType;
  };

  // Read the configuration into intermediate variables
  std::vector<Static> statics;
  std::vector<Route> routes;
  std::set<std::string> controllers;

  auto allModules = cmls::Module::allModules(m_application->root());
  std::vector<cmls::Module> modules; 
  auto lines = cmls::split(data, {'\n'});
  for (auto l : lines)
  {
    auto args = cmls::parseArguments(l, true);
    if (args.size() > 0)
    {
      auto command = cmls::strToLower(args[0]);
      if (command == "import")
      {
        if (args.size() != 2)
        {
          LogError("Invalid import statement, requires two (and only two) parameters");
          return false;
        }

        auto mf = allModules.find(args[1]);
        if (mf == allModules.end())
        {
          LogError("Import: Module \"%s\" could not be found", args[1].c_str());
          return false;
        }

        modules.push_back(mf->second);
      }
      else if (command == "route")
      {
        if (args.size() != 4)
        {
          LogError("Invalid route statement, requires four (and only four) parameters");
          return false;
        }

        HttpMethod newMethod = HM_None;
        auto methodList = cmls::split(args[1], {','});
        for (auto m : methodList)
        {
          newMethod = HttpMethod(newMethod | cmls::httpMethod(m));
        }

        auto ctrlName = cmls::trim(args[3]);
        auto patternTxt = cmls::trim(args[2]);

        routes.push_back({patternTxt, newMethod, ctrlName});
        controllers.insert(ctrlName);

      }
      else if (command == "static")
      {
        statics.push_back({cmls::split(args[1], {','}), args[2]});
      }
      else
      {
        LogError("Invalid statement in site configuration");
        return false;
      }

    }

  }


  // Generate the configuration C++ source file
  std::stringstream src;
  src << "#include<Seamless/Seamless.hpp>\n";
  for (auto m : modules)
  {
    for (auto i : m.includes())
    {
      src << "#include " << i << "\n";
    }
  }
  
  src << "extern \"C\" __declspec(dllexport) void cmls_apply_configuration(cmls::Application *application)\n";
  src << "{\n";

  for (auto ctrl : controllers)
  {
    src << "  application->controller(\"" << ctrl << "\", new " << ctrl << "(application));\n";
  }

  for (auto route : routes)
  {
    src << "  application->route((cmls::HttpMethod)" << (uint16_t)route.method << ", \"" << route.patternText << "\", application->controller(\"" << route.controllerName << "\"));\n";
  }

  for (auto stati : statics)
  {
    src << "  application->staticRoute({";

    for (auto s : stati.extensions)
    {
      src << "\"" << s << "\", ";
    }

    src << "}, \"" << stati.mimeType << "\");\n";
  }

  src << "}\n";

  // Write the generated source file
  cmls::File cpp(m_application->sitePath() + "\\site.cpp");
  if (!cpp.writeString(src.str()))
  {
    LogError("Failed to write generated configuration source");
    return false;
  }

  // Compile the generated source file
  cmls::CppCompiler compiler;
  cmls::CppProject cppProject;

  cmls::Directory intermediate(m_application->sitePath() + "\\intermediate\\");
  if (!intermediate.create())
  {
    LogError("Failed to create intermediate directory for site configuration build");
    return false;
  }

  cppProject.targetName = "site";
  cppProject.targetType = TT_DynamicLibrary;
  cppProject.targetPath = m_application->sitePath();
  cppProject.intermediatePath = intermediate.fullPath();
  cppProject.modules = modules;
  cppProject.sources = {cpp.fullPath()};

  if (!compiler.compile(cppProject))
  {
    LogError("Failed to build configuration:\n-----\n%s\n-----\n", compiler.output().c_str());
    return false;
  }
  
  // Reload the compiled configuration
  cmls::File dll(m_application->sitePath() + "\\site.dll");
  m_moduleHandle = LoadLibraryA(dll.fullPath().c_str());
  if (!m_moduleHandle)
  {
    LogError("LoadLibraryA failed");
    return false;
  }

  m_entryPoint = (cmls::ConfigurationEntryPoint)GetProcAddress((HMODULE)m_moduleHandle, "cmls_apply_configuration");
  if (!m_entryPoint)
  {
    LogError("GetProcAddress failed");
    return false;
  }

  // Apply the freshly generated, compiled and loaded configuration
  m_entryPoint(m_application);

  return true;
}
