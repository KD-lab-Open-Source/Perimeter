/*=============================================================================
    Copyright (c) 2003 Joel de Guzman

    Use, modification and distribution is subject to the Boost Software
    License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#include <boost/test/minimal.hpp>
#include <boost/spirit/fusion/sequence/tuple.hpp>
#include <boost/spirit/fusion/sequence/io.hpp>
#include <boost/spirit/fusion/sequence/equal_to.hpp>
#include <boost/spirit/fusion/sequence/make_tuple.hpp>
#include <boost/spirit/fusion/algorithm/insert.hpp>
#include <boost/mpl/vector_c.hpp>
#include <boost/mpl/begin_end.hpp>
#include <boost/mpl/advance.hpp>
#include <boost/mpl/int.hpp>
#include <string>

int
test_main(int, char*[])
{
    using namespace boost::fusion;

    std::cout << tuple_open('[');
    std::cout << tuple_close(']');
    std::cout << tuple_delimiter(", ");

/// Testing insert

    {
        char const* s = "Ruby";
        typedef tuple<int, char, double, char const*> tuple_type;
        tuple_type t1(1, 'x', 3.3, s);
        tuple_iterator<2, tuple_type> pos(t1);

        std::cout << insert(t1, pos, 123456) << std::endl;
        BOOST_TEST((insert(t1, pos, 123456)
            == make_tuple(1, 'x', 123456, 3.3, s)));

        std::cout << insert(t1, end(t1), 123456) << std::endl;
        BOOST_TEST((insert(t1, end(t1), 123456)
            == make_tuple(1, 'x', 3.3, s, 123456)));

        std::cout << insert(t1, begin(t1), "glad") << std::endl;
        BOOST_TEST((insert(t1, begin(t1), "glad")
            == make_tuple(std::string("glad"), 1, 'x', 3.3, s)));
    }

#ifdef FUSION_COMFORMING_COMPILER
    {
        typedef boost::mpl::vector_c<int, 1, 2, 3, 4, 5> mpl_vec;
        typedef boost::mpl::begin<mpl_vec>::type mpl_vec_begin;
        typedef boost::mpl::advance<mpl_vec_begin, boost::mpl::int_<3> >::type mpl_vec_at3;

        std::cout << insert(mpl_vec(), mpl_vec_at3(), boost::mpl::int_<66>()) << std::endl;
        BOOST_TEST((insert(mpl_vec(), mpl_vec_at3(), boost::mpl::int_<66>())
            == make_tuple(1, 2, 3, 66, 4, 5)));
    }
#endif

    return 0;
}

