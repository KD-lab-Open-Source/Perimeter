/*=============================================================================
    Copyright (c) 2003 Joel de Guzman

    Use, modification and distribution is subject to the Boost Software
    License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#if !defined(FUSION_ALGORITHM_FIND_IF_HPP)
#define FUSION_ALGORITHM_FIND_IF_HPP

#include <boost/spirit/fusion/algorithm/detail/find_if.ipp>
#include <boost/spirit/fusion/sequence/begin.hpp>
#include <boost/spirit/fusion/sequence/end.hpp>

#ifdef FUSION_COMFORMING_COMPILER
#include <boost/utility/enable_if.hpp>
#include <boost/spirit/fusion/sequence/is_sequence.hpp>
#include <boost/spirit/fusion/sequence/type_sequence.hpp>
#endif

namespace boost { namespace fusion
{
    template <typename Pred, typename Sequence>
    struct result_of_find_if
    {
        typedef typename
            detail::static_find_if<
                typename result_of_begin<Sequence>::type
              , typename result_of_end<Sequence>::type
              , Pred
            >::type
        type;
    };

    template <typename Pred, typename Sequence>
    inline typename result_of_find_if<Pred, Sequence const>::type
    find_if(sequence_base<Sequence> const& seq)
    {
        typedef
            detail::static_find_if<
                typename result_of_begin<Sequence const>::type
              , typename result_of_end<Sequence const>::type
              , Pred
            >
        filter;

        return filter::apply(fusion::begin(seq.cast()));
    }

#ifdef FUSION_COMFORMING_COMPILER

    template <typename Pred, typename Sequence>
    inline typename lazy_disable_if<
        fusion::is_sequence<Sequence>
      , result_of_find_if<Pred, type_sequence<Sequence const> const>
    >::type
    find_if(Sequence)
    {
        return fusion::find_if<Pred>(type_sequence<Sequence const>());
    }

#endif

}} // namespace boost::fusion

#endif

