//-----------------------------------------------------------------------------
// boost-libs variant/test/test4.cpp source file
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

#include "jobs.h"

#include <string>

struct class_a;

using boost::variant;

typedef variant<std::string, class_a, float> var_type_1;
typedef variant<std::string, class_a, short> var_type_2;

#include "class_a.h"

int test_main(int , char* [])
{
   using namespace boost;

   var_type_1 v1;
   var_type_2 v2;

   v1 = class_a();
   verify(v1, spec<class_a>(), "[V] class_a(5511)");

   verify(v2, spec<std::string>(), "[V] ");

   v2 = "abcde";
   verify(v2, spec<std::string>(), "[V] abcde");

   v2 = v1;
   verify(v2, spec<class_a>(), "[V] class_a(5511)");

   v2 = 5;
   v1 = v2;

   return boost::exit_success;
}
