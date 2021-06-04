/*
 * Copyright (c) 1999 
 * Boris Fomitchev
 *
 * This material is provided "as is", with absolutely no warranty expressed
 * or implied. Any use is at your own risk.
 *
 * Permission to use or copy this software for any purpose is hereby granted 
 * without fee, provided the above notices are retained on all copies.
 * Permission to modify the code and to distribute modified code is granted,
 * provided the above notices are retained, and a notice that the code was
 * modified is included with the above copyright notice.
 *
 */

#ifndef _STLP_CMATH_H_HEADER
#define _STLP_CMATH_H_HEADER

#if defined(__sun) && defined(__GNUC__)
extern "C" {
extern float __acosf(float);
extern float __asinf(float);
extern float __atanf(float);
extern float __atan2f(float, float);
extern float __ceilf(float);
extern float __cosf(float);
extern float __coshf(float);
extern float __expf(float);
extern float __fabsf(float);
extern float __floorf(float);
extern float __fmodf(float, float);
extern float __frexpf(float, int *);
extern float __ldexpf(float, int);
extern float __logf(float);
extern float __log10f(float);
extern float __modff(float, float *);
extern float __powf(float, float);
extern float __sinf(float);
extern float __sinhf(float);
extern float __sqrtf(float);
extern float __tanf(float);
extern float __tanhf(float);

extern long double __acosl(long double);
extern long double __asinl(long double);
extern long double __atanl(long double);
extern long double __atan2l(long double, long double);
extern long double __ceill(long double);
extern long double __cosl(long double);
extern long double __coshl(long double);
extern long double __expl(long double);
extern long double __fabsl(long double);
extern long double __floorl(long double);
extern long double __fmodl(long double, long double);
extern long double __frexpl(long double, int *);
extern long double __ldexpl(long double, int);
extern long double __logl(long double);
extern long double __log10l(long double);
extern long double __modfl(long double, long double *);
extern long double __powl(long double, long double);
extern long double __sinl(long double);
extern long double __sinhl(long double);
extern long double __sqrtl(long double);
extern long double __tanl(long double);
extern long double __tanhl(long double);
}
#endif

#ifndef __sun
#  define _STLP_MATH_INLINE(float_type, func, cfunc) \
     inline float_type func (float_type x) { return ::cfunc(x); }
#  define _STLP_MATH_INLINE2(float_type, type, func, cfunc) \
     inline float_type func (float_type x, type y) { return ::cfunc(x,y); }
