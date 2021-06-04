//  Boost string_algo library replace.hpp header file  ---------------------------//

//  Copyright Pavol Droba 2002-2003. Use, modification and
//  distribution is subject to the Boost Software License, Version
//  1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for updates, documentation, and revision history.

#ifndef BOOST_STRING_REPLACE_HPP
#define BOOST_STRING_REPLACE_HPP

#include <boost/algorithm/string/config.hpp>
#include <boost/algorithm/string/collection_traits.hpp>
#include <boost/algorithm/string/iterator_range.hpp>
#include <boost/algorithm/string/find_format.hpp>
#include <boost/algorithm/string/finder.hpp>
#include <boost/algorithm/string/formatter.hpp>
#include <boost/algorithm/string/compare.hpp>

/*! \file
    Defines various replace algorithms. Each algorithm replaces
    a part(s) of the input according to a searching and a replace criterium.
*/

namespace boost {
    namespace algorithm {

//  replace_range --------------------------------------------------------------------//

        //! Replace range algorithm
        /*!
            Substitute the given range in the input sequence with the
            format sequence. The result is copied to the given output iterator.

            \param Output A output iterator to which the result will be copied
            \param Input An input sequence
            \param SearchRange A range in the input to be substituted
            \param Format A substitute sequence
            \return An output iterator pointing just after last inserted character
        */
        template<
            typename OutputIteratorT,
            typename Collection1T, 
            typename Collection2T>
        inline OutputIteratorT replace_range_copy(
            OutputIteratorT Output,
            const Collection1T& Input,
            const iterator_range<
                BOOST_STRING_TYPENAME 
                    const_iterator_of<Collection1T>::type>& SearchRange,
            const Collection2T& Format)
        {
            return find_format_copy(
                Output,
                Input,
                range_finder(SearchRange),
                const_formatter(Format));
        }

        //! Replace range algorithm
        /*!
            Substitute a given range in the input sequence with the
            format sequence. The result is a modified copy of the input.

            \param Input An input sequence
            \param SearchRange A range in the input to be substituted
            \param Format A substitute sequence
            \return A modified copy of the input
        */
        template<typename SequenceT, typename CollectionT>
        inline SequenceT replace_range_copy( 
            const SequenceT& Input,
            const iterator_range<
                BOOST_STRING_TYPENAME 
                    const_iterator_of<SequenceT>::type>& SearchRange,
            const CollectionT& Format)
        {
            return find_format_copy(
                Input,
                range_finder(SearchRange),
                const_formatter(Format));
        }

        //! Replace range algorithm
        /*!
            Substitute a given range in the input sequence with the
            format sequence. The input sequence is modified in-place.

            \param Input An input sequence
            \param SearchRange A range in the input to be substituted
            \param Format A substitute sequence
        */
        template<typename SequenceT, typename CollectionT>
        inline void replace_range( 
            SequenceT& Input,
            const iterator_range<
                BOOST_STRING_TYPENAME 
                    iterator_of<SequenceT>::type>& SearchRange,
            const CollectionT& Format)
        {
            find_format(
                Input,
                range_finder(SearchRange),
                const_formatter(Format));
        }

//  replace_first --------------------------------------------------------------------//

        //! Replace first algorithm
        /*!
            Substitute a first match of the search sequence in the input 
            with the format sequence. The result is copied to the given output iterator.

            \param Output A output iterator to which the result will be copied
            \param Input An input sequence
            \param Search A string to be searched for. 
            \param Format A substitute sequence
            \return An output iterator pointing just after last inserted character
        */
        template<
            typename OutputIteratorT,
            typename Collection1T, 
            typename Collection2T,
            typename Collection3T>
        inline OutputIteratorT replace_first_copy(
            OutputIteratorT Output,
            const Collection1T& Input,
            const Collection2T& Search,
            const Collection3T& Format)
        {
            return find_format_copy(
                Output,
                Input,
                first_finder(Search),
                const_formatter(Format) );
        }

        //! Replace first algorithm
        /*!
            Substitute a first match of the search sequence in the input 
            with the format sequence. The result is a modified copy of the input.

            \param Input An input sequence
            \param Search A string to be searched for. 
            \param Format A substitute sequence
            \return A modified copy of the input
        */
        template<typename SequenceT, typename Collection1T, typename Collection2T>
        inline SequenceT replace_first_copy( 
            const SequenceT& Input,
            const Collection1T& Search,
            const Collection2T& Format )
        {
            return find_format_copy( 
                Input,
                first_finder(Search),
                const_formatter(Format) );
        }

