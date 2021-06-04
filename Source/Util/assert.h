#ifdef _DEBUG
#include "xutil.h" 
#define _assert(err, file, line) \
    do                                                              \
    {  static int ignore = 0;                                   \
        if (!ignore)				\
		switch(DiagAssert ( 0 , (const char*)err , (const char*)file  , line)){  \
			case 1: ignore = 1; break; \
			case 2: __asm { int 3 }; break; \
			}\
    } while (0)
 #else
#undef assert
#define assert(exp)     ((void)0)
#define _assert(err, file, line) 
 #endif

