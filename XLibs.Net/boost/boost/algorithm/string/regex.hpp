//  Boost string_algo library regex.hpp header file  ---------------------------//

//  Copyright Pavol Droba 2002-2003. Use, modification and
//  distribution is subject to the Boost Software License, Version
//  1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for updates, documentation, and revision history.

#ifndef BOOST_STRING_REGEX_HPP
#define BOOST_STRING_REGEX_HPP

#include <boost/algorithm/string/config.hpp>
#include <boost/regex.hpp>
#include <boost/algorithm/string/collection_traits.hpp>
#include <boost/algorithm/string/iterator_range.hpp>
#include <boost/algorithm/string/find_format.hpp>
#include <boost/algorithm/string/regex_find_format.hpp>
#include <boost/algorithm/string/formatter.hpp>
#include <boost/algorithm/string/iter_find.hpp>

/*! \file
    Defines regex variant of the algorithms. 
*/

namespace boost {
    namespace algorithm {

//  find_regex  -----------------------------------------------//

        //! Find regex algorithm
        /*!
            Search for a subsequence matching the given regex in the input.
            
            \param Input A container which will be searched.
            \param Rx A regular expression
            \param Flags Regex options
            \return 
                An \c iterator_range delimiting the match. 
                Returned iterator is either \c InputContainerT::iterator or 
                \c InputContainerT::const_iterator, depending on the constness of 
                the input parameter.
        */
        template< 
            typename CollectionT, 
            typename CharT, 
            typename RegexTraitsT, typename RegexAllocatorT>
        inline iterator_range< 
            BOOST_STRING_TYPENAME result_iterator_of<CollectionT>::type >
        find_regex( 
            CollectionT& Input, 
            const reg_expression<CharT, RegexTraitsT, RegexAllocatorT>& Rx,
            match_flag_type Flags=match_default )
        {
            return regex_finder(Rx,Flags)(
                begin(Input), end(Input) );
        }

//  replace_regex --------------------------------------------------------------------//

        //! Replace regex algorithm
        /*!
            Search for a subsequence matching given regex and format it with 
            the specified format. The result is copied to the given output iterator.

            \param Output A output iterator to which the result will be copied
            \param Input An input sequence
            \param Rx A regular expression
            \param Format Regex format definition
            \param Flags Regex options
            \return An output iterator pointing just after last inserted character
        */
        template< 
            typename OutputIteratorT,
            typename CollectionT, 
            typename CharT, 
            typename RegexTraitsT, typename RegexAllocatorT,
            typename FormatStringTraitsT, typename FormatStringAllocatorT >
        inline OutputIteratorT replace_regex_copy(
            OutputIteratorT Output,
            const CollectionT& Input,
            const reg_expression<CharT, RegexTraitsT, RegexAllocatorT>& Rx,
            const std::basic_string<CharT, FormatStringTraitsT, FormatStringAllocatorT>& Format,
            match_flag_type Flags=match_default | format_default )
        {
            return find_format_copy( 
                Output,
                Input,
                regex_finder( Rx, Flags ),
                regex_formatter( Format, Flags ) );
        }

        //! Replace regex algorithm
        /*!
            Search for a subsequence matching given regex and format it with 
            the specified format. The result is a modified copy of the input.

            \param Input An input sequence
            \param Rx A regular expression
            \param Format Regex format definition
            \param Flags Regex options
            \return A modified copy of the input
        */
        template< 
            typename SequenceT, 
            typename CharT, 
            typename RegexTraitsT, typename RegexAllocatorT,
            typename FormatStringTraitsT, typename FormatStringAllocatorT >
        inline SequenceT replace_regex_copy( 
            const SequenceT& Input,
            const reg_expression<CharT, RegexTraitsT, RegexAllocatorT>& Rx,
            const std::basic_string<CharT, FormatStringTraitsT, FormatStringAllocatorT>& Format,
            match_flag_type Flags=match_default | format_default )
        {
            return find_format_copy( 
                Input,
                regex_finder( Rx, Flags ),
                regex_formatter( Format, Flags ) );
        }

