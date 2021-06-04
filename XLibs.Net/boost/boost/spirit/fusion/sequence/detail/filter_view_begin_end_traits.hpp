/*=============================================================================
    Copyright (c) 2003 Joel de Guzman

    Use, modification and distribution is subject to the Boost Software
    License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#if !defined(FUSION_SEQUENCE_DETAIL_FILTER_VIEW_BEGIN_END_TRAITS_HPP)
#define FUSION_SEQUENCE_DETAIL_FILTER_VIEW_BEGIN_END_TRAITS_HPP

#include <boost/spirit/fusion/detail/config.hpp>

namespace boost { namespace fusion
{
    struct filter_view_tag;

    template <typename View, typename Pred>
    struct filter_view;

    template <typename First, typename Last, typename Pred>
    struct filter_iterator;

    template <>
    struct begin_traits<filter_view_tag>
    {
        template <typename Sequence>
        struct algorithm
        {
            typedef typename Sequence::first_type first_type;
            typedef typename Sequence::last_type last_type;
            typedef typename Sequence::pred_type pred_type;
            typedef filter_iterator<first_type, last_type, pred_type> type;

            static type
            apply(Sequence& s)
            {
                return type(s.first);
            }
        };
    };

    template <>
    struct end_traits<filter_view_tag>
    {
        template <typename Sequence>
        struct algorithm
        {
            typedef typename Sequence::last_type last_type;
            typedef typename Sequence::pred_type pred_type;
            typedef filter_iterator<last_type, last_type, pred_type> type;

            static type
            apply(Sequence& s)
            {
                return type(s.last);
            }
        };
    };
}}

namespace boost { namespace mpl
{
    template <typename Tag>
    struct begin_traits;

    template <typename Tag>
    struct end_traits;

    template <>
    struct begin_traits<fusion::filter_view_tag>
        : fusion::begin_traits<fusion::filter_view_tag> {};

    template <>
    struct end_traits<fusion::filter_view_tag>
        : fusion::end_traits<fusion::filter_view_tag> {};
}}

#endif


