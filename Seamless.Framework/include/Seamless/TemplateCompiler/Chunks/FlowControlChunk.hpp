
#pragma once

#include <Seamless/TemplateCompiler/Chunks/ResolvedChunk.hpp>

namespace cmls
{
  /** A flow control chunk, i.e. @for(...) */
  class CMLS_API FlowControlChunk : public cmls::ResolvedChunk
  {
  public:
    FlowControlChunk(cmls::TemplateCompiler *compiler, std::string const &statementLine);
    virtual ~FlowControlChunk();

    /** Output the code from this chunk */
    virtual std::string finalizeChunk() override;

    std::string const &statementLine() const;

  protected:
    std::string m_statementLine;
  };
}