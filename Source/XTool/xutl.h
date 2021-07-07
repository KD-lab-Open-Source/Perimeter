
#ifndef __XUTL_H
#define __XUTL_H

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
        __argv.push_back(argv[i]);
        __argc++;
    }
#endif
}

inline const char* check_command_line(const char* switch_str)
{
    for(int i = 1; i < __argc; i ++){
        const char* s = strstr(__argv[i], switch_str);
        if(s)
            return s += strlen(switch_str);
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

//Obtains pairs of lowercase and original path from Resource paths cache which match the path start
std::vector<std::pair<std::string, std::string>> get_resource_paths(const std::string& path);

//Do a conversion for RESOURCE paths
std::string convert_path_resource(const char* path, bool parent_only = false);

//Adds char to string in end if not present
void terminate_with_char(std::string& buffer, const char chr);

//Replicate legacy behavior and add dot if not present
void terminate_float_with_dot(char* buffer, size_t max);

//Scans dir and creates resource paths cache, it can update only a certain subdirectory to avoid rescanning all files
void scan_resource_paths(std::string scan_path = "");

static bool startsWith(const std::string& str, const std::string& prefix)
{
    return str.size() >= prefix.size() && 0 == str.compare(0, prefix.size(), prefix);
}

static bool endsWith(const std::string& str, const std::string& suffix)
{
    return str.size() >= suffix.size() && 0 == str.compare(str.size()-suffix.size(), suffix.size(), suffix);
}


#endif
