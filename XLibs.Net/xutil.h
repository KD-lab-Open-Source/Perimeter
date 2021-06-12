/*
******************************************************************************
**									    **
**		     XTOOL	Windows 32 API version 3.0nt		    **
**			   for Windows-NT, Windows 95			    **
**									    **
**	  (C)1993-97 by K-D Lab::KranK, K-D Lab::Steeler, Dr.Tronick	    **
**									    **
******************************************************************************
*/

#ifndef __XUTIL_H
#define __XUTIL_H



///////////////////////////////////
//		XStream
///////////////////////////////////


///////////////////////////////////
//		ErrH
///////////////////////////////////

#ifndef NULL
#define NULL	0L
#endif

#define X_WINNT 		0x0001
#define X_WIN32S		0x0002
#define X_WIN95 		0x0003

#define XERR_ALL		0xFFFF
#define XERR_NONE		0x0000

#define XERR_USER		0x0001
#define XERR_CTRLBRK		0x0004
#define XERR_MATH		0x0008
#define XERR_FLOAT		0x0010
#define XERR_CRITICAL		0x0020
#define XERR_SIGNAL		0x0040

#define XERR_ACCESSVIOLATION	0x0080
#define XERR_DEBUG		0x0100
#define XERR_UNKNOWN		0x0200

struct XErrorHandler
{
#ifndef _FINAL_VERSION_
	unsigned flags;
	unsigned state;

	const char* prefix;
	const char* postfix;
	bool flag_errorOrAssertHandling;

	void (*restore_func)(void);

		 XErrorHandler(void);
		~XErrorHandler(void);

	void	 SetFlags(unsigned f);
	void	 ClearFlags(unsigned f);
	unsigned InfoFlags(unsigned f);

	void	 SetPrefix(const char* s);
	void	 SetPostfix(const char* s);
	void	 SetRestore(void (*rf)(void));
	void	 SetState(int s){state=s;}
	
	void	 RTC(const char *file,unsigned int line, const char *expr);
	void	 WriteLog(char* error, char* msg);
	bool     IsErrorOrAssertHandling(){ return flag_errorOrAssertHandling; }
#endif
	void	 Abort(const char* message, int code = XERR_USER, int addval = -1, const char* subj = NULL);
	void	 Exit(void);
};

static XErrorHandler ErrH;

// Use this macro for after any operation for errors diagnostic
//#define XAssert(expr) ErrH.RTC(__FILE__,__LINE__,expr)
#define XAssert(expr)


#if (!defined(_FINAL_VERSION_) || defined(_DEBUG)) && !defined(NASSERT)

// Use d3dFlipToGdiSurface() for D3D Fullscreen modes
void SetAssertRestoreGraphicsFunction(void(*func)());

int DiagAssert(unsigned long dwOverrideOpts, const char* szMsg, const char* szFile, unsigned long dwLine ) ;

#undef NDEBUG
#define xxassert(exp, msg) \
    do                                                              \
    {  static int ignore = 0;                                   \
        if ( !(exp) && !ignore)				\
		switch(DiagAssert ( 0 ,  msg, __FILE__  , __LINE__)){  \
			case 1: ignore = 1; break; \
			case 2: __asm { int 3 }; break; \
			}\
    } while (0)

#define xassert(exp) xxassert(exp, #exp)

#else  //  ...

#define SetAssertRestoreGraphicsFunction(func)
#define xxassert(exp, msg) 
#define xassert(exp) 

#endif  //  ...

#undef assert
#define assert(x)	xassert(x)


///////////////////////////////////
//		Utils
///////////////////////////////////

void* xalloc(unsigned sz);
void* xrealloc(void* p,unsigned sz);
void  xfree(void* p);

#define XALLOC(a)	xalloc(a)
#define XREALLOC(a,b)	xrealloc(a,b)
#define XFREE(a)	xfree(a)

