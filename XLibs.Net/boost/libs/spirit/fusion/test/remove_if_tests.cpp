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
#include <boost/spirit/fusion/algorithm/remove_if.hpp>
#include <boost/type_traits/is_class.hpp>
#include <boost/type_traits/is_same.hpp>
#include <boost/mpl/not.hpp>
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

    using boost::mpl::_;
    using boost::mpl::not_;
    using boost::is_class;
    using boost::is_same;

    std::cout << tuple_open('[');
    std::cout << tuple_close(']');
    std::cout << tuple_delimiter(", ");

/// Testing remove_if

    X x; Y y;
    typedef tuple<Y, char, long, X, bool, double> tuple_type;
    tuple_type t(y, '@', 987654, x, true, 6.6);

    {
        std::cout << remove_if<not_<is_class<_> > >(t) << std::endl;
        BOOST_TEST((remove_if<not_<is_class<_> > >(t)
            == make_tuple(y, x)));
    }

    {
        std::cout << remove_if<is_class<_> >(t) << std::endl;
        BOOST_TEST((remove_if<is_class<_> >(t)
            == make_tuple('@', 987654, true, 6.6)));
    }

#ifdef FUSION_COMFORMING_COMPILER
    {
        typedef boost::mpl::vector<Y, char, long, X, bool, double> mpl_vec;
        BOOST_TEST((remove_if<not_<is_class<_> > >(mpl_vec())
            == tuple<Y, X>()));
        BOOST_TEST((remove_if<is_class<_> >(mpl_vec())
            == tuple<char, long, bool, double>()));
    }
#endif

    return 0;
}

