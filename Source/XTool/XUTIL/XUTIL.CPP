
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

static const std::string curdir_path = std::string(".") + PATH_SEP;

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

void prepare_path(std::string& res_path, const std::string& source_path) {
    res_path = convert_path(res_path.c_str());
    if (res_path == "." || res_path == source_path) {
        res_path.clear();
    }
    if (res_path.empty()) {
        return;
    }
    
    if (!source_path.empty()) {
        size_t root_pos = res_path.find(source_path);
        if (root_pos != std::string::npos && root_pos == 0) {
            res_path.erase(0, source_path.size());
        }
    }
    
    //Remove ./ since it screws with setExtention
    size_t path_pos = res_path.find(curdir_path);
    if (path_pos != std::string::npos && path_pos == 0) {
        res_path.erase(0, curdir_path.size());
    }
}

///Adds a new internal path relative to root path
std::string add_resource_path_internal(std::unordered_map<std::string, std::string>& paths,
                                       std::string res_path,
                                       const std::string& destination_path, 
                                       const std::string& source_path,
                                       bool replace_files, bool replace_dirs) {
    
    //Use destination path directly if not empty
    std::string path_lwr = destination_path;
    if (path_lwr.empty() || endsWith(path_lwr, PATH_SEP_STR)) {
        //Make relative path from res_path
        path_lwr = res_path;
        prepare_path(path_lwr, source_path);
        path_lwr = destination_path + path_lwr;
    }
    
    //Lowercase it for case-insensitive path matching
    strlwr(path_lwr.data());
    if (startsWith(path_lwr, "resource")
        || startsWith(path_lwr, "cache_font")
        || startsWith(path_lwr, "addons")
        || startsWith(path_lwr, "scripts")
        || endsWith(path_lwr, ".ini")) {
        //Remove ./ from res path since it can mess with some code dealing with extensions
        prepare_path(res_path, "");

        bool path_is_directory = std::filesystem::is_directory(std::filesystem::path(res_path));

        //Create absolute path too
        std::string res_path_lwr = convert_path(res_path.c_str());
        strlwr(res_path_lwr.data());
        
        //Check if an override occurs
        std::string prev_path = paths[path_lwr];
        std::string prev_path_abs;
        if (!prev_path.empty() && prev_path != res_path) {
            if ((path_is_directory && !replace_dirs) || (!path_is_directory && !replace_files)) {
                //printf("Not replacing path %s with %s\n", prev_path.c_str(), res_path.c_str());
                return "";
            }
            
            //printf("Replacing path %s with %s\n", prev_path.c_str(), res_path.c_str());
            
            //TODO override old absolute paths with the new path until proper struct is implemented to group paths
            std::filesystem::path prev_path_path(prev_path);
            if (prev_path_path.is_absolute()) {
                prev_path_abs = convert_path_resource(path_lwr.c_str());
                strlwr(prev_path_abs.data());
            }
        }
        
        //Store path relative and absolute internal paths pointing the real FS path
        paths[res_path_lwr] = paths[path_lwr] = res_path;
        if (!prev_path_abs.empty()) {
            paths[prev_path_abs] = res_path;
        }
        if (path_is_directory) {
            //In case of path pointing a dir, store PATH_SEP terminated too
            terminate_with_char(path_lwr, PATH_SEP);
            terminate_with_char(res_path_lwr, PATH_SEP);
            paths[res_path_lwr] = paths[path_lwr] = res_path + PATH_SEP;
            if (!prev_path_abs.empty()) {
                terminate_with_char(prev_path_abs, PATH_SEP);
                paths[prev_path_abs] = res_path;
            }
        }
    }
    return path_lwr;
}

//Since some code may call this statically, we use a flag to know if this is the first time
bool resourcePathsFirstScan = true;
std::unordered_map<std::string, std::string> resourcePaths;
std::string content_root_path;

