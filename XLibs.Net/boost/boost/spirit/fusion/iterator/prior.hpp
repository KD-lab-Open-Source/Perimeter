/*=============================================================================
    Copyright (c) 2003 Joel de Guzman

    Use, modification and distribution is subject to the Boost Software
    License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#if !defined(FUSION_ITERATOR_PRIOR_HPP)
#define FUSION_ITERATOR_PRIOR_HPP

#include <boost/spirit/fusion/detail/config.hpp>

namespace boost { namespace fusion
{
    template <typename Tag>
    struct prior_traits
    {
        template <typename Iterator>
        struct algorithm {};
    };

    template <typename Iterator>
    struct result_of_prior
    {
        typedef typename
            prior_traits<FUSION_GET_TAG(Iterator)>::
                template algorithm<Iterator>::type
        type;
    };

    template <typename Iterator>
    inline typename result_of_prior<Iterator>::type
    prior(Iterator const& i)
    {
        return prior_traits<FUSION_GET_TAG(Iterator)>::
            template algorithm<Iterator>::apply(i);
    }

}}

#endif
