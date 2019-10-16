
#include "Seamless/TemplateCompiler/Chunks/VirtualChunk.hpp"

#include "Seamless/TemplateCompiler/TemplateCompiler.hpp"

cmls::VirtualChunk::VirtualChunk(cmls::TemplateCompiler *compiler, std::string const &variableName)
  : cmls::ResolvedChunk(compiler)
  , m_variableName(variableName)
{

}

cmls::VirtualChunk::~VirtualChunk()
{

}

std::string cmls::VirtualChunk::finalizeChunk()
{
  return context()->finalizeVirtual(m_variableName);
}

std::string const & cmls::VirtualChunk::variableName() const
{
  return m_variableName;
}
