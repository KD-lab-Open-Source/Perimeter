#include <list>
#include <iostream>
#include <filesystem>
#include <csignal>
#include <sstream>
#include <vector>
#include <SDL.h>
#include "files/files.h"
#include "tweaks.h"
#include "xstream.h"
#include "xerrhand.h"
#include "xutl.h"

static void (*assert_restore_graphics_function)() = nullptr;

#if (!defined(_FINAL_VERSION_) || defined(_DEBUG) || defined(PERIMETER_DEBUG_ASSERT)) && !defined(NASSERT)
void SetAssertRestoreGraphicsFunction(void(*func)())
{
	assert_restore_graphics_function = func;
}
#endif

#ifndef OPTION_DISABLE_STACKTRACE
#define BOOST_STACKTRACE_GNU_SOURCE_NOT_REQUIRED 1
#include <boost/stacktrace.hpp>

#endif

#ifndef _WIN32
#define APIENTRY
#endif

typedef void (*sighandler) (int);
std::string lastException;

static const char *defprefix 	= "XHANDLER INFORM";
static const char *exceptMSG 	= "EXCEPTION OCCURED";
static const char *rterrorMSG	= "RUN-TIME ERROR";

XErrorHandler ErrH;

void setSignalHandler(sighandler signalHandler) {
//     signal(SIGSEGV, signalHandler);
//     signal(SIGABRT, signalHandler);
//     signal(SIGFPE, signalHandler);
//     signal(SIGILL, signalHandler);
//     signal(SIGTERM, signalHandler);
// #ifdef SIGBUS //UNIX specific
//     signal(SIGBUS, signalHandler);
// #endif
}

void setTerminateHandler(std::terminate_handler terminateHandler) {
    std::set_terminate(terminateHandler);
}

void restoreSignalHandler() {
    setSignalHandler(SIG_DFL);
}

