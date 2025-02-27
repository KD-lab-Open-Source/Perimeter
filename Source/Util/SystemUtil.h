#ifndef __SYSTEM_UTIL_H__
#define __SYSTEM_UTIL_H__

/////////////////////////////////////////////////////////////////////////////////
//		Memory check
/////////////////////////////////////////////////////////////////////////////////
#ifdef _DEBUG
#define DBGCHECK
//void win32_check();
//#define DBGCHECK	win32_check();
#else
#define DBGCHECK
#endif

/////////////////////////////////////////////////////////////////////////////////
//		File/Paths
/////////////////////////////////////////////////////////////////////////////////

bool create_directories(const std::string& path, std::error_code* error = nullptr);

/////////////////////////////////////////////////////////////////////////////////
//		Key Press
/////////////////////////////////////////////////////////////////////////////////
#include "xkey.h"
#include <SDL_events.h>

#ifndef WM_MOUSEWHEEL
#define WM_MOUSEWHEEL (WM_MOUSELAST + 1)
#endif

bool applicationHasFocus();
bool applicationIsGo();

void initKeyboardMapping();
SDL_Scancode getSDLScanCodeFromVK(uint32_t key);
uint32_t getModFlagFromKey(uint32_t key);
bool isPressed(uint32_t key);
inline bool isShiftPressed() { return isPressed(VK_SHIFT); }
inline bool isControlPressed() { return isPressed(VK_CONTROL); }
inline bool isAltPressed() { return isPressed(VK_ALT); }

const uint32_t KBD_CTRL = 1 << 8;
const uint32_t KBD_SHIFT = 1 << 9;
const uint32_t KBD_ALT = 1 << 10;

struct sKey {
    union
    {
        struct
        {
            uint8_t key;
            uint8_t ctrl : 1;
            uint8_t shift : 1;
            uint8_t alt	: 1;
        };
        uint32_t fullkey;
    };

    explicit sKey(SDL_Keysym keysym);

    explicit sKey(uint32_t key_, bool set_mods_pressed);

    sKey();
	
	bool pressed() const {
		return key != 0
        && isPressed(key)
        && !(ctrl ^ isControlPressed())
        && !(shift ^ isShiftPressed())
        && !(alt ^ isAltPressed());
	}
};


#ifdef _WIN32
//TODO get rid of HWND once we have ingame dev UI
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#include <windows.h>
extern HWND hWndVisGeneric;
#endif

// ---   Focus   ------------------------------
extern SDL_Window* sdlWindow;
inline void RestoreFocus() { SDL_RaiseWindow(sdlWindow); }

// ---   Ini file   ------------------------------

uint32_t ReadIniString(const char* section, const char* key, const char* defaultVal,
                       char* returnBuffer, uint32_t bufferSize, const std::string& filePath);

uint32_t WriteIniString(const char* section, const char* key, const char* value, const std::string& filePath);

class IniManager
{
private:
    void* ini_instance = nullptr;
    bool pending_changes = false;
	std::string fname_;
    bool is_full_path;
    std::string getFilePath();
public:
	bool check_existence = true;
	explicit IniManager(const char* fname, bool check_existence = true, bool full_path = false);
    ~IniManager();
    bool load();
    bool save();
	const char* get(const char* section, const char* key);
	void put(const char* section, const char* key, const char* val);
	int getInt(const char* section, const char* key);
	bool getInt(const char* section, const char* key, int& value);
	void putInt(const char* section, const char* key, int val);
    float getFloat(const char* section, const char* key);
    bool getFloat(const char* section, const char* key, float& value);
	void putFloat(const char* section, const char* key, float val);
	void getFloatArray(const char* section, const char* key, int size, float array[]);
	void putFloatArray(const char* section, const char* key, int size, const float array[]);
};

// ---  Files ------------------------------

std::string setExtension(const std::string& file_name, const char* extension);
std::string getExtension(const std::string& file_name, bool process);
std::string getPrefFilePath(const std::string& file_name);

// --- Settings ------
IniManager* getSettings();
std::string getStringSettings(const std::string& keyName, const std::string& defaultValue = "");
void putStringSettings(const std::string& keyName, const std::string& value);

// --- Formatting ------
std::string formatTimeWithHour(int timeMilis);
std::string formatTimeWithoutHour(int timeMilis);

#endif //__SYSTEM_UTIL_H__