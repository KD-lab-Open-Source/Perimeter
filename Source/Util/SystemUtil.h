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
#define VK_TILDE	0xC0

#ifndef WM_MOUSEWHEEL
#define WM_MOUSEWHEEL (WM_MOUSELAST + 1)
#endif

bool applicationHasFocus();
bool applicationIsGo();

inline bool isPressed(int key) { return applicationHasFocus() && (GetAsyncKeyState(key) & 0x8000); } 
inline bool isShiftPressed() { return isPressed(VK_SHIFT); }
inline bool isControlPressed() { return isPressed(VK_CONTROL); }
inline bool isAltPressed() { return isPressed(VK_MENU); }

const unsigned int KBD_CTRL = 1 << 8;
const unsigned int KBD_SHIFT = 1 << 9;
const unsigned int KBD_MENU = 1 << 10;

struct sKey
{
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

	sKey(int fullkey_ = 0, bool set_by_async_funcs = false) { 
		fullkey = fullkey_;
		if(set_by_async_funcs){ 
			ctrl = isControlPressed(); 
			shift = isShiftPressed(); 
			menu = isAltPressed(); 
		} 
		// добавляем расширенные коды для командных кодов
		if(key == VK_CONTROL) 
			ctrl |= 1;
		if(key == VK_SHIFT)
			shift |= 1;
		if(key == VK_MENU)
			menu |= 1;
	}
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
	const char* fname_;
	bool check_existence_;
public:
	IniManager(const char* fname, bool check_existence = true) { fname_ = fname; check_existence_ = check_existence; }
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
inline string setExtention(const char* file_name, const char* extention)
{
	string str = file_name;
	unsigned int pos = str.rfind(".");
	if(pos != string::npos)
		str.erase(pos, str.size());
	return str + "." + extention;
}

inline string getExtention(const char* file_name)
{
	string str = file_name;
	unsigned int pos = str.rfind(".");
	if(pos != string::npos){
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
string getStringFromReg(const string& folderName, const string& keyName);
void putStringToReg(const string& folderName, const string& keyName, const string& value);

// --- Formatting ------
string formatTimeWithHour(int timeMilis);
string formatTimeWithoutHour(int timeMilis);

#endif //__SYSTEM_UTIL_H__