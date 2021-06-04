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
  *   FILE     jgrep.cpp
  *   VERSION  see <boost/version.hpp>
  */

#include <stdio.h>
#include <algorithm>
#include <boost/regex.hpp>
#ifdef JM_OLD_IOSTREAM
#include <iostream.h>
#else
#include <iostream>
using std::cout;
using std::cin;
using std::cerr;
using std::endl;
#endif
#ifdef __BORLANDC__
#  pragma hrdstop
#endif

#ifdef BOOST_REGEX_V3
#include <boost/regex/v3/fileiter.hpp>
#else
#include <boost/regex/v4/fileiter.hpp>
#endif

#include "jgrep.h"
#ifndef BOOST_REGEX_NO_FILEITER

//
// class ogrep_predicate
// outputs the results of regex_grep to screen:
template <class iterator, class Allocator >
class ogrep_predicate
{
   unsigned int& lines;
   const char* filename;
   unsigned int last_line;
   iterator end_of_storage, last_line_start;
public:
   ogrep_predicate(unsigned int& i, const char* p, iterator start, iterator end) : lines(i), filename(p), last_line(-1), end_of_storage(end), last_line_start(start) {}
   ogrep_predicate(const ogrep_predicate& o) : lines(o.lines), filename(o.filename), last_line(o.last_line), end_of_storage(o.end_of_storage), last_line_start(o.last_line_start) {}
   bool operator () (const boost::match_results<iterator, Allocator>& i);
};

// ideally we'd ignor the allocator type and use a template member function
// to deel with the allocator type passed to regex_grep, unfortunately most
// compilers don't support this feature yet, so we'll have to be sure that
// the allocator passed to instances of this class match that used in our
// regular expression classes.

template <class iterator, class Allocator>
bool ogrep_predicate<iterator, Allocator>::operator()(const boost::match_results<iterator, Allocator>& i)
{
   // if we haven't printed the filename yet, then do it now:
   if(last_line == (unsigned int)-1)
   {
      cout << "File " << filename << ":" << endl;
      last_line = 0;
   }
   // calculate which line we are on, by adding the number of newlines 
   // we've skipped in the last search:
   unsigned int current_line = last_line + std::count(last_line_start, end_of_storage, '\n');
   // if we haven't already printed this line out, then do it now:
   if(last_line != current_line)
   {
      ++lines;
      last_line = current_line;
      if(count_only == 0)
      {
         if(show_lines)
            cout << current_line << "\t";
         const char* nls = "\n";
         iterator ptr = std::find_end(last_line_start, i[0].first, nls, nls+1);
         ++ptr;
         iterator ptr2 = ptr;
         while((ptr2 != end_of_storage) && (*ptr2 != '\n'))++ptr2;
         while(ptr != ptr2)
         {
            cout.put(*ptr);
            ++ptr;
         }
         cout << endl;
      }
   }
   // set the last line seen to the start of the current match:
   last_line_start = i[0].first;
   return true;
}

using namespace boost;

void process_grep(const char* file)
{
   try{
   mapfile f(file);
   unsigned int count = 0;
   ogrep_predicate<mapfile::iterator, boost::match_results<mapfile::iterator>::allocator_type> oi(count, file, f.begin(), f.end());
   if(files_only)
   {
      bool ok;
      boost::match_results<mapfile::iterator> m;
         ok = regex_search(f.begin(), f.end(), m, e, match_not_dot_newline | match_not_dot_null);
      if(ok)
         cout << "File " << file << endl;
   }
   else
   {
      regex_grep(oi, f.begin(), f.end(), e, match_not_dot_newline | match_not_dot_null);
      if(count)
      {
         if(verbose || count_only)
         {
            cout << count << " lines match" << endl;
            return;
         }
      }
      else if(verbose)
      {
         cout << "File " << file << "(" << f.size() << "bytes):" << endl << "0 lines match" << endl;
      }
   }
   }
   catch(const std::exception& e)
   {
      std::cerr << std::endl << e.what() << std::endl;
   }
   catch(...)
   {
   }
}

#endif

