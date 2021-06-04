// STLport configuration file
// It is internal STLport header - DO NOT include it directly
// Microsoft Visual C++ 6.0, 7.0, 7.1, 8.0 Beta, ICL

#if defined (_STLP_USING_PLATFORM_SDK_COMPILER)
/* This is 64 bits platform SDK specific settings. There is no clear way to 
 * recognize the SDK coming with a compiler from the one freely available.
 * For the moment we hope that there is only one SDK for 64 bits windows so
 * we simply detect it using the _WIN64 macro.
 */
#  define _STLP_NATIVE_INCLUDE_PATH ../crt
#  define _STLP_VENDOR_GLOBAL_CSTD
#  define _STLP_VENDOR_TERMINATE_STD
#  define _STLP_GLOBAL_NEW_HANDLER
#  if (_MSC_VER <= 1400)
//We hope this bug will be fixed in future versions.
#    define _STLP_NEW_DONT_THROW_BAD_ALLOC 1
#  endif
#endif

#define _STLP_CALL __cdecl

#ifndef _STLP_LONG_LONG
#  define _STLP_LONG_LONG __int64
#endif

#define _STLP_PRAGMA_ONCE

// these switches depend on compiler flags
#ifndef _CPPUNWIND
#  define _STLP_DONT_USE_EXCEPTIONS 1
#endif

#if defined (_MT) && !defined (_STLP_NO_THREADS) && !defined (_REENTRANT)
#  define _REENTRANT 1
#endif

#if !defined (_NATIVE_WCHAR_T_DEFINED)
#  define _STLP_WCHAR_T_IS_USHORT 1
#endif

#define _STLP_MINIMUM_IMPORT_STD
#define _STLP_NO_VENDOR_STDLIB_L 1

#if defined (_STLP_MSVC)

#  ifndef _CPPRTTI
#    define _STLP_NO_RTTI 1
#  endif

#  ifndef _STLP_MSVC50_COMPATIBILITY
#    define _STLP_MSVC50_COMPATIBILITY   1
#  endif

#  define _STLP_DLLEXPORT_NEEDS_PREDECLARATION 1
#  define _STLP_HAS_SPECIFIC_PROLOG_EPILOG 1

// # ifndef __BUILDING_STLPORT
// #  define _STLP_USE_TEMPLATE_EXPORT 1
// # endif
#  if (_STLP_MSVC <= 1400)
#    define _STLP_STATIC_CONST_INIT_BUG   1
#  endif  //  (_STLP_MSVC <= 1400)

#  if (_STLP_MSVC >= 1310)
#    define _STLP_FULL_ADL_IMPLEMENTED 1
#  endif  //  (_STLP_MSVC >= 1310)

#  if (_STLP_MSVC >= 1300)
#    undef _STLP_NO_UNCAUGHT_EXCEPT_SUPPORT
#    if !defined (_STLP_DONT_USE_EXCEPTIONS)
#      define _STLP_NOTHROW throw()
#    endif
#  endif  //  (_STLP_MSVC >= 1300)

#  if (_STLP_MSVC <= 1300) 
#    define _STLP_NO_CLASS_PARTIAL_SPECIALIZATION 1
#    define _STLP_NO_FUNCTION_TMPL_PARTIAL_ORDER 1
// there is no partial spec, and MSVC breaks on simulating it for iterator_traits queries
#    define _STLP_USE_OLD_HP_ITERATOR_QUERIES
#    define _STLP_NO_TYPENAME_IN_TEMPLATE_HEADER
#    define _STLP_NO_METHOD_SPECIALIZATION 1
#    define _STLP_DEF_CONST_PLCT_NEW_BUG 1
#    define _STLP_NO_TYPENAME_ON_RETURN_TYPE 1
// VC++ cannot handle default allocator argument in template constructors
#    define _STLP_NEEDS_EXTRA_TEMPLATE_CONSTRUCTORS
#    define _STLP_NO_QUALIFIED_FRIENDS    1
#    define _STLP_NO_FRIEND_TEMPLATES
// fails to properly resolve call to sin() from within sin()
#  endif /* _STLP_MSVC <= 1300 */

#  if (_STLP_MSVC >= 1200)
#    define _STLP_HAS_NATIVE_FLOAT_ABS 1
#  endif

