#include <string>
#include <list>
#include <cfloat>
#include <iostream>
#include "xstream.h"
#include "xerrhand.h"
#include "xglobal.h"

static void (*assert_restore_graphics_function)() = 0;

#if (!defined(_FINAL_VERSION_) || defined(_DEBUG)) && !defined(NASSERT)
void SetAssertRestoreGraphicsFunction(void(*func)())
{
	assert_restore_graphics_function = func;
}
#endif

void xtSysFinit(void);

#define CONV_BUFFER_LEN	63
char convBuf[CONV_BUFFER_LEN + 1];

#ifndef OPTION_DISABLE_STACKTRACE
#define BOOST_STACKTRACE_GNU_SOURCE_NOT_REQUIRED 1
#include "boost/stacktrace.hpp"
#endif

#ifndef _WIN32
#define APIENTRY
#endif

char *defprefix 	= "XHANDLER  INFORM";

char *ctrlMSG		= "CTRL-BREAK TERMINATION";
char *exceptMSG 	= "EXCEPTION OCCURED";
char *debugMSG		= "DEBUG PROHIBITION";
char *winopenMSG	= "WINDOW NOT OPEN";
char *winclassregMSG	= "WINDOW CLASS REGISTRATION ERROR";
char *threadcreatMSG	= "THREAD CREATION ERROR";
char *semaphcreatMSG	= "SEMAPHORE CREATION ERROR";
char *rterrorMSG	= "RUN-TIME ERROR";

#pragma warning (disable : 4073)
#pragma init_seg(lib)
XErrorHandler ErrH;

void win32_break(char* error,char* msg) {
    std::cerr << "--------------------------------\n";
    std::cerr << error << "\n";
    std::cerr << msg << "\n";
    std::cerr  << "--------------------------------" << std::endl;
    _ASSERT(FALSE) ;
}

#define HANDLE_EXEPT(string,mask)\
	{ if (ErrH.flags & (mask))\
		strcat(msg,(string));\
	    else\
		return EXCEPTION_CONTINUE_SEARCH;\
	  break; }

char* qwtoa(uint64_t a)
{
    sprintf(convBuf, "%llX", a);
    size_t len = 16 - strlen(convBuf);
    for(int i = 0; i < len;i++)
        convBuf[i] = '0';
    sprintf(convBuf + len, "%llX", a);
    return convBuf;
}
char* dwtoa(uint32_t a)
{
    sprintf(convBuf, "%X", a);
	size_t len = 8 - strlen(convBuf);
	for(int i = 0; i < len;i++)
		convBuf[i] = '0';
    sprintf(convBuf + len, "%X", a);
	return convBuf;
}
char* wtoa(uint16_t a)
{
    sprintf(convBuf, "%X", a);
    size_t len = 4 - strlen(convBuf);
    for(int i = 0; i < len;i++)
        convBuf[i] = '0';
    sprintf(convBuf + len, "%X", a);
	return convBuf;
}
char* uctoa(uint8_t a)
{
    sprintf(convBuf, "%X", a);
    size_t len = 2 - strlen(convBuf);
    if(len == 1)
        convBuf[0] = '0';
    sprintf(convBuf + len, "%X", a);
	return convBuf;
}


bool getStackTrace(std::list<std::string>& lines) {
#ifndef OPTION_DISABLE_STACKTRACE
    //Get current
    auto st = boost::stacktrace::stacktrace();
    //Check if failed to load
    if (st.empty()) {
        return false;
    }
    //Write lines
    for (size_t i = 0; i < st.size(); ++i) {
        //Just store the name instead of full name as we don't really care our own name
        if (i == 0) {
            lines.push_front(std::string() + __func__ + " <- last call");
            continue;
        }

        //Pull the stacktrace info
        std::string line = boost::stacktrace::detail::to_string(&st.as_vector()[i], 1);
        std::string::size_type size = line.size();
        if (10 <= size) {
            //Remove the start number and end newline
            line = line.substr(4, size - 5);
            lines.push_front(line);
        }
    }
#endif
    return !lines.empty();
}

