#include "exodus.h"
#include "tweaks.h"
#include <cstring>
#include <cctype>
#include <thread>
#include <sys/stat.h>
#include <SDL.h>
#include <pwd.h>
#include <string>

short GetAsyncKeyState(int vKey) {
    //TODO use SDL2?
    return 0;
}

void SetFocus(HWND hwnd) {
    SDL_RaiseWindow(fromHWND(hwnd));
}

void ShowCursor(bool show) {
    SDL_ShowCursor(show);
}

void SetCursor(HCURSOR cursor) {
    SDL_SetCursor(cursor);
}

HANDLE LoadImage(void*, const char* name, UINT type, int width, int height, UINT) {
    SDL_Surface* surface = SDL_LoadBMP(name);
    
    if (!surface) {
        SDL_PRINT_ERROR("LoadImage SDL_LoadBMP");
    } else {
        if (type == IMAGE_CURSOR) {
            SDL_Cursor* cursor = SDL_CreateColorCursor(
                    surface, 0, 0
            );
            if (cursor) {
                return reinterpret_cast<HANDLE>(cursor);
            } else {
                SDL_PRINT_ERROR("LoadImage SDL_CreateColorCursor");
            }
        }
    }
    
    return nullptr;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

DWORD GetPrivateProfileString(const char* section,const char* key,const char* defaultVal,
                              const char* returnBuffer, DWORD bufferSize, const char* filePath) {
    //TODO
}

DWORD WritePrivateProfileString(const char* section,const char* key,const char* value, const char* filePath) {
    //TODO
}

bool GetComputerName(char* out, DWORD* size) {
    if (gethostname(out, *size) == 0) {
        *size = SDL_strlen(out) + 1;
        return true;
    }
    *out = 0;
    *size = 0;
    return false;
}

bool GetUserName(char* out, DWORD* size) {
    struct passwd *pwd = getpwuid(getuid());
    if (pwd) {
        size_t maxsize = *size;
        SDL_strlcpy(out, pwd->pw_name, maxsize);
        *size = std::min(maxsize, SDL_strlen(out) + 1);
        return true;
    }
    
    *out = 0;
    *size = 0;
    return false;
}

void ZeroMemory(void *p, size_t n) {
    memset(p, 0, n);
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

int _mkdir(const char* path) {
    mkdir(path, 0);
}

char* _fullpath(char* absolutePath, const char* relativePath, size_t maxLength) {
    bool malloced = absolutePath == nullptr;
    if (malloced) {
        absolutePath = static_cast<char*>(malloc(maxLength));
    }
    if (realpath(relativePath, absolutePath) != nullptr) {
        return absolutePath;
    }
    if (malloced) {
        free(absolutePath);
    }

}

void _splitpath(const char* path, char*, char* dir, char* fname, char* ext) {
    const char* fullpath = realpath(path, nullptr);
    std::string fullpath_str = std::string() + fullpath;
    
    //Get dir and file
    std::size_t lastpath_limiter = fullpath_str.find_last_of('/');
    std::string dir_str = fullpath_str.substr(0, lastpath_limiter);
    SDL_strlcpy(dir, dir_str.c_str(), _MAX_DIR);
    std::string file_str = fullpath_str.substr(lastpath_limiter, fullpath_str.length());
    
    //Get file name and ext
    std::size_t lastext_limiter = file_str.find_last_of('.');
    std::string filename_str = file_str.substr(0, lastext_limiter);
    SDL_strlcpy(fname, filename_str.c_str(), _MAX_FNAME);
    std::string ext_str = file_str.substr(lastext_limiter, file_str.length());
    SDL_strlcpy(ext, ext_str.c_str(), _MAX_EXT);
    
    free((void*) fullpath);
}

void _makepath(char* path, const char* drive, const char* dir, const char* fname, const char* ext) {
    std::string fullpath = std::string() + dir + fname + ext;
    SDL_strlcpy(path, fullpath.c_str(), MAX_PATH);
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
