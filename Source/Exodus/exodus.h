#pragma once
#ifndef PERIMETER_WINDOWS_H
#define PERIMETER_WINDOWS_H

#include <cstdint>
#include <windows.h>
#include <unistd.h>
        
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Win32 macros/types stuff

//For some reason not present in dxvk-native headers
typedef uint8_t UCHAR;
typedef LPVOID PVOID;
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

DWORD GetPrivateProfileString(const char* section,const char* key,const char* defaultVal,
                              char* returnBuffer, DWORD bufferSize, const char* filePath);

DWORD WritePrivateProfileString(const char* section,const char* key,const char* value, const char* filePath);

bool GetComputerName(char* out, DWORD* size);

bool GetUserName(char* out, DWORD* size);

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

DWORD WaitForSingleObject(HANDLE event, uint64_t milliseconds);

DWORD WaitForMultipleObjects(int count, HANDLE* events, bool waitAll, uint64_t milliseconds);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define D3D9b_SDK_VERSION D3D_SDK_VERSION

#endif //PERIMETER_WINDOWS_H
