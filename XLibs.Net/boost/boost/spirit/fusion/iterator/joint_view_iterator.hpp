/*=============================================================================
    Copyright (c) 2003 Joel de Guzman

    Use, modification and distribution is subject to the Boost Software
    License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#if !defined(FUSION_ITERATOR_JOINT_VIEW_ITERATOR_HPP)
#define FUSION_ITERATOR_JOINT_VIEW_ITERATOR_HPP

#include <boost/spirit/fusion/iterator/detail/iterator_base.hpp>
#include <boost/spirit/fusion/iterator/detail/joint_view_iterator/deref_traits.hpp>
#include <boost/spirit/fusion/iterator/detail/joint_view_iterator/next_traits.hpp>
#include <boost/spirit/fusion/iterator/detail/joint_view_iterator/value_traits.hpp>
#include <boost/spirit/fusion/iterator/equal_to.hpp>

namespace boost { namespace fusion
{
    struct joint_view_iterator_tag;

    template <typename First, typename Last, typename Concat>
    struct joint_view_iterator
        : iterator_base<joint_view_iterator<First, Last, Concat> >
    {
        BOOST_STATIC_ASSERT((!equal_to<First, Last>::value));

        typedef joint_view_iterator_tag tag;
        typedef First first_type;
        typedef Last last_type;
        typedef Concat concat_type;

        joint_view_iterator(First const& first, Concat const& concat)
            : first(first), concat(concat)
        {}

        First first;
        Concat concat;
    };
}}

#endif


