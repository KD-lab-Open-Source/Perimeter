//-----------------------------------------------------------------------------
// boost-libs variant/test/class_a.cpp source file
// See http://www.boost.org for updates, documentation, and revision history.
//-----------------------------------------------------------------------------
//
// Copyright (c) 2003
// Eric Friedman, Itay Maman
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee, 
// provided that the above copyright notice appears in all copies and 
// that both the copyright notice and this permission notice appear in 
// supporting documentation. No representations are made about the 
// suitability of this software for any purpose. It is provided "as is" 
// without express or implied warranty.

#include <algorithm> // for std::swap
#include <sstream>
#include <iostream>
#include <assert.h>

#include "class_a.h"


using namespace std;

class_a::~class_a()
{
   assert(self_p_ == this);
}

class_a::class_a(int n)
{
   n_ = n;
   self_p_ = this;
}

class_a::class_a(const class_a& other)
{
   n_ = other.n_;
   self_p_ = this;
}


class_a& class_a::operator=(const class_a& rhs)
{
   class_a temp(rhs);
   swap(temp);

   return *this;
}

void class_a::swap(class_a& other)
{
   std::swap(n_, other.n_);
}

int class_a::get() const
{
   return n_;
}




std::ostream& operator<<(std::ostream& strm, const class_a& a)
{
   return strm << "class_a(" << a.get() << ")";
}
