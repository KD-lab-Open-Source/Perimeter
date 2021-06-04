//  (C) Copyright Gennadiy Rozental 2002-2003.
//  (C) Copyright Gennadiy Rozental & Ullrich Koethe 2001.
//  Use, modification, and distribution are subject to the 
//  Boost Software License, Version 1.0. (See accompanying file 
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org/libs/test for the library home page.

// Boost.Test
#include <boost/test/unit_test.hpp>
using boost::unit_test_framework::test_suite;

// STL
#include <vector>
#include <string>

void check_string( std::string const& s )
{
    // reports 'error in "check_string": test s.substr( 0, 3 ) == "hdr" failed [actual_value != hdr]'
    BOOST_CHECK_EQUAL( s.substr( 0, 3 ), "hdr" );
}

namespace {
    std::string const params[] = { "hdr1 ", "hdr2", "3  " };
}

test_suite*
init_unit_test_suite( int /*argc*/, char* /*argv*/[] ) {
    test_suite* test= BOOST_TEST_SUITE( "Unit test example 4" );

    test->add( BOOST_PARAM_TEST_CASE( &check_string, (std::string const*)params, params+3 ), 1 );

    return test; 
}

// EOF
