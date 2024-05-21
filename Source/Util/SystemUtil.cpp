#include <cinttypes>
#include "StdAfx.h"
#include <SDL.h>
#include "Config.h"
#include "UnitAttribute.h"
#include "GameContent.h"
#include "files/files.h"

#define SI_SUPPORT_IOSTREAMS 1
#include <SimpleIni.h>

//#include <crtdbg.h>

/////////////////////////////////////////////////////////////////////////////////
//		Memory check
/////////////////////////////////////////////////////////////////////////////////
//void win32_check()
//{
//	_ASSERTE(_CrtCheckMemory()) ;
//}

/////////////////////////////////////////////////////////////////////////////////
//		Keys
/////////////////////////////////////////////////////////////////////////////////

//If you wonder why we are converting SDL2 keys into VK_*, mostly to keep compatibility with controls.ini and
//existing code

std::vector<std::vector<SDL_Scancode>> vk_to_scancodes = {};

int16_t GetVKFromScanCode(SDL_Scancode scancode) {
    switch (scancode) {
        default:
        case SDL_SCANCODE_UNKNOWN:           break;
        case SDL_SCANCODE_A:                 return 'A';
        case SDL_SCANCODE_B:                 return 'B';
        case SDL_SCANCODE_C:                 return 'C';
        case SDL_SCANCODE_D:                 return 'D';
        case SDL_SCANCODE_E:                 return 'E';
        case SDL_SCANCODE_F:                 return 'F';
        case SDL_SCANCODE_G:                 return 'G';
        case SDL_SCANCODE_H:                 return 'H';
        case SDL_SCANCODE_I:                 return 'I';
        case SDL_SCANCODE_J:                 return 'J';
        case SDL_SCANCODE_K:                 return 'K';
        case SDL_SCANCODE_L:                 return 'L';
        case SDL_SCANCODE_M:                 return 'M';
        case SDL_SCANCODE_N:                 return 'N';
        case SDL_SCANCODE_O:                 return 'O';
        case SDL_SCANCODE_P:                 return 'P';
        case SDL_SCANCODE_Q:                 return 'Q';
        case SDL_SCANCODE_R:                 return 'R';
        case SDL_SCANCODE_S:                 return 'S';
        case SDL_SCANCODE_T:                 return 'T';
        case SDL_SCANCODE_U:                 return 'U';
        case SDL_SCANCODE_V:                 return 'V';
        case SDL_SCANCODE_W:                 return 'W';
        case SDL_SCANCODE_X:                 return 'X';
        case SDL_SCANCODE_Y:                 return 'Y';
        case SDL_SCANCODE_Z:                 return 'Z';
        case SDL_SCANCODE_1:                 return '1';
        case SDL_SCANCODE_2:                 return '2';
        case SDL_SCANCODE_3:                 return '3';
        case SDL_SCANCODE_4:                 return '4';
        case SDL_SCANCODE_5:                 return '5';
        case SDL_SCANCODE_6:                 return '6';
        case SDL_SCANCODE_7:                 return '7';
        case SDL_SCANCODE_8:                 return '8';
        case SDL_SCANCODE_9:                 return '9';
        case SDL_SCANCODE_0:                 return '0';
        case SDL_SCANCODE_RETURN:            return VK_RETURN;
        case SDL_SCANCODE_ESCAPE:            return VK_ESCAPE;
        case SDL_SCANCODE_BACKSPACE:         return VK_BACK;
        case SDL_SCANCODE_TAB:               return VK_TAB;
        case SDL_SCANCODE_SPACE:             return VK_SPACE;
        case SDL_SCANCODE_MINUS:             return VK_MINUS;
        case SDL_SCANCODE_EQUALS:            return VK_EQUALS;
        case SDL_SCANCODE_LEFTBRACKET:       return VK_LEFTBRACKET;
        case SDL_SCANCODE_RIGHTBRACKET:      return VK_RIGHTBRACKET;
        case SDL_SCANCODE_NONUSHASH:
        case SDL_SCANCODE_BACKSLASH:         return VK_BACKSLASH;
        case SDL_SCANCODE_SEMICOLON:         return VK_SEMICOLON;
        case SDL_SCANCODE_APOSTROPHE:        return VK_QUOTE;
        case SDL_SCANCODE_GRAVE:             return VK_TILDE;
        case SDL_SCANCODE_COMMA:             return VK_COMMA;
        case SDL_SCANCODE_PERIOD:            return VK_PERIOD;
        case SDL_SCANCODE_SLASH:             return VK_SLASH;
        case SDL_SCANCODE_CAPSLOCK:          return VK_CAPITAL;
        case SDL_SCANCODE_F1:                return VK_F1;
        case SDL_SCANCODE_F2:                return VK_F2;
        case SDL_SCANCODE_F3:                return VK_F3;
        case SDL_SCANCODE_F4:                return VK_F4;
        case SDL_SCANCODE_F5:                return VK_F5;
        case SDL_SCANCODE_F6:                return VK_F6;
        case SDL_SCANCODE_F7:                return VK_F7;
        case SDL_SCANCODE_F8:                return VK_F8;
        case SDL_SCANCODE_F9:                return VK_F9;
        case SDL_SCANCODE_F10:               return VK_F10;
        case SDL_SCANCODE_F11:               return VK_F11;
        case SDL_SCANCODE_F12:               return VK_F12;
        case SDL_SCANCODE_PRINTSCREEN:       return 0;
        case SDL_SCANCODE_SCROLLLOCK:        return VK_SCROLL;
        case SDL_SCANCODE_PAUSE:             return VK_PAUSE;
        case SDL_SCANCODE_INSERT:            return VK_INSERT;
        case SDL_SCANCODE_HOME:              return VK_HOME;
        case SDL_SCANCODE_PAGEUP:            return VK_PRIOR;
        case SDL_SCANCODE_DELETE:            return VK_DELETE;
        case SDL_SCANCODE_END:               return VK_END;
        case SDL_SCANCODE_PAGEDOWN:          return VK_NEXT;
        case SDL_SCANCODE_RIGHT:             return VK_RIGHT;
        case SDL_SCANCODE_LEFT:              return VK_LEFT;
        case SDL_SCANCODE_DOWN:              return VK_DOWN;
        case SDL_SCANCODE_UP:                return VK_UP;
        case SDL_SCANCODE_NUMLOCKCLEAR:      return VK_NUMLOCK;
        case SDL_SCANCODE_KP_DIVIDE:         return VK_DIVIDE;
        case SDL_SCANCODE_KP_MULTIPLY:       return VK_MULTIPLY;
        case SDL_SCANCODE_KP_MINUS:          return VK_SUBTRACT;
        case SDL_SCANCODE_KP_PLUS:           return VK_ADD;
        case SDL_SCANCODE_KP_ENTER:          return VK_RETURN;
        case SDL_SCANCODE_KP_0:              return VK_NUMPAD0;
        case SDL_SCANCODE_KP_1:              return VK_NUMPAD1;
        case SDL_SCANCODE_KP_2:              return VK_NUMPAD2;
        case SDL_SCANCODE_KP_3:              return VK_NUMPAD3;
        case SDL_SCANCODE_KP_4:              return VK_NUMPAD4;
        case SDL_SCANCODE_KP_5:              return VK_NUMPAD5;
        case SDL_SCANCODE_KP_6:              return VK_NUMPAD6;
        case SDL_SCANCODE_KP_7:              return VK_NUMPAD7;
        case SDL_SCANCODE_KP_8:              return VK_NUMPAD8;
        case SDL_SCANCODE_KP_9:              return VK_NUMPAD9;
        case SDL_SCANCODE_KP_PERIOD:         return VK_PERIOD;
        case SDL_SCANCODE_NONUSBACKSLASH:    return VK_LESS;
        case SDL_SCANCODE_APPLICATION:
        case SDL_SCANCODE_POWER:             return 0;
        case SDL_SCANCODE_KP_EQUALS:         return VK_EQUALS;
        case SDL_SCANCODE_F13:               return VK_F13;
        case SDL_SCANCODE_F14:               return VK_F14;
        case SDL_SCANCODE_F15:               return VK_F15;
        case SDL_SCANCODE_F16:               return VK_F16;
        case SDL_SCANCODE_F17:               return VK_F17;
        case SDL_SCANCODE_F18:               return VK_F18;
        case SDL_SCANCODE_F19:               return VK_F19;
        case SDL_SCANCODE_F20:               return VK_F20;
        case SDL_SCANCODE_F21:               return VK_F21;
        case SDL_SCANCODE_F22:               return VK_F22;
        case SDL_SCANCODE_F23:               return VK_F23;
        case SDL_SCANCODE_F24:               return VK_F24;
        case SDL_SCANCODE_EXECUTE:
        case SDL_SCANCODE_HELP:
        case SDL_SCANCODE_MENU:
        case SDL_SCANCODE_SELECT:
        case SDL_SCANCODE_STOP:
        case SDL_SCANCODE_AGAIN:
        case SDL_SCANCODE_UNDO:
        case SDL_SCANCODE_CUT:
        case SDL_SCANCODE_COPY:
        case SDL_SCANCODE_PASTE:
        case SDL_SCANCODE_FIND:
        case SDL_SCANCODE_MUTE:
        case SDL_SCANCODE_VOLUMEUP:
        case SDL_SCANCODE_VOLUMEDOWN:        return 0;
        case SDL_SCANCODE_KP_COMMA:          return VK_COMMA;
        case SDL_SCANCODE_KP_EQUALSAS400:
        case SDL_SCANCODE_INTERNATIONAL1:
        case SDL_SCANCODE_INTERNATIONAL2:
        case SDL_SCANCODE_INTERNATIONAL3:
        case SDL_SCANCODE_INTERNATIONAL4:
        case SDL_SCANCODE_INTERNATIONAL5:
        case SDL_SCANCODE_INTERNATIONAL6:
        case SDL_SCANCODE_INTERNATIONAL7:
        case SDL_SCANCODE_INTERNATIONAL8:
        case SDL_SCANCODE_INTERNATIONAL9:
        case SDL_SCANCODE_LANG1:
        case SDL_SCANCODE_LANG2:
        case SDL_SCANCODE_LANG3:
        case SDL_SCANCODE_LANG4:
        case SDL_SCANCODE_LANG5:
        case SDL_SCANCODE_LANG6:
        case SDL_SCANCODE_LANG7:
        case SDL_SCANCODE_LANG8:
        case SDL_SCANCODE_LANG9:
        case SDL_SCANCODE_ALTERASE:
        case SDL_SCANCODE_SYSREQ:
        case SDL_SCANCODE_CANCEL:
        case SDL_SCANCODE_CLEAR:
        case SDL_SCANCODE_PRIOR:
        case SDL_SCANCODE_RETURN2:           return 0;
        case SDL_SCANCODE_SEPARATOR:         return VK_SEPARATOR;
        case SDL_SCANCODE_OUT:
        case SDL_SCANCODE_OPER:
        case SDL_SCANCODE_CLEARAGAIN:
        case SDL_SCANCODE_CRSEL:
        case SDL_SCANCODE_EXSEL:
        case SDL_SCANCODE_KP_00:
        case SDL_SCANCODE_KP_000:
        case SDL_SCANCODE_THOUSANDSSEPARATOR:
        case SDL_SCANCODE_DECIMALSEPARATOR:
        case SDL_SCANCODE_CURRENCYUNIT:
        case SDL_SCANCODE_CURRENCYSUBUNIT:
        case SDL_SCANCODE_KP_LEFTPAREN:
        case SDL_SCANCODE_KP_RIGHTPAREN:
        case SDL_SCANCODE_KP_LEFTBRACE:
        case SDL_SCANCODE_KP_RIGHTBRACE:     return 0;
        case SDL_SCANCODE_KP_TAB:            return VK_TAB;
        case SDL_SCANCODE_KP_BACKSPACE:      return VK_BACK;
        case SDL_SCANCODE_KP_A:
        case SDL_SCANCODE_KP_B:
        case SDL_SCANCODE_KP_C:
        case SDL_SCANCODE_KP_D:
        case SDL_SCANCODE_KP_E:
        case SDL_SCANCODE_KP_F:
        case SDL_SCANCODE_KP_XOR:
        case SDL_SCANCODE_KP_POWER:
        case SDL_SCANCODE_KP_PERCENT:        return 0;
        case SDL_SCANCODE_KP_LESS:           return VK_LESS;
        case SDL_SCANCODE_KP_GREATER:
        case SDL_SCANCODE_KP_AMPERSAND:
        case SDL_SCANCODE_KP_DBLAMPERSAND:
        case SDL_SCANCODE_KP_VERTICALBAR:
        case SDL_SCANCODE_KP_DBLVERTICALBAR:
        case SDL_SCANCODE_KP_COLON:
        case SDL_SCANCODE_KP_HASH:           return 0;
        case SDL_SCANCODE_KP_SPACE:          return VK_SPACE;
        case SDL_SCANCODE_KP_AT:
        case SDL_SCANCODE_KP_EXCLAM:
        case SDL_SCANCODE_KP_MEMSTORE:
        case SDL_SCANCODE_KP_MEMRECALL:
        case SDL_SCANCODE_KP_MEMCLEAR:
        case SDL_SCANCODE_KP_MEMADD:
        case SDL_SCANCODE_KP_MEMSUBTRACT:
        case SDL_SCANCODE_KP_MEMMULTIPLY:
        case SDL_SCANCODE_KP_MEMDIVIDE:
        case SDL_SCANCODE_KP_PLUSMINUS:
        case SDL_SCANCODE_KP_CLEAR:
        case SDL_SCANCODE_KP_CLEARENTRY:
        case SDL_SCANCODE_KP_BINARY:
        case SDL_SCANCODE_KP_OCTAL:
        case SDL_SCANCODE_KP_DECIMAL:
        case SDL_SCANCODE_KP_HEXADECIMAL:    return 0;
        case SDL_SCANCODE_LCTRL:             return VK_CONTROL;
        case SDL_SCANCODE_LSHIFT:            return VK_SHIFT;
        case SDL_SCANCODE_LALT:              return VK_ALT;
        case SDL_SCANCODE_LGUI:              return 0;
        case SDL_SCANCODE_RCTRL:             return VK_CONTROL;
        case SDL_SCANCODE_RSHIFT:            return VK_SHIFT;
        case SDL_SCANCODE_RALT:              return VK_ALT;
        case SDL_SCANCODE_RGUI:
        case SDL_SCANCODE_MODE:
        case SDL_SCANCODE_AUDIONEXT:
        case SDL_SCANCODE_AUDIOPREV:
        case SDL_SCANCODE_AUDIOSTOP:
        case SDL_SCANCODE_AUDIOPLAY:
        case SDL_SCANCODE_AUDIOMUTE:
        case SDL_SCANCODE_MEDIASELECT:
        case SDL_SCANCODE_WWW:
        case SDL_SCANCODE_MAIL:
        case SDL_SCANCODE_CALCULATOR:
        case SDL_SCANCODE_COMPUTER:
        case SDL_SCANCODE_AC_SEARCH:
        case SDL_SCANCODE_AC_HOME:
        case SDL_SCANCODE_AC_BACK:
        case SDL_SCANCODE_AC_FORWARD:
        case SDL_SCANCODE_AC_STOP:
        case SDL_SCANCODE_AC_REFRESH:
        case SDL_SCANCODE_AC_BOOKMARKS:
        case SDL_SCANCODE_BRIGHTNESSDOWN:
        case SDL_SCANCODE_BRIGHTNESSUP:
        case SDL_SCANCODE_DISPLAYSWITCH:
        case SDL_SCANCODE_KBDILLUMTOGGLE:
        case SDL_SCANCODE_KBDILLUMDOWN:
        case SDL_SCANCODE_KBDILLUMUP:
        case SDL_SCANCODE_EJECT:
        case SDL_SCANCODE_SLEEP:
        case SDL_SCANCODE_APP1:
        case SDL_SCANCODE_APP2:
        case SDL_SCANCODE_AUDIOREWIND:
        case SDL_SCANCODE_AUDIOFASTFORWARD:  return 0;
    }
    return -1;
}

