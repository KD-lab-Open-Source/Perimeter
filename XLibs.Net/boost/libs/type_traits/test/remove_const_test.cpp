
//  (C) Copyright John Maddock 2000. 
//  Use, modification and distribution are subject to the 
//  Boost Software License, Version 1.0. (See accompanying file 
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "test.hpp"
#include "check_type.hpp"
#ifdef TEST_STD
#  include <type_traits>
#else
#  include <boost/type_traits/remove_const.hpp>
#endif

BOOST_DECL_TRANSFORM_TEST3(remove_const_test_1, ::boost::remove_const, const)
BOOST_DECL_TRANSFORM_TEST(remove_const_test_2, ::boost::remove_const, volatile, volatile)
BOOST_DECL_TRANSFORM_TEST(remove_const_test_3, ::boost::remove_const, const volatile, volatile)
BOOST_DECL_TRANSFORM_TEST0(remove_const_test_4, ::boost::remove_const)
BOOST_DECL_TRANSFORM_TEST(remove_const_test_6, ::boost::remove_const, *const, *)
BOOST_DECL_TRANSFORM_TEST(remove_const_test_7, ::boost::remove_const, *volatile, *volatile)
BOOST_DECL_TRANSFORM_TEST(remove_const_test_8, ::boost::remove_const, *const volatile, *volatile)
BOOST_DECL_TRANSFORM_TEST(remove_const_test_9, ::boost::remove_const, *, *)
BOOST_DECL_TRANSFORM_TEST(remove_const_test_11, ::boost::remove_const, volatile*, volatile*)
BOOST_DECL_TRANSFORM_TEST(remove_const_test_12, ::boost::remove_const, const[2], [2])
BOOST_DECL_TRANSFORM_TEST(remove_const_test_13, ::boost::remove_const, volatile[2], volatile[2])
BOOST_DECL_TRANSFORM_TEST(remove_const_test_14, ::boost::remove_const, const volatile[2], volatile[2])
BOOST_DECL_TRANSFORM_TEST(remove_const_test_15, ::boost::remove_const, [2], [2])
BOOST_DECL_TRANSFORM_TEST(remove_const_test_16, ::boost::remove_const, const*, const*)
BOOST_DECL_TRANSFORM_TEST(remove_const_test_17, ::boost::remove_const, const*const, const*)

TT_TEST_BEGIN(remove_const)

BOOST_CHECK_TYPE(int, int);   

   remove_const_test_1();
   remove_const_test_2();
   remove_const_test_3();
   remove_const_test_4();
   remove_const_test_6();
   remove_const_test_7();
   remove_const_test_8();
   remove_const_test_9();
   remove_const_test_11();
   remove_const_test_12();
   remove_const_test_13();
   remove_const_test_14();
   remove_const_test_15();
   remove_const_test_16();
   remove_const_test_17();

TT_TEST_END








