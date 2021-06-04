/*=============================================================================
    Copyright (c) 2003 Joel de Guzman

    Use, modification and distribution is subject to the Boost Software
    License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#if !defined(FUSION_SEQUENCE_DETAIL_TUPLE_GET_TRAITS_HPP)
#define FUSION_SEQUENCE_DETAIL_TUPLE_GET_TRAITS_HPP

#include <boost/spirit/fusion/detail/config.hpp>
#include <boost/spirit/fusion/detail/access.hpp>
#include <boost/spirit/fusion/sequence/detail/tuple_access_result.hpp>

namespace boost { namespace fusion
{
    struct tuple_tag;

    namespace detail
    {
        template <int N>
        struct tuple_access;
    }

    template <typename Tag>
    struct tuple_get_traits;

    namespace detail
    {
        template <int N, typename Tuple>
        struct tuple_get_traits_impl
        {
            typedef BOOST_DEDUCED_TYPENAME
                ::boost::fusion::detail::tuple_access_result<N, Tuple>::type
            type;

            static type
            apply(Tuple& t);
        };

        template <int N, typename Tuple>
        inline typename tuple_get_traits_impl<N, Tuple>::type
        tuple_get_traits_impl<N, Tuple>::apply(Tuple& t)
        {
            return detail::tuple_access<N>::get(t);
        }
    }

    template <>
    struct tuple_get_traits<tuple_tag>
    {
        template <int N, typename Tuple>
        struct algorithm : detail::tuple_get_traits_impl<N, Tuple> {};
    };
}}

#endif
