/*=============================================================================
    Copyright (c) 2003 Joel de Guzman

    Use, modification and distribution is subject to the Boost Software
    License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#if !defined(FUSION_ITERATOR_DETAIL_TUPLE_ITERATOR_NEXT_TRAITS_HPP)
#define FUSION_ITERATOR_DETAIL_TUPLE_ITERATOR_NEXT_TRAITS_HPP

#include <boost/spirit/fusion/detail/config.hpp>
#include <boost/static_assert.hpp>
#include <boost/mpl/next.hpp>
#include <boost/mpl/less.hpp>

namespace boost { namespace fusion
{
    struct tuple_iterator_tag;

    template <int N, typename Tuple>
    struct tuple_iterator;

    template <typename Tag>
    struct next_traits;

    namespace detail
    {
        template <typename Iterator>
        struct tuple_iterator_next_traits_impl
        {
            typedef FUSION_GET_INDEX(Iterator) index;
            typedef FUSION_GET_TUPLE(Iterator) tuple_;
            typedef FUSION_GET_SIZE(tuple_) size;
            BOOST_STATIC_ASSERT((::boost::mpl::less<index, size>::value));

            typedef typename mpl::next<index>::type next;
            typedef tuple_iterator<FUSION_GET_VALUE(next), tuple_> type;

            static type
            apply(Iterator const& i);
        };

        template <typename Iterator>
        inline typename tuple_iterator_next_traits_impl<Iterator>::type
        tuple_iterator_next_traits_impl<Iterator>::apply(Iterator const& i)
        {
            return type(detail::ref(i.get_tuple()));
        }
    }

    template <>
    struct next_traits<tuple_iterator_tag>
    {
        template <typename Iterator>
        struct algorithm : detail::tuple_iterator_next_traits_impl<Iterator> {};
    };
}}

#endif

