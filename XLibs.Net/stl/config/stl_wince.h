/*
 * File to have Windows CE Toolkit for VC++ 5.0 working with STLport
 * 09 - 03 - 1999
 * Origin : Giuseppe Govi - g.govi@iol.it
 */

#ifndef _STLP_WINCE_H
#define _STLP_WINCE_H

// this flag is being used by STLport
#   define _STLP_WINCE

#ifndef _MT                            // Always threaded in CE
  #define _MT
#endif

#define _STLP_NO_NATIVE_MBSTATE_T
#define _STLP_NO_TYPEINFO
#define _STLP_NO_BAD_ALLOC
#define _STLP_NO_NEW_NEW_HEADER

// tell other parts no threads are there
#   define _STLP_NO_THREADS 1

// not all new-style headers are available...
# define _STLP_HAS_NO_NEW_C_HEADERS

#     undef _STLP_HAS_NO_EXCEPTIONS
#     define _STLP_HAS_NO_EXCEPTIONS
#     undef _STLP_NO_EXCEPTION_HEADER
#     define _STLP_NO_EXCEPTION_HEADER

// we have to use malloc instead of new
# undef  _STLP_USE_NEWALLOC
# define _STLP_USE_MALLOC

//# ifdef _STLP_MSVC
//#     pragma warning (disable: 4786)
//# endif

#ifdef _STLP_WINCE_USE_OUTPUTDEBUGSTRING
#define _STLP_WINCE_TRACE(msg)   OutputDebugString(msg)
#else
#define _STLP_WINCE_TRACE(msg)   MessageBox(NULL,(msg),NULL,MB_OK)
#endif

#ifndef __THROW_BAD_ALLOC
#define __THROW_BAD_ALLOC _STLP_WINCE_TRACE(L"out of memory"); ExitThread(1)
#endif

#ifndef _SIZE_T_DEFINED
typedef unsigned int size_t;
#define _SIZE_T_DEFINED
#endif

#ifndef _WCHAR_T_DEFINED
typedef unsigned short wchar_t;
#define _WCHAR_T_DEFINED
#endif

#ifndef _TIME_T_DEFINED
typedef unsigned long time_t;
#define _TIME_T_DEFINED
#endif

//ptrdiff_t is not defined in Windows CE SDK
#ifndef _PTRDIFF_T_DEFINED
typedef int ptrdiff_t;
#define _PTRDIFF_T_DEFINED
#endif

//clock_t is not defined in Windows CE SDK
#ifndef _CLOCK_T_DEFINED
typedef long clock_t;
#define _CLOCK_T_DEFINED
#endif

//struct tm is not defined in Windows CE SDK
#ifndef _TM_DEFINED
struct tm {
        int tm_sec;     /* seconds after the minute - [0,59] */
        int tm_min;     /* minutes after the hour - [0,59] */
        int tm_hour;    /* hours since midnight - [0,23] */
        int tm_mday;    /* day of the month - [1,31] */
        int tm_mon;     /* months since January - [0,11] */
        int tm_year;    /* years since 1900 */
        int tm_wday;    /* days since Sunday - [0,6] */
        int tm_yday;    /* days since January 1 - [0,365] */
        int tm_isdst;   /* daylight savings time flag */
        };
#define _TM_DEFINED
#endif

// Some useful routines that are missing in Windows CE SDK
#ifdef __cplusplus
extern "C"
{
#endif

  char *      __cdecl getenv(const char *);
  struct tm * __cdecl gmtime(const time_t *);
  int         __cdecl remove(const char *);
  int         __cdecl rename(const char *, const char *);
  time_t      __cdecl time(time_t *);

  #if (_WIN32_WCE < 300)
    char * __cdecl strrchr(const char *, int);
  #endif

#ifdef __cplusplus
}

#  ifndef __PLACEMENT_NEW_INLINE
inline void *__cdecl operator new(size_t, void *_P) { return (_P); }
#    define __PLACEMENT_NEW_INLINE
#  endif

#endif /* __cplusplus */

#if !defined(WIN32_LEAN_AND_MEAN)       // Minimise windows includes
  #define WIN32_LEAN_AND_MEAN
#endif
#if !defined(VC_EXTRALEAN)
  #define VC_EXTRALEAN
#endif
#if !defined(STRICT)
  #define STRICT
#endif
#if !defined(NOMINMAX)
  #define NOMINMAX
#endif

#ifndef __WINDOWS__
#include <windows.h>
#endif

#ifndef _ABORT_DEFINED
# define _STLP_ABORT() TerminateProcess(GetCurrentProcess(), 0)
# define _ABORT_DEFINED
#endif

#ifndef _ASSERT_DEFINED
# define assert(expr) _STLP_ASSERT(expr)
# define _ASSERT_DEFINED
#endif

// they say it's needed 
# include <windows.h>

#endif /* _STLP_WCE_H */


