//  Boost string_algo library finder.hpp header file  ---------------------------//

//  Copyright Pavol Droba 2002-2003. Use, modification and
//  distribution is subject to the Boost Software License, Version
//  1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for updates, documentation, and revision history.

#ifndef BOOST_STRING_FINDER_HPP
#define BOOST_STRING_FINDER_HPP

#include <boost/algorithm/string/config.hpp>
#include <boost/algorithm/string/constants.hpp>
#include <boost/algorithm/string/collection_traits.hpp>
#include <boost/algorithm/string/iterator_range.hpp>
#include <boost/algorithm/string/detail/finder.hpp>
#include <boost/algorithm/string/compare.hpp>

/*! \file
    Defines Finder generators. Finder object is a functor which is able to 
    find a subsequence matching a specific criterium in an input
    sequence. 
    Finders are used as a pluggable components for replace, find 
    and split facilities. This header contains generator functions 
    for finders provided in this library.
*/

namespace boost {
    namespace algorithm {

//  Finder generators ------------------------------------------//
        
        //! "First" finder 
        /*!
            Construct the \c first_finder. The finder searches for the first
            occurrence of the search sequence in a given input.
            The result is given as a \c iterator_range delimiting the match.

            \param Search A sequence to be searched for.
            \return An instance of the \c first_finder object
        */
        template<typename ContainerT>
        inline detail::first_finderF<
            BOOST_STRING_TYPENAME const_iterator_of<ContainerT>::type,
            is_equal>
        first_finder( const ContainerT& Search )
        {
            return 
                detail::first_finderF<
                    BOOST_STRING_TYPENAME 
                        const_iterator_of<ContainerT>::type,
                    is_equal>( Search, is_equal() ) ;
        }

        //! "First" finder
        /*!
            Construct the \c first_finder. The finder searches for the first
            occurrence of the search sequence in a given input.
            The result is given as a \c iterator_range delimiting the match.

            Elements are compared using the given predicate.

            \param Search A sequence to be searched for.
            \param Comp An element comparison predicate
            \return An instance of the \c first_finder object
        */
        template<typename ContainerT,typename PredicateT>
        inline detail::first_finderF<
            BOOST_STRING_TYPENAME const_iterator_of<ContainerT>::type,
            PredicateT>
        first_finder( 
            const ContainerT& Search, PredicateT Comp )
        {
            return 
                detail::first_finderF<
                    BOOST_STRING_TYPENAME 
                        const_iterator_of<ContainerT>::type,
                    PredicateT>( Search, Comp );
        }

        //! "Last" finder
        /*!
            Construct the \c last_finder. The finder searches for the last
            occurrence of the search sequence in a given input.
            The result is given as a \c iterator_range delimiting the match.

            \param Search A sequence to be searched for.
            \return An instance of the \c last_finder object
        */
        template<typename ContainerT>
        inline detail::last_finderF<
            BOOST_STRING_TYPENAME const_iterator_of<ContainerT>::type,
            is_equal>
        last_finder( const ContainerT& Search )
        {
            return 
                detail::last_finderF<
                    BOOST_STRING_TYPENAME 
                        const_iterator_of<ContainerT>::type,
                    is_equal>( Search, is_equal() );
        }
        //! "Last" finder
        /*!
            Construct the \c last_finder. The finder searches for the last
            occurrence of the search sequence in a given input.
            The result is given as a \c iterator_range delimiting the match.

            Elements are compared using the given predicate.

            \param Search A sequence to be searched for.
            \param Comp An element comparison predicate
            \return An instance of the \c last_finder object
        */
        template<typename ContainerT, typename PredicateT>
        inline detail::last_finderF<
            BOOST_STRING_TYPENAME const_iterator_of<ContainerT>::type,
            PredicateT>
        last_finder( const ContainerT& Search, PredicateT Comp )
        {
            return 
                detail::last_finderF<
                    BOOST_STRING_TYPENAME 
                        const_iterator_of<ContainerT>::type,
                    PredicateT>( Search, Comp ) ;
        }

