//  Boost operations_test.cpp  -----------------------------------------------//

//  Copyright Beman Dawes 2002.
//  Use, modification, and distribution is subject to the Boost Software
//  License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

//  See library home page at http://www.boost.org/libs/filesystem

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/exception.hpp>
namespace fs = boost::filesystem;

#include <boost/config.hpp>
#include <boost/test/minimal.hpp>
#include <boost/concept_check.hpp>
#include <boost/bind.hpp>
using boost::bind;

#include <fstream>
#include <iostream>
#include <string>
#include <cerrno>
#include <ctime>

# ifdef BOOST_NO_STDC_NAMESPACE
    namespace std { using ::asctime; using ::gmtime; using ::localtime;
    using ::difftime; using ::time; using ::tm; using ::mktime; }
# endif

namespace
{
  bool report_throws;
  fs::directory_iterator end_itr;

  void create_file( const fs::path & ph, const std::string & contents )
  {
    std::ofstream f( ph.native_file_string().c_str() );
    if ( !f )
      throw fs::filesystem_error( "operations_test create_file",
        ph, errno );
    if ( !contents.empty() ) f << contents;
  }

  void verify_file( const fs::path & ph, const std::string & expected )
  {
    std::ifstream f( ph.native_file_string().c_str() );
    if ( !f )
      throw fs::filesystem_error( "operations_test verify_file",
        ph, errno );
    std::string contents;
    f >> contents;
    if ( contents != expected )
      throw fs::filesystem_error( "operations_test verify_file",
        ph, " contents \"" + contents
        + "\" != \"" + expected + "\"" );
  }

  template< typename F >
    bool throws_fs_error( F func, fs::error_code ec =
      ::boost::filesystem::no_error ) // VC++ 7.1 build 2292 won't accept fs::
  {
    try { func(); }

    catch ( const fs::filesystem_error & ex )
    {
      if ( report_throws ) std::cout << ex.what() << "\n";
      if ( ec == fs::no_error || ec == ex.error() ) return true;
      std::cout << "filesystem_error::error() reports " << ex.error()
        << ", should be " << ec
        << "\n native_error() is " << ex.native_error()
        << std::endl;
    }
    return false;
  }

} // unnamed namespace

//  test_main  ---------------------------------------------------------------//

