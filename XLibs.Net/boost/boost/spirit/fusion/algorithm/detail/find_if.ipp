/*=============================================================================
    Copyright (c) 2003 Joel de Guzman

    Use, modification and distribution is subject to the Boost Software
    License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#if !defined(FUSION_ALGORITHM_DETAIL_FIND_IF_HPP)
#define FUSION_ALGORITHM_DETAIL_FIND_IF_HPP

#include <boost/mpl/apply_if.hpp>
#include <boost/mpl/or.hpp>
#include <boost/mpl/lambda.hpp>
#include <boost/spirit/fusion/iterator/value_of.hpp>
#include <boost/spirit/fusion/iterator/equal_to.hpp>
#include <boost/spirit/fusion/iterator/next.hpp>

namespace boost { namespace fusion { namespace detail
{
    template <typename Iterator, typename Pred>
    struct apply_filter
    {
        typedef typename
            mpl::apply1<
                Pred
              , typename value_of<Iterator>::type
            >::type
        type;
    };

    template <typename First, typename Last, typename Pred>
    struct main_find_if;

    template <typename First, typename Last, typename Pred>
    struct recursive_find_if
    {
        typedef typename
            main_find_if<
                typename result_of_next<First>::type, Last, Pred
            >::type
        type;
    };

    template <typename First, typename Last, typename Pred>
    struct main_find_if
    {
        typedef mpl::or_<
            equal_to<First, Last>
          , apply_filter<First, Pred> >
        filter;

        typedef typename
            mpl::apply_if<
                filter
              , mpl::identity<First>
              , recursive_find_if<First, Last, Pred>
            >::type
        type;
    };

    template <typename First, typename Last, typename Pred>
    struct static_find_if
    {
        typedef typename
            main_find_if<
                First
              , Last
              , typename mpl::lambda<Pred>::type
            >::type
        type;

        template <typename Iterator>
        static type
        apply(Iterator const& iter, mpl::true_)
        {
            return iter;
        };

        template <typename Iterator>
        static type
        apply(Iterator const& iter, mpl::false_)
        {
            return apply(fusion::next(iter));
        };

        template <typename Iterator>
        static type
        apply(Iterator const& iter)
        {
            typedef equal_to<Iterator, type> found;
            return apply(iter, found());
        };
    };

}}}

#endif
