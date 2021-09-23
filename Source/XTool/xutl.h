
#ifndef __XUTL_H
#define __XUTL_H

#define GET_PREF_PATH() SDL_GetPrefPath("KranX Productions", "Perimeter")
#define PRINTF_FLOATING_FORMAT "%.*f"
#define PRINTF_LONG_FLOATING_FORMAT "%.*Lf"

unsigned int XRnd(unsigned int m);
void XRndSet(unsigned int m);
unsigned int XRndGet();

/*
__forceinline int BitSR(int x)
{
	int return_var;
	_asm {
		mov eax, x
		cdq
		xor eax,edx
		sub     eax,edx
		bsr     eax,eax
		mov [return_var],eax
	}
	return return_var;
}
*/

// Copied C version from Vangers
// TODO(amdmi3): very inefficient; rewrite
inline int BitSR(int x) {
    unsigned int a = abs(x);
    for (int i = 31; i > 0; i--)
        if (a & (1 << i))
            return i;

    return 0;
}

void initclock();
int clocki();
double clockf();

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <vector>
#include <filesystem>

#ifndef _WIN32
//Hacky way to "store" argc/argv so they can be accessed later like in Windows
extern int __argc;
extern std::vector<const char*> __argv;
#endif

static void setup_argcv(int argc, char *argv[]) {
#ifndef _WIN32
    for(int i = 0; i < argc; i ++){
        //printf("%d %s\n", i, argv[i]);
        __argv.push_back(argv[i]);
        __argc++;
    }
#endif
}

inline const char* check_command_line(const char* switch_str)
{
    for(int i = 1; i < __argc; i ++){
        const char* s = strstr(__argv[i], switch_str);
        if(s) {
            s += strlen(switch_str);
            if (*s == '=') s += 1;
            return s;
        }
    }
    return nullptr;
}

//https://docs.microsoft.com/en-gb/windows/win32/sysinfo/converting-a-time-t-value-to-a-file-time
struct _FILETIME;
void EpochToFileTime(int64_t epoch, _FILETIME* pft);

bool MessageBoxQuestion(const char* title, const char* message, uint32_t flags = 0);

//Replace string "search" with "result"
void string_replace(std::string& input, const char* find, const char* paste);

//Converts Windows/POSIX to native path
std::string convert_path(const char* path);

//Do a conversion for Windows -> POSIX paths
std::string convert_path_posix(const char* path);

//Do a conversion for RESOURCE paths
std::string convert_path_resource(const char* path, bool parent_only = false);

//Obtain pairs of lowercase and original path from Resource paths cache which match the path start
std::vector<std::pair<std::string, std::string>> get_resource_paths_recursive(std::string path);

//Obtain pairs of lowercase and original path from Resource paths cache which match the path start,
//only if
std::vector<std::pair<std::string, std::string>> get_resource_paths_directory(std::string path);

//Clears the current loaded resource paths
void clear_resource_paths();

//Sets the current resources root path
void set_content_root_path(const std::string& path);
 
//Returns the current resources root path
const std::string& get_content_root_path();

//Scans source dir and creates resource paths cache, it can update only a certain subdirectory to avoid rescanning all files
//Removes the source path in each scanned path before saving to internal resource path list to destination path
bool scan_resource_paths(std::string destination_path = "", std::string source_path = "");

//Adds char to string in end if not present
void terminate_with_char(std::string& buffer, char chr);

//Replicate legacy behavior and add dot if not present
void terminate_float_text(char* buffer, size_t max, int digits);

//Replaces all "from" in "text" with "to"
void string_replace_all(std::string& text, const std::string& from, const std::string& to);

static bool startsWith(const std::string& str, const std::string& prefix)
{
    //SRC: https://stackoverflow.com/questions/874134/find-out-if-string-ends-with-another-string-in-c
    return str.size() >= prefix.size() && 0 == str.compare(0, prefix.size(), prefix);
}

static bool endsWith(const std::string& str, const std::string& suffix)
{
    //SRC: https://stackoverflow.com/questions/874134/find-out-if-string-ends-with-another-string-in-c
    return str.size() >= suffix.size() && 0 == str.compare(str.size()-suffix.size(), suffix.size(), suffix);
}

#endif
