/*
 *
 * Copyright (c) 2003
 * Dr John Maddock
 *
 * Use, modification and distribution are subject to the 
 * Boost Software License, Version 1.0. (See accompanying file 
 * LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 *
 */

#include <boost/regex.hpp>
#include <boost/concept_archetype.hpp>
#if defined(BOOST_MSVC)
// this lets us compile at warning level 4 without seeing concept-check related warnings
#  pragma warning(disable:4100)
#endif
#include <boost/concept_check.hpp>
#include <boost/detail/workaround.hpp>


int main()
{
#ifndef BOOST_NO_WREGEX
   // VC6 and VC7 can't cope with the iterator architypes, 
   // don't bother testing as it doesn't work:
#if !BOOST_WORKAROUND(_MSC_VER, < 1310)
   typedef boost::bidirectional_iterator_archetype<wchar_t> iterator_type;
   typedef boost::input_iterator_archetype<wchar_t> input_iterator_type;
   input_iterator_type i, j;
#if!defined(BOOST_NO_MEMBER_TEMPLATES) && !defined(__IBMCPP__)
   boost::wregex r(i, j);
   r.assign(i, j);
#else
   boost::wregex r;
#endif
   iterator_type a, b;
   boost::detail::dummy_constructor dummy;
   boost::output_iterator_archetype<wchar_t> out(dummy);
   std::wstring s;
   boost::match_results<iterator_type> what;

   boost::regex_match(a, b, r);
   boost::regex_match(a, b, what, r);
   boost::regex_search(a, b, r);
   boost::regex_search(a, b, what, r);
   out = boost::regex_replace(out, a, b, r, s, boost::match_default);
   s = boost::regex_replace(s, r, s, boost::match_default);

   out = what.format(out, s, boost::format_default);
   s = what.format(s, boost::format_default);

   boost::function_requires<
      boost::ForwardIteratorConcept<
         boost::regex_iterator<iterator_type>
      >
   >();
   // this fails with glibc++v2 :
#if !BOOST_WORKAROUND(__GNUC__, < 3) && !BOOST_WORKAROUND(BOOST_MSVC, <1300)
   boost::function_requires<
      boost::ForwardIteratorConcept<
         boost::regex_token_iterator<iterator_type>
      >
   >();
#endif // GCC 2.x
#endif // MSVC 6
#endif // BOOST_NO_WREGEX
   return 0;
}


