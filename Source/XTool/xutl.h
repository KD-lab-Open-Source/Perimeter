
#ifndef __XUTL_H
#define __XUTL_H

#define GET_PREF_PATH() SDL_GetPrefPath("KD Vision", "Perimeter")
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

///Stores argc/argv from main()
void setup_argcv(int argc, char *argv[]);

///Returns the value by the switch name from key=value at argv
const char* check_command_line(const char* switch_str);

bool MessageBoxQuestion(const char* title, const char* message, uint32_t flags = 0);

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