        //! Replace first algorithm
        /*!
            Substitute a first match of the search sequence in the input 
            with the format sequence. The input sequence is modified in-place.

            \param Input An input sequence
            \param Search A string to be searched for. 
            \param Format A substitute sequence
        */
        template<typename SequenceT, typename Collection1T, typename Collection2T>
        inline void replace_first( 
            SequenceT& Input,
            const Collection1T& Search,
            const Collection2T& Format )
        {
            find_format( 
                Input, 
                first_finder(Search),
                const_formatter(Format) );
        }

//  replace_first ( case insensitive ) ---------------------------------------------//

        //! Replace first algorithm ( case insensitive )
        /*!
            Substitute a first match of the search sequence in the input 
            with the format sequence. The result is a modified copy of the input.
            Searching is case insensitive.

            \param Output A output iterator to which the result will be copied
            \param Input An input sequence
            \param Search A string to be searched for. 
            \param Format A substitute sequence
            \param Loc a locale used for case insensitive comparison
            \return A modified copy of the input
        */
        template<
            typename OutputIteratorT,
            typename Collection1T, 
            typename Collection2T,
            typename Collection3T>
        inline OutputIteratorT ireplace_first_copy(
            OutputIteratorT Output,
            const Collection1T& Input,
            const Collection2T& Search,
            const Collection3T& Format,
            const std::locale& Loc=std::locale() )
        {
            return find_format_copy(
                Output,
                Input,
                first_finder(Search, is_iequal(Loc)),
                const_formatter(Format) );
        }

         //! Replace first algorithm ( case insensitive )
        /*!
            Substitute a first match of the search sequence in the input 
            with the format sequence. The result is copied to the given output iterator.
            Searching is case insensitive.

            \param Input An input sequence
            \param Search A string to be searched for. 
            \param Format A substitute sequence
            \param Loc a locale used for case insensitive comparison
            \return An output iterator pointing just after last inserted character
        */
        template<typename SequenceT, typename Collection2T, typename Collection1T>
        inline SequenceT ireplace_first_copy( 
            const SequenceT& Input,
            const Collection2T& Search,
            const Collection1T& Format,
            const std::locale& Loc=std::locale() )
        {
            return find_format_copy( 
                Input,
                first_finder(Search, is_iequal(Loc)),
                const_formatter(Format) );
        }

        //! Replace first algorithm ( case insensitive )
        /*!
            Substitute a first match of the search sequence in the input 
            with the format sequence. Input sequence is modified in-place.
            Searching is case insensitive.

            \param Input An input sequence
            \param Search A string to be searched for. 
            \param Format A substitute sequence
            \param Loc a locale used for case insensitive comparison
        */
        template<typename SequenceT, typename Collection1T, typename Collection2T>
        inline void ireplace_first( 
            SequenceT& Input,
            const Collection1T& Search,
            const Collection2T& Format,
            const std::locale& Loc=std::locale() )
        {
            find_format( 
                Input, 
                first_finder(Search, is_iequal(Loc)),
                const_formatter(Format) );
        }

//  replace_last --------------------------------------------------------------------//

        //! Replace last algorithm
        /*!
            Substitute a last match of the search sequence in the input 
            with the format sequence. The result is copied to the given output iterator.

            \param Output A output iterator to which the result will be copied
            \param Input An input sequence
            \param Search A string to be searched for. 
            \param Format A substitute sequence
            \return An output iterator pointing just after last inserted character
        */
        template<
            typename OutputIteratorT,
            typename Collection1T, 
            typename Collection2T,
            typename Collection3T>
        inline OutputIteratorT replace_last_copy(
            OutputIteratorT Output,
            const Collection1T& Input,
            const Collection2T& Search,
            const Collection3T& Format )
        {
            return find_format_copy(
                Output,
                Input,
                last_finder(Search),
                const_formatter(Format) );
        }

        //! Replace last algorithm
        /*!
            Substitute a last match of the search sequence in the input 
            with the format sequence. The result is a modified copy of the input.

            \param Input An input sequence
            \param Search A string to be searched for. 
            \param Format A substitute sequence
            \return A modified copy of the input
        */
        template<typename SequenceT, typename Collection1T, typename Collection2T>
        inline SequenceT replace_last_copy( 
            const SequenceT& Input,
            const Collection1T& Search,
            const Collection2T& Format )
        {
            return find_format_copy( 
                Input,
                last_finder(Search),
                const_formatter(Format) );
        }

