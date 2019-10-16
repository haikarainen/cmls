
#include "Seamless/TemplateCompiler/Chunks/CompilationChunk.hpp"

#include "Seamless/TemplateCompiler/TemplateCompiler.hpp"

cmls::CompilationChunk::CompilationChunk(cmls::TemplateCompiler *compiler)
  : m_context(compiler)
{
  compiler->registerChunk(this);
}

cmls::CompilationChunk::~CompilationChunk()
{
  context()->unregisterChunk(this);
}

cmls::TemplateCompiler * cmls::CompilationChunk::context() const
{
  return m_context;
}
