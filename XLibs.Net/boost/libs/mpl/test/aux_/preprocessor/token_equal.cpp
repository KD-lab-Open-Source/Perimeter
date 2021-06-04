
// + file: libs/mpl/test/aux_/token_eqaul.cpp
// + last modified: 03/may/03

// Copyright (c) 2003
// Aleksey Gurtovoy
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee, 
// provided that the above copyright notice appears in all copies and 
// that both the copyright notice and this permission notice appear in 
// supporting documentation. No representations are made about the 
// suitability of this software for any purpose. It is provided "as is" 
// without express or implied warranty.
//
// See http://www.boost.org/libs/mpl for documentation.

#include "boost/mpl/aux_/preprocessor/token_equal.hpp"

#include "boost/preprocessor/logical/not.hpp"
#include "boost/preprocessor/debug/assert.hpp"

#define AUX_ASSERT_EQUAL(x, y) \
    BOOST_PP_ASSERT( BOOST_MPL_PP_TOKEN_EQUAL(x, y) ) \
/**/

#define AUX_ASSERT_NOT_EQUAL(x, y) \
    BOOST_PP_ASSERT( BOOST_PP_NOT( BOOST_MPL_PP_TOKEN_EQUAL(x, y) ) ) \
/**/

int main()
{
    #define BOOST_MPL_PP_TOKEN_EQUAL_apple(x) x
    #define BOOST_MPL_PP_TOKEN_EQUAL_orange(x) x

    AUX_ASSERT_NOT_EQUAL( apple, abc )
    AUX_ASSERT_NOT_EQUAL( abc, apple )
    AUX_ASSERT_NOT_EQUAL( apple, orange )
    AUX_ASSERT_NOT_EQUAL( orange, apple )

    AUX_ASSERT_EQUAL( apple, apple )
    AUX_ASSERT_EQUAL( orange, orange )

    return 0;
}