        //! Replace regex algorithm
        /*!
            Search for a subsequence matching given regex and format it with 
            the specified format. The input sequence is modified in-place.

            \param Input An input sequence
            \param Rx A regular expression
            \param Format Regex format definition
            \param Flags Regex options
        */
        template< 
            typename SequenceT, 
            typename CharT, 
            typename RegexTraitsT, typename RegexAllocatorT,
            typename FormatStringTraitsT, typename FormatStringAllocatorT >
        inline void replace_regex( 
            SequenceT& Input,
            const reg_expression<CharT, RegexTraitsT, RegexAllocatorT>& Rx,
            const std::basic_string<CharT, FormatStringTraitsT, FormatStringAllocatorT>& Format,
            match_flag_type Flags=match_default | format_default )
        {
            find_format( 
                Input,
                regex_finder( Rx, Flags ),
                regex_formatter( Format, Flags ) );
        }

//  replace_all_regex --------------------------------------------------------------------//

        //! Replace all regex algorithm
        /*!
            Format all subsequences, matching given regex, with the specified format. 
            The result is copied to the given output iterator.

            \param Output A output iterator to which the result will be copied
            \param Input An input sequence
            \param Rx A regular expression
            \param Format Regex format definition
            \param Flags Regex options
            \return An output iterator pointing just after last inserted character
        */
        template< 
            typename OutputIteratorT,
            typename CollectionT, 
            typename CharT, 
            typename RegexTraitsT, typename RegexAllocatorT,
            typename FormatStringTraitsT, typename FormatStringAllocatorT >
        inline OutputIteratorT replace_all_regex_copy(
            OutputIteratorT Output,
            const CollectionT& Input,
            const reg_expression<CharT, RegexTraitsT, RegexAllocatorT>& Rx,
            const std::basic_string<CharT, FormatStringTraitsT, FormatStringAllocatorT>& Format,
            match_flag_type Flags=match_default | format_default )
        {
            return find_format_all_copy( 
                Output,
                Input,
                regex_finder( Rx, Flags ),
                regex_formatter( Format, Flags ) );
        }

        //! Replace all regex algorithm
        /*!
            Format all subsequences, mathing given regex, with the specified format. 
            The result is a modified copy of the input.

            \param Input An input sequence
            \param Rx A regular expression
            \param Format Regex format definition
            \param Flags Regex options
            \return A modified copy of the input
        */
        template< 
            typename SequenceT, 
            typename CharT, 
            typename RegexTraitsT, typename RegexAllocatorT,
            typename FormatStringTraitsT, typename FormatStringAllocatorT >
        inline SequenceT replace_all_regex_copy( 
            const SequenceT& Input,
            const reg_expression<CharT, RegexTraitsT, RegexAllocatorT>& Rx,
            const std::basic_string<CharT, FormatStringTraitsT, FormatStringAllocatorT>& Format,
            match_flag_type Flags=match_default | format_default )
        {
            return find_format_all_copy( 
                Input,
                regex_finder( Rx, Flags ),
                regex_formatter( Format, Flags ) );
        }

        //! Replace all regex algorithm
        /*!
            Format all subsequences, matching given regex, with the specified format. 
            The input sequence is modified in-place.

            \param Input An input sequence
            \param Rx A regular expression
            \param Format Regex format definition
            \param Flags Regex options
        */
        template< 
            typename SequenceT, 
            typename CharT, 
            typename RegexTraitsT, typename RegexAllocatorT,
            typename FormatStringTraitsT, typename FormatStringAllocatorT >
        inline void replace_all_regex( 
            SequenceT& Input,
            const reg_expression<CharT, RegexTraitsT, RegexAllocatorT>& Rx,
            const std::basic_string<CharT, FormatStringTraitsT, FormatStringAllocatorT>& Format,
            match_flag_type Flags=match_default | format_default )
        {
            find_format_all( 
                Input,
                regex_finder( Rx, Flags ),
                regex_formatter( Format, Flags ) );
        }

//  erase_regex --------------------------------------------------------------------//

