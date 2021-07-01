
#include <fstream>
#include <cstring>
#include <SDL.h>
#include <filesystem>
#include <algorithm>
#include <unordered_map>
#include "tweaks.h"
#ifdef _WIN32 
#include <windows.h>
#endif
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

void string_replace(std::string& input, const char* find, const char* paste)
{
    size_t pos = 0;
    while (true) {
        pos = input.find(find, pos);
        if(pos >= input.size())
            break;
        input.replace(pos, strlen(find), paste);
        pos += strlen(paste);
    }
}

std::string convert_path(const char* path) {
    std::string conv;
    size_t size = strlen(path);
    for (int i = 0; i < size; ++i) {
#ifdef _WIN32
        conv.push_back(path[i] == '/' ? PATH_SEP : path[i]);
#else
        conv.push_back(path[i] == '\\' ? PATH_SEP : path[i]);
#endif
    }
    return conv;
}

std::string convert_path_posix(const char* path) {
    std::string conv;
    size_t size = strlen(path);
    for (int i = 0; i < size; ++i) {
        conv.push_back(path[i] == '\\' ? '/' : path[i]);
    }
    return conv;
}

std::unordered_map<std::string, std::string> resourcePaths;

std::string get_dir_iterator_path(const std::filesystem::path& path) {
    std::string path_str = path.string();
    size_t pos = path_str.find("./");
    if (pos != std::string::npos && pos == 0) {
        path_str.erase(0, 2);
    }
    return path_str;
}

void scan_resource_paths() {
    resourcePaths.clear();
    for (const auto & entry : std::filesystem::recursive_directory_iterator(".")) {
        if (entry.is_regular_file() || entry.is_directory()) {
            std::string path = get_dir_iterator_path(entry.path());
            std::string path_lwr = path;
            strlwr(path_lwr.data());
            if (startsWith(path_lwr, "resource")
             || startsWith(path_lwr, "cache_font")
             || startsWith(path_lwr, "scripts")
             || endsWith(path_lwr, ".ini")) {
                resourcePaths[path_lwr] = path;
            }
        }
    }
}

std::vector<std::pair<std::string, std::string>> get_resource_paths(std::string path) {
    std::vector<std::pair<std::string, std::string>> paths;
    for (const auto& entry : resourcePaths) {
        if (startsWith(entry.first, path)) {
            paths.emplace_back(entry);
        }
    }
    return paths;
}

std::string convert_path_resource(const char* path) {
#ifdef PERIMETER_EXODUS
    xxassert(!resourcePaths.empty(), "Resource paths cache is empty");
    std::string conv = convert_path_posix(path);
    strlwr(conv.data());
    std::string result = resourcePaths[conv];
    return result;
#else
    return std::string(path);
#endif
}

void terminate_with_char(char* buffer, const char chr, size_t max) {
    size_t size = strlen(buffer);

    //Check if already has
    if (0 < size && buffer[size - 1] == chr) {
        return;
    }

    //Append to end and store
    std::string str = std::string() + chr;
    strncat(buffer, str.c_str(), max);
}

void terminate_with_dot(char* buffer, size_t max) {
    size_t size = strlen(buffer);

    //Check if already has one or we cant add it
    for (int i = 0; i < size; ++i) {
        if (buffer[i] == '.' || buffer[i] == 'e') {
            return;
        }
    }

    //Append to end and store
    strncat(buffer, ".", max);
}
