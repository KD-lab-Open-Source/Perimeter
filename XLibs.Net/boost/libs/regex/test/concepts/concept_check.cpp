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


#include <cstdlib> // for abort
#include <boost/regex.hpp>

#if defined(BOOST_MSVC)
// this lets us compile at warning level 4 without seeing concept-check related warnings
#  pragma warning(disable:4100)
#endif

#include <boost/concept_archetype.hpp>
#include <boost/concept_check.hpp>
#include <boost/detail/workaround.hpp>

#ifdef BOOST_NO_STDC_NAMESPACE
namespace std{ using ::abort; }
#endif


int main()
{
   // VC6 and VC7 can't cope with the iterator architypes, 
   // don't bother testing as it doesn't work:
#if !BOOST_WORKAROUND(_MSC_VER, < 1310)
   typedef boost::bidirectional_iterator_archetype<char> iterator_type;
   typedef boost::input_iterator_archetype<char> input_iterator_type;
   input_iterator_type i, j;
#if!defined(BOOST_NO_MEMBER_TEMPLATES) && !defined(__IBMCPP__) && !BOOST_WORKAROUND(__GNUC__, < 3)
   boost::regex r(i, j);
   r.assign(i, j);
#else
   boost::regex r;
#endif
   iterator_type a, b;
   boost::detail::dummy_constructor dummy;
   boost::output_iterator_archetype<char> out(dummy);
   std::string s;
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
#endif
   //
   // verify basic_regex member functions:
   //
   const char* c_exp = "abc";
   r = c_exp;
   r = r;
   r = s;
   if((r.mark_count()) || (0 == r.max_size()) || (r.empty()) || (0 == r.size()) || (r.begin() == r.end())) std::abort();

   r.assign(r);
   r.assign(c_exp);
   r.assign(c_exp, boost::regex::perl);
   r.assign(c_exp, 1, boost::regex::perl);
   r.assign(s);
   r.assign(s, boost::regex::perl);
   r.assign(c_exp, c_exp+1);
   r.assign(c_exp, c_exp+1, boost::regex::perl);
#endif 

#ifndef BOOST_NO_STD_ITERATOR
   //
   //check iterators work with std lib algorithms:
   //
   boost::cregex_iterator ri, rj;
   std::distance(ri, rj);
   std::advance(ri, 0);
   boost::cregex_token_iterator rk, rm;
   std::distance(rk, rm);
   std::advance(rk, 0);
#endif
   return 0;
}


