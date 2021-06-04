//  (C) Copyright Gennadiy Rozental 2001-2003.
//  Use, modification, and distribution are subject to the 
//  Boost Software License, Version 1.0. (See accompanying file 
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org/libs/test for the library home page.

// Boost.Test
#include <boost/test/test_tools.hpp>

int add( int i, int j ) { return i+j; }

int test_main( int, char* [] )  // note the name!
{
    // six ways to detect and report the same error:
    BOOST_CHECK( add(2,2) == 4 );          // #1 continues on error

    BOOST_REQUIRE( add(2,2) == 4 );        // #2 throws on error

    if ( add(2,2) != 4 )
        BOOST_ERROR( "Ouch...");           // #3 continues on error

    if ( add(2,2) != 4 )
        BOOST_FAIL( "Ouch..." );           // #4 throws on error

    if ( add(2,2) != 4 )
        throw "Oops...";                   // #5 throws on error

    return add(2,2) == 4 ? 0 : 1;          // #6 returns error code
}

// EOF
