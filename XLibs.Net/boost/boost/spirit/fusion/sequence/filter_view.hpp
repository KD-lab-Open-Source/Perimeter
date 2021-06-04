/*=============================================================================
    Copyright (c) 2003 Joel de Guzman

    Use, modification and distribution is subject to the Boost Software
    License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#if !defined(FUSION_SEQUENCE_FILTER_VIEW_HPP)
#define FUSION_SEQUENCE_FILTER_VIEW_HPP

#include <boost/spirit/fusion/detail/access.hpp>
#include <boost/spirit/fusion/iterator/filter_view_iterator.hpp>
#include <boost/spirit/fusion/sequence/detail/filter_view_begin_end_traits.hpp>
#include <boost/spirit/fusion/sequence/detail/sequence_base.hpp>
#include <boost/spirit/fusion/sequence/begin.hpp>
#include <boost/spirit/fusion/sequence/end.hpp>

namespace boost { namespace fusion
{
    struct filter_view_tag;

    template <typename Sequence, typename Pred>
    struct filter_view : sequence_base<filter_view<Sequence, Pred> >
    {
        typedef filter_view_tag tag;
        typedef typename result_of_begin<Sequence>::type first_type;
        typedef typename result_of_end<Sequence>::type last_type;
        typedef Pred pred_type;

        filter_view(Sequence& seq)
            : first(fusion::begin(seq))
            , last(fusion::end(seq))
        {}

        first_type first;
        last_type last;
    };
}}

#endif


