/*
 *
 * Copyright (c) 1998-2002
 * Dr John Maddock
 *
 * Use, modification and distribution are subject to the 
 * Boost Software License, Version 1.0. (See accompanying file 
 * LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 *
 */
 
 /*
  *   LOCATION:    see http://www.boost.org for most recent version.
  *   FILE         regex_test.cpp
  *   VERSION      see <boost/version.hpp>
  *   DESCRIPTION: Builds regression test program with default
  *                locale and narrow character tests.  Also
  *                instantiates all the templates in the library
  *                as a sanity check.
  */

// disable automatic selection of support library:
#define BOOST_REGEX_NO_LIB
#define BOOST_REGEX_STATIC_LINK
#define BOOST_REGEX_NO_EXTERNAL_TEMPLATES

#include <boost/regex.hpp>
#include <boost/regex/src.cpp>

#ifdef BOOST_MSVC
#  pragma warning(disable: 4660)
#endif
//
// instantiate templates used:
//
namespace boost{

#if defined(__GNUC__) && defined(__BEOS__)
#define ra_it const char*
#define test_char_type char
#else
typedef const char* ra_it;
typedef char test_char_type;
#endif
typedef std::basic_string<test_char_type> test_string_type;

bool pred1(const match_results<ra_it>&)
{ return true; }

typedef bool (*pred1_type)(const match_results<ra_it>&);
typedef bool (*pred2_type)(const match_results<test_string_type::const_iterator>&);

//check that all the defined flags are available:
regex::flag_type f = regex::escape_in_lists | regex::char_classes | regex::intervals | regex::limited_ops
                       | regex::newline_alt | regex::bk_plus_qm | regex::bk_braces
                       | regex::bk_parens | regex::bk_refs | regex::bk_vbar | regex::use_except
                       | regex::failbit | regex::literal | regex::icase | regex::nocollate | regex::basic
                       | regex::extended | regex::normal | regex::emacs | regex::awk | regex::grep | regex::egrep | regex::sed;

template class reg_expression<test_char_type>;
template struct sub_match<ra_it>;
template class match_results<ra_it>;
#if 0
template bool regex_match(ra_it,
                 ra_it,
                 match_results<ra_it>& m,
                 const reg_expression<test_char_type>& e,
                 unsigned flags);
template bool regex_match(ra_it,
                 ra_it,
                 const reg_expression<test_char_type>& e,
                 unsigned flags);
template bool regex_search(ra_it,
                ra_it,
                match_results<ra_it>& m,
                const reg_expression<test_char_type>& e,
                unsigned flags);
template unsigned int regex_grep(pred1_type,
                        ra_it,
                        ra_it,
                        const reg_expression<test_char_type>& e,
                        unsigned flags);
template test_char_type* regex_format(test_char_type*,
                            const match_results<ra_it>& m,
                            const test_char_type* fmt,
                            unsigned flags);
template test_char_type* regex_format(test_char_type*,
                            const match_results<ra_it>& m,
                            const test_string_type& fmt,
                            unsigned flags);
template test_char_type* regex_replace(test_char_type*,
                          ra_it,
                          ra_it,
                          const reg_expression<test_char_type>&,
                          const test_char_type*,
                          unsigned int flags);
template test_char_type* regex_replace(test_char_type*,
                           ra_it,
                           ra_it,
                           const reg_expression<test_char_type>& e,
                           const test_string_type&,
                           unsigned int flags);
template std::size_t regex_split(test_string_type*,
                        test_string_type&,
                        const reg_expression<test_char_type>&,
                        unsigned flags,
                        std::size_t);
template std::size_t regex_split(test_string_type*,
                        test_string_type& s,
                        const reg_expression<test_char_type>& e,
                        unsigned flags);

template std::size_t regex_split(test_string_type*, test_string_type&);

#ifndef BOOST_NO_FUNCTION_TEMPLATE_ORDERING
//
// the following prototypes are only available if partial ordering
// of template functions is supported:
//
template bool regex_match(const test_char_type*,
                 match_results<const test_char_type*>& m,
                 const reg_expression<test_char_type>& e,
                 unsigned flags);
template bool regex_match(const test_string_type&,
                 match_results<test_string_type::const_iterator>&,
                 const reg_expression<test_char_type>&,
                 unsigned flags);
template bool regex_match(const test_char_type*,
                 const reg_expression<test_char_type>&,
                 unsigned flags);
template bool regex_match(const test_string_type&,
                 const reg_expression<test_char_type>&,
                 unsigned flags);
template bool regex_search(const test_char_type*,
                match_results<const test_char_type*>&,
                const reg_expression<test_char_type>&,
                unsigned flags);
template bool regex_search(const test_string_type&,
                match_results<test_string_type::const_iterator>&,
                const reg_expression<test_char_type>&,
                unsigned flags);
template unsigned int regex_grep(pred1_type,
              const test_char_type*,
              const reg_expression<test_char_type>&,
              unsigned flags);
template unsigned int regex_grep(pred2_type,
              const test_string_type&,
              const reg_expression<test_char_type>&,
              unsigned flags);
template test_string_type regex_format
                                 (const match_results<test_string_type::const_iterator>& m,
                                  const test_char_type*,
                                  unsigned flags);
template test_string_type regex_format
                                 (const match_results<test_string_type::const_iterator>&,
                                  const test_string_type&,
                                  unsigned flags);
template test_string_type regex_replace(const test_string_type&,
                                     const reg_expression<test_char_type>&,
                                     const test_char_type*,
                                     unsigned int flags);
template test_string_type regex_replace(const test_string_type&,
                                     const reg_expression<test_char_type>&,
                                     const test_string_type&,
                                     unsigned int flags);

#endif
#endif
} // namespace boost

//
// include regression test source files:
//
#ifdef BOOST_REGEX_USE_WIN32_LOCALE
#define BOOST_RE_TEST_LOCALE_W32
#elif !defined(BOOST_REGEX_USE_C_LOCALE)
#define BOOST_RE_TEST_LOCALE_CPP
#endif

#include "tests.cpp"
#include "parse.cpp"
#include "regress.cpp"

//
// Como goes into an infinite loop trying to link this,
// just have it fail for now:
//
#if defined(__COMO__) && defined(_MSC_VER)
#error "Comeau in VC6 mode goes into an infinite loop trying to link this program!!!"
#endif




