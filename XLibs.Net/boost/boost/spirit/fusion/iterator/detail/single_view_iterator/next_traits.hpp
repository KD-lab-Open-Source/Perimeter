/*=============================================================================
    Copyright (c) 2003 Joel de Guzman

    Use, modification and distribution is subject to the Boost Software
    License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#if !defined(FUSION_ITERATOR_DETAIL_SINGLE_VIEW_ITERATOR_NEXT_TRAITS_HPP)
#define FUSION_ITERATOR_DETAIL_SINGLE_VIEW_ITERATOR_NEXT_TRAITS_HPP

#include <boost/spirit/fusion/detail/config.hpp>

namespace boost { namespace fusion
{
    struct single_view_iterator_tag;

    template <typename T>
    struct single_view_iterator_end;

    template <typename T>
    struct single_view_iterator;

    template <typename Tag>
    struct next_traits;

    template <>
    struct next_traits<single_view_iterator_tag>
    {
        template <typename Iterator>
        struct algorithm
        {
            typedef single_view_iterator_end<
                FUSION_GET_VALUE_TYPE(Iterator)>
            type;

            static type
            apply(Iterator)
            {
                FUSION_RETURN_DEFAULT_CONSTRUCTED;
            };
        };
    };
}}

#endif


