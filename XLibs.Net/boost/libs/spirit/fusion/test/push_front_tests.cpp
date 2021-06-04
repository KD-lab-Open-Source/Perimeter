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
#include <boost/spirit/fusion/algorithm/push_front.hpp>
#include <boost/mpl/vector_c.hpp>
#include <string>

int
test_main(int, char*[])
{
    using namespace boost::fusion;

    std::cout << tuple_open('[');
    std::cout << tuple_close(']');
    std::cout << tuple_delimiter(", ");

/// Testing push_front

    {
        char const* s = "Ruby";
        typedef tuple<int, char, double, char const*> tuple_type;
        tuple_type t1(1, 'x', 3.3, s);

        {
            std::cout << push_front(t1, 123456) << std::endl;
            BOOST_TEST((push_front(t1, 123456)
                == make_tuple(123456, 1, 'x', 3.3, s)));
        }

        {
            std::cout << push_front(t1, "lively") << std::endl;
            BOOST_TEST((push_front(t1, "lively")
                == make_tuple(std::string("lively"), 1, 'x', 3.3, s)));
        }
    }

#ifdef FUSION_COMFORMING_COMPILER
    {
        typedef boost::mpl::vector_c<int, 2, 3, 4, 5, 6> mpl_vec;
        std::cout << boost::fusion::push_front(mpl_vec(), boost::mpl::int_<1>()) << std::endl;
        BOOST_TEST((boost::fusion::push_front(mpl_vec(), boost::mpl::int_<1>())
            == make_tuple(1, 2, 3, 4, 5, 6)));
    }
#endif

    return 0;
}