bool scan_resource_paths(std::string destination_path, std::string source_path) {
    //Use destination as source path assuming its a call to refresh subdir resources in root
    if (source_path.empty() && !destination_path.empty()) {
        source_path = destination_path;
    }
    bool same_paths = source_path == destination_path;
    bool replace_files = !same_paths;
    
    //Remove root or any other stuff from source before adding
    prepare_path(source_path, content_root_path);
    if (!content_root_path.empty() && content_root_path != curdir_path) {
        source_path = content_root_path + source_path;
    }
    
    //Prepare destination too
    prepare_path(destination_path, content_root_path);
    strlwr(destination_path.data());

    //If is actually a file then just add it
    if (std::filesystem::is_regular_file(std::filesystem::path(source_path))) {
        std::string source_parent_path = std::filesystem::path(source_path).parent_path().string();
        terminate_with_char(source_parent_path, PATH_SEP);
        add_resource_path_internal(resourcePaths,
                                   source_path,
                                   destination_path,
                                   source_parent_path,
                                   true, false);
        resourcePathsFirstScan = false;
        return true;
    }

    //We need this to scan dirs in current dir
    if (content_root_path == curdir_path) {
        source_path = curdir_path + source_path;
    }
    
    //Check if is actually a dir
    if (!std::filesystem::is_directory(source_path)) {
        return false;
    }
    terminate_with_char(source_path, PATH_SEP);
    terminate_with_char(destination_path, PATH_SEP);

    std::unordered_map<std::string, std::string> paths;
    if (replace_files) {
        paths = resourcePaths;
    } else {
        //Remove all paths that match with path to scan by not including them
        for (const auto& entry: resourcePaths) {
            if (!startsWith(entry.second, source_path)) {
                paths.insert(entry);
            }
        }
    }

    //Add path as resource since iterator wont add it
    if (!destination_path.empty() && same_paths) {
        add_resource_path_internal(paths, source_path, destination_path, source_path, replace_files, false);
    }
    
    //Do recursive search on source path
    for (const auto& entry : std::filesystem::recursive_directory_iterator(std::filesystem::path(source_path))) {
        if (entry.is_regular_file() || entry.is_directory()) {
            add_resource_path_internal(paths, entry.path().string(), destination_path, source_path, replace_files, false);
        }
    }
    
    //Store collected paths
    resourcePaths = paths;
    resourcePathsFirstScan = false;
    return true;
}

bool scan_resource_paths(const std::string& path) {
    return scan_resource_paths(path, path);
}

std::vector<std::pair<std::string, std::string>> get_resource_paths_recursive(std::string path) {
    prepare_path(path, content_root_path);
    strlwr(path.data());
    std::vector<std::pair<std::string, std::string>> paths;
    for (const auto& entry : resourcePaths) {
        if (startsWith(entry.first, path)) {
            paths.emplace_back(entry);
        }
    }
    return paths;
}

std::vector<std::pair<std::string, std::string>> get_resource_paths_directory(std::string path) {
    prepare_path(path, content_root_path);
    strlwr(path.data());
    terminate_with_char(path, PATH_SEP);
    std::vector<std::pair<std::string, std::string>> paths;
    for (const auto& entry : resourcePaths) {
        if (startsWith(entry.first, path)) {
            //Only add paths that don't have any separator, we use posix sep because internals are always posix
            std::string entry_path = entry.first;
            prepare_path(entry_path, path);
            if (entry_path.empty()) continue;
            
            size_t pos = entry_path.find(PATH_SEP);
            if (pos != std::string::npos) continue;
            
            paths.emplace_back(entry);
        }
    }
    return paths;
}

void clear_resource_paths() {
    resourcePathsFirstScan = true;
    resourcePaths.clear();
}

void set_content_root_path(const std::string& path) {
    content_root_path = path;
    terminate_with_char(content_root_path, PATH_SEP);
}

const std::string& get_content_root_path() {
    return content_root_path;
}

std::string convert_path_resource(const char* path, bool parent_only) {
    if (resourcePathsFirstScan) {
        //Avoid potential crash if we are called during static init
        fprintf(stderr, "Called convert_path_resource before scan_resource_paths! %s\n", path);
        return "";
    }
    std::string conv = convert_path(path);
    if (startsWith(conv, curdir_path)) {
        conv.erase(0, 2);
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
