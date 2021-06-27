
#ifndef __XUTL_H
#define __XUTL_H

unsigned int XRnd(unsigned int m);
void XRndSet(unsigned int m);
unsigned int XRndGet();

#ifndef __ROUND__
#define __ROUND__

// Modern compilers already have std::round
#if defined(_MSC_VER) && (_MSC_VER < 1900)

__forceinline int round(double x)
{
	int a;
	_asm {
		fld x
		fistp dword ptr a
	}
	return a;
}

__forceinline int round(float x)
{
	int a;
	_asm {
		fld x
		fistp dword ptr a
	}
	return a;
}

#endif

template <class T> 
FORCEINLINE T sqr(const T& x){ return x * x; }

template <class T> 
FORCEINLINE int SIGN(const T& x) { return x ? (x > 0 ? 1 : -1 ) : 0; }

#endif //__ROUND__

/*
__forceinline int BitSR(int x)
{
	int return_var;
	_asm {
		mov eax, x
		cdq
		xor eax,edx
		sub     eax,edx
		bsr     eax,eax
		mov [return_var],eax
	}
	return return_var;
}
*/

// Copied C version from Vangers
// TODO(amdmi3): very inefficient; rewrite
inline int BitSR(int x) {
    unsigned int a = abs(x);
    for (int i = 31; i > 0; i--)
        if (a & (1 << i))
            return i;

    return 0;
}

void initclock();
int clocki();
double clockf();

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef PERIMETER_EXODUS
#include <vector>
//Hacky way to "store" argc/argv so they can be accessed later like in windows
static int __argc = 0;
static std::vector<char*> __argv;
#endif

inline const char* check_command_line(const char* switch_str)
{
    for(int i = 1; i < __argc; i ++){
        const char* s = strstr(__argv[i], switch_str);
        if(s)
            return s += strlen(switch_str);
    }
    return nullptr;
}

bool MessageBoxQuestion(const char* title, const char* message, uint32_t flags = 0);

#endif
