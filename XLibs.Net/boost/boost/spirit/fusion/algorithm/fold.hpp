/*=============================================================================
    Copyright (c) 2003 Joel de Guzman

    Use, modification and distribution is subject to the Boost Software
    License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#if !defined(FUSION_ALGORITHM_FOLD_HPP)
#define FUSION_ALGORITHM_FOLD_HPP

#include <boost/spirit/fusion/algorithm/detail/fold.ipp>

#ifdef FUSION_COMFORMING_COMPILER
#include <boost/utility/enable_if.hpp>
#include <boost/spirit/fusion/sequence/is_sequence.hpp>
#include <boost/spirit/fusion/sequence/type_sequence.hpp>
#endif

namespace boost { namespace fusion
{
    template <typename Sequence, typename State, typename F>
    struct result_of_fold
    {
        typedef typename
            detail::static_fold<
                typename result_of_begin<Sequence>::type
              , typename result_of_end<Sequence>::type
              , State
              , F
            >::type
        type;
    };

    template <typename Sequence, typename State, typename F>
    inline typename result_of_fold<Sequence, State, F>::type
    fold(sequence_base<Sequence> const& seq, State const& state, F const& f)
    {
        return detail::fold(
            fusion::begin(seq.cast())
          , fusion::end(seq.cast())
          , state
          , f
          , is_same<
                typename result_of_begin<Sequence>::type
              , typename result_of_end<Sequence>::type>()
        );
    }

#ifdef FUSION_COMFORMING_COMPILER

    template <typename Sequence, typename State, typename F>
    inline typename lazy_disable_if<
        fusion::is_sequence<Sequence>
      , result_of_fold<type_sequence<Sequence const> const, State, F>
    >::type
    fold(Sequence, State const& state, F const& f)
    {
        return fusion::fold(type_sequence<Sequence const>(), state, f);
    }

#endif

}}

#endif

