
#pragma once

#include <Seamless/Export.hpp>
#include <Seamless/CppCompiler/ICppCompiler.hpp>

namespace cmls 
{
  class CMLS_API CppCompiler_MSBuild : public cmls::ICppCompiler
  {
  public:
    /** Compiles the given project */
    virtual bool compile(CppProject const &project) override;
  };

}