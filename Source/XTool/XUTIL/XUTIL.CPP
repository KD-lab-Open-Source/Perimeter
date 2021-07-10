
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

#ifndef _WIN32
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

const std::string curdir = std::string(".") + PATH_SEP;

void prepare_path(std::string& res_path) {
    if (res_path == "." || res_path == curdir) {
        res_path.clear();
        return;
    }
    
    res_path = convert_path(res_path.c_str());
    size_t pos = res_path.find(curdir);
    if (pos != std::string::npos && pos == 0) {
        res_path.erase(0, curdir.size());
    }
}

std::string add_resource_path_internal(std::unordered_map<std::string, std::string>& paths, std::string res_path) {
    prepare_path(res_path);
    std::string path_lwr = res_path;
    strlwr(path_lwr.data());
    if (startsWith(path_lwr, "resource")
        || startsWith(path_lwr, "cache_font")
        || startsWith(path_lwr, "scripts")
        || endsWith(path_lwr, ".ini")) {
        paths[path_lwr] = res_path;
        if (is_directory(std::filesystem::path(res_path))) {
            terminate_with_char(path_lwr, PATH_SEP);
            paths[path_lwr] = res_path + PATH_SEP;
        }
    }
    return path_lwr;
}

//Since some code may call this statically, we use a flag to know if this is the first time
bool resourcePathsFirstScan = true;
std::unordered_map<std::string, std::string> resourcePaths;

void scan_resource_paths(std::string scan_path) {
    //Prepare scan path
    prepare_path(scan_path);
    
    //Create a temporary copy until all is scanned
    std::unordered_map<std::string, std::string> paths;
    if (!scan_path.empty()) {
        //Check if path is valid
        if (!std::filesystem::exists(scan_path)) {
            return;
        }
        
        //Copy all paths except those that match with path to scan
        for (const auto& entry : resourcePaths) {
            if (!startsWith(entry.first, scan_path)) {
                paths[entry.first] = entry.second;
            }
        }
        
        //Also add path itself since is not root
        add_resource_path_internal(paths, scan_path);
    }
    
    //Do recursive search on scan path if its a dir
    //Otherwise we are done already as the scan_path itself was added before
    if (scan_path.empty()) scan_path = curdir;
    if (is_directory(std::filesystem::path(scan_path))) {
        for (const auto& entry : std::filesystem::recursive_directory_iterator(scan_path)) {
            if (entry.is_regular_file() || entry.is_directory()) {
                add_resource_path_internal(paths, entry.path().string());
            }
        }
    }
    
    //Store collected paths
    resourcePaths = std::move(paths);
    resourcePathsFirstScan = false;
}

std::vector<std::pair<std::string, std::string>> get_resource_paths(const std::string& path) {
    std::vector<std::pair<std::string, std::string>> paths;
    for (const auto& entry : resourcePaths) {
        if (startsWith(entry.first, path)) {
            paths.emplace_back(entry);
        }
    }
    return paths;
}

std::string convert_path_resource(const char* path, bool parent_only) {
    if (resourcePathsFirstScan) {
        resourcePaths = std::unordered_map<std::string, std::string>();
        scan_resource_paths();
    }
    std::string conv = convert_path(path);
    if (startsWith(conv, "./")) {
        string_replace(conv, "./", "");
    }
    std::string result;
    if (parent_only) {
        //Search the parent and append filename
        std::filesystem::path path_fs(conv);
        conv = path_fs.parent_path().string();
        strlwr(conv.data());
        result = resourcePaths[conv];
        if (!result.empty()) {
            result += PATH_SEP + path_fs.filename().string();
        }
    } else {
        strlwr(conv.data());
        result = resourcePaths[conv];
    }
    return result;
}

void terminate_with_char(std::string& buffer, const char chr) {
    //Check if already has
    if (!buffer.empty() && buffer[buffer.length() - 1] != chr) {
        //Append to end and store
        buffer += chr;
    }
}

void terminate_float_with_dot(char* buffer, size_t max) {
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
