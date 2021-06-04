//  (C) Copyright Gennadiy Rozental 2001-2003.
//  Use, modification, and distribution are subject to the 
//  Boost Software License, Version 1.0. (See accompanying file 
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org/libs/test for the library home page.
//
//  File        : $RCSfile: auto_unit_test_test_mult1.cpp,v $
//
//  Version     : $Revision: 1.3 $
//
//  Description : test auto unit test facility to work with multiple compilation units
// ***************************************************************************

// Boost.Test
#define BOOST_AUTO_TEST_MAIN
#include <boost/test/auto_unit_test.hpp>

BOOST_AUTO_UNIT_TEST( test )
{
    BOOST_CHECK( true );
}

//____________________________________________________________________________//

// ***************************************************************************
//  Revision History :
//  
//  $Log: auto_unit_test_test_mult1.cpp,v $
//  Revision 1.3  2003/12/01 00:42:37  rogeeff
//  prerelease cleaning
//

// ***************************************************************************

// EOF
