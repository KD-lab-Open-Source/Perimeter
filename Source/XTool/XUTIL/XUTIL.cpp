
#include <fstream>
#include <cstring>
#include <codecvt>
#include <locale>
#include <algorithm>
#include <SDL.h>
#include <cwchar>
#include <cwctype>
#include "tweaks.h"
#ifdef _WIN32 
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#include <windows.h>
#endif
#include "xutil.h"
#include "xmath.h"

std::wstring_convert<std::codecvt_utf8<char16_t>, char16_t> utf8cvt;

bool argcv_setup_done = false;
int app_argc = 0;
std::vector<std::string> app_argv;

static unsigned int XRndValue = 83838383;

unsigned int XRnd(unsigned int m)
{ 
	XRndValue = XRndValue*214013L + 2531011L;
	if(!m)
		return 0;
	return ((XRndValue>> 16) & 0x7fff) % m; 
}

void XRndSet(unsigned int m)
{
	XRndValue = m;
}

unsigned int XRndGet()
{
	return XRndValue;
}

void setup_argcv(int argc, char *argv[]) {
    //Pick the args
    for(int i = 0; i < argc; i ++){
#ifdef DEBUG_ARGV
        printf("setup_argcv args %d %s\n", i, argv[i]);
#endif
        app_argv.emplace_back(argv[i]);
        app_argc++;
    }
    argcv_setup_done = true;
}

void decode_version(const char* version_str, uint16_t version[3]) {
    if (version_str == nullptr) return;
    XBuffer buf(const_cast<char*>(version_str), strlen(version_str) + 1);
    version[0] = 0;
    version[1] = 0;
    version[2] = 0;
    char c;
    if (*buf.buf == 'v') {
        //Old versions had v before numbers, skip it
        buf > c;
    }
    buf >= version[0];
    buf > c;
    if (c != '.') ErrH.Abort("Can't parse version", XERR_CRITICAL, 1, version_str);
    buf >= version[1];
    buf > c;
    if (c != '.') {
        //Old x.YY format, transform to X.0.Y
        version[2] = version[1];
        version[1] = 0;
    } else {
        buf >= version[2];
    }
}

int compare_versions(const uint16_t left[3], const uint16_t right[3]) {
    for (int i = 0; i < 3; ++i) {
        if (left[i] < right[i]) return i + 1;
        else if (left[i] > right[i]) return -(i + 1);
    }
    return 0;
}

int compare_versions(const uint16_t left[3], const char* right) {
    static uint16_t version[3];
    decode_version(right, version);
    return compare_versions(left, version);
}

const char* check_command_line(const char* switch_str) {
    if (!argcv_setup_done) {
        fprintf(stderr, "Called check_command_line %s before setup_argcv\n", switch_str);
        xassert(0);
    }
    std::string switch_key(switch_str);
    switch_key += "=";
    for(int i = 1; i < app_argc; i ++){
        const char* arg = app_argv[i].c_str();
        if (startsWith(arg, "tmp_")) {
            arg += 4;
        }
        if (startsWith(arg, switch_key)) {
            arg += switch_key.length();
            return arg;
        }
    }
    return nullptr;
}

//TODO reconfigure this based on current locale using yes/no in qdTextDB and use this as default values
static SDL_MessageBoxButtonData messageBoxQuestionButtons[] = {
        /* .flags, .buttonid, .text */
        { SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT, 1, "YES / ДА" },
        { SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT, 2, "NO / НЕТ"  },
};

bool MessageBoxQuestion(const char* title, const char* message, uint32_t flags) {
    if (flags == 0) {
        flags = SDL_MESSAGEBOX_INFORMATION;
    }

    const SDL_MessageBoxData data = {
            flags, /* .flags */
            nullptr, /* .window */
            title, /* .title */
            message, /* .message */
            SDL_arraysize(messageBoxQuestionButtons), /* .numbuttons */
            messageBoxQuestionButtons, /* .buttons */
            nullptr /* .colorScheme */
    };

    int choice = 0;
    int err = SDL_ShowMessageBox(&data, &choice);
    if (err) {
        fprintf(stdout, "MessageBoxQuestion: %s - %s\n", title, message);
        SDL_PRINT_ERROR("Creating MessageBoxQuestion window");
    }
    return choice == 1;
}

