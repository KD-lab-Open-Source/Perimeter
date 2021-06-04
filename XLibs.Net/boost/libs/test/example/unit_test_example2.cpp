//  (C) Copyright Gennadiy Rozental 2002-2003.
//  (C) Copyright Gennadiy Rozental & Ullrich Koethe 2001.
//  Use, modification, and distribution are subject to the 
//  Boost Software License, Version 1.0. (See accompanying file 
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org/libs/test for the library home page.

// Boost.Test
#include <boost/test/unit_test.hpp>
using boost::unit_test_framework::test_suite;

void force_division_by_zero()
{
    // unit test framework can catch operating system signals
    BOOST_CHECKPOINT("About to force division by zero!");
    int i = 1, j = 0;
    i = i / j;
}

void infinite_loop()
{
    // unit test framework can break infinite loops by timeout
#ifdef __unix  // don't have timeout on other platforms
    BOOST_CHECKPOINT("About to enter an infinite loop!");
    while(1);
#else
    BOOST_MESSAGE( "Timeout support is not implemented on your platform" );
#endif
}

test_suite*
init_unit_test_suite( int argc, char * argv[] ) {
    test_suite* test= BOOST_TEST_SUITE( "Unit test example 2" );

    test->add( BOOST_TEST_CASE( &force_division_by_zero ) );
    test->add( BOOST_TEST_CASE( &infinite_loop ), 0, /* timeout */ 2 );

    return test; 
}

// EOF
