/*=============================================================================
    Copyright (c) 2003 Joel de Guzman

    Use, modification and distribution is subject to the Boost Software
    License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#if !defined(FUSION_ALGORITHM_FILTER_HPP)
#define FUSION_ALGORITHM_FILTER_HPP

#include <boost/spirit/fusion/sequence/filter_view.hpp>

#ifdef FUSION_COMFORMING_COMPILER
#include <boost/utility/enable_if.hpp>
#include <boost/spirit/fusion/sequence/is_sequence.hpp>
#include <boost/spirit/fusion/sequence/type_sequence.hpp>
#endif

namespace boost { namespace fusion
{
    template <typename Pred, typename Sequence>
    struct result_of_filter
    {
        typedef filter_view<Sequence, Pred> type;
    };

    template <typename Pred, typename Sequence>
    inline filter_view<Sequence const, Pred>
    filter(sequence_base<Sequence> const& seq)
    {
        return filter_view<Sequence const, Pred>(seq.cast());
    }

#ifdef FUSION_COMFORMING_COMPILER

    template <typename Pred, typename Sequence>
    inline typename lazy_disable_if<
        fusion::is_sequence<Sequence>
      , result_of_filter<Pred, type_sequence<Sequence const> const>
    >::type
    filter(Sequence)
    {
        return fusion::filter<Pred>(type_sequence<Sequence const>());
    }

#endif

}}

#endif

