#include "exodus.h"
#include "tweaks.h"
#include <cstring>
#include <cctype>
#include <thread>
#include <SDL.h>
#include <pwd.h>
#include <string>
#include <filesystem>
#include <fcntl.h>
#include <SimpleIni.h>
#include "xutl.h"

//Usual open but with path conversion
int _open(const char* path, int oflags, int sflags) {
    //File may not exist, so we need to convert only parent path
    bool parent_only = oflags & _O_CREAT;
    std::string path_open = convert_path_resource(path, parent_only);
    return open(path_open.c_str(), oflags, sflags);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

unsigned int _controlfp(unsigned int newval, unsigned int mask) {
    return 0;
}

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

bool GetComputerName(char* out, uint32_t* size) {
    if (gethostname(out, *size) == 0) {
        *size = SDL_strlen(out) + 1;
        return true;
    }
    *out = 0;
    *size = 0;
    return false;
}

bool GetUserName(char* out, uint32_t* size) {
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

//According to MSDN: Both __iscsym and __iswcsym return a nonzero value if c is a letter, underscore, or digit. 
int __iscsym(int c) {
    if (c == '_') return 1;
    return isalnum(c);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

char* _fullpath(char* absolutePath, const char* relativePath, size_t maxLength) {
    if (relativePath) {
        bool malloced = absolutePath == nullptr;
        if (malloced) {
            absolutePath = static_cast<char*>(malloc(maxLength));
        }
        if (realpath(convert_path(relativePath).c_str(), absolutePath) != nullptr) {
            return absolutePath;
        }
        if (malloced) {
            free(absolutePath);
        }
    }
    return nullptr;
}

void _splitpath(const char* path, char* drive, char* dir, char* fname, char* ext) {
    std::filesystem::path path_input = convert_path(path);
    
    //Not used, but just in case
    if (drive) {
        *drive = 0;
    }
    
    //Get dir, file and extension from path
    std::string dir_str = path_input.parent_path().string();
    if (dir) {
        terminate_with_char(dir_str, PATH_SEP);
        SDL_strlcpy(dir, dir_str.c_str(), _MAX_DIR);
    }
    std::string ext_str = path_input.extension().string();
    if (ext) {
        SDL_strlcpy(ext, ext_str.c_str(), _MAX_EXT);
    }
    //Remove extension from filename and store
    if (fname) {
        std::string filename_str = path_input.filename().string();
        string_replace_all(filename_str, ext_str, "");
        SDL_strlcpy(fname, filename_str.c_str(), _MAX_FNAME);
    }
}

void _makepath(char* path, const char*, const char* dir, const char* fname, const char* ext) {
    std::filesystem::path fullpath = std::string(dir);
    fullpath.append(std::string(fname) + ext);
    SDL_strlcpy(path, fullpath.string().c_str(), MAX_PATH);
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