        //! Replace last algorithm
        /*!
            Substitute a last match of the search sequence in the input 
            with the format sequence. Input sequence is modified in-place.

            \param Input An input sequence
            \param Search A string to be searched for. 
            \param Format A substitute sequence
        */
        template<typename SequenceT, typename Collection1T, typename Collection2T>
        inline void replace_last( 
            SequenceT& Input,
            const Collection1T& Search,
            const Collection2T& Format )
        {
            find_format( 
                Input, 
                last_finder(Search),
                const_formatter(Format) );
        }

//  replace_last ( case insensitive ) -----------------------------------------------//

        //! Replace last algorithm ( case insensitive )
        /*!
            Substitute a last match of the search sequence in the input 
            with the format sequence. The result is copied to the given output iterator.
            Searching is case insensitive.

            \param Output A output iterator to which the result will be copied
            \param Input An input sequence
            \param Search A string to be searched for. 
            \param Format A substitute sequence
            \param Loc a locale used for case insensitive comparison
            \return An output iterator pointing just after last inserted character
        */
        template<
            typename OutputIteratorT,
            typename Collection1T, 
            typename Collection2T,
            typename Collection3T>
        inline OutputIteratorT ireplace_last_copy(
            OutputIteratorT Output,
            const Collection1T& Input,
            const Collection2T& Search,
            const Collection3T& Format,
            const std::locale& Loc=std::locale() )
        {
            return find_format_copy(
                Output,
                Input,
                last_finder(Search, is_iequal(Loc)),
                const_formatter(Format) );
        }

        //! Replace last algorithm ( case insensitive )
        /*!
            Substitute a last match of the search sequence in the input 
            with the format sequence. The result is a modified copy of the input.
            Searching is case insensitive.

            \param Input An input sequence
            \param Search A string to be searched for. 
            \param Format A substitute sequence
            \param Loc a locale used for case insensitive comparison
            \return A modified copy of the input
        */
        template<typename SequenceT, typename Collection1T, typename Collection2T>
        inline SequenceT ireplace_last_copy( 
            const SequenceT& Input,
            const Collection1T& Search,
            const Collection2T& Format,
            const std::locale& Loc=std::locale() )
        {
            return find_format_copy( 
                Input,
                last_finder(Search, is_iequal(Loc)),
                const_formatter(Format) );
        }

        //! Replace last algorithm ( case insensitive )
        /*!
            Substitute a last match of the search sequence in the input 
            with the format sequence.The input sequence is modified in-place.
            Searching is case insensitive.

            \param Input An input sequence
            \param Search A string to be searched for. 
            \param Format A substitute sequence
            \param Loc a locale used for case insensitive comparison
            \return A reference to the modified input
        */
        template<typename SequenceT, typename Collection1T, typename Collection2T>
        inline void ireplace_last( 
            SequenceT& Input,
            const Collection1T& Search,
            const Collection2T& Format,
            const std::locale& Loc=std::locale() )
        {
            find_format( 
                Input, 
                last_finder(Search, is_iequal(Loc)),
                const_formatter(Format) );
        }

//  replace_nth --------------------------------------------------------------------//

        //! Replace nth algorithm
        /*!
            Substitute an Nth match of the search sequence in the input 
            with the format sequence. The result is copied to the given output iterator.

            \param Output A output iterator to which the result will be copied
            \param Input An input sequence
            \param Search A string to be searched for. 
            \param Nth An index of the match to be replaced. The index is 0-based.
            \param Format A substitute sequence
            \return An output iterator pointing just after last inserted character
        */
        template<
            typename OutputIteratorT,
            typename Collection1T, 
            typename Collection2T,
            typename Collection3T>
        inline OutputIteratorT replace_nth_copy(
            OutputIteratorT Output,
            const Collection1T& Input,
            const Collection2T& Search,
            unsigned int Nth,
            const Collection3T& Format )
        {
            return find_format_copy(
                Output,
                Input,
                nth_finder(Search, Nth),
                const_formatter(Format) );
        }

        //! Replace nth algorithm
        /*!
            Substitute an Nth match of the search sequence in the input 
            with the format sequence. The result is a modified copy of the input.

            \param Input An input sequence
            \param Search A string to be searched for. 
            \param Nth An index of the match to be replaced. The index is 0-based.
            \param Format A substitute sequence
            \return A modified copy of the input
        */
        template<typename SequenceT, typename Collection1T, typename Collection2T>
        inline SequenceT replace_nth_copy( 
            const SequenceT& Input,
            const Collection1T& Search,
            unsigned int Nth,
            const Collection2T& Format )
        {
            return find_format_copy( 
                Input,
                nth_finder(Search, Nth),
                const_formatter(Format) );
        }

