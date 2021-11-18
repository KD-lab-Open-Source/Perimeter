
#include <fstream>
#include <cstring>
#include <SDL.h>
#include <algorithm>
#include "tweaks.h"
#ifdef _WIN32 
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#include <windows.h>
#endif
#include "xutil.h"
#include "xmath.h"

#ifndef _WIN32
bool argcv_setup_done = false;
int __argc = 0;
std::vector<const char*> __argv;
#endif

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
#ifndef _WIN32
    for(int i = 0; i < argc; i ++){
        //printf("%d %s\n", i, argv[i]);
        __argv.push_back(argv[i]);
        __argc++;
    }
    argcv_setup_done = true;
#endif
}

const char* check_command_line(const char* switch_str) {
#ifndef _WIN32
    if (!argcv_setup_done) {
        fprintf(stderr, "Called check_command_line %s before setup_argcv\n", switch_str);
    }
#endif
    std::string switch_key(switch_str);
    switch_key += "=";
    for(int i = 1; i < __argc; i ++){
        const char* arg = __argv[i];
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

bool MessageBoxQuestion(const char* title, const char* message, uint32_t flags) {
    const SDL_MessageBoxButtonData buttons[] = {
            /* .flags, .buttonid, .text */
            { SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT, 1, "Yes" },
            { SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT, 2, "No"  },
    };

    if (flags == 0) {
        flags = SDL_MESSAGEBOX_INFORMATION;
    }

    const SDL_MessageBoxData data = {
            flags, /* .flags */
            nullptr, /* .window */
            title, /* .title */
            message, /* .message */
            SDL_arraysize(buttons), /* .numbuttons */
            buttons, /* .buttons */
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

//Converts string to double manually since we can't rely on stdlib for deterministic results
double string_to_double(const char* in, char** end) {
    char* ptr = const_cast<char*>(in);
    if (end) {
        *end = ptr;
    }
    if (!in) return 0;
    double value = 0; //Stores final value
    uint64_t integer = 0; //Stores integer part
    uint64_t fraction = 0; //Stores fraction part
    int64_t floating = -1; //Amount of numbers to shift the fraction part
    int64_t exponent = 0; //Amount of numbers to shift the value
    bool negative = false;
    bool foundNumber = false;
    uint64_t tmp;
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
            //Switch to floating mode
            floating = 0;
        } else if ('e' == c) {
            //Advance e
            ptr++;
            //Get exponent and break
            exponent = strtol(ptr, &ptr, 10);
            break;
        } else if (c == '-' && !negative) {
            negative = true;
        } else {
            //Finished here
            break;
        }
        ptr++;
    }
    
    //Assemble all parts
    if (floating != -1 && fraction != 0) {
        value = static_cast<double>(fraction);
        value /= static_cast<double>(xm::pow(10.0, static_cast<double>(floating)));
    }
    value += static_cast<double>(integer);
    if (exponent != 0) {
        value *= static_cast<double>(xm::pow(10.0, static_cast<double>(exponent)));
    }
    if (negative) {
        value = -value;
    }

    if (end) {
        *end = ptr;
    }

#if defined(PERIMETER_DEBUG) && 0
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
    return static_cast<float>(string_to_double(in, end));
}

