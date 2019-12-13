#pragma once

#ifdef OPENAIRDLL_EXPORTS
#define OPENAIRDLL_API  __declspec(dllexport)
#else
#define OPENAIRDLL_API  __declspec(dllimport)
#endif

