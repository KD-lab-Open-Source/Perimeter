/*=============================================================================
    Copyright (c) 2003 Joel de Guzman

    Use, modification and distribution is subject to the Boost Software
    License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#include <boost/test/minimal.hpp>
#include <boost/spirit/fusion/sequence/tuple.hpp>
#include <boost/spirit/fusion/algorithm/for_each.hpp>
#include <boost/mpl/vector_c.hpp>

struct print
{
    template <typename T>
    void operator()(T const& v) const
    {
        std::cout << "[ " << v << " ] ";
    }
};

struct increment
{
    template <typename T>
    void operator()(T& v) const
    {
        ++v;
    }
};

int
test_main(int, char*[])
{
    using namespace boost::fusion;

/// Testing for_each

    {
        typedef tuple<int, char, double, char const*> tuple_type;
        tuple_type t(1, 'x', 3.3, "Ruby");
        for_each(t, print());
        std::cout << std::endl;
    }

    {
        typedef tuple<int, char, double, char const*> tuple_type;
        tuple_type t(1, 'x', 3.3, "Ruby");
        for_each(t, increment());
        std::cout << std::endl;
    }

#ifdef FUSION_COMFORMING_COMPILER
    {
        typedef boost::mpl::vector_c<int, 2, 3, 4, 5, 6> mpl_vec;
        boost::fusion::for_each(mpl_vec(), print());
        std::cout << std::endl;
    }
#endif

    return 0;
}

