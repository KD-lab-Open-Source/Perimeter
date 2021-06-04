
// + file: libs/mpl/test/multiset.cpp
// + last modified: 05/nov/03

// Copyright Aleksey Gurtovoy 2003
//
// Use, modification and distribution are subject to the Boost Software 
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy 
// at http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/mpl for documentation.

#include "boost/mpl/multiset/multiset0.hpp"
#include "boost/mpl/insert.hpp"
#include "boost/mpl/count.hpp"
#include "boost/static_assert.hpp"

using namespace boost;
using namespace boost::mpl;

struct abstract
{
    virtual ~abstract() = 0;
};

int main()
{
    typedef multiset0<> s0;
    typedef insert<s0,int>::type s1;
    typedef insert<s1,char&>::type s2;
    typedef insert<s2,int>::type s3;
    typedef insert<s3,abstract>::type s4;
    
    BOOST_STATIC_ASSERT((count<s0,int>::value == 0));
    BOOST_STATIC_ASSERT((count<s1,int>::value == 1));
    BOOST_STATIC_ASSERT((count<s2,int>::value == 1));
    BOOST_STATIC_ASSERT((count<s2,char&>::value == 1));
    BOOST_STATIC_ASSERT((count<s3,int>::value == 2));
    BOOST_STATIC_ASSERT((count<s3,char&>::value == 1));
    BOOST_STATIC_ASSERT((count<s4,abstract>::value == 1));

    return 0;
}
