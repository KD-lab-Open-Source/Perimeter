
//  (C) Copyright John Maddock 2000. 
//  Use, modification and distribution are subject to the 
//  Boost Software License, Version 1.0. (See accompanying file 
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "test.hpp"
#include "check_integral_constant.hpp"
#ifdef TEST_STD
#  include <type_traits>
#else
#  include <boost/type_traits/is_convertible.hpp>
#endif

template <class T>
struct convertible_from
{
    convertible_from(T);
};

TT_TEST_BEGIN(is_convertible)

BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_convertible<Derived,Base>::value), true);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_convertible<Derived,Derived>::value), true);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_convertible<Base,Base>::value), true);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_convertible<Base,Derived>::value), false);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_convertible<Derived,Derived>::value), true);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_convertible<NonDerived,Base>::value), false);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_convertible<float,int>::value), true);
   
// The following four do not compile without member template support:
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_convertible<float,void>::value), false);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_convertible<void,void>::value), true);

BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_convertible<enum1, int>::value), true);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_convertible<Derived*, Base*>::value), true);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_convertible<Base*, Derived*>::value), false);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_convertible<Derived&, Base&>::value), true);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_convertible<Base&, Derived&>::value), false);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_convertible<const Derived*, const Base*>::value), true);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_convertible<const Base*, const Derived*>::value), false);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_convertible<const Derived&, const Base&>::value), true);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_convertible<const Base&, const Derived&>::value), false);

BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_convertible<const int *, int*>::value), false);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_convertible<const int&, int&>::value), false);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_convertible<const int*, int[3]>::value), false);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_convertible<const int&, int>::value), true);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_convertible<int(&)[4], const int*>::value), true);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_convertible<int(&)(int), int(*)(int)>::value), true);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_convertible<int *, const int*>::value), true);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_convertible<int&, const int&>::value), true);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_convertible<int[2], int*>::value), true);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_convertible<int[2], const int*>::value), true);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_convertible<const int[2], int*>::value), false);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_convertible<int*, int[3]>::value), false);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_convertible<test_abc3, const test_abc1&>::value), true);

BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_convertible<non_pointer, void*>::value), true);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_convertible<non_pointer, int*>::value), false);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_convertible<non_int_pointer, int*>::value), true);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_convertible<non_int_pointer, void*>::value), true);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_convertible<test_abc1&, test_abc2&>::value), false);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_convertible<test_abc1&, int_constructible>::value), false);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_convertible<int_constructible, test_abc1&>::value), false);

//
// the following tests all involve user defined conversions which do
// not compile with Borland C++ Builder 5:
//
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_convertible<int, int_constructible>::value), true);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_convertible<float,convertible_from<float> >::value), true);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_convertible<float,convertible_from<float const&> >::value), true);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_convertible<float,convertible_from<float&> >::value), true);

BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_convertible<float,convertible_from<char> >::value), true);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_convertible<float,convertible_from<char const&> >::value), true);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_convertible<float,convertible_from<char&> >::value), false);

BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_convertible<char,convertible_from<char> >::value), true);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_convertible<char,convertible_from<char const&> >::value), true);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_convertible<char,convertible_from<char&> >::value), true);

BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_convertible<float&,convertible_from<float> >::value), true);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_convertible<float const&,convertible_from<float> >::value), true);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_convertible<float&,convertible_from<float&> >::value), true);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_convertible<float const&,convertible_from<float const&> >::value), true);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_convertible<float&,convertible_from<float const&> >::value), true);

//
// the following all generate warnings unless we can find a way to
// suppress them:
//
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_convertible<float,int>::value), true);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_convertible<double,int>::value), true);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_convertible<double,float>::value), true);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_convertible<long,int>::value), true);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_convertible<int,char>::value), true);


TT_TEST_END








