/*=============================================================================
    Copyright (c) 2003 Joel de Guzman

    Use, modification and distribution is subject to the Boost Software
    License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#if !defined(FUSION_SEQUENCE_DETAIL_SINGLE_VIEW_BEGIN_END_TRAITS_HPP)
#define FUSION_SEQUENCE_DETAIL_SINGLE_VIEW_BEGIN_END_TRAITS_HPP

#include <boost/spirit/fusion/detail/config.hpp>

namespace boost { namespace fusion
{
    struct single_view_tag;

    template <typename T>
    struct single_view_iterator_end;

    template <typename T>
    struct single_view_iterator;

    template <typename Tag>
    struct begin_traits;

    namespace single_view_detail
    {
        template <typename Sequence>
        struct begin_traits_impl
        {
            typedef single_view_iterator<
                FUSION_GET_VALUE_TYPE(Sequence)>
            type;

            static type
            apply(Sequence const& s);
        };

        template <typename Sequence>
        inline typename begin_traits_impl<Sequence>::type
        begin_traits_impl<Sequence>::apply(Sequence const& s)
        {
            return type(s.val);
        }

        template <typename Sequence>
        struct end_traits_impl
        {
            typedef single_view_iterator_end<
                FUSION_GET_VALUE_TYPE(Sequence)>
            type;

            static type
            apply(Sequence const&);
        };

        template <typename Sequence>
        inline typename end_traits_impl<Sequence>::type
        end_traits_impl<Sequence>::apply(Sequence const&)
        {
            FUSION_RETURN_DEFAULT_CONSTRUCTED;
        }
    }

    template <>
    struct begin_traits<single_view_tag>
    {
        template <typename Sequence>
        struct algorithm : single_view_detail::begin_traits_impl<Sequence> {};
    };

    template <typename Tag>
    struct end_traits;

    template <>
    struct end_traits<single_view_tag>
    {
        template <typename Sequence>
        struct algorithm : single_view_detail::end_traits_impl<Sequence> {};
    };
}}

namespace boost { namespace mpl
{
    template <typename Tag>
    struct begin_traits;

    template <typename Tag>
    struct end_traits;

    template <>
    struct begin_traits<fusion::single_view_tag>
        : fusion::begin_traits<fusion::single_view_tag> {};

    template <>
    struct end_traits<fusion::single_view_tag>
        : fusion::end_traits<fusion::single_view_tag> {};
}}

#endif


