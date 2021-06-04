/*=============================================================================
    Copyright (c) 2003 Joel de Guzman

    Use, modification and distribution is subject to the Boost Software
    License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#include <boost/test/minimal.hpp>
#include <boost/spirit/fusion/sequence/tuple.hpp>
#include <boost/spirit/fusion/sequence/io.hpp>
#include <boost/spirit/fusion/algorithm/fold.hpp>
#include <boost/type_traits/is_same.hpp>
#include <boost/mpl/if.hpp>
#include <boost/mpl/next.hpp>
#include <boost/mpl/int.hpp>
#include <boost/mpl/vector.hpp>

using boost::mpl::if_;
using boost::mpl::int_;
using boost::is_same;

struct add_ints_only
{
    template <typename T, typename State>
    struct result
    {
        typedef State type;
    };

    template <typename T, typename State>
    State const&
    operator()(T const& x, State const& state) const
    {
        return state;
    }

    int
    operator()(int x, int state) const
    {
        return x + state;
    }
};

struct count_ints
{
    template <typename T, typename CountT>
    struct result
    {
        typedef typename
            if_<
                is_same<T, int>
              , typename boost::mpl::next<CountT>::type
              , CountT
            >::type
        type;
    };

    template <typename T, typename CountT>
    typename result<T, CountT>::type
    operator()(T const&, CountT const&) const
    {
        typedef typename result<T, CountT>::type result;
        return result();
    }
};

int
test_main(int, char*[])
{
    using namespace boost::fusion;

/// Testing fold

    {
        typedef tuple<int, char, int, double> tuple_type;
        tuple_type t(12345, 'x', 678910, 3.36);
        int result = fold(t, 0, add_ints_only());
        std::cout << result << std::endl;
        BOOST_TEST(result == 12345+678910);
    }

    {
        typedef tuple<int> tuple_type;
        tuple_type t(12345);

        int n = boost::fusion::fold(t, int_<0>(), count_ints());
        std::cout << n << std::endl;
        BOOST_TEST(n == 1);
    }

    {
        typedef tuple<int, char, int, double, int> tuple_type;
        tuple_type t(12345, 'x', 678910, 3.36, 8756);

        int n = boost::fusion::fold(t, int_<0>(), count_ints());
        std::cout << n << std::endl;
        BOOST_TEST(n == 3);
    }

#ifdef FUSION_COMFORMING_COMPILER
    {
        typedef boost::mpl::vector<int, char, int, double, int> mpl_vec;
        int n = boost::fusion::fold(mpl_vec(), int_<0>(), count_ints());
        std::cout << n << std::endl;
        BOOST_TEST(n == 3);
    }
#endif

    return 0;
}