int MessageBoxChoice(const char* title, const char* message, const std::vector<std::string>& buttons, uint32_t flags) {
    if (flags == 0) {
        flags = SDL_MESSAGEBOX_INFORMATION;
    }
    
    std::vector<SDL_MessageBoxButtonData> buttonData;
    int i = 1;
    for (auto& button : buttons) {
        buttonData.push_back({ 0, i, button.c_str() });
        i++;
    }

    const SDL_MessageBoxData data = {
            flags, /* .flags */
            nullptr, /* .window */
            title, /* .title */
            message, /* .message */
            static_cast<int>(buttonData.size()), /* .numbuttons */
            buttonData.data(), /* .buttons */
            nullptr /* .colorScheme */
    };

    int choice = 0;
    int err = SDL_ShowMessageBox(&data, &choice);
    if (err) {
        fprintf(stdout, "MessageBoxChoice: %s\n", title);
        SDL_PRINT_ERROR("Creating MessageBoxQuestion window");
    }
    
    return choice;
}

std::u16string utf8_to_utf16(const char* str) {
    std::u16string conv;
    try {
        conv = utf8cvt.from_bytes(str);
    } catch (std::exception& e) {
        fprintf(stderr, "utf8_to_utf16 exception %s with input '%s'\n", e.what(), str);
    }
    return conv;
}

std::string utf16_to_utf8(const std::u16string& str) {
    std::string conv;
    try {
        conv = utf8cvt.to_bytes(str);
    } catch (std::exception& e) {
        fprintf(stderr, "utf16_to_utf8 exception %s\n", e.what());
    }
    return conv;
}

void terminate_with_char(std::string& buffer, const char chr) {
    //Check if already has
    if (!buffer.empty() && buffer[buffer.length() - 1] != chr) {
        //Append to end and store
        buffer += chr;
    }
}

void terminate_float_text(char* buffer, size_t size_max, int digits) {
    int size = static_cast<int>(strlen(buffer));
    digits = 0 < digits ? digits : 0;

    //Check if already has one or we cant add it
    int pos_dot = -1;
    bool all_zeros = true;
    int zeros_amount = 0;
    if (0 < size) {
        for (int i = size - 1; 0 <= i; --i) {
            if (buffer[i] == 'e') {
                return;
            }
            if (buffer[i] == '.') {
                pos_dot = i;
                break;
            }
            if (all_zeros) {
                if (buffer[i] == '0') {
                    zeros_amount++;
                } else {
                    all_zeros = false;
                }
            }
        }
    }
    
    //Remove zeros that are exceeded
    int after_dot = size - 1 - pos_dot;
    if (0 < zeros_amount && digits < after_dot) {
        size = pos_dot + 1 + (std::max)(digits, after_dot - zeros_amount);
        buffer[size] = '\0';
    }

    //Append to end if missing
    if (pos_dot == -1) {
        strncat(buffer, ".", size_max);
        pos_dot = size;
        size++;
    }
    
    //Add zeros to fit digits if its positive
    after_dot = size - 1 - pos_dot;
    for (int i = 0; i < digits - after_dot; ++i) {
        strncat(buffer, "0", size_max);
    }
}

void string_replace_all(std::string& text, const std::string& from, const std::string& to)
{
    //SRC: https://stackoverflow.com/questions/2896600/how-to-replace-all-occurrences-of-a-character-in-string
    if (text.empty() || from.empty()) return;
    
    std::string newString;
    newString.reserve(text.length());  // avoids a few memory allocations

    std::string::size_type lastPos = 0;
    std::string::size_type findPos;

    while(std::string::npos != (findPos = text.find(from, lastPos)))
    {
        newString.append(text, lastPos, findPos - lastPos);
        newString += to;
        lastPos = findPos + from.length();
    }

    // Care for the rest after last occurrence
    newString += text.substr(lastPos);

    text.swap(newString);
}

std::string string_to_capitalize(const char* str) {
    std::u16string conv = utf8_to_utf16(str);
    if (conv.empty()) return str;

    //Process each UTF16 code
    for (int i = 0; i < conv.size(); ++i) {
        auto c = conv[i];
        if (i == 0) {
            c = std::towupper(c);
        } else {
            c = std::towlower(c);
        }
        conv[i] = c;
    }

    //Convert UTF-16 into UTF-8
    std::string output = utf16_to_utf8(conv);

    return output;
}

std::string string_to_lower(const char* str) {
    std::u16string conv = utf8_to_utf16(str);
    if (conv.empty()) return str;
    
    //Process each UTF16 code
    for (int i = 0; i < conv.size(); ++i) {
        auto c = conv[i];
        c = std::towlower(c);
        conv[i] = c;
    }

    //Convert UTF-16 into UTF-8
    std::string output = utf16_to_utf8(conv);
    
    return output;
}

