//-----------------------------------------------------------------------------
// boost-libs variant/test/test6.cpp header file
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

#include "boost/test/minimal.hpp"
#include "boost/variant.hpp"

#include <iostream>

#include "jobs.h"


//Just Another Class
struct jac
{
   jac() { }
   jac(int ) { }
   jac(const char* ) { }
};

std::ostream& operator<<(std::ostream& out, const jac& )
{
   out << "jac ";
   return out;
}


void run()
{
   using boost::variant;

   variant<jac, int, double*, const double*> v1;   
   variant<int, char, double*, const double*, char*> v2;

   v1 = v2;

   verify(v1, spec<int>());
   verify(v2, spec<int>());

   verify_not(v1, spec<jac>());
   verify_not(v1, spec<double*>());
   verify_not(v1, spec<const double*>());

   verify_not(v2, spec<char>());
   verify_not(v2, spec<double*>());
   verify_not(v2, spec<const double*>());
   verify_not(v2, spec<char*>());


   variant<jac, const double*> v3;
   variant<int, unsigned char, double*> v4;

   v3 = v4;
   verify(v3, spec<jac>());
   verify(v4, spec<int>());
   verify_not(v4, spec<unsigned char>());
}



int test_main(int , char* [])
{
   run();
   return 0;
}