long APIENTRY exHandler(EXCEPTION_POINTERS *except_info)
{
	_clearfp();
	_controlfp( _controlfp(0,0) & ~(0), _MCW_EM );
	SetUnhandledExceptionFilter(NULL);

	static char msg[10000];

	strcpy(msg, "");

	switch(except_info->ExceptionRecord->ExceptionCode)
	{
	 case EXCEPTION_INT_DIVIDE_BY_ZERO:
		 HANDLE_EXEPT("INTEGER DIVIDE ERROR",XERR_MATH)
	 case EXCEPTION_INT_OVERFLOW:
		 HANDLE_EXEPT("INTEGER OVERFLOW",XERR_MATH)
	 case EXCEPTION_ACCESS_VIOLATION:
		 HANDLE_EXEPT("MEMORY ACCESS VIOLATION",XERR_ACCESSVIOLATION)
	 case EXCEPTION_BREAKPOINT:
		 HANDLE_EXEPT("A BREAKPOINT WAS ENCOUNTERED",XERR_DEBUG)
	 case EXCEPTION_DATATYPE_MISALIGNMENT:
		 HANDLE_EXEPT("DATA TYPE MISALIGMENT",XERR_ACCESSVIOLATION)
	 case EXCEPTION_SINGLE_STEP:
		 HANDLE_EXEPT("TRACE TRAP",XERR_DEBUG)
	 case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
		 HANDLE_EXEPT("OUT OF BOUNDS FOR ARRAY",XERR_ACCESSVIOLATION)
	 case EXCEPTION_FLT_DENORMAL_OPERAND:
		 HANDLE_EXEPT("FLOATING POINT DENORMAL OPERAND",XERR_FLOAT)
	 case EXCEPTION_FLT_DIVIDE_BY_ZERO:
		 HANDLE_EXEPT("FLOATING POINT DIVIDE BY ZERO",XERR_FLOAT)
	 case EXCEPTION_FLT_INEXACT_RESULT:
		 HANDLE_EXEPT("FLOATING POINT INEXACT RESULT",XERR_FLOAT)
	 case EXCEPTION_FLT_INVALID_OPERATION:
		 HANDLE_EXEPT("FLOATING POINT INVALID OPERATION",XERR_FLOAT)
	 case EXCEPTION_FLT_OVERFLOW:
		 HANDLE_EXEPT("FLOATING POINT OVERFLOW",XERR_FLOAT)
	 case EXCEPTION_FLT_STACK_CHECK:
		 HANDLE_EXEPT("FLOATING POINT STACK CHECK FAILED",XERR_FLOAT)
	 case EXCEPTION_FLT_UNDERFLOW:
		 HANDLE_EXEPT("FLOATING POINT UNDERFLOW",XERR_FLOAT)
	 case EXCEPTION_PRIV_INSTRUCTION:
		 HANDLE_EXEPT("PRIVELEGED INSTRUCTION",XERR_ACCESSVIOLATION)
	 default:
		 HANDLE_EXEPT("UNKNOWN ERROR TYPE",XERR_UNKNOWN)
	}

	strcat(msg," AT LOCATION 0x");
#ifdef PERIMETER_ARCH_64
	strcat(msg, qwtoa(except_info->ExceptionRecord->ExceptionAddress));
#else
    strcat(msg, dwtoa(except_info->ExceptionRecord->ExceptionAddress));
#endif

	static int attempt_to_show_context = 0;
	if(!attempt_to_show_context){
		PCONTEXT p = except_info -> ContextRecord;
		if((p -> ContextFlags & CONTEXT_INTEGER) && (p -> ContextFlags & CONTEXT_CONTROL) &&
			(p -> ContextFlags & CONTEXT_CONTROL)){
#ifdef PERIMETER_ARCH_64
            attempt_to_show_context = 1;
            strcat(msg,"\r\n\r\nRegisters:\r\n");
            strcat(msg,"RAX="); strcat(msg, qwtoa(p -> Rax));
            strcat(msg,"  CS="); strcat(msg, wtoa(p -> SegCs));
            strcat(msg,"  RIP="); strcat(msg, qwtoa(p -> Rip));
            strcat(msg,"  EFLAGS="); strcat(msg, dwtoa(p -> EFlags));

            strcat(msg,"\r\nRBX="); strcat(msg, qwtoa(p -> Rbx));
            strcat(msg,"  SS="); strcat(msg, wtoa(p -> SegSs));
            strcat(msg,"  RSP="); strcat(msg, qwtoa(p -> Rsp));
            strcat(msg,"  RBP="); strcat(msg, qwtoa(p -> Rbp));

            strcat(msg,"\r\nRCX="); strcat(msg, qwtoa(p -> Rcx));
            strcat(msg,"  DS="); strcat(msg, wtoa(p -> SegDs));
            strcat(msg,"  RSI="); strcat(msg, qwtoa(p -> Rsi));
            strcat(msg,"  FS="); strcat(msg, wtoa(p -> SegFs));

            strcat(msg,"\r\nRDX="); strcat(msg, qwtoa(p -> Rdx));
            strcat(msg,"  ES="); strcat(msg, wtoa(p -> SegEs));
            strcat(msg,"  RDI="); strcat(msg, qwtoa(p -> Rdi));
            strcat(msg,"  GS="); strcat(msg, wtoa(p -> SegGs));

            strcat(msg,"\r\n\r\nBytes at CS::RIP:\r\n");
            unsigned char* code = (unsigned char*)(p -> Rip);
            for(int i = 0;i < 16;i++){
                strcat(msg, uctoa(code[i]));
                strcat(msg," ");
            }

            strcat(msg,"\r\n\r\nStack dump:\r\n");
            unsigned int* stack = (unsigned int*)(p -> Rsp);
            for(int i = 0;i < 32;i++){
                strcat(msg, qwtoa(stack[i]));
                strcat(msg, (i & 7) == 7 ? "\r\n" : " ");
            }
#else
			attempt_to_show_context = 1;
			strcat(msg,"\r\n\r\nRegisters:\r\n");
			strcat(msg,"EAX="); strcat(msg, dwtoa(p -> Eax));
			strcat(msg,"  CS="); strcat(msg, wtoa(p -> SegCs));
			strcat(msg,"  EIP="); strcat(msg, dwtoa(p -> Eip));
			strcat(msg,"  EFLAGS="); strcat(msg, dwtoa(p -> EFlags));

			strcat(msg,"\r\nEBX="); strcat(msg, dwtoa(p -> Ebx));
			strcat(msg,"  SS="); strcat(msg, wtoa(p -> SegSs));
			strcat(msg,"  ESP="); strcat(msg, dwtoa(p -> Esp));
			strcat(msg,"  EBP="); strcat(msg, dwtoa(p -> Ebp));

			strcat(msg,"\r\nECX="); strcat(msg, dwtoa(p -> Ecx));
			strcat(msg,"  DS="); strcat(msg, wtoa(p -> SegDs));
			strcat(msg,"  ESI="); strcat(msg, dwtoa(p -> Esi));
			strcat(msg,"  FS="); strcat(msg, wtoa(p -> SegFs));

			strcat(msg,"\r\nEDX="); strcat(msg, dwtoa(p -> Edx));
			strcat(msg,"  ES="); strcat(msg, wtoa(p -> SegEs));
			strcat(msg,"  EDI="); strcat(msg, dwtoa(p -> Edi));
			strcat(msg,"  GS="); strcat(msg, wtoa(p -> SegGs));

			strcat(msg,"\r\n\r\nBytes at CS::EIP:\r\n");
			unsigned char* code = (unsigned char*)(p -> Eip);
			for(int i = 0;i < 16;i++){
				strcat(msg, uctoa(code[i]));
				strcat(msg," ");
            }

			strcat(msg,"\r\n\r\nStack dump:\r\n");
			unsigned int* stack = (unsigned int*)(p -> Esp);
			for(int i = 0;i < 32;i++){
				strcat(msg, dwtoa(stack[i]));
				strcat(msg, (i & 7) == 7 ? "\r\n" : " ");
            }
#endif

            std::list<std::string> linesStackTrace;
            strcat(msg,"\r\nCall stack:\r\n");
            if (getStackTrace(linesStackTrace)) {
                for (std::string line : linesStackTrace) {
                    strcat(msg, line.c_str());
                    strcat(msg, "\r\n");
                }
            } else {
                strcat(msg,"Not available\r\n");
            }
        }
    }


	ErrH.WriteLog(exceptMSG, msg);

#if defined(_DEBUG) || defined(_EXCEPTION_CATCH)
	win32_break(exceptMSG,msg);
	return EXCEPTION_CONTINUE_EXECUTION;
#else
	ErrH.Abort(exceptMSG,XERR_USER,-1,msg);
	return EXCEPTION_EXECUTE_HANDLER;
#endif
}


