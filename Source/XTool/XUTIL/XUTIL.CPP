
#include <fstream>
#include <cstring>
#include <SDL.h>
#include "tweaks.h"
#include "xutil.h"

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
    SDL_ShowMessageBox(&data, &choice);
    return choice == 1;
}

std::string convert_path(const char* path) {
    std::string conv;
    size_t size = strlen(path);
    for (int i = 0; i < size; ++i) {
        conv.push_back(path[i] == '\\' ? '/' : path[i]);
    }
    return conv;
}

void terminate_with_char(char* buffer, const char chr, size_t max, bool onlyEnd) {
    size_t size = strlen(buffer);

    //Check if already has
    if (onlyEnd) {
        if (0 < size && buffer[size - 1] == chr) {
            return;
        }
    } else {
        for (int i = 0; i < size; ++i) {
            if (buffer[i] == chr) {
                return;
            }
        }
    }

    //Append to end and store
    std::string str = std::string() + chr;
    strncat(buffer, str.c_str(), max);
}

void terminate_with_dot(char* buffer, size_t max) {
    terminate_with_char(buffer, '.', max, false);
}
