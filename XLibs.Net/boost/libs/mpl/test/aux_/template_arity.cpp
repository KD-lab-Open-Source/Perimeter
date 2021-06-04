
// + file: libs/mpl/test/aux_/template_arity.cpp
// + last modified: 10/jul/03

// Copyright (c) 2000-03
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

#include "boost/mpl/aux_/template_arity.hpp"
#include "boost/static_assert.hpp"

#   if defined(BOOST_EXTENDED_TEMPLATE_PARAMETERS_MATCHING)

using namespace boost::mpl;

struct my {};
template< typename T1 > struct my1 {};
template< typename T1, typename T2 = void > struct my2 {};

int main()
{
    BOOST_STATIC_ASSERT((aux::template_arity<my>::value == -1));
    BOOST_STATIC_ASSERT((aux::template_arity< my1<int> >::value == 1));
    BOOST_STATIC_ASSERT((aux::template_arity< my2<int,long> >::value == 2));
    BOOST_STATIC_ASSERT((aux::template_arity< my2<int> >::value == 2));
    
    return 0;
}

#   endif
