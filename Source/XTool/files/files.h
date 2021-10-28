#ifndef PERIMETER_FILES_H
#define PERIMETER_FILES_H

/*
 * This implements a system to abstract real OS paths from game paths, thus the game can request
 * a filesystem_entry at path resources/icons/logo.tga and the entry will contain the real path used by OS
 * such as C:\Games\Perimeter\Resources\ICONS\logo.tga or ~/Games/Perimeter/RESOURCES/icons/logo.tga and other
 * extra information contained in struct if desired.
 * 
 * This allow mixing \ and / styled paths and adds case insensitivity like present at Windows filesystem in filesystems
 * that are case sensitive such as ones Linux uses usually. As fixing the entire codebase and correct paths incoming
 * from models and scripts would be too costly, this solution was more reasonable to implement only at a cost of
 * a possible path lookup overhead (optimizations may be possible, currently is a big hash map) and the initial
 * scanning of dirs required when starting up (a cache system or bg scanning may be implemented to speedup).
 * 
 * An initially unintended feature of this system is the possibility of overlay-ing parts of directory tree
 * and the possibility of supporting addons that "add" files to game without altering the real dirs structure.
 */

#include <memory>

//Use POSIX for MS funcs
#ifdef PERIMETER_EXODUS
#define _O_BINARY 0
#define _O_RDONLY O_RDONLY
#define _O_WRONLY O_WRONLY
#define _O_TRUNC O_TRUNC
#define _O_CREAT O_CREAT
#define _S_IREAD S_IREAD
#define _S_IWRITE S_IWRITE
#define _write write
#define _read read
#define _lseek lseek
#define _close close
#else
//For _open and other POSIX funcs
#include <io.h>
#endif

//Settings for scanning
struct filesystem_scan_options {
public:
    bool verbose = false;
    bool replace_files = false;
    bool replace_dirs = false;
    bool remove_dir_content = false;
    bool prepend_destination_to_path = false;
    
    void set(const filesystem_scan_options* options) {
        if (options == nullptr) return;
        this->verbose = options->verbose;
        this->replace_files = options->replace_files;
        this->replace_dirs = options->replace_dirs;
        this->remove_dir_content = options->remove_dir_content;
        this->prepend_destination_to_path = options->prepend_destination_to_path;
    }
};

//Contains entry for filesystem
class filesystem_entry {
public:
    //Relative path in game structure of this entry that serves as key 
    std::string key;
    //Real path including content root that serves as key of this entry 
    std::string key_content;
    
    //Path relative to content root
    std::string path_content_relative;
    //Real path including content root where this entry can be found
    std::string path_content;
    
    //Is this entry a directory?
    bool is_directory;
    
    //Original entry that this entry has replaced
    std::shared_ptr<filesystem_entry> original_entry;
    
    explicit filesystem_entry():
            key(), key_content(), path_content_relative(), path_content(), is_directory(false), original_entry() {
    }
    ~filesystem_entry() = default;
    
    void set(filesystem_entry* entry);
};

//Splits path into parent and filename
void split_path_parent(const std::string& path, std::string& parent, std::string* filename = nullptr);

//Converts Windows/POSIX to native path
std::string convert_path_native(const char* path);

//Do a conversion for Windows -> POSIX paths
std::string convert_path_posix(const char* path);

//Do a conversion from path key to filesystem entry path_content
std::string convert_path_content(const std::string& path, bool parent_only = false);

//Obtain filesystem entry from path key
filesystem_entry* get_content_entry(std::string path);

//Obtain pairs of lowercase and original path from Resource paths cache which match the path start
std::vector<filesystem_entry*> get_content_entries_recursive(std::string path);

//Obtain pairs of lowercase and original path from Resource paths cache which match the path start,
//only if
std::vector<filesystem_entry*> get_content_entries_directory(std::string path);

//Dumps current filesystem entries into XStream for debugging purposes
void dump_filesystem_entries(const std::string& xs);

//Clears the current loaded content paths
void clear_content_entries();

//Sets the current resources root path
void set_content_root_path(const std::string& path);

//Returns the current resources root path
const std::string& get_content_root_path();

//Scans source dir and creates resource paths cache, it can update only a certain subdirectory to avoid rescanning all files
//Removes the source path in each scanned path before saving to internal resource path list to destination path
bool scan_resource_paths(std::string destination_path = "", std::string source_path = "", const filesystem_scan_options* options = nullptr);

//Usual POSIX open but with path conversion
int file_open(const char* path, int oflags, int sflags = 0);

#endif //PERIMETER_FILES_H