#  if (_STLP_MSVC == 1300)
#    define _STLP_NO_MOVE_SEMANTIC
#  endif

#  if (_STLP_MSVC < 1300)
/*
 * dums: VC6 do not handle correctly member templates of class that are explicitely
 * instanciated to be exported. There is a workaround, seperate the non template methods
 * from the template ones within 2 different classes and only export the non template one.
 * It is implemented for basic_string and locale at the writing of this note. 
 * This problem has finally been concidered as not important enough to not use this feature. 
 * Moreover boost (www.boost.org) required it to be granted.
 * The workaround is activated thanks to the _STLP_USE_MSVC6_MEM_T_BUG_WORKAROUND macro defined
 * later in this config file.
 */
/*
#    if defined (_DLL)
#      define _STLP_NO_MEMBER_TEMPLATES 1
#    endif
*/

// boris : not defining this macro for SP5 causes other problems
//#    if !defined (_MSC_FULL_VER) || (_MSC_FULL_VER < 12008804 )
#    define _STLP_NO_USING_FOR_GLOBAL_FUNCTIONS 1
//#    endif
#    define _STLP_DONT_USE_BOOL_TYPEDEF 1
#    define _STLP_DONT_RETURN_VOID 1
#  endif

/*
 * MSVC6 is known to have many trouble with namespace management but
 * MSVC .Net 2003 and 2005 also have a bug difficult to reproduce without
 * STLport:
 * namespace stlp_std {
 *   typedef int foo_int;
 * }
 * #include <map>
 * const foo_int bar = 0;
 *
 * As you can see foo is available without namespace specification as if 
 * a using namespace stlp_std has been performed. Defining _STLP_USING_NAMESPACE_BUG
 * restore the expected compilation error.
 */
#  define _STLP_USING_NAMESPACE_BUG 1

#endif /* _STLP_MSVC */

#if (_MSC_VER <= 1310) 
#  define _STLP_VENDOR_GLOBAL_CSTD
#endif /* (_MSC_VER <= 1310) */

#if (_MSC_VER < 1300)
#  define _STLP_NO_IEC559_SUPPORT 1
#endif

#if (_MSC_VER >= 1300)
/* Starting with MSVC 7.0 and compilers simulating it,
 * we assume that the new SDK is granted:
 */
#  define _STLP_NEW_PLATFORM_SDK 1
#endif

#if (_MSC_VER < 1300) || defined (UNDER_CE) // including MSVC 6.0
//  these work, as long they are inline
#  define _STLP_INLINE_MEMBER_TEMPLATES 1
#  define _STLP_NO_MEMBER_TEMPLATE_KEYWORD 1
#  define _STLP_GLOBAL_NEW_HANDLER 1
#  define _STLP_DONT_SUPPORT_REBIND_MEMBER_TEMPLATE 1
#  define _STLP_NEW_DONT_THROW_BAD_ALLOC 1
#  define _STLP_VENDOR_UNEXPECTED_STD
#endif

#if (_STLP_MSVC > 1100)
typedef char __stl_char;
#  define _STLP_DEFAULTCHAR __stl_char
#endif /* (_STLP_MSVC < 1100 ) */

#if ( _MSC_VER<=1010 )
// "bool" is reserved in MSVC 4.1 while <yvals.h> absent, so :
// #    define _STLP_USE_ABBREVS           1
#  define _STLP_NO_BAD_ALLOC
#  define _STLP_HAS_NO_NEW_C_HEADERS 1
#  define _STLP_NO_NEW_NEW_HEADER 1
#elif (_MSC_VER < 1100)
// VC++ 4.2 and higher
#  define _STLP_YVALS_H 1
#  define _STLP_USE_NO_IOSTREAMS 1
#endif /* 1010 */

