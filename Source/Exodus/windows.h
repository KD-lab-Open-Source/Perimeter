#ifndef PERIMETER_WINDOWS_H
#define PERIMETER_WINDOWS_H

#define WINAPI

struct _FILETIME {
    unsigned short dwLowDateTime;
    unsigned short dwHighDateTime;
};

//TODO this is some Windows window handler pointer, it should be abstracted with SDL2 like in Vangers
typedef void *HWND;

//TODO replace the usages in codebase
#include <cstdint>
typedef uint8_t BYTE;
typedef uint16_t WORD;
typedef uint32_t DWORD;
typedef uint32_t UINT;
typedef const char* LPCSTR;

//TODO this comes from Win32
#define MAX_PATH 260

//TODO use SDL2 or something?
void GetCurrentDirectory(unsigned short size, char* path);

#endif //PERIMETER_WINDOWS_H
