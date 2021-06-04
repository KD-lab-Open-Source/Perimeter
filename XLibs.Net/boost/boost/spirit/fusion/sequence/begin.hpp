/*=============================================================================
    Copyright (c) 2003 Joel de Guzman

    Use, modification and distribution is subject to the Boost Software
    License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#if !defined(FUSION_SEQUENCE_BEGIN_HPP)
#define FUSION_SEQUENCE_BEGIN_HPP

#include <boost/spirit/fusion/detail/config.hpp>
#include <boost/spirit/fusion/sequence/detail/sequence_base.hpp>

#ifdef FUSION_COMFORMING_COMPILER
#include <boost/utility/enable_if.hpp>
#include <boost/spirit/fusion/sequence/is_sequence.hpp>
#endif

namespace boost { namespace fusion
{
    template <typename Tag>
    struct begin_traits
    {
        template <typename Sequence>
        struct algorithm {};
    };

    template <typename Sequence>
    struct result_of_begin
    {
        typedef typename
            begin_traits<FUSION_GET_TAG(Sequence)>::
                template algorithm<Sequence>::type
        type;
    };

    template <typename Sequence>
    inline typename result_of_begin<Sequence const>::type
    begin(sequence_base<Sequence> const& seq)
    {
        return begin_traits<FUSION_GET_TAG(Sequence)>::
            template algorithm<Sequence const>::apply(seq.cast());
    }

    template <typename Sequence>
    inline typename result_of_begin<Sequence>::type
    begin(sequence_base<Sequence>& seq)
    {
        return begin_traits<FUSION_GET_TAG(Sequence)>::
            template algorithm<Sequence>::apply(seq.cast());
    }

#ifdef FUSION_COMFORMING_COMPILER

    template <typename Sequence>
    struct type_sequence;

    template <typename Sequence>
    inline typename lazy_disable_if<
        fusion::is_sequence<Sequence>
      , result_of_begin<type_sequence<Sequence const> >
    >::type
    begin(Sequence)
    {
        return fusion::begin(type_sequence<Sequence const>());
    }

#endif
}}

#endif
