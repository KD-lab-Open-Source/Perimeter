#include "tweaks.h"

#ifndef PERIMETER_EXODUS
#define open _open
#endif

#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <filesystem>
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <cstring>

#include "xerrhand.h"
#include "xutl.h"
#include "xstream.h"

#include "files.h"

typedef std::unordered_map<std::string, std::shared_ptr<filesystem_entry>> filesystem_entries_map;

static const std::string curdir_path = std::string(".") + PATH_SEP;

bool content_debug_flag = false;

//Since some code may call this statically, we use a flag to know if this is the first time
bool first_scan_flag = true;

filesystem_entries_map filesystem_entries;

std::string content_root_path;

void filesystem_entry::set(filesystem_entry* entry) {
    if (entry == nullptr) return;
    this->key = entry->key;
    this->key_content = entry->key_content;
    this->path_content_relative = entry->path_content_relative;
    this->path_content = entry->path_content;
    this->is_directory = entry->is_directory;
    this->original_entry = entry->original_entry;
}

///Prepares path to remove source path 
void prepare_path(std::string& path, const std::string& source_path) {
    path = convert_path_native(path);
    if (path == "." || path == source_path) {
        path.clear();
    }
    if (path.empty()) {
        return;
    }

    if (!source_path.empty()) {
        size_t root_pos = path.find(source_path);
        if (root_pos != std::string::npos && root_pos == 0) {
            path.erase(0, source_path.size());
        }
    }

    //Remove ./ since it screws with setExtension
    size_t path_pos = path.find(curdir_path);
    if (path_pos != std::string::npos && path_pos == 0) {
        path.erase(0, curdir_path.size());
    }
}

void split_path_parent(const std::string& path, std::string& parent, std::string* filename) {
    std::filesystem::path path_fs(convert_path_native(path));
    parent = path_fs.parent_path().string();
    if (parent.empty()) {
        parent = content_root_path;
    }
    if (filename) {
        *filename = path_fs.filename().string();
    }
}

std::string convert_path_native(const std::string& path) {
    std::string conv;
    size_t size = path.size();
    for (int i = 0; i < size; ++i) {
#ifdef _WIN32
        conv.push_back(path[i] == '/' ? PATH_SEP : path[i]);
#else
        conv.push_back(path[i] == '\\' ? PATH_SEP : path[i]);
#endif
    }
    return conv;
}

std::string convert_path_posix(const std::string& path) {
    std::string conv;
    size_t size = path.size();
    for (int i = 0; i < size; ++i) {
        conv.push_back(path[i] == '\\' ? '/' : path[i]);
    }
    return conv;
}

std::shared_ptr<filesystem_entry> get_content_entry_internal(filesystem_entries_map& paths, const std::string& path) {
    if (paths.count(path)) {
        return paths.at(path);
    }
    return nullptr;
}

std::string convert_path_content(const std::string& path, bool parent_only) {
    if (first_scan_flag) {
        //Avoid potential crash if we are called during static init
        fprintf(stderr, "Called convert_path_content before scan_resource_paths! '%s'\n", path.c_str());
        if (content_debug_flag) {
            ErrH.Abort("content_debug: Called convert_path_content before scan_resource_paths!");
        }
        return "";
    }
    if (path.empty()) {
        return "";
    }
    std::string conv = convert_path_native(path);
    if (startsWith(conv, curdir_path)) {
        conv.erase(0, 2);
    }
    std::string result;
    if (parent_only) {
        //Search the parent and append filename
        std::filesystem::path path_fs(conv);
        conv = path_fs.parent_path().string();
        if (conv.empty()) {
            result = content_root_path + path_fs.filename().string();
        } else {
            strlwr(conv.data());
            filesystem_entry* entry = get_content_entry_internal(filesystem_entries, conv).get();
            if (entry) {
                result = entry->path_content + PATH_SEP + path_fs.filename().string();
            }
        }
    } else {
        strlwr(conv.data());
        filesystem_entry* entry = get_content_entry_internal(filesystem_entries, conv).get();
        if (entry) {
            result = entry->path_content;
        }
    }
    if (result.empty() && content_debug_flag) {
        fprintf(stderr, "convert_path_content failed for %s / %s\n", path.c_str(), conv.c_str());
    }
    return result;
}

filesystem_entry* get_content_entry(std::string path) {
    prepare_path(path, content_root_path);
    strlwr(path.data());
    return get_content_entry_internal(filesystem_entries, path).get();
}

