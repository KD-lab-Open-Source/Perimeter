
//  (C) Copyright John Maddock 2000. 
//  Use, modification and distribution are subject to the 
//  Boost Software License, Version 1.0. (See accompanying file 
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "test.hpp"
#include "check_integral_constant.hpp"
#ifdef TEST_STD
#  include <type_traits>
#else
#  include <boost/type_traits/is_member_function_pointer.hpp>
#endif

TT_TEST_BEGIN(is_member_function_pointer)

BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_member_function_pointer<f1>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_member_function_pointer<f2>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_member_function_pointer<f3>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_member_function_pointer<void*>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_member_function_pointer<mf1>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_member_function_pointer<mf2>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_member_function_pointer<mf3>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_member_function_pointer<mf4>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_member_function_pointer<cmf>::value, true);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_member_function_pointer<mp>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_member_function_pointer<void>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_member_function_pointer<test_abc1>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_member_function_pointer<incomplete_type>::value, false);
BOOST_CHECK_INTEGRAL_CONSTANT(::tt::is_member_function_pointer<foo0_t>::value, false);

TT_TEST_END








