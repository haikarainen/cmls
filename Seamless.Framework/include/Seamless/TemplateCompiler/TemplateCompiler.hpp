
#pragma once

#include <Seamless/Export.hpp>
#include <Seamless/CompilerCommon/Module.hpp>

#include <string>
#include <vector>
#include <set>
#include <map>

namespace cmls
{

  class CompilationChunk;
  
  struct CMLS_API TemplateCompilerSettings
  {
    /** Will optimize output C++ for speed and memory */
    bool optimize = true;
  };


  class CMLS_API TemplateCompiler
  {
  public:
    TemplateCompiler(std::string const &root, TemplateCompilerSettings settings = TemplateCompilerSettings{});
    virtual ~TemplateCompiler();

    /** Compiles the given .cmls file to an output .cpp file. */
    bool compile(std::string const &inputPath, std::string const &outputPath);

    /** Bootstraps compiled C++ into a compilable full C++ source file */
    std::string generateCppUnit(std::string const &sourceCpp);

    /** Compiles CMLS code to C++ */
    std::string compileString(std::string const &sourceCmls);

    /** Resolves the virtual with the given variable name. Returns the resolved output C++. Called by @virtual() in the finalize step. */
    std::string finalizeVirtual(std::string const &variableName);

    /** Implements the virtual with the given unparsed source. Called by @implement() */
    void implementVirtual(std::string const &variableName, std::string const &unparsedSource);

    /** Pushes the virtual with the given unparsed source. Called by @push() */
    void pushVirtual(std::string const &variableName, std::string const &unparsedSource);

    /** Registers the given chunk for memory-tracking purposes. This is automatically called by chunk constructor */
    void registerChunk(CompilationChunk *chunk);

    /** Unregisters the given chunk for memory-tracking purposes. This is automatically called by virtual chunk destructor */
    void unregisterChunk(CompilationChunk *chunk);

    /** Adds an #include to the generated output. Must be wrapped in either "" or <>. */
    void addInclude(std::string const &includePathWithQuotesOrTags);

    /** Adds a module to be imported */
    void addImport(std::string const &moduleName);

    /** Adds a parameter to be used in the compiled view */
    void addParameter(std::string const &typeName, std::string const &parameterName);

    std::vector<cmls::Module> importedModules();

    std::string const &root() const;

  protected:
    
    /** Resolves the given chunk until it's fully expanded/resolved and ready to be finalized. */
    std::vector<cmls::CompilationChunk*> resolveChunk(cmls::CompilationChunk *chunk);

    /** Optimize the given resolved chunkset by merging stringliterals together etc. */
    std::vector<cmls::CompilationChunk*> optimizeChunkSet(std::vector<cmls::CompilationChunk*> const &chunkSet);

    /** Finalizes the resolved chunkset and returns main C++ code */
    std::string finalizeChunkSet(std::vector<cmls::CompilationChunk*> const &chunkSet);

    std::string m_root;

    /** Global compiler settings */
    TemplateCompilerSettings m_settings;

    std::set<cmls::CompilationChunk*> m_chunkCache;
    std::map<std::string, std::vector<std::string>> m_virtualCache;
    std::vector<std::string> m_includes;
    std::vector<std::string> m_imports;
    std::map<std::string, std::string> m_parameters;
  };

}