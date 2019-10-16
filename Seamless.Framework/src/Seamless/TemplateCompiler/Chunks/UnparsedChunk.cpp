
#include "Seamless/TemplateCompiler/Chunks/UnparsedChunk.hpp"
#include "Seamless/TemplateCompiler/Chunks/CodeLiteralChunk.hpp"
#include "Seamless/TemplateCompiler/Chunks/FlowControlChunk.hpp"
#include "Seamless/TemplateCompiler/Chunks/StringLiteralChunk.hpp"
#include "Seamless/TemplateCompiler/Chunks/InlinePrintChunk.hpp"
#include "Seamless/TemplateCompiler/Chunks/VirtualChunk.hpp"
#include "Seamless/TemplateCompiler/TemplateCompiler.hpp"

#include "Seamless/Filesystem.hpp"
#include "Seamless/String.hpp"
#include "Seamless/Error.hpp"

cmls::UnparsedChunk::UnparsedChunk(cmls::TemplateCompiler *compiler, std::string const &source)
  : cmls::CompilationChunk(compiler)
  , m_source(source)
{

}

cmls::UnparsedChunk::~UnparsedChunk()
{

}

std::vector<cmls::CompilationChunk*> cmls::UnparsedChunk::resolveChunk()
{
  std::vector<cmls::CompilationChunk*> chunks;

  // First check for code literals, <@c++ @>
  // Will separate this chunk into a series of UnparsedChunk's and CodeLiteralChunk's
  if (resolveCodeLiterals(chunks))
  {
    return chunks;
  }

  // No code literals in this unparsed chunk, split source into lines and parse them 
  auto lines = cmls::splitLines(m_source);
  ResolveContext resolveCtx;

  for (auto line : lines)
  {
    if (resolveCapture(resolveCtx, line))
    {
      continue;
    }

    if (resolveLineStatement(resolveCtx, chunks, line))
    {
      continue;
    }

    if (resolveInlinePrints(resolveCtx, chunks, line))
    {
      continue;
    }

    if (resolveVirtuals(resolveCtx, chunks, line))
    {
      continue;
    }

    // Nothing to be recognized in this line, output as string literal
    chunks.push_back(new cmls::StringLiteralChunk(context(), line));
  }

  return chunks;
}

std::string const & cmls::UnparsedChunk::source() const
{
  return m_source;
}

bool cmls::UnparsedChunk::resolveCodeLiterals(std::vector<cmls::CompilationChunk*> &outChunks)
{
  static std::string const startTag = "<@c++";
  static std::string const endTag = "@>";
  size_t i = 0;
  size_t offset = 0;
  bool foundCodeLiteral = false;

  do
  {
    i = m_source.find(startTag, offset);
    if (i != std::string::npos)
    {
      foundCodeLiteral = true;

      if (i != 0)
      {
        outChunks.push_back(new cmls::UnparsedChunk(context(), cmls::substring(m_source, offset, i - offset)));
      }

      auto e = m_source.find(endTag, i);
      if (e == std::string::npos)
      {
        e = 0;
      }

      outChunks.push_back(new cmls::CodeLiteralChunk(context(), cmls::substring(m_source, i + startTag.size(), e - i - startTag.size())));
      offset = e + endTag.size();
    }
  } while (i != std::string::npos);

  if (foundCodeLiteral)
  {
    if (offset < m_source.size())
    {
      outChunks.push_back(new cmls::UnparsedChunk(context(), cmls::substring(m_source, offset)));
    }
    return true;
  }

  return false;
}

