#include "ICppCompiler.hpp"

std::string const& cmls::ICppCompiler::output() const
{
  return m_output;
}


void cmls::ICppCompiler::writeOutput(std::string const& outputLine)
{
  m_output += outputLine + "\n";
}
