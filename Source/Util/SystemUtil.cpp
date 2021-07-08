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

//If you wonder why we are converting SDL2 keys into VK_*, mostly to keep compatibility with controls.init and
//existing code

bool isPressed(uint32_t key) {
    if (!applicationHasFocus()) return false;

    //According to key type, use diff SDL2 methods
    SDL_Keycode keycode;
    switch (key) {
        case VK_LBUTTON:
        case VK_MBUTTON:
        case VK_RBUTTON:
            return SDL_GetMouseState(nullptr, nullptr) & key;
        case VK_SHIFT:
            return SDL_GetModState() & KMOD_SHIFT;
        case VK_CONTROL:
            return SDL_GetModState() & KMOD_CTRL;
        case VK_MENU:
            return SDL_GetModState() & KMOD_ALT;
        case VK_BACK:           keycode = SDLK_BACKSPACE; break;
        case VK_TAB:            keycode = SDLK_TAB; break;
        case VK_RETURN:         keycode = SDLK_RETURN; break;
        case VK_PAUSE:          keycode = SDLK_PAUSE; break;
        case VK_CAPITAL:        keycode = SDLK_CAPSLOCK; break;
        case VK_ESCAPE:         keycode = SDLK_ESCAPE; break;
        case VK_SPACE:          keycode = SDLK_SPACE; break;
        case VK_PRIOR:          keycode = SDLK_PAGEUP; break;
        case VK_NEXT:           keycode = SDLK_PAGEDOWN; break;
        case VK_END:            keycode = SDLK_END; break;
        case VK_HOME:           keycode = SDLK_HOME; break;
        case VK_LEFT:           keycode = SDLK_LEFT; break;
        case VK_UP:             keycode = SDLK_UP; break;
        case VK_RIGHT:          keycode = SDLK_RIGHT; break;
        case VK_DOWN:           keycode = SDLK_DOWN; break;
        case VK_INSERT:         keycode = SDLK_INSERT; break;
        case VK_DELETE:         keycode = SDLK_DELETE; break;
        case VK_F1:             keycode = SDLK_F1; break;
        case VK_F2:             keycode = SDLK_F2; break;
        case VK_F3:             keycode = SDLK_F3; break;
        case VK_F4:             keycode = SDLK_F4; break;
        case VK_F5:             keycode = SDLK_F5; break;
        case VK_F6:             keycode = SDLK_F6; break;
        case VK_F7:             keycode = SDLK_F7; break;
        case VK_F8:             keycode = SDLK_F8; break;
        case VK_F9:             keycode = SDLK_F9; break;
        case VK_F10:            keycode = SDLK_F10; break;
        case VK_F11:            keycode = SDLK_F11; break;
        case VK_F12:            keycode = SDLK_F12; break;
        case VK_TILDE:          keycode = SDLK_BACKSLASH; break;
        case VK_ADD:            keycode = SDLK_PLUS; break;
        case VK_SEPARATOR:      keycode = SDLK_SEPARATOR; break;
        case VK_SUBTRACT:       keycode = SDLK_MINUS; break;
        default:
#ifdef PERIMETER_DEBUG
            printf("Unknown VK keycode requested %u\n", key);
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

sKey::sKey(SDL_Keysym keysym, bool set_by_async_funcs) {
    fullkey = 0;
    switch (keysym.sym) {
        case SDLK_BACKSPACE:    fullkey = VK_BACK; break;
        case SDLK_TAB:          fullkey = VK_TAB; break;
        case SDLK_RETURN:       fullkey = VK_RETURN; break;
        case SDLK_LSHIFT:
        case SDLK_RSHIFT:       fullkey = VK_SHIFT; break;
        case SDLK_LCTRL:
        case SDLK_RCTRL:        fullkey = VK_CONTROL; break;
        case SDLK_LALT:
        case SDLK_RALT:         fullkey = VK_MENU; break;
        case SDLK_PAUSE:        fullkey = VK_PAUSE; break;
        case SDLK_CAPSLOCK:     fullkey = VK_CAPITAL; break;
        case SDLK_ESCAPE:       fullkey = VK_ESCAPE; break;
        case SDLK_SPACE:        fullkey = VK_SPACE; break;
        case SDLK_PAGEUP:       fullkey = VK_PRIOR; break;
        case SDLK_PAGEDOWN:     fullkey = VK_NEXT; break;
        case SDLK_END:          fullkey = VK_END; break;
        case SDLK_HOME:         fullkey = VK_HOME; break;
        case SDLK_LEFT:         fullkey = VK_LEFT; break;
        case SDLK_UP:           fullkey = VK_UP; break;
        case SDLK_RIGHT:        fullkey = VK_RIGHT; break;
        case SDLK_DOWN:         fullkey = VK_DOWN; break;
        case SDLK_INSERT:       fullkey = VK_INSERT; break;
        case SDLK_DELETE:       fullkey = VK_DELETE; break;
        case SDLK_F1:           fullkey = VK_F1; break;
        case SDLK_F2:           fullkey = VK_F2; break;
        case SDLK_F3:           fullkey = VK_F3; break;
        case SDLK_F4:           fullkey = VK_F4; break;
        case SDLK_F5:           fullkey = VK_F5; break;
        case SDLK_F6:           fullkey = VK_F6; break;
        case SDLK_F7:           fullkey = VK_F7; break;
        case SDLK_F8:           fullkey = VK_F8; break;
        case SDLK_F9:           fullkey = VK_F9; break;
        case SDLK_F10:          fullkey = VK_F10; break;
        case SDLK_F11:          fullkey = VK_F11; break;
        case SDLK_F12:          fullkey = VK_F12; break;
        case SDLK_BACKSLASH:    fullkey = VK_TILDE; break;
        case SDLK_PLUS:         fullkey = VK_ADD; break;
        case SDLK_SEPARATOR:    fullkey = VK_SEPARATOR; break;
        case SDLK_MINUS:        fullkey = VK_SUBTRACT; break;
        default:
            //Apparently game uses uppercase ASCII codes for keys
            uint8_t byte = keysym.sym & 0xFF;
            if (byte >= 'a' && byte <= 'z') {
                fullkey = toupper(byte);
            } else if (byte >= '0' && byte <= '9') {
                fullkey = byte;
            } else {
#ifdef PERIMETER_DEBUG
                printf("Unknown SDL key requested scan %d sym %d\n", keysym.scancode, keysym.sym);
#endif
            }
    }
    
    //Add modifiers
    auto mod = keysym.mod;
    if ((mod & KMOD_SHIFT) != 0) {
        fullkey |= KBD_SHIFT;
        shift |= 1;
    }
    if ((mod & KMOD_CTRL) != 0) {
        fullkey |= KBD_CTRL;
        ctrl |= 1;
    }
    if ((mod & KMOD_GUI) != 0) {
        fullkey |= KBD_MENU;
        menu |= 1;
    }
    
    // Same as normal sKey constructor

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
#endif

sKey::sKey(int fullkey_, bool set_by_async_funcs) {
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
