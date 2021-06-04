/*=============================================================================
    Copyright (c) 2003 Joel de Guzman

    Use, modification and distribution is subject to the Boost Software
    License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#if !defined(FUSION_SEQUENCE_DETAIL_TUPLE_BEGIN_END_TRAITS_HPP)
#define FUSION_SEQUENCE_DETAIL_TUPLE_BEGIN_END_TRAITS_HPP

#include <boost/spirit/fusion/detail/config.hpp>

namespace boost { namespace fusion
{
    struct tuple_tag;

    template <int N, typename Tuple>
    struct tuple_iterator;

    template <typename Tag>
    struct begin_traits;

    template <typename Tag>
    struct end_traits;

    namespace tuple_detail
    {
        template <typename Tuple>
        struct begin_traits_impl
        {
            typedef tuple_iterator<0, Tuple> type;

            static type
            apply(Tuple& t);
        };

        template <typename Tuple>
        inline typename begin_traits_impl<Tuple>::type
        begin_traits_impl<Tuple>::apply(Tuple& t)
        {
            return type(t);
        }

        template <typename Tuple>
        struct end_traits_impl
        {
            typedef typename Tuple::size size;
            typedef tuple_iterator<FUSION_GET_VALUE(size), Tuple> type;

            static type
            apply(Tuple& t);
        };

        template <typename Tuple>
        inline typename end_traits_impl<Tuple>::type
        end_traits_impl<Tuple>::apply(Tuple& t)
        {
            return type(t);
        }
    }

    template <>
    struct begin_traits<tuple_tag>
    {
        template <typename Tuple>
        struct algorithm : tuple_detail::begin_traits_impl<Tuple> {};
    };

    template <>
    struct end_traits<tuple_tag>
    {
        template <typename Tuple>
        struct algorithm : tuple_detail::end_traits_impl<Tuple> {};
    };
}}

#include <boost/mpl/begin_end.hpp>
#include <boost/mpl/clear.hpp>
#include <boost/mpl/insert.hpp>
#include <boost/mpl/empty.hpp>
#include <boost/spirit/fusion/sequence/tuple_forward.hpp>

namespace boost { namespace fusion
{
    template <typename Sequence>
    struct result_of_generate;

    template <typename Sequence, typename T>
    struct result_of_push_front;
}}

namespace boost { namespace mpl
{
    // these mpl traits really ought to be placed somewhere else

    template <typename Tag>
    struct begin_traits;

    template <>
    struct begin_traits<fusion::tuple_tag>
        : fusion::begin_traits<fusion::tuple_tag> {};

    template <typename Tag>
    struct end_traits;

    template <>
    struct end_traits<fusion::tuple_tag>
        : fusion::end_traits<fusion::tuple_tag> {};

    template <typename Tag>
    struct clear_traits;

    template <>
    struct clear_traits<fusion::tuple_tag>
    {
        template <typename Tuple>
        struct algorithm
        {
            typedef fusion::tuple<> type;
        };
    };

    template <typename Tag>
    struct push_front_traits;

    template <>
    struct push_front_traits<fusion::tuple_tag>
    {
        template <typename Tuple, typename T>
        struct algorithm
        {
            typedef typename
                fusion::result_of_generate<
                    typename fusion::result_of_push_front<Tuple, T>::type
                >::type
            type;
        };
    };
}}

#endif
