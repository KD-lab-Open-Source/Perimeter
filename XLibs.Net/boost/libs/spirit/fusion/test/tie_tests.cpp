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
#include <boost/spirit/fusion/sequence/tie.hpp>
#include <boost/spirit/fusion/sequence/make_tuple.hpp>

namespace
{
    // something to prevent warnings for unused variables
    template<class T> void dummy(const T&) {}

    // no public default constructor
    class foo
    {
    public:

        explicit foo(int v) : val(v) {}

        bool operator==(const foo& other) const
        {
            return val == other.val;
        }

    private:

        foo() {}
        int val;
    };
}

int
test_main(int, char *[])
{
    using namespace boost::fusion;

    int a;
    char b;
    foo c(5);

    tie(a, b, c) = make_tuple(2, 'a', foo(3));
    BOOST_TEST(a == 2);
    BOOST_TEST(b == 'a');
    BOOST_TEST(c == foo(3));

    tie(a, ignore, c) = make_tuple((short int)5, false, foo(5));
    BOOST_TEST(a == 5);
    BOOST_TEST(b == 'a');
    BOOST_TEST(c == foo(5));

    //  testing assignment from std::pair
    int i, j;
    tie (i, j) = std::make_pair(1, 2);
    BOOST_TEST(i == 1 && j == 2);

    tuple<int, int, float> ta;

#ifdef E11
    ta = std::make_pair(1, 2); // should fail, tuple is of length 3, not 2
#endif

    dummy(ta);

    return 0;
}
