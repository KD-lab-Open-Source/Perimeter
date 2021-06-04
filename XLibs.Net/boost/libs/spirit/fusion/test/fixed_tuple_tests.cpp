/*=============================================================================
    Copyright (c) 2001-2003 Joel de Guzman

    Use, modification and distribution is subject to the Boost Software
    License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#include <boost/test/minimal.hpp>
#include <boost/spirit/fusion/sequence/tuple10.hpp>
#include <boost/spirit/fusion/sequence/tuple_size.hpp>
#include <boost/spirit/fusion/sequence/tuple_element.hpp>
#include <boost/spirit/fusion/sequence/get.hpp>

#ifndef __COMO_VERSION__
// comeau is too slow for these big tuples
# include <boost/spirit/fusion/sequence/tuple20.hpp>
# include <boost/spirit/fusion/sequence/tuple30.hpp>
#if !defined(__BORLANDC__) || (__BORLANDC__ > 0x551)
// borland chokes on very big tuples
# include <boost/spirit/fusion/sequence/tuple40.hpp>
# include <boost/spirit/fusion/sequence/tuple50.hpp>
#endif
#endif

#include <boost/type_traits/is_same.hpp>
#include <boost/static_assert.hpp>
#include <iostream>
#include <utility> // for std::pair

int
test_main(int, char*[])
{
    using namespace boost::fusion;
    using namespace boost;
    using namespace std;

    {
        tuple0 t;
        (void) t;
        cout << "(): " << sizeof(t) << endl;
    }

    {
        typedef tuple1<int> type;
        type t;
        BOOST_STATIC_ASSERT(tuple_size<type>::value == 1);

        BOOST_TEST(get<0>(t) == 0);
        BOOST_STATIC_ASSERT((is_same<int, tuple_element<0, type>::type>::value));
    }

    {
        typedef tuple1<int> type;
        type t(123);
        BOOST_TEST(get<0>(t) == 123);
        cout << "(int): " << sizeof(t) << endl;
    }

    {
        tuple1<int> t1(123L); // try conversion from long to int
        tuple1<double> t2(t1); // try copy
        (void)t2;
    }

    {
        typedef tuple2<int, char> type;
        type t;
        BOOST_STATIC_ASSERT(tuple_size<type>::value == 2);

        BOOST_TEST(get<0>(t) == 0);
        BOOST_TEST(get<1>(t) == char());

        BOOST_STATIC_ASSERT((is_same<int, tuple_element<0, type>::type>::value));
        BOOST_STATIC_ASSERT((is_same<char, tuple_element<1, type>::type>::value));
    }

    {
        typedef tuple2<int, char> type;
        type t(123, 'x');
        BOOST_TEST(get<0>(t) == 123);
        BOOST_TEST(get<1>(t) == 'x');
        cout << "(int, char): " << sizeof(t) << endl;
    }

    {
        tuple2<int, int> t1(123, 456);
        tuple2<double, float> t2(t1); // try copy
        tuple2<int, int> t3(std::make_pair(123, 456)); // try copy from pair
        (void)t2;
        (void)t3;
    }

    {
        typedef tuple3<int, char, double> type;
        type t;
        BOOST_STATIC_ASSERT(tuple_size<type>::value == 3);

        BOOST_TEST(get<0>(t) == 0);
        BOOST_TEST(get<1>(t) == char());
        BOOST_TEST(get<2>(t) == double());

        BOOST_STATIC_ASSERT((is_same<int, tuple_element<0, type>::type>::value));
        BOOST_STATIC_ASSERT((is_same<char, tuple_element<1, type>::type>::value));
        BOOST_STATIC_ASSERT((is_same<double, tuple_element<2, type>::type>::value));
    }

    {
        typedef tuple3<int, char, double> type;
        type t(123, 'x', 123.456);
        BOOST_TEST(get<0>(t) == 123);
        BOOST_TEST(get<1>(t) == 'x');
        BOOST_TEST(get<2>(t) >= 123.455 && get<2>(t) <= 123.457);
        cout << "(int, char, double): " << sizeof(t) << endl;
    }

    {
        typedef tuple4<int, char, double, bool> type;
        type t(123, 'x', 123.456, true);
        cout << "(int, char, double, bool): " << sizeof(t) << endl;
    }

    {
        typedef tuple4<int, char, bool, double> type;
        type t(123, 'x', true, 123.456);
        cout << "(int, char, bool, double): " << sizeof(t) << endl;
    }

    {
        typedef tuple7<bool, char, short, int, long, float, double> type;
        type t(false, 'x', 3, 4, 5, 6.0, 7.0);

        BOOST_TEST(get<0>(t) == false);
        BOOST_TEST(get<1>(t) == 'x');
        BOOST_TEST(get<2>(t) == 3);
        BOOST_TEST(get<3>(t) == 4);
        BOOST_TEST(get<4>(t) == 5);
        BOOST_TEST(get<5>(t) >= 5.9 && get<5>(t) <= 6.1);
        BOOST_TEST(get<6>(t) >= 6.9 && get<6>(t) <= 7.1);

        BOOST_STATIC_ASSERT((is_same<bool, tuple_element<0, type>::type>::value));
        BOOST_STATIC_ASSERT((is_same<char, tuple_element<1, type>::type>::value));
        BOOST_STATIC_ASSERT((is_same<short, tuple_element<2, type>::type>::value));
        BOOST_STATIC_ASSERT((is_same<int, tuple_element<3, type>::type>::value));
        BOOST_STATIC_ASSERT((is_same<long, tuple_element<4, type>::type>::value));
        BOOST_STATIC_ASSERT((is_same<float, tuple_element<5, type>::type>::value));
        BOOST_STATIC_ASSERT((is_same<double, tuple_element<6, type>::type>::value));
        cout << "(bool, char, short, int, long, float, double): " << sizeof(t) << endl;
    }

    {
        typedef tuple10<int, int, int, int, int, int, int, int, int, int> type;
        type t; // compile check only
        cout << "tuple10 of int: " << sizeof(t) << endl;
    }

#ifndef __COMO_VERSION__
// comeau is too slow for these big tuples
    {
        typedef tuple20<
            int, int, int, int, int, int, int, int, int, int
          , int, int, int, int, int, int, int, int, int, int> type;

        type t; // compile check only
        cout << "tuple20 of int: " << sizeof(t) << endl;
    }

    {
        typedef tuple30<
            int, int, int, int, int, int, int, int, int, int
          , int, int, int, int, int, int, int, int, int, int
          , int, int, int, int, int, int, int, int, int, int> type;

        type t; // compile check only
        cout << "tuple30 of int: " << sizeof(t) << endl;
    }

#if !defined(__BORLANDC__) || (__BORLANDC__ > 0x551)
// borland chokes on very big tuples
    {
        typedef tuple40<
            int, int, int, int, int, int, int, int, int, int
          , int, int, int, int, int, int, int, int, int, int
          , int, int, int, int, int, int, int, int, int, int
          , int, int, int, int, int, int, int, int, int, int> type;

        type t; // compile check only
        cout << "tuple40 of int: " << sizeof(t) << endl;
    }

    {
        typedef tuple50<
            int, int, int, int, int, int, int, int, int, int
          , int, int, int, int, int, int, int, int, int, int
          , int, int, int, int, int, int, int, int, int, int
          , int, int, int, int, int, int, int, int, int, int
          , int, int, int, int, int, int, int, int, int, int> type;

        type t; // compile check only
        cout << "tuple50 of int: " << sizeof(t) << endl;
    }
#endif
#endif

    return 0;
}

