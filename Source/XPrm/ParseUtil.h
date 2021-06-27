
#pragma once
#pragma warning(disable:4305 4250)

#include <filesystem>

/////////////////////////////////////////////////////////////////////////////////////////////
//		Exceptions
/////////////////////////////////////////////////////////////////////////////////////////////
class parsing_error : public std::logic_error 
{
public:
	parsing_error(const char* __s) : logic_error(__s) {}
};

class duplicate_token : public parsing_error 
{
public:
  duplicate_token(const char* __s) : parsing_error(__s) {}
};

class unexpected_token : public parsing_error 
{
public:
  unexpected_token(const char* __s) : parsing_error(__s) {}
};

class expected_token : public parsing_error 
{
public:
  expected_token(const char* __s) : parsing_error(__s) {}
};


//////////////////////////////////////////////////////////////////////////////////
//		File Util
//////////////////////////////////////////////////////////////////////////////////
inline FILETIME getFileTime(const char* fname, FILETIME* last_write)
{
    auto ftime = std::filesystem::last_write_time(fname);
    auto duration = ftime.time_since_epoch();
    int64_t nanos = std::chrono::duration_cast<std::chrono::nanoseconds>(duration).count();
    EpochToFileTime(nanos, last_write);
}
inline bool operator < (const FILETIME& t1, const FILETIME& t2) { return t1.dwHighDateTime != t2.dwHighDateTime ? t1.dwHighDateTime < t2.dwHighDateTime : t1.dwLowDateTime < t2.dwLowDateTime; } 
inline bool operator <= (const FILETIME& t1, const FILETIME& t2) { return t1.dwHighDateTime != t2.dwHighDateTime ? t1.dwHighDateTime <= t2.dwHighDateTime : t1.dwLowDateTime <= t2.dwLowDateTime; } 
inline bool operator == (const FILETIME& t1, const FILETIME& t2) { return t1.dwHighDateTime == t2.dwHighDateTime && t1.dwLowDateTime == t2.dwLowDateTime; } 

inline void replace(std::string& s, const char* src, const char* dest)
{
	int pos = 0;
	while(1){
		pos = s.find(src, pos);
		if(pos >= s.size())
			break;
		s.replace(pos, strlen(src), dest);
		pos += strlen(dest);
		}
}

inline std::string expand_spec_chars(const std::string& str)
{
	std::string s(str);
	replace(s, "\\", "\\\\");
	replace(s, "\n", "\\n");
	replace(s, "\r", "\\r");
	replace(s, "\a", "\\a");
	replace(s, "\t", "\\t");
	replace(s, "\v", "\\v");
	return s;
}

inline std::string& collapse_spec_chars(std::string& s)
{
	int pos = 0;
	while(1){
		pos = s.find("\\", pos);
		if(pos >= s.size() - 1)
			break;
		char* dest;
		switch(s[pos + 1]){
			case '\\':
				dest = "\\";
				break;
			case 'n':
				dest = "\n";
				break;
			case 'r':
				dest = "\r";
				break;
			case 'a':
				dest = "\a";
				break;
			case 't':
				dest = "\t";
				break;
			case 'v':
				dest = "\v";
				break;

			default:
				throw parsing_error("Unknown special character");
			}

		s.replace(pos, 2, dest);
		pos += strlen(dest);
		}

	return s;
}


inline int is_name(const char* str)
{
	return isalpha(str[0]) || str[0] == '_';
}

inline std::string get_exe_path()
{
    std::string exe_path = __argv[0];
	int pos = exe_path.rfind("\\") + 1;
	exe_path.erase(pos, exe_path.size() - pos);
	return exe_path;
}

inline unsigned CRC(const char* str, unsigned crc)
{
	if(!str)
		return crc;
	while(*str){
		unsigned x = *str;
		crc ^= x >> 3;
		crc ^= x << 28;
		crc ^= crc >> 3;
		crc ^= crc << 28;
		str++;
		} 
	return crc;
}

inline unsigned CRC(unsigned x, unsigned crc)
{
	crc ^= x >> 3;
	crc ^= x << 28;
	crc ^= crc >> 3;
	crc ^= crc << 28;
	return crc;
}

inline std::string strip_string(const char* str)
{
    std::string parameter(str);
	parameter.erase(0, 1);
	parameter.pop_back();
	return parameter;
}

inline std::string strip_file_name(const char* str)
{
    std::string name(str);
	int pos = name.rfind("\\");
	if(pos != std::string::npos)
		name.erase(0, pos + 1);
	return name;
}

