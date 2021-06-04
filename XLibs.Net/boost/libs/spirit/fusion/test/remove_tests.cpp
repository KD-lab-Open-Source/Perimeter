/*=============================================================================
    Copyright (c) 2003 Joel de Guzman

    Use, modification and distribution is subject to the Boost Software
    License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#include <boost/test/minimal.hpp>
#include <boost/spirit/fusion/sequence/tuple.hpp>
#include <boost/spirit/fusion/sequence/io.hpp>
#include <boost/spirit/fusion/sequence/make_tuple.hpp>
#include <boost/spirit/fusion/sequence/equal_to.hpp>
#include <boost/spirit/fusion/algorithm/remove.hpp>
#include <boost/mpl/vector.hpp>

struct X
{
    operator char const*() const
    {
        return "<X-object>";
    }
};

struct Y
{
    operator char const*() const
    {
        return "<Y-object>";
    }
};

int
test_main(int, char*[])
{
    using namespace boost::fusion;

    std::cout << tuple_open('[');
    std::cout << tuple_close(']');
    std::cout << tuple_delimiter(", ");

/// Testing remove

    X x; Y y;
    typedef tuple<Y, char, long, X, bool, double> tuple_type;
    tuple_type t(y, '@', 987654, x, true, 6.6);

    {
        std::cout << remove<X>(t) << std::endl;
        BOOST_TEST((remove<X>(t)
            == make_tuple(y, '@', 987654, true, 6.6)));
    }

    {
        std::cout << remove<Y>(t) << std::endl;
        BOOST_TEST((remove<Y>(t)
            == make_tuple('@', 987654, x, true, 6.6)));
    }

    {
        std::cout << remove<long>(t) << std::endl;
        BOOST_TEST((remove<long>(t)
            == make_tuple(y, '@', x, true, 6.6)));
    }

#ifdef FUSION_COMFORMING_COMPILER
    {
        typedef boost::mpl::vector<Y, char, long, X, bool, double> mpl_vec;
        BOOST_TEST((remove<X>(mpl_vec())
            == tuple<Y, char, long, bool, double>()));
        BOOST_TEST((remove<Y>(mpl_vec())
            == tuple<char, long, X, bool, double>()));
        BOOST_TEST((remove<long>(mpl_vec())
            == tuple<Y, char, X, bool, double>()));
    }
#endif

    return 0;
}

