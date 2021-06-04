//  (C) Copyright Gennadiy Rozental 2001-2003.
//  (C) Copyright Beman Dawes 2000.
//  Use, modification, and distribution are subject to the 
//  Boost Software License, Version 1.0. (See accompanying file 
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org/libs/test for the library home page.
//
//  File        : $RCSfile: test_exec_fail2.cpp,v $
//
//  Version     : $Revision: 1.11 $
//
//  Description : test failures reported by different Test Tools.
//  Should fail during run.
// ***************************************************************************

// Boost.Test
#include <boost/test/test_tools.hpp>

int test_main( int, char *[] )  // note the name
{
    int v = 1;

    if( v < 10 ) { // to eliminate unreachable return statement warning
        BOOST_CHECK( v == 2 );
        BOOST_ERROR( "sample BOOST_ERROR call" );
        BOOST_REQUIRE( 2 == v );

        throw "Opps! should never reach this point";
    }

    return 1;
}

//____________________________________________________________________________//

// ***************************************************************************
//  Revision History :
//  
//  $Log: test_exec_fail2.cpp,v $
//  Revision 1.11  2003/12/01 00:42:38  rogeeff
//  prerelease cleaning
//

// ***************************************************************************

// EOF
