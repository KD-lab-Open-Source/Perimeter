#ifndef PERIMETER_WINDOWS_H
#define PERIMETER_WINDOWS_H

#define WINAPI

///////////////////////////////

#include <cstdint>

typedef uint8_t BYTE;
typedef uint8_t BOOLEAN;

typedef int16_t SHORT;
typedef uint16_t USHORT;
typedef uint16_t WORD;

typedef int32_t INT;
typedef int32_t LONG;

typedef uint32_t UINT;
typedef uint32_t ULONG;
typedef uint32_t DWORD;

typedef const char* LPCSTR;

/////////////////////////

struct _FILETIME {
    unsigned short dwLowDateTime;
    unsigned short dwHighDateTime;
};

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

//TODO this is some Windows window handler pointer, it should be abstracted with SDL2 like in Vangers
typedef void *HWND;


//TODO this comes from Win32
#define MAX_PATH 260

//TODO use SDL2 or something?
void GetCurrentDirectory(unsigned short size, char* path);

#endif //PERIMETER_WINDOWS_H
