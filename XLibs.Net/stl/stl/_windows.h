/*
 * Copyright (c) 1997-1999
 * Silicon Graphics Computer Systems, Inc.
 *
 * Copyright (c) 1999 
 * Boris Fomitchev
 *
 * Copyright (c) 2003 
 * Francois Dumont
 *
 * This material is provided "as is", with absolutely no warranty expressed
 * or implied. Any use is at your own risk.
 *
 * Permission to use or copy this software for any purpose is hereby granted 
 * without fee, provided the above notices are retained on all copies.
 * Permission to modify the code and to distribute modified code is granted,
 * provided the above notices are retained, and a notice that the code was
 * modified is included with the above copyright notice.
 *
 */
#ifndef _STLP_INTERNAL_WINDOWS_H
#define _STLP_INTERNAL_WINDOWS_H

#if !defined (_STLP_WINDOWS_H_INCLUDED)
#  define _STLP_WINDOWS_H_INCLUDED
#  if !(defined ( _STLP_MSVC ) || defined (__BORLANDC__) || defined (__ICL) || defined (__WATCOMC__) || \
        defined (__MINGW32__) || defined (__DMC__))
#    if defined (_STLP_USE_MFC)
#      include <afx.h>
#    else
#      include <windows.h>
#    endif
#  else
// This section serves as a replacement for windows.h header for Visual C++
extern "C" {
#    if (defined(_M_AMD64) || defined(_M_IA64) || (!defined(_STLP_WCE) && defined(_M_MRX000)) || defined(_M_ALPHA) || \
        (defined(_M_PPC) && (_MSC_VER >= 1000))) && !defined(RC_INVOKED)
#      define InterlockedIncrement       _InterlockedIncrement
#      define InterlockedDecrement       _InterlockedDecrement
#      define InterlockedExchange        _InterlockedExchange
//Here we use a different macro name than the InterlockedExchangePointer SDK function
// to avoid macro definition conflict.
#      define STLPInterlockedExchangePointer _InterlockedExchangePointer
#      define _STLP_STDCALL
#    else
#      if defined (_MAC)
#        define _STLP_STDCALL _cdecl
#      else
#        define _STLP_STDCALL __stdcall
#      endif
#    endif

#    if defined (_STLP_NEW_PLATFORM_SDK)
_STLP_IMPORT_DECLSPEC long _STLP_STDCALL InterlockedIncrement(long volatile *);
_STLP_IMPORT_DECLSPEC long _STLP_STDCALL InterlockedDecrement(long volatile *);
_STLP_IMPORT_DECLSPEC long _STLP_STDCALL InterlockedExchange(long volatile *, long);
#      if defined (STLPInterlockedExchangePointer)
_STLP_IMPORT_DECLSPEC void* _STLP_STDCALL STLPInterlockedExchangePointer(void* volatile *, void*);
#      endif
_STLP_IMPORT_DECLSPEC long _STLP_STDCALL InterlockedCompareExchange(long volatile *, long, long);
#    elif defined (_STLP_WCE)

// start of eMbedded Visual C++ specific section

#include <windef.h> // needed for basic windows types

long WINAPI InterlockedIncrement(long*);
long WINAPI InterlockedDecrement(long*);
long WINAPI InterlockedExchange(long*, long);

#if defined(x86)
#include <winbase.h> // needed for inline versions of Interlocked* functions
#endif

#define MessageBox MessageBoxW
int WINAPI MessageBoxW(HWND hWnd, LPCWSTR lpText, LPCWSTR lpCaption, UINT uType);

#define wvsprintf wvsprintfW
int WINAPI wvsprintfW(LPWSTR, LPCWSTR, va_list ArgList);

void WINAPI ExitThread(DWORD dwExitCode);

#if !defined(COREDLL)
#define _STLP_WCE_WINBASEAPI DECLSPEC_IMPORT
#else
#define _STLP_WCE_WINBASEAPI
#endif

_STLP_WCE_WINBASEAPI int WINAPI MultiByteToWideChar(UINT CodePage, DWORD dwFlags, LPCSTR lpMultiByteStr,
     int      cbMultiByte, LPWSTR lpWideCharStr, int cchWideChar);

_STLP_WCE_WINBASEAPI UINT WINAPI GetACP();

_STLP_WCE_WINBASEAPI BOOL WINAPI TerminateProcess(HANDLE hProcess, DWORD uExitCode);

#define OutputDebugString OutputDebugStringW
void WINAPI OutputDebugStringW(LPCWSTR);

_STLP_WCE_WINBASEAPI void WINAPI Sleep(DWORD);

#undef _STLP_WCE_WINBASEAPI

// end of eMbedded Visual C++ specific section

#    else
// boris : for the latest SDK, you may actually need the other version of the declaration (above)
// even for earlier VC++ versions. There is no way to tell SDK versions apart, sorry ...
_STLP_IMPORT_DECLSPEC long _STLP_STDCALL InterlockedIncrement(long*);
_STLP_IMPORT_DECLSPEC long _STLP_STDCALL InterlockedDecrement(long*);
_STLP_IMPORT_DECLSPEC long _STLP_STDCALL InterlockedExchange(long*, long);
#    endif

#    if !defined (STLPInterlockedExchangePointer)
//This API function do not exist in the old platform SDK and is equivalent to
//InterlockedExchange on 32 bits platform:
#      define STLPInterlockedExchangePointer(a, b) (void*)InterlockedExchange((long*)a, (long)b)
#    endif

#    if !defined(_STLP_WCE)
_STLP_IMPORT_DECLSPEC void _STLP_STDCALL Sleep(unsigned long);
_STLP_IMPORT_DECLSPEC void _STLP_STDCALL OutputDebugStringA(const char* lpOutputString);
#    endif

#    if defined (InterlockedIncrement)
#      pragma intrinsic(_InterlockedIncrement)
#      pragma intrinsic(_InterlockedDecrement)
#      pragma intrinsic(_InterlockedExchange)
#      pragma intrinsic(_InterlockedExchangePointer)
#    endif
} /* extern "C" */

#  endif /* STL_MSVC __BORLANDC__ __ICL __WATCOMC__ __MINGW32__ __DMC__*/

#endif /* _STLP_WINDOWS_H_INCLUDED */

#endif /* _STLP_INTERNAL_WINDOWS_H */