void initKeyboardMapping() {
    vk_to_scancodes.clear();
    for (int i = 0; i < 0xFF; ++i) {
        vk_to_scancodes.emplace_back();
    }
    for (int i = 0; i < SDL_NUM_SCANCODES; ++i) {
        SDL_Scancode scancode = static_cast<SDL_Scancode>(i);
        int16_t vk = GetVKFromScanCode(scancode);
        if (vk <= 0) continue;
        vk_to_scancodes[vk].emplace_back(scancode);
    }
}

bool isPressed(uint32_t key) {
    if (!applicationHasFocus()) return false;
    if (key >= vk_to_scancodes.size()) {
        xassert(0);
#ifdef PERIMETER_DEBUG
        fprintf(stderr, "VK key out of bounds requested %" PRIu32 "\n", key);
#endif
        return false;
    }
    
    //Handle special keys
    switch (key) {
        case VK_LBUTTON:
        case VK_MBUTTON:
        case VK_RBUTTON:
            return SDL_GetMouseState(nullptr, nullptr) & key;
        case VK_SHIFT:
            return SDL_GetModState() & KMOD_SHIFT;
        case VK_CONTROL:
            return SDL_GetModState() & KMOD_CTRL;
        case VK_ALT:
            return SDL_GetModState() & KMOD_ALT;
        default:
            break;
    }
    
    //Check normal keys
    const std::vector<SDL_Scancode>& scancodes = vk_to_scancodes[key];
    if (scancodes.empty()) {
#ifdef PERIMETER_DEBUG
        fprintf(stderr, "Unmmaped VK key requested %" PRIu32 "\n", key);
#endif
        return false;
    }
    
    //Get state of keys
    int numkeys;
    const Uint8* state = SDL_GetKeyboardState(&numkeys);

    //Check the scancodes associated to this key if one is pressed
    for (const SDL_Scancode& scancode : scancodes) {
        if (scancode >= numkeys) {
            xassert(0);
#ifdef PERIMETER_DEBUG
            fprintf(stderr,
                    "VK key %" PRIu32 " has a scancode out of bounds %" PRIu32 "\n",
                    key, static_cast<uint32_t>(scancode));
#endif
            continue;
        }
        if (state[scancode]) return true;
    }
    return false;
}

