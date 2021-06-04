/*=============================================================================
    Copyright (C) 1999-2003 Jaakko Järvi
    Copyright (c) 2003 Joel de Guzman

    Use, modification and distribution is subject to the Boost Software
    License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#include <boost/test/minimal.hpp>
#include <boost/spirit/fusion/sequence/tuple.hpp>
#include <boost/spirit/fusion/sequence/tuple_element.hpp>
#include <boost/spirit/fusion/sequence/get.hpp>
#include <iostream>

namespace
{
    // something to prevent warnings for unused variables
    template<class T> void dummy(const T&) {}

    class A {};
}

int
test_main(int, char *[])
{
    using namespace boost::fusion;

    //  JDG 10-21-2003: previously not availiable without
    //  partial template specialization. Now OK with VC6/7

    double d = 2.7;
    A a;
    tuple<int, double&, const A&, int> t(1, d, a, 2);
    const tuple<int, double&, const A, int> ct(t);

    int i  = get<0>(t);
    int i2 = get<3>(t);

    BOOST_TEST(i == 1 && i2 == 2);

    int j  = get<0>(ct);
    BOOST_TEST(j == 1);

    get<0>(t) = 5;
    BOOST_TEST(get<0>(t) == 5);

//  get<0>(ct) = 5; // can't assign to const

    double e = get<1>(t);
    BOOST_TEST(e > 2.69 && e < 2.71);

    get<1>(t) = 3.14+i;
    BOOST_TEST(get<1>(t) > 4.13 && get<1>(t) < 4.15);

//  get<4>(t) = A(); // can't assign to const
//  dummy(get<5>(ct)); // illegal index

    ++get<0>(t);
    BOOST_TEST(get<0>(t) == 6);

    BOOST_STATIC_ASSERT(!(
        boost::is_const<tuple_element<0, tuple<int, float> >::type>::value));

    // constness should not affect
    BOOST_STATIC_ASSERT(!(
        boost::is_const<tuple_element<0, const tuple<int, float> >::type>::value));

    BOOST_STATIC_ASSERT(!(
        boost::is_const<tuple_element<1, tuple<int, float> >::type>::value));

    // constness should not affect
    BOOST_STATIC_ASSERT(!(
        boost::is_const<tuple_element<1, const tuple<int, float> >::type>::value));

    dummy(i); dummy(i2); dummy(j); dummy(e); // avoid warns for unused variables

    return 0;
}
