////////////////////////////////////////////////////////////////////////////////
//
// Additional include for making STL more convenient
//
// Author: Alexandre Kotliar, K-D Lab
//
////////////////////////////////////////////////////////////////////////////////
#ifndef __STL_ADDITION_H__
#define __STL_ADDITION_H__

// For disabling some warning 
#pragma warning( disable : 4786 4284 4800)
//#pragma warning( disable : 4018)
#pragma warning( disable : 4244 4018)
#pragma warning( disable : 4554 4996)

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

#if defined(_MSC_VER) && (_MSC_VER < 1900)
#define THREAD_LOCAL __declspec( thread )
#else
#define THREAD_LOCAL thread_local
#endif

#ifdef _MSC_VER
    #if _MSC_VER == 1100 //MSVisual C++ 5.0
    #define FORCEINLINE inline
    #else
    #define FORCEINLINE __forceinline
    #endif
#else
#define FORCEINLINE inline //__attribute__((always_inline))
#endif //_MSC_VER

#ifndef _WIN32
//TODO is strcasecmp available in non linux platforms?
#define stricmp strcasecmp
//TODO this is some Windows window handler pointer, it should be abstracted with SDL2 like in Vangers
typedef void *HWND;
//TODO replace the usages in codebase
#include <cstdint>
typedef uint8_t BYTE;
typedef uint16_t WORD;
typedef uint32_t DWORD;
typedef uint32_t UINT;
//TODO this comes from Win32
#define MAX_PATH 260
//TODO use SDL2 or something?
void GetCurrentDirectory(unsigned short size, char* path) {
}
#endif


#endif // __STL_ADDITION_H__