sKey::sKey(SDL_Keysym keysym) {
    int16_t vk = GetVKFromScanCode(keysym.scancode);
    if (vk <= 0) {
        key = 0;
#ifdef PERIMETER_DEBUG
        if (vk < 0) {
            fprintf(stderr, "Unknown SDL key requested scan %d sym %d\n", keysym.scancode, keysym.sym);
        }
#endif
    } else {
        key = vk;
    }
    //store base key at fullkey before applying extras
    fullkey = key;
    
    //Add modifiers
    ctrl = 0;
    shift = 0;
    alt = 0;

    auto mod = keysym.mod;
    if ((mod & KMOD_SHIFT) != 0) {
        fullkey |= KBD_SHIFT;
        shift |= 1;
    }
    if ((mod & KMOD_CTRL) != 0) {
        fullkey |= KBD_CTRL;
        ctrl |= 1;
    }
    if ((mod & KMOD_ALT) != 0) {
        fullkey |= KBD_ALT;
        alt |= 1;
    }

    // добавляем расширенные коды для командных кодов
    if (key == VK_CONTROL) {
        ctrl |= 1;
    }
    if (key == VK_SHIFT) {
        shift |= 1;
    }
    if (key == VK_ALT) {
        alt |= 1;
    }
}

sKey::sKey(int key_) {
    fullkey = key = key_;
    ctrl = isControlPressed();
    shift = isShiftPressed();
    alt = isAltPressed();
    // добавляем расширенные коды для командных кодов
    if (key == VK_CONTROL) {
        ctrl |= 1;
    }
    if (key == VK_SHIFT) {
        shift |= 1;
    }
    if (key == VK_ALT) {
        alt |= 1;
    }
}

