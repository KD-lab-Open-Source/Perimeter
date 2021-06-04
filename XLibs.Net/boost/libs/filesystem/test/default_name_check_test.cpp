//  default_name_check_test program  ------------------------------------------//

//  Copyright Beman Dawes 2003.
//  Use, modification, and distribution is subject to the Boost Software
//  License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

//  See library home page at http://www.boost.org/libs/filesystem

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/exception.hpp>

namespace fs = boost::filesystem;
using boost::filesystem::path;

#include <boost/test/minimal.hpp>

int test_main( int, char*[] )
{
  BOOST_TEST( path::default_name_check_writable() );
  path::default_name_check( fs::no_check );
  BOOST_TEST( !path::default_name_check_writable() );
  BOOST_TEST( path::default_name_check() == fs::no_check );
  return 0;
}

