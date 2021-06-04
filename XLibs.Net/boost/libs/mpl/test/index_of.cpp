//-----------------------------------------------------------------------------
// libs mpl/test/index_of.cpp source file
// See http://www.boost.org for updates, documentation, and revision history.
//-----------------------------------------------------------------------------
//
// Copyright (c) 2003
// Eric Friedman
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee, 
// provided that the above copyright notice appears in all copies and 
// that both the copyright notice and this permission notice appear in 
// supporting documentation. No representations are made about the 
// suitability of this software for any purpose. It is provided "as is" 
// without express or implied warranty.

#include "boost/mpl/index_of.hpp"

#include "boost/static_assert.hpp"
#include "boost/mpl/list.hpp"
#include "boost/mpl/void.hpp"

namespace mpl = boost::mpl;

int test_main(int , char* [])
{
    typedef mpl::list< int, double, float >::type types;

    typedef mpl::index_of< types, int    >::type index_of_int;
    typedef mpl::index_of< types, double >::type index_of_double;
    typedef mpl::index_of< types, float  >::type index_of_float;
    typedef mpl::index_of< types, char   >::type index_of_char;

    BOOST_STATIC_ASSERT(( index_of_int::value    == 0 ));
    BOOST_STATIC_ASSERT(( index_of_double::value == 1 ));
    BOOST_STATIC_ASSERT(( index_of_float::value  == 2 ));

    BOOST_STATIC_ASSERT(( mpl::is_void_< index_of_char >::value ));

    return 0;
}
