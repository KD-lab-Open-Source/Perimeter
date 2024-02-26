// This file imports sokol header with implementation def set
#include <string>
#include "xerrhand.h"
#if !defined(_WIN32) && defined(SOKOL_D3D11)
//These func's are not available in non windows with DXVK, make them macro
#define LoadLibraryA(a) (nullptr)
#define GetProcAddress(a,b) (nullptr)
#endif
#define SOKOL_IMPL
#define SOKOL_ASSERT xassert
#ifdef __GNUG__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wextra"
#endif
#include <sokol_gfx.h>
#include <sokol_log.h>
#ifdef __GNUG__
#pragma GCC diagnostic pop
#endif
