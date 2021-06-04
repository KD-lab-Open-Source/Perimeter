//  Boost string_algo library find.hpp header file  ---------------------------//

//  Copyright Pavol Droba 2002-2003. Use, modification and
//  distribution is subject to the Boost Software License, Version
//  1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for updates, documentation, and revision history.

#ifndef BOOST_STRING_FIND_HPP
#define BOOST_STRING_FIND_HPP

#include <boost/algorithm/string/config.hpp>
#include <boost/algorithm/string/collection_traits.hpp>
#include <boost/algorithm/string/iterator_range.hpp>
#include <boost/algorithm/string/finder.hpp>
#include <boost/algorithm/string/compare.hpp>
#include <boost/algorithm/string/constants.hpp>

/*! \file
    Defines a set of find algorithms. The algorithms are searching
    for a subsequence of the input. The result is given as an \c iterator_range
    delimiting the substring.
*/

namespace boost {
    namespace algorithm {

//  Generic find -----------------------------------------------//

        //! Generic find algorithm
        /*!
            Search the input using the given finder.

            \param Input A collection which will be searched.
            \param Finder Finder object used for searching.
            \return 
                An \c iterator_range delimiting the match. 
                Returned iterator is either \c CollectionT::iterator or 
                \c CollectionT::const_iterator, depending on the constness of 
                the input parameter.
        */
        template<typename CollectionT, typename FinderT>
        inline iterator_range< 
            BOOST_STRING_TYPENAME result_iterator_of<CollectionT>::type>
        find( 
            CollectionT& Input, 
            FinderT Finder)
        {
            return Finder(begin(Input),end(Input));
        }

//  find_first  -----------------------------------------------//

        //! Find first algorithm
        /*!
            Search for a first match of search sequence in the 
            input collection. 
            
            \param Input A container which will be searched.
            \param Search A string to be searched for.
            \return 
                An \c iterator_range delimiting the match. 
                Returned iterator is either \c CollectionT::iterator or 
                \c CollectionT::const_iterator, depending on the constness of 
                the input parameter.
        */
        template<typename Collection1T, typename Collection2T>
        inline iterator_range< 
            BOOST_STRING_TYPENAME result_iterator_of<Collection1T>::type>
        find_first( 
            Collection1T& Input, 
            const Collection2T& Search)
        {
            return first_finder(Search)(
                begin(Input),end(Input));
        }

        //! Find first algorithm ( case insensitive )
        /*!
            Search for a first match of search sequence in the 
            input collection. Searching is case insensitive.
            
            \param Input A container which will be searched.
            \param Search A string to be searched for.
            \param Loc a locale used for case insensitive comparison
            \return 
                An \c iterator_range delimiting the match. 
                Returned iterator is either \c Collection1T::iterator or 
                \c Collection1T::const_iterator, depending on the constness of 
                the input parameter.
        */
        template<typename Collection1T, typename Collection2T>
        inline iterator_range< 
            BOOST_STRING_TYPENAME result_iterator_of<Collection1T>::type>
        ifind_first( 
            Collection1T& Input, 
            const Collection2T& Search,
            const std::locale& Loc=std::locale())
        {
            return first_finder(Search,is_iequal(Loc))(
                begin(Input),end(Input));
        }

//  find_last  -----------------------------------------------//

        //! Find last algorithm
        /*!
            Search for a last match of search sequence in the 
            input collection. 
            
            \param Input A container which will be searched.
            \param Search A string to be searched for.
            \return 
                An \c iterator_range delimiting the match. 
                Returned iterator is either \c Collection1T::iterator or 
                \c Collection1T::const_iterator, depending on the constness of 
                the input parameter.
        */
        template<typename Collection1T, typename Collection2T>
        inline iterator_range< 
            BOOST_STRING_TYPENAME result_iterator_of<Collection1T>::type>
        find_last( 
            Collection1T& Input, 
            const Collection2T& Search)
        {
            return last_finder(Search)(
                begin(Input),end(Input));
        }

        //! Find last algorithm ( case insensitive )
        /*!
            Search for a last match of search sequence in the 
            input collection. Searching is case insensitive.
            
            \param Input A container which will be searched.
            \param Search A string to be searched for.
            \param Loc a locale used for case insensitive comparison
            \return 
                An \c iterator_range delimiting the match. 
                Returned iterator is either \c Collection1T::iterator or 
                \c Collection1T::const_iterator, depending on the constness of 
                the input parameter.
        */
        template<typename Collection1T, typename Collection2T>
        inline iterator_range< 
            BOOST_STRING_TYPENAME result_iterator_of<Collection1T>::type>
        ifind_last( 
            Collection1T& Input, 
            const Collection2T& Search,
            const std::locale& Loc=std::locale())
        {
            return last_finder(Search, is_iequal(Loc))(
                begin(Input),end(Input));
        }

//  find_nth ----------------------------------------------------------------------//

