//  (C) Copyright Gennadiy Rozental 2003.
//  Use, modification, and distribution are subject to the 
//  Boost Software License, Version 1.0. (See accompanying file 
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org/libs/test for the library home page.
//
//  File        : $RCSfile: test_case_template_test.cpp,v $
//
//  Version     : $Revision: 1.5 $
//
//  Description : tests function template test case
// ***************************************************************************

// Boost.Test
#include <boost/test/unit_test_suite.hpp>
#include <boost/test/unit_test_result.hpp>
#include <boost/test/test_tools.hpp>
#include <boost/test/test_case_template.hpp>

#if !defined ( __GNUC__ ) || __GNUC__ > 2
#include <boost/test/detail/nullstream.hpp>
typedef boost::onullstream onullstream_type;
#else
typedef boost::test_toolbox::output_test_stream onullstream_type;
#endif

// Boost
#include <boost/mpl/range_c.hpp>
#include <boost/mpl/list_c.hpp>
#include <boost/scoped_ptr.hpp>

namespace tf  = boost::unit_test_framework;
namespace mpl = boost::mpl;

// STL
#include <iostream>

//____________________________________________________________________________//

template<typename Number>
void test0( Number* = 0 )
{
    BOOST_CHECK_EQUAL( 2, Number::value );
}

BOOST_META_FUNC_TEST_CASE( test0 );

//____________________________________________________________________________//

template<typename Number>
void test1( Number* = 0 )
{
    BOOST_CHECK_EQUAL( 6, Number::value );

    BOOST_REQUIRE( 2 <= Number::value );

    BOOST_CHECK_EQUAL( 3, Number::value );
}

BOOST_META_FUNC_TEST_CASE( test1 );

//____________________________________________________________________________//

template<typename Number>
void test2( Number* = 0 )
{
    throw Number();
}

BOOST_META_FUNC_TEST_CASE( test2 );

//____________________________________________________________________________//

int test_main( int, char* [] ) 
{
    tf::unit_test_counter               num_of_failures;
    bool                                exception_caught;
    onullstream_type                    null_output;
    boost::scoped_ptr<tf::test_case>    test;  

    tf::unit_test_log::instance().set_log_stream( null_output );

    {
        tf::unit_test_result_saver saver;
        typedef boost::mpl::list_c<int,2,2,2,2,2,2,2> only_2;
        test.reset( BOOST_FUNC_TEMPLATE_TEST_CASE( test0, only_2  ) );

        test->run();

        tf::unit_test_result::instance().failures_details( num_of_failures, exception_caught );
    }

    tf::unit_test_log::instance().set_log_stream( std::cout );
    BOOST_CHECK_EQUAL( num_of_failures, (std::size_t)0 );
    BOOST_CHECK( !exception_caught );

    tf::unit_test_log::instance().set_log_stream( null_output );

    {
    tf::unit_test_result_saver saver;
    typedef boost::mpl::range_c<int,0,10> one_to_ten;
    test.reset( BOOST_FUNC_TEMPLATE_TEST_CASE( test0, one_to_ten ) );

    test->run();

    tf::unit_test_result::instance().failures_details( num_of_failures, exception_caught );
    }

    tf::unit_test_log::instance().set_log_stream( std::cout );
    BOOST_CHECK_EQUAL( num_of_failures, (std::size_t)9 );
    BOOST_CHECK( !exception_caught );

    tf::unit_test_log::instance().set_log_stream( null_output );

    {
        tf::unit_test_result_saver saver;
        typedef boost::mpl::range_c<int,1,5> one_to_five;
        test.reset( BOOST_FUNC_TEMPLATE_TEST_CASE( test1, one_to_five ) );

        test->run();

        tf::unit_test_result::instance().failures_details( num_of_failures, exception_caught );
    }

    tf::unit_test_log::instance().set_log_stream( std::cout );
    BOOST_CHECK_EQUAL( num_of_failures, (std::size_t)7 );
    BOOST_CHECK( !exception_caught );

    tf::unit_test_log::instance().set_log_stream( null_output );
    bool passed = false;
    {
        tf::unit_test_result_saver saver;
        typedef boost::mpl::range_c<int,1,3> one_to_three;
        test.reset( BOOST_FUNC_TEMPLATE_TEST_CASE( test2, one_to_three ) );

        test->run();

        tf::unit_test_result::instance().failures_details( num_of_failures, exception_caught );
        passed = tf::unit_test_result::instance().has_passed();
    }

    tf::unit_test_log::instance().set_log_stream( std::cout );
    BOOST_CHECK_EQUAL( num_of_failures, (std::size_t)0 );
    BOOST_CHECK( !exception_caught );
    BOOST_CHECK( !passed );

    return 0;
}

//____________________________________________________________________________//

// ***************************************************************************
//  Revision History :
//  
//  $Log: test_case_template_test.cpp,v $
//  Revision 1.5  2003/12/01 00:42:38  rogeeff
//  prerelease cleaning
//

// ***************************************************************************

// EOF
