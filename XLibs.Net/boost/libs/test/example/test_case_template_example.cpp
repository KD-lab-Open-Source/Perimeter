//  (C) Copyright Gennadiy Rozental 2001-2003.
//  Use, modification, and distribution are subject to the 
//  Boost Software License, Version 1.0. (See accompanying file 
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org/libs/test for the library home page.

// Boost.Test
#include <boost/test/test_tools.hpp>
#include <boost/test/test_case_template.hpp>
using boost::unit_test_framework::test_suite;

// Boost.MPL
#include <boost/mpl/range_c.hpp>

template<typename Number>
void free_test_function( Number* = 0 ) {
    BOOST_CHECK_EQUAL( 2, (int const)Number::value );
}

BOOST_META_FUNC_TEST_CASE( free_test_function );

test_suite*
init_unit_test_suite( int, char* [] ) {
    test_suite* test= BOOST_TEST_SUITE( "Test case template example" );

    typedef boost::mpl::range_c<int,0,10> numbers;

    test->add( BOOST_FUNC_TEMPLATE_TEST_CASE( free_test_function, numbers ) );

    return test;
}

// EOF
