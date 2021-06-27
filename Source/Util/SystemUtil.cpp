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
const char* IniManager::get(const char* section, const char* key)
{
	static char buf[256];
	static char path[MAX_PATH];

	if(_fullpath(path,fname_,MAX_PATH) == NULL)
		ErrH.Abort("Ini file not found: ", XERR_USER, 0, fname_);
	if(!GetPrivateProfileString(section,key,NULL,buf,256,path)){
		*buf = 0;
		xassert_s(!check_existence_, (string("Не определен ключ ") + fname_ + " " + section + " " + key).c_str());
	}

	return buf;
}
void IniManager::put(const char* section, const char* key, const char* val)
{
	static char path[MAX_PATH];

	if(_fullpath(path,fname_,MAX_PATH) == NULL) {
		ErrH.Abort("Ini file not found: ", XERR_USER, 0, fname_);
	}

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
