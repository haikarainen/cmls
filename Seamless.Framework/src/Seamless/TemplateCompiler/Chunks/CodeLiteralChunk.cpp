
#include "Seamless/TemplateCompiler/Chunks/CodeLiteralChunk.hpp"

#include <sstream>

cmls::CodeLiteralChunk::CodeLiteralChunk(cmls::TemplateCompiler *compiler, std::string const &code)
  : cmls::ResolvedChunk(compiler)
  , m_code(code)
{

}

cmls::CodeLiteralChunk::~CodeLiteralChunk()
{

}

std::string cmls::CodeLiteralChunk::finalizeChunk()
{
  std::stringstream outputStream;
  outputStream << m_code;
  return outputStream.str();
}

std::string const & cmls::CodeLiteralChunk::code() const
{
  return m_code;
}