#if defined (_STLP_MSVC) && (_STLP_MSVC < 1200) /* before VC++ 6.0 */
// #  define _STLP_NO_MEMBER_TEMPLATES 1
// #  define _STLP_NO_EXPLICIT_FUNCTION_TMPL_ARGS 1
#  define _STLP_DONT_SIMULATE_PARTIAL_SPEC_FOR_TYPE_TRAITS 1
#  define _STLP_DONT_USE_PARTIAL_SPEC_WRKD 1
#  define _STLP_QUALIFIED_SPECIALIZATION_BUG 1
#  define _STLP_NON_TYPE_TMPL_PARAM_BUG 1 
#  define _STLP_THROW_RETURN_BUG 1
#  define _STLP_NO_MEMBER_TEMPLATE_CLASSES 1
#  define _STLP_DEF_CONST_DEF_PARAM_BUG 1
#endif

#if defined (_STLP_MSVC) && ( _STLP_MSVC < 1100 )
#  ifndef _STLP_USE_NO_IOSTREAMS
#    define _STLP_USE_NO_IOSTREAMS
#  endif
// #  define _STLP_NESTED_TYPE_PARAM_BUG 1
// Debug mode does not work for 4.2
#  if defined (_STLP_DEBUG)
#    pragma message ("STLport debug mode does not work for VC++ 4.2, turning _STLP_DEBUG off ...")
#    undef _STLP_DEBUG
#  endif
#  define _STLP_NO_BOOL            1
#  define _STLP_NEED_TYPENAME      1
#  define _STLP_NEED_EXPLICIT      1
#  define _STLP_NEED_MUTABLE       1
#  define _STLP_NO_PARTIAL_SPECIALIZATION_SYNTAX
#  define _STLP_LIMITED_DEFAULT_TEMPLATES 1

// up to 4.2, library is in global namespace
#  define _STLP_VENDOR_GLOBAL_STD
#  define _STLP_NONTEMPL_BASE_MATCH_BUG 1
#  define _STLP_BROKEN_USING_DIRECTIVE  1
#  define _STLP_NO_ARROW_OPERATOR 1
#  define _STLP_NO_SIGNED_BUILTINS 1
#  define _STLP_NO_EXCEPTION_SPEC 1
#  undef  _STLP_DEFAULT_TYPE_PARAM
#  undef  _STLP_HAS_NO_NAMESPACES
#  undef  _STLP_NO_AT_MEMBER_FUNCTION
#  undef  _STLP_NO_MEMBER_TEMPLATES
#  undef  _STLP_NO_MEMBER_TEMPLATE_CLASSES
#  define _STLP_HAS_NO_NAMESPACES 1
#  define _STLP_NO_AT_MEMBER_FUNCTION 1
#  define _STLP_NO_MEMBER_TEMPLATES 1
#  define _STLP_NO_MEMBER_TEMPLATE_CLASSES 1
#endif /* 1100 */

// If we are under Windows CE, include appropriate config
#if defined (UNDER_CE) && (_MSC_VER < 1200)
    // Microsoft Visual C++ 5 with Windows CE Toolkit;
    // could also be Visual C++ 6 with toolkit, but we can't detect that.
    // the Windows CE Toolkit is obsolete, anyway
#    include <config/stl_wince.h>
#endif /* UNDER_CE */

#if (_MSC_VER < 1300) // MSVC 6.0 and earlier
// defined for DEBUG and NDEBUG too, to allow user mix own debug build with STLP release library
#  define _STLP_USE_ABBREVS
#endif

#if !defined (_STLP_WINCE)
#  define _STLP_EXPORT_DECLSPEC __declspec(dllexport)
#  define _STLP_IMPORT_DECLSPEC __declspec(dllimport)
#endif

#if !(defined (_STLP_MSVC) && _STLP_MSVC < 1100)
#  define _STLP_CLASS_EXPORT_DECLSPEC __declspec(dllexport)
#  define _STLP_CLASS_IMPORT_DECLSPEC __declspec(dllimport)
#endif

//dums 12/05/2005: removed _WINDLL check, this macro means that the user is building a dll
// but not what the user wants to be linked to.
#if defined (__DLL) || defined (_DLL) || defined (_RTLDLL) || defined (_AFXDLL)
#  if !defined (_STLP_USE_STATIC_LIB)
#    if !defined (_STLP_USE_DYNAMIC_LIB)
#      define _STLP_USE_DYNAMIC_LIB
#    endif
#  else
/* The user is forcing use of STLport as a dynamic library. We signal it so
 * that the STLport namespace will be modify to report such a combination 
 * and force the user to link with the rebuild STLport library.
 */
