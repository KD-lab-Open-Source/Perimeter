/* This header turns off warnings that STLport headers generate for compiled user code.
 * This file is intended to work with compilers from Microsoft.
 * For warnings while compiling STLport, see src/vc_warning_disable.h.
 */

#if (_MSC_VER > 1000)
#  if defined (_STLP_USE_MSVC6_MEM_T_BUG_WORKAROUND)
/*
 * 31/07/2004: dums - now that we do not export the basic_string class anymore but only a base class
 * we have to disable this warning as the string are used as data members type of many iostream classes.
 */
#    pragma warning ( disable : 4251 )  // ignore template classes being exported in .dll's
#  endif

#  if (_MSC_VER < 1200) // VC5 and earlier
#    pragma warning( disable : 4389 ) // '==' : signed/unsigned mismatch
// multiple copy constructors/assignment operators specified,
// with member templates are bogus...
#    pragma warning( disable : 4521 )
#    pragma warning( disable : 4522 )
#  endif

#  if (_MSC_VER < 1300) // VC6, eVC3, eVC4
#    pragma warning( disable : 4097 ) // typedef-name used as based class of (...)
#    pragma warning( disable : 4231 ) // non standard extension : 'extern' before template instanciation
#    pragma warning( disable : 4244 ) // implicit conversion: possible loss of data
#    pragma warning( disable : 4284 ) // for -> operator
//This warning is necessary because of the native platform headers:
#    pragma warning( disable : 4290 ) // c++ exception specification ignored
#    pragma warning( disable : 4514 ) // unreferenced inline function has been removed
#    pragma warning( disable : 4660 ) // template-class specialization '...' is already instantiated
#    pragma warning( disable : 4701 ) // local variable '...' may be used without having been initialized
#    pragma warning( disable : 4710 ) // function (...) not inlined
#    pragma warning( disable : 4786 ) // identifier truncated to 255 characters
#  endif

#  if (_MSC_VER <= 1310)
#    pragma warning( disable : 4355 ) // this used in base member initializer list
#    pragma warning( disable : 4511 ) // copy constructor cannot be generated
#  endif

//Pool of common warnings for all MSVC supported versions:
//Many are only useful if warning level is set to 4.
#  pragma warning( disable : 4100 ) // unreferenced formal parameter
#  pragma warning( disable : 4127 ) // conditional expression is constant
#  pragma warning( disable : 4146 ) // unary minus operator applied to unsigned type, result still unsigned
#  pragma warning( disable : 4245 ) // conversion from 'enum ' to 'unsigned int', signed/unsigned mismatch
#  pragma warning( disable : 4510 ) // default constructor cannot be generated
#  pragma warning( disable : 4512 ) // assignment operator could not be generated
#  pragma warning( disable : 4571 ) // catch(...) blocks compiled with /EHs do not catch or re-throw Structured Exceptions
#  pragma warning( disable : 4610 ) // struct '...' can never be instantiated - user defined construtor required

// dums: This warning, signaling deprecated C functions like strncpy,
// will have to be fixed one day:
#  pragma warning( disable : 4996 ) // deprecated C symbols

#endif
