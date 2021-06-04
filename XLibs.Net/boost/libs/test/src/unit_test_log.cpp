//  (C) Copyright Gennadiy Rozental 2001-2003.
//  Use, modification, and distribution are subject to the 
//  Boost Software License, Version 1.0. (See accompanying file 
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org/libs/test for the library home page.
//
//  File        : $RCSfile: unit_test_log.cpp,v $
//
//  Version     : $Revision: 1.19 $
//
//  Description : implemets Unit Test Log, Majority of implementation details
//  are hidden in this file with use of pimpl idiom.
// ***************************************************************************

// Boost.Test
#include <boost/test/unit_test_log.hpp>
#include <boost/test/unit_test_log_formatter.hpp>
#include <boost/test/detail/supplied_log_formatters.hpp>
#include <boost/test/detail/unit_test_parameters.hpp>
#include <boost/test/unit_test_result.hpp>

// BOOST
#include <boost/scoped_ptr.hpp>
#include <boost/progress.hpp>

// STL
#include <algorithm>

# ifdef BOOST_NO_STDC_NAMESPACE
namespace std { using ::strcmp; }
# endif

namespace boost {

namespace unit_test_framework {

// ************************************************************************** //
// **************                 unit_test_log                ************** //
// ************************************************************************** //

struct unit_test_log::Impl {
    // Constructor
    Impl() : m_stream( &std::cout ) {}

    // log data
    std::ostream*       m_stream;
    log_level           m_threshold_level;
    boost::scoped_ptr<boost::progress_display> m_progress_display;
    boost::scoped_ptr<unit_test_log_formatter> m_log_formatter;

    // entry data
    bool                m_entry_in_progress;
    bool                m_entry_has_value;
    log_entry_data      m_entry_data;

    // checkpoint data
    log_checkpoint_data m_checkpoint_data;

