/*=============================================================================
    Copyright (c) 2003 Joel de Guzman

    Use, modification and distribution is subject to the Boost Software    License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at    http://www.boost.org/LICENSE_1_0.txt)==============================================================================*/
#include <boost/test/minimal.hpp>
#include <boost/spirit/fusion/sequence/tuple.hpp>
#include <boost/spirit/fusion/sequence/io.hpp>
#include <boost/spirit/fusion/sequence/equal_to.hpp>
#include <boost/spirit/fusion/sequence/make_tuple.hpp>
#include <boost/spirit/fusion/sequence/range.hpp>

int
test_main(int, char*[])
{
    using namespace boost::fusion;

    std::cout << tuple_open('[');
    std::cout << tuple_close(']');
    std::cout << tuple_delimiter(", ");

/// Testing range

    {
        char const* s = "Ruby";
        typedef tuple<int, char, double, char const*> tuple_type;
        tuple_type t1(1, 'x', 3.3, s);

        {
            typedef tuple_iterator<1, tuple_type> i1t;
            typedef tuple_iterator<3, tuple_type> i3t;

            i1t i1(t1);
            i3t i3(t1);

            range<i1t, i3t> slice(i1, i3);
            std::cout << slice << std::endl;
            BOOST_TEST(slice == make_tuple('x', 3.3));
        }

        {
            typedef tuple_iterator<0, tuple_type> i1t;
            typedef tuple_iterator<0, tuple_type> i3t;

            i1t i1(t1);
            i3t i3(t1);

            range<i1t, i3t> slice(i1, i3);
            std::cout << slice << std::endl;
            BOOST_TEST(slice == tuple<>());
        }
    }

    return 0;
}

