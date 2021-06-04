/*=============================================================================
    Copyright (c) 2003 Joel de Guzman

    Use, modification and distribution is subject to the Boost Software
    License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#include <boost/test/minimal.hpp>
#include <boost/spirit/fusion/sequence/tuple.hpp>
#include <boost/spirit/fusion/sequence/io.hpp>
#include <boost/spirit/fusion/algorithm/find_if.hpp>
#include <boost/mpl/vector.hpp>
#include <boost/type_traits/is_same.hpp>

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
    using boost::is_same;
    using boost::mpl::_;

/// Testing find_if

    {
        typedef tuple<int, char, int, double> tuple_type;
        tuple_type t(12345, 'x', 678910, 3.36);

        std::cout << *find_if<is_same<_, char> >(t) << std::endl;
        BOOST_TEST((*find_if<is_same<_, char> >(t) == 'x'));

        std::cout << *find_if<is_same<_, int> >(t) << std::endl;
        BOOST_TEST((*find_if<is_same<_, int> >(t) == 12345));

        std::cout << *find_if<is_same<_, double> >(t) << std::endl;
        BOOST_TEST((*find_if<is_same<_, double> >(t) == 3.36));
    }

#ifdef FUSION_COMFORMING_COMPILER
    {
        typedef boost::mpl::vector<int, char, X, double> mpl_vec;
        BOOST_TEST((*find_if<is_same<_, X> >(mpl_vec()) == 12345));
    }
#endif

    return 0;
}

