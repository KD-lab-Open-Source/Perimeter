/*=============================================================================
    Copyright (c) 2003 Joel de Guzman

    Use, modification and distribution is subject to the Boost Software
    License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#if !defined(FUSION_SEQUENCE_DETAIL_TUPLE_ELEMENT_TRAITS_IPP)
#define FUSION_SEQUENCE_DETAIL_TUPLE_ELEMENT_TRAITS_IPP

#include <boost/spirit/fusion/detail/config.hpp>
#include <boost/mpl/at.hpp>

namespace boost { namespace fusion
{
    struct tuple_tag;

    template <typename Tag>
    struct tuple_element_traits;

    template <>
    struct tuple_element_traits<tuple_tag>
    {
        template <typename N, typename Tuple>
        struct algorithm
        {
            typedef typename
                mpl::at<FUSION_GET_TYPES(Tuple), N>::type
            type;
        };
    };
}}

#endif
