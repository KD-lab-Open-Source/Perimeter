/*=============================================================================
    Copyright (c) 2003 Joel de Guzman

    Use, modification and distribution is subject to the Boost Software
    License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#if !defined(FUSION_ALGORITHM_FOR_EACH_HPP)
#define FUSION_ALGORITHM_FOR_EACH_HPP

#include <boost/spirit/fusion/sequence/begin.hpp>
#include <boost/spirit/fusion/sequence/end.hpp>
#include <boost/spirit/fusion/iterator/equal_to.hpp>
#include <boost/spirit/fusion/algorithm/detail/for_each.ipp>

#ifdef FUSION_COMFORMING_COMPILER
#include <boost/utility/enable_if.hpp>
#include <boost/spirit/fusion/sequence/is_sequence.hpp>
#include <boost/spirit/fusion/sequence/type_sequence.hpp>
#endif

namespace boost { namespace fusion
{
    template <typename Sequence, typename F>
    struct result_of_for_each
    {
        typedef void type;
    };

    template <typename Sequence, typename F>
    inline void
    for_each(sequence_base<Sequence> const& seq, F const& f)
    {
        detail::for_each(
                fusion::begin(seq.cast())
              , fusion::end(seq.cast())
              , f
              , equal_to<
                    typename result_of_begin<Sequence>::type
                  , typename result_of_end<Sequence>::type>());
    }

    template <typename Sequence, typename F>
    inline void
    for_each(sequence_base<Sequence>& seq, F const& f)
    {
        detail::for_each(
                fusion::begin(seq.cast())
              , fusion::end(seq.cast())
              , f
              , equal_to<
                    typename result_of_begin<Sequence>::type
                  , typename result_of_end<Sequence>::type>());
    }

#ifdef FUSION_COMFORMING_COMPILER

    template <typename Sequence, typename F>
    inline typename disable_if<
        fusion::is_sequence<Sequence>
      , void
    >::type
    for_each(Sequence, F const& f)
    {
        return fusion::for_each(type_sequence<Sequence const>(), f);
    }

#endif

}}

#endif