sKey::sKey() {
    fullkey = 0;
}

/////////////////////////////////////////////////////////////////////////////////
//		File/Paths
/////////////////////////////////////////////////////////////////////////////////

bool create_directories(const std::string& path, std::error_code* error) {
    std::string current;
    std::string part;
    size_t size = path.size();
    for (int i = 0; i < size; ++i) {
        char c = path[i];
        if (c != '\\' && c != '/') {
            part += c;
            
            //If this is last char then continue by creating dir on last part
            if (i + 1 < size) {
                continue;
            }
        }
        
        //Use native PATH_SEP and add path until now to parent
        if (!current.empty()) {
            current += PATH_SEP;
        }
        current += part;
        part.clear();
        
        //Check if parent path exists, if so don't do anything
        //We use our function for case-insensitivity
        std::string path_content = convert_path_content(current);
        if (!path_content.empty()) {
            //In case real path has different case than one provided
            current = path_content;
            continue;
        }
        
        //Create dir since doesn't exist
        std::filesystem::path current_fs = std::filesystem::u8path(current);
        bool result;
        if (error) {
            result = std::filesystem::create_directory(current_fs, *error);
        } else {
            result = std::filesystem::create_directory(current_fs);
        }
        
        //Add this new dir to paths
        xassert(result);
        if (result) {
            scan_resource_paths(current);
        }
    }
    xassert(part.empty());
    std::filesystem::path current_fs = std::filesystem::u8path(current);
    bool result = std::filesystem::is_directory(current_fs);
    if (result) {
        scan_resource_paths(current);
    }
    return result;
}

