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
#include <boost/spirit/fusion/sequence/type_sequence.hpp>
#include <boost/spirit/fusion/algorithm/transform.hpp>
#include <boost/type_traits/is_class.hpp>
#include <boost/type_traits/is_same.hpp>
#include <boost/mpl/range_c.hpp>

#include <boost/spirit/fusion/sequence/generate.hpp>

struct square
{
    template <typename T>
    struct result
    {
        typedef int type;
    };

    template <typename T>
    int operator()(T x) const
    {
        return x * x;
    }
};

int
test_main(int, char*[])
{
    using namespace boost::fusion;

    std::cout << tuple_open('[');
    std::cout << tuple_close(']');
    std::cout << tuple_delimiter(", ");

/// Testing the transform

    {
        typedef boost::mpl::range_c<int, 5, 9> mpl_list1;
        typedef type_sequence<mpl_list1> sequence_type;
        sequence_type sequence;

        std::cout << transform(sequence, square()) << std::endl;
        BOOST_TEST((transform(sequence, square()) == make_tuple(25, 36, 49, 64)));
    }

#ifdef FUSION_COMFORMING_COMPILER
    {
        typedef boost::mpl::range_c<int, 5, 9> mpl_list1;
        std::cout << transform(mpl_list1(), square()) << std::endl;
        BOOST_TEST((transform(mpl_list1(), square()) == make_tuple(25, 36, 49, 64)));
    }
#endif

    return 0;
}

