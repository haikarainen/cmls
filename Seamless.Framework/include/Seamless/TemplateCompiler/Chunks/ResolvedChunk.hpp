
#pragma once

#include <Seamless/TemplateCompiler/Chunks/CompilationChunk.hpp>

namespace cmls
{

  /** A resolved chunk, i.e. something that is ready to output final text */
  class CMLS_API ResolvedChunk : public cmls::CompilationChunk
  {
  public:
    ResolvedChunk(cmls::TemplateCompiler *compiler);
    virtual ~ResolvedChunk();

    /** Returns {} */
    virtual std::vector<CompilationChunk*> resolveChunk() override;
    virtual std::string finalizeChunk() = 0;
  protected:
  };

}