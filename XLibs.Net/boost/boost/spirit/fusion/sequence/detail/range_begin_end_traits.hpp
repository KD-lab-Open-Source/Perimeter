/*=============================================================================
    Copyright (c) 2003 Joel de Guzman

    Use, modification and distribution is subject to the Boost Software
    License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#if !defined(FUSION_SEQUENCE_DETAIL_RANGE_BEGIN_END_TRAITS_HPP)
#define FUSION_SEQUENCE_DETAIL_RANGE_BEGIN_END_TRAITS_HPP

#include <boost/spirit/fusion/detail/config.hpp>

namespace boost { namespace fusion
{
    struct range_tag;

    template <typename First, typename Last>
    struct range;

    template <typename Tag>
    struct begin_traits;

    template <>
    struct begin_traits<range_tag>
    {
        template <typename Sequence>
        struct algorithm
        {
            typedef typename Sequence::begin_type type;

            static type
            apply(Sequence const& s)
            {
                return s.first;
            }
        };
    };

    template <typename Tag>
    struct end_traits;

    template <>
    struct end_traits<range_tag>
    {
        template <typename Sequence>
        struct algorithm
        {
            typedef typename Sequence::end_type type;

            static type
            apply(Sequence const& s)
            {
                return s.last;
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
    struct begin_traits<fusion::range_tag>
        : fusion::begin_traits<fusion::range_tag> {};

    template <>
    struct end_traits<fusion::range_tag>
        : fusion::end_traits<fusion::range_tag> {};
}}

#endif


