
#include "Seamless/TemplateCompiler/Chunks/InlinePrintChunk.hpp"

#include <sstream>

cmls::InlinePrintChunk::InlinePrintChunk(cmls::TemplateCompiler *compiler, std::string const &statement)
  : cmls::ResolvedChunk(compiler)
  , m_statement(statement)
{

}

cmls::InlinePrintChunk::~InlinePrintChunk()
{

}

std::string cmls::InlinePrintChunk::finalizeChunk()
{
  std::stringstream outputStream;
  outputStream << "\t\tresponse << (" << m_statement << ");\n";
  return outputStream.str();
}

std::string const & cmls::InlinePrintChunk::statement() const
{
  return m_statement;
}