        //! Erase regex algorithm
        /*!
            Remove a subsequence matching given regex from the input.
            The result is copied to the given output iterator.

            \param Output A output iterator to which the result will be copied
            \param Input An input sequence
            \param Rx A regular expression
            \param Flags Regex options
            \return An output iterator pointing just after last inserted character
        */
        template< 
            typename OutputIteratorT,
            typename CollectionT, 
            typename CharT, 
            typename RegexTraitsT, typename RegexAllocatorT >
        inline OutputIteratorT erase_regex_copy(
            OutputIteratorT Output,
            const CollectionT& Input,
            const reg_expression<CharT, RegexTraitsT, RegexAllocatorT>& Rx,
            match_flag_type Flags=match_default )
        {
            return find_format_copy(
                Output,
                Input,
                regex_finder( Rx, Flags ),
                empty_formatter( Input ) );
        }

        //! Erase regex algorithm
        /*!
            Remove a subsequence matching given regex from the input.
            The result is a modified copy of the input.

            \param Input An input sequence
            \param Rx A regular expression
            \param Flags Regex options
            \return A modified copy of the input
        */
        template< 
            typename SequenceT, 
            typename CharT, 
            typename RegexTraitsT, typename RegexAllocatorT >
        inline SequenceT erase_regex_copy( 
            const SequenceT& Input,
            const reg_expression<CharT, RegexTraitsT, RegexAllocatorT>& Rx,
            match_flag_type Flags=match_default )
        {
            return find_format_copy( 
                Input, 
                regex_finder( Rx, Flags ),
                empty_formatter( Input ) );
        }

        //! Erase regex algorithm
        /*!
            Remove a subsequence matching given regex from the input.
            The input sequence is modified in-place.

            \param Input An input sequence
            \param Rx A regular expression
            \param Flags Regex options
        */
        template< 
            typename SequenceT, 
            typename CharT, 
            typename RegexTraitsT, typename RegexAllocatorT >
        inline void erase_regex( 
            SequenceT& Input,
            const reg_expression<CharT, RegexTraitsT, RegexAllocatorT>& Rx,
            match_flag_type Flags=match_default )
        {
            find_format( 
                Input, 
                regex_finder( Rx, Flags ),
                empty_formatter( Input ) );
        }

//  erase_all_regex --------------------------------------------------------------------//

        //! Erase all regex algorithm
        /*!
            Erase all subsequences, matching given regex, from the input.
            The result is copied to the given output iterator.

            \param Output A output iterator to which the result will be copied
            \param Input An input sequence
            \param Rx A regular expression
            \param Flags Regex options
            \return An output iterator pointing just after last inserted character
        */
        template< 
            typename OutputIteratorT,
            typename CollectionT, 
            typename CharT, 
            typename RegexTraitsT, typename RegexAllocatorT >
        inline OutputIteratorT erase_all_regex_copy(
            OutputIteratorT Output,
            const CollectionT& Input,
            const reg_expression<CharT, RegexTraitsT, RegexAllocatorT>& Rx,
            match_flag_type Flags=match_default )
        {
            return find_format_all_copy(
                Output,
                Input,
                regex_finder( Rx, Flags ),
                empty_formatter( Input ) );
        }

        //! Erase all regex algorithm
        /*!
            Erase all subsequences, matching given regex, from the input.
            The result is a modified copy of the input.

            \param Input An input sequence
            \param Rx A regular expression
            \param Flags Regex options
            \return A modified copy of the input
        */
        template< 
            typename SequenceT, 
            typename CharT, 
            typename RegexTraitsT, typename RegexAllocatorT >
        inline SequenceT erase_all_regex_copy( 
            const SequenceT& Input,
            const reg_expression<CharT, RegexTraitsT, RegexAllocatorT>& Rx,
            match_flag_type Flags=match_default )
        {
            return find_format_all_copy( 
                Input, 
                regex_finder( Rx, Flags ),
                empty_formatter( Input ) );
        }

