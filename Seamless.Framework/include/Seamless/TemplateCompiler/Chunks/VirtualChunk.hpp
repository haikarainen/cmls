
#pragma once

#include <Seamless/TemplateCompiler/Chunks/ResolvedChunk.hpp>

namespace cmls
{
  /** A string literal chunk, i.e. straight up markup */
  class CMLS_API VirtualChunk : public cmls::ResolvedChunk
  {
  public:
    VirtualChunk(cmls::TemplateCompiler *compiler, std::string const &variableName);
    virtual ~VirtualChunk();

    /** Output the code from this chunk */
    virtual std::string finalizeChunk() override;

    std::string const &variableName() const;

  protected:
    std::string m_variableName;
  };
}

