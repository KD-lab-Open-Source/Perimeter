#ifndef __VISERROR_H__
#define __VISERROR_H__

#define VERR_END			"###"

class cVisError
{
public:
	cVisError& operator << (int a);
	cVisError& operator << (float a);
	cVisError& operator << (const char *a);
	cVisError& operator << (std::string& a);
private:
	std::string buf;
};
extern cVisError VisError;

#undef xassert_s
#ifdef PERIMETER_DEBUG_ASSERT
#define xassert_s(exp, str) { std::string s = #exp; s += "\n"; s += str; xxassert(exp,s.c_str()); }
#else
#define xassert_s(exp, str) 
#endif

#endif //__VISERROR_H__