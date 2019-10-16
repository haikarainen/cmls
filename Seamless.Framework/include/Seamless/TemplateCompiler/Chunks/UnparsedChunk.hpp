
#pragma once

#include <Seamless/TemplateCompiler/Chunks/CompilationChunk.hpp>

namespace cmls
{

  /** An unparsed source chunk, i.e. unparsed cmls */
  class CMLS_API UnparsedChunk : public cmls::CompilationChunk
  {
    /** Capture mode used for capture mechanism */
    enum CaptureMode : uint8_t
    {
      CM_None,
      CM_Push,
      CM_Implement
    };

    /** A context to neatly resolve unparsed chunks. Persistent through one resolve. */
    struct ResolveContext
    {
      CaptureMode captureMode = CM_None;
      std::string captureName;
      std::string captureData;
    };

  public:
    UnparsedChunk(cmls::TemplateCompiler *compiler, std::string const &source);
    virtual ~UnparsedChunk();

    /** Resolves this chunk */
    virtual std::vector<CompilationChunk*> resolveChunk() override;

    std::string const &source() const;

  protected:

    /** Resolves <@c++ ... @> tags, returns true if found */
    bool resolveCodeLiterals(std::vector<cmls::CompilationChunk*> &outChunks);

    /** Resolves capture mechanism (including @done line-statement), returns true if active */
    bool resolveCapture(ResolveContext &resolveCtx, std::string const &line);

    /** Resolves all line statements except @done, returns true if found */
    bool resolveLineStatement(ResolveContext &resolveCtx, std::vector<cmls::CompilationChunk*> &outChunks, std::string const &line);

    /** Resolves {{ ... }} inline prints, returns true if found  */
    bool resolveInlinePrints(ResolveContext &resolveCtx, std::vector<cmls::CompilationChunk*> &outChunks, std::string const &line);

    /** Resolves @virtual(...) statements, returns true if found */
    bool resolveVirtuals(ResolveContext &resolveCtx, std::vector<CompilationChunk*> &outChunks, std::string const& line);

    /** Resolves flow control line-statements, returns true if found */
    bool resolveFlowControl(std::vector<cmls::CompilationChunk*> &outChunks, std::string const &statementLine);

    /** Resolves @resolve line-statements, returns true if found */
    bool resolveResolve(std::vector<cmls::CompilationChunk*> &outChunks, std::string const &statementLine);

    /** Resolves @import line-statements, returns true if found */
    bool resolveImport(std::string const &statementLine);

    /** Resolves @include line-statements, returns true if found */
    bool resolveInclude(std::string const &statementLine);

    /** Resolves @parameter line-statements, returns true if found */
    bool resolveParameter(std::string const &statementLine);

    /** Resolves @push line-statements, returns true if found */
    bool resolvePush(ResolveContext &resolveCtx, std::string const &statementLine);

    /** Resolves @implement line-statements, returns true if found */
    bool resolveImplement(ResolveContext &resolveCtx, std::string const &statementLine);

    std::string m_source;
  };

}