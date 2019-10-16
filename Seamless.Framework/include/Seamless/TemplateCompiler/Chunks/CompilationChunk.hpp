
#pragma once

#include <Seamless/Export.hpp>

#include <vector>
#include <string>

namespace cmls
{
  class TemplateCompiler;

  /** Baseclass for a compilation chunk */
  class CMLS_API CompilationChunk
  {
  public:
    CompilationChunk(cmls::TemplateCompiler *compiler);
    virtual ~CompilationChunk();

    virtual std::vector<CompilationChunk*> resolveChunk() = 0;

    cmls::TemplateCompiler *context() const;

  protected:
    cmls::TemplateCompiler *m_context = nullptr;
  };
}