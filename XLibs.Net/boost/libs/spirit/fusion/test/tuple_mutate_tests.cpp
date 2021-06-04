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

namespace
{
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

    tuple<int, float, bool, foo> t1(5, 12.2f, true, foo(4));
    get<0>(t1) = 6;
    get<1>(t1) = 2.2f;
    get<2>(t1) = false;
    get<3>(t1) = foo(5);

    BOOST_TEST(get<0>(t1) == 6);
    BOOST_TEST(get<1>(t1) > 2.1f && get<1>(t1) < 2.3f);
    BOOST_TEST(get<2>(t1) == false);
    BOOST_TEST(get<3>(t1) == foo(5));

    return 0;
}
