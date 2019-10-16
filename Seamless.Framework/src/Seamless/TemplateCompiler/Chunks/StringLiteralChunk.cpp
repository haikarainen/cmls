
#include "Seamless/TemplateCompiler/Chunks/StringLiteralChunk.hpp"

#include "Seamless/String.hpp"

#include <sstream>

cmls::StringLiteralChunk::StringLiteralChunk(cmls::TemplateCompiler *compiler, std::string const &string)
  : cmls::ResolvedChunk(compiler)
  , m_string(string)
{

}

cmls::StringLiteralChunk::~StringLiteralChunk()
{

}

std::string cmls::StringLiteralChunk::finalizeChunk()
{
  std::string escapedString = cmls::escapeString(m_string);
  std::stringstream outputStream;
  outputStream << "\t\tresponse << " << escapedString << ";\n";
  return outputStream.str();
}

std::string const & cmls::StringLiteralChunk::string() const
{
  return m_string;
}

void cmls::StringLiteralChunk::append(std::string const &str)
{
  m_string += str;
}

