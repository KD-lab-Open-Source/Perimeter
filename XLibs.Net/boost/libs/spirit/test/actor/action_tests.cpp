/*=============================================================================
    Copyright (c) 2003 Jonathan de Halleux (dehalleux@pelikhan.com)
    http://spirit.sourceforge.net/

    Use, modification and distribution is subject to the Boost Software
    License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#include "action_tests.hpp"
#include <boost/test/unit_test.hpp>

boost::unit_test_framework::test_suite*
init_unit_test_suite( int /*argc*/, char* /*argv*/[] )
{
    boost::unit_test_framework::test_suite* test=
        BOOST_TEST_SUITE( "Spirit Actor Test Suite" );

    assign_action_test(test);
    assign_key_action_test(test);
    clear_action_test(test);
    decrement_action_test(test);
    erase_action_test(test);
    increment_action_test(test);
    insert_key_action_test(test);
    push_front_action_test(test);
    push_back_action_test(test);
    swap_action_test(test);

    return test;
}
