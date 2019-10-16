
#pragma once

#include <Seamless/Export.hpp>

#include <vector>
#include <map>
#include <string>

namespace cmls
{
  /** Specifies a CMLS module (external dependency library) */
  class CMLS_API Module
  {
  public:

    static std::map<std::string, cmls::Module> allModules(std::string const &rootPath);


    Module();
    Module(std::string const& path);
    virtual ~Module();

    /** CMLS implicit includes. Only used by the template compiler. */
    std::vector<std::string> includes() const;

    /** C++ defines. Only used by the C++ compiler. */
    std::vector<std::string> defines() const;

    /** C++ link libraries. Only used by the C++ compiler. */
    std::vector<std::string> links() const;

    /** C++ include paths. Only used by the C++ compiler. */
    std::vector<std::string> includePaths() const;

    /** C++ linker paths. Only used by the C++ compiler. */
    std::vector<std::string> linkerPaths() const;

  protected:

    void parseConfiguration();

    std::string m_path;

    std::string implicitIncludePath() const;
    std::string implicitLinkerPath() const;

    std::vector<std::string> m_links;
    std::vector<std::string> m_defines;
    std::vector<std::string> m_additionalIncludePaths;
    std::vector<std::string> m_additionalLinkerPaths;
    std::vector<std::string> m_implicitIncludes;

  };
}