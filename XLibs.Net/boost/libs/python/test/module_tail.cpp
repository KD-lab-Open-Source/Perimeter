// Copyright David Abrahams 2001. Permission to copy, use,
// modify, sell and distribute this software is granted provided this
// copyright notice appears in all copies. This software is provided
// "as is" without express or implied warranty, and with no claim as
// to its suitability for any purpose.

#if defined(_WIN32)
# ifdef __MWERKS__
#  pragma ANSI_strict off
# endif
# include <windows.h>
# ifdef __MWERKS__
#  pragma ANSI_strict reset
# endif

# ifdef _MSC_VER
#  pragma warning(push)
#  pragma warning(disable:4297)
extern "C" void straight_to_debugger(unsigned int, EXCEPTION_POINTERS*)
{
    throw;
}
extern "C" void (*old_translator)(unsigned, EXCEPTION_POINTERS*)
         = _set_se_translator(straight_to_debugger);
#  pragma warning(pop)
# endif

#endif // _WIN32

