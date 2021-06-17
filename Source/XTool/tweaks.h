#ifndef __TWEAKS_H__
#define __TWEAKS_H__

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
//Apply non MSVC specific tweaks
#include "../Exodus/msvc.h"
#endif


#endif // __TWEAKS_H__