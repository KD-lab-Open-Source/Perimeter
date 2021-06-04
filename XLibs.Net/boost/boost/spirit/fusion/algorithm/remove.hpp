/*=============================================================================
    Copyright (c) 2003 Joel de Guzman

    Use, modification and distribution is subject to the Boost Software
    License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#if !defined(FUSION_ALGORITHM_REMOVE_HPP)
#define FUSION_ALGORITHM_REMOVE_HPP

#include <boost/spirit/fusion/sequence/filter_view.hpp>
#include <boost/mpl/not.hpp>
#include <boost/type_traits/is_same.hpp>

#ifdef FUSION_COMFORMING_COMPILER
#include <boost/utility/enable_if.hpp>
#include <boost/spirit/fusion/sequence/is_sequence.hpp>
#include <boost/spirit/fusion/sequence/type_sequence.hpp>
#endif

namespace boost { namespace fusion
{
    template <typename T, typename Sequence>
    struct result_of_remove
    {
        typedef filter_view<Sequence, mpl::not_<is_same<mpl::_, T> > > type;
    };

    template <typename T, typename Sequence>
    inline filter_view<Sequence const, mpl::not_<is_same<mpl::_, T> > >
    remove(sequence_base<Sequence> const& seq)
    {
        return filter_view<
            Sequence const
          , mpl::not_<is_same<mpl::_, T>
        > >(seq.cast());
    }

#ifdef FUSION_COMFORMING_COMPILER

    template <typename T, typename Sequence>
    inline typename lazy_disable_if<
        fusion::is_sequence<Sequence>
      , result_of_remove<T, type_sequence<Sequence const> const>
    >::type
    remove(Sequence)
    {
        return fusion::remove<T>(type_sequence<Sequence const>());
    }

#endif

}}

#endif

