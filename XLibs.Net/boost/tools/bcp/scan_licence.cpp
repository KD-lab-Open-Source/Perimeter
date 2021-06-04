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

void bcp_implementation::scan_licence(const fs::path& p, const fileview& v)
{
   std::pair<const licence_info*, int> licences = get_licences();
   //
   // scan file for all the licences in the list:
   //
   int licence_count = 0;
   int author_count = 0;
   for(int i = 0; i < licences.second; ++i)
   {
      if(boost::regex_search(v.begin(), v.end(), licences.first[i].licence_signature))
      {
         // add this licence to the list:
         m_licence_data[i].files.insert(p);
         ++licence_count;
         //
         // scan for the associated copyright declarations:
         //
         boost::regex_iterator<const char*> cpy(v.begin(), v.end(), licences.first[i].copyright_signature);
         boost::regex_iterator<const char*> ecpy;
         while(cpy != ecpy)
         {
            // extract the copy holders as a list:
            std::string author_list = cpy->format(licences.first[i].copyright_formatter, boost::format_all);
            // now enumerate that list for all the names:
            static const boost::regex author_separator("(?:\\s*,(?!\\s*(?:inc|ltd)\\b)\\s*|\\s+(,\\s*)?(and|&)\\s+)|by\\s+", boost::regex::perl | boost::regex::icase);
            boost::regex_token_iterator<std::string::const_iterator> atr(author_list.begin(), author_list.end(), author_separator, -1);
            boost::regex_token_iterator<std::string::const_iterator> eatr;
            while(atr != eatr)
            {
               // get the reformatted authors name:
               std::string name = format_authors_name(*atr);
               // add to list of authors for this file:
               if(name.size() && name[0] != '-')
               {
                  m_licence_data[i].authors.insert(name);
                  // add file to author index:
                  m_author_data[name].insert(p);
                  ++author_count;
               }
               ++atr;
            }
            ++cpy;
         }
      }
   }
   if(licence_count == 0)
      m_unknown_licences.insert(p);
   if(licence_count && !author_count)
      m_unknown_authors.insert(p);
}

