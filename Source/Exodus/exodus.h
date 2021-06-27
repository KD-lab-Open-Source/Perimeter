#ifndef PERIMETER_WINDOWS_H
#define PERIMETER_WINDOWS_H

#include <cstdint>
#include <windows.h>
#include <unistd.h>

inline SDL_Window* fromHWND(HWND hWindow) {
    return reinterpret_cast<SDL_Window*>(hWindow);
}

inline HWND toHWND(SDL_Window* pWindow) {
    return reinterpret_cast<HWND>(pWindow);
}
        
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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

struct _FILETIME {
    unsigned short dwLowDateTime;
    unsigned short dwHighDateTime;
};

//this comes from Win32
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
#define _open open
#define _write write
#define _read read
#define _lseek lseek
#define _close close

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//TODO we should handle FPU config stuff on non Win32?

#define _MCW_EM 0
#define _MCW_PC 0
#define _PC_24 0

#define _clearfp()
unsigned int _controlfp(unsigned int newval, unsigned int mask) { return 0; }

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

DWORD GetPrivateProfileString(const char* section,const char* key,const char* defaultVal,
                              const char* returnBuffer, DWORD bufferSize, const char* filePath);

DWORD WritePrivateProfileString(const char* section,const char* key,const char* value, const char* filePath);

bool GetComputerName(char* out, DWORD* size);

bool GetUserName(char* out, DWORD* size);

void ZeroMemory(void* p, std::size_t n);

short GetAsyncKeyState(int vKey);

void SetFocus(HWND hwnd);

void Sleep(uint32_t millis);

char* _strlwr(char* str);
char* _strupr(char* str);

#define strlwr _strlwr
#define strupr _strupr

#define IsCharAlphaNumeric isalnum

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Path/Dir stuff

int _mkdir(const char* path);

char *_fullpath(char* absolutePath, const char* relativePath, size_t maxLength);

void _splitpath(const char* path, char*, char* dir, char* fname, char* ext);

void _makepath(char* path, const char* drive, const char* dir, const char* fname, const char* ext);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Basic wrapper for Win32 CRITICAL_SECTION stuff
//TODO replace this with posix mutex calls directly on Windows builds?

#include <pthread.h>

typedef pthread_mutex_t CRITICAL_SECTION;

void EnterCriticalSection(CRITICAL_SECTION *m);
void LeaveCriticalSection(CRITICAL_SECTION *m);
void InitializeCriticalSection(CRITICAL_SECTION *m);
void DeleteCriticalSection(CRITICAL_SECTION *m);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//Messaging stuff im not interested in even #ifdef-ing

struct MSG {};
#define PM_NOREMOVE 0

bool PeekMessage(MSG*, void*, uint32_t, uint32_t, uint32_t) { return false; }
bool GetMessage(MSG*, void*, uint32_t, uint32_t) { return true; }
void TranslateMessage(MSG*) {}
void DispatchMessage(MSG*) {}
void WaitMessage() {}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Event/Thread stuff

#include <pevents.h>

#define INFINITE neosmart::WAIT_INFINITE

#define WAIT_OBJECT_0 0

HANDLE CreateEvent(int, bool manualReset, bool initialState, int) {
    return neosmart::CreateEvent(manualReset, initialState);
}

void DestroyEvent(HANDLE event) { 
    neosmart::DestroyEvent(reinterpret_cast<neosmart::neosmart_event_t>(event));
}

void SetEvent(HANDLE event) {
    neosmart::SetEvent(reinterpret_cast<neosmart::neosmart_event_t>(event));
}

void ResetEvent(HANDLE event) { 
    neosmart::ResetEvent(reinterpret_cast<neosmart::neosmart_event_t>(event));
}

DWORD WaitForSingleObject(HANDLE event, uint64_t milliseconds) {
    return neosmart::WaitForEvent(reinterpret_cast<neosmart::neosmart_event_t>(event), milliseconds);
}

DWORD WaitForMultipleObjects(int count, HANDLE* events, bool waitAll, uint64_t milliseconds) {
    return neosmart::WaitForMultipleEvents(reinterpret_cast<neosmart::neosmart_event_t*>(events), count, waitAll, milliseconds);
}

HANDLE CreateThread(void*, size_t,  void *(*start_address) (void *), void* arg, DWORD, DWORD*) {
    pthread_t* tid = 0;
    pthread_create(tid, nullptr, start_address, arg);
    return neosmart::CreateEvent(true, false);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define D3D9b_SDK_VERSION D3D_SDK_VERSION

#endif //PERIMETER_WINDOWS_H
