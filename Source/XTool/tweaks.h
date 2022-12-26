#ifndef __TWEAKS_H__
#define __TWEAKS_H__

// For disabling some warning 
#pragma warning( disable : 4786 4284 4800)
//#pragma warning( disable : 4018)
#pragma warning( disable : 4244 4018)
#pragma warning( disable : 4554 4996)

// disable FP contractions
#ifdef _MSC_VER
#pragma fp_contract (off)
#else
#pragma STDC FP_CONTRACT OFF
#endif

#ifdef __ICL
#pragma warning( disable : 880 1125)
#endif

#ifndef FOR_EACH
#define FOR_EACH(list, iterator) \
  for((iterator) = (list).begin(); (iterator) != (list).end(); ++(iterator))
#endif

#ifndef FOR_EACH_AUTO
#define FOR_EACH_AUTO(list, iterator) \
  for(auto iterator = (list).begin(); (iterator) != (list).end(); ++(iterator))
#endif

#define SDL_PRINT_ERROR(MSG) fprintf(stderr, "Error at %s - SDL %s\n", MSG, SDL_GetError())
#define SDL_FATAL_ERROR(MSG) ErrH.Abort("Error at " MSG, XERR_USER, 0, SDL_GetError())

#ifndef FORCEINLINE
#ifdef _MSC_VER
    #if _MSC_VER == 1100 //MSVisual C++ 5.0
    #define FORCEINLINE inline
    #else
    #define FORCEINLINE __forceinline
    #endif
#else
#define FORCEINLINE inline //__attribute__((always_inline))
#endif //_MSC_VER
#endif //FORCEINLINE

#ifndef _WIN32
//TODO strcasecmp is part of POSIX, which is good enough for now
#define stricmp strcasecmp
#endif

//Select per platform path separator
#ifdef _WIN32
#define PATH_SEP '\\'
#define PATH_SEP_STR "\\"
#else
#define PATH_SEP '/'
#define PATH_SEP_STR "/"
#endif

#include "types.h"

#ifdef PERIMETER_EXODUS
//Apply exodus specific tweaks
#include "../Exodus/exodus.h"
#else
#define DestroyEvent CloseHandle
#define strdup _strdup
#define THREAD_ID DWORD
#endif

#define NO_COPY_CONSTRUCTOR(T) \
    T(const T&) = delete; \
    T& operator=(T const&) = delete;

#endif // __TWEAKS_H__
