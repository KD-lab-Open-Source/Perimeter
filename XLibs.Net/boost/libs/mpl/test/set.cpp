
// + file: libs/mpl/test/set.cpp
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

#include "boost/mpl/set/set0.hpp"
#include "boost/mpl/has_key.hpp"
#include "boost/mpl/order.hpp"
#include "boost/mpl/insert.hpp"
#include "boost/mpl/begin_end.hpp"

#include "boost/mpl/aux_/test.hpp"

using namespace boost::mpl;
using namespace boost;

CTT_test_case( false_positives, (S)(T) )
    {
    CTT_assert(( !has_key<S,T>::type::value )); // notation test
    CTT_assert(( !has_key<S,T>::value ));

    CTT_assert_same(3, (BOOST_MSVC_TYPENAME order<S,T>::type, void_));
    }

CTT_test_case( empty_set, (T) )
    {
    typedef set0<> s;
    
    CTT_for_each( 
          (T) CTT_basic_modifiers(T)
        , false_positives
        , (s)(_1)
        );
    }

CTT_test_case( one_element_set, (T) )
    {
    typedef s_item<T, set0<> > s;

    CTT_for_each( 
          CTT_basic_modifiers(T)
        , false_positives
        , (s)(_1)
        );
    
    CTT_assert(( has_key<s,T>::type::value )); // notation test
    CTT_assert(( has_key<s,T>::value ));
    
    CTT_assert_equal(3, (order<s,T>::type::value, 1)); // notation test
    CTT_assert_equal(3, (order<s,T>::value, 1));
    }

CTT_test_case( insertion, (T) )
    {
    typedef s_item<T, set0<> > s1;
    typedef typename insert< set0<>,T >::type s2;

    CTT_assert_same(2, (s1, s2));
    }

/*
CTT_test_case( erasure, (T) )
{
    typedef s_mask<char,s_item<int,s_item<char, set0<> > > > s;

    CTT_TEST_ASSERT(( !has_key<s,char>::type::value ));
    CTT_TEST_ASSERT(( has_key<s,int>::type::value ));
    CTT_TEST_ASSERT(( order<s,int>::type::value == 1 ));
    CTT_TEST_ASSERT(( is_same< order<s,char>::type,void_ >::value ));
    
    typedef begin<s>::type first;
    typedef end<s>::type last;

    CTT_TEST_ASSERT(( is_same<first::type,int>::value ));
    CTT_TEST_ASSERT(( is_same<first::next,last>::value ));

}
*/
int main()
{
    CTT_for_each( CTT_basic_types(), empty_set, (_1));
//    CTT_for_each( CTT_basic_types(), one_element_set, (_1));
    CTT_for_each( CTT_basic_types(), insertion, (_1));

    return 0;
}