XErrorHandler::XErrorHandler() {
	prefix		= defprefix;
	postfix 	= NULL;
	restore_func	= 0;
	state		= 0;
	flags		= XERR_ALL;

	SetUnhandledExceptionFilter((LPTOP_LEVEL_EXCEPTION_FILTER)&exHandler);
}

XErrorHandler::~XErrorHandler() {
	if ((!state) && postfix)
		MessageBox(0,postfix,prefix,MB_OK);
}

void XErrorHandler::SetFlags(unsigned f)
{
	flags |= f;
}

void XErrorHandler::ClearFlags(unsigned f)
{
	flags ^= f;
}

void XErrorHandler::Abort(const char* message, int code, int val, const char* subj)
{
    /* TODO
    this originally displayed a Dialog with stacktrace to debug the issue that caused Abort
    Vangers has SDL2 dialog impl for this, we should do the same

    string text = "";
    text += sprintf("XErrH::Abort!\nMessage %s\nCode %d Val %d\n", message, code, addval);
    if (subj) {
        text += sprintf("Subject %s\n", subj);
    }
    */

	static char outmsg[10000]="";
	UINT attr=MB_OK;

	if(restore_func){
		restore_func();
		restore_func = 0;
	}

	strcpy(outmsg,message);
	if(subj){
		strcat(outmsg,"\r");
		strcat(outmsg,subj);
	}
	if(val){
		attr|=MB_ICONSTOP;
		if(val != -1){
			strcat(outmsg,"\r");
			strcat(outmsg,"CODE: 0x");
            sprintf(convBuf, "%X", val);
			strcat(outmsg, convBuf);
		}
	}

	state = 1;

	MessageBox(NULL,outmsg,prefix,attr | MB_TOPMOST | MB_SYSTEMMODAL);
    fprintf(stderr, outmsg);
	exit(code);
}

