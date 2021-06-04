/*=============================================================================
    Copyright (c) 1999-2003 Jaakko Järvi
    Copyright (c) 2001-2003 Joel de Guzman

    Use, modification and distribution is subject to the Boost Software
    License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#if !defined(FUSION_SEQUENCE_GET_HPP)
#define FUSION_SEQUENCE_GET_HPP

#include <boost/spirit/fusion/detail/config.hpp>
#include <boost/spirit/fusion/sequence/detail/sequence_base.hpp>

namespace boost { namespace fusion
{
    ///////////////////////////////////////////////////////////////////////////
    //
    //  get function
    //
    //      Given a constant integer N and a sequence, returns a reference to
    //      the Nth element of the sequence. (N is a zero based index). Usage:
    //
    //          get<N>(seq)
    //
    ///////////////////////////////////////////////////////////////////////////
    template <typename Tag>
    struct tuple_get_traits
    {
        template <int N, typename Sequence>
        struct algorithm {};
    };

    template <int N, typename Sequence>
    struct result_of_get
    {
        typedef FUSION_GET_TAG(Sequence) tag;

        typedef typename
            tuple_get_traits<tag>::
                template algorithm<N, Sequence>::type
        type;
    };

    template <int N, typename Sequence>
    inline typename result_of_get<N, Sequence const>::type
    get(sequence_base<Sequence> const& seq FUSION_GET_MSVC_WORKAROUND)
    {
        typedef FUSION_GET_TAG(Sequence) tag;

        return tuple_get_traits<tag>::
            template algorithm<N, Sequence const>::apply(seq.cast());
    }

    template <int N, typename Sequence>
    inline typename result_of_get<N, Sequence>::type
    get(sequence_base<Sequence>& seq FUSION_GET_MSVC_WORKAROUND)
    {
        typedef FUSION_GET_TAG(Sequence) tag;

        return tuple_get_traits<tag>::
            template algorithm<N, Sequence>::apply(seq.cast());
    }
}}

#endif