unsigned int XRnd(unsigned int m);
void XRndSet(unsigned int m);
unsigned int XRndGet();

char* XFindNext(void);
char* XFindFirst(char* mask);

#ifndef M_PI
#define M_PI	3.14159265358979323846
#endif

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
__forceinline T sqr(const T& x){ return x*x; }

template <class T> 
__forceinline int SIGN(const T& x) { return x ? (x > 0 ? 1 : -1 ) : 0; }

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

void initclock(void);
int clocki(void);
double clockf(void);

//void xtDeleteFile(char* fname);

const char* check_command_line(const char* switch_str); // 0 или строка после ключа для анализа


///////////////////////////////////
//		__XCPUID_H
///////////////////////////////////

/*
#define CPU_INTEL		0x00000001

// Intel features IDs

#define INTEL_MMX		0x00800000

// Intel Family IDs
#define CPU_INTEL_386		3
#define CPU_INTEL_486		4
#define CPU_INTEL_PENTIUM	5
#define CPU_INTEL_PENTIUM_PRO	6

//Intel Pentium model's
#define PENTIUM_P5A		0
#define PENTIUM_P5		1
#define PENTIUM_P54C		2
#define PENTIUM_P54T		3
#define PENTIUM_MMX		4
#define PENTIUM_DX4		5
#define PENTIUM_P5OVER		6
#define PENTIUM_P54CA		7

// Intel Pentium Pro model's
#define PENTIUM_PROA		0
#define PENTIUM_PRO		1
#define PENTIUM_PRO2		3
#define PENTIUM_PRO55CT 	4
#define PENTIUM_PROF		5

extern unsigned int xt_processor_id;
extern unsigned int xt_processor_type;
extern unsigned int xt_processor_family;
extern unsigned int xt_processor_model;
extern unsigned int xt_processor_revision;
extern unsigned int xt_processor_features;

unsigned int xt_get_cpuid(void);
char* xt_getMMXstatus(void);

extern int xt_mmxUse;

///////////////////////////////////
//		__XZIP_H__
///////////////////////////////////

typedef unsigned long ulong;

ulong ZIP_compress(char* trg,ulong trgsize,char* src,ulong srcsize);
ulong ZIP_GetExpandedSize(char* p);
void ZIP_expand(char* trg,ulong trgsize,char* src,ulong srcsize);

*/

///////////////////////////////////
//		Automatic linking
///////////////////////////////////
#ifndef _XUTIL_NO_AUTOMATIC_LIB

#if defined(_MT)
#define _MT_SUFFIX "Mt"
#elif defined(_MTD)
#define _MT_SUFFIX "MtDll"
#else
#define _MT_SUFFIX 
#endif//_MT

#ifdef _DEBUG
#define _DEBUG_SUFFIX "Dbg"
#else //_DEBUG
#define _DEBUG_SUFFIX 
#endif //_DEBUG

#if (_MSC_VER == 1200)
# define _VC_SUFFIX ""
#elif (_MSC_VER >= 1300 && _MSC_VER < 1400)
# define _VC_SUFFIX "VC7"
#elif (_MSC_VER >= 1400)
# define _VC_SUFFIX "VC8"
#else
#define _VC_SUFFIX ""
#endif

#ifndef __XTOOL_H
#define _LIB_NAME "XUtil" _VC_SUFFIX _DEBUG_SUFFIX _MT_SUFFIX ".lib"
#else
#define _LIB_NAME "XTool" _VC_SUFFIX _DEBUG_SUFFIX _MT_SUFFIX ".lib"
#endif

#ifndef NDEBUG
#pragma message("Automatically linking with " _LIB_NAME)
#pragma comment(lib, _LIB_NAME)
#endif

#undef _VC_SUFFIX
#undef _DEBUG_SUFFIX
#undef _MT_SUFFIX
#undef _LIB_NAME

#endif // _XUTIL_NO_AUTOMATIC_LIB

#endif // __XUTIL_H
