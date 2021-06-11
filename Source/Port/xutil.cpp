#include <StdAfx.h>
#include <Windows.h>

int DiagAssert(unsigned long dwOverrideOpts, const char* szMsg, const char* szFile, unsigned long dwLine)
{
    // 0 = retry
    // 1 = ignore
    // 2 = abort
    if (!dwOverrideOpts)
    {
        int status = MessageBoxA(0, szMsg, szFile, MB_ABORTRETRYIGNORE);
        switch (status)
        {
        case IDABORT: return 2;
        case IDIGNORE: return 1;
        case IDRETRY: return 0;
        }
    }
    return 1;
}

__int64 getRDTSC()
{
    return __rdtsc();
}

unsigned int XRnd(unsigned int m)
{
    return rand() % m;
}

void XRndSet(unsigned int m)
{
    srand(m);
}

int clocki(void)
{
    return GetTickCount();
}

double clockf()
{
    return GetTickCount();
}

XErrorHandler ErrH;

XErrorHandler::XErrorHandler()
{

}

XErrorHandler::~XErrorHandler()
{

}

void XErrorHandler::Abort(const char* message, int code /*= XERR_USER*/, int addval /*= -1*/, const char* subj /*= NULL*/)
{

}

void XErrorHandler::Exit()
{

}

void	 XErrorHandler::SetPrefix(const char* s) {}
void	 XErrorHandler::SetPostfix(const char* s) {}
void	 XErrorHandler::SetRestore(void (*rf)(void)) {}

const char* check_command_line(const char* switch_str)
{
    return NULL;
}

void allocation_tracking(char* title)
{

}
void allocation_tracking(char* title, AllocationAccumulator* state)
{

}

void SetAssertRestoreGraphicsFunction(void(*func)())
{

}

unsigned int xt_get_cpuid(void)
{
    return 0;
}
char* xt_getMMXstatus(void)
{
    return NULL;
}

void initclock(void)
{

}

class TimerDataList {};

Profiler::Profiler() : timers(*new TimerDataList()) {}
Profiler::~Profiler() {}

void Profiler::quant(unsigned long curLogicQuant)
{

}

Profiler& Profiler::instance()
{
    static Profiler p;
    return p;
}

void Profiler::start_stop()
{

}

void  StatisticalData::print(XBuffer& buf)
{

}