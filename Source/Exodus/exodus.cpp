#include "exodus.h"
#include "tweaks.h"
#include <cstring>
#include <cctype>
#include <thread>
#include <SDL.h>
#include <string>
#include <filesystem>
#include <SimpleIni.h>
#include "files/files.h"
#include "xutl.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

uint32_t GetPrivateProfileString(const char* section,const char* key,const char* defaultVal,
                                 char* returnBuffer, uint32_t bufferSize, const char* filePath) {
    CSimpleIniA ini;
    SI_Error rc = ini.LoadFile(filePath);
    if (rc < 0) {
        fprintf(stderr, "Error reading %s file: %d\n", filePath, rc);
        return 0;
    };
    const char* val = ini.GetValue(section, key, defaultVal);
    if (val) {
        SDL_strlcpy(returnBuffer, val, bufferSize);
    } else {
        *returnBuffer = 0;
    }
    return 1;
}

uint32_t WritePrivateProfileString(const char* section,const char* key,const char* value, const char* filePath) {
    CSimpleIniA ini;
    SI_Error rc = ini.LoadFile(filePath);
    if (rc < 0) {
        fprintf(stderr, "Error loading %s file for writing: %d\n", filePath, rc);
        return 0;
    };
    rc = ini.SetValue(section, key, value);
    if (rc < 0) {
        fprintf(stderr, "Error writing %s %s %s in file %s: %d\n", section, key, value, filePath, rc);
        return 0;
    };
    ini.SaveFile(filePath);
    if (rc < 0) {
        fprintf(stderr, "Error writing %s file: %d\n", filePath, rc);
        return 0;
    };
    return 1;
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

//According to MSDN: Both __iscsym and __iswcsym return a nonzero value if c is a letter, underscore, or digit. 
int __iscsym(int c) {
    if (c == '_') return 1;
    return isalnum(c);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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

uint32_t WaitForSingleObject(HANDLE event, uint64_t milliseconds) {
    return neosmart::WaitForEvent(reinterpret_cast<neosmart::neosmart_event_t>(event), milliseconds);
}

uint32_t WaitForMultipleObjects(int count, HANDLE* events, bool waitAll, uint64_t milliseconds) {
    return neosmart::WaitForMultipleEvents(reinterpret_cast<neosmart::neosmart_event_t*>(events), count, waitAll, milliseconds);
}
