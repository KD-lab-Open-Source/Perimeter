#ifndef PERIMETER_WINDOWS_H
#define PERIMETER_WINDOWS_H

struct _FILETIME {
    unsigned short dwLowDateTime;
    unsigned short dwHighDateTime;
};

//this comes from Win32
#define MAX_PATH 260

//TODO use SDL2 or something?
void GetCurrentDirectory(unsigned short size, char* path);

#endif //PERIMETER_WINDOWS_H