//Converts string to float/double manually since we can't rely on stdlib for deterministic results
template<typename FT, typename IT, typename UIT>
FT string_to_floating(const char* in, char** end) {
    char* ptr = const_cast<char*>(in);
    if (end) {
        *end = ptr;
    }
    if (!in) return 0;
    FT value = 0; //Stores final value
    UIT integer = 0; //Stores integer part
    UIT fraction = 0; //Stores fraction part
    IT floating = -1; //Amount of numbers to shift the fraction part
    IT exponent = 0; //Amount of numbers to shift the value
    bool negative = false;
    bool raw = false;
    bool foundNumber = false;
    UIT tmp;
    char tmpstr[2] = { 0, 0 };
    
    while (*ptr != 0) {
        char c = *ptr;
        
        //Ignore whitespaces
        if (!foundNumber) {
            switch (c) {
                case ' ':
                case '+':
                case '\n':
                case '\r':
                case '\t':
                    ptr++;
                    continue;
                default:
                    foundNumber = true;
                    break;
            }
        }
        
        if ('0' <= c && c <= '9') {
            //Is a digit
            tmpstr[0] = c;
            tmp = strtoul(tmpstr, nullptr, 10);
            if (floating != -1) {
                floating++;
                fraction *= 10;
                fraction += tmp;
            } else {
                integer *= 10;
                integer += tmp;
            }
        } else if ('.' == c && floating == -1) {
            xassert(!raw);
            //Switch to floating mode
            floating = 0;
        } else if ('e' == c || 'E' == c) {
            xassert(!raw);
            //Advance e
            ptr++;
            //Get exponent and break
            exponent = strtol(ptr, &ptr, 10);
            break;
        } else if (c == '-' && !negative) {
            xassert(!raw);
            negative = true;
        } else if (c == 'X' && !raw) {
            xassert(!negative);
            xassert(floating == -1);
            xassert(exponent == 0);
            raw = true;
        } else {
            //Finished here
            break;
        }
        ptr++;
    }

    if (end) {
        *end = ptr;
    }
    
    if (raw) {
        //Write IT binary value into FT type
        UIT* value_raw = checked_reinterpret_cast_ptr<FT, UIT>(&value);
        *value_raw = integer;
    } else {
        //Assemble all parts
        if (floating != -1 && fraction != 0) {
            value = static_cast<FT>(fraction);
            value /= static_cast<FT>(xm::pow(10.0, static_cast<double>(floating)));
        }
        value += static_cast<FT>(integer);
        if (exponent != 0) {
            value *= static_cast<FT>(xm::pow(10.0, static_cast<double>(exponent)));
        }
        if (negative) {
            value = -value;
        }
    }
    
    return value;
}

double string_to_double(const char* in, char** end) {
    double value = static_cast<double>(string_to_floating<double, int64_t, uint64_t>(in, end));
#if defined(PERIMETER_DEBUG) && 0
    char* ptr = *end;
    size_t len = ptr - in;
        if (len == 0) {
            printf("empty? ");
            len = 5;
        }
        ptr = (char*) malloc(len);
        ptr = strncpy(ptr, in, len - 1);
        printf("string_to_float: '%s' -> %f\n", ptr, value);
    
        //Check against strtod
        double other = strtod(in, &ptr);
        xassert(xm::round(other * 100) == xm::round(value * 100));
        if (end) {
            xassert(ptr == *end);
        }
#endif
    return value;
}

float string_to_float(const char* in, char** end) {
    return static_cast<float>(string_to_floating<float, int32_t, uint32_t>(in, end));
}

void encode_raw_float(XBuffer* buffer, float value) {
    uint32_t* value_raw = checked_reinterpret_cast_ptr<float, uint32_t>(&value);
    (*buffer) < 'X' <= (*value_raw);
}

void encode_raw_double(XBuffer* buffer, double value) {
    uint64_t* value_raw = checked_reinterpret_cast_ptr<double, uint64_t>(&value);
    (*buffer) < 'X' <= (*value_raw);
}

std::string BreakLongLines(const char* ptext, size_t max_width, char endline) {
    std::string text;
    if (ptext) {
        size_t line_len = 0;
        while (true) {
            const char c = *ptext;
            if (c == '\0') {
                break;
            }

            ptext++;
            if (c == endline) {
                line_len = 0;
                text += endline;
            } else {
                text += c;
                line_len += 1;
                if (line_len >= max_width) {
                    line_len = 0;
                    text += endline;
                }
            }
        }
    }
    return text;
}