void XErrorHandler::Exit(void)
{
	if(restore_func){
		restore_func();
		restore_func = 0;
	}
	exit(0);
}

unsigned XErrorHandler::InfoFlags(unsigned f)
{
	return flags & f;
}

void XErrorHandler::SetPrefix(const char* s)
{
	prefix = s;
}

void XErrorHandler::SetPostfix(const char* s)
{
	postfix = s;
}

void XErrorHandler::SetRestore(void (*rf)(void))
{
	restore_func = rf;
}

void XErrorHandler::RTC(const char *file, unsigned int line, const char *expr)
{
	char msg[256];
	strcpy(msg, file);
	strcat(msg,", LINE: ");
    sprintf(convBuf, "%d", line);
    strcat(msg, convBuf);
    strcat(msg,"\r\n");
    strcat(msg,expr);
	Abort(rterrorMSG,XERR_USER,-1,msg);
}

void XErrorHandler::WriteLog(char* error, char* msg)
{
	static XStream ErrHStream;
	ErrHStream.open("XErrH.log",XS_OUT);
	ErrHStream < "--------------------------------\r\n";
	ErrHStream < prefix < "\r\n\r\n\r\n";

	//if(assertsBuffer.tell())
	//	ErrHStream < assertsBuffer < "\r\n";

	ErrHStream < error < "\r\n";
	ErrHStream < msg < "\r\n";
	ErrHStream < "--------------------------------\r\n";
	ErrHStream.close();
}

