/*=============================================================================
    Copyright (c) 2003 Joel de Guzman

    Use, modification and distribution is subject to the Boost Software
    License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#if !defined(FUSION_SEQUENCE_DETAIL_TYPE_SEQUENCE_BEGIN_END_TRAITS_HPP)
#define FUSION_SEQUENCE_DETAIL_TYPE_SEQUENCE_BEGIN_END_TRAITS_HPP

#include <boost/spirit/fusion/detail/config.hpp>
#include <boost/mpl/begin_end.hpp>

namespace boost { namespace fusion
{
    template <typename Iterator>
    struct type_sequence_iterator;

    struct type_sequence_tag;

    template <typename SequenceT>
    struct type_sequence;

    template <typename Tag>
    struct begin_traits;

    template <>
    struct begin_traits<type_sequence_tag>
    {
        template <typename Sequence>
        struct algorithm
        {
            typedef type_sequence_iterator<
                typename mpl::begin<typename Sequence::sequence_type>::type>
            type;

            static type
            apply(Sequence)
            {
                FUSION_RETURN_DEFAULT_CONSTRUCTED;
            }
        };
    };

    template <typename Tag>
    struct end_traits;

    template <>
    struct end_traits<type_sequence_tag>
    {
        template <typename Sequence>
        struct algorithm
        {
            typedef type_sequence_iterator<
                typename mpl::end<typename Sequence::sequence_type>::type>
            type;

            static type
            apply(Sequence)
            {
                FUSION_RETURN_DEFAULT_CONSTRUCTED;
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
    struct begin_traits<fusion::type_sequence_tag>
        : fusion::begin_traits<fusion::type_sequence_tag> {};

    template <>
    struct end_traits<fusion::type_sequence_tag>
        : fusion::end_traits<fusion::type_sequence_tag> {};
}}

#endif


