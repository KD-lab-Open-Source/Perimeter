/*
 *
 * Copyright (c) 2004
 * Francois Dumont
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

#ifndef _STLP_BOOST_TYPE_TRAITS_H
#define _STLP_BOOST_TYPE_TRAITS_H

#include <boost/type_traits/is_integral.hpp>
#include <boost/type_traits/is_float.hpp>
#include <boost/type_traits/has_trivial_constructor.hpp>
#include <boost/type_traits/has_trivial_copy.hpp>
#include <boost/type_traits/has_trivial_assign.hpp>
#include <boost/type_traits/has_trivial_destructor.hpp>
#include <boost/type_traits/is_pod.hpp>
#include <boost/type_traits/is_pointer.hpp>
#include <boost/type_traits/is_reference.hpp>
#include <boost/type_traits/remove_cv.hpp>
#include <boost/type_traits/is_same.hpp>

/*
 * This file mostly wraps boost type_traits in the STLport type_traits.
 * When checking a type traits like trivial assign operator for instance
 * both the boost value and STLport values has to be taken into account
 * as we don't know what the user might have prefer, specializing the boost
 * type traits or the STLport one.
 */
_STLP_BEGIN_NAMESPACE

template <class _Tp> struct _IsRef {
  enum { _Ret = ::boost::is_reference<_Tp>::value };
};

template <class _Tp> struct _IsPtr {
  enum { _Ret = ::boost::is_pointer<_Tp>::value };
};

template <class _Tp> struct _IsPtrType {
  enum { is_pointer = ::boost::is_pointer<_Tp>::value };
  typedef typename __bool2type<is_pointer>::_Ret _Type;
  static _Type _Ret() { return _Type(); }
};

template <class _Tp> struct _Is_integer {
  enum { is_integral = ::boost::is_integral<_Tp>::value };
  typedef typename __bool2type<is_integral>::_Ret _Integral;
};

template <class _Tp> struct _Is_rational {
  enum { is_float = ::boost::is_float<_Tp>::value };
  typedef typename __bool2type<is_float>::_Ret _Rational;
};

template <class _Tp>
struct __type_traits {
  enum { trivial_constructor = ::boost::has_trivial_constructor<_Tp>::value };
  typedef typename __bool2type<trivial_constructor>::_Ret has_trivial_default_constructor;

  enum { trivial_copy = ::boost::has_trivial_copy<_Tp>::value };
  typedef typename __bool2type<trivial_copy>::_Ret has_trivial_copy_constructor;

  enum { trivial_assign = ::boost::has_trivial_assign<_Tp>::value };
  typedef typename __bool2type<trivial_assign>::_Ret has_trivial_assignment_operator;

  enum { trivial_destructor = ::boost::has_trivial_destructor<_Tp>::value };
  typedef typename __bool2type<trivial_destructor>::_Ret has_trivial_destructor;

  enum { pod = ::boost::is_pod<_Tp>::value };
  typedef typename __bool2type<pod>::_Ret is_POD_type;
};

template <class _Tp1, class _Tp2>
struct _BothPtrType {
  enum { pointer1 = ::boost::is_pointer<_Tp1>::value };
  enum { pointer2 = ::boost::is_pointer<_Tp2>::value };

  typedef typename __bool2type<pointer1 && pointer2>::_Ret _Type;
  static _Type _Ret() { return _Type(); }
};


template <class _Tp1, class _Tp2>
struct _OKToMemCpy {
  typedef typename ::boost::remove_cv<_Tp1>::type uncv1;
  typedef typename ::boost::remove_cv<_Tp2>::type uncv2;

  enum { same = ::boost::is_same<uncv1, uncv2>::value };
  enum { boost_trivial_assign = ::boost::has_trivial_assign<uncv1>::value };
  typedef typename __type_traits<uncv1>::has_trivial_assignment_operator _TrivialAssign;
  enum { stlport_trivial_assign = __type2bool<_TrivialAssign>::_Ret };
  enum { trivial_assign = boost_trivial_assign || stlport_trivial_assign };

  typedef typename __bool2type<same && trivial_assign>::_Ret _Type;
  static _Type _Answer() { return _Type(); }
};

template <class _Tp>
struct _TrivialUCopy {
  enum { boost_trivial_copy = ::boost::has_trivial_copy<_Tp>::value };
  typedef typename __type_traits<_Tp>::has_trivial_copy_constructor _TrivialCopy;
  enum { stlport_trivial_copy = __type2bool<_TrivialCopy>::_Ret };
  enum { trivial_copy = boost_trivial_copy || stlport_trivial_copy };

  enum { boost_trivial_assign = ::boost::has_trivial_assign<_Tp>::value };
  typedef typename __type_traits<_Tp>::has_trivial_assignment_operator _TrivialAssign;
  enum { stlport_trivial_assign = __type2bool<_TrivialAssign>::_Ret };
  enum { trivial_assign = boost_trivial_assign || stlport_trivial_assign };

  typedef typename __bool2type<trivial_copy && trivial_assign>::_Ret _Ret;
};

template <class _Tp1, class _Tp2>
struct _TrivialUCopyAux {
  enum { trivial_copy = _TrivialUCopy<_Tp1>::trivial_copy };
  enum { trivial_assign = _TrivialUCopy<_Tp1>::trivial_assign };

  typedef typename ::boost::remove_cv<_Tp1>::type uncv1;
  typedef typename ::boost::remove_cv<_Tp2>::type uncv2;
  enum { same = ::boost::is_same<uncv1, uncv2>::value };

  typedef typename __bool2type<trivial_copy && trivial_assign && same>::_Ret _Type;
  static _Type _Answer() { return _Type(); }
};

template <class _Tp>
struct _DefaultZeroValue {
  enum { is_integral = ::boost::is_integral<_Tp>::value };
  enum { is_float = ::boost::is_float<_Tp>::value };
  enum { is_pointer = ::boost::is_pointer<_Tp>::value };

  typedef typename __bool2type<is_integral || is_float || is_pointer>::_Ret _Type;
  static _Type _Answer() { return _Type(); }
};

_STLP_END_NAMESPACE

#endif /* _STLP_BOOST_TYPE_TRAITS_H */
