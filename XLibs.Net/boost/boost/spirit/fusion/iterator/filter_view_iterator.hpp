/*=============================================================================
    Copyright (c) 2003 Joel de Guzman

    Use, modification and distribution is subject to the Boost Software
    License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#if !defined(FUSION_ITERATOR_FILTER_VIEW_ITERATOR_HPP)
#define FUSION_ITERATOR_FILTER_VIEW_ITERATOR_HPP

#include <boost/spirit/fusion/iterator/detail/iterator_base.hpp>
#include <boost/spirit/fusion/iterator/detail/filter_view_iterator/deref_traits.hpp>
#include <boost/spirit/fusion/iterator/detail/filter_view_iterator/next_traits.hpp>
#include <boost/spirit/fusion/iterator/detail/filter_view_iterator/value_traits.hpp>
#include <boost/spirit/fusion/algorithm/detail/find_if.ipp>

namespace boost { namespace fusion
{
    struct filter_view_iterator_tag;

    template <typename First, typename Last, typename Pred>
    struct filter_iterator : iterator_base<filter_iterator<First, Last, Pred> >
    {
        typedef filter_view_iterator_tag tag;
        typedef detail::static_find_if<First, Last, Pred> filter;
        typedef typename filter::type first_type;
        typedef Last last_type;
        typedef Pred pred_type;
        typedef filter_iterator<First, Last, Pred> self_type;

        filter_iterator(First const& first)
            : first(filter::apply(first)) {}

        first_type first;
    };
}}

#endif