std::vector<filesystem_entry*> get_content_entries_recursive(std::string path) {
    prepare_path(path, content_root_path);
    strlwr(path.data());
    std::vector<filesystem_entry*> paths;
    for (const auto& entry : filesystem_entries) {
        if (path.empty() || startsWith(entry.first, path)) {
            paths.emplace_back(entry.second.get());
        }
    }
    return paths;
}

std::vector<filesystem_entry*> get_content_entries_directory(std::string path) {
    prepare_path(path, content_root_path);
    strlwr(path.data());
    terminate_with_char(path, PATH_SEP);
    std::vector<filesystem_entry*> paths;
    for (const auto& entry : filesystem_entries) {
        if (path.empty() || startsWith(entry.first, path)) {
            //Only add paths that don't have any separator
            std::string entry_path = entry.first;
            prepare_path(entry_path, path);
            if (entry_path.empty()) continue;

            size_t pos = entry_path.find(PATH_SEP);
            if (pos != std::string::npos) continue;
            
            if (entry.second) {
                paths.emplace_back(entry.second.get());
            } else {
                xxassert(0, "Entry has null pointer");
            }
        }
    }
    return paths;
}

void dump_filesystem_entries(const std::string& path) {
    XStream xs(path, XS_OUT);
    for (const auto& pair : filesystem_entries) {
        filesystem_entry* entry = pair.second.get();
        xs <   "+-[Entry]-+";
        xs < "\n| Map  Key: " < pair.first.c_str();
        xs < "\n| Rel  Key: " < entry->key.c_str();
        xs < "\n| Abs  Key: " < entry->key_content.c_str();
        xs < "\n| Abs Path: " < entry->path_content.c_str();
        xs < "\n| Rel Path: " < entry->path_content_relative.c_str();
        filesystem_entry* original = entry->original_entry.get();
        while (original) {
            xs < "\n| Replaces: " < original->path_content.c_str();
            original = original->original_entry.get();
        }
        xs < "\n+---------+\n\n";
    }
    xs.close();
}

void clear_content_entries() {
    first_scan_flag = true;
    content_root_path.clear();
    filesystem_entries.clear();
}

void set_content_root_path(const std::string& path) {
    content_root_path = path;
    terminate_with_char(content_root_path, PATH_SEP);
}

const std::string& get_content_root_path() {
    return content_root_path;
}

///Adds a new filesystem entry
filesystem_entry* add_filesystem_entry_internal( // NOLINT(misc-no-recursion)
        filesystem_entries_map& paths,
        std::string path_content,
        const std::string& destination_path,
        const std::string& source_path,
        const filesystem_scan_options& options
) {
    //Remove ./ from res path since it can mess with some code dealing with extensions
    prepare_path(path_content, "");

    //Make relative path from path_content
    std::string path_source_relative = path_content;
    prepare_path(path_source_relative, source_path);

    //Use destination path directly if not empty
    std::string entry_key = destination_path;
    if (entry_key.empty() || options.prepend_destination_to_path) {
        entry_key = destination_path + path_source_relative;
    }

    //Lowercase it for case-insensitive path matching
    strlwr(entry_key.data());
    if (startsWith(entry_key, "resource")
        || startsWith(entry_key, "cache_font")
        || startsWith(entry_key, "mods")
        || startsWith(entry_key, "scripts")
        || endsWith(entry_key, ".ini")) {
        
        bool path_is_directory = std::filesystem::is_directory(std::filesystem::path(path_content));

        //Create absolute path too
        std::string entry_key_content = convert_path_native(path_content);
        strlwr(entry_key_content.data());
        std::string entry_key_root = content_root_path + entry_key;
        strlwr(entry_key_root.data());

        //Check if an override occurs
        std::shared_ptr<filesystem_entry> entry = get_content_entry_internal(paths, entry_key);
        if (!entry) entry = get_content_entry_internal(paths, entry_key_root);
        if (entry) {
            if (entry->path_content == path_content) {
                //Already added
                return entry.get();
            } else if ((path_is_directory && !options.replace_dirs) || (!path_is_directory && !options.replace_files)) {
                if (entry->key_content == entry_key_content) {
                    if (options.verbose) {
                        printf("Not replacing same path %s\n", path_content.c_str());
                    }
                } else {
                    if (options.verbose) {
                        printf("Not replacing path %s with %s\n", entry->path_content.c_str(), path_content.c_str());
                    }
                }
                return entry.get();
            } else {
                if (options.verbose) {
                    printf("Replacing path %s with %s\n", entry->path_content.c_str(), path_content.c_str());
                }
            }
        }
        
        //Create entry if previous doesnt exist
        if (!entry) {
            entry.reset(new filesystem_entry());
        } else if (entry) {
            filesystem_entry* original_entry = new filesystem_entry();
            original_entry->set(entry.get());
            entry->original_entry.reset(original_entry);
        }
        
        //Configure entry, this may replace previous data so the old keys redirect to new paths
        entry->key = entry_key;
        entry->key_content = entry_key_content;
        entry->path_content = path_content;
        std::string path_content_relative = path_content;
        prepare_path(path_content_relative, content_root_path);
        entry->path_content_relative = path_content_relative;
        entry->is_directory = path_is_directory;

        //Store path relative and absolute internal paths pointing the real FS path
        paths[entry->key] = entry;
        paths[entry->key_content] = entry;
        paths[entry_key_root] = entry;
        
        if (entry->is_directory) {
            if (endsWith(path_content, PATH_SEP_STR)) {
                path_content.erase(path_content.length()-1);
            } else {
                terminate_with_char(path_content, PATH_SEP);
            }
            add_filesystem_entry_internal(paths, path_content, destination_path, source_path, options);
        }

        return entry.get();
    }
    return nullptr;
}

