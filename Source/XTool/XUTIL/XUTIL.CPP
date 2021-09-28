
#include <fstream>
#include <cstring>
#include <SDL.h>
#include <algorithm>
#include <unordered_map>
#include "tweaks.h"
#ifdef _WIN32 
#include <windows.h>
#endif
#include "xutil.h"

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

void EpochToFileTime(int64_t epoch, _FILETIME* pft) {
    int64_t ll = (epoch * 10000000LL) + 116444736000000000LL;
    pft->dwLowDateTime = (unsigned short) ll;
    pft->dwHighDateTime = (unsigned short) (ll >> 32);
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
