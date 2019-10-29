
#pragma once

#if defined(BLOG_BUILD)
#define BLOG_API __declspec(dllexport)
#else 
#define BLOG_API __declspec(dllimport)
#endif