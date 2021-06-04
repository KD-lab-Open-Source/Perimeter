/*
 *
 * Copyright (c) 2003 Dr John Maddock
 * Use, modification and distribution is subject to the 
 * Boost Software License, Version 1.0. (See accompanying file 
 * LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 *
 */

#include "licence_info.hpp"
#include "bcp_imp.hpp"
#include "fileview.hpp"
#include <fstream>
#include <iomanip>
#include <cstring>
#include <stdexcept>
#include <boost/lexical_cast.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/throw_exception.hpp>

//
// split_path is a small helper for outputting a path name, 
// complete with a link to that path:
//
struct split_path
{
   const fs::path& root;
   const fs::path& file;
   split_path(const fs::path& r, const fs::path& f)
      : root(r), file(f){}
};

std::ostream& operator << (std::ostream& os, const split_path& p)
{
   os << "<a href=\"" << (p.root / p.file).string() << "\">" << p.file.string() << "</a>";
   return os;
}

std::string make_link_target(const std::string& s)
{
   // convert an arbitrary string into something suitable
   // for an <a> name:
   std::string result;
   for(unsigned i = 0; i < s.size(); ++i)
   {
      result.append(1, static_cast<std::string::value_type>(std::isalnum(s[i]) ? s[i] : '_'));
   }
   return result;
}


