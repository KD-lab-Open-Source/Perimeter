/*=============================================================================
    Copyright (c) 2003 Joel de Guzman

    Use, modification and distribution is subject to the Boost Software
    License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#if !defined(FUSION_ALGORITHM_INSERT_HPP)
#define FUSION_ALGORITHM_INSERT_HPP

#include <boost/spirit/fusion/sequence/single_view.hpp>
#include <boost/spirit/fusion/sequence/joint_view.hpp>
#include <boost/spirit/fusion/sequence/range.hpp>
#include <boost/spirit/fusion/sequence/begin.hpp>
#include <boost/spirit/fusion/sequence/end.hpp>

#ifdef FUSION_COMFORMING_COMPILER
#include <boost/utility/enable_if.hpp>
#include <boost/spirit/fusion/sequence/is_sequence.hpp>
#include <boost/spirit/fusion/sequence/type_sequence.hpp>
#endif

namespace boost { namespace fusion
{
    template <typename Sequence, typename Position, typename T>
    struct result_of_insert
    {
        typedef typename result_of_begin<Sequence>::type first_type;
        typedef typename result_of_end<Sequence>::type last_type;

        typedef single_view<T> insert_type;
        typedef range<first_type, Position> left_type;
        typedef range<Position, last_type> right_type;
        typedef joint_view<left_type, insert_type> left_insert_type;
        typedef joint_view<left_insert_type, right_type> type;
    };

    template <typename Sequence, typename Position, typename T>
    inline typename result_of_insert<Sequence const, Position, T>::type
    insert(sequence_base<Sequence> const& seq, Position const& pos, T const& x)
    {
        typedef result_of_insert<Sequence const, Position, T> inserter;

        typedef typename inserter::left_type left_type;
        typedef typename inserter::right_type right_type;
        typedef typename inserter::left_insert_type left_insert_type;
        typedef typename inserter::insert_type insert_type;
        typedef typename inserter::type result;

        left_type left(fusion::begin(seq.cast()), pos);
        right_type right(pos, fusion::end(seq.cast()));
        insert_type ins(x);
        left_insert_type left_insert(left, ins);
        return result(left_insert, right);
    }

#ifdef FUSION_COMFORMING_COMPILER

    template <typename Sequence, typename Position, typename T>
    inline typename lazy_disable_if<
        fusion::is_sequence<Sequence>
      , result_of_insert<
            type_sequence<Sequence const> const
          , type_sequence_iterator<Position>
          , T>
    >::type
    insert(Sequence, Position, T const& x)
    {
        return fusion::insert(
            type_sequence<Sequence const>()
          , type_sequence_iterator<Position>()
          , x);
    }

#endif

}} // namespace boost::fusion

#endif