bool scan_resource_paths(std::string destination_path, std::string source_path, const filesystem_scan_options* options) {
    //Use destination as source path assuming its a call to refresh subdir resources in root
    if (source_path.empty() && !destination_path.empty()) {
        source_path = destination_path;
    }
    bool same_paths = source_path == destination_path;

    //Remove root or any other stuff from source before adding
    prepare_path(source_path, content_root_path);
    if (!content_root_path.empty() && content_root_path != curdir_path) {
        source_path = content_root_path + source_path;
    }

    //Prepare destination too
    prepare_path(destination_path, content_root_path);
    strlwr(destination_path.data());

    //Setup options
    filesystem_scan_options scanOptions;
    scanOptions.verbose = content_debug_flag;
    scanOptions.replace_files = true;
    scanOptions.replace_dirs = false; //Might cause problems with code that appends stuff after dir path
    scanOptions.remove_dir_content = false;
    scanOptions.prepend_destination_to_path = endsWith(destination_path, PATH_SEP_STR);
    scanOptions.set(options);

    //If is actually a file then just add it
    if (std::filesystem::is_regular_file(std::filesystem::path(source_path))) {
        std::string source_parent_path = std::filesystem::path(source_path).parent_path().string();
        terminate_with_char(source_parent_path, PATH_SEP);
        add_filesystem_entry_internal(filesystem_entries,
                                      source_path,
                                      destination_path,
                                      source_parent_path,
                                      scanOptions);
        first_scan_flag = false;
        return true;
    }

    //Set different flags if is a dir and there is no options
    if (!options) {
        scanOptions.remove_dir_content = same_paths;
        scanOptions.prepend_destination_to_path = true;
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

    filesystem_entries_map paths;
    if (!scanOptions.remove_dir_content) {
        paths = filesystem_entries;
    } else {
        //Remove all paths that match with path to scan by not including them
        for (const auto& entry: filesystem_entries) {
            const filesystem_entry& fs_entry = *entry.second;
            //if (!startsWith(fs_entry.path_content, source_path)) {
            if (!startsWith(fs_entry.key, destination_path)) {
                paths.insert(entry);
            } else if (scanOptions.verbose) {
                printf("Removing dir content %s %s\n", fs_entry.key.c_str(), fs_entry.path_content.c_str());
            }
        }
    }

    //Add path as resource since iterator wont add it
    if (!destination_path.empty() && scanOptions.remove_dir_content) {
        add_filesystem_entry_internal(paths, source_path, destination_path, source_path, scanOptions);
    }

    //Do recursive search on source path
    for (const auto& entry : std::filesystem::recursive_directory_iterator(std::filesystem::path(source_path))) {
        if (entry.is_regular_file() || entry.is_directory()) {
            add_filesystem_entry_internal(paths, entry.path().string(), destination_path, source_path, scanOptions);
        }
    }

    //Store collected paths
    filesystem_entries.clear();
    filesystem_entries = paths;
    first_scan_flag = false;
    return true;
}

bool scan_resource_paths(const std::string& path) {
    return scan_resource_paths(path, path);
}

int file_open(const char* path, int oflags, int sflags) {
    //File may not exist, so we need to convert only parent path
    bool parent_only = oflags & _O_CREAT;
    std::string path_open = convert_path_content(path, parent_only);
    return open(path_open.c_str(), oflags, sflags);
}
