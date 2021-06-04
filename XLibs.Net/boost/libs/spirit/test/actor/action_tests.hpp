/*=============================================================================
    Copyright (c) 2003 Jonathan de Halleux (dehalleux@pelikhan.com)
    http://spirit.sourceforge.net/

    Use, modification and distribution is subject to the Boost Software
    License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#ifndef BOOST_SPIRIT_XOR_ASSIGN_ACTOR_TEST_HPP
#define BOOST_SPIRIT_XOR_ASSIGN_ACTOR_TEST_HPP

namespace boost{ namespace unit_test_framework{
    class test_suite;
}
}

///////////////////////////////////////////////////////////////////////////////
// Test suite for actors
///////////////////////////////////////////////////////////////////////////////
void assign_action_test(boost::unit_test_framework::test_suite* test);
void assign_key_action_test(boost::unit_test_framework::test_suite* test);
void clear_action_test(boost::unit_test_framework::test_suite* test);
void decrement_action_test(boost::unit_test_framework::test_suite* test);
void erase_action_test(boost::unit_test_framework::test_suite* test);
void increment_action_test(boost::unit_test_framework::test_suite* test);
void insert_key_action_test(boost::unit_test_framework::test_suite* test);
void push_back_action_test(boost::unit_test_framework::test_suite* test);
void push_front_action_test(boost::unit_test_framework::test_suite* test);
void swap_action_test(boost::unit_test_framework::test_suite* test);

#endif
