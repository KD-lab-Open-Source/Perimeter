#ifndef PERIMETER_WINDOWS_H
#define PERIMETER_WINDOWS_H

#include <cstdint>

//For some reason not present in dxvk-native headers
typedef uint8_t UCHAR;

struct _FILETIME {
    unsigned short dwLowDateTime;
    unsigned short dwHighDateTime;
};

//this comes from Win32
#define MAX_PATH 260

void ZeroMemory(void *p, std::size_t n);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void GetCurrentDirectory(unsigned short size, char* path);

void Sleep(uint32_t millis);

char* _strlwr(char* str);
char* _strupr(char* str);

#define strlwr _strlwr
#define strupr _strupr

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Basic wrapper for Win32 CRITICAL_SECTION stuff
//TODO replace this with posix mutex calls directly on Windows builds?

#include <pthread.h>

typedef pthread_mutex_t CRITICAL_SECTION;

static void EnterCriticalSection(CRITICAL_SECTION *m);
static void LeaveCriticalSection(CRITICAL_SECTION *m);
static void InitializeCriticalSection(CRITICAL_SECTION *m);
static void DeleteCriticalSection(CRITICAL_SECTION *m);

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

#define D3D9b_SDK_VERSION D3D_SDK_VERSION

#endif //PERIMETER_WINDOWS_H