#    define _STLP_USING_CROSS_NATIVE_RUNTIME_LIB
#  endif
#else
#  if !defined(_STLP_USE_DYNAMIC_LIB)
#    if !defined (_STLP_USE_STATIC_LIB)
#      define _STLP_USE_STATIC_LIB
#    endif
#  else
/* Idem previous remark but the user forces use of the static native runtime.
 */
#    define _STLP_USING_CROSS_NATIVE_RUNTIME_LIB
#  endif
#endif

#if defined(_STLP_USE_DYNAMIC_LIB)
#  undef  _STLP_USE_DECLSPEC
#  define _STLP_USE_DECLSPEC 1
#  if (_STLP_MSVC >= 1200) && (_STLP_MSVC < 1300)
#    define _STLP_USE_MSVC6_MEM_T_BUG_WORKAROUND 1
#  endif
#endif

#if !defined (_STLP_IMPORT_TEMPLATE_KEYWORD)
#  if !defined (_MSC_EXTENSIONS) || defined(_STLP_MSVC) && _STLP_MSVC >= 1300
#    define _STLP_IMPORT_TEMPLATE_KEYWORD
#  else
#    define _STLP_IMPORT_TEMPLATE_KEYWORD extern
#  endif
#endif
#define _STLP_EXPORT_TEMPLATE_KEYWORD

#if (defined (__ICL) && (__ICL < 450)) || (_MSC_VER < 1200)
//    only static STLport lib now works for ICL and VC 5.0
#  undef  _STLP_USE_STATIC_LIB
#  define _STLP_USE_STATIC_LIB
//    disable hook which makes template symbols to be searched for in the library
#  undef _STLP_NO_CUSTOM_IO
#endif

#if !defined (__BUILDING_STLPORT) && !defined (_STLP_DONT_USE_AUTO_LINK) && \
    !defined (_STLP_NO_IOSTREAMS) && !defined (_STLP_USE_NO_IOSTREAMS)

#  define _STLP_STRINGIZE(X) _STLP_STRINGIZE_AUX(X)
#  define _STLP_STRINGIZE_AUX(X) #X

#  if defined (_STLP_DEBUG)
#    define _STLP_LIB_OPTIM_MODE "stld"
#  elif defined (_DEBUG)
#    define _STLP_LIB_OPTIM_MODE "d"
#  else
#    define _STLP_LIB_OPTIM_MODE ""
#  endif

#  if defined (_STLP_USE_DYNAMIC_LIB)
#    define _STLP_LIB_TYPE ""
#  else
#    define _STLP_LIB_TYPE "_static"
#  endif

#  define _STLP_VERSION_STR _STLP_STRINGIZE(_STLPORT_MAJOR)"."_STLP_STRINGIZE(_STLPORT_MINOR)

#  define _STLP_STLPORT_LIB "stlport"_STLP_LIB_OPTIM_MODE""_STLP_LIB_TYPE"."_STLP_VERSION_STR".lib"

#  if defined (_STLP_VERBOSE_AUTO_LINK)
#    pragma message ("STLport: Auto linking to "_STLP_STLPORT_LIB)
#  endif
#  pragma comment (lib, _STLP_STLPORT_LIB)

#  undef _STLP_STLPORT_LIB
#  undef _STLP_LIB_OPTIM_MODE
#  undef _STLP_LIB_TYPE
#  undef _STLP_STRINGIZE_AUX
#  undef _STLP_STRINGIZE

#endif /* _STLP_DONT_USE_AUTO_LINK */

#if defined (_STLP_USING_PLATFORM_SDK_COMPILER)
/* The Windows 64 bits SDK required for the moment link to bufferoverflowU.lib for
 * additional buffer overrun checks. Rather than require the STLport build system and
 * users to explicitely link with it we use the MSVC auto link feature.
 */
#  if !defined (_STLP_DONT_USE_AUTO_LINK) || defined (__BUILDING_STLPORT)
#    pragma comment (lib, "bufferoverflowU.lib")
#    if defined (_STLP_VERBOSE_AUTO_LINK)
#      pragma message ("STLport: Auto linking to bufferoverflowU.lib")
#    endif
#  endif
#endif

// Local Variables:
// mode:C++
// End:
