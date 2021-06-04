/*=============================================================================
    Copyright (c) 2003 Joel de Guzman

    Use, modification and distribution is subject to the Boost Software
    License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#if !defined(FUSION_ALGORITHM_ERASE_HPP)
#define FUSION_ALGORITHM_ERASE_HPP

#include <boost/spirit/fusion/sequence/single_view.hpp>
#include <boost/spirit/fusion/sequence/joint_view.hpp>
#include <boost/spirit/fusion/sequence/range.hpp>
#include <boost/spirit/fusion/sequence/begin.hpp>
#include <boost/spirit/fusion/sequence/end.hpp>
#include <boost/spirit/fusion/iterator/next.hpp>
#include <boost/spirit/fusion/iterator/equal_to.hpp>

#ifdef FUSION_COMFORMING_COMPILER
#include <boost/utility/enable_if.hpp>
#include <boost/spirit/fusion/sequence/is_sequence.hpp>
#include <boost/spirit/fusion/sequence/type_sequence.hpp>
#endif

namespace boost { namespace fusion
{
    template <typename Sequence, typename Position>
    struct result_of_erase
    {
        typedef typename result_of_begin<Sequence>::type first_type;
        typedef typename result_of_end<Sequence>::type last_type;
        BOOST_STATIC_ASSERT((!equal_to<Position, last_type>::value));

        typedef typename result_of_next<Position>::type next_type;
        typedef range<first_type, Position> left_type;
        typedef range<next_type, last_type> right_type;
        typedef joint_view<left_type, right_type> type;
    };

    template <typename Sequence, typename Position>
    inline typename result_of_erase<Sequence const, Position>::type
    erase(sequence_base<Sequence> const& seq, Position const& pos)
    {
        typedef result_of_erase<Sequence const, Position> remover;

        typedef typename remover::left_type left_type;
        typedef typename remover::right_type right_type;
        typedef typename remover::type result;

        left_type left(fusion::begin(seq.cast()), pos);
        right_type right(fusion::next(pos), fusion::end(seq.cast()));
        return result(left, right);
    }

#ifdef FUSION_COMFORMING_COMPILER

    template <typename Sequence, typename Position>
    inline typename lazy_disable_if<
        fusion::is_sequence<Sequence>
      , result_of_erase<
            type_sequence<Sequence const> const
          , type_sequence_iterator<Position> >
    >::type
    erase(Sequence, Position)
    {
        return fusion::erase(
            type_sequence<Sequence const>()
          , type_sequence_iterator<Position>());
    }

#endif

}} // namespace boost::fusion

#endif