// ---   Ini file   ---------------------
//TODO move CSimpleIniA into IniManager so we avoid loading/saving file each time we read/write a key

uint32_t ReadIniString(const char* section, const char* key, const char* defaultVal,
                       char* returnBuffer, uint32_t bufferSize, const std::string& filePath) {
    //Load file content
    std::ifstream istream(std::filesystem::u8path(filePath), std::ios::in | std::ios::binary);
    CSimpleIniA ini;
    SI_Error rc = ini.LoadData(istream);
    if (rc < 0) {
        fprintf(stderr, "Error reading %s file: %d\n", filePath.c_str(), rc);
        return 0;
    }
    istream.close();
    
    //Read value and close
    const char* val = ini.GetValue(section, key, defaultVal);
    if (val) {
        SDL_strlcpy(returnBuffer, val, bufferSize);
    } else {
        *returnBuffer = 0;
    }
    
    ini.Reset();
    
    return 1;
}

uint32_t WriteIniString(const char* section, const char* key, const char* value, const std::string& filePath) {
    //Load file content
    std::ifstream istream(std::filesystem::u8path(filePath), std::ios::in | std::ios::binary);
    CSimpleIniA ini;
    SI_Error rc = ini.LoadData(istream);
    if (rc < 0) {
        fprintf(stderr, "Error loading %s file for writing: %d\n", filePath.c_str(), rc);
        return 0;
    }
    istream.close();
    
    //Write our value
    rc = ini.SetValue(section, key, value);
    if (rc < 0) {
        fprintf(stderr, "Error writing %s %s %s in file %s: %d\n", section, key, value, filePath.c_str(), rc);
        return 0;
    }
    
    //Save file changes
    std::ofstream ostream(std::filesystem::u8path(filePath), std::ios::out | std::ios::binary);
    rc = ini.Save(ostream);
    if (rc < 0) {
        fprintf(stderr, "Error writing %s file: %d\n", filePath.c_str(), rc);
        return 0;
    }
    ostream.close();

    ini.Reset();

    return 1;
}

