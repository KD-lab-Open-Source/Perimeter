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
//		File find
/////////////////////////////////////////////////////////////////////////////////
const char* win32_findfirst(const char* mask);
const char* win32_findnext();


/////////////////////////////////////////////////////////////////////////////////
//		Key Press
/////////////////////////////////////////////////////////////////////////////////
#include "xkey.h"
#ifdef PERIMETER_EXODUS_WINDOW
#include <SDL_events.h>
#endif
//#define VK_TILDE	0xC0

#ifndef WM_MOUSEWHEEL
#define WM_MOUSEWHEEL (WM_MOUSELAST + 1)
#endif

bool applicationHasFocus();
bool applicationIsGo();

#ifdef PERIMETER_EXODUS_WINDOW
bool isPressed(uint32_t key);
#else
inline bool isPressed(int key) {
    return GetAsyncKeyState(key) & 0x8000;
} 
#endif

inline bool isShiftPressed() { return isPressed(VK_SHIFT); }
inline bool isControlPressed() { return isPressed(VK_CONTROL); }
inline bool isAltPressed() { return isPressed(VK_MENU); }

const unsigned int KBD_CTRL = 1 << 8;
const unsigned int KBD_SHIFT = 1 << 9;
const unsigned int KBD_MENU = 1 << 10;

struct sKey {
    union
    {
        struct
        {
            unsigned char key;
            unsigned char ctrl : 1;
            unsigned char shift : 1;
            unsigned char menu	: 1;
        };
        int fullkey;
    };

#ifdef PERIMETER_EXODUS_WINDOW
    explicit sKey(SDL_Keysym keysym, bool set_by_async_funcs = false);
#endif

	sKey(int fullkey_ = 0, bool set_by_async_funcs = false);
	
	bool pressed() const {
		return isPressed(key) && !(ctrl ^ isControlPressed()) && !(shift ^ isShiftPressed()) && !(menu ^ isAltPressed());
	}
};



// ---   Focus   ------------------------------
extern HWND hWndVisGeneric;
inline void RestoreFocus() { SetFocus(hWndVisGeneric); }

// ---   Ini file   ------------------------------
class IniManager
{
	std::string fname_;
	bool check_existence_;
public:
	IniManager(const char* fname, bool check_existence = true);
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
inline std::string setExtention(const char* file_name, const char* extention)
{
	std::string str = file_name;
	size_t pos = str.rfind(".");
	if(pos != std::string::npos)
		str.erase(pos, str.size());
	return str + "." + extention;
}

inline std::string getExtention(const char* file_name)
{
	std::string str = file_name;
	size_t pos = str.rfind(".");
	if(pos != std::string::npos){
		str.erase(0, pos + 1);
		strlwr((char*)str.c_str());
		while(isspace(str[str.size() - 1]))
			str.erase(str.size() - 1);
		return str;
	}
	else
		return "";
}

// --- Registry ------
std::string getStringFromReg(const std::string& folderName, const std::string& keyName);
void putStringToReg(const std::string& folderName, const std::string& keyName, const std::string& value);

// --- Formatting ------
std::string formatTimeWithHour(int timeMilis);
std::string formatTimeWithoutHour(int timeMilis);

#endif //__SYSTEM_UTIL_H__