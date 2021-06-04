/*=============================================================================
    Copyright (c) 2001-2003 Joel de Guzman

    Use, modification and distribution is subject to the Boost Software
    License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#if !defined(FUSION_SEQUENCE_TUPLE_ELEMENT_HPP)
#define FUSION_SEQUENCE_TUPLE_ELEMENT_HPP

#include <boost/spirit/fusion/detail/config.hpp>
#include <boost/mpl/int.hpp>

namespace boost { namespace fusion
{
    ///////////////////////////////////////////////////////////////////////////
    //
    //  tuple_element metafunction
    //
    //      Given a constant integer N and a tuple type Tuple, returns the
    //      tuple element type at slot N. (N is a zero based index). Usage:
    //
    //          tuple_element<N, Tuple>::type
    //
    ///////////////////////////////////////////////////////////////////////////
    template <typename Tag>
    struct tuple_element_traits
    {
        template <typename N, typename Tuple>
        struct algorithm {};
    };

    template <int N, typename Tuple>
    struct tuple_element
        : tuple_element_traits<FUSION_GET_TAG(Tuple)>::
            template algorithm<mpl::int_<N>, Tuple>
    {};
}}

#endif
