/*=============================================================================
    Copyright (c) 2003 Joel de Guzman

    Use, modification and distribution is subject to the Boost Software
    License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#if !defined(FUSION_SEQUENCE_RANGE_HPP)
#define FUSION_SEQUENCE_RANGE_HPP

#include <boost/spirit/fusion/detail/access.hpp>
#include <boost/spirit/fusion/sequence/detail/range_begin_end_traits.hpp>
#include <boost/spirit/fusion/sequence/detail/sequence_base.hpp>

namespace boost { namespace fusion
{
    struct range_tag;

    template <typename First, typename Last>
    struct range : sequence_base<range<First, Last> >
    {
        typedef First begin_type;
        typedef Last end_type;
        typedef range_tag tag;

        range(First const& first, Last const& last)
            : first(first), last(last) {}

        First first;
        Last last;
    };
}}

#endif


