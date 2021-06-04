//  (C) Copyright Gennadiy Rozental 2001-2003.
//  (C) Copyright Ullrich Koethe 2001.
//  Use, modification, and distribution are subject to the 
//  Boost Software License, Version 1.0. (See accompanying file 
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org/libs/test for the library home page.
//
//  File        : $RCSfile: test_tools.cpp,v $
//
//  Version     : $Revision: 1.27 $
//
//  Description : supplies offline implemtation for the Test Tools
// ***************************************************************************

// Boost.Test
#include <boost/test/test_tools.hpp>
#include <boost/test/unit_test_result.hpp>

// BOOST
#include <boost/config.hpp>
#include <boost/minmax.hpp>

// STL
#include <fstream>
#include <iostream>
#include <algorithm>
#include <cstring>  
#include <string>
#include <cctype>

# ifdef BOOST_NO_STDC_NAMESPACE
namespace std { using ::strcmp; using ::strncmp; using ::strlen; using ::isprint; }
# endif

namespace boost {

namespace test_toolbox {

namespace detail {

// ************************************************************************** //
// **************            TOOL BOX Implementation           ************** //
// ************************************************************************** //

void
checkpoint_impl( wrap_stringstream& message, c_string_literal file_name, std::size_t line_num )
{
    BOOST_UT_LOG_BEGIN( file_name, line_num, unit_test_framework::log_test_suites )
        unit_test_framework::checkpoint( message.str() )
    BOOST_UT_LOG_END
}

//____________________________________________________________________________//

void
message_impl( wrap_stringstream& message, c_string_literal file_name, std::size_t line_num )
{
    BOOST_UT_LOG_BEGIN( file_name, line_num, unit_test_framework::log_messages )
        message.str()
    BOOST_UT_LOG_END
}

//____________________________________________________________________________//

void
warn_and_continue_impl( bool predicate, wrap_stringstream& message,
                        c_string_literal file_name, std::size_t line_num, bool add_fail_pass )
{
    if( !predicate ) {
        BOOST_UT_LOG_BEGIN( file_name, line_num, unit_test_framework::log_warnings )
            (add_fail_pass ? "condition " : "") << message.str() << (add_fail_pass ? " is not satisfied" : "" )
        BOOST_UT_LOG_END
    }
    else {
        BOOST_UT_LOG_BEGIN( file_name, line_num, unit_test_framework::log_successful_tests )
            "condition " << message.str() << " is satisfied"
        BOOST_UT_LOG_END
    }
}

//____________________________________________________________________________//

void
warn_and_continue_impl( extended_predicate_value const& v, wrap_stringstream& message, c_string_literal file_name, std::size_t line_num,
                        bool add_fail_pass )
{
    warn_and_continue_impl( !!v,
        message << (add_fail_pass && !v ? " is not satisfied. " : "" ) << *(v.p_message),
        file_name, line_num, false );
}

//____________________________________________________________________________//

bool
test_and_continue_impl( bool predicate, wrap_stringstream& message,
                        c_string_literal file_name, std::size_t line_num,
                        bool add_fail_pass, unit_test_framework::log_level loglevel )
{
    if( !predicate ) {
        unit_test_framework::unit_test_result::instance().inc_failed_assertions();

        BOOST_UT_LOG_BEGIN( file_name, line_num, loglevel )
            (add_fail_pass ? "test " : "") << message.str() << (add_fail_pass ? " failed" : "")
        BOOST_UT_LOG_END

        return true;
    }
    else {
        unit_test_framework::unit_test_result::instance().inc_passed_assertions();

        BOOST_UT_LOG_BEGIN( file_name, line_num, unit_test_framework::log_successful_tests )
            (add_fail_pass ? "test " : "") << message.str() << (add_fail_pass ? " passed" : "")
        BOOST_UT_LOG_END

        return false;
    }
}

//____________________________________________________________________________//

bool
test_and_continue_impl( extended_predicate_value const& v, wrap_stringstream& message,
                        c_string_literal file_name, std::size_t line_num,
                        bool add_fail_pass, unit_test_framework::log_level loglevel )
{
    return test_and_continue_impl( !!v,
        message << (add_fail_pass ? (!v ? " failed. " : " passed. ") : "") << *(v.p_message),
        file_name, line_num, false, loglevel );
}

//____________________________________________________________________________//

void
test_and_throw_impl( bool predicate, wrap_stringstream& message,
                     c_string_literal file_name, std::size_t line_num,
                     bool add_fail_pass, unit_test_framework::log_level loglevel )
{
    if( test_and_continue_impl( predicate, message, file_name, line_num, add_fail_pass, loglevel ) ) {
        throw test_tool_failed(); // error already reported by test_and_continue_impl
    }
}

//____________________________________________________________________________//

void
test_and_throw_impl( extended_predicate_value const& v, wrap_stringstream& message,
                     c_string_literal file_name, std::size_t line_num,
                     bool add_fail_pass, unit_test_framework::log_level loglevel )
{
    if( test_and_continue_impl( v, message, file_name, line_num, add_fail_pass, loglevel ) ) {
        throw test_tool_failed(); // error already reported by test_and_continue_impl
    }
}

//____________________________________________________________________________//

bool
equal_and_continue_impl( c_string_literal left, c_string_literal right, wrap_stringstream& message,
                         c_string_literal file_name, std::size_t line_num,
                         unit_test_framework::log_level loglevel )
{
    bool predicate = (left && right) ? std::strcmp( left, right ) == 0 : (left == right);

    left  = left  ? left  : "null string";
    right = right ? right : "null string";

    if( !predicate ) {
        return test_and_continue_impl( false,
            wrap_stringstream().ref() << "test " << message.str() << " failed [" << left << " != " << right << "]",
            file_name, line_num, false, loglevel );
    }

    return test_and_continue_impl( true, message, file_name, line_num, true, loglevel );
}

//____________________________________________________________________________//

bool
is_defined_impl( c_string_literal symbol_name, c_string_literal symbol_value )
{
//    return std::strncmp( symbol_name, symbol_value, std::strlen( symbol_name ) ) != 0;
    return std::strcmp( symbol_name, symbol_value + 2 ) != 0;
}

//____________________________________________________________________________//

// ************************************************************************** //
// **************               log print helper               ************** //
// ************************************************************************** //

void
print_log_value<char>::operator()( std::ostream& ostr, char t )
{
    if( (std::isprint)( t ) )
        ostr << '\'' << t << '\'';
    else
        ostr << std::hex 
        // showbase is only available for new style streams:
#ifndef BOOST_NO_STD_LOCALE
        << std::showbase
#else
        << "0x"
#endif
        << (int)t;
}

//____________________________________________________________________________//

void
print_log_value<unsigned char>::operator()( std::ostream& ostr, unsigned char t )
{
    ostr << std::hex 
        // showbase is only available for new style streams:
#ifndef BOOST_NO_STD_LOCALE
        << std::showbase
#else
        << "0x"
#endif
       << (int)t;
}

//____________________________________________________________________________//

} // namespace detail

// ************************************************************************** //
// **************               output_test_stream             ************** //
// ************************************************************************** //

struct output_test_stream::Impl
{
    std::fstream    m_pattern_to_match_or_save;
    bool            m_match_or_save;
    std::string     m_synced_string;

