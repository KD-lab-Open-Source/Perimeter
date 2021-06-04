//  (C) Copyright Gennadiy Rozental 2001-2003.
//  Use, modification, and distribution are subject to the 
//  Boost Software License, Version 1.0. (See accompanying file 
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org/libs/test for the library home page.
//
//  File        : $RCSfile: unit_test_main.cpp,v $
//
//  Version     : $Revision: 1.13 $
//
//  Description : main function implementation for Unit Test Framework
// ***************************************************************************

#include <boost/test/unit_test.hpp>   // for unit_test framework
#include <boost/test/unit_test_result.hpp>
#include <boost/test/unit_test_log.hpp>
#include <boost/test/detail/unit_test_parameters.hpp>
#include <boost/test/detail/unit_test_monitor.hpp>

// BOOST
#include <boost/scoped_ptr.hpp>

// STL
#include <string>
#include <iostream>                   // for cout, cerr

extern boost::unit_test_framework::test_suite* init_unit_test_suite( int argc, char* argv[] );  // prototype for user's test suite init function

// ************************************************************************** //
// **************                 unit test main               ************** //
// ************************************************************************** //

int
main( int argc, char* argv[] )
{
    using namespace boost::unit_test_framework;
    using namespace boost::unit_test_framework::detail;

    bool    no_result_code;
    bool    print_build_info;

    // set the log level
    unit_test_log::instance().set_log_threshold_level_by_name( retrieve_framework_parameter( LOG_LEVEL, &argc, argv ) );

    // set the log/report format
    std::string output_format = retrieve_framework_parameter( OUTPUT_FORMAT, &argc, argv );
    
    if( output_format.empty() ) {
        unit_test_log::instance().set_log_format( retrieve_framework_parameter( LOG_FORMAT, &argc, argv ) );
        unit_test_result::set_report_format( retrieve_framework_parameter( REPORT_FORMAT, &argc, argv ) );
    }
    else {
        unit_test_log::instance().set_log_format( output_format );
        unit_test_result::set_report_format( output_format );
    }

    // set the result code and build info flags
    no_result_code    = retrieve_framework_parameter( NO_RESULT_CODE, &argc, argv ) == "no";
    print_build_info  = retrieve_framework_parameter( BUILD_INFO, &argc, argv ) == "yes";

    // set catch_system_error switch
    unit_test_monitor::catch_system_errors( retrieve_framework_parameter( CATCH_SYS_ERRORS, &argc, argv ) != "no" );

    // save report level for future reporting
    std::string report_level = retrieve_framework_parameter( REPORT_LEVEL, &argc, argv );

    // init master unit test suite
    boost::scoped_ptr<test_suite> test( init_unit_test_suite( argc, argv ) );
    if( !test ) {
        std::cerr << "*** Fail to initialize test suite" << std::endl;
        return boost::exit_test_failure;
    }

    // start testing
    unit_test_log::instance().start( print_build_info );
    unit_test_log::instance().header( test->size() );
    test->run();
    unit_test_log::instance().finish( test->size() );

    // report results
    unit_test_result::instance().report( report_level, std::cerr );

    // return code
    return no_result_code ? boost::exit_success : unit_test_result::instance().result_code();
}

// ***************************************************************************
//  Revision History :
//  
//  $Log: unit_test_main.cpp,v $
//  Revision 1.13  2003/12/01 00:42:37  rogeeff
//  prerelease cleaning
//

// ***************************************************************************

// EOF
