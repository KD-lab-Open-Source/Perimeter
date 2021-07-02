#include "StdAfx.h"
#include <SDL.h>

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

#ifdef PERIMETER_EXODUS_WINDOW
bool isPressed(uint32_t key) {
    if (!applicationHasFocus()) return false;

    //According to key type, use diff SDL2 methods
    switch (key) {
        case VK_LBUTTON:
        case VK_MBUTTON:
        case VK_RBUTTON:
            return SDL_GetMouseState(nullptr, nullptr) & key;
        case VK_SHIFT:
        case VK_CONTROL:
        case VK_MENU:
            return SDL_GetModState() & key;
        default:
            SDL_Keycode keycode;
            switch (key) {
                case VK_BACK:    keycode = SDLK_BACKSPACE; break;
                case VK_RETURN:  keycode = SDLK_RETURN; break;
                case VK_SHIFT:   keycode = SDLK_LSHIFT; break;
                case VK_CONTROL: keycode = SDLK_LCTRL; break;
                case VK_MENU:    keycode = SDLK_MENU; break;
                case VK_PAUSE:   keycode = SDLK_PAUSE; break;
                case VK_ESCAPE:  keycode = SDLK_ESCAPE; break;
                case VK_SPACE:   keycode = SDLK_SPACE; break;
                case VK_INSERT:  keycode = SDLK_INSERT; break;
                case VK_DELETE:  keycode = SDLK_DELETE; break;
                case VK_F1:      keycode = SDLK_F1; break;
                case VK_F2:      keycode = SDLK_F2; break;
                case VK_F3:      keycode = SDLK_F3; break;
                case VK_F4:      keycode = SDLK_F4; break;
                case VK_F5:      keycode = SDLK_F5; break;
                case VK_F6:      keycode = SDLK_F6; break;
                case VK_F7:      keycode = SDLK_F7; break;
                case VK_F8:      keycode = SDLK_F8; break;
                case VK_F9:      keycode = SDLK_F9; break;
                case VK_F11:     keycode = SDLK_F10; break;
                case VK_F12:     keycode = SDLK_F11; break;
                case VK_TILDE:   keycode = SDLK_BACKSLASH; break;
                default:
#ifdef PERIMETER_DEBUG_ASSERT
                    ErrH.Abort("Unknown key requested", XERR_USER, key);
#endif
                    return false;
            }

            //Get state of keys
            int numkeys;
            const Uint8 *state = SDL_GetKeyboardState(&numkeys);

            //Convert VK to scancode and return state
            SDL_Scancode scancode = SDL_GetScancodeFromKey(keycode);
            if (scancode >= numkeys) return false;
            return state[scancode];
    }
}
#endif

/////////////////////////////////////////////////////////////////////////////////
//		File find
/////////////////////////////////////////////////////////////////////////////////

#ifdef PERIMETER_EXODUS
//TODO maybe we should refactor this, storing search data as static doesnt sound nice
const char* win32_findnext() {
    //TODO
    return nullptr;
}
const char* win32_findfirst(const char* mask) {
    //TODO
    return nullptr;
}
#else
static WIN32_FIND_DATA FFdata;
static HANDLE FFh;

const char* win32_findnext()
{
	if(FindNextFile(FFh,&FFdata) == TRUE){
		//if(FFdata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) return win32_findnext();
		return FFdata.cFileName;
		}
	else {
		FindClose(FFh);
		return NULL;
		}
}

const char* win32_findfirst(const char* mask)
{
	FFh = FindFirstFile(mask,&FFdata);
	if(FFh == INVALID_HANDLE_VALUE) return NULL;
	//if(FFdata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) return win32_findnext();
	return FFdata.cFileName;
}
#endif

// ---   Ini file   ---------------------
IniManager::IniManager(const char* fname, bool check_existence) {
    fname_ = fname;
    check_existence_ = check_existence;
}

const char* IniManager::get(const char* section, const char* key)
{
	static char buf[256];
    static char path[MAX_PATH];
    
    std::string pathres = convert_path_resource(fname_.c_str());
    if(pathres.empty())
        ErrH.Abort("Ini file not found: ", XERR_USER, 0, fname_.c_str());

    //GetPrivateProfileString needs full path
    if(_fullpath(path, pathres.c_str(), MAX_PATH) == NULL)
        ErrH.Abort("Ini file not found: ", XERR_USER, 0, pathres.c_str());
    
	if(!GetPrivateProfileString(section,key,NULL,buf,256,path)){
		*buf = 0;
		if (check_existence_) {
            fprintf(stderr, "INI key not found %s %s %s\n", path, section, key);
        }
	}

	return buf;
}
void IniManager::put(const char* section, const char* key, const char* val)
{
    static char path[MAX_PATH];

    std::string pathres = convert_path_resource(fname_.c_str());
    if(pathres.empty())
        ErrH.Abort("Ini file not found: ", XERR_USER, 0, fname_.c_str());

    //WritePrivateProfileString needs full path
    if(_fullpath(path, pathres.c_str(), MAX_PATH) == NULL)
        ErrH.Abort("Ini file not found: ", XERR_USER, 0, pathres.c_str());

	WritePrivateProfileString(section,key,val,path);
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

std::string getStringFromReg(const std::string& folderName, const std::string& keyName) {
    //TODO we should read this in some file
	std::string res;
#ifndef PERIMETER_EXODUS
	HKEY hKey;
	char name[PERIMETER_CONTROL_NAME_SIZE];
	DWORD nameLen = PERIMETER_CONTROL_NAME_SIZE;
	LONG lRet;

	lRet = RegOpenKeyEx( HKEY_CURRENT_USER, folderName.c_str(), 0, KEY_QUERY_VALUE, &hKey );

	if ( lRet == ERROR_SUCCESS ) {
		lRet = RegQueryValueEx( hKey, keyName.c_str(), NULL, NULL, (LPBYTE) name, &nameLen );

		if ( (lRet == ERROR_SUCCESS) && nameLen && (nameLen <= PERIMETER_CONTROL_NAME_SIZE) ) {
			res = name;
		}

		RegCloseKey( hKey );
	}
#endif
	return res;
}
void putStringToReg(const std::string& folderName, const std::string& keyName, const std::string& value) {
	//TODO we should store this in some file
#ifndef PERIMETER_EXODUS
	HKEY hKey;
	DWORD dwDisposition;
	LONG lRet;
	
	lRet = RegCreateKeyEx( HKEY_CURRENT_USER, folderName.c_str(), 0, "", 0, KEY_ALL_ACCESS, NULL, &hKey, &dwDisposition );

	if ( lRet == ERROR_SUCCESS ) {
		lRet = RegSetValueEx( hKey, keyName.c_str(), 0, REG_SZ, (LPBYTE) (value.c_str()), value.length() );

		RegCloseKey( hKey );
	}
#endif
}

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
