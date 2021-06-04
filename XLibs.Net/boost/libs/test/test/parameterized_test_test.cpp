//  (C) Copyright Gennadiy Rozental 2002-2003.
//  Use, modification, and distribution are subject to the 
//  Boost Software License, Version 1.0. (See accompanying file 
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org/libs/test for the library home page.
//
//  File        : $RCSfile: parameterized_test_test.cpp,v $
//
//  Version     : $Revision: 1.12 $
//
//  Description : tests parameterized tests
// ***************************************************************************

// Boost.Test
#include <boost/test/unit_test_suite.hpp>
#include <boost/test/unit_test_result.hpp>
#include <boost/test/test_tools.hpp>
#if !defined ( __GNUC__ ) || __GNUC__ > 2
#include <boost/test/detail/nullstream.hpp>
typedef boost::onullstream onullstream_type;
#else
typedef boost::test_toolbox::output_test_stream onullstream_type;
#endif

namespace utf=boost::unit_test_framework;

#include <list>
#include <iostream>
#include <boost/scoped_ptr.hpp>

//____________________________________________________________________________//

void test0( int i )
{
    BOOST_CHECK( i%2 == 0 );
}

//____________________________________________________________________________//

void test1( int i )
{
    BOOST_CHECK( i%2 == 0 );
    if( i%3 == 0 ) {
        throw 124;
    }
}

//____________________________________________________________________________//


#if BOOST_WORKAROUND(__BORLANDC__, BOOST_TESTED_AT(0x570))
#define BOOST_PARAM_TEST_CASE__( arg1, arg2, arg3 ) \
    boost::unit_test_framework::create_test_case<int*,int>( (arg1), std::string( "" ), (arg2), (arg3) )
#else
#define BOOST_PARAM_TEST_CASE__( arg1, arg2, arg3 ) BOOST_PARAM_TEST_CASE( arg1, arg2, arg3 )
#endif

