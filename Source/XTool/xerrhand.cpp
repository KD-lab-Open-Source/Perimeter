#include <list>
#include <cfloat>
#include <iostream>
#include <sstream>
#ifdef _WIN32
#include <windows.h>
#include <process.h>
#include <crtdbg.h>
#endif
#include "xstream.h"
#include "xerrhand.h"
#include <SDL.h>
#include <csignal>

static void (*assert_restore_graphics_function)() = 0;

#if (!defined(_FINAL_VERSION_) || defined(_DEBUG)) && !defined(NASSERT)
void SetAssertRestoreGraphicsFunction(void(*func)())
{
	assert_restore_graphics_function = func;
}
#endif

#define CONV_BUFFER_LEN	63
char convBuf[CONV_BUFFER_LEN + 1];

#ifndef OPTION_DISABLE_STACKTRACE
#define BOOST_STACKTRACE_GNU_SOURCE_NOT_REQUIRED 1
#include "boost/stacktrace.hpp"
#endif

#ifndef _WIN32
#define APIENTRY
#endif

typedef void (*sighandler) (int);
std::string lastException;
std::terminate_handler originalTerminateHandler = nullptr;

char *defprefix 	= "XHANDLER  INFORM";
char *exceptMSG 	= "EXCEPTION OCCURED";
char *rterrorMSG	= "RUN-TIME ERROR";

#pragma warning (disable : 4073)
#pragma init_seg(lib)
XErrorHandler ErrH;

//All the Win32 specific error handling
#ifdef _WIN32
void win32_break(char* error,char* msg) {
    std::cerr << "--------------------------------\n";
    std::cerr << error << "\n";
    std::cerr << msg << "\n";
    std::cerr  << "--------------------------------" << std::endl;
    _ASSERT(FALSE) ;
}

#define HANDLE_EXEPT(string,mask) { strcat(msg,(string)); break; }

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

long APIENTRY exHandler(EXCEPTION_POINTERS *except_info)
{
    fprintf(stderr, "exHandler\n");
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
	strcat(msg, qwtoa((uint64_t) except_info->ExceptionRecord->ExceptionAddress));
#else
    strcat(msg, dwtoa((uint32_t) except_info->ExceptionRecord->ExceptionAddress));
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
        }
    }

#if defined(_DEBUG) || defined(_EXCEPTION_CATCH)
	win32_break(exceptMSG,msg);
	return EXCEPTION_CONTINUE_EXECUTION;
#else
	ErrH.Abort(exceptMSG,XERR_USER,-1,msg);
	return EXCEPTION_EXECUTE_HANDLER;
#endif
}
#endif //_WIN32

void setSignalHandler(sighandler signalHandler) {
    signal(SIGSEGV, signalHandler);
    signal(SIGABRT, signalHandler);
    signal(SIGFPE, signalHandler);
    signal(SIGILL, signalHandler);
    signal(SIGTERM, signalHandler);
#ifdef SIGBUS //UNIX specific
    signal(SIGBUS, signalHandler);
#endif
}

void setTerminateHandler(std::terminate_handler terminateHandler) {
    std::set_terminate(terminateHandler);
}

void restoreSignalHandler() {
    setSignalHandler(SIG_DFL);
}

void handleTerminate() {
    fprintf(stderr, "handleTerminate\n");

    raise(SIGTERM);
}

void handleSignal(int sig) {
    fprintf(stderr, "handleSignal\n");
    //We don't want recursive signal handler calls
    restoreSignalHandler();

    //Get signal name
    std::string sigName;
    switch (sig) {
        case SIGSEGV:
            sigName = "Segmentation violation";
            break;
        case SIGABRT:
            sigName = "Abort";
            break;
        case SIGFPE:
            sigName = "Floating-point exception";
            break;
        case SIGILL:
            sigName = "Illegal instruction";
            break;
        case SIGTERM:
            sigName = "Termination";
            break;
#ifdef SIGBUS
        case SIGBUS:
            sigName = "Bus";
            break;
#endif
        default:
            sigName = "Unknown " + std::to_string(sig);
            break;
    }

    //Get exception, for some reason rethrowing doesnt work in MinGW
#if !defined(_WIN32) || defined(_MSC_VER)
    std::exception_ptr e = std::current_exception();
    if (e != nullptr) {
        try {
            std::rethrow_exception(e);
        } catch (std::domain_error& e) { // Inherits logic_error
            lastException = "Domain error " + std::string(e.what());
        } catch (std::invalid_argument& e) { // Inherits logic_error
            lastException = "Invalid argument " + std::string(e.what());
        } catch (std::length_error& e) { // Inherits logic_error
            lastException = "Length error " + std::string(e.what());
        } catch (std::out_of_range& e) { // Inherits logic_error
            lastException = "Out of range " + std::string(e.what());
        } catch (std::range_error& e) { // Inherits runtime_error
            lastException = "Range error " + std::string(e.what());
        } catch (std::overflow_error& e) { // Inherits runtime_error
            lastException = "Overflow error " + std::string(e.what());
        } catch (std::underflow_error& e) { // Inherits runtime_error
            lastException = "Underflow error " + std::string(e.what());
        } catch (std::logic_error& e) { // Inherits exception
            lastException = "Logic error " + std::string(e.what());
        } catch (std::runtime_error& e) { // Inherits exception
            lastException = "Runtime error " + std::string(e.what());
        } catch (std::bad_exception& e) { // Inherits exception
            lastException = "Bad exception " + std::string(e.what());
        } catch (std::exception& e) {
            lastException = "Base exception " + std::string(e.what());
        } catch (...) {
            lastException = "Unknown exception";
        }
    }
#endif

    std::ostringstream errorMessage;
    errorMessage << "Catched Signal: " << sigName << std::endl;
    if (!lastException.empty()) {
        errorMessage << "Last Exception:" << lastException << std::endl;
    }
    std::string errorMsgStr = errorMessage.str();
    ErrH.Abort(errorMsgStr.c_str(), XERR_SIGNAL, sig);

    //Agur
    raise(sig);
}