void bcp_implementation::output_licence_info()
{
   std::pair<const licence_info*, int> licences = get_licences();

   std::map<int, licence_data>::const_iterator i, j;
   i = m_licence_data.begin();
   j = m_licence_data.end();

   std::ofstream os(m_dest_path.native_file_string().c_str());
   if(!os)
   {
      std::string msg("Error opening ");
      msg += m_dest_path.string();
      msg += " for output.";
      std::runtime_error e(msg);
      boost::throw_exception(e);
   }
   os << 
      "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0 Transitional//EN\">\n"
      "<html>\n"
      "<head>\n"
      "<title>Boost Licence Dependency Information</title>\n"
      "</head>\n"
      "<body>\n"
      "<H1>Boost Licence Dependency Information</H1>\n"
      "<H2>Contents</h2>\n"
      "<pre><a href=\"#input\">Input Information</a>\n"
      "<a href=\"#summary\">Licence Summary</a>\n"
      "<a href=\"#details\">Licence Details</a>\n";

   while(i != j)
   {
      // title:
      os << "   <A href=\"#" << make_link_target(licences.first[i->first].licence_name) 
         << "\">" << licences.first[i->first].licence_name << "</a>\n";
      ++i;
   }

   os << "<a href=\"#files\">Files with no recognised licence</a>\n"
      "<a href=\"#authors\">Files with no recognised copyright holder</a>\n"
      "<a href=\"#copyright\">Copyright Holder Information</a>\n"
      "<a href=\"#depend\">File Dependency Information</a>\n"
      "</pre>";

   //
   // input Information:
   //
   os << "<a name=\"input\"></a><h2>Input Information</h2>\n";
   if(m_scan_mode)
      os << "<P>The following files were scanned for boost dependencies:<BR>";
   else
      os << "<P>The following Boost modules were checked:<BR>";

   std::list<std::string>::const_iterator si = m_module_list.begin();
   std::list<std::string>::const_iterator sj = m_module_list.end();
   while(si != sj)
   {
      os << *si << "<BR>";
      ++si;
   }
   os << "</p><p>The Boost path was: <code>" << m_boost_path.string() << "</code></P>";
   //
   // extract the boost version number from the boost directory tree, 
   // not from this app (which may have been built from a previous
   // version):
   //
   fileview version_file(m_boost_path / "boost/version.hpp");
   boost::regex version_regex(
      "^[[:blank:]]*#[[:blank:]]*define[[:blank:]]+BOOST_VERSION[[:blank:]]+(\\d+)");
   boost::cmatch what;
   if(boost::regex_search(version_file.begin(), version_file.end(), what, version_regex))
   {
      int version = boost::lexical_cast<int>(what.str(1));
      os << "<p>The Boost version is: " << version / 100000 << "." << version / 100 % 1000 << "." << version % 100 << "</P>\n";
   }

   //
   // output each licence:
   //
   i = m_licence_data.begin();
   j = m_licence_data.end();
   //
   // start with the summary:
   //
   os << "<a name=\"summary\"></a><h2>Licence Summary</h2>\n";
   while(i != j)
   {
      // title:
      os << 
         "<H3>" << licences.first[i->first].licence_name << "</H3>\n";
      // licence text:
      os << "<BLOCKQUOTE>" << licences.first[i->first].licence_text << "</BLOCKQUOTE>";
      // Copyright holders:
      os << "<P>This licence is used by " << i->second.authors.size() 
         << " authors and " << i->second.files.size() 
         << " files <a href=\"#" << make_link_target(licences.first[i->first].licence_name) << "\">(see details)</a>";
      os << "</P></BLOCKQUOTE>\n";
      ++i;
   }
   //
   // and now the details:
   //
   i = m_licence_data.begin();
   j = m_licence_data.end();
   os << "<a name=\"details\"></a><h2>Licence Details</h2>\n";
   while(i != j)
   {
      // title:
      os << 
         "<H3><A name=\"" << make_link_target(licences.first[i->first].licence_name) 
         << "\"></a>" << licences.first[i->first].licence_name << "</H3>\n";
      // licence text:
      os << "<BLOCKQUOTE>" << licences.first[i->first].licence_text << "</BLOCKQUOTE>";
      // Copyright holders:
      os << "<P>This licence is used by the following " << i->second.authors.size() << " copyright holders:</P>\n<BLOCKQUOTE><P>";
      std::set<std::string>::const_iterator x, y;
      x = i->second.authors.begin();
      y = i->second.authors.end();
      while(x != y)
      {
         os << *x << "<BR>\n";
         ++x;
      }
      os << "</P></BLOCKQUOTE>\n";
     // Files using this licence:
      os << "<P>This licence applies to the following " << i->second.files.size() << " files:</P>\n<BLOCKQUOTE><P>";
      std::set<fs::path, path_less>::const_iterator m, n;
      m = i->second.files.begin();
      n = i->second.files.end();
      while(m != n)
      {
         os << split_path(m_boost_path, *m) << "<br>\n";
         ++m;
      }
      os << "</P></BLOCKQUOTE>\n";
      ++i;
   }
   //
   // Output list of files not found to be under licence control:
   //
   os << "<h2><a name=\"files\"></a>Files With No Recognisable Licence</h2>\n"
      "<P>The following " << m_unknown_licences.size() << " files had no recognisable licence information:</P><BLOCKQUOTE><P>\n";
   std::set<fs::path, path_less>::const_iterator i2, j2;
   i2 = m_unknown_licences.begin();
   j2 = m_unknown_licences.end();
   while(i2 != j2)
   {
      os << split_path(m_boost_path, *i2) << "<br>\n";
      ++i2;
   }
   os << "</p></BLOCKQUOTE>";
   //
   // Output list of files with no found copyright holder:
   //
   os << "<h2><a name=\"authors\"></a>Files With No Recognisable Copyright Holder</h2>\n"
      "<P>The following " << m_unknown_authors.size() << " files had no recognisable copyright holder:</P>\n<BLOCKQUOTE><P>";
   i2 = m_unknown_authors.begin();
   j2 = m_unknown_authors.end();
   while(i2 != j2)
   {
      os << split_path(m_boost_path, *i2) << "<br>\n";
      ++i2;
   }
   os << "</p></BLOCKQUOTE>";

   //
   // output a table of copyright information:
   //
   os << "<H2><a name=\"copyright\"></a>Copyright Holder Information</H2><table border=\"1\">\n";
   std::map<std::string, std::set<fs::path, path_less> >::const_iterator ad, ead; 
   ad = m_author_data.begin();
   ead = m_author_data.end();
   while(ad != ead)
   {
      os << "<tr><td>" << ad->first << "</td><td>";
      std::set<fs::path, path_less>::const_iterator fi, efi;
      fi = ad->second.begin();
      efi = ad->second.end();
      while(fi != efi)
      {
         os << split_path(m_boost_path, *fi) << " ";
         ++fi;
      }
      os << "</td></tr>\n";
      ++ad;
   }
   os << "</table>\n";

   //
   // output file dependency information:
   //
   os << "<H2><a name=\"depend\"></a>File Dependency Information</H2><BLOCKQUOTE><pre>\n";
   std::map<fs::path, fs::path, path_less>::const_iterator dep, last_dep;
   std::set<fs::path, path_less>::const_iterator fi, efi;
   fi = m_copy_paths.begin();
   efi = m_copy_paths.end();
   while(fi != efi)
   {
      os << split_path(m_boost_path, *fi);
      dep = m_dependencies.find(*fi);
      last_dep = m_dependencies.end();
      std::set<fs::path, path_less> seen_deps;
      while(true)
      {
         os << " -> ";
         if(fs::exists(m_boost_path / dep->second))
            os << split_path(m_boost_path, dep->second);
         else if(fs::exists(dep->second))
            os << split_path(fs::path(), dep->second);
         else
            os << dep->second.string();
         if(seen_deps.find(dep->second) != seen_deps.end())
         {
            os << " <I>(Cirular dependency!)</I>";
            break; // circular dependency!!!
         }
         seen_deps.insert(dep->second);
         last_dep = dep;
         dep = m_dependencies.find(dep->second);
         if((dep == m_dependencies.end()) || (0 == compare_paths(dep->second, last_dep->second)))
            break;
      }
      os << "\n";
      ++fi;
   }
   os << "</pre></BLOCKQUOTE>\n";

   os << "</body></html>\n";

   if(!os)
   {
      std::string msg("Error writing to ");
      msg += m_dest_path.string();
      msg += ".";
      std::runtime_error e(msg);
      boost::throw_exception(e);
   }

}
