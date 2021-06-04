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
  *   FILE:        regex.cpp
  *   VERSION:     see <boost/version.hpp>
  *   DESCRIPTION: Misc boost::regbase member funnctions.
  */


#define BOOST_REGEX_SOURCE

#include <new>
#include <boost/regex.hpp>
#include <boost/throw_exception.hpp>

#if defined(BOOST_REGEX_HAS_MS_STACK_GUARD) && defined(_MSC_VER) && (_MSC_VER >= 1300)
#  include <malloc.h>
#endif

#if defined(BOOST_REGEX_NON_RECURSIVE) && !defined(BOOST_REGEX_V3)
#if BOOST_REGEX_MAX_CACHE_BLOCKS == 0
#include <new>
#else
#include <boost/regex/v4/mem_block_cache.hpp>
#endif
#endif


namespace boost{

//
// fix: these are declared out of line here to ensure
// that dll builds contain the Virtual table for these
// types - this ensures that exceptions can be thrown
// from the dll and caught in an exe.
bad_pattern::~bad_pattern() throw() {}
bad_expression::~bad_expression() throw() {}

regbase::regbase()
   : _flags(regbase::failbit){}

regbase::regbase(const regbase& b)
   : _flags(b._flags){}


namespace re_detail{

//
// error checking API:
//
BOOST_REGEX_DECL void BOOST_REGEX_CALL verify_options(boost::regex::flag_type /*ef*/, match_flag_type mf)
{
#ifndef BOOST_REGEX_V3
   //
   // can't mix match_extra with POSIX matching rules:
   //
   if((mf & match_extra) && (mf & match_posix))
   {
      std::logic_error msg("Usage Error: Can't mix regular expression captures with POSIX matching rules");
      throw_exception(msg);
   }
#endif
}

#ifdef BOOST_REGEX_HAS_MS_STACK_GUARD

BOOST_REGEX_DECL void BOOST_REGEX_CALL reset_stack_guard_page()
{
#if defined(BOOST_REGEX_HAS_MS_STACK_GUARD) && defined(_MSC_VER) && (_MSC_VER >= 1300)
   _resetstkoflw();
#else
   //
   // We need to locate the current page being used by the stack,
   // move to the page below it and then deallocate and protect
   // that page.  Note that ideally we would protect only the lowest
   // stack page that has been allocated: in practice there
   // seems to be no easy way to locate this page, in any case as
   // long as the next page is protected, then Windows will figure
   // the rest out for us...
   //
   SYSTEM_INFO si;
   GetSystemInfo(&si);
   MEMORY_BASIC_INFORMATION mi;
   DWORD previous_protection_status;
   //
   // this is an address in our stack space:
   //
   LPBYTE page = (LPBYTE)&page;
   //
   // Get the current memory page in use:
   //
   VirtualQuery(page, &mi, sizeof(mi));
   //
   // Go to the page one below this:
   //
   page = (LPBYTE)(mi.BaseAddress)-si.dwPageSize;
   //
   // Free and protect everything from the start of the
   // allocation range, to the end of the page below the
   // one in use:
   //
   if (!VirtualFree(mi.AllocationBase, (LPBYTE)page - (LPBYTE)mi.AllocationBase, MEM_DECOMMIT)
      || !VirtualProtect(page, si.dwPageSize, PAGE_GUARD | PAGE_READWRITE, &previous_protection_status))
   {
      throw std::bad_exception();
   }
#endif
}
#endif

BOOST_REGEX_DECL void BOOST_REGEX_CALL raise_regex_exception(const std::string& msg)
{
   bad_expression e(msg);
   throw_exception(e);
}

#if defined(BOOST_REGEX_NON_RECURSIVE) && !defined(BOOST_REGEX_V3)

#if BOOST_REGEX_MAX_CACHE_BLOCKS == 0

BOOST_REGEX_DECL void* BOOST_REGEX_CALL get_mem_block()
{
   return ::operator new(BOOST_REGEX_BLOCKSIZE);
}

BOOST_REGEX_DECL void BOOST_REGEX_CALL put_mem_block(void* p)
{
   ::operator delete(p);
}

#else

mem_block_cache block_cache = { 0, 0, };

BOOST_REGEX_DECL void* BOOST_REGEX_CALL get_mem_block()
{
   return block_cache.get();
}

BOOST_REGEX_DECL void BOOST_REGEX_CALL put_mem_block(void* p)
{
   block_cache.put(p);
}

#endif

#endif

} // namespace re_detail



} // namespace boost

#if defined(BOOST_RE_USE_VCL) && defined(BOOST_REGEX_DYN_LINK)

int WINAPI DllEntryPoint(HINSTANCE , unsigned long , void*)
{
   return 1;
}
#endif

#if defined(__IBMCPP__) && defined(BOOST_REGEX_DYN_LINK)
//
// Is this correct - linker complains without it ?
//
int main()
{ 
   return 0; 
}

#endif