        //! "Nth" finder
        /*!
            Construct the \c nth_finder. The finder searches for the n-th
            occurrence of the search sequence in a given input.
            The result is given as a \c iterator_range delimiting the match.

            \param Search A sequence to be searched for.
            \param Nth An index of the match to be find
            \return An instance of the \c nth_finder object
        */
        template<typename ContainerT>
        inline detail::nth_finderF<
            BOOST_STRING_TYPENAME const_iterator_of<ContainerT>::type,
            is_equal>
        nth_finder( 
            const ContainerT& Search, 
            unsigned int Nth)
        {
            return 
                detail::nth_finderF<
                    BOOST_STRING_TYPENAME 
                        const_iterator_of<ContainerT>::type,
                    is_equal>( Search, Nth, is_equal() ) ;
        }
        //! "Nth" finder
        /*!
            Construct the \c nth_finder. The finder searches for the n-th
            occurrence of the search sequence in a given input.
            The result is given as a \c iterator_range delimiting the match.

            Elements are compared using the given predicate.

            \param Search A sequence to be searched for.
            \param Nth An index of the match to be find
            \param Comp An element comparison predicate
            \return An instance of the \c nth_finder object
        */
        template<typename ContainerT, typename PredicateT>
        inline detail::nth_finderF<
            BOOST_STRING_TYPENAME const_iterator_of<ContainerT>::type,
            PredicateT>
        nth_finder( 
            const ContainerT& Search, 
            unsigned int Nth, 
            PredicateT Comp )
        {
            return 
                detail::nth_finderF<
                    BOOST_STRING_TYPENAME 
                        const_iterator_of<ContainerT>::type,
                    PredicateT>( Search, Nth, Comp );
        }

        //! "Head" finder
        /*!
            Construct the \c head_finder. The finder returns a head of a given
            input. Head is a prefix of a sequence up to n elements in
            size. If an input has less then n elements, whole input is 
            considered a head.
            The result is given as a \c iterator_range delimiting the match.

            \param N The size of the head
            \return An instance of the \c head_finder object
        */
        inline detail::head_finderF
        head_finder( unsigned int N )
        {
            return detail::head_finderF(N);
        }
        
        //! "Tail" finder
        /*!
            Construct the \c tail_finder. The finder returns a tail of a given
            input. Tail is a suffix of the sequence up to n elements in
            size. If an input has less then n elements, whole input is 
            considered a head.
            The result is given as a \c iterator_range delimiting the match.

            \param N The size of the head
            \return An instance of the \c tail_finder object
        */
        inline detail::tail_finderF
        tail_finder( unsigned int N )
        {
            return detail::tail_finderF(N);
        }

        //! "Token" finder
        /*!
            Construct the \c token_finder. The finder searches for a token 
            specified by a predicate. It is similar to std::find_if 
            algorithm, with an exception that it return a range of
            instead of a single iterator.

            If bCompress is set to true, adjacent matching tokens are 
            concatenated into one match. Thus the finder can be used to 
            search for continuous segments of elements satisfying the 
            given predicate.

            The result is given as a \c iterator_range delimiting the match.

            \param Pred An element selection predicate
            \param eCompress Compress flag
            \return An instance of the \c token_finder object
        */
        template< typename PredicateT >
        inline detail::token_finderF<PredicateT>
        token_finder( 
            PredicateT Pred, 
            token_compress_mode_type eCompress=token_compress_off )
        {
            return detail::token_finderF<PredicateT>( Pred, eCompress );
        }

        //! "Range" finder
        /*!
            Construct the \c range_finder. The finder does not perform 
            any operation. It simply returns the given range for 
            any input. 

            \param Begin Beginning of the range
            \param End End of the range
            \return An instance of the \c range_finger object
        */
        template< typename ForwardIteratorT >
        inline detail::range_finderF<ForwardIteratorT>
        range_finder(
            ForwardIteratorT Begin,
            ForwardIteratorT End )
        {
            return detail::range_finderF<ForwardIteratorT>( Begin, End );
        }

        //! "Range" finder
        /*!
            \param Range The range.

            \overload
        */
        template< typename ForwardIteratorT >
        inline detail::range_finderF<ForwardIteratorT>
        range_finder( iterator_range<ForwardIteratorT> Range )
        {
            return detail::range_finderF<ForwardIteratorT>( Range );
        }

    } // namespace algorithm

    // pull the names to the boost namespace
    using algorithm::first_finder;
    using algorithm::last_finder;
    using algorithm::nth_finder;
    using algorithm::head_finder;
    using algorithm::tail_finder;
    using algorithm::token_finder;
    using algorithm::range_finder;

} // namespace boost


#endif  // BOOST_STRING_FINDER_HPP
