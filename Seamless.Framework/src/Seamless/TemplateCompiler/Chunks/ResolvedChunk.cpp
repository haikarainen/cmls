
#include "Seamless/TemplateCompiler/Chunks/ResolvedChunk.hpp"

cmls::ResolvedChunk::ResolvedChunk(cmls::TemplateCompiler *compiler)
  : cmls::CompilationChunk(compiler)
{

}

cmls::ResolvedChunk::~ResolvedChunk()
{

}

std::vector<cmls::CompilationChunk*> cmls::ResolvedChunk::resolveChunk()
{
  return {};
}