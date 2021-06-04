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
#include <boost/spirit/fusion/iterator/tuple_iterator.hpp>
#include <boost/spirit/fusion/sequence/filter_view.hpp>
#include <boost/type_traits/is_class.hpp>
#include <boost/type_traits/is_same.hpp>
#include <boost/mpl/not.hpp>

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

/// Testing filter_view

    { // Testing the static find_if (internal function)

        typedef tuple<int, char, long, X> tuple_type;

        tuple_type t(1, 'x', 987654, X());
        typedef tuple_iterator<0, tuple_type> begin;
        typedef tuple_iterator<4, tuple_type> end;
        typedef detail::static_find_if<begin, end, is_same<_, long> > filter;
        typedef filter::type type;

        BOOST_TEST(*type(t) == 987654);
        std::cout << *type(t) << std::endl;
        std::cout << *filter::apply(begin(t)) << std::endl;
        BOOST_TEST(*type(t) == *filter::apply(begin(t)));
    }

    {
        typedef tuple<Y, char, long, X, bool, double> tuple_type;

        X x; Y y;
        tuple_type t(y, '@', 987654, x, true, 6.6);
        filter_view<tuple_type const, not_<is_class<_> > > view(t);
        std::cout << view << std::endl;
        BOOST_TEST(view == make_tuple('@', 987654, true, 6.6));
    }

    return 0;
}

