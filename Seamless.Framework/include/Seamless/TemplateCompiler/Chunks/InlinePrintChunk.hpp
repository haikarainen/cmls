
#pragma once

#include <Seamless/TemplateCompiler/Chunks/ResolvedChunk.hpp>

namespace cmls
{
  /** An inline print chunk, i.e. "{{ model->name }}" */
  class CMLS_API InlinePrintChunk : public cmls::ResolvedChunk
  {
  public:
    InlinePrintChunk(cmls::TemplateCompiler *compiler, std::string const &statement);
    virtual ~InlinePrintChunk();

    /** Output the code from this chunk */
    virtual std::string finalizeChunk() override;

    std::string const &statement() const;

  protected:
    std::string m_statement;
  };
}