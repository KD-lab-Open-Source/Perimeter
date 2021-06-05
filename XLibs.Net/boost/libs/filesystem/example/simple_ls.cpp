//  simple_ls program  -------------------------------------------------------//

//  © Copyright Jeff Garland and Beman Dawes, 2002

//  Use, modification, and distribution is subject to the Boost Software
//  License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org/libs/filesystem for documentation.

#include "boost/filesystem/operations.hpp"
#include "boost/filesystem/path.hpp"
#include <iostream>

namespace fs = boost::filesystem;

int main( int argc, char* argv[] )
{

  fs::path full_path( fs::initial_path() );

  if ( argc > 1 )
    full_path = fs::system_complete( fs::path( argv[1], fs::native ) );
  else
    std::cout << "\nusage:   simple_ls [path]" << std::endl;

  unsigned long file_count = 0;
  unsigned long dir_count = 0;
  unsigned long err_count = 0;

  if ( !fs::exists( full_path ) )
  {
    std::cout << "\nNot found: " << full_path.native_file_string() << std::endl;
    return 1;
  }

  if ( fs::is_directory( full_path ) )
  {
    std::cout << "\nIn directory: "
              << full_path.native_directory_string() << "\n\n";
    fs::directory_iterator end_iter;
    for ( fs::directory_iterator dir_itr( full_path );
          dir_itr != end_iter;
          ++dir_itr )
    {
      try
      {
        if ( fs::is_directory( *dir_itr ) )
        {
          ++dir_count;
          std::cout << dir_itr->leaf()<< " [directory]\n";
        }
        else
        {
          ++file_count;
          std::cout << dir_itr->leaf() << "\n";
        }
      }
      catch ( const std::exception & ex )
      {
        ++err_count;
        std::cout << dir_itr->leaf() << " " << ex.what() << std::endl;
      }
    }
    std::cout << "\n" << file_count << " files\n"
              << dir_count << " directories\n"
              << err_count << " errors\n";
  }
  else // must be a file
  {
    std::cout << "\nFound: " << full_path.native_file_string() << "\n";    
  }
  return 0;
}
