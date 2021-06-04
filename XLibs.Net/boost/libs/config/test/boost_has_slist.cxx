//  (C) Copyright John Maddock 2001. 
//  Use, modification and distribution are subject to the 
//  Boost Software License, Version 1.0. (See accompanying file 
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org/libs/config for most recent version.

//  MACRO:         BOOST_HAS_SLIST
//  TITLE:         <slist>
//  DESCRIPTION:   The C++ implementation provides the (SGI) slist class.

#include <slist>

#ifndef BOOST_STD_EXTENSION_NAMESPACE
#define BOOST_STD_EXTENSION_NAMESPACE std
#endif

namespace boost_has_slist{

template <class T, class Alloc>
void foo(const BOOST_STD_EXTENSION_NAMESPACE::slist<T,Alloc>& )
{
}


int test()
{
   BOOST_STD_EXTENSION_NAMESPACE::slist<int> l;
   foo(l);
   return 0;
}

}






