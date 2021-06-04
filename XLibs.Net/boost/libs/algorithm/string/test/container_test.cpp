//  Boost string_algo library substr_test.cpp file  ------------------//

//  Copyright Pavol Droba 2002-2003. Use, modification and
//  distribution is subject to the Boost Software License, Version
//  1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for updates, documentation, and revision history.

// Include unit test framework
#include <boost/test/included/test_exec_monitor.hpp>

#include <string>
#include <utility>
#include <vector>

#include <boost/algorithm/string/collection_traits.hpp>
#include <boost/type_traits.hpp>

// equals predicate is used for result comparison
#include <boost/algorithm/string/predicate.hpp>

// test tools
#include <boost/test/test_tools.hpp>


using namespace std;
using namespace boost;

namespace algo = ::boost::algorithm;

template< typename T >
void argument_cv_test( const T& C, const string& strResult )
{
    BOOST_CHECK( equals( make_iterator_range(algo::begin(C),algo::end(C)), strResult ) );
    BOOST_CHECK( algo::size(C)==strResult.size() );
    BOOST_CHECK( algo::empty(C)==strResult.empty() );
}

template< typename T >
void argument_test( T& C, const string& strResult )
{
    BOOST_CHECK( equals( make_iterator_range(algo::begin(C),algo::end(C)), strResult ) );
    BOOST_CHECK( algo::size(C)==strResult.size() );
    BOOST_CHECK( algo::empty(C)==strResult.empty() );
}

void container_test()
{
    BOOST_CHECKPOINT( "type test" );
        
    // types test
    BOOST_REQUIRE( (is_same< 
        algo::result_iterator_of<string>::type, 
        string::iterator>::value) );
    BOOST_REQUIRE( (is_same< 
        algo::result_iterator_of<const string>::type, 
        string::const_iterator>::value) );
    BOOST_REQUIRE( (is_same< 
        algo::result_iterator_of<char[4]>::type, char*>::value) );
    BOOST_REQUIRE( (is_same< 
        algo::result_iterator_of<const char[4]>::type, 
        const char*>::value) );
    BOOST_REQUIRE( (is_same< 
        algo::result_iterator_of<char*>::type, char*>::value) );
    BOOST_REQUIRE( (is_same< 
        algo::result_iterator_of<const char*>::type, 
        const char*>::value) );
    BOOST_REQUIRE( (is_same< 
        algo::result_iterator_of<
            pair<string::iterator, string::iterator> >::type, string::iterator>::value) );
    BOOST_REQUIRE( (is_same< 
         algo::result_iterator_of<
            const pair<string::iterator, string::iterator> >::type, 
            string::iterator>::value) );

    BOOST_CHECKPOINT( "non-empty sequence test" );
    
    string str1("abc");
    vector<char> vec1( str1.begin(), str1.end() );
    pair<string::iterator, string::iterator> pair1=
        make_pair( str1.begin(), str1.end() );
    char ach1[]="abc";
    char *pch1="abc";

    // begin/end tests
    argument_cv_test( str1, "abc" );
    argument_test( str1, "abc" );
    argument_cv_test( vec1, "abc" );
    argument_test( vec1, "abc" );
    argument_cv_test( pair1, "abc" );
    argument_test( pair1, "abc" );
    argument_cv_test( ach1, "abc" );
    argument_test( ach1, "abc" );
    argument_cv_test( pch1, "abc" );
    argument_test( pch1, "abc" );

    BOOST_CHECKPOINT( "empty sequence test" );

    string str2;
    vector<char> vec2( str2.begin(), str2.end() );
    pair<string::iterator, string::iterator> pair2=
        make_pair( str2.begin(), str2.end() );
    char ach2[]="";
    char *pch2=0;

    // begin/end tests
    argument_cv_test( str2, "" );
    argument_test( str2, "" );
    argument_cv_test( vec2, "" );
    argument_test( vec2, "" );
    argument_cv_test( pair2, "" );
    argument_test( pair2, "" );
    argument_cv_test( ach2, "" );
    argument_test( ach2, "" );
    argument_cv_test( pch2, "" );
    argument_test( pch2, "" );
};


// test main 
int test_main( int, char*[] )
{
    container_test();

    return 0;
}