bool cmls::UnparsedChunk::resolveCapture(ResolveContext &resolveCtx, std::string const &line)
{
  // If we capture, output to capture buffer
  if (resolveCtx.captureMode != CM_None)
  {
    auto trimmedLine = cmls::trim(line);

    if (cmls::strBeginsWith(trimmedLine, "@done"))
    {
      switch (resolveCtx.captureMode)
      {
      case CM_Push:
        context()->pushVirtual(resolveCtx.captureName, resolveCtx.captureData);
        break;
      case CM_Implement:
        context()->implementVirtual(resolveCtx.captureName, resolveCtx.captureData);
        break;
      }

      resolveCtx.captureName = "";
      resolveCtx.captureData = "";
      resolveCtx.captureMode = CM_None;
    }
    else
    {
      resolveCtx.captureData += line;
    }

    return true;
  }

  return false;
}

bool cmls::UnparsedChunk::resolveLineStatement(ResolveContext &resolveCtx, std::vector<cmls::CompilationChunk*> &outChunks, std::string const &line)
{
  std::string trimmedLine = cmls::trim(line);

  if (trimmedLine.size() > 0)
  {
    if (trimmedLine[0] == '@')
    {
      std::string statementLine = cmls::substring(trimmedLine, 1);

      if (resolveFlowControl(outChunks, statementLine))
      {
        return true;
      }

      if (resolveResolve(outChunks, statementLine))
      {
        return true;
      }

      if (resolveImport(statementLine))
      {
        return true;
      }

      if (resolveInclude(statementLine))
      {
        return true;
      }

      if (resolveParameter(statementLine))
      {
        return true;
      }

      if (resolvePush(resolveCtx, statementLine))
      {
        return true;
      }

      if (resolveImplement(resolveCtx, statementLine))
      {
        return true;
      }

    }
  }

  return false;
}

bool cmls::UnparsedChunk::resolveFlowControl(std::vector<cmls::CompilationChunk*> &outChunks,std::string const &statementLine)
{
  if (cmls::strBeginsWithAny(statementLine, { "if", "for", "while", "do", "end" }))
  {
    outChunks.push_back(new cmls::FlowControlChunk(context(), statementLine));
    return true;
  }

  return false;
}

bool cmls::UnparsedChunk::resolveResolve(std::vector<cmls::CompilationChunk*> &outChunks, std::string const &statementLine)
{
  if (cmls::strBeginsWith(statementLine, "resolve"))
  {
    auto filename = context()->root() + "\\" + cmls::trim(cmls::substring(statementLine, 7), " \"'<>");
    cmls::File file(filename);

    if (!file.exist())
    {
      LogError("File doesn't exist (%s)", file.fullPath().c_str());
      return true;
    }

    auto f = cmls::stringFromFile(file.fullPath());
    if (!f.has_value())
    {
      // @todo error handling
      LogError("Could not load file from path (%s)", filename.c_str());
      return true;
    }

    outChunks.push_back(new cmls::UnparsedChunk(context(), f.value()));
    return true;
  }

  return false;
}

bool cmls::UnparsedChunk::resolveImport(std::string const &statementLine)
{
  if (cmls::strBeginsWith(statementLine, "import"))
  {
    auto moduleName = cmls::trim(cmls::substring(statementLine, 6), " \"'<>");
    context()->addImport(moduleName);
    return true;
  }

  return false;
}

bool cmls::UnparsedChunk::resolveInclude(std::string const &statementLine)
{
  if (cmls::strBeginsWith(statementLine, "include"))
  {
    auto includePath = cmls::trim(cmls::substring(statementLine, 7));
    context()->addInclude(includePath);
    return true;
  }

  return false;
}

bool cmls::UnparsedChunk::resolveParameter(std::string const &statementLine)
{
  if (cmls::strBeginsWith(statementLine, "parameter"))
  {
    auto args = cmls::split(cmls::trim(cmls::substring(statementLine, 9)), { ' ' });

    if (args.size() != 2)
    {
      LogError("Invalid parameter, expects a type and a name separated by spaces");
      return true;
    }

    context()->addParameter(args[0], args[1]);
    return true;
  }

  return false;
}

