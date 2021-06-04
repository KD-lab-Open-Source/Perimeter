/*=============================================================================
    Copyright (c) 2003 Joel de Guzman

    Use, modification and distribution is subject to the Boost Software
    License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#if !defined(FUSION_SEQUENCE_DETAIL_TUPLE_SIZE_TRAITS_HPP)
#define FUSION_SEQUENCE_DETAIL_TUPLE_SIZE_TRAITS_HPP

namespace boost { namespace fusion
{
    struct tuple_tag;

    template <typename Tag>
    struct tuple_size_traits;

    template <>
    struct tuple_size_traits<tuple_tag>
    {
        template <typename Tuple>
        struct algorithm : Tuple::size {};
    };

}}

#endif