void getStackTrace(std::ostringstream& stream) {
#ifndef OPTION_DISABLE_STACKTRACE
    //Get current
    auto st = boost::stacktrace::stacktrace();
    //Check if failed to load
    if (st.empty()) {
        return;
    }
    //Write lines
    for (size_t i = 0; i < st.size(); ++i) {
        //Just store the name instead of full name as we don't really care our own name
        if (i == 0) {
            stream << "getStackTrace <- last call" << std::endl;
            continue;
        }

        //Pull the stacktrace info
        std::string line = boost::stacktrace::detail::to_string(&st.as_vector()[i], 1);
        std::string::size_type size = line.size();
        if (10 <= size) {
            //Remove the start number and end newline
            line = line.substr(4, size - 5);
            stream << line << std::endl;
        }
    }
#else
    stream << "OPTION_DISABLE_STACKTRACE set, no stacktrace available" << std::endl;
#endif
}

XErrorHandler::XErrorHandler() {
	prefix		= defprefix;
	restore_func	= nullptr;
	state		= 0;

#ifndef __HAIKU__
    log_name = "logfile.txt";
#else
    log_name = SDL_GetPrefPath("KranX Productions", "Perimeter");
	log_name += "/logfile.txt";
#endif
    log_file.open(log_name.c_str(),std::ios::out|std::ios::trunc);
    log_file.close();

    //Register handler
#ifdef _WIN32
    SetUnhandledExceptionFilter((LPTOP_LEVEL_EXCEPTION_FILTER)&exHandler);
#else
    setTerminateHandler(handleTerminate);
#endif
    setSignalHandler(handleSignal);
}

XErrorHandler::~XErrorHandler() {
    if(log_file.is_open()) {
        log_file.close();
    }
}

void XErrorHandler::Abort(const char* message, int code, int val, const char* subj)
{
	if (restore_func) {
		restore_func();
		restore_func = nullptr;
	}

	state = 1;



    //Assemble text
    std::ostringstream stream;
    if (prefix) {
        stream << prefix << std::endl;
    }
    stream << "Error ocurred! Code: " << code << " Val: " << val << std::endl;
    stream << message << std::endl;
    if (subj) {
        stream << "Subj: " << subj << std::endl;
    }

    std::list<std::string> linesStackTrace;
    stream << std::endl << "Call stack:" << std::endl;
    getStackTrace(stream);
    stream << std::endl << "Please send:" << std::endl <<
            " - This message" << std::endl <<
            " - Log file from " << SDL_GetBasePath() << log_name.c_str() << std::endl <<
            " - Your savegame" << std::endl <<
            "To https://t.me/PerimeterGame or https://github.com/KranX/Perimeter" << std::endl;
    std::string str =  stream.str();

    //Write to log
    log_file.open(log_name.c_str(),std::ios::out|std::ios::app);

    log_file<<str;

    log_file.close();

    fprintf(stderr, "%s", str.c_str());

    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,
                             "Perimeter error",
                             str.c_str(),
                             nullptr);

    //MessageBox(NULL,outmsg,prefix,attr | MB_TOPMOST | MB_SYSTEMMODAL);

	exit(code);
}

void XErrorHandler::Exit()
{
	if(restore_func) {
		restore_func();
		restore_func = nullptr;
	}
	exit(0);
}
void XErrorHandler::SetPrefix(const char* s)
{
	prefix = s;
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