int test_main( int argc, char * argv[] )
{
  if ( argc > 1 && *argv[1]=='-' && *(argv[1]+1)=='t' ) report_throws = true;

  std::string platform( BOOST_PLATFORM );

  // The choice of platform is make at runtime rather than compile-time
  // so that compile errors for all platforms will be detected even though
  // only the current platform is runtime tested.
# if defined( BOOST_POSIX )
    platform = "POSIX";
# elif defined( BOOST_WINDOWS )
    platform = "Windows";
# else
    platform = ( platform == "Win32" || platform == "Win64" || platform == "Cygwin" )
               ? "Windows"
               : "POSIX";
# endif
  std::cout << "Platform is " << platform << '\n';

  std::cout << "initial_path().string() is\n  \""
            << fs::initial_path().string()
            << "\"\n";
  std::cout << "initial_path().native_file_string() is\n  \""
            << fs::initial_path().native_file_string()
            << "\"\n";

  BOOST_TEST( fs::initial_path().is_complete() );
  BOOST_TEST( fs::current_path().is_complete() );
  BOOST_TEST( fs::initial_path().string() == fs::current_path().string() );

  BOOST_TEST( fs::complete( "" ).empty() );
  BOOST_TEST( fs::complete( "/" ).string()
    == fs::initial_path().root_path().string() );
  BOOST_TEST( fs::complete( "foo" ).string()
    == fs::initial_path().string()+"/foo" );
  BOOST_TEST( fs::complete( "/foo" ).string()
    == fs::initial_path().root_path().string()+"foo" );

  fs::path dir(  fs::initial_path() / "temp_fs_test_directory" );
  
  // Windows only tests
  if ( platform == "Windows" )
  {
    BOOST_TEST( dir.string().size() > 1
      && dir.string()[1] == ':' ); // verify path includes drive

    BOOST_TEST( fs::system_complete( "" ).empty() );
    BOOST_TEST( fs::system_complete( "/" ).string()
      == fs::initial_path().root_path().string() );
    BOOST_TEST( fs::system_complete( "foo" ).string()
      == fs::initial_path().string()+"/foo" );
    BOOST_TEST( fs::system_complete( "/foo" ).string()
      == fs::initial_path().root_path().string()+"foo" );

//    BOOST_TEST( fs::complete( fs::path( "c:", fs::native ) ).string()
//      == fs::initial_path().string() );
//    BOOST_TEST( fs::complete( fs::path( "c:foo", fs::native ) ).string()
//      == fs::initial_path().string()+"/foo" );
    BOOST_TEST( fs::complete( fs::path( "c:/", fs::native ) ).string()
      == "c:/" );
    BOOST_TEST( fs::complete( fs::path( "c:/foo", fs::native ) ).string()
      ==  "c:/foo" );
    BOOST_TEST( fs::complete( fs::path( "//share", fs::native ) ).string()
      ==  "//share" );

    BOOST_TEST( fs::system_complete( fs::path( fs::initial_path().root_name(),
      fs::native ) ).string() == fs::initial_path().string() );
    BOOST_TEST( fs::system_complete( fs::path( fs::initial_path().root_name()
      + "foo", fs::native ) ).string() == fs::initial_path().string()+"/foo" );
    BOOST_TEST( fs::system_complete( fs::path( "c:/", fs::native ) ).string()
      == "c:/" );
    BOOST_TEST( fs::system_complete( fs::path( "c:/foo", fs::native ) ).string()
      ==  "c:/foo" );
    BOOST_TEST( fs::system_complete( fs::path( "//share", fs::native ) ).string()
      ==  "//share" );
  }

  else if ( platform == "POSIX" )
  {
    BOOST_TEST( fs::system_complete( "" ).empty() );
    BOOST_TEST( fs::initial_path().root_path().string() == "/" );
    BOOST_TEST( fs::system_complete( "/" ).string() == "/" );
    BOOST_TEST( fs::system_complete( "foo" ).string()
      == fs::initial_path().string()+"/foo" );
    BOOST_TEST( fs::system_complete( "/foo" ).string()
      == fs::initial_path().root_path().string()+"foo" );
  }

  fs::path ng( " no-way, Jose", fs::native );

  fs::remove_all( dir );  // in case residue from prior failed tests
  BOOST_TEST( !fs::exists( dir ) );

  // the bound functions should throw, so throws_fs_error() should return true
  BOOST_TEST( throws_fs_error( bind( fs::is_directory, ng ), fs::not_found_error ) );
  BOOST_TEST( throws_fs_error( bind( fs::file_size, ng ), fs::not_found_error ) );
  BOOST_TEST( throws_fs_error( bind( fs::is_directory, dir ) ) );
  BOOST_TEST( throws_fs_error( bind( fs::_is_empty, dir ) ) );

  BOOST_TEST( fs::create_directory( dir ) );

  BOOST_TEST( fs::exists( dir ) );
  BOOST_TEST( fs::_is_empty( dir ) );
  BOOST_TEST( fs::is_directory( dir ) );
  BOOST_TEST( throws_fs_error( bind( fs::file_size, dir ),
    fs::is_directory_error ) );
  BOOST_TEST( !fs::create_directory( dir ) );

  BOOST_TEST( !fs::symbolic_link_exists( dir ) );
  BOOST_TEST( !fs::symbolic_link_exists( "nosuchfileordirectory" ) );

  fs::path d1( dir / "d1" );
  BOOST_TEST( fs::create_directory( d1 ) );
  BOOST_TEST( fs::exists( d1 ) );
  BOOST_TEST( fs::is_directory( d1 ) );
  BOOST_TEST( fs::_is_empty( d1 ) );

  boost::function_requires< boost::InputIteratorConcept< fs::directory_iterator > >();

  {
    fs::directory_iterator dir_itr( dir );
    BOOST_TEST( dir_itr->leaf() == "d1" );
  }

  // create a second directory named d2
  fs::path d2( dir / "d2" );
  fs::create_directory(d2 );
  BOOST_TEST( fs::exists( d2 ) );
  BOOST_TEST( fs::is_directory( d2 ) );

  {
    fs::directory_iterator dir_itr( dir );
    BOOST_TEST( dir_itr->leaf() == "d1" || dir_itr->leaf() == "d2" );
    if ( dir_itr->leaf() == "d1" )
    {
      BOOST_TEST( (++dir_itr)->leaf() == "d2" );
    }
    else
    {
      BOOST_TEST( (++dir_itr)->leaf() == "d1" );
    }
  }

  { // *i++ must work to meet the standard's InputIterator requirements
    fs::directory_iterator dir_itr( dir );
    BOOST_TEST( dir_itr->leaf() == "d1" || dir_itr->leaf() == "d2" );
    if ( dir_itr->leaf() == "d1" )
    {
      // Note result for single_pass_traversal requirements is
      // different than for input_iterator requirements
      BOOST_TEST( (*dir_itr++).leaf() == "d2" );
      BOOST_TEST( dir_itr->leaf() == "d2" );
    }
    else
    {
      // Note result for single_pass_traversal requirements is
      // different than for input_iterator requirements
      BOOST_TEST( (*dir_itr++).leaf() == "d1" );
      BOOST_TEST( dir_itr->leaf() == "d1" );
    }
  }

  // create an empty file named "f0"
  fs::path file_ph( dir / "f0");
  create_file( file_ph, "" );
  BOOST_TEST( fs::exists( file_ph ) );
  BOOST_TEST( !fs::is_directory( file_ph ) );
  BOOST_TEST( fs::_is_empty( file_ph ) );
  BOOST_TEST( fs::file_size( file_ph ) == 0 );
  BOOST_TEST( throws_fs_error( bind( fs::create_directory, file_ph ),
    fs::not_directory_error ) );

  // create a file named "f1"
  file_ph = dir / "f1";
  create_file( file_ph, "foobar1" );

  // equivalence tests
  fs::path ng2("does_not_exist2");
  BOOST_TEST( throws_fs_error( bind( fs::equivalent, ng, ng2 ) ) );
  BOOST_TEST( fs::equivalent( file_ph, dir / "f1" ) );
  BOOST_TEST( fs::equivalent( dir, d1 / ".." ) );
  BOOST_TEST( !fs::equivalent( file_ph, dir ) );
  BOOST_TEST( !fs::equivalent( dir, file_ph ) );
  BOOST_TEST( !fs::equivalent( d1, d2 ) );
  BOOST_TEST( !fs::equivalent( dir, ng ) );
  BOOST_TEST( !fs::equivalent( ng, dir ) );
  BOOST_TEST( !fs::equivalent( file_ph, ng ) );
  BOOST_TEST( !fs::equivalent( ng, file_ph ) );

  std::time_t ft = fs::last_write_time( file_ph );
  std::cout << "UTC should currently be about " << std::asctime(std::gmtime(&ft)) << "\n";
  std::cout << "Local time should currently be about " << std::asctime(std::localtime(&ft)) << std::endl;

  // hard to test time exactly, but except under the most unusual circumstances,
  // time since file creation should be no more than one minute, I'm hoping.
  double time_diff = std::difftime( std::time(0), fs::last_write_time( file_ph ) );
  BOOST_TEST( time_diff >= 0.0 && time_diff < 60.0 );

  BOOST_TEST( fs::exists( file_ph ) );
  BOOST_TEST( !fs::is_directory( file_ph ) );
  BOOST_TEST( fs::file_size( file_ph ) == 7 );
  verify_file( file_ph, "foobar1" );

#if !BOOST_WORKAROUND(__BORLANDC__, <= 0x564)
  std::tm * tmp = std::localtime( &ft );
  --tmp->tm_year;
  std::cout << "Change year to " << tmp->tm_year << std::endl;
  fs::last_write_time( file_ph, std::mktime( tmp ) );
  std::cout << "Now get time difference" << std::endl;
  time_diff = std::difftime( std::time(0), fs::last_write_time( file_ph ) );
  BOOST_TEST( time_diff >= 365*24*3600.0 && time_diff < (366*24*3600.0 + 60.0) );
  ft = fs::last_write_time( file_ph );
  std::cout << "Local time one year ago should currently be about " << std::asctime(std::localtime(&ft)) << "\n";
  fs::last_write_time( file_ph, std::time_t() );
  time_diff = std::difftime( std::time(0), fs::last_write_time( file_ph ) );
  BOOST_TEST( time_diff >= 0.0 && time_diff < 60.0 );
  ft = fs::last_write_time( file_ph );
  std::cout << "Local time should currently be about " << std::asctime(std::localtime(&ft)) << "\n";
#else
  std::cout <<
    "<note>\n"
    "Changing a file time via boost::filesystem::last_write_time() fails for this compiler\n"
    "This will not affect other uses of the library.\n"
    "</note>\n";
#endif

  // there was an inital bug in directory_iterator that caused premature
  // close of an OS handle. This block will detect regression.
  {
    fs::directory_iterator di;
    { di = fs::directory_iterator( dir ); }
    BOOST_TEST( ++di != fs::directory_iterator() );
  }

  // copy_file() tests
  fs::copy_file( file_ph, d1 / "f2" );
  BOOST_TEST( fs::exists( file_ph ) );
  BOOST_TEST( fs::exists( d1 / "f2" ) );
  BOOST_TEST( !fs::is_directory( d1 / "f2" ) );
  verify_file( d1 / "f2", "foobar1" );

  // rename() test case numbers refer to operations.htm#rename table

  // [case 1] make sure can't rename() a non-existent file
  BOOST_TEST( !fs::exists( d1 / "f99" ) );
  BOOST_TEST( !fs::exists( d1 / "f98" ) );
  BOOST_TEST( throws_fs_error( bind( fs::rename, d1 / "f99", d1 / "f98" ),
    fs::not_found_error ) );
  BOOST_TEST( throws_fs_error( bind( fs::rename, fs::path(""), d1 / "f98" ),
    fs::not_found_error ) );

  // [case 2] rename() target.empty()
  BOOST_TEST( throws_fs_error( bind( fs::rename, file_ph, "" ),
    fs::not_found_error ) );

  // [case 3] make sure can't rename() to an existent file or directory
  BOOST_TEST( fs::exists( dir / "f1" ) );
  BOOST_TEST( fs::exists( d1 / "f2" ) );
  BOOST_TEST( throws_fs_error( bind( fs::rename, dir / "f1", d1 / "f2" ) ) );
  // several POSIX implementations (cygwin, openBSD) report ENOENT instead of EEXIST,
  // so we don't verify error type on the above test.
  BOOST_TEST( throws_fs_error( bind( fs::rename, dir, d1 ) ) );

  // [case 4A] can't rename() file to a nonexistent parent directory
  BOOST_TEST( !is_directory( dir / "f1" ) );
  BOOST_TEST( !exists( dir / "d3/f3" ) );
  BOOST_TEST( throws_fs_error( bind( fs::rename, dir / "f1", dir / "d3/f3" ),
    fs::not_found_error ) );

  // [case 4B] rename() file in same directory
  BOOST_TEST( fs::exists( d1 / "f2" ) );
  BOOST_TEST( !fs::exists( d1 / "f50" ) );
  fs::rename( d1 / "f2", d1 / "f50" );
  BOOST_TEST( !fs::exists( d1 / "f2" ) );
  BOOST_TEST( fs::exists( d1 / "f50" ) );
  fs::rename( d1 / "f50", d1 / "f2" );
  BOOST_TEST( fs::exists( d1 / "f2" ) );
  BOOST_TEST( !fs::exists( d1 / "f50" ) );

  // [case 4C] rename() file d1/f2 to d2/f3
  fs::rename( d1 / "f2", d2 / "f3" );
  BOOST_TEST( !fs::exists( d1 / "f2" ) );
  BOOST_TEST( !fs::exists( d2 / "f2" ) );
  BOOST_TEST( fs::exists( d2 / "f3" ) );
  BOOST_TEST( !fs::is_directory( d2 / "f3" ) );
  verify_file( d2 / "f3", "foobar1" );
  fs::rename( d2 / "f3", d1 / "f2" );
  BOOST_TEST( fs::exists( d1 / "f2" ) );

  // [case 5A] rename() directory to nonexistent parent directory
  BOOST_TEST( fs::exists( d1 ) );
  BOOST_TEST( !fs::exists( dir / "d3/d5" ) );
  BOOST_TEST( !fs::exists( dir / "d3" ) );
  BOOST_TEST( throws_fs_error( bind( fs::rename, d1, dir / "d3/d5" ),
    fs::not_found_error ) );

  // [case 5B] rename() on directory
  fs::path d3( dir / "d3" );
  BOOST_TEST( fs::exists( d1 ) );
  BOOST_TEST( fs::exists( d1 / "f2" ) );
  BOOST_TEST( !fs::exists( d3 ) );
  fs::rename( d1, d3 );
  BOOST_TEST( !fs::exists( d1 ) );
  BOOST_TEST( fs::exists( d3 ) );
  BOOST_TEST( fs::is_directory( d3 ) );
  BOOST_TEST( !fs::exists( d1 / "f2" ) );
  BOOST_TEST( fs::exists( d3 / "f2" ) );
  fs::rename( d3, d1 );
  BOOST_TEST( fs::exists( d1 ) );
  BOOST_TEST( fs::exists( d1 / "f2" ) );
  BOOST_TEST( !fs::exists( d3 ) );

  // [case 5C] rename() rename and move d1 to d2 / "d20"
  BOOST_TEST( exists( d1 ) );
  BOOST_TEST( !exists( d2 / "d20" ) );
  BOOST_TEST( fs::exists( d1 / "f2" ) );
  fs::rename( d1, d2 / "d20" );
  BOOST_TEST( !exists( d1 ) );
  BOOST_TEST( exists( d2 / "d20" ) );
  BOOST_TEST( fs::exists( d2 / "d20" / "f2" ) );
  fs::rename( d2 / "d20", d1 );
  BOOST_TEST( exists( d1 ) );
  BOOST_TEST( !exists( d2 / "d20" ) );
  BOOST_TEST( fs::exists( d1 / "f2" ) );

  // remove() tests on file
  file_ph = dir / "shortlife";
  BOOST_TEST( !fs::exists( file_ph ) );
  create_file( file_ph, "" );
  BOOST_TEST( fs::exists( file_ph ) );
  BOOST_TEST( !fs::is_directory( file_ph ) );
  BOOST_TEST( fs::remove( file_ph ) );
  BOOST_TEST( !fs::exists( file_ph ) );
  BOOST_TEST( !fs::remove( "no-such-file" ) );
  BOOST_TEST( !fs::remove( "no-such-directory/no-such-file" ) );

  // remove() test on directory
  d1 = dir / "shortlife_dir";
  BOOST_TEST( !fs::exists( d1 ) );
  fs::create_directory( d1 );
  BOOST_TEST( fs::exists( d1 ) );
  BOOST_TEST( fs::is_directory( d1 ) );
  BOOST_TEST( fs::_is_empty( d1 ) );
  BOOST_TEST( throws_fs_error( bind( fs::remove, dir ), fs::not_empty_error ) );
  BOOST_TEST( fs::remove( d1 ) );
  BOOST_TEST( !fs::exists( d1 ) );

  // post-test cleanup
  BOOST_TEST( fs::remove_all( dir ) != 0 );
  // above was added just to simplify testing, but it ended up detecting
  // a bug (failure to close an internal search handle). 
  BOOST_TEST( !fs::exists( dir ) );
  BOOST_TEST( fs::remove_all( dir ) == 0 );

  return 0;
} // main