bool cmls::UnparsedChunk::resolvePush(ResolveContext &resolveCtx, std::string const &statementLine)
{
  if (cmls::strBeginsWith(statementLine, "push"))
  {
    auto rem = cmls::trim(cmls::substring(statementLine, 4));
    if (rem.size() > 0)
    {
      size_t firstSeparator = rem.find_first_of(" \t", 0);
      if (firstSeparator == std::string::npos)
      {
        resolveCtx.captureMode = CM_Push;
        resolveCtx.captureName = rem;
        resolveCtx.captureData = "";
      }
      else
      {
        auto variableName = cmls::substring(rem, 0, firstSeparator);
        auto source = cmls::substring(rem, firstSeparator + 1, 0);
        context()->pushVirtual(variableName, source);
      }
    }
    else
    {
      LogError("push requires at least one argument");
    }

    return true;
  }

  return false;
}

bool cmls::UnparsedChunk::resolveImplement(ResolveContext &resolveCtx, std::string const &statementLine)
{
  if (cmls::strBeginsWith(statementLine, "implement"))
  {
    auto rem = cmls::trim(cmls::substring(statementLine, 9));
    if (rem.size() > 0)
    {
      size_t firstSeparator = rem.find_first_of(" \t", 0);
      if (firstSeparator == std::string::npos)
      {
        resolveCtx.captureMode = CM_Implement;
        resolveCtx.captureName = rem;
        resolveCtx.captureData = "";
      }
      else
      {
        auto variableName = cmls::substring(rem, 0, firstSeparator);
        auto source = cmls::substring(rem, firstSeparator + 1, 0);
        context()->implementVirtual(variableName, source);
      }
    }
    else
    {
      LogError("push requires at least one argument");
    }

    return true;
  }

  return false;
}

bool cmls::UnparsedChunk::resolveInlinePrints(ResolveContext &resolveCtx, std::vector<cmls::CompilationChunk*> &outChunks, std::string const &line)
{
  // Then check for inline prints: {{ statement }}
  static std::string const startTag = "{{";
  static std::string const endTag = "}}";
  bool inlinePrintFound = false;
  size_t i = 0;
  size_t offset = 0;

  do
  {
    i = line.find(startTag, offset);
    if (i != std::string::npos)
    {
      inlinePrintFound = true;

      if (i != 0)
      {
        outChunks.push_back(new cmls::UnparsedChunk(context(), cmls::substring(line, offset, i - offset)));
      }

      auto e = line.find(endTag, i);
      if (e == std::string::npos)
      {
        e = 0;
      }

      outChunks.push_back(new cmls::InlinePrintChunk(context(), cmls::substring(line, i + startTag.size(), e - i - startTag.size())));
      offset = e + endTag.size();
    }
  } while (i != std::string::npos);

  if (inlinePrintFound)
  {
    if (offset < line.size())
    {
      outChunks.push_back(new cmls::UnparsedChunk(context(), cmls::substring(line, offset)));
    }
    
    return true;
  }

  return false;
}

bool cmls::UnparsedChunk::resolveVirtuals(ResolveContext &resolveCtx, std::vector<CompilationChunk*> &outChunks, std::string const& line)
{
  bool virtualFound = false;
  size_t i = 0;
  size_t offset = 0;
  static std::string const startTag = "@virtual(";
  static std::string const endTag = ")";
  do
  {
    i = line.find(startTag, offset);
    if (i != std::string::npos)
    {
      virtualFound = true;
      if (i != 0)
      {
        outChunks.push_back(new cmls::UnparsedChunk(context(), cmls::substring(line, offset, i - offset)));
      }

      auto e = line.find(endTag, i);
      if (e == std::string::npos)
      {
        e = 0;
      }

      outChunks.push_back(new cmls::VirtualChunk(context(), cmls::substring(line, i + startTag.size(), e - i - startTag.size())));
      offset = e + endTag.size();
    }
  } while (i != std::string::npos);

  if (virtualFound)
  {
    if (offset < line.size())
    {
      outChunks.push_back(new cmls::UnparsedChunk(context(), cmls::substring(line, offset)));
    }

    return true;
  }

  return false;
}

