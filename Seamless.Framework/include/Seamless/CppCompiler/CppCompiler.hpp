
#pragma once

#include "Seamless/Platform.hpp"


#if defined(CMLS_WIN64)

#include "Seamless/CppCompiler/CppCompiler_MSBuild.hpp"

namespace cmls
{
  using CppCompiler = cmls::CppCompiler_MSBuild;
}

#elif defined(CMLS_LINUX)

// @todo, implement via gcc or clang commandline (libclang is too much effort)
#error Not implemented.

#endif