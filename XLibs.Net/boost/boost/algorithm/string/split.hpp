//  Boost string_algo library find.hpp header file  ---------------------------//

//  Copyright Pavol Droba 2002-2003. Use, modification and
//  distribution is subject to the Boost Software License, Version
//  1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for updates, documentation, and revision history.

#ifndef BOOST_STRING_SPLIT_HPP
#define BOOST_STRING_SPLIT_HPP

#include <boost/algorithm/string/config.hpp>
#include <boost/algorithm/string/collection_traits.hpp>
#include <boost/algorithm/string/iterator_range.hpp>
#include <boost/algorithm/string/iter_find.hpp>
#include <boost/algorithm/string/finder.hpp>
#include <boost/algorithm/string/compare.hpp>

/*! \file
    Defines basic split algorithms. 
    Split algorithms can be used to divide a sequence
    into several parts according to a given criterium.
    Result is given as a 'container of containers' where
    elements are copies or references to extracted parts.
    This file contains some common specializations of generic
    algorithms contained in the file split2.hpp
*/

namespace boost {
    namespace algorithm {

//  find_all  ------------------------------------------------------------//

        //! Find all algorithm
        /*!
            This algorithm finds all occurrences of the search sequence
            in the input. A result is given as a 'container of containers'.
            Each match of the search sequence is represented by one
            element in the result.

            \param Result A 'container container' to container the result of search.
                Both outer and inner container must have constructor taking a pair
                of iterators as an argument.
                Typical type of the result is 
                    \c std::vector<boost::iterator_range<iterator>).
                (each element of such a vector will container a range delimiting 
                a match).
            \param Input A container which will be searched.
            \param Search A string to be searched for.
            \return A reference the result

            \note Prior content of the result will be overridden.
        */
        template< typename SequenceSequenceT, typename Collection1T, typename Collection2T >
        inline SequenceSequenceT& find_all(
            SequenceSequenceT& Result,
            Collection1T& Input,
            const Collection2T& Search)
        {
            return iter_find(
                Result,
                Input,
                first_finder(Search) );        
        }

        //! Find all algorithm ( case insensitive ) 
        /*!
            This algorithm finds all occurrences of the search sequence
            in the input. A result is given as a 'container of containers'.
            Each match of the search sequence is represented by one
            element in the result. Searching is case insensitive.

            \param Result A 'container container' to container the result of search.
                Both outer and inner container must have constructor taking a pair
                of iterators as an argument.
                Typical type of the result is 
                    \c std::vector<boost::iterator_range<iterator>>.
                (each element of such a vector will container a range delimiting 
                a match).
            \param Input A container which will be searched.
            \param Search A string to be searched for.
            \param Loc a locale used for case insensitive comparison
            \return A reference the result

            \note Prior content of the result will be overridden.
        */
        template< typename SequenceSequenceT, typename Collection1T, typename Collection2T >
        inline SequenceSequenceT& ifind_all(
            SequenceSequenceT& Result,
            Collection1T& Input,
            const Collection2T& Search,
            const std::locale& Loc=std::locale() )
        {
            return iter_find(
                Result,
                Input,
                first_finder(Search, is_iequal(Loc) ) );        
        }


//  tokenize  -------------------------------------------------------------//

        //! Split algorithm
        /*! 
            Tokenize expression. This function is equivalent of C strtok. Input
            sequence is split into tokens, separated by separators. Separators 
            are given in the mean of predicate.

            \param Result A 'container container' to container the result of search.
                Both outer and inner container must have constructor taking a pair
                of iterators as an argument.
                Typical type of the result is 
                    \c std::vector<boost::iterator_range<iterator>>.
                (each element of such a vector will container a range delimiting 
                a match).

            \param Input A container which will be searched.
            \param Pred A predicate to identify separators. This predicate is 
                supposed to return true if a given element is a separator.
            \param eCompress If eCompress argument is set to token_compress_on, adjacent 
                separators are merged together. Otherwise, every two separators
                delimit a token.
            \return A reference the result

            \note Prior content of the result will be overridden.
        */
        template< typename SequenceSequenceT, typename CollectionT, typename PredicateT >
        inline SequenceSequenceT& split(
            SequenceSequenceT& Result,
            CollectionT& Input,
            PredicateT Pred,
            token_compress_mode_type eCompress=token_compress_off )
        {
            return iter_split(
                Result,
                Input,
                token_finder( Pred, eCompress ) );         
        }

    } // namespace algorithm

    // pull names to the boost namespace
    using algorithm::find_all;
    using algorithm::ifind_all;
    using algorithm::split;    

} // namespace boost


#endif  // BOOST_STRING_SPLIT_HPP