        //! Replace nth algorithm
        /*!
            Substitute an Nth match of the search sequence in the input 
            with the format sequence. Input sequence is modified in-place.

            \param Input An input sequence
            \param Search A string to be searched for. 
            \param Nth An index of the match to be replaced. The index is 0-based.
            \param Format A substitute sequence
        */
        template<typename SequenceT, typename Collection1T, typename Collection2T>
        inline void replace_nth( 
            SequenceT& Input,
            const Collection1T& Search,
            unsigned int Nth,
            const Collection2T& Format )
        {
            find_format( 
                Input, 
                nth_finder(Search, Nth),
                const_formatter(Format) );
        }

//  replace_nth ( case insensitive ) -----------------------------------------------//
        
        //! Replace nth algorithm ( case insensitive )
        /*!
            Substitute an Nth match of the search sequence in the input 
            with the format sequence. The result is copied to the given output iterator.
            Searching is case insensitive.

            \param Output A output iterator to which the result will be copied
            \param Input An input sequence
            \param Search A string to be searched for. 
            \param Nth An index of the match to be replaced. The index is 0-based.
            \param Format A substitute sequence
            \param Loc a locale used for case insensitive comparison
            \return An output iterator pointing just after last inserted character
        */
        template<
            typename OutputIteratorT,
            typename Collection1T, 
            typename Collection2T,
            typename Collection3T>
        inline OutputIteratorT ireplace_nth_copy(
            OutputIteratorT Output,
            const Collection1T& Input,
            const Collection2T& Search,
            unsigned int Nth,
            const Collection3T& Format,
            const std::locale& Loc=std::locale() )
        {
            return find_format_copy(
                Output,
                Input,
                nth_finder(Search, Nth, is_iequal(Loc) ),
                const_formatter(Format) );
        }

        //! Replace nth algorithm ( case insensitive )
        /*!
            Substitute an Nth match of the search sequence in the input 
            with the format sequence. The result is a modified copy of the input.
            Searching is case insensitive.

            \param Input An input sequence
            \param Search A string to be searched for. 
            \param Nth An index of the match to be replaced. The index is 0-based.
            \param Format A substitute sequence
            \param Loc a locale used for case insensitive comparison
            \return A modified copy of the input
        */
        template<typename SequenceT, typename Collection1T, typename Collection2T>
        inline SequenceT ireplace_nth_copy( 
            const SequenceT& Input,
            const Collection1T& Search,
            unsigned int Nth,
            const Collection2T& Format,
            const std::locale& Loc=std::locale() )
        {
            return find_format_copy( 
                Input,
                nth_finder(Search, Nth, is_iequal(Loc)),
                const_formatter(Format) );
        }

        //! Replace nth algorithm ( case insensitive )
        /*!
            Substitute an Nth match of the search sequence in the input 
            with the format sequence. Input sequence is modified in-place.
            Searching is case insensitive.

            \param Input An input sequence
            \param Search A string to be searched for. 
            \param Nth An index of the match to be replaced. The index is 0-based.
            \param Format A substitute sequence
            \param Loc a locale used for case insensitive comparison
        */
        template<typename SequenceT, typename Collection1T, typename Collection2T>
        inline void ireplace_nth( 
            SequenceT& Input,
            const Collection1T& Search,
            unsigned int Nth,
            const Collection2T& Format,
            const std::locale& Loc=std::locale() )
        {
            find_format( 
                Input, 
                nth_finder(Search, Nth, is_iequal(Loc)),
                const_formatter(Format) );
        }

//  replace_all --------------------------------------------------------------------//

        //! Replace all algorithm
        /*!
            Substitute all occurrences of the search sequence in the input 
            with the format sequence. The result is copied to the given output iterator.

            \param Output A output iterator to which the result will be copied
            \param Input An input sequence
            \param Search A string to be searched for. 
            \param Format A substitute sequence
            \return An output iterator pointing just after last inserted character
        */
        template<
            typename OutputIteratorT,
            typename Collection1T, 
            typename Collection2T,
            typename Collection3T>
        inline OutputIteratorT replace_all_copy(
            OutputIteratorT Output,
            const Collection1T& Input,
            const Collection2T& Search,
            const Collection3T& Format )
        {
            return find_format_all_copy(
                Output,
                Input,
                first_finder(Search),
                const_formatter(Format) );
        }

