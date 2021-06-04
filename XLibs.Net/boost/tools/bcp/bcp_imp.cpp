/*
 *
 * Copyright (c) 2003 Dr John Maddock
 * Use, modification and distribution is subject to the 
 * Boost Software License, Version 1.0. (See accompanying file 
 * LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 *
 * This file implements the bcp_implementation virtuals.
 */

#include "bcp_imp.hpp"
#include <boost/filesystem/operations.hpp>
#include <iostream>
#include <stdexcept>

bcp_implementation::bcp_implementation()
  : m_list_mode(false), m_licence_mode(false), m_cvs_mode(false), m_unix_lines(false), m_scan_mode(false)
{
}

bcp_implementation::~bcp_implementation()
{
}

bcp_application::~bcp_application()
{
}

void bcp_implementation::enable_list_mode()
{
   m_list_mode = true;
}

void bcp_implementation::enable_cvs_mode()
{
   m_cvs_mode = true;
}

void bcp_implementation::enable_scan_mode()
{
   m_scan_mode = true;
}

void bcp_implementation::enable_licence_mode()
{
   m_licence_mode = true;
}

void bcp_implementation::enable_unix_lines()
{
   m_unix_lines = true;
}

void bcp_implementation::set_boost_path(const char* p)
{
   m_boost_path = fs::path(p, fs::native);
}

void bcp_implementation::set_destination(const char* p)
{
   m_dest_path = fs::path(p, fs::native);
}

void bcp_implementation::add_module(const char* p)
{
   m_module_list.push_back(p);
}

int bcp_implementation::run()
{
   //
   // check output path is OK:
   //
   if(!m_list_mode && !m_licence_mode && !fs::exists(m_dest_path))
   {
      std::string msg("Destination path does not exist: ");
      msg.append(m_dest_path.native_file_string());
      std::runtime_error e(msg);
      boost::throw_exception(e);
   }
   // start by building a list of permitted files
   // if m_cvs_mode is true:
   if(m_cvs_mode)
   {
      scan_cvs_path(fs::path());
   }
   //
   // scan through modules looking for the equivalent
   // file to add to our list:
   //
   std::list<std::string>::const_iterator i = m_module_list.begin();
   std::list<std::string>::const_iterator j = m_module_list.end();
   while(i != j)
   {
      //
      // convert *i to a path - could be native or portable:
      //
      fs::path module;
      fs::path exmodule;
      try{
         module = fs::path(*i);
         exmodule = fs::path(*i + ".hpp");
      }
      catch(...)
      {
         module = fs::path(*i, fs::native);
         exmodule = fs::path(*i + ".hpp", fs::native);
      }
      
      if(m_scan_mode)
      {
         // in scan mode each module must be a real file:
         add_file_dependencies(module, true);
      }
      else
      {
         int count = 0;
         if(fs::exists(m_boost_path / "tools" / module))
         {
            add_path(fs::path("tools") / module);
            ++count;
         }
         if(fs::exists(m_boost_path / "libs" / module))
         {
            add_path(fs::path("libs") / module);
            ++count;
         }
         if(fs::exists(m_boost_path / "boost" / module))
         {
            add_path(fs::path("boost") / module);
            ++count;
         }
         if(fs::exists(m_boost_path / "boost" / exmodule))
         {
            add_path(fs::path("boost") / exmodule);
            ++count;
         }
         if(fs::exists(m_boost_path / module))
         {
            add_path(module);
            ++count;
         }
      }
      ++i;
   }
   //
   // now perform output:
   //
   std::set<fs::path, path_less>::iterator m, n;
   m = m_copy_paths.begin();
   n = m_copy_paths.end();
   if(!m_licence_mode)
   {
      while(m != n)
      {
         if(m_list_mode)
            std::cout << m->string() << "\n";
         else
            copy_path(*m);
         ++m;
      }
   }
   else
      output_licence_info();
   return 0;
}

pbcp_application bcp_application::create()
{
   pbcp_application result(static_cast<bcp_application*>(new bcp_implementation()));
   return result;
}
