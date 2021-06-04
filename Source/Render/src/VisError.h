#ifndef __VISERROR_H__
#define __VISERROR_H__

#define VERR_END			"###"

class cVisError
{
public:
	cVisError& operator << (int a);
	cVisError& operator << (float a);
	cVisError& operator << (const char *a);
	cVisError& operator << (string& a);
private:
	string buf;
};
extern cVisError VisError;

#undef xassert_s
#ifndef _FINAL_VERSION_
#define xassert_s(exp, str) { string s = #exp; s += "\n"; s += str; xxassert(exp,s.c_str()); }
#else
#define xassert_s(exp, str) 
#endif

#endif //__VISERROR_H__