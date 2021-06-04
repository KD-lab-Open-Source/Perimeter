/*=============================================================================
    Copyright (c) 2003 Joel de Guzman

    Use, modification and distribution is subject to the Boost Software
    License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#if !defined(FUSION_ALGORITHM_TRANSFORM_HPP)
#define FUSION_ALGORITHM_TRANSFORM_HPP

#include <boost/spirit/fusion/sequence/transform_view.hpp>

#ifdef FUSION_COMFORMING_COMPILER
#include <boost/utility/enable_if.hpp>
#include <boost/spirit/fusion/sequence/is_sequence.hpp>
#include <boost/spirit/fusion/sequence/type_sequence.hpp>
#endif

namespace boost { namespace fusion
{
    template <typename Sequence, typename F>
    struct result_of_transform
    {
        typedef transform_view<Sequence, F> type;
    };

    template <typename Sequence, typename F>
    inline transform_view<Sequence const, F>
    transform(sequence_base<Sequence> const& seq, F const& f)
    {
        return transform_view<Sequence const, F>(seq.cast(), f);
    }

#ifdef FUSION_COMFORMING_COMPILER

    template <typename Sequence, typename F>
    inline typename lazy_disable_if<
        fusion::is_sequence<Sequence>
      , result_of_transform<type_sequence<Sequence const> const, F>
    >::type
    transform(Sequence, F const& f)
    {
        return fusion::transform(type_sequence<Sequence const>(), f);
    }

#endif

}}

#endif

