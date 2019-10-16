
#pragma once

#include <Seamless/Export.hpp>
#include <Seamless/CompilerCommon/Module.hpp>

#include <vector>
#include <string>

namespace cmls 
{


  enum CppTargetType
  {
    TT_Executable,
    TT_DynamicLibrary
  };

  struct CMLS_API CppProject
  {
    /** List of source files, must be absolute or relative to intermediatePath */
    std::vector<std::string> sources;

    /** The path where intermediate files are placed (e.g. project files), must be absolute or relative to working directory */
    std::string intermediatePath;

    /** The target name, output filename without extension (.dll, .so, .exe) */
    std::string targetName = "undefined";

    /** The target path, where the output file will be placed. Must be absolute or relative to working directory. */
    std::string targetPath = "";

    /** The type of output file. .dll, exe, etc. */
    CppTargetType targetType = TT_DynamicLibrary;

    /** List of modules to include in this project */
    std::vector<cmls::Module> modules;
  };
  
  class CMLS_API ICppCompiler
  {
  public:
    /** Compiles the given project */
    virtual bool compile(CppProject const &project) = 0;

    std::string const &output() const;

    void writeOutput(std::string const &outputLine);

  protected:
    /** @todo replace this with proper build messages */
    std::string m_output;

  };
}