#  define _STLP_MATH_INLINE_D(float_type, func, cfunc) 
#  define _STLP_MATH_INLINE2_D(float_type, type, func, cfunc)
#else
#  ifdef __GNUC__
#    if 0
#      define _STLP_MATH_INLINE(float_type, func, cfunc) \
       inline float_type func (float_type x) { return ::__##cfunc(x); }
#      define _STLP_MATH_INLINE2(float_type, type, func, cfunc) \
       inline float_type func (float_type x, type y) { return ::__##cfunc(x,y); }
#      define _STLP_MATH_INLINE_D(float_type, func, cfunc) 
#      define _STLP_MATH_INLINE2_D(float_type, type, func, cfunc)
#    endif
#    if 1
#      define _STLP_MATH_INLINE(float_type, func, cfunc) \
       inline float_type func (float_type x) { return ::cfunc(x); }
#      define _STLP_MATH_INLINE2(float_type, type, func, cfunc) \
       inline float_type func (float_type x, type y) { return ::cfunc(x,y); }
#      define _STLP_MATH_INLINE_D(float_type, func, cfunc) 
#      define _STLP_MATH_INLINE2_D(float_type, type, func, cfunc)
#    endif
#  endif
#  ifdef __SUNPRO_CC
#    define _STLP_MATH_INLINE(float_type, func, cfunc) \
       inline float_type func (float_type x) { return _STLP_VENDOR_CSTD::__##cfunc(x); }
#    define _STLP_MATH_INLINE_D(float_type, func, cfunc) \
       inline float_type func (float_type x) { return _STLP_VENDOR_CSTD::cfunc(x); }
#    define _STLP_MATH_INLINE2(float_type, type, func, cfunc) \
       inline float_type func (float_type x, type y) { return _STLP_VENDOR_CSTD::__##cfunc(x,y); }
#    define _STLP_MATH_INLINE2_D(float_type, type, func, cfunc) \
       inline float_type func (float_type x, type y) { return _STLP_VENDOR_CSTD::cfunc(x,y); }
#  endif
#endif

#define _STLP_MATH_INLINEX(__type,func,cfunc) \
  inline __type func (__type x) { return __STATIC_CAST(__type,::cfunc((double)x)); }
#define _STLP_MATH_INLINE2X(__type1,__type2,func,cfunc) \
  inline __type1 func (__type1 x, __type2 y) { return __STATIC_CAST(__type1,::cfunc((double)x,y)); }
#define _STLP_MATH_INLINE2PX(__type,func,cfunc) \
  inline __type func (__type x, __type *y) { \
     double tmp1, tmp2; \
     tmp1 = ::cfunc(__STATIC_CAST(double,x),&tmp2); *y = __STATIC_CAST(__type,tmp2); \
     return __STATIC_CAST(__type,tmp1); }
#define _STLP_MATH_INLINE2XX(__type,func,cfunc) \
  inline __type func (__type x, __type y) { return __STATIC_CAST(__type,::cfunc((double)x,(double)y)); }

#if !defined (_STLP_NO_LONG_DOUBLE) && !defined (_STLP_NO_VENDOR_MATH_L) && !defined (_STLP_NO_VENDOR_MATH_F) // && defined(_STLP_VENDOR_LONG_DOUBLE_MATH)
#  define _STLP_DEF_MATH_INLINE(func,cf) \
  _STLP_MATH_INLINE(float,func,cf##f) \
  _STLP_MATH_INLINE_D(double,func,cf) \
  _STLP_MATH_INLINE(long double,func,cf##l)
#  define _STLP_DEF_MATH_INLINE2(func,cf) \
  _STLP_MATH_INLINE2(float,float,func,cf##f) \
  _STLP_MATH_INLINE2_D(double,double,func,cf) \
  _STLP_MATH_INLINE2(long double,long double,func,cf##l)
#  define _STLP_DEF_MATH_INLINE2P(func,cf) \
  _STLP_MATH_INLINE2(float,float *,func,cf##f) \
  _STLP_MATH_INLINE2_D(double,double *,func,cf) \
  _STLP_MATH_INLINE2(long double,long double *,func,cf##l)
#  define _STLP_DEF_MATH_INLINE2PI(func,cf) \
  _STLP_MATH_INLINE2(float,int *,func,cf##f) \
  _STLP_MATH_INLINE2_D(double,int *,func,cf) \
  _STLP_MATH_INLINE2(long double,int *,func,cf##l)
#  define _STLP_DEF_MATH_INLINE2I(func,cf) \
  _STLP_MATH_INLINE2(float,int,func,cf##f) \
  _STLP_MATH_INLINE2_D(double,int,func,cf) \
  _STLP_MATH_INLINE2(long double,int,func,cf##l)
#else // !_STLP_NO_LONG_DOUBLE
#  if !defined (_STLP_NO_LONG_DOUBLE)
#    if !defined (_STLP_NO_VENDOR_MATH_F)
#      define _STLP_DEF_MATH_INLINE(func,cf) \
      _STLP_MATH_INLINE(float,func,cf##f) \
      _STLP_MATH_INLINEX(long double,func,cf)
#      define _STLP_DEF_MATH_INLINE2(func,cf) \
      _STLP_MATH_INLINE2(float,float,func,cf##f) \
      _STLP_MATH_INLINE2XX(long double,func,cf)
#      define _STLP_DEF_MATH_INLINE2P(func,cf) \
      _STLP_MATH_INLINE2(float,float *,func,cf##f) \
      _STLP_MATH_INLINE2PX(long double,func,cf)
#      define _STLP_DEF_MATH_INLINE2PI(func,cf) \
      _STLP_MATH_INLINE2(float,int *,func,cf##f) \
      _STLP_MATH_INLINE2X(long double,int *,func,cf)
#      define _STLP_DEF_MATH_INLINE2I(func,cf) \
      _STLP_MATH_INLINE2(float,int,func,cf##f) \
      _STLP_MATH_INLINE2X(long double,int,func,cf)
#    else
#      define _STLP_DEF_MATH_INLINE(func,cf) \
      _STLP_MATH_INLINEX(float,func,cf) \
      _STLP_MATH_INLINEX(long double,func,cf)
#      define _STLP_DEF_MATH_INLINE2(func,cf) \
      _STLP_MATH_INLINE2XX(float,func,cf) \
      _STLP_MATH_INLINE2XX(long double,func,cf)
#      define _STLP_DEF_MATH_INLINE2P(func,cf) \
      _STLP_MATH_INLINE2PX(float,func,cf) \
      _STLP_MATH_INLINE2PX(long double,func,cf)
#      define _STLP_DEF_MATH_INLINE2PI(func,cf) \
      _STLP_MATH_INLINE2X(float,int *,func,cf) \
      _STLP_MATH_INLINE2X(long double,int *,func,cf)
#      define _STLP_DEF_MATH_INLINE2I(func,cf) \
      _STLP_MATH_INLINE2X(float,int,func,cf) \
      _STLP_MATH_INLINE2X(long double,int,func,cf)
#    endif
#  else
#    if !defined (_STLP_NO_VENDOR_MATH_F)
#      define _STLP_DEF_MATH_INLINE(func,cf) \
      _STLP_MATH_INLINE(float,func,cf##f)
#      define _STLP_DEF_MATH_INLINE2(func,cf) \
      _STLP_MATH_INLINE2(float,float,func,cf##f)
#      define _STLP_DEF_MATH_INLINE2P(func,cf) \
      _STLP_MATH_INLINE2(float,float *,func,cf##f)
#      define _STLP_DEF_MATH_INLINE2PI(func,cf) \
      _STLP_MATH_INLINE2(float,int *,func,cf##f)
#      define _STLP_DEF_MATH_INLINE2I(func,cf) \
      _STLP_MATH_INLINE2(float,int,func,cf##f)
#    else // _STLP_NO_VENDOR_MATH_F
#      define _STLP_DEF_MATH_INLINE(func,cf) \
      _STLP_MATH_INLINEX(float,func,cf)
#      define _STLP_DEF_MATH_INLINE2(func,cf) \
      _STLP_MATH_INLINE2XX(float,func,cf)
#      define _STLP_DEF_MATH_INLINE2P(func,cf) \
      _STLP_MATH_INLINE2PX(float,func,cf)
#      define _STLP_DEF_MATH_INLINE2PI(func,cf) \
      _STLP_MATH_INLINE2X(float,int *,func,cf)
#      define _STLP_DEF_MATH_INLINE2I(func,cf) \
      _STLP_MATH_INLINE2X(float,int,func,cf)
#    endif // _STLP_NO_VENDOR_MATH_F
#  endif
#endif // !_STLP_NO_LONG_DOUBLE

#if defined (_STLP_WCE) || \
   (defined(_STLP_MSVC) && (_STLP_MSVC <= 1300) && defined (_MSC_EXTENSIONS) /* && !defined(_STLP_WCE_NET) */)
/*
 * dums: VC6 has all the required C++ functions but only define them if
 * _MSC_EXTENSIONS is not defined (a bug?). STLport just do the same
 * thing also when _MSC_EXTENSIONS is defined.
 */
//We have to tell the compilers that abs, acos ... math functions are not intrinsic
//otherwise we have Internal Compiler Error in release mode...
#  pragma warning(push)
#  pragma warning(disable: 4162) // no function with C linkage found
#  pragma warning(disable: 4163) // not available as an intrinsic function
#  pragma function (abs, acos, asin, atan, atan2, cos, cosh, exp, fabs, fmod, log, log10, sin, sinh, sqrt, tan, tanh)
#  if defined (_STLP_WCE)
#    pragma function (ceil, floor)
#  endif
#  define _STLP_RESTORE_FUNCTION_INTRINSIC
#endif // _STLP_MSVC && _STLP_MSVC <= 1300 && !_STLP_WCE && _MSC_EXTENSIONS

//MSVC starting with .Net 2003 has already all math functions in global namespace.
//As Intel C++ compiler icl include MSVC headers it also have all math functions in ::
// so we use _MSC_VER rather than _STLP_MSVC
#if !defined (_MSC_VER) || (_MSC_VER < 1310)
#  if !(defined(__sun) && defined(__SUNPRO_CC))
inline double abs(double __x) { return ::fabs(__x); }
#  endif
#  ifndef __MVS__
_STLP_DEF_MATH_INLINE(abs,fabs)
#  else // __MVS__ has native long double abs?
inline float abs(float __x) { return ::fabsf(__x); }
#  endif

_STLP_DEF_MATH_INLINE(acos,acos)
_STLP_DEF_MATH_INLINE(asin,asin)
_STLP_DEF_MATH_INLINE(atan,atan)
_STLP_DEF_MATH_INLINE2(atan2,atan2)
_STLP_DEF_MATH_INLINE(ceil,ceil)
_STLP_DEF_MATH_INLINE(cos,cos)
_STLP_DEF_MATH_INLINE(cosh,cosh)
_STLP_DEF_MATH_INLINE(exp,exp)
_STLP_DEF_MATH_INLINE(fabs,fabs)
_STLP_DEF_MATH_INLINE(floor,floor)
_STLP_DEF_MATH_INLINE2(fmod,fmod)
_STLP_DEF_MATH_INLINE2PI(frexp,frexp)

_STLP_DEF_MATH_INLINE2I(ldexp,ldexp)
_STLP_DEF_MATH_INLINE(log,log)
_STLP_DEF_MATH_INLINE(log10,log10)
_STLP_DEF_MATH_INLINE2P(modf,modf)

#  if 0 // Unknown OS, where float/long double modf missed
// fbp : float versions are not always available

// Context of define of _STLP_VENDOR_LONG_DOUBLE_MATH should be changed:
// many OS has *l math functions... -ptr

#    if !defined(_STLP_VENDOR_LONG_DOUBLE_MATH)  //*ty 11/25/2001 - 
inline float modf (float __x, float* __y)     { 
  double __dd[2]; 
  double __res = ::modf((double)__x, __dd); 
  __y[0] = (float)__dd[0] ; __y[1] = (float)__dd[1]; 
  return (float)__res; 
}
#    else  //*ty 11/25/2001 - i.e. for apple SCpp
inline float modf (float __x, float* __y)     { 
  long double __dd[2]; 
  long double __res = ::modfl(__STATIC_CAST(long double,__x), __dd); 
  __y[0] = __STATIC_CAST(float,__dd[0]); __y[1] = __STATIC_CAST(float,__dd[1]); 
  return __STATIC_CAST(float,__res);
}
#    endif  //*ty 11/25/2001 - 
// fbp : long double versions are not available
inline long double modf (long double __x, long double* __y) { 
  double __dd[2]; 
  double __res = ::modf((double)__x, __dd); 
  __y[0] = (long double)__dd[0] ; __y[1] = (long double)__dd[1]; 
  return (long double)__res; 
}
#  endif // 0

_STLP_DEF_MATH_INLINE2(pow,pow)

#  if !defined(_STLP_MSVC) /* || (_STLP_MSVC > 1300) */ || defined(_STLP_WCE) || !defined (_MSC_EXTENSIONS) /* && !defined(_STLP_WCE_NET) */
#    ifndef _STLP_NO_VENDOR_MATH_F
#      ifndef __sun
inline float       pow(float __x, int __y)       { return ::powf(__x, __STATIC_CAST(float,__y)); }
#      else
#        ifndef __SUNPRO_CC
inline float       pow(float __x, int __y)       { return ::__powf(__x, __STATIC_CAST(float,__y)); }
#        else
inline float       pow(float __x, int __y)       { return _STLP_VENDOR_CSTD::__powf(__x, __STATIC_CAST(float,__y)); }
#        endif
#      endif
#    else
inline float       pow(float __x, int __y)       { return __STATIC_CAST(float,::pow(__x, __STATIC_CAST(float,__y))); }
#    endif
inline double      pow(double __x, int __y)      { return ::pow(__x, __STATIC_CAST(double,__y)); }
#    if !defined (_STLP_NO_LONG_DOUBLE)
#      if !defined(_STLP_NO_VENDOR_MATH_L)
#        ifndef __sun
inline long double pow(long double __x, int __y) { return ::powl(__x, __STATIC_CAST(long double,__y)); }
#        else
#          ifndef __SUNPRO_CC
inline long double pow(long double __x, int __y) { return ::__powl(__x, __STATIC_CAST(long double,__y)); }
#          else
inline long double pow(long double __x, int __y) { return _STLP_VENDOR_CSTD::__powl(__x, __STATIC_CAST(long double,__y)); }
#          endif
#        endif
#      else
inline long double pow(long double __x, int __y) { return __STATIC_CAST(long double,::pow(__x, __STATIC_CAST(long double,__y))); }
#      endif
#    endif
#  else
//The MS native pow version has a bugged overload so it is not imported
//in the STLport namespace.
//Here is the bugged version:
//inline double pow(int __x, int __y)            { return (_Pow_int(__x, __y)); }
inline double      pow(double __x, int __y)      { return (_Pow_int(__x, __y)); }
inline float       pow(float __x, int __y)       { return (_Pow_int(__x, __y)); }
inline long double pow(long double __x, int __y) { return (_Pow_int(__x, __y)); }
#  endif

_STLP_DEF_MATH_INLINE(sin,sin)
_STLP_DEF_MATH_INLINE(sinh,sinh)
_STLP_DEF_MATH_INLINE(sqrt,sqrt)
_STLP_DEF_MATH_INLINE(tan,tan)
_STLP_DEF_MATH_INLINE(tanh,tanh)

#endif

#if (defined (_STLP_MSVC) && !defined (_STLP_WCE)) || defined (__ICL) || defined(__sun)
#  ifdef __SUNPRO_CC
_STLP_MATH_INLINE2XX(double,hypot,hypot)
#  endif
_STLP_MATH_INLINE2XX(float,hypot,hypot)
inline long double hypot(long double x, long double y) { return sqrt(x*x + y*y); }
#else
#  if defined (_STLP_USE_UCLIBC) || defined (_STLP_WCE)
inline double hypot(double x, double y) { return sqrt(x*x + y*y); }
#  endif
_STLP_DEF_MATH_INLINE2(hypot,hypot)
#endif

#if defined (_STLP_RESTORE_FUNCTION_INTRINSIC)
//restoration of the default intrinsic status of those functions:
#  pragma intrinsic (abs, acos, asin, atan, atan2, cos, cosh, exp, fabs, fmod, log, log10, sin, sinh, sqrt, tan, tanh)
#  if defined (_STLP_WCE)
#    pragma intrinsic (ceil, floor)
#  endif
#  pragma warning(pop)
#  undef _STLP_RESTORE_FUNCTION_INTRINSIC
#endif // _STLP_MSVC && _STLP_MSVC <= 1300 && !_STLP_WCE && _MSC_EXTENSIONS

#if defined (_STLP_IMPORT_VENDOR_CSTD) && !defined (_STLP_NO_CSTD_FUNCTION_IMPORTS)
_STLP_BEGIN_NAMESPACE
using ::abs;
using ::acos;
using ::asin;
using ::atan;
using ::atan2;
using ::ceil;
using ::cos;
using ::cosh;
using ::exp;
using ::fabs;
using ::floor;
using ::fmod;
using ::frexp;
using ::hypot;
using ::ldexp;
using ::log;
using ::log10;
using ::modf;
using ::pow;
using ::sin;
using ::sinh;
using ::sqrt;
using ::tan;
using ::tanh;

#if defined(__GNUC__) && (__GNUC__ < 3)
inline double abs(double __x) { return ::fabs(__x); }
_STLP_DEF_MATH_INLINE(abs,fabs)
#endif

_STLP_END_NAMESPACE
#endif

#endif /* CMATH_H */

// Local Variables:
// mode:C++
// End:
