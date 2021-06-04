
// + file: libs/mpl/test/aux_/is_seq.cpp
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

#include "boost/mpl/aux_/preprocessor/is_seq.hpp"

#include "boost/preprocessor/logical/not.hpp"
#include "boost/preprocessor/debug/assert.hpp"

#define AUX_ASSERT_IS_SEQ( seq ) \
    BOOST_PP_ASSERT( BOOST_MPL_PP_IS_SEQ(seq) ) \
/**/

#define AUX_ASSERT_IS_NOT_SEQ( seq ) \
    BOOST_PP_ASSERT( BOOST_PP_NOT( BOOST_MPL_PP_IS_SEQ(seq) ) ) \
/**/

#define SEQ (a)(b)(c)

int main()
{
    AUX_ASSERT_IS_NOT_SEQ( a )
    AUX_ASSERT_IS_SEQ( (a) )
    AUX_ASSERT_IS_SEQ( (a)(b) )
    AUX_ASSERT_IS_SEQ( (a)(b)(c) )
    AUX_ASSERT_IS_SEQ( SEQ )

    return 0;
}