IniManager::IniManager(const char* fname, bool check_existence_, bool full_path) {
    fname_ = fname;
    check_existence = check_existence_;
    is_full_path = full_path;
}

std::string IniManager::getFilePath() {
    if (!is_full_path) {
        std::string pathres = convert_path_content(fname_);
        if (pathres.empty()) {
            pathres = convert_path_content(fname_, true);
        }
        if (pathres.empty()) {
            ErrH.Abort("Ini file not found: ", XERR_USER, 0, fname_.c_str());
        }

        return pathres;
    } else {
        return fname_;
    }
}

const char* IniManager::get(const char* section, const char* key)
{
	static char buf[256];
    std::string path = getFilePath();
    
	if(!ReadIniString(section, key, nullptr, buf, 256, path)){
		*buf = 0;
		if (check_existence) {
            fprintf(stderr, "INI key not found %s %s %s\n", path.c_str(), section, key);
        }
	}

	return buf;
}
void IniManager::put(const char* section, const char* key, const char* val)
{
    std::string path = getFilePath();
    
    WriteIniString(section, key, val, path);
}

int IniManager::getInt(const char* section, const char* key) 
{ 
	return atoi(get(section, key));
}

bool IniManager::getInt(const char* section, const char* key, int& value) 
{
	const char* str=get(section, key);
	if(*str) {
		value=atoi(str);
		return true;
	}
	else return false;

}
void IniManager::putInt(const char* section, const char* key, int val) 
{
	std::string str = std::to_string(val);
	put(section, key, str.c_str());
}

float IniManager::getFloat(const char* section, const char* key) 
{ 
	return atof(get(section, key)); 
}
void IniManager::putFloat(const char* section, const char* key, float val) 
{
	XBuffer buf;
	buf <= val;
	put(section, key, buf);
}

