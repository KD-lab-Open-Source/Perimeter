
#ifndef __XUTL_H
#define __XUTL_H

#include <cstdint>

#define GET_PREF_PATH() SDL_GetPrefPath("K-D LAB", "Perimeter")
#define PRINTF_FLOATING_FORMAT "%.*f"

struct XBuffer;

unsigned int XRnd(unsigned int m);
void XRndSet(unsigned int m);
unsigned int XRndGet();

///Get high performance counter
uint64_t getPerformanceCounter();

///How much ticks are in one second in high performance counter
uint64_t getPerformanceFrequency();

///Initializes clock
void initclock();

///Current time in integer millisecs since start
int clocki();

///Current time in fractional millisecs since start
double clockf();

///Current time in integer microsecs since start
uint64_t clock_us();

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <vector>
#include <filesystem>

//Hacky way to "store" argc/argv so they can be accessed later like in Windows
extern int app_argc;
extern std::vector<std::string> app_argv;

///Stores argc/argv from main()
void setup_argcv(int argc, char *argv[]);

///Decodes string version X.Y.Z into [X, Y, Z] int array
void decode_version(const char* version_str, uint16_t version[3]);

///Compares 2 int arrays and shows positive 1 starting index if right is higher, neg if left is higher
/// left 3,1,2 right 3,2,1 will return 2
/// left 3,0,2 right 3,0,0 will return -3 
/// left 3,0,2 right 3,0,2 will return 0
int compare_versions(const uint16_t left[3], const uint16_t right[3]);

///Compares int array and string, same as compare_versions above
int compare_versions(const uint16_t left[3], const char* right);

///Returns the value by the switch name from key=value at argv
const char* check_command_line(const char* switch_str);

bool MessageBoxQuestion(const char* title, const char* message, uint32_t flags = 0);

int MessageBoxChoice(const char* title, const char* message, const std::vector<std::string>& buttons, uint32_t flags = 0);

//Convert UTF-8 into UTF-16
std::u16string utf8_to_utf16(const char* str);

//Convert UTF-16 into UTF-8
std::string utf16_to_utf8(const std::u16string& str);

//Adds char to string in end if not present
void terminate_with_char(std::string& buffer, char chr);

//Replicate legacy behavior and add dot if not present
void terminate_float_text(char* buffer, size_t max, int digits);

//Replaces all "from" in "text" with "to"
void string_replace_all(std::string& text, const std::string& from, const std::string& to);

//Changes string to Capitalized
std::string string_to_capitalize(const char* str);

//Changes string to lowercase
std::string string_to_lower(const char* str);

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

//Converts string to double, same as strtod
double string_to_double(const char* in, char** end);

//Converts string to float, same as strtof
float string_to_float(const char* in, char** end);

//Encodes a float using "raw format" in text form
void encode_raw_float(XBuffer* buffer, float value);

//Encodes a double using "raw format" in text form
void encode_raw_double(XBuffer* buffer, double value);

//Breaks lines that exceed max_width by adding endline
std::string BreakLongLines(const char* ptext, size_t max_width, char endline = '\n');

#define UTF8_TO_WCHAR(VAR, VAL) \
    std::u16string u16string_##VAR = utf8_to_utf16(VAL); \
    const wchar_t* wchar_##VAR = checked_reinterpret_cast_ptr<const char16_t, const wchar_t>( u16string_##VAR.c_str());

#endif
