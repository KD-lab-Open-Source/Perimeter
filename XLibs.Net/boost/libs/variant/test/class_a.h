//-----------------------------------------------------------------------------
// boost-libs variant/libs/test/class_a.h header file
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

#ifndef _CLASSA_H_INC_
#define _CLASSA_H_INC_


#include <iosfwd>

struct class_a 
{
   ~class_a();
   class_a(int n = 5511);
   class_a(const class_a& other);

   class_a& operator=(const class_a& rhs);
   void swap(class_a& other);

   int get() const;

private:
   int n_;
   class_a* self_p_;

}; //Class_a

std::ostream& operator<<(std::ostream& strm, const class_a& a);

 

#endif //_CLASSA_H_INC_
