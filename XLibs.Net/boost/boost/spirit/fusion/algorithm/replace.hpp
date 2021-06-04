/*=============================================================================
    Copyright (c) 2003 Joel de Guzman

    Use, modification and distribution is subject to the Boost Software
    License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#if !defined(FUSION_ALGORITHM_REPLACE_HPP)
#define FUSION_ALGORITHM_REPLACE_HPP

#include <boost/spirit/fusion/sequence/single_view.hpp>
#include <boost/spirit/fusion/sequence/joint_view.hpp>
#include <boost/spirit/fusion/sequence/range.hpp>
#include <boost/spirit/fusion/sequence/begin.hpp>
#include <boost/spirit/fusion/sequence/end.hpp>
#include <boost/spirit/fusion/iterator/equal_to.hpp>

#ifdef FUSION_COMFORMING_COMPILER
#include <boost/utility/enable_if.hpp>
#include <boost/spirit/fusion/sequence/is_sequence.hpp>
#include <boost/spirit/fusion/sequence/type_sequence.hpp>
#endif

namespace boost { namespace fusion
{
    template <typename Sequence, typename Position, typename T>
    struct result_of_replace
    {
        typedef typename result_of_begin<Sequence>::type first_type;
        typedef typename result_of_end<Sequence>::type last_type;
        typedef typename result_of_next<Position>::type next_type;
        BOOST_STATIC_ASSERT((!equal_to<Position, last_type>::value));

        typedef single_view<T> insert_type;
        typedef range<first_type, Position> left_type;
        typedef range<next_type, last_type> right_type;
        typedef joint_view<left_type, insert_type> left_replace_type;
        typedef joint_view<left_replace_type, right_type> type;
    };

    template <typename Sequence, typename Position, typename T>
    inline typename result_of_replace<Sequence const, Position, T>::type
    replace(sequence_base<Sequence> const& seq, Position const& pos, T const& x)
    {
        typedef result_of_replace<Sequence const, Position, T> replacer;

        typedef typename replacer::left_type left_type;
        typedef typename replacer::right_type right_type;
        typedef typename replacer::left_replace_type left_replace_type;
        typedef typename replacer::insert_type insert_type;
        typedef typename replacer::type result;

        left_type left(fusion::begin(seq), pos);
        right_type right(fusion::next(pos), fusion::end(seq));
        insert_type ins(x);
        left_replace_type left_replace(left, ins);
        return result(left_replace, right);
    }

#ifdef FUSION_COMFORMING_COMPILER

    template <typename Sequence, typename Position, typename T>
    inline typename lazy_disable_if<
        fusion::is_sequence<Sequence>
      , result_of_replace<
            type_sequence<Sequence const> const
          , type_sequence_iterator<Position>
          , T>
    >::type
    replace(Sequence, Position, T const& x)
    {
        return fusion::replace(
            type_sequence<Sequence const>()
          , type_sequence_iterator<Position>()
          , x);
    }

#endif

}}

#endif