void IniManager::getFloatArray(const char* section, const char* key, int size, float array[])
{
	const char* str = get(section, key); 
	XBuffer buf((void*)str, strlen(str) + 1);
	for(int i = 0; i < size; i++)
		buf >= array[i];
}
void IniManager::putFloatArray(const char* section, const char* key, int size, const float array[])
{
	XBuffer buf(256, 1);
	for(int i = 0; i < size; i++)
		buf <= array[i] < " ";
	put(section, key, buf);
}

// ---  Files ------------------------------

std::string getExtension(const std::string& file_name, bool process) {
    std::string str = convert_path_posix(file_name);
    size_t pos_dot = str.rfind('.');
    size_t pos_path = str.rfind('/');
    //Only get extension if dot is after the last path sep if any
    if (pos_dot != std::string::npos && (pos_path == std::string::npos || pos_path < pos_dot)) {
        str.erase(0, pos_dot + 1);
        if (process) {
            str = string_to_lower(str.c_str());
            while (isspace(str[str.size() - 1])) {
                str.erase(str.size() - 1);
            }
        }
        return str;
    } else {
        return "";
    }
}

std::string setExtension(const std::string& file_name, const char* extension) {
    std::string str = file_name;
    std::string old_ext = getExtension(file_name, false);
    if (!old_ext.empty()) {
        str.erase(str.size() - old_ext.length() - 1, str.size());
    }
    if (extension) {
        if (!endsWith(str, ".")) {
            str += ".";
        }
        str += extension;
    }
    return str;
}

// --- Settings ------

static IniManager* settingsManager;

IniManager* getSettings() {
    if (!settingsManager) {
        const char* prefPath_ptr = GET_PREF_PATH();
        std::string prefPath;
        if (prefPath_ptr) {
            prefPath = prefPath_ptr;
            SDL_free((void*) prefPath_ptr);
        }
        prefPath += "Settings.ini";
        if (!std::filesystem::exists(std::filesystem::u8path(prefPath))) {
            //Create file if doesn't exist
            XStream f;
            if (f.open(prefPath,XS_OUT)) {
                f.close();
            } else {
                //Failed to create in user dir, use game content
                prefPath = "Settings.ini";
                if (f.open(prefPath,XS_OUT)) {
                    f.close();
                }
            }
        }
        settingsManager = new IniManager(prefPath.c_str(), false, true);
    }
    return settingsManager;
}

std::string getStringSettings(const std::string& keyName, const std::string& defaultValue) {
    std::string res;
    bool found = false;

    IniManager* ini = getSettings();
    std::string key = terGameContentBase == GAME_CONTENT::CONTENT_NONE ? "Global" : getGameContentEnumName(terGameContentBase);
    const char* result = ini->get(key.c_str(), keyName.c_str());
    if (*result) {
        found = true;
        res = result;
    }
    
    return found ? res : defaultValue;
}

void putStringSettings(const std::string& keyName, const std::string& value) {
    IniManager* ini = getSettings();
    std::string key = terGameContentBase == GAME_CONTENT::CONTENT_NONE ? "Global" : getGameContentEnumName(terGameContentBase); 
    ini->put(key.c_str(), keyName.c_str(), value.c_str());
}

// --- Formatting ------

std::string formatTimeWithHour(int timeMilis) {
	std::string res;
	if (timeMilis >= 0) {
		int sec = timeMilis / 1000.0f;
		int min = sec / 60.0f;
		sec -= min * 60;
		int hour = min / 60.0f; 
		min -= hour * 60; 
		char str[11];
		sprintf(str, "%d", hour);
		res = (hour < 10) ? "0" : "";
		res += std::string(str) + ":";
		sprintf(str, "%d", min);
		res += (min < 10) ? "0" : "";
		res += std::string(str) + ":";
		sprintf(str, "%d", sec);
		res += (sec < 10) ? "0" : "";
		res += std::string(str);
	}
	return res;
}

std::string formatTimeWithoutHour(int timeMilis) {
	std::string res;
	if (timeMilis >= 0) {
		int sec = timeMilis / 1000.0f;
		int min = sec / 60.0f;
		sec -= min * 60;
		char str[11];
		sprintf(str, "%d", min);
		res = (min < 10) ? "0" : "";
		res += std::string(str) + ":";
		sprintf(str, "%d", sec);
		res += (sec < 10) ? "0" : "";
		res += std::string(str);
	}
	return res;
}
