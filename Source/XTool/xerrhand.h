#ifndef _ERRH_H
#define _ERRH_H

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
	unsigned flags;
	unsigned state;

	const char* prefix;
	const char* postfix;

	void (*restore_func)();

     XErrorHandler();
    ~XErrorHandler();

	void	 SetFlags(unsigned f);
	void	 ClearFlags(unsigned f);
	unsigned InfoFlags(unsigned f);

	void	 SetPrefix(const char* s);
	void	 SetPostfix(const char* s);
	void	 SetRestore(void (*rf)());
	void	 SetState(int s){state=s;}
	void	 Abort(const char* message, int code = XERR_USER, int addval = -1, const char* subj = NULL);
	void	 Exit();
	void	 RTC(const char *file,unsigned int line, const char *expr);
	void	 WriteLog(char* error, char* msg);
};

extern XErrorHandler ErrH;

// Use this macro for after any operation for errors diagnostic
#define XAssert(expr) ErrH.RTC(__FILE__,__LINE__,expr)


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

#endif /* _ERRH_H */