int test_main( int, char* [] ) {
    utf::unit_test_counter              num_of_failures;
    bool                                exception_caught;
    bool                                passed;
    onullstream_type                    null_output;
    boost::scoped_ptr<utf::test_case>   test;  

    utf::unit_test_log::instance().set_log_stream( null_output );
    {
    utf::unit_test_result_saver saver;
    int test_data[] = { 2, 2, 2 };
    test.reset( BOOST_PARAM_TEST_CASE__( &test0, (int*)test_data, (int*)test_data + sizeof(test_data)/sizeof(int) ) );

    test->run();

    utf::unit_test_result::instance().failures_details( num_of_failures, exception_caught );
    }

    utf::unit_test_log::instance().set_log_stream( std::cout );
    BOOST_CHECK( num_of_failures == 0 );
    BOOST_CHECK( !exception_caught );

    utf::unit_test_log::instance().set_log_stream( null_output );
    {
    utf::unit_test_result_saver saver;
    int test_data[] = { 1, 2, 2 };    
    test.reset( BOOST_PARAM_TEST_CASE__( &test0, (int*)test_data, (int*)test_data + sizeof(test_data)/sizeof(int) ) );

    test->run();

    utf::unit_test_result::instance().failures_details( num_of_failures, exception_caught );
    }

    utf::unit_test_log::instance().set_log_stream( std::cout );
    BOOST_CHECK( num_of_failures == 1 );
    BOOST_CHECK( !exception_caught );

    utf::unit_test_log::instance().set_log_stream( null_output );
    {
    utf::unit_test_result_saver saver;
    int test_data[] = { 1, 1, 2 };    
    test.reset( BOOST_PARAM_TEST_CASE__( &test0, (int*)test_data, (int*)test_data + sizeof(test_data)/sizeof(int) ) );

    test->run();

    utf::unit_test_result::instance().failures_details( num_of_failures, exception_caught );
    }

    utf::unit_test_log::instance().set_log_stream( std::cout );
    BOOST_CHECK_EQUAL( num_of_failures, (std::size_t)2 );
    BOOST_CHECK( !exception_caught );

    utf::unit_test_log::instance().set_log_stream( null_output );
    {
    utf::unit_test_result_saver saver;
    int test_data[] = { 1, 1, 1 };    
    test.reset( BOOST_PARAM_TEST_CASE__( &test0, (int*)test_data, (int*)test_data + sizeof(test_data)/sizeof(int) ) );

    test->run();

    utf::unit_test_result::instance().failures_details( num_of_failures, exception_caught );
    }

    utf::unit_test_log::instance().set_log_stream( std::cout );
    BOOST_CHECK_EQUAL( num_of_failures, (std::size_t)3 );
    BOOST_CHECK( !exception_caught );

    utf::unit_test_log::instance().set_log_stream( null_output );
    {
    utf::unit_test_result_saver saver;
    int test_data[] = { 6, 6, 6 };    
    test.reset( BOOST_PARAM_TEST_CASE__( &test1, (int*)test_data, (int*)test_data + sizeof(test_data)/sizeof(int) ) );

    test->run();

    utf::unit_test_result::instance().failures_details( num_of_failures, exception_caught );
    passed = utf::unit_test_result::instance().has_passed();
    }

    utf::unit_test_log::instance().set_log_stream( std::cout );
    BOOST_CHECK_EQUAL( num_of_failures, (std::size_t)0 );
    BOOST_CHECK( !exception_caught );
    BOOST_CHECK( !passed );

    utf::unit_test_log::instance().set_log_stream( null_output );
    {
    utf::unit_test_result_saver saver;
    int test_data[] = { 0, 3, 9 };    
    test.reset( BOOST_PARAM_TEST_CASE__( &test1, (int*)test_data, (int*)test_data + sizeof(test_data)/sizeof(int) ) );

    test->run();

    utf::unit_test_result::instance().failures_details( num_of_failures, exception_caught );
    }

    utf::unit_test_log::instance().set_log_stream( std::cout );
    BOOST_CHECK_EQUAL( num_of_failures, (std::size_t)2 );
    BOOST_CHECK( !exception_caught );

    utf::unit_test_log::instance().set_log_stream( null_output );
    {
    utf::unit_test_result_saver saver;
    int test_data[] = { 2, 3, 9 };
    test.reset( BOOST_PARAM_TEST_CASE__( &test1, (int*)test_data, (int*)test_data + sizeof(test_data)/sizeof(int) ) );

    test->run();

    utf::unit_test_result::instance().failures_details( num_of_failures, exception_caught );
    }

    utf::unit_test_log::instance().set_log_stream( std::cout );
    BOOST_CHECK_EQUAL( num_of_failures, (std::size_t)2 );
    BOOST_CHECK( !exception_caught );

    utf::unit_test_log::instance().set_log_stream( null_output );
    {
    utf::unit_test_result_saver saver;
    int test_data[] = { 3, 2, 6 };    
    test.reset( BOOST_PARAM_TEST_CASE__( &test1, (int*)test_data, (int*)test_data + sizeof(test_data)/sizeof(int) ) );

    test->run();

    utf::unit_test_result::instance().failures_details( num_of_failures, exception_caught );
    }

    utf::unit_test_log::instance().set_log_stream( std::cout );
    BOOST_CHECK_EQUAL( num_of_failures, (std::size_t)1 );
    BOOST_CHECK( !exception_caught );

    return 0;
}

//____________________________________________________________________________//

// ***************************************************************************
//  Revision History :
//  
//  $Log: parameterized_test_test.cpp,v $
//  Revision 1.12  2003/12/03 01:22:40  rogeeff
//  *** empty log message ***
//
//  Revision 1.11  2003/12/01 00:42:37  rogeeff
//  prerelease cleaning
//

// ***************************************************************************

// EOF
