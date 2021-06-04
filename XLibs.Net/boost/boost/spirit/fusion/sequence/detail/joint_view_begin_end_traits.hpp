/*=============================================================================
    Copyright (c) 2003 Joel de Guzman

    Use, modification and distribution is subject to the Boost Software
    License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#if !defined(FUSION_SEQUENCE_DETAIL_JOINT_VIEW_BEGIN_END_TRAITS_HPP)
#define FUSION_SEQUENCE_DETAIL_JOINT_VIEW_BEGIN_END_TRAITS_HPP

#include <boost/spirit/fusion/detail/config.hpp>
#include <boost/spirit/fusion/iterator/equal_to.hpp>
#include <boost/mpl/if.hpp>

namespace boost { namespace fusion
{
    struct joint_view_tag;

    template <typename View1, typename View2>
    struct joint_view;

    template <typename First, typename Last, typename Concat>
    struct joint_view_iterator;

    template <>
    struct begin_traits<joint_view_tag>
    {
        template <typename Sequence>
        struct algorithm
        {
            typedef typename Sequence::first_type first_type;
            typedef typename Sequence::last_type last_type;
            typedef typename Sequence::concat_type concat_type;
            typedef equal_to<first_type, last_type> equal_to;

            typedef typename
                mpl::if_<
                    equal_to
                  , concat_type
                  , joint_view_iterator<first_type, last_type, concat_type>
                >::type
            type;

            static type
            apply(Sequence& s, mpl::true_)
            {
                return s.concat;
            }

            static type
            apply(Sequence& s, mpl::false_)
            {
                return type(s.first, s.concat);
            }

            static type
            apply(Sequence& s)
            {
                return apply(s, equal_to());
            }
        };
    };

    template <>
    struct end_traits<joint_view_tag>
    {
        template <typename Sequence>
        struct algorithm
        {
            typedef typename Sequence::concat_last_type type;

            static type
            apply(Sequence& s)
            {
                return s.concat_last;
            }
        };
    };
}}

namespace boost { namespace mpl
{
    template <typename Tag>
    struct begin_traits;

    template <typename Tag>
    struct end_traits;

    template <>
    struct begin_traits<fusion::joint_view_tag>
        : fusion::begin_traits<fusion::joint_view_tag> {};

    template <>
    struct end_traits<fusion::joint_view_tag>
        : fusion::end_traits<fusion::joint_view_tag> {};
}}

#endif


