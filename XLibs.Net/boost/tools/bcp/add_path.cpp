/*
 *
 * Copyright (c) 2003 Dr John Maddock
 * Use, modification and distribution is subject to the 
 * Boost Software License, Version 1.0. (See accompanying file 
 * LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 *
 * This file implements the following:
 *    void bcp_implementation::add_path(const fs::path& p)
 *    void bcp_implementation::add_directory(const fs::path& p)
 *    void bcp_implementation::add_file(const fs::path& p)
 *    void bcp_implementation::add_dependent_lib(const std::string& libname)
 */

#include "bcp_imp.hpp"
#include "fileview.hpp"
#include <boost/regex.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/exception.hpp>
#include <iostream>

void bcp_implementation::add_path(const fs::path& p)
{
   if(fs::exists(m_boost_path / p) && fs::is_directory(m_boost_path / p))
      add_directory(p);
   else
      add_file(p);
}

void bcp_implementation::add_directory(const fs::path& p)
{
   //
   // don't add files created by build system
   //
   if((p.leaf() == "bin") || (p.leaf() == "bin-stage"))
      return; 
   //
   // don't add directories not under version control:
   //
   if(m_cvs_mode && !fs::exists(m_boost_path / p / "CVS/Entries"))
      return;
   //
   // enermerate files and directories:
   //
   fs::directory_iterator i(m_boost_path / p);
   fs::directory_iterator j;
   while(i != j)
   {
      //
      // we need to convert *i back into
      // a relative path, what follows is a hack:
      //
      std::string s(i->string());
      if(m_boost_path.string().size())
         s.erase(0, m_boost_path.string().size() + 1);
      if(!m_dependencies.count(fs::path(s))) 
         m_dependencies[fs::path(s)] = p; // set up dependency tree
      add_path(fs::path(s));
      ++i;
   }
}

void bcp_implementation::add_file(const fs::path& p)
{
   //
   // if the file does not exist in cvs then don't do anything with it:
   //
   if(m_cvs_mode && (m_cvs_paths.find(p) == m_cvs_paths.end()))
      return;
   //
   // if we've already seen the file return:
   //
   if(m_copy_paths.find(p) != m_copy_paths.end())
      return;
   //
   // add the file to our list:
   //
   m_copy_paths.insert(p);
   //
   // if this is a source file, scan for dependencies:
   //
   if(is_source_file(p))
   {
      add_file_dependencies(p, false);
   }
   //
   // if this is a html file, scan for dependencies:
   //
   if(is_html_file(p))
   {
      static const boost::regex e(
         "<(?:img[^>]*src=|link[^>]*href=)(\"[^\"]+\"|\\w+)[^>]*>"
         );

      fileview view(m_boost_path / p);
      boost::regex_token_iterator<const char*> i(view.begin(), view.end(), e, 1);
      boost::regex_token_iterator<const char*> j;
      while(i != j)
      {
         //
         // extract the dependent name:
         //
         std::string s(*i);
         if(s[0] == '\"')
         {
            // remove quotes:
            assert(s.size() > 2);
            s.erase(0, 1);
            s.erase(s.size() - 1);
         }
         //
         // if the name starts with ./ remove it
         // or we'll get an error:
         if(s.compare(0, 2, "./") == 0)
            s.erase(0, 2);
         if(s.find(':') == std::string::npos)
         {
            // only concatonate if it's a relative path
            // rather than a URL:
            fs::path dep(p.branch_path() / s);
            if(!m_dependencies.count(dep)) 
               m_dependencies[dep] = p; // set up dependency tree
            add_path(dep);
         }
         ++i;
      }
   }
   //
   // now scan for "special" dependencies:
   // anything that we can't automatically detect...
   //
   static const std::pair<fs::path, fs::path>
      specials[] = {
         std::pair<fs::path, fs::path>("boost/config.hpp", "boost/config"),
         std::pair<fs::path, fs::path>("tools/build/allyourbase.jam", "Jamrules"),
         std::pair<fs::path, fs::path>("tools/build/allyourbase.jam", "project-root.jam"),
         std::pair<fs::path, fs::path>("tools/build/allyourbase.jam", "boost-build.jam"),
         std::pair<fs::path, fs::path>("boost/preprocessor/iterate.hpp", "boost/preprocessor/iteration"),
         std::pair<fs::path, fs::path>("boost/function.hpp", "boost/function/detail"),
         std::pair<fs::path, fs::path>("boost/regex/config.hpp", "boost/regex/user.hpp"),
         std::pair<fs::path, fs::path>("boost/signals/signal_template.hpp", "boost/function"),
         std::pair<fs::path, fs::path>("boost/mpl/list.hpp", "boost/mpl/list"),
         std::pair<fs::path, fs::path>("boost/mpl/list_c.hpp", "boost/mpl/list"),
         std::pair<fs::path, fs::path>("boost/mpl/vector.hpp", "boost/mpl/vector"),
         std::pair<fs::path, fs::path>("boost/mpl/vector_c.hpp", "boost/mpl/vector"),
      };
   for(unsigned int n = 0; n < (sizeof(specials)/sizeof(specials[0])); ++n)
   {
      if(0 == compare_paths(specials[n].first, p))
      {
         if(!m_dependencies.count(specials[n].second)) 
            m_dependencies[specials[n].second] = p; // set up dependency tree
         add_path(specials[n].second);
      }
   }

}

