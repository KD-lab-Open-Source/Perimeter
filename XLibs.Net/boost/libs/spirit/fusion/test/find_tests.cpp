/*=============================================================================
    Copyright (c) 2003 Joel de Guzman

    Use, modification and distribution is subject to the Boost Software
    License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#include <boost/test/minimal.hpp>
#include <boost/spirit/fusion/sequence/tuple.hpp>
#include <boost/spirit/fusion/algorithm/find.hpp>
#include <boost/spirit/fusion/iterator/deref.hpp>
#include <boost/mpl/vector.hpp>

struct X
{
    operator int() const
    {
        return 12345;
    }
};
int
test_main(int, char*[])
{
    using namespace boost::fusion;

/// Testing find

    {
        typedef tuple<int, char, int, double> tuple_type;
        tuple_type t(12345, 'x', 678910, 3.36);

        std::cout << *find<char>(t) << std::endl;
        BOOST_TEST(*find<char>(t) == 'x');

        std::cout << *find<int>(t) << std::endl;
        BOOST_TEST(*find<int>(t) == 12345);

        std::cout << *find<double>(t) << std::endl;
        BOOST_TEST(*find<double>(t) == 3.36);
    }

#ifdef FUSION_COMFORMING_COMPILER
    {
        typedef boost::mpl::vector<int, char, X, double> mpl_vec;
        BOOST_TEST((*find<X>(mpl_vec()) == 12345));
    }
#endif

    return 0;
}