    // helper functions
    std::ostream&       stream()            { return *m_stream; }
    void                flush_entry()       { stream() << std::endl; }
    void                clear_entry_data()
    {
        m_entry_data.clear();
        m_entry_in_progress = false;
        m_entry_has_value   = false;
    }
    void                set_checkpoint( checkpoint const& cp )
    {
        m_checkpoint_data.m_message = cp.m_message;
        m_checkpoint_data.m_file    = m_entry_data.m_file;
        m_checkpoint_data.m_line    = m_entry_data.m_line;
    }
    void                clear_checkpoint()  { m_checkpoint_data.clear(); }
};

//____________________________________________________________________________//

unit_test_log::unit_test_log() : m_pimpl( new Impl() )
{
    m_pimpl->m_threshold_level = log_all_errors;

    m_pimpl->m_log_formatter.reset( new detail::msvc65_like_log_formatter( *this ) );

    m_pimpl->clear_entry_data();
    m_pimpl->clear_checkpoint();
}

//____________________________________________________________________________//

unit_test_log&
unit_test_log::instance()
{
    static unit_test_log the_instance;

    return the_instance;
}

//____________________________________________________________________________//

unit_test_log::~unit_test_log()
{
    delete m_pimpl;
}

//____________________________________________________________________________//

void
unit_test_log::set_log_stream( std::ostream& str )
{
    if( m_pimpl->m_entry_in_progress )
        return;

    m_pimpl->m_stream = &str;
}

//____________________________________________________________________________//

void
unit_test_log::set_log_threshold_level( log_level lev )
{
    if( m_pimpl->m_entry_in_progress )
        return;

    m_pimpl->m_threshold_level = lev;
}

//____________________________________________________________________________//

void
unit_test_log::set_log_threshold_level_by_name( std::string const& lev )
{
    if( m_pimpl->m_entry_in_progress )
        return;

    struct my_pair {
        c_string_literal    level_name;
        log_level           level_value;
    };

    static const my_pair name_value_map[] = {
        { "all"             , log_successful_tests },
        { "success"         , log_successful_tests },
        { "test_suite"      , log_test_suites },
        { "messages"        , log_messages },
        { "warnings"        , log_warnings },
        { "all_errors"      , log_all_errors },
        { "cpp_exceptions"  , log_cpp_exception_errors },
        { "system_errors"   , log_system_errors },
        { "fatal_errors"    , log_fatal_errors },
        { "progress"        , log_progress_only},
        { "nothing"         , log_nothing },
    };

    static int const map_size = sizeof(name_value_map)/sizeof(my_pair);

    if( lev.empty() )
        return;

    for( int i=0; i < map_size; i++ ) {
        if( lev == name_value_map[i].level_name ) {
            set_log_threshold_level( name_value_map[i].level_value );
            return;
        }
    }
}

//____________________________________________________________________________//

void
unit_test_log::clear_checkpoint()
{
    m_pimpl->clear_checkpoint();
}

//____________________________________________________________________________//

void
unit_test_log::track_test_case_scope( test_case const& tc, bool in_out )
{
    if( m_pimpl->m_threshold_level > log_test_suites )
        return;

    *this << begin();

    m_pimpl->m_log_formatter->track_test_case_scope( m_pimpl->stream(), tc, in_out );
    m_pimpl->m_entry_has_value = true;

    *this << end();
}

//____________________________________________________________________________//

unit_test_log&
unit_test_log::operator<<( begin const& )
{
    if( m_pimpl->m_entry_in_progress )
        *this << end();

    m_pimpl->m_entry_in_progress = true;

    return *this;
}

//____________________________________________________________________________//

unit_test_log&
unit_test_log::operator<<( end const& )
{
    if( m_pimpl->m_entry_has_value ) {
        m_pimpl->m_log_formatter->end_log_entry( m_pimpl->stream() );
        m_pimpl->flush_entry();
    }

    m_pimpl->clear_entry_data();

    return *this;
}

//____________________________________________________________________________//

char
set_unix_slash( char in )
{
    return in == '\\' ? '/' : in;
}

unit_test_log&
unit_test_log::operator<<( file const& f )
{
    if( m_pimpl->m_entry_in_progress ) {
        m_pimpl->m_entry_data.m_file = f.m_file_name;

        // normalize file name
        std::transform( m_pimpl->m_entry_data.m_file.begin(), m_pimpl->m_entry_data.m_file.end(), 
                        m_pimpl->m_entry_data.m_file.begin(),
                        &set_unix_slash );
    }

    return *this;
}

//____________________________________________________________________________//

unit_test_log&
unit_test_log::operator<<( line const& l )
{
    if( m_pimpl->m_entry_in_progress )
        m_pimpl->m_entry_data.m_line = l.m_line_num;

    return *this;
}

//____________________________________________________________________________//

unit_test_log&
unit_test_log::operator<<( level const& lev )
{
    if( m_pimpl->m_entry_in_progress )
        m_pimpl->m_entry_data.m_level = lev.m_level;

    return *this;
}

//____________________________________________________________________________//

unit_test_log&
unit_test_log::operator<<( checkpoint const& cp )
{
    if( m_pimpl->m_entry_in_progress )
        m_pimpl->set_checkpoint( cp );

    return *this;
}

//____________________________________________________________________________//

unit_test_log&
unit_test_log::operator<<( log_exception const& re )
{
    if( m_pimpl->m_entry_in_progress && m_pimpl->m_entry_data.m_level >= m_pimpl->m_threshold_level ) {
        m_pimpl->m_log_formatter->log_exception( m_pimpl->stream(), unit_test_result::instance().test_case_name(), re.m_what );
        m_pimpl->m_entry_has_value = true;
    }

    return *this;
}

//____________________________________________________________________________//

unit_test_log&
unit_test_log::operator<<( log_progress const& )
{
    if( m_pimpl->m_progress_display )
        ++(*m_pimpl->m_progress_display);

    return *this;
}

//____________________________________________________________________________//

unit_test_log&
unit_test_log::operator<<( std::string const& value )
{
    if( m_pimpl->m_entry_in_progress && m_pimpl->m_entry_data.m_level >= m_pimpl->m_threshold_level && !value.empty() ) {
        if( !m_pimpl->m_entry_has_value ) {
            switch( m_pimpl->m_entry_data.m_level ) {
            case log_successful_tests:
                m_pimpl->m_log_formatter->begin_log_entry( m_pimpl->stream(), unit_test_log_formatter::BOOST_UTL_ET_INFO );
                break;
            case log_messages:
                m_pimpl->m_log_formatter->begin_log_entry( m_pimpl->stream(), unit_test_log_formatter::BOOST_UTL_ET_MESSAGE );
                break;
            case log_warnings:
                m_pimpl->m_log_formatter->begin_log_entry( m_pimpl->stream(), unit_test_log_formatter::BOOST_UTL_ET_WARNING );
                break;
            case log_all_errors:
            case log_cpp_exception_errors:
            case log_system_errors:
                m_pimpl->m_log_formatter->begin_log_entry( m_pimpl->stream(), unit_test_log_formatter::BOOST_UTL_ET_ERROR );
                break;
            case log_fatal_errors:
                m_pimpl->m_log_formatter->begin_log_entry( m_pimpl->stream(), unit_test_log_formatter::BOOST_UTL_ET_FATAL_ERROR );
                break;
            case log_progress_only:
            case log_nothing:
            case log_test_suites:
                return *this;
            }
        }

        m_pimpl->m_log_formatter->log_entry_value( m_pimpl->stream(), value );
        m_pimpl->m_entry_has_value = true;
    }

    return *this;
}

//____________________________________________________________________________//

unit_test_log&
unit_test_log::operator<<( c_string_literal value )
{
    return *this << std::string( value ? value : "" );
}

//____________________________________________________________________________//

void
unit_test_log::start( bool print_build_info )
{
    m_pimpl->m_log_formatter->start_log( m_pimpl->stream(), print_build_info );
}

//____________________________________________________________________________//

void
unit_test_log::header( unit_test_counter test_cases_amount )
{
    if( m_pimpl->m_threshold_level != log_nothing && test_cases_amount > 0 )
        m_pimpl->m_log_formatter->log_header( m_pimpl->stream(), test_cases_amount );

    if( m_pimpl->m_threshold_level == log_progress_only )
        m_pimpl->m_progress_display.reset(
            new boost::progress_display( test_cases_amount, m_pimpl->stream() ) );
    else
        m_pimpl->m_progress_display.reset();
}

//____________________________________________________________________________//

void
unit_test_log::finish( unit_test_counter test_cases_amount )
{
    if( test_cases_amount == 1 )
        *this << log_progress();

    m_pimpl->m_log_formatter->finish_log( m_pimpl->stream() );
}

//____________________________________________________________________________//

void
unit_test_log::set_log_format( std::string const& logformat )
{
    if( m_pimpl->m_entry_in_progress )
        return;

    struct my_pair {
        c_string_literal    format_name;
        output_format       format_value;
    };

    static const my_pair name_value_map[] = {
        { "HRF" , HRF },
        { "XML" , XML },
    };

    static int const map_size = sizeof(name_value_map)/sizeof(my_pair);

    output_format of = HRF;
    for( int i=0; i < map_size; i++ ) {
        if( logformat == name_value_map[i].format_name ) {
            of = name_value_map[i].format_value;
            break;
        }
    }

    if( of == HRF )
        set_log_formatter( new detail::msvc65_like_log_formatter( *this ) );
    else
        set_log_formatter( new detail::xml_log_formatter( *this ) );
}

//____________________________________________________________________________//

void
unit_test_log::set_log_formatter( unit_test_log_formatter* the_formatter )
{
    m_pimpl->m_log_formatter.reset( the_formatter );
}

//____________________________________________________________________________//

log_entry_data const&
unit_test_log::entry_data() const
{
    return m_pimpl->m_entry_data;
}

//____________________________________________________________________________//

log_checkpoint_data const&
unit_test_log::checkpoint_data() const
{
    return m_pimpl->m_checkpoint_data;
}

//____________________________________________________________________________//

} // namespace unit_test_framework

} // namespace boost

// ***************************************************************************
//  Revision History :
//
//  $Log: unit_test_log.cpp,v $
//  Revision 1.19  2003/12/01 00:42:37  rogeeff
//  prerelease cleaning
//

// ***************************************************************************

// EOF
