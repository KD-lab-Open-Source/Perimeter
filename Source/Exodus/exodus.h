#pragma once
#ifndef PERIMETER_EXODUS_H
#define PERIMETER_EXODUS_H

#include <cstdint>
#include <cstdlib>
#include <unistd.h>
        
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Win32 macros/types stuff

/*
//Win32 types and their equivalent
 
using INT     = int32_t;
using UINT    = uint32_t;

using LONG    = int32_t;
using ULONG   = uint32_t;

using HRESULT = int32_t;

using WCHAR   = wchar_t;

using BOOL    = INT;

using VOID    = void;

using BYTE    = uint8_t;

using SHORT   = int16_t;
using USHORT  = uint16_t;

using FLOAT    = float;

using DWORD   = uint32_t;
using WORD    = uint16_t;

using HDC      = HANDLE;
using HWND     = HANDLE;
using HKEY     = HANDLE;
using COLORREF = DWORD;

using LPSTR    = char*;
using LPCSTR   = const char*;

struct POINT {
    LONG x;
    LONG y;
};

struct RECT {
    LONG left;
    LONG top;
    LONG right;
    LONG bottom;
};

struct SIZE {
    LONG cx;
    LONG cy;
};
 */

typedef void* HANDLE;

//For some reason not present in dxvk-native headers
typedef uint8_t UCHAR;
typedef void* PVOID;
#ifdef PERIMETER_ARCH_64
typedef uint64_t UINT_PTR;
typedef int64_t LONG_PTR;
#else
typedef unsigned int UINT_PTR;
typedef long LONG_PTR;
#endif
typedef UINT_PTR WPARAM;
typedef LONG_PTR LPARAM;
typedef const char* LPCSTR;
typedef LPCSTR LPCTSTR;

struct _FILETIME {
    unsigned short dwLowDateTime;
    unsigned short dwHighDateTime;
};
typedef _FILETIME FILETIME;

#define MAX_PATH 260
#define _MAX_DRIVE   3
#define _MAX_FNAME   256
#define _MAX_DIR   _MAX_FNAME
#define _MAX_EXT   _MAX_FNAME
#define MAX_COMPUTERNAME_LENGTH 31
#define UNLEN 256 //Defined in lmcons.h

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Use POSIX for MS funcs

#define _O_BINARY 0
#define _O_RDONLY O_RDONLY
#define _O_WRONLY O_WRONLY
#define _O_TRUNC O_TRUNC
#define _O_CREAT O_CREAT
#define _S_IREAD S_IREAD
#define _S_IWRITE S_IWRITE
int _open(const char* path, int oflags, int sflags = 0);
#define _write write
#define _read read
#define _lseek lseek
#define _close close

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//FPU control
//TODO we should handle FPU config stuff on non Win32?

#define _MCW_EM 0
#define _MCW_PC 0
#define _PC_24 0

#define _clearfp()
unsigned int _controlfp(unsigned int newval, unsigned int mask);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Window/UI related

//These seem to be used as mask for storing state in game code
#define MK_LBUTTON  0b1
#define MK_RBUTTON 0b10

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

uint32_t GetPrivateProfileString(const char* section,const char* key,const char* defaultVal,
                              char* returnBuffer, uint32_t bufferSize, const char* filePath);

uint32_t WritePrivateProfileString(const char* section,const char* key,const char* value, const char* filePath);

bool GetComputerName(char* out, uint32_t* size);

bool GetUserName(char* out, uint32_t* size);

void ZeroMemory(void* p, std::size_t n);

void Sleep(uint32_t millis);

char* _strlwr(char* str);
char* _strupr(char* str);

int __iscsym(int c);

#define strlwr _strlwr
#define strupr _strupr

#define IsCharAlphaNumeric isalnum

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Path/Dir stuff

char *_fullpath(char* absolutePath, const char* relativePath, size_t maxLength);

void _splitpath(const char* path_out, char* drive, char* dir, char* fname, char* ext);

void _makepath(char* path, const char* drive, const char* dir, const char* fname, const char* ext);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Event/Thread stuff

#include <pevents.h>

#define INFINITE neosmart::WAIT_INFINITE

#define WAIT_OBJECT_0 0

HANDLE CreateEvent(int, bool manualReset, bool initialState, int);

void DestroyEvent(HANDLE event);

void SetEvent(HANDLE event);

void ResetEvent(HANDLE event);

uint32_t WaitForSingleObject(HANDLE event, uint64_t milliseconds);

uint32_t WaitForMultipleObjects(int count, HANDLE* events, bool waitAll, uint64_t milliseconds);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define D3D9b_SDK_VERSION D3D_SDK_VERSION

#endif //PERIMETER_EXODUS_H
