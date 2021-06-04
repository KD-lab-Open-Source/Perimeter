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
  *   FILE:        regex_synch.cpp
  *   VERSION:     see <boost/version.hpp>
  *   DESCRIPTION: Thread synch helper functions, for regular
  *                expression library.
  */


#define BOOST_REGEX_SOURCE

#include <boost/regex/config.hpp>
#ifdef BOOST_REGEX_V3
#include <boost/regex/v3/regex_synch.hpp>
#else
#include <boost/regex/v4/regex_synch.hpp>
#endif

namespace boost{
   namespace re_detail{

void BOOST_REGEX_CALL re_init_threads()
{
   BOOST_RE_GUARD_STACK
#ifdef BOOST_HAS_THREADS
   if(p_re_lock == 0)
      p_re_lock = new critical_section();
   cs_guard g(*p_re_lock);
   ++re_lock_count;
#endif
}

void BOOST_REGEX_CALL re_free_threads()
{
   BOOST_RE_GUARD_STACK
#ifdef BOOST_HAS_THREADS
   cs_guard g(*p_re_lock);
   --re_lock_count;
   if(re_lock_count == 0)
   {
      g.acquire(false);
      delete p_re_lock;
      p_re_lock = 0;
   }
#endif
}

#ifdef BOOST_HAS_THREADS

BOOST_REGEX_DECL critical_section* p_re_lock = 0;

BOOST_REGEX_DECL unsigned int re_lock_count = 0;

#endif

   } // namespace re_detail
} // namespace boost