        //! Find n-th algorithm 
        /*!
            Search for an n-th match of search sequence in the 
            input collection.         
            
            \param Input A container which will be searched.
            \param Search A string to be searched for.
            \param Nth An index of the match to be found.
            \return 
                An \c iterator_range delimiting the match. 
                Returned iterator is either \c Collection1T::iterator or 
                \c Collection1T::const_iterator, depending on the constness of 
                the input parameter.
        */
        template<typename Collection1T, typename Collection2T>
        inline iterator_range< 
            BOOST_STRING_TYPENAME result_iterator_of<Collection1T>::type>
        find_nth( 
            Collection1T& Input, 
            const Collection2T& Search,
            unsigned int Nth)
        {
            return nth_finder(Search,Nth)(
                begin(Input),end(Input));
        }

        //! Find n-th algorithm ( case insensitive ).
        /*!
            Search for an n-th match of search sequence in the 
            input collection. Searching is case insensitive.
            
            \param Input A container which will be searched.
            \param Search A string to be searched for.
            \param Nth An index of the match to be found.
            \param Loc a locale used for case insensitive comparison
            \return 
                An \c iterator_range delimiting the match. 
                Returned iterator is either \c Collection1T::iterator or 
                \c Collection1T::const_iterator, depending on the constness of 
                the input parameter.
        */
        template<typename Collection1T, typename Collection2T>
        inline iterator_range< 
            BOOST_STRING_TYPENAME result_iterator_of<Collection1T>::type>
        ifind_nth( 
            Collection1T& Input, 
            const Collection2T& Search,
            unsigned int Nth,
            const std::locale& Loc=std::locale())
        {
            return nth_finder(Search,Nth,is_iequal(Loc))(
                begin(Input),end(Input));
        }

//  find_head ----------------------------------------------------------------------//

        //! Find head algorithm
        /*!
            Get the head of the input. Head is a prefix of 
            a seqence of given size. If the sequence is shorter then required,
            whole sequence if considered to be the head.

            \param Input A container which will be searched.
            \param N A length of the head
            \return 
                An \c iterator_range delimiting the match. 
                Returned iterator is either \c Collection1T::iterator or 
                \c Collection1T::const_iterator, depending on the constness of 
                the input parameter.
        */
        template<typename CollectionT>
        inline iterator_range< 
            BOOST_STRING_TYPENAME result_iterator_of<CollectionT>::type>
        find_head( 
            CollectionT& Input, 
            unsigned int N)
        {
            return head_finder(N)(
                begin(Input),end(Input));      
        }

//  find_tail ----------------------------------------------------------------------//

        //! Find tail algorithm
        /*!
            Get the tail of the input. Head is a suffix of 
            a seqence of given size. If the sequence is shorter then required,
            whole sequence if considered to be the tail.

            \param Input A container which will be searched.
            \param N A length of the tail
            \return 
                An \c iterator_range delimiting the match. 
                Returned iterator is either \c CollectionT::iterator or 
                \c CollectionT::const_iterator, depending on the constness of 
                the input parameter.
        */
        template<typename CollectionT>
        inline iterator_range< 
            BOOST_STRING_TYPENAME result_iterator_of<CollectionT>::type>
        find_tail( 
            CollectionT& Input, 
            unsigned int N)
        {
            return tail_finder(N)(
                begin(Input),end(Input));      
        }

//  find_token --------------------------------------------------------------------//

        //! Find token algorithm
        /*!
            Look for a given token in the collection. Token is specified by a predicate.
            If bCompressed is given, adjacent tokens are considered to be one match.
            
            \param Input A container which will be searched.
            \param Pred An unary predicate to identify a token
            \param eCompress Enable/Disable compressing of adjacent tokens
            \return 
                An \c iterator_range delimiting the match. 
                Returned iterator is either \c CollectionT::iterator or 
                \c CollectionT::const_iterator, depending on the constness of 
                the input parameter.
        */
        template<typename CollectionT, typename PredicateT>
        inline iterator_range< 
            BOOST_STRING_TYPENAME result_iterator_of<CollectionT>::type>
        find_token( 
            CollectionT& Input,
            PredicateT Pred,
            token_compress_mode_type eCompress=token_compress_off)
        {
            return token_finder(Pred, eCompress)(
                begin(Input),end(Input));       
        }

    } // namespace algorithm

    // pull names to the boost namespace
    using algorithm::find;
    using algorithm::find_first;
    using algorithm::ifind_first;
    using algorithm::find_last;
    using algorithm::ifind_last;
    using algorithm::find_nth;
    using algorithm::ifind_nth;
    using algorithm::find_head;
    using algorithm::find_tail;
    using algorithm::find_token;

} // namespace boost


#endif  // BOOST_STRING_FIND_HPP
