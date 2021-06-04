//  bll_and_function.cpp  - The Boost Lambda Library -----------------------
//
// Copyright (C) 2000-2003 Jaakko Jдrvi (jaakko.jarvi@cs.utu.fi)
// Copyright (C) 2000-2003 Gary Powell (powellg@amazon.com)
//
// Permission to copy, use, sell and distribute this software is granted
// provided this copyright notice appears in all copies. 
// Permission to modify the code and to distribute modified code is granted
// provided this copyright notice appears in all copies, and a notice 
// that the code was modified is included with the copyright notice.
//
// This software is provided "as is" without express or implied warranty, 
// and with no claim as to its suitability for any purpose.
//
// For more information, see www.boost.org

// test using BLL and boost::function

//#include <boost/test/minimal.hpp>    // see "Header Implementation Option"
#include <boost/test/test_tools.hpp>

#include "boost/lambda/lambda.hpp"
#include "boost/lambda/bind.hpp"
#include "boost/lambda/algorithm.hpp"

#include <vector>
#include <map>
#include <set>
#include <string>

#include <iostream>



void test_foreach() {
  using namespace boost::lambda;
  
  int a[10][20];
  int sum = 0;

  std::for_each(a, a + 10, 
                bind(ll::for_each(), _1, _1 + 20, 
                     protect((_1 = var(sum), ++var(sum)))));

  sum = 0;
  std::for_each(a, a + 10, 
                bind(ll::for_each(), _1, _1 + 20, 
                     protect((sum += _1))));

  BOOST_TEST(sum == (199 + 1)/ 2 * 199);
}

// More tests needed (for all algorithms)

int test_main(int, char *[]) {

  test_foreach();

  return 0;
}