        //! Replace all algorithm
        /*!
            Substitute all occurrences of the search sequence in the input 
            with the format sequence. The result is a modified copy of the input.

            \param Input An input sequence
            \param Search A string to be searched for. 
            \param Format A substitute sequence
            \return A modified copy of the input
        */
        template<typename SequenceT, typename Collection1T, typename Collection2T>
        inline SequenceT replace_all_copy( 
            const SequenceT& Input,
            const Collection1T& Search,
            const Collection2T& Format )
        {
            return find_format_all_copy( 
                Input,
                first_finder(Search),
                const_formatter(Format) );
        }

        //! Replace all algorithm
        /*!
            Substitute all occurrences of the search sequence in the input 
            with the format sequence. The input sequence is modified in-place.

            \param Input An input sequence
            \param Search A string to be searched for. 
            \param Format A substitute sequence
            \return A reference to the modified input
        */
        template<typename SequenceT, typename Collection1T, typename Collection2T>
        inline void replace_all( 
            SequenceT& Input,
            const Collection1T& Search,
            const Collection2T& Format )
        {
            find_format_all( 
                Input, 
                first_finder(Search),
                const_formatter(Format) );
        }
        
//  replace_all ( case insensitive ) -----------------------------------------------//

        //! Replace all algorithm ( case insensitive )
        /*!
            Substitute all occurrences of the search sequence in the input 
            with the format sequence. The result is copied to the given output iterator.
            Searching is case insensitive.

            \param Output A output iterator to which the result will be copied
            \param Input An input sequence
            \param Search A string to be searched for. 
            \param Format A substitute sequence
            \param Loc a locale used for case insensitive comparison
            \return An output iterator pointing just after last inserted character
        */
        template<
            typename OutputIteratorT,
            typename Collection1T, 
            typename Collection2T,
            typename Collection3T>
        inline OutputIteratorT ireplace_all_copy(
            OutputIteratorT Output,
            const Collection1T& Input,
            const Collection2T& Search,
            const Collection3T& Format,
            const std::locale& Loc=std::locale() )
        {
            return find_format_all_copy(
                Output,
                Input,
                first_finder(Search, is_iequal(Loc)),
                const_formatter(Format) );
        }

        //! Replace all algorithm ( case insensitive )
        /*!
            Substitute all occurrences of the search sequence in the input 
            with the format sequence. The result is a modified copy of the input.
            Searching is case insensitive.

            \param Input An input sequence
            \param Search A string to be searched for. 
            \param Format A substitute sequence
            \param Loc a locale used for case insensitive comparison
            \return A modified copy of the input
        */
        template<typename SequenceT, typename Collection1T, typename Collection2T>
        inline SequenceT ireplace_all_copy( 
            const SequenceT& Input,
            const Collection1T& Search,
            const Collection2T& Format,
            const std::locale& Loc=std::locale() )
        {
            return find_format_all_copy( 
                Input,
                first_finder(Search, is_iequal(Loc)),
                const_formatter(Format) );
        }

        //! Replace all algorithm ( case insensitive )
        /*!
            Substitute all occurrences of the search sequence in the input 
            with the format sequence.The input sequence is modified in-place.
            Searching is case insensitive.

            \param Input An input sequence
            \param Search A string to be searched for. 
            \param Format A substitute sequence
            \param Loc a locale used for case insensitive comparison
        */
        template<typename SequenceT, typename Collection1T, typename Collection2T>
        inline void ireplace_all( 
            SequenceT& Input,
            const Collection1T& Search,
            const Collection2T& Format,
            const std::locale& Loc=std::locale() )
        {
            find_format_all( 
                Input, 
                first_finder(Search, is_iequal(Loc)),
                const_formatter(Format) );
        }
        
//  replace_head --------------------------------------------------------------------//

        //! Replace head algorithm
        /*!
            Replace the head of the input with the given format sequence. 
            Head is a prefix of a seqence of given size. 
            If the sequence is shorter then required, whole sequence if 
            considered to be the head. The result is copied to the given output iterator.

            \param Output A output iterator to which the result will be copied
            \param Input An input sequence
            \param N A length of the head
            \param Format A substitute sequence
            \return An output iterator pointing just after last inserted character
        */
        template<
            typename OutputIteratorT,
            typename Collection1T, 
            typename Collection2T>
        inline OutputIteratorT replace_head_copy(
            OutputIteratorT Output,
            const Collection1T& Input,
            unsigned int N,
            const Collection2T& Format )
        {
            return find_format_copy(
                Output,
                Input,
                head_finder(N),
                const_formatter(Format) );
        }

