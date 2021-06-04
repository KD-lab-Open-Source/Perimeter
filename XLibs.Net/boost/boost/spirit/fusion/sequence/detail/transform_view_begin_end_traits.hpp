/*=============================================================================
    Copyright (c) 2003 Joel de Guzman

    Use, modification and distribution is subject to the Boost Software
    License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#if !defined(FUSION_SEQUENCE_DETAIL_TRANSFORM_VIEW_BEGIN_END_TRAITS_HPP)
#define FUSION_SEQUENCE_DETAIL_TRANSFORM_VIEW_BEGIN_END_TRAITS_HPP

#include <boost/spirit/fusion/detail/config.hpp>

namespace boost { namespace fusion
{
    struct transform_view_tag;

    template <typename Sequence, typename F>
    struct transform_view;

    template <typename First, typename F>
    struct transform_view_iterator;

    template <typename Tag>
    struct begin_traits;

    template <typename Tag>
    struct end_traits;

    template <>
    struct begin_traits<transform_view_tag>
    {
        template <typename Sequence>
        struct algorithm
        {
            typedef typename Sequence::first_type first_type;
            typedef typename Sequence::transform_type transform_type;
            typedef transform_view_iterator<first_type, transform_type> type;

            static type
            apply(Sequence& s)
            {
                return type(s.first, s.f);
            }
        };
    };

    template <>
    struct end_traits<transform_view_tag>
    {
        template <typename Sequence>
        struct algorithm
        {
            typedef typename Sequence::last_type last_type;
            typedef typename Sequence::transform_type transform_type;
            typedef transform_view_iterator<last_type, transform_type> type;

            static type
            apply(Sequence& s)
            {
                return type(s.last, s.f);
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
    struct begin_traits<fusion::transform_view_tag>
        : fusion::begin_traits<fusion::transform_view_tag> {};

    template <>
    struct end_traits<fusion::transform_view_tag>
        : fusion::end_traits<fusion::transform_view_tag> {};
}}

#endif


