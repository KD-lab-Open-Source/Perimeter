/*=============================================================================
    Copyright (c) 2003 Joel de Guzman

    Use, modification and distribution is subject to the Boost Software
    License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#if !defined(FUSION_ALGORITHM_REMOVE_IF_HPP)
#define FUSION_ALGORITHM_REMOVE_IF_HPP

#include <boost/spirit/fusion/sequence/filter_view.hpp>
#include <boost/mpl/not.hpp>

#ifdef FUSION_COMFORMING_COMPILER
#include <boost/utility/enable_if.hpp>
#include <boost/spirit/fusion/sequence/is_sequence.hpp>
#include <boost/spirit/fusion/sequence/type_sequence.hpp>
#endif

namespace boost { namespace fusion
{
    template <typename Pred, typename Sequence>
    struct result_of_remove_if
    {
        typedef filter_view<Sequence, mpl::not_<Pred> > type;
    };

    template <typename Pred, typename Sequence>
    inline filter_view<Sequence const, mpl::not_<Pred> >
    remove_if(sequence_base<Sequence> const& seq)
    {
        return filter_view<Sequence const, mpl::not_<Pred> >(seq.cast());
    }

#ifdef FUSION_COMFORMING_COMPILER

    template <typename Pred, typename Sequence>
    inline typename lazy_disable_if<
        fusion::is_sequence<Sequence>
      , result_of_remove_if<Pred, type_sequence<Sequence const> const>
    >::type
    remove_if(Sequence)
    {
        return fusion::remove_if<Pred>(type_sequence<Sequence const>());
    }

#endif

}}

#endif