        //! Erase all regex algorithm
        /*!
            Erase all subsequences, matching given regex, from the input.
            The input sequence is modified in-place.

            \param Input An input sequence
            \param Rx A regular expression
            \param Flags Regex options
        */
        template< 
            typename SequenceT, 
            typename CharT, 
            typename RegexTraitsT, typename RegexAllocatorT>
        inline void erase_all_regex( 
            SequenceT& Input,
            const reg_expression<CharT, RegexTraitsT, RegexAllocatorT>& Rx,
            match_flag_type Flags=match_default )
        {
            find_format_all( 
                Input, 
                regex_finder( Rx, Flags ),
                empty_formatter( Input ) );
        }

//  find_all_regex ------------------------------------------------------------------//

        //! Find all regex algorithm
        /*!
            This algorithm finds all substrings matching the give regex
            in the input. The result is given as a 'container of containers'.
            Each match of the search sequence is represented by one
            element in the result.

            \param Result A 'container container' to container the result of search.
                Both outer and inner container must have constructor taking a pair
                of iterators as an argument.
                Typical type of the result is 
                    \c std::vector<boost::iterator_range<iterator>>.
                (each element of such a vector will container a range delimiting 
                a match).
            \param Input A container which will be searched.
            \param Rx A regular expression
            \param Flags Regex options
            \return A reference to the result

            \note Prior content of the result will be overridden.
        */
        template< 
            typename SequenceSequenceT, 
            typename CollectionT,         
            typename CharT, 
            typename RegexTraitsT, typename RegexAllocatorT >
        inline SequenceSequenceT& find_all_regex(
            SequenceSequenceT& Result,
            const CollectionT& Input,
            const reg_expression<CharT, RegexTraitsT, RegexAllocatorT>& Rx,
            match_flag_type Flags=match_default )
        {
            return iter_find(
                Result,
                Input,
                regex_finder(Rx,Flags) );         
        }

//  split_regex ------------------------------------------------------------------//

        //! Split regex algorithm
        /*! 
            Tokenize expression. This function is equivalent of C strtok. Input
            sequence is split into tokens, separated  by separators. Separator
            is an every match of the given regex.

            \param Result A 'container container' to container the result of search.
                Both outer and inner container must have constructor taking a pair
                of iterators as an argument.
                Typical type of the result is 
                    'c std::vector<boost::iterator_range<iterator>>.
                (each element of such a vector will container a range delimiting 
                a match).
            \param Input A container which will be searched.
            \param Rx A regular expression
            \param Flags Regex options
            \return A reference to the result

            \note Prior content of the result will be overridden.
        */
        template< 
            typename SequenceSequenceT, 
            typename CollectionT,         
            typename CharT, 
            typename RegexTraitsT, typename RegexAllocatorT >
        inline SequenceSequenceT& split_regex(
            SequenceSequenceT& Result,
            const CollectionT& Input,
            const reg_expression<CharT, RegexTraitsT, RegexAllocatorT>& Rx,
            match_flag_type Flags=match_default )
        {
            return iter_split(
                Result,
                Input,
                regex_finder(Rx,Flags) );         
        }

    } // namespace algorithm

    // pull names into the boost namespace
    using algorithm::find_regex;
    using algorithm::replace_regex;
    using algorithm::replace_regex_copy;
    using algorithm::replace_all_regex;
    using algorithm::replace_all_regex_copy;
    using algorithm::erase_regex;
    using algorithm::erase_regex_copy;
    using algorithm::erase_all_regex;
    using algorithm::erase_all_regex_copy;
    using algorithm::find_all_regex;
    using algorithm::split_regex;

} // namespace boost


#endif  // BOOST_STRING_REGEX_HPP
