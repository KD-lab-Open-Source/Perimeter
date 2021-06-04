/*=============================================================================
    Copyright (c) 2003 Joel de Guzman

    Use, modification and distribution is subject to the Boost Software
    License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#if !defined(FUSION_ITERATOR_DETAIL_TUPLE_ITERATOR_PRIOR_TRAITS_HPP)
#define FUSION_ITERATOR_DETAIL_TUPLE_ITERATOR_PRIOR_TRAITS_HPP

#include <boost/spirit/fusion/detail/config.hpp>
#include <boost/static_assert.hpp>
#include <boost/mpl/prior.hpp>
#include <boost/mpl/greater_equal.hpp>
#include <boost/mpl/int.hpp>

namespace boost { namespace fusion
{
    struct tuple_iterator_tag;

    template <int N, typename Tuple>
    struct tuple_iterator;

    template <typename Tag>
    struct prior_traits;

    namespace detail
    {
        template <typename Iterator>
        struct tuple_iterator_prior_traits_impl
        {
            typedef FUSION_GET_INDEX(Iterator) index;
            typedef FUSION_GET_TUPLE(Iterator) tuple_;
            BOOST_STATIC_ASSERT((
                ::boost::mpl::greater_equal<index, mpl::int_<0> >::value));

            typedef typename mpl::prior<index>::type prior;
            typedef tuple_iterator<FUSION_GET_VALUE(prior), tuple_> type;

            static type
            apply(Iterator const& i);
        };

        template <typename Iterator>
        inline typename tuple_iterator_prior_traits_impl<Iterator>::type
        tuple_iterator_prior_traits_impl<Iterator>::apply(Iterator const& i)
        {
            return type(detail::ref(i.get_tuple()));
        }
    }

    template <>
    struct prior_traits<tuple_iterator_tag>
    {
        template <typename Iterator>
        struct algorithm : detail::tuple_iterator_prior_traits_impl<Iterator> {};
    };
}}

#endif

