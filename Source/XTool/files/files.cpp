#include "tweaks.h"

#ifdef _WIN32
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

std::filesystem::path content_root_path = std::filesystem::path();
std::string content_root_path_str;

void filesystem_entry::set(filesystem_entry* entry) {
    if (entry == nullptr) return;
    this->key = entry->key;
    this->key_content = entry->key_content;
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
    std::filesystem::path path_fs = std::filesystem::u8path(convert_path_native(path));
    parent = path_fs.parent_path().u8string();
    if (parent.empty()) {
        parent = content_root_path_str;
    }
    if (filename) {
        *filename = path_fs.filename().u8string();
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
        std::filesystem::path path_fs = std::filesystem::u8path(conv);
        conv = path_fs.parent_path().u8string();
        if (conv.empty()) {
            result = content_root_path_str + path_fs.filename().u8string();
        } else {
            conv = string_to_lower(conv.c_str());
            filesystem_entry* entry = get_content_entry_internal(filesystem_entries, conv).get();
            if (entry) {
                result = entry->path_content + PATH_SEP + path_fs.filename().u8string();
            }
        }
    } else {
        conv = string_to_lower(conv.c_str());
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
    prepare_path(path, content_root_path_str);
    path = string_to_lower(path.c_str());
    return get_content_entry_internal(filesystem_entries, path).get();
}

std::vector<filesystem_entry*> get_content_entries_recursive(std::string path) {
    prepare_path(path, content_root_path_str);
    path = string_to_lower(path.c_str());
    std::vector<filesystem_entry*> paths;
    for (const auto& entry : filesystem_entries) {
        if (path.empty() || startsWith(entry.first, path)) {
            paths.emplace_back(entry.second.get());
        }
    }
    return paths;
}

std::vector<filesystem_entry*> get_content_entries_directory(std::string path) {
    prepare_path(path, content_root_path_str);
    path = string_to_lower(path.c_str());
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
        std::string line;
        filesystem_entry* entry = pair.second.get();
        line +=   "+-[Entry]-+";
        line += "\n| Map  Key: ";
        line += pair.first;
        line += "\n| Rel  Key: ";
        line += entry->key;
        line += "\n| Abs  Key: ";
        line += entry->key_content;
        line += "\n| Abs Path: ";
        line += entry->path_content;
        filesystem_entry* original = entry->original_entry.get();
        while (original) {
            line += "\n| Replaces: ";
            line += original->path_content;
            original = original->original_entry.get();
        }
        line += "\n+---------+\n\n";
        xs < line.c_str();
    }
    xs.close();
}

void clear_content_entries() {
    first_scan_flag = true;
    content_root_path.clear();
    content_root_path_str.clear();
    filesystem_entries.clear();
}

void set_content_root_path(const std::filesystem::path& path) {
    if (path == "." || path == curdir_path) {
        content_root_path.clear();
        content_root_path_str.clear();
    } else {
        content_root_path = path;
        content_root_path_str = content_root_path.u8string();
        if (!content_root_path_str.empty()) {
            terminate_with_char(content_root_path_str, PATH_SEP);
        }
    }
}

const std::filesystem::path& get_content_root_path() {
    return content_root_path;
}

const std::string& get_content_root_path_str() {
    return content_root_path_str;
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
    //Remove root since working directory is already there
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
    entry_key = string_to_lower(entry_key.c_str());
    if (startsWith(entry_key, "resource")
        || startsWith(entry_key, "cache")
        || startsWith(entry_key, "mods")
        || startsWith(entry_key, "scripts")
        || startsWith(entry_key, "crashdata")
        || endsWith(entry_key, ".ini")) {
        
        bool path_is_directory = std::filesystem::is_directory(std::filesystem::u8path(path_content));

        //Create absolute path too
        std::string entry_key_content = convert_path_native(path_content);
        entry_key_content = string_to_lower(entry_key_content.c_str());

        //Check if an override occurs
        std::shared_ptr<filesystem_entry> entry = get_content_entry_internal(paths, entry_key);
        //if (!entry) entry = get_content_entry_internal(paths, entry_key_root);
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
        entry->is_directory = path_is_directory;

        //Store path relative and absolute internal paths pointing the real FS path
        paths[entry->key] = entry;
        paths[entry->key_content] = entry;
        
        if (entry->is_directory) {
            std::string destination_path_copy = destination_path;
            std::string source_path_copy = source_path;
            if (endsWith(path_content, PATH_SEP_STR)) {
                path_content.erase(path_content.length()-1);
                if (endsWith(destination_path_copy, PATH_SEP_STR)) {
                    destination_path_copy.erase(destination_path_copy.length()-1);
                }
                if (endsWith(source_path_copy, PATH_SEP_STR)) {
                    source_path_copy.erase(source_path_copy.length()-1);
                }
            } else {
                terminate_with_char(path_content, PATH_SEP);
                terminate_with_char(destination_path_copy, PATH_SEP);
                terminate_with_char(source_path_copy, PATH_SEP);
            }
            add_filesystem_entry_internal(paths, path_content, destination_path_copy, source_path_copy, options);
        }

        return entry.get();
    }
    return nullptr;
}

bool scan_resource_paths(std::string destination_path, std::string source_path, const filesystem_scan_options* options) {    
    //Use destination as source path assuming its a call to refresh subdir resources in root
    if (source_path.empty()) {
        if (destination_path.empty()) {
            fprintf(stderr, "Attempted to rescan a empty path\n");
            xassert(0);
            return false;
        }
        source_path = destination_path;
    }
    bool same_paths = source_path == destination_path;

    //Remove root or any other stuff from source before adding
    prepare_path(source_path, content_root_path_str);

    //Prepare destination too
    prepare_path(destination_path, content_root_path_str);
    destination_path = string_to_lower(destination_path.c_str());

    //Setup options
    filesystem_scan_options scanOptions;
    scanOptions.verbose = content_debug_flag;
    scanOptions.replace_files = true;
    scanOptions.replace_dirs = false; //Might cause problems with code that appends stuff after dir path
    scanOptions.remove_dir_content = false;
    scanOptions.prepend_destination_to_path = endsWith(destination_path, PATH_SEP_STR);
    scanOptions.set(options);

    //If is actually a file then just add it
    if (std::filesystem::is_regular_file(std::filesystem::u8path(source_path))) {
        std::string source_parent_path = std::filesystem::u8path(source_path).parent_path().u8string();
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

    //Check if is actually a dir
    std::filesystem::path source_dir = std::filesystem::u8path(source_path.empty() ? curdir_path : source_path);
    if (!std::filesystem::is_directory(source_dir)) {
        fprintf(stderr, "scan_resource_paths not a directory %s\n", source_path.c_str());
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
    for (const auto& entry : std::filesystem::recursive_directory_iterator(source_dir)) {
        if (entry.is_regular_file() || entry.is_directory()) {
            add_filesystem_entry_internal(paths, entry.path().u8string(), destination_path, source_path, scanOptions);
        }
    }

    //Store collected paths
    filesystem_entries.clear();
    filesystem_entries = paths;
    first_scan_flag = false;
    return true;
}

int file_open(const char* path, int oflags, int sflags) {
    //File may not exist, so we need to convert only parent path
    bool parent_only = oflags & _O_CREAT;
    std::string path_open = convert_path_content(path, parent_only);
    return open(path_open.c_str(), oflags, sflags);
}
