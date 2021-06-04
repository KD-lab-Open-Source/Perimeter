/*=============================================================================
    Copyright (c) 2003 Joel de Guzman

    Use, modification and distribution is subject to the Boost Software
    License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#if !defined(FUSION_SEQUENCE_GENERATE_HPP)
#define FUSION_SEQUENCE_GENERATE_HPP

#include <boost/spirit/fusion/sequence/begin.hpp>
#include <boost/spirit/fusion/sequence/end.hpp>
#include <boost/spirit/fusion/sequence/detail/generate.hpp>
#include <boost/spirit/fusion/sequence/detail/sequence_base.hpp>

#ifdef FUSION_COMFORMING_COMPILER
#include <boost/utility/enable_if.hpp>
#include <boost/spirit/fusion/sequence/is_sequence.hpp>
#endif

namespace boost { namespace fusion
{
    template <typename Sequence>
    struct result_of_generate
    {
        typedef typename
            detail::result_of_generate<
                typename result_of_begin<Sequence>::type
              , typename result_of_end<Sequence>::type
            >::type
        type;
    };

    template <typename Sequence>
    inline typename result_of_generate<Sequence const>::type
    generate(sequence_base<Sequence> const& seq)
    {
        return detail::generate(
            fusion::begin(seq.cast())
          , fusion::end(seq.cast()));
    }

#ifdef FUSION_COMFORMING_COMPILER

    template <typename Sequence>
    struct type_sequence;

    template <typename Sequence>
    inline typename lazy_disable_if<
        fusion::is_sequence<Sequence>
      , result_of_generate<type_sequence<Sequence const> >
    >::type
    generate(Sequence)
    {
        return generate(type_sequence<Sequence const>());
    }

#endif
}}

#endif


