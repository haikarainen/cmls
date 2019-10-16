
#pragma once

#include <Seamless/TemplateCompiler/Chunks/ResolvedChunk.hpp>

namespace cmls
{
  /** A code literal chunk, i.e. straight up C++ code */
  class CMLS_API CodeLiteralChunk : public cmls::ResolvedChunk
  {
  public:
    CodeLiteralChunk(cmls::TemplateCompiler *compiler, std::string const &code);
    virtual ~CodeLiteralChunk();

    /** Output the code from this chunk */
    virtual std::string finalizeChunk() override;

    std::string const &code() const;

  protected:
    std::string m_code;
  };
}