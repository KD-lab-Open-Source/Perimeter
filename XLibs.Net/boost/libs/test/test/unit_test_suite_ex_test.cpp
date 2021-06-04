//  (C) Copyright Gennadiy Rozental 2001-2003.
//  Use, modification, and distribution are subject to the 
//  Boost Software License, Version 1.0. (See accompanying file 
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org/libs/test for the library home page.
//
//  File        : $RCSfile: unit_test_suite_ex_test.cpp,v $
//
//  Version     : $Revision: 1.13 $
//
//  Description : tests an extentions to Unit Test Framework
// ***************************************************************************

// Boost.Test
#include <boost/test/unit_test_suite_ex.hpp>
#include <boost/test/test_tools.hpp>

using boost::unit_test_framework::test_suite;

#if !defined(BOOST_MSVC) && !defined(__SUNPRO_CC)
// BOOST
#include <boost/bind.hpp>
#endif

#include <boost/function.hpp>
using namespace boost;

// STL
#include <list>
#include <utility>

//____________________________________________________________________________//

void test0()
{
    BOOST_MESSAGE( "Hello there" );
}

//____________________________________________________________________________//

void test1( int arg )
{
    BOOST_CHECK( (arg & 0x80) == 0 );
}

//____________________________________________________________________________//

void test2( int arg, int mask )
{
    BOOST_CHECK( (arg & mask) != 0 );
}

//____________________________________________________________________________//

struct sub_test_suite : public test_suite {
    typedef std::list<std::pair<int,int> > mask_list_type;
    sub_test_suite()
    {
        parameters_list.push_back( 1 );
        parameters_list.push_back( 5 );
        parameters_list.push_back( 6 );
        parameters_list.push_back( 7 );
        parameters_list.push_back( 15 );

        masks_list.push_back( std::make_pair( 0x01, 1 ) );
        masks_list.push_back( std::make_pair( 0x04, 1 ) );
        masks_list.push_back( std::make_pair( 0x80, 5 ) );


#if !defined(BOOST_MSVC) && !defined(__SUNPRO_CC)
        for( mask_list_type::iterator it = masks_list.begin(); it != masks_list.end(); ++it ) {
            function1<void,int> fct = bind( &test2, _1, it->first );
            add( BOOST_PARAM_TEST_CASE( fct, parameters_list.begin(), parameters_list.end() ), it->second );
        }
#endif

        function1<void,int> fct = &test1;
        add( BOOST_PARAM_TEST_CASE( fct, parameters_list.begin(), parameters_list.end() ) );
    }

    
    std::list<int> parameters_list;
    mask_list_type masks_list; // mask/"num of errors expected" list
};

//____________________________________________________________________________//

test_suite*
init_unit_test_suite( int /*argc*/, char* /*argv*/[] ) {
    test_suite* test = BOOST_TEST_SUITE("unit_test_suite extensions test");

    function0<void> fct1 = &test0;
    test->add( BOOST_TEST_CASE( fct1 ) );

#if !defined(BOOST_MSVC) && !defined(__SUNPRO_CC)
    function0<void> fct2 = bind( &test2, 12345, 0xcdf );
    test->add( BOOST_TEST_CASE( fct2 ) );
#endif

    test->add( new sub_test_suite );

    return test;
}

//____________________________________________________________________________//

// ***************************************************************************
//  Revision History :
//  
//  $Log: unit_test_suite_ex_test.cpp,v $
//  Revision 1.13  2003/12/01 00:42:38  rogeeff
//  prerelease cleaning
//

// ***************************************************************************

// EOF