void handleTerminate() {
    fprintf(stderr, "handleTerminate\n");

    abort();
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
#ifdef PERIMETER_DEBUG
            ErrH.Exit();
            return;
#else
            sigName = "Termination";
#endif
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

std::string decodeStackAddress(const void* addr) {
    std::string line;
#ifndef OPTION_DISABLE_STACKTRACE
    //Create frame and try to decode data
    boost::stacktrace::frame frame(addr);
    line += boost::stacktrace::detail::to_hex_array(frame.address()).data();
    line += "|" + frame.name();
    std::string detail = boost::stacktrace::detail::to_string(&frame, 1);
    std::string::size_type detail_size = detail.size();
    if (10 <= detail_size) {
        //Remove the start number and end newline
        line += "|" + detail.substr(4, detail_size - 5);
    }
#endif

    return line;
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
    std::string cmdline = "stack_reference=";
    cmdline += boost::stacktrace::detail::to_hex_array(reinterpret_cast<const void*>(&decodeStackAddress)).data();
    cmdline += " stack_frames=";
    for (size_t i = 0; i < st.size(); ++i) {        
        //Just store the name instead of full name as we don't really care our own name
        if (i == 0) {
            stream << "getStackTrace <- last call" << std::endl;
            continue;
        }

        //Pull the stacktrace info and decode
        const boost::stacktrace::frame& frame = st.as_vector()[i];
        
        cmdline += boost::stacktrace::detail::to_hex_array(frame.address()).data();
        cmdline += ",";
        
        std::string line = decodeStackAddress(frame.address());
        stream << line << std::endl;
    }
    
    stream << cmdline << std::endl;
#else
    stream << "OPTION_DISABLE_STACKTRACE set, no stacktrace available" << std::endl;
#endif
}

XErrorHandler::XErrorHandler() {
    prefix = defprefix;
    crash_func = nullptr;
    restore_func = nullptr;
    state = 0;
    log_path.clear();
    const char* lop_path_ptr = GET_PREF_PATH();
    if (lop_path_ptr) {
        log_path = lop_path_ptr;
        SDL_free((void*) lop_path_ptr);
    }
    log_path += "logfile.txt";
	if (std::filesystem::exists(std::filesystem::u8path(log_path))) {
        std::error_code error;
        std::filesystem::remove(std::filesystem::u8path(log_path), error);
        if (error) {
           fprintf(stderr, "Error deleting log file: %d %s at %s\n",  error.value(), error.message().c_str(), log_path.c_str());
        }
    }

    //Register handler
    setTerminateHandler(handleTerminate);
    setSignalHandler(handleSignal);
}

XErrorHandler::~XErrorHandler() {
}

void XErrorHandler::RedirectStdio() const {
#ifdef GPX
    return;
#endif

    if (log_path.empty() || check_command_line("no_console_redirect") != nullptr) {
        return;
    }
    //Check if we should redirect stdio
    printf("Redirecting console stdio output into log file at %s, to prevent this pass arg no_console_redirect=1\n", log_path.c_str());
    fflush(stdout);

    //Reopen streams, Win32 needs wide char version to handle cyrillic
    
    bool out_fail, err_fail;
#ifdef _WIN32
    UTF8_TO_WCHAR(log_path, log_path.c_str())
    out_fail = _wfreopen(wchar_log_path, L"a", stdout) == nullptr;
    err_fail = _wfreopen(wchar_log_path, L"a", stderr) == nullptr;
#else
    out_fail = freopen(log_path.c_str(), "a", stdout) == nullptr;
    err_fail = freopen(log_path.c_str(), "a", stderr) == nullptr;
#endif
    if (out_fail && err_fail) fprintf(stderr, "Error redirecting stdout and stderr\n");
    else if (out_fail) fprintf(stderr, "Error redirecting stdout\n");
    else if (err_fail) fprintf(stderr, "Error redirecting stderr\n");
    //Disable buffering because we don't flush, specially if crash happens
    setvbuf(stdout, nullptr, _IONBF, 0);
    setvbuf(stderr, nullptr, _IONBF, 0);
}

void XErrorHandler::Abort(const char* message, int code, int val, const char* subj)
{
    fprintf(stderr, "XErrorHandler::Abort called!\n");
	if (restore_func) {
		restore_func();
		restore_func = nullptr;
	}

	state = 1;

    //Assemble text
    std::ostringstream stream;
    if (!prefix.empty()) {
        stream << prefix << std::endl;
    }
    stream << "Error ocurred! Code: " << code << " Val: " << val << std::endl;
    stream << message << std::endl;
    if (subj) {
        stream << "Subj: " << subj << std::endl;
    }
    stream << "Clock: " << clocki() << std::endl;

    std::string crash_path = get_content_root_path_str() + CRASH_DIR;
    std::list<std::string> linesStackTrace;
    stream << std::endl << "Call stack:" << std::endl;
    getStackTrace(stream);
    stream << std::endl << "Please send:" << std::endl <<
           " - This message" << std::endl <<
           " - Log file from " << log_path.c_str() << std::endl <<
           " - Crash files from " << crash_path << std::endl <<
            "To https://t.me/PerimeterGame or https://github.com/KD-lab-Open-Source/Perimeter" << std::endl;
    std::string str =  stream.str();

    fprintf(stderr, "%s\n", str.c_str());

    ShowErrorMessage(str.c_str());

    if (crash_func) {
        //Store pointer before calling to avoid cyclic calls in case handler fails
        auto func = crash_func;
        crash_func = nullptr;
        func();
    }

	exit(code);
}


void XErrorHandler::Abort(const std::string& message, int code, int val, const char* subj) {
    Abort(message.c_str(), code, val, subj);
}

bool XErrorHandler::ShowErrorMessage(const char* message) {
    std::string text = BreakLongLines(message, 160);
    int err = SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,
                                       "Perimeter error",
                                       text.c_str(),
                                       nullptr);
    if (err) {
        SDL_PRINT_ERROR("Creating error window");
    }

    return err != 0;
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

void XErrorHandler::SetCrash(void (*cf)(void))
{
    crash_func = cf;
}

void ErrH_RTC(const char *file,unsigned int line, const char *expr) {
    std::string msg = file;
    msg += ":" + std::to_string(line) + "\r\n" + expr;
    ErrH.Abort(rterrorMSG,XERR_USER,-1,msg.c_str());
}