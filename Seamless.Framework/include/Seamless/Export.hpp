
#pragma once

#if defined(CMLS_BUILD)
  #define CMLS_API __declspec(dllexport)
#else
  #define CMLS_API __declspec(dllimport)
#endif