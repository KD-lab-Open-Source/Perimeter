
// + file: libs/mpl/aux_/test/test_case.cpp
// + last modified: 03/may/03

// Copyright (c) 2002-03
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

#include "boost/mpl/aux_/test.hpp"

CTT_test_case( false_positives, (S)(T) )
{
    CTT_assert_same( 2, (S, char) );
    CTT_assert_not_same( 2, (S, bool) );
    //T t;
}

CTT_test_case( empty_set, (T) )
    {
    CTT_for_each( 
          (T)(T const)
        , false_positives
        , (char)(_1)
        );
    }

int main()
{
    CTT_test( empty_set, (bool) );
    CTT_for_each( (int)(long), empty_set, (_1) );

    return 0;
}
