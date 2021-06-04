/*=============================================================================
    Copyright (c) 2003 Joel de Guzman

    Use, modification and distribution is subject to the Boost Software
    License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#if !defined(FUSION_ALGORITHM_PUSH_FRONT_HPP)
#define FUSION_ALGORITHM_PUSH_FRONT_HPP

#include <boost/spirit/fusion/sequence/single_view.hpp>
#include <boost/spirit/fusion/sequence/joint_view.hpp>
#include <boost/spirit/fusion/sequence/detail/sequence_base.hpp>

#ifdef FUSION_COMFORMING_COMPILER
#include <boost/utility/enable_if.hpp>
#include <boost/spirit/fusion/sequence/is_sequence.hpp>
#include <boost/spirit/fusion/sequence/type_sequence.hpp>
#endif

namespace boost { namespace fusion
{
    template <typename Sequence, typename T>
    struct result_of_push_front
    {
        typedef joint_view<single_view<T>, Sequence> type;
    };

    template <typename Sequence, typename T>
    inline typename result_of_push_front<Sequence const, T>::type
    push_front(sequence_base<Sequence> const& seq, T const& x)
    {
        typedef joint_view<single_view<T>, Sequence const> result;
        single_view<T> val(x);
        return result(val, seq.cast());
    }

#ifdef FUSION_COMFORMING_COMPILER

    template <typename Sequence, typename T>
    inline typename lazy_disable_if<
        fusion::is_sequence<Sequence>
      , result_of_push_front<type_sequence<Sequence const> const, T>
    >::type
    push_front(Sequence, T const& x)
    {
        return fusion::push_front(type_sequence<Sequence const>(), x);
    }

#endif

}} // namespace boost::fusion

#endif

