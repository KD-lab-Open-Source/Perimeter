/*=============================================================================
    Copyright (C) 1999-2003 Jaakko Järvi
    Copyright (c) 2003 Joel de Guzman

    Use, modification and distribution is subject to the Boost Software
    License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#include <boost/test/minimal.hpp>
#include <boost/spirit/fusion/sequence/tuple.hpp>
#include <boost/spirit/fusion/sequence/get.hpp>
#include <boost/spirit/fusion/sequence/tuple_size.hpp>
#include <boost/spirit/fusion/sequence/is_sequence.hpp>

int
test_main(int, char *[])
{
    using namespace boost::fusion;

    {   // testing const tuples

        const tuple<int, float> t1(5, 3.3f);
        BOOST_TEST(get<0>(t1) == 5);
        BOOST_TEST(get<1>(t1) == 3.3f);
    }

    {   // testing length

        typedef tuple<int, float, double> t1;
        typedef tuple<> t2;

        BOOST_STATIC_ASSERT(tuple_size<t1>::value == 3);
        BOOST_STATIC_ASSERT(tuple_size<t2>::value == 0);
    }

    {   // testing is_sequence

        typedef tuple<int, float, double> t1;
        typedef tuple<> t2;
        typedef tuple<char> t3;

        BOOST_STATIC_ASSERT(is_sequence<t1>::value);
        BOOST_STATIC_ASSERT(is_sequence<t2>::value);
        BOOST_STATIC_ASSERT(is_sequence<t3>::value);
        BOOST_STATIC_ASSERT(!is_sequence<int>::value);
        BOOST_STATIC_ASSERT(!is_sequence<char>::value);
    }

    return 0;
}