        //! Replace head algorithm
        /*!
            Replace the head of the input with the given format sequence. 
            Head is a prefix of a seqence of given size. 
            If the sequence is shorter then required, whole sequence if 
            considered to be the head. The result is a modified copy of the input.

            \param Input An input sequence
            \param N A length of the head
            \param Format A substitute sequence
            \return A modified copy of the input
        */
        template<typename SequenceT, typename CollectionT>
        inline SequenceT replace_head_copy( 
            const SequenceT& Input,
            unsigned int N,
            const CollectionT& Format )
        {
            return find_format_copy( 
                Input,
                head_finder(N),
                const_formatter(Format) );
        }

        //! Replace head algorithm
        /*!
            Replace the head of the input with the given format sequence. 
            Head is a prefix of a seqence of given size. 
            If the sequence is shorter then required, whole sequence if 
            considered to be the head. The input sequence is modified in-place.

            \param Input An input sequence
            \param N A length of the head
            \param Format A substitute sequence
        */
        template<typename SequenceT, typename CollectionT>
        inline void replace_head( 
            SequenceT& Input,
            unsigned int N,
            const CollectionT& Format )
        {
            find_format( 
                Input, 
                head_finder(N),
                const_formatter(Format) );
        }

//  replace_tail --------------------------------------------------------------------//

        //! Replace tail algorithm
        /*!
            Replace the tail of the input with the given format sequence. 
            Tail is a suffix of a seqence of given size. 
            If the sequence is shorter then required, whole sequence if 
            considered to be the tail. The result is copied to the given output iterator.

            \param Output A output iterator to which the result will be copied
            \param Input An input sequence
            \param N A length of the head
            \param Format A substitute sequence
            \return An output iterator pointing just after last inserted character
        */
        template<
            typename OutputIteratorT,
            typename Collection1T, 
            typename Collection2T>
        inline OutputIteratorT replace_tail_copy(
            OutputIteratorT Output,
            const Collection1T& Input,
            unsigned int N,
            const Collection2T& Format )
        {
            return find_format_copy(
                Output,
                Input,
                tail_finder(N),
                const_formatter(Format) );
        }

        //! Replace tail algorithm
        /*!
            Replace the tail of the input with the given format sequence. 
            Tail is a suffix of a seqence of given size. 
            If the sequence is shorter then required, whole sequence is 
            considered to be the tail. The result is a modified copy of the input.

            \param Input An input sequence
            \param N A length of the head
            \param Format A substitute sequence
            \return A modified copy of the input
        */
        template<typename SequenceT, typename CollectionT>
        inline SequenceT replace_tail_copy( 
            const SequenceT& Input,
            unsigned int N,
            const CollectionT& Format )
        {
            return find_format_copy( 
                Input,
                tail_finder(N),
                const_formatter(Format) );
        }

        //! Replace tail algorithm
        /*!
            Replace the tail of the input with the given format sequence. 
            Tail is a suffix of a seqence of given size. 
            If the sequence is shorter then required, whole sequence is 
            considered to be the tail. The input sequence is modified in-place.

            \param Input An input sequence
            \param N A length of the head
            \param Format A substitute sequence
        */
        template<typename SequenceT, typename CollectionT>
        inline void replace_tail( 
            SequenceT& Input,
            unsigned int N,
            const CollectionT& Format )
        {
            find_format( 
                Input, 
                tail_finder(N),
                const_formatter(Format) );
        }

    } // namespace algorithm

    // pull names to the boost namespace
    using algorithm::replace_range_copy;
    using algorithm::replace_range;
    using algorithm::replace_first_copy;
    using algorithm::replace_first;
    using algorithm::ireplace_first_copy;
    using algorithm::ireplace_first;
    using algorithm::replace_last_copy;
    using algorithm::replace_last;
    using algorithm::ireplace_last_copy;
    using algorithm::ireplace_last;
    using algorithm::replace_nth_copy;
    using algorithm::replace_nth;
    using algorithm::ireplace_nth_copy;
    using algorithm::ireplace_nth;
    using algorithm::replace_all_copy;
    using algorithm::replace_all;
    using algorithm::ireplace_all_copy;
    using algorithm::ireplace_all;
    using algorithm::replace_head_copy;
    using algorithm::replace_head;
    using algorithm::replace_tail_copy;
    using algorithm::replace_tail;

} // namespace boost

#endif  // BOOST_REPLACE_HPP
