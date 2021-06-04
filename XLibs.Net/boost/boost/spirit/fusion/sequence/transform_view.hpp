/*=============================================================================
    Copyright (c) 2003 Joel de Guzman

    Use, modification and distribution is subject to the Boost Software
    License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#if !defined(FUSION_SEQUENCE_TRANSFORM_VIEW_HPP)
#define FUSION_SEQUENCE_TRANSFORM_VIEW_HPP

#include <boost/spirit/fusion/detail/access.hpp>
#include <boost/spirit/fusion/iterator/transform_view_iterator.hpp>
#include <boost/spirit/fusion/sequence/detail/transform_view_begin_end_traits.hpp>
#include <boost/spirit/fusion/sequence/detail/sequence_base.hpp>
#include <boost/spirit/fusion/sequence/begin.hpp>
#include <boost/spirit/fusion/sequence/end.hpp>

namespace boost { namespace fusion
{
    struct transform_view_tag;

    template <typename Sequence, typename F>
    struct transform_view : sequence_base<transform_view<Sequence, F> >
    {
        typedef transform_view_tag tag;
        typedef typename result_of_begin<Sequence>::type first_type;
        typedef typename result_of_end<Sequence>::type last_type;
        typedef F transform_type;

        transform_view(Sequence& seq, F f)
            : first(fusion::begin(seq))
            , last(fusion::end(seq))
            , f(f)
        {}

        first_type first;
        last_type last;
        transform_type f;
    };
}}

#endif


