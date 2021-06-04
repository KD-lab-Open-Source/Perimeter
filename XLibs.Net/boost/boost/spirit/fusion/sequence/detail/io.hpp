/*=============================================================================
    Copyright (c) 1999-2003 Jaakko Järvi
    Copyright (c) 1999-2003 Jeremiah Willcock
    Copyright (c) 2001-2003 Joel de Guzman

    Use, modification and distribution is subject to the Boost Software
    License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#if !defined(FUSION_SEQUENCE_DETAIL_IO_HPP)
#define FUSION_SEQUENCE_DETAIL_IO_HPP

#include <iostream>
#include <boost/spirit/fusion/sequence/detail/manip.hpp>

#include <boost/mpl/bool.hpp>
#include <boost/spirit/fusion/sequence/begin.hpp>
#include <boost/spirit/fusion/sequence/end.hpp>
#include <boost/spirit/fusion/iterator/deref.hpp>
#include <boost/spirit/fusion/iterator/next.hpp>
#include <boost/spirit/fusion/iterator/equal_to.hpp>

namespace boost { namespace fusion { namespace detail
{
    template <typename Tag>
    struct delimiter_io
    {
        // print a delimiter
        template <typename OS>
        static void
        print(OS& os, char const* delim, mpl::false_ = mpl::false_())
        {
            detail::string_ios_manip<Tag, OS> manip(os);
            manip.print(delim);
        }

        template <typename OS>
        static void
        print(OS& os, char const* delim, mpl::true_)
        {
        }

        // read a delimiter
        template <typename IS>
        static void
        read(IS& is, char const* delim, mpl::false_ = mpl::false_())
        {
            detail::string_ios_manip<Tag, IS> manip(is);
            manip.read(delim);
        }

        template <typename IS>
        static void
        read(IS& is, char const* delim, mpl::true_)
        {
        }
    };

    struct print_sequence_loop
    {
        template <typename OS, typename First, typename Last>
        static void
        apply(OS& os, First const&, Last const&, mpl::true_)
        {
        }

        template <typename OS, typename First, typename Last>
        static void
        apply(OS& os, First const& first, Last const& last, mpl::false_)
        {
            equal_to<
                BOOST_DEDUCED_TYPENAME result_of_next<First>::type
              , Last
            >
            is_last;

            os << *first;
            delimiter_io<tuple_delimiter_tag>::print(os, " ", is_last);
            apply(os, fusion::next(first), last, is_last);
        }

        template <typename OS, typename First, typename Last>
        static void
        apply(OS& os, First const& first, Last const& last)
        {
            equal_to<First, Last> eq;
            apply(os, first, last, eq);
        }
    };

    struct read_sequence_loop
    {
        template <typename IS, typename First, typename Last>
        static void
        apply(IS& is, First const&, Last const&, mpl::true_)
        {
        }

        template <typename IS, typename First, typename Last>
        static void
        apply(IS& is, First const& first, Last const& last, mpl::false_)
        {
          	equal_to<
                BOOST_DEDUCED_TYPENAME result_of_next<First>::type
              , Last
            >
            is_last;

            is >> *first;
            delimiter_io<tuple_delimiter_tag>::read(is, " ", is_last);
            apply(is, fusion::next(first), last, is_last);
        }

        template <typename IS, typename First, typename Last>
        static void
        apply(IS& is, First const& first, Last const& last)
        {
            equal_to<First, Last> eq;
            apply(is, first, last, eq);
        }
    };

    template <typename OS, typename Sequence>
    inline void
    print_sequence(OS& os, Sequence const& seq)
    {
        delimiter_io<tuple_open_tag>::print(os, "(");
        print_sequence_loop::apply(os, fusion::begin(seq), fusion::end(seq));
        delimiter_io<tuple_close_tag>::print(os, ")");
    }

    template <typename IS, typename Sequence>
    inline void
    read_sequence(IS& is, Sequence& seq)
    {
        delimiter_io<tuple_open_tag>::read(is, "(");
        read_sequence_loop::apply(is, fusion::begin(seq), fusion::end(seq));
        delimiter_io<tuple_close_tag>::read(is, ")");
    }
}}}

#endif
