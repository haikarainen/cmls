
#include "Seamless/TemplateCompiler/Chunks/FlowControlChunk.hpp"

#include "Seamless/String.hpp"
#include "Seamless/Error.hpp"

#include <sstream>

cmls::FlowControlChunk::FlowControlChunk(cmls::TemplateCompiler *compiler,std::string const &line)
  : ResolvedChunk(compiler)
  , m_statementLine(line)
{

}

cmls::FlowControlChunk::~FlowControlChunk()
{

}

std::string cmls::FlowControlChunk::finalizeChunk()
{
  if (cmls::strBeginsWithAny(m_statementLine, {"if", "for", "while", "do"}))
  {
    std::stringstream outputStream;
    outputStream << statementLine() << "\n{\n" ;
    return outputStream.str();
  }
  else if (cmls::strBeginsWith(m_statementLine, "end"))
  {
    std::string endLine = cmls::trim(cmls::substring(m_statementLine, 3));

    std::stringstream outputStream;
    outputStream << "}";
    
    if (endLine.size() > 0)
    {
      outputStream << "while" << endLine << ";";
    }
    
    outputStream << "\n";
    return outputStream.str();
  }
  else
  {
    LogError("invalid chunk line");
    // @todo notify compiler
    return "";
  }

}

std::string const & cmls::FlowControlChunk::statementLine() const
{
  return m_statementLine;
}

