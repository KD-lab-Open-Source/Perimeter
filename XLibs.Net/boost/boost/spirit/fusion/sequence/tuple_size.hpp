/*=============================================================================
    Copyright (c) 2001-2003 Joel de Guzman

    Use, modification and distribution is subject to the Boost Software
    License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#if !defined(FUSION_SEQUENCE_TUPLE_SIZE_HPP)
#define FUSION_SEQUENCE_TUPLE_SIZE_HPP

#include <boost/spirit/fusion/detail/config.hpp>

namespace boost { namespace fusion
{
    ///////////////////////////////////////////////////////////////////////////
    //
    //  tuple_size metafunction
    //
    //      Get the size of TupleT. Usage:
    //
    //          tuple_size<TupleT>::value
    //
    ///////////////////////////////////////////////////////////////////////////
    template <typename Tag>
    struct tuple_size_traits
    {
        template <typename Tuple>
        struct algorithm {};
    };

    template <typename Tuple>
    struct tuple_size
        : tuple_size_traits<FUSION_GET_TAG(Tuple)>::template algorithm<Tuple>
    {};

}}

#endif
