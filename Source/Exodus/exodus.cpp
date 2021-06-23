#include "exodus.h"
#include <cstring>
#include <cctype>
#include <thread>
#include <sys/stat.h>

void _mkdir(const char* path) {
    mkdir(path, 0);
}

void ZeroMemory(void *p, size_t n) {
    memset(p, 0, n);
}

void _splitpath(const char *path, char *drive, char *dir, char *fname, char *ext) {
    //TODO set dir fname and ext
}

void _makepath(const char *path, char *drive, char *dir, char *fname, char *ext) {
    //TODO set path from dir fname and ext
}

void GetCurrentDirectory(unsigned short size, char* path) {
    //TODO use SDL2 method to get dir
}

void Sleep(uint32_t millis) {
    std::this_thread::sleep_for(std::chrono::milliseconds(millis));
}

char* _strlwr(char* str)
{
    unsigned char* p = (unsigned char *) str;

    while (*p) {
        *p = tolower((unsigned char) *p);
        p++;
    }

    return str;
}

char* _strupr(char* str)
{
    unsigned char* p = (unsigned char *) str;

    while (*p) {
        *p = toupper((unsigned char) *p);
        p++;
    }

    return str;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void EnterCriticalSection(CRITICAL_SECTION *m) {
    pthread_mutex_lock(m);
}

void LeaveCriticalSection(CRITICAL_SECTION *m) {
    pthread_mutex_unlock(m);
}

void InitializeCriticalSection(CRITICAL_SECTION *m) {
    pthread_mutex_init(m, nullptr);
}

void DeleteCriticalSection(CRITICAL_SECTION *m) {
    pthread_mutex_destroy(m);
}