    char            get_char()
    {
        char res;
        do {
            m_pattern_to_match_or_save.get( res );
        } while( res == '\r'                        &&
                 !m_pattern_to_match_or_save.fail() &&
                 !m_pattern_to_match_or_save.eof() );

        return res;
    }

    void            check_and_fill( extended_predicate_value& res )
    {
        if( !res.p_predicate_value.get() )
            *(res.p_message) << "Output content: \"" << m_synced_string << '\"';
    }
};

//____________________________________________________________________________//

output_test_stream::output_test_stream( std::string const& pattern_file_name, bool match_or_save )
: m_pimpl( new Impl )
{
    if( !pattern_file_name.empty() )
        m_pimpl->m_pattern_to_match_or_save.open( pattern_file_name.c_str(), match_or_save ? std::ios::in : std::ios::out );

    m_pimpl->m_match_or_save = match_or_save;
}

//____________________________________________________________________________//

output_test_stream::output_test_stream( c_string_literal pattern_file_name, bool match_or_save )
: m_pimpl( new Impl )
{
    if( pattern_file_name && pattern_file_name[0] != '\0' )
        m_pimpl->m_pattern_to_match_or_save.open( pattern_file_name, match_or_save ? std::ios::in : std::ios::out );

    m_pimpl->m_match_or_save = match_or_save;
}

//____________________________________________________________________________//

output_test_stream::~output_test_stream()
{
}

//____________________________________________________________________________//

extended_predicate_value
output_test_stream::is_empty( bool flush_stream )
{
    sync();

    result_type res( m_pimpl->m_synced_string.empty() );

    m_pimpl->check_and_fill( res );

    if( flush_stream )
        flush();

    return res;
}

//____________________________________________________________________________//

extended_predicate_value
output_test_stream::check_length( std::size_t length_, bool flush_stream )
{
    sync();

    result_type res( m_pimpl->m_synced_string.length() == length_ );

    m_pimpl->check_and_fill( res );

    if( flush_stream )
        flush();

    return res;
}

//____________________________________________________________________________//

extended_predicate_value
output_test_stream::is_equal( c_string_literal arg, bool flush_stream )
{
    sync();

    result_type res( m_pimpl->m_synced_string == arg );

    m_pimpl->check_and_fill( res );

    if( flush_stream )
        flush();

    return res;
}

//____________________________________________________________________________//

extended_predicate_value
output_test_stream::is_equal( std::string const& arg, bool flush_stream )
{
    sync();

    result_type res( m_pimpl->m_synced_string == arg );

    m_pimpl->check_and_fill( res );

    if( flush_stream )
        flush();

    return res;
}

//____________________________________________________________________________//

extended_predicate_value
output_test_stream::is_equal( c_string_literal arg, std::size_t n, bool flush_stream )
{
    sync();

    result_type res( m_pimpl->m_synced_string == std::string( arg, n ) );

    m_pimpl->check_and_fill( res );

    if( flush_stream )
        flush();

    return res;
}

//____________________________________________________________________________//

extended_predicate_value
output_test_stream::match_pattern( bool flush_stream )
{
    sync();

    result_type result( true );

    if( !m_pimpl->m_pattern_to_match_or_save.is_open() ) {
        result = false;
        *(result.p_message) << "Couldn't open pattern file for " 
            << ( m_pimpl->m_match_or_save ? "reading" : "writing");
    }
    else {
        if( m_pimpl->m_match_or_save ) {
            for ( std::string::size_type i = 0; i < m_pimpl->m_synced_string.length(); ++i ) {
                char c = m_pimpl->get_char();

                result = !m_pimpl->m_pattern_to_match_or_save.fail() && 
                         !m_pimpl->m_pattern_to_match_or_save.eof() && 
                         (m_pimpl->m_synced_string[i] == c);

                if( !result ) {
                    std::string::size_type suffix_size  = std_min( m_pimpl->m_synced_string.length() - i,
                                                                    static_cast<std::string::size_type>(5) );

                    // try to log area around the mismatch 
                    *(result.p_message) << "Mismatch at position " << i << '\n'
                        << "..." << m_pimpl->m_synced_string.substr( i, suffix_size ) << "..." << '\n'
                        << "..." << c;

                    std::string::size_type counter = suffix_size;
                    while( --counter ) {
                        char c = m_pimpl->get_char();

                        if( m_pimpl->m_pattern_to_match_or_save.fail() || 
                            m_pimpl->m_pattern_to_match_or_save.eof() )
                            break;

                        *(result.p_message) << c;
                    }

                    *(result.p_message) << "...";

                    // skip rest of the bytes. May help for further matching
                    m_pimpl->m_pattern_to_match_or_save.ignore( m_pimpl->m_synced_string.length() - i - suffix_size);
                    break;
                }
            }
        }
        else {
            m_pimpl->m_pattern_to_match_or_save.write( m_pimpl->m_synced_string.c_str(), 
                                                       static_cast<std::streamsize>( m_pimpl->m_synced_string.length() ) );
            m_pimpl->m_pattern_to_match_or_save.flush();
        }
    }

    if( flush_stream )
        flush();

    return result;
}

//____________________________________________________________________________//

void
output_test_stream::flush()
{
    m_pimpl->m_synced_string.erase();

#ifndef BOOST_NO_STRINGSTREAM
    str( std::string() );
#else
    seekp( 0, std::ios::beg );
#endif
}

//____________________________________________________________________________//

std::size_t
output_test_stream::length()
{
    sync();

    return m_pimpl->m_synced_string.length();
}

//____________________________________________________________________________//

void
output_test_stream::sync()
{
#ifdef BOOST_NO_STRINGSTREAM
    m_pimpl->m_synced_string.assign( str(), pcount() );
    freeze( false );
#else
    m_pimpl->m_synced_string = str();
#endif
}

//____________________________________________________________________________//

} // namespace test_toolbox

} // namespace boost

// ***************************************************************************
//  Revision History :
//  
//  $Log: test_tools.cpp,v $
//  Revision 1.27  2004/03/10 09:39:45  tknapen
//  parenthesis around std::isprint because it's a macro on mipspro
//
//  Revision 1.26  2004/02/26 18:27:01  eric_niebler
//  remove minmax hack from win32.hpp and fix all places that could be affected by the minmax macros
//
//  Revision 1.25  2003/12/01 00:42:37  rogeeff
//  prerelease cleaning
//

// ***************************************************************************

// EOF
