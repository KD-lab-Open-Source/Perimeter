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
//#define VK_TILDE	0xC0

#ifndef WM_MOUSEWHEEL
#define WM_MOUSEWHEEL (WM_MOUSELAST + 1)
#endif

bool applicationHasFocus();
bool applicationIsGo();

bool isPressed(uint32_t key);
inline bool isShiftPressed() { return isPressed(VK_SHIFT); }
inline bool isControlPressed() { return isPressed(VK_CONTROL); }
inline bool isAltPressed() { return isPressed(VK_ALT); }

const unsigned int KBD_CTRL = 1 << 8;
const unsigned int KBD_SHIFT = 1 << 9;
const unsigned int KBD_ALT = 1 << 10;

struct sKey {
    union
    {
        struct
        {
            unsigned char key;
            unsigned char ctrl : 1;
            unsigned char shift : 1;
            unsigned char alt	: 1;
        };
        int fullkey;
    };

    explicit sKey(SDL_Keysym keysym, bool set_by_async_funcs = false);

	sKey(int key_ = 0, bool set_by_async_funcs = false);
	
	bool pressed() const {
		return isPressed(key) && !(ctrl ^ isControlPressed()) && !(shift ^ isShiftPressed()) && !(alt ^ isAltPressed());
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
	std::string fname_;
    bool is_full_path;
    std::string getFilePath();
public:
	bool check_existence = true;
	explicit IniManager(const char* fname, bool check_existence = true, bool full_path = false);
	const char* get(const char* section, const char* key);
	void put(const char* section, const char* key, const char* val);
	int getInt(const char* section, const char* key);
	bool getInt(const char* section, const char* key, int& value);
	void putInt(const char* section, const char* key, int val);
	float getFloat(const char* section, const char* key);
	void putFloat(const char* section, const char* key, float val);
	void getFloatArray(const char* section, const char* key, int size, float array[]);
	void putFloatArray(const char* section, const char* key, int size, const float array[]);
};

// ---  Files ------------------------------

std::string setExtension(const std::string& file_name, const char* extension);
std::string getExtension(const std::string& file_name, bool process);

// --- Settings ------
IniManager* getSettings();
std::string getStringSettings(const std::string& keyName, const std::string& defaultValue = "");
void putStringSettings(const std::string& keyName, const std::string& value);

// --- Formatting ------
std::string formatTimeWithHour(int timeMilis);
std::string formatTimeWithoutHour(int timeMilis);

#endif //__SYSTEM_UTIL_H__