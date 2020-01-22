#pragma once

#if defined(__NT__)

#ifdef OPENAIRDLL_EXPORTS
#define OPENAIRDLL_API  __declspec(dllexport)
#else
#define OPENAIRDLL_API  __declspec(dllimport)
#endif

#elif defined(__UNIX__)

#if defined(__MAC__)
#else // LINUX

#ifdef OPENAIRDLL_EXPORTS
#define OPENAIRDLL_API 
#else 
#define OPENAIRDLL_API  
#endif

#endif // LINUX

#endif // __UNIX__
