
// + file: libs/mpl/test/aux_/has_xxx.cpp
// + last modified: 09/jun/03

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

#include "boost/mpl/aux_/has_xxx.hpp"
#include "boost/mpl/aux_/config/workaround.hpp"
#include "boost/static_assert.hpp"

BOOST_MPL_HAS_XXX_TRAIT_DEF(xxx)

struct a1;
struct a2 {};
struct a3 { void xxx(); };
struct a4 { int xxx; };
struct a5 { static int xxx(); };

struct b1 { typedef int xxx; };
struct b2 { struct xxx; };
struct b3 { typedef int& xxx; };
struct b4 { typedef int* xxx; };
struct b5 { typedef int xxx[10]; };
struct b6 { typedef void (*xxx)(); };
struct b7 { typedef void (xxx)(); };

template <class T> struct outer;
template <class T> struct inner { typedef typename T::type type; };

int main()
{
#if !BOOST_WORKAROUND(BOOST_MSVC, <= 1300)
    BOOST_STATIC_ASSERT(!has_xxx<int&>::value);
    BOOST_STATIC_ASSERT(!has_xxx<int*>::value);
    BOOST_STATIC_ASSERT(!has_xxx<int[]>::value);
    BOOST_STATIC_ASSERT(!has_xxx<int (*)()>::value);
#endif
    BOOST_STATIC_ASSERT(!has_xxx<int>::value);
    BOOST_STATIC_ASSERT(!has_xxx<a1>::value);
    BOOST_STATIC_ASSERT(!has_xxx<a2>::value);
    BOOST_STATIC_ASSERT(!has_xxx<a3>::value);
    BOOST_STATIC_ASSERT(!has_xxx<a4>::value);
    BOOST_STATIC_ASSERT(!has_xxx<a5>::value);

    BOOST_STATIC_ASSERT(has_xxx<b1>::value);
    BOOST_STATIC_ASSERT(has_xxx<b2>::value);
    BOOST_STATIC_ASSERT(has_xxx<b3>::value);
    BOOST_STATIC_ASSERT(has_xxx<b4>::value);
    BOOST_STATIC_ASSERT(has_xxx<b5>::value);
    BOOST_STATIC_ASSERT(has_xxx<b6>::value);
    BOOST_STATIC_ASSERT(has_xxx<b7>::value);

    BOOST_STATIC_ASSERT(!has_xxx<outer<inner<int> > >::value);

    return 0;
}
