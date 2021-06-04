/*=============================================================================
    Copyright (c) 2003 Joel de Guzman

    Use, modification and distribution is subject to the Boost Software
    License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#if !defined(FUSION_ALGORITHM_DETAIL_FOR_EACH_IPP)
#define FUSION_ALGORITHM_DETAIL_FOR_EACH_IPP

#include <boost/mpl/bool.hpp>
#include <boost/spirit/fusion/iterator/equal_to.hpp>
#include <boost/spirit/fusion/iterator/next.hpp>
#include <boost/spirit/fusion/iterator/deref.hpp>

namespace boost { namespace fusion { namespace detail
{
    template <typename First, typename Last, typename F>
    inline void
    for_each(First const&, Last const&, F const&, mpl::true_)
    {
    }

    template <typename First, typename Last, typename F>
    inline void
    for_each(First const& first, Last const& last, F const& f, mpl::false_)
    {
        f(*first);
        for_each(fusion::next(first), last, f
            , equal_to<typename result_of_next<First>::type, Last>());
    }

//    template <typename First, typename Last, typename F>
//    inline void
//    for_each_non_const(First&, Last&, F const&, mpl::true_)
//    {
//    }
//
//    template <typename First, typename Last, typename F>
//    inline void
//    for_each_non_const(First& first, Last& last, F const& f, mpl::false_)
//    {
//        f(*first);
//        for_each(fusion::next(first), last, f
//            , equal_to<typename result_of_next<First>::type, Last>());
//    }
}}}

#endif