void bcp_implementation::add_dependent_lib(const std::string& libname, const fs::path& p)
{
   //
   // if the boost library libname has source associated with it
   // then add the source to our list:
   //
   if(fs::exists(m_boost_path / "libs" / libname / "src"))
   {
      if(!m_dependencies.count(fs::path("libs") / libname / "src")) 
         m_dependencies[fs::path("libs") / libname / "src"] = p; // set up dependency tree
      add_path(fs::path("libs") / libname / "src");
      if(fs::exists(m_boost_path / "libs" / libname / "build"))
      {
         if(!m_dependencies.count(fs::path("libs") / libname / "build")) 
            m_dependencies[fs::path("libs") / libname / "build"] = p; // set up dependency tree
         add_path(fs::path("libs") / libname / "build");
      }
   }
}

void bcp_implementation::add_file_dependencies(const fs::path& p, bool scanfile)
{
   static const boost::regex e(
      "^[[:blank:]]*#[[:blank:]]*include[[:blank:]]*[\"<]([^\">]+)[\">]"
      );

   if(!m_dependencies.count(p)) 
      m_dependencies[p] = p; // set terminal dependency

   fileview view;
   if(scanfile)
      view.open(p);
   else
      view.open(m_boost_path / p);
   if(m_licence_mode && !scanfile)
      scan_licence(p, view);
   boost::regex_token_iterator<const char*> i(view.begin(), view.end(), e, 1);
   boost::regex_token_iterator<const char*> j;
   while(i != j)
   {
      //
      // *i contains the name of the include file,
      // check first for a file of that name in the
      // same directory as the file we are scanning,
      // and if that fails, then check the boost-root:
      //
      fs::path include_file;
      try{
         include_file = i->str();
      }
      catch(const fs::filesystem_error& e)
      {
         std::cerr << "Can't parse filename " << *i << " included by file " << p.string() << std::endl;
         ++i;
         continue;
      }
      fs::path test_file(m_boost_path / p.branch_path() / include_file);
      if(fs::exists(test_file) && !fs::is_directory(test_file) && (p.branch_path().string() != "boost"))
      {
         if(!m_dependencies.count(p.branch_path() / include_file)) 
            m_dependencies[p.branch_path() / include_file] = p;
         add_path(p.branch_path() / include_file);
      }
      else if(fs::exists(m_boost_path / include_file))
      {
         if(!m_dependencies.count(include_file)) 
            m_dependencies[include_file] = p;
         add_path(include_file);
      }
      ++i;
   }
   //
   // if the file contains a cpp_main / unit_test_main / test_main
   // it is dependent upon Boost.test even if it doesn't
   // include any of the Boost.test headers directly.
   //
   static const boost::regex m("^\\s*int\\s+(?:cpp_main|test_main|unit_test_main)");
   boost::cmatch what;
   if(boost::regex_search(view.begin(), view.end(), what, m))
   {
      add_dependent_lib("test", p);
   }
   //
   // grab the name of the library to which the header belongs, 
   // and if that library has source then add the source to our
   // list:
   //
   // this regex catches boost/libname.hpp or boost/libname/whatever:
   //
   static const boost::regex lib1("boost/([^\\./]+)(?!detail).*");
   boost::smatch swhat;
   if(boost::regex_match(p.string(), swhat, lib1))
   {
      add_dependent_lib(swhat.str(1), p);
   }
   //
   // and this one catches boost/x/y/whatever (for example numeric/ublas):
   //
   static const boost::regex lib2("boost/([^/]+/[^/]+)/(?!detail).*");
   if(boost::regex_match(p.string(), swhat, lib2))
   {
      add_dependent_lib(swhat.str(1), p);
   }
}
