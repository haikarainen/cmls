
#pragma once

#include <Seamless/TemplateCompiler/Chunks/ResolvedChunk.hpp>

namespace cmls 
{
  /** A string literal chunk, i.e. straight up markup */
  class CMLS_API StringLiteralChunk : public cmls::ResolvedChunk
  {
  public:
    StringLiteralChunk(cmls::TemplateCompiler *compiler, std::string const &string);
    virtual ~StringLiteralChunk();

    /** Output the code from this chunk */
    virtual std::string finalizeChunk() override;

    std::string const &string() const;

    void append(std::string const &str);

  protected:
    std::string m_string;
  };
}

