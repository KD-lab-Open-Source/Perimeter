/*
 *
 * Copyright (c) 2003 Dr John Maddock
 * Use, modification and distribution is subject to the 
 * Boost Software License, Version 1.0. (See accompanying file 
 * LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 *
 */

#include "licence_info.hpp"


std::pair<const licence_info*, int> get_licences()
{
   static const char* generic_author_sig = 
      "(?:"
         "(?:"
            "Copyright|\\(c\\)|©"
         ")[[:blank:]]+"
      "){1,2}"
      "(?:"
         "\\d[^[:alpha:]]+"
            "([[:alpha:]]"
               "(?:"
                  "(?!Use\\b|Permission\\b|All\\b"
               ")[^\\n\\d]"
            ")+"
         ")"
         "|"
            "([[:alpha:]][^\\n\\d]+"
               "(?:\\n[^\\n\\d]+"
            ")??"
         ")(?:19|20)\\d{2}"
      ")"
      "|"
      "Authors:[[:blank:]]+"
         "([[:alpha:]][^\\n\\d]+"
      ")";

   static const char* generic_author_format = 
      "(?1$1)(?2$2)(?3$3)";

   static const licence_info licences[] = 
   {
      licence_info( boost::regex("Use\\W+modification\\W+and\\W+distribution\\W+(?:is|are)\\W+subject\\W+to\\W+the\\W+"
         "Boost\\W+Software\\W+License\\W+Version\\W+1.0", boost::regex::perl | boost::regex::icase)
         ,
         boost::regex(generic_author_sig, boost::regex::perl | boost::regex::icase)
         ,
         generic_author_format
         ,
         "Boost Software License, Version 1.0"
         ,
         "<P>Copyright (c) <I>Date</I> <I>Author</I></P>"
         "<P>Use, modification and distribution is subject to the "
         "Boost Software License, Version 1.0. (See accompanying file "
         "LICENSE_1_0.txt or copy at <a href=\"http://www.boost.org/LICENSE_1_0.txt\">http://www.boost.org/LICENSE_1_0.txt)</a></P>"
       )
      ,
      licence_info( boost::regex("Permission\\W+to\\W+use\\W+copy\\W+modify\\W+distribute\\W+and\\W+sell\\W+this\\W+software\\W+and\\W+its\\W+documentation"
         "\\W+for\\W+any\\W+purpose\\W+is\\W+hereby\\W+granted\\W+without\\W+fee"
         "\\W+provided\\W+that\\W+the\\W+above\\W+copyright\\W+notice\\W+appears?\\W+in\\W+all\\W+copies\\W+and"
         "\\W+that\\W+both\\W+(the|that)\\W+copyright\\W+notice\\W+and\\W+this\\W+permission\\W+notice\\W+appears?"
         "\\W+in\\W+supporting\\W+documentation.{1, 100}\\W+no\\W+representations"
         "\\W+(are\\W+made\\W+)?about\\W+the\\W+suitability\\W+of\\W+this\\W+software\\W+for\\W+any\\W+purpose"
         "\\W+It\\W+is\\W+provided\\W+as\\W+is\\W+without\\W+express\\W+or\\W+implied\\W+warranty"
         "|preprocessed\\W+version\\W+of\\W+boost/mpl/.*\\.hpp\\W+header\\W+see\\W+the\\W+original\\W+for\\W+copyright\\W+information", boost::regex::perl | boost::regex::icase)
         ,
         boost::regex(generic_author_sig, boost::regex::perl | boost::regex::icase)
         ,
         generic_author_format
         ,
         "SGI Style Licence"
         ,
         "<P>Copyright (c) <I>Date</I><BR>"
         "<I>Author</I><BR>"
         "<BR>"
         "Permission to use, copy, modify, distribute and sell this software "
         "and its documentation for any purpose is hereby granted without fee, "
         "provided that the above copyright notice appear in all copies and "
         "that both that copyright notice and this permission notice appear "
         "in supporting documentation.  <I>Author</I> makes no representations "
         "about the suitability of this software for any purpose. "
         "It is provided \"as is\" without express or implied warranty.</P>"
       )
      ,
      licence_info( boost::regex("Permission\\W+to\\W+use\\W+copy\\W+modify\\W+distribute\\W+and\\W+sell\\W+this\\W+software"
         "\\W+for\\W+any\\W+purpose\\W+is\\W+hereby\\W+granted\\W+without\\W+fee"
         "\\W+provided\\W+that\\W+the\\W+above\\W+copyright\\W+notice\\W+appears?\\W+in\\W+all\\W+copies\\W+and"
         "\\W+that\\W+both\\W+(the|that)\\W+copyright\\W+notice\\W+and\\W+this\\W+permission\\W+notice\\W+appears?"
         "\\W+in\\W+supporting\\W+documentation.{1, 100}\\W+no\\W+representations"
         "\\W+(are\\W+made\\W+)?about\\W+the\\W+suitability\\W+of\\W+this\\W+software\\W+for\\W+any\\W+purpose"
         "\\W+It\\W+is\\W+provided\\W+as\\W+is\\W+without\\W+express(ed)?\\W+or\\W+implied\\W+warranty", boost::regex::perl | boost::regex::icase)
         ,
         boost::regex(generic_author_sig, boost::regex::perl | boost::regex::icase)
         ,
         generic_author_format
         ,
         "Old style Boost licence #1"
         ,
         "<P>Copyright (c) <I>Date</I><BR>"
         "<I>Author</I><BR>"
         "<BR>"
         "Permission to use, copy, modify, distribute and sell this software "
         "for any purpose is hereby granted without fee, "
         "provided that the above copyright notice appear in all copies and "
         "that both that copyright notice and this permission notice appears? "
         "in supporting documentation.  <I>Author</I> makes no representations "
         "about the suitability of this software for any purpose. "
         "It is provided \"as is\" without express or implied warranty.</P>"
       )
      ,
      licence_info(
         boost::regex(
            "Permission\\W+to\\W+copy\\W+use\\W+modify\\W+sell\\W+and\\W+distribute\\W+this\\W+software"
            "\\W+is\\W+granted\\W+provided\\W+this\\W+copyright\\W+notice\\W+appears\\W+in\\W+all\\W+copies"
            "\\W+This\\W+software\\W+is\\W+provided\\W+as\\W+is\\W+without\\W+express\\W+or\\W+implied"
            "\\W+warranty\\W+and\\W+with\\W+no\\W+claim\\W+as\\W+to\\W+its\\W+suitability\\W+for\\W+any\\W+purpose"
            , boost::regex::perl | boost::regex::icase
         )
         ,
         boost::regex(generic_author_sig, boost::regex::perl | boost::regex::icase)
         ,
         generic_author_format
         ,
         "Old style Boost licence #2"
         ,
         "<P>Copyright (c) <I>Date</I> <I>Author</I>.<BR><BR>\n"
         "Permission to copy, use, modify, sell and distribute this software<BR>\n"
         "is granted provided this copyright notice appears in all copies.<BR>\n"
         "This software is provided \"as is\" without express or implied<BR>\n"
         "warranty, and with no claim as to its suitability for any purpose.</P>\n"
       )
      ,
      licence_info(
         boost::regex(
            "Permission\\W+to\\W+copy\\W+use\\W+modify\\W+sell\\W+and\\W+distribute\\W+this\\W+software\\W+is\\W+granted\\W+provided"
            "\\W+this\\W+copyright\\W+notice\\W+appears\\W+in\\W+all\\W+copies\\W+This\\W+software\\W+is"
            "\\W+provided\\W+as\\W+is\\W+without\\W+express\\W+or\\W+implied\\W+warranty\\W+and\\W+with"
            "\\W+no\\W+claim\\W+at\\W+to\\W+its\\W+suitability\\W+for\\W+any\\W+purpose"
            , boost::regex::perl | boost::regex::icase
         )
         ,
         boost::regex(generic_author_sig, boost::regex::perl | boost::regex::icase)
         ,
         generic_author_format
         ,
         "Old style Boost licence #3"
         ,
         "<P>(C) Copyright <I>Author</I> <I>Date</I>.  Permission to copy, use, "
         "modify, sell, and distribute this software is granted provided "
         "this copyright notice appears in all copies.  This software is "
         "provided \"as is\" without express or implied warranty, and with "
         "no claim at to its suitability for any purpose.</p>\n"
       )
      ,
      licence_info( boost::regex("Permission\\W+to\\W+copy\\W+use\\W+sell\\W+and\\W+distribute\\W+this\\W+software\\W+is\\W+granted"
                     "\\W+provided\\W+this\\W+copyright\\W+notice\\W+appears\\W+in\\W+all\\W+copies"
                     "\\W+Permission\\W+to\\W+modify\\W+the\\W+code\\W+and\\W+to\\W+distribute\\W+modified\\W+code\\W+is\\W+granted"
                     "\\W+provided\\W+this\\W+copyright\\W+notice\\W+appears\\W+in\\W+all\\W+copies\\W+and\\W+a\\W+notice"
                     "\\W+that\\W+the\\W+code\\W+was\\W+modified\\W+is\\W+included\\W+with\\W+the\\W+copyright\\W+notice"
                     , boost::regex::perl | boost::regex::icase)
         ,
         boost::regex(generic_author_sig, boost::regex::perl | boost::regex::icase)
         ,
         generic_author_format
         ,
         "Old style Boost licence #4"
         ,
         "<P>Copyright (C) <I>Date Author</I><BR>"
         "<BR>"
         "Permission to copy, use, sell and distribute this software is granted\n"
         "provided this copyright notice appears in all copies.\n"
         "Permission to modify the code and to distribute modified code is granted\n"
         "provided this copyright notice appears in all copies, and a notice\n"
         "that the code was modified is included with the copyright notice.</P>\n"
         "<P>This software is provided \"as is\" without express or implied warranty,\n"
         "and with no claim as to its suitability for any purpose.</P>"
       )
      ,
      licence_info( boost::regex("This\\W+file\\W+is\\W+part\\W+of\\W+the\\W+(Boost\\W+Graph|Generic\\W+Graph\\W+Component)\\W+Library"
                     "\\W+You\\W+should\\W+have\\W+received\\W+a\\W+copy\\W+of\\W+the\\W+License\\W+Agreement\\W+for\\W+the"
                     "\\W+(Boost|Generic)\\W+Graph\\W+(Component\\W+)?Library\\W+along\\W+with\\W+the\\W+software;\\W+see\\W+the\\W+file\\W+LICENSE"
                     "(\\W+If\\W+not\\W+contact\\W+Office\\W+of\\W+Research\\W+University\\W+of\\W+Notre\\W+Dame\\W+Notre"
                     "\\W+Dame\\W+IN\\W+46556)?"
                     "\\W+Permission\\W+to\\W+modify\\W+the\\W+code\\W+and\\W+to\\W+distribute\\W+modified\\W+code\\W+is"
                     "\\W+granted\\W+provided\\W+the\\W+text\\W+of\\W+this\\W+NOTICE\\W+is\\W+retained\\W+a\\W+notice\\W+that"
                     "\\W+the\\W+code\\W+was\\W+modified\\W+is\\W+included\\W+with\\W+the\\W+above\\W+COPYRIGHT\\W+NOTICE\\W+and"
                     "\\W+with\\W+the\\W+COPYRIGHT\\W+NOTICE\\W+in\\W+the\\W+LICENSE\\W+file\\W+and\\W+that\\W+the\\W+LICENSE"
                     "\\W+file\\W+is\\W+distributed\\W+with\\W+the\\W+modified\\W+code\\W+"
                     "\\W+LICENSOR\\W+MAKES\\W+NO\\W+REPRESENTATIONS\\W+OR\\W+WARRANTIES\\W+EXPRESS\\W+OR\\W+IMPLIED"
                     "\\W+By\\W+way\\W+of\\W+example\\W+but\\W+not\\W+limitation\\W+Licensor\\W+MAKES\\W+NO"
                     "\\W+REPRESENTATIONS\\W+OR\\W+WARRANTIES\\W+OF\\W+MERCHANTABILITY\\W+OR\\W+FITNESS\\W+FOR\\W+ANY"
                     "\\W+PARTICULAR\\W+PURPOSE\\W+OR\\W+THAT\\W+THE\\W+USE\\W+OF\\W+THE\\W+LICENSED\\W+SOFTWARE\\W+COMPONENTS"
                     "\\W+OR\\W+DOCUMENTATION\\W+WILL\\W+NOT\\W+INFRINGE\\W+ANY\\W+PATENTS\\W+COPYRIGHTS\\W+TRADEMARKS"
                     "\\W+OR\\W+OTHER\\W+RIGHTS"
            , boost::regex::perl | boost::regex::icase)
         ,
         boost::regex(generic_author_sig, boost::regex::perl | boost::regex::icase)
         ,
         generic_author_format
         ,
         "Boost.Graph licence"
         ,
         "<P>Copyright <I>Date</I> University of Notre Dame.<BR>"
         "Authors: Andrew Lumsdaine, Lie-Quan Lee, Jeremy G. Siek</P>"
         "<P>This file is part of the Boost Graph Library</P>"
         "<P>You should have received a copy of the <A href=\"http://www.boost.org/libs/graph/LICENCE\">License Agreement</a> for the "
         "Boost Graph Library along with the software; see the file <A href=\"http://www.boost.org/libs/graph/LICENCE\">LICENSE</a>. "
         "If not, contact Office of Research, University of Notre Dame, Notre "
         "Dame, IN 46556.</P>"
         "<P>Permission to modify the code and to distribute modified code is "
         "granted, provided the text of this NOTICE is retained, a notice that "
         "the code was modified is included with the above COPYRIGHT NOTICE and "
         "with the COPYRIGHT NOTICE in the <A href=\"http://www.boost.org/libs/graph/LICENCE\">LICENSE</a> file, and that the <A href=\"http://www.boost.org/libs/graph/LICENCE\">LICENSE</a> "
         "file is distributed with the modified code.</P>"
         "<P>LICENSOR MAKES NO REPRESENTATIONS OR WARRANTIES, EXPRESS OR IMPLIED.<BR> "
         "By way of example, but not limitation, Licensor MAKES NO "
         "REPRESENTATIONS OR WARRANTIES OF MERCHANTABILITY OR FITNESS FOR ANY "
         "PARTICULAR PURPOSE OR THAT THE USE OF THE LICENSED SOFTWARE COMPONENTS "
         "OR DOCUMENTATION WILL NOT INFRINGE ANY PATENTS, COPYRIGHTS, TRADEMARKS "
         "OR OTHER RIGHTS.</P>"
       )
      ,
      licence_info( boost::regex("Permission\\W+to\\W+copy\\W+use\\W+modify\\W+sell\\W+and\\W+distribute\\W+this\\W+software\\W+is"
                     "\\W+granted\\W+provided\\W+this\\W+copyright\\W+notice\\W+appears\\W+in\\W+all\\W+copies\\W+and"
                     "\\W+modified\\W+version\\W+are\\W+clearly\\W+marked\\W+as\\W+such\\W+This\\W+software\\W+is\\W+provided"
                     "\\W+as\\W+is\\W+without\\W+express\\W+or\\W+implied\\W+warranty\\W+and\\W+with\\W+no\\W+claim\\W+as\\W+to\\W+its"
                     "\\W+suitability\\W+for\\W+any\\W+purpose"
                     , boost::regex::perl | boost::regex::icase)
         ,
         boost::regex(generic_author_sig, boost::regex::perl | boost::regex::icase)
         ,
         generic_author_format
         ,
         "Old style Boost licence #5"
         ,
         "<P>Copyright (C) <I>Date Author</I></P>"
         "<p>Permission to copy, use, modify, sell and distribute this software is "
         "granted, provided this copyright notice appears in all copies and "
         "modified version are clearly marked as such. This software is provided "
         "\"as is\" without express or implied warranty, and with no claim as to its "
         "suitability for any purpose.</P>"
       )
      ,
      licence_info( boost::regex("This\\W+file\\W+can\\W+be\\W+redistributed\\W+and\\W+or\\W+modified\\W+under\\W+the\\W+terms\\W+found"
                     "\\W+in\\W+copyright\\W+html"
                     "\\W+This\\W+software\\W+and\\W+its\\W+documentation\\W+is\\W+provided\\W+as\\W+is\\W+without\\W+express\\W+or"
                     "\\W+implied\\W+warranty\\W+and\\W+with\\W+no\\W+claim\\W+as\\W+to\\W+its\\W+suitability\\W+for\\W+any\\W+purpose"
            , boost::regex::perl | boost::regex::icase)
         ,
         boost::regex(generic_author_sig, boost::regex::perl | boost::regex::icase)
         ,
         generic_author_format
         ,
         "Boost.Pool licence"
         ,
         "<P>This file can be redistributed and/or modified under the terms found "
         "in <a href=\"http://www.boost.org/libs/pool/doc/copyright.html\">copyright.html</a></P>\n"
         "<P>This software and its documentation is provided \"as is\" without express or "
         "implied warranty, and with no claim as to its suitability for any purpose</P>"
       )
      ,
      licence_info(boost::regex("Permission\\W+to\\W+use\\W+copy\\W+modify\\W+sell\\W+and\\W+distribute\\W+this\\W+software"
                     "\\W+is\\W+hereby\\W+granted\\W+without\\W+fee\\W+provided\\W+that\\W+the\\W+above\\W+copyright\\W+notice"
                     "\\W+appears\\W+in\\W+all\\W+copies\\W+and\\W+that\\W+both\\W+that\\W+copyright\\W+notice\\W+and\\W+this"
                     "\\W+permission\\W+notice\\W+appear\\W+in\\W+supporting\\W+documentation"
                     ".{1,100}\\W+(make\\W+any\\W+representation|makes\\W+no\\W+representations)\\W+about\\W+the\\W+suitability\\W+of\\W+this"
                     "\\W+software\\W+for\\W+any\\W+purpose\\W+It\\W+is\\W+provided\\W+as\\W+is\\W+without\\W+express\\W+or"
                     "\\W+implied\\W+warranty"
                     , boost::regex::perl | boost::regex::icase)
         ,
         boost::regex(generic_author_sig, boost::regex::perl | boost::regex::icase)
         ,
         generic_author_format
         ,
         "Old style Boost licence #6"
         ,
         "<P>Copyright <I>Author Data</I></P>"
         "<P>Permission to use, copy, modify, sell, and distribute this software "
         "is hereby granted without fee provided that the above copyright notice "
         "appears in all copies and that both that copyright notice and this "
         "permission notice appear in supporting documentation, "
         "<I>Author</I> makes no representations about the suitability of this "
         "software for any purpose. It is provided \"as is\" without express or "
         "implied warranty.</P>"
       )
      ,
      licence_info( boost::regex("Permission\\W+to\\W+copy"
                     "\\W+use\\W+modify\\W+sell\\W+and\\W+distribute\\W+this\\W+software\\W+is\\W+granted\\W+provided"
                     "\\W+this\\W+copyright\\W+notice\\W+appears\\W+in\\W+all\\W+copies\\W+of\\W+the\\W+source\\W+This"
                     "\\W+software\\W+is\\W+provided\\W+as\\W+is\\W+without\\W+express\\W+or\\W+implied\\W+warranty"
                     "\\W+and\\W+with\\W+no\\W+claim\\W+as\\W+to\\W+its\\W+suitability\\W+for\\W+any\\W+purpose"
                     , boost::regex::perl | boost::regex::icase)
         ,
         boost::regex(generic_author_sig, boost::regex::perl | boost::regex::icase)
         ,
         generic_author_format
         ,
         "Old style Boost licence #7"
         ,
         "<P>Copyright <I>Author Date</I>. Permission to copy, "
         "use, modify, sell and distribute this software is granted provided "
         "this copyright notice appears in all copies of the source. This "
         "software is provided \"as is\" without express or implied warranty, "
         "and with no claim as to its suitability for any purpose."
       )
      ,
      licence_info(boost::regex("This\\W+software\\W+is\\W+provided\\W+as-is\\W+without\\W+any\\W+express\\W+or\\W+implied"
      "\\W+warranty\\W+In\\W+no\\W+event\\W+will\\W+the\\W+copyright\\W+holder\\W+be\\W+held\\W+liable\\W+for"
      "\\W+any\\W+damages\\W+arising\\W+from\\W+the\\W+use\\W+of\\W+this\\W+software"
      "\\W+Permission\\W+is\\W+granted\\W+to\\W+anyone\\W+to\\W+use\\W+this\\W+software\\W+for\\W+any\\W+purpose"
      "\\W+including\\W+commercial\\W+applications\\W+and\\W+to\\W+alter\\W+it\\W+and\\W+redistribute"
      "\\W+it\\W+freely\\W+subject\\W+to\\W+the\\W+following\\W+restrictions:"
      "\\W+1\\W+The\\W+origin\\W+of\\W+this\\W+software\\W+must\\W+not\\W+be\\W+misrepresented;\\W+you\\W+must"
      "\\W+not\\W+claim\\W+that\\W+you\\W+wrote\\W+the\\W+original\\W+software\\W+If\\W+you\\W+use\\W+this"
      "\\W+software\\W+in\\W+a\\W+product\\W+an\\W+acknowledgment\\W+in\\W+the\\W+product\\W+documentation"
      "\\W+would\\W+be\\W+appreciated\\W+but\\W+is\\W+not\\W+required"
      "\\W+2\\W+Altered\\W+source\\W+versions\\W+must\\W+be\\W+plainly\\W+marked\\W+as\\W+such\\W+and\\W+must"
      "\\W+not\\W+be\\W+misrepresented\\W+as\\W+being\\W+the\\W+original\\W+software"
      "\\W+3\\W+This\\W+notice\\W+may\\W+not\\W+be\\W+removed\\W+or\\W+altered\\W+from\\W+any\\W+source"
      "\\W+distribution"
                     , boost::regex::perl | boost::regex::icase)
         ,
         boost::regex(generic_author_sig, boost::regex::perl | boost::regex::icase)
         ,
         generic_author_format
         ,
         "Old style Boost licence #8"
         ,
         "<P>Phoenix V0.9<BR>Copyright (c) <I>Date</I> Joel de Guzman</P>"
         "<P>This software is provided 'as-is', without any express or implied "
         "warranty. In no event will the copyright holder be held liable for "
         "any damages arising from the use of this software.</P>"
         "<P>Permission is granted to anyone to use this software for any purpose, "
         "including commercial applications, and to alter it and redistribute "
         "it freely, subject to the following restrictions:</P>"
         "<P>1.  The origin of this software must not be misrepresented; you must "
         "not claim that you wrote the original software. If you use this "
         "software in a product, an acknowledgment in the product documentation "
         "would be appreciated but is not required.</P>"
         "2.  Altered source versions must be plainly marked as such, and must "
         "not be misrepresented as being the original software. </P>"
         "<P>3.  This notice may not be removed or altered from any source "
         "distribution. "
       )
      ,
      licence_info( boost::regex("Permission\\W+to\\W+use\\W+copy\\W+modify\\W+sell\\W+and\\W+distribute\\W+this\\W+software"
                     "\\W+is\\W+hereby\\W+granted\\W+without\\W+fee\\W+provided\\W+that\\W+the\\W+above\\W+copyright\\W+notice"
                     "\\W+appears\\W+in\\W+all\\W+copies\\W+and\\W+that\\W+both\\W+that\\W+copyright\\W+notice\\W+and\\W+this"
                     "\\W+permission\\W+notice\\W+appear\\W+in\\W+supporting\\W+documentation"
                     "\\W+None\\W+of\\W+the\\W+above\\W+authors\\W+nor.{1,100}make\\W+any"
                     "\\W+representation\\W+about\\W+the\\W+suitability\\W+of\\W+this\\W+software\\W+for\\W+any"
                     "\\W+purpose\\W+It\\W+is\\W+provided\\W+as\\W+is\\W+without\\W+express\\W+or\\W+implied\\W+warranty"
                     , boost::regex::perl | boost::regex::icase)
         ,
         boost::regex(generic_author_sig, boost::regex::perl | boost::regex::icase)
         ,
         generic_author_format
         ,
         "Old style Boost licence #9"
         ,
         "<P>Copyright <I> Author Date</I><BR>"
         "Permission to use, copy, modify, sell, and distribute this software "
         "is hereby granted without fee provided that the above copyright notice "
         "appears in all copies and that both that copyright notice and this "
         "permission notice appear in supporting documentation, <BR>"
         "None of the above authors nor <I>Author's Organisation</I> make any "
         "representation about the suitability of this software for any "
         "purpose. It is provided \"as is\" without express or implied warranty."
       )
      ,
      licence_info( boost::regex("Permission\\W+to\\W+use\\W+copy\\W+modify\\W+and\\W+distribute\\W+this\\W+software\\W+for\\W+any"
                     "\\W+purpose\\W+is\\W+hereby\\W+granted\\W+without\\W+fee\\W+provided\\W+that\\W+this\\W+copyright\\W+and"
                     "\\W+permissions\\W+notice\\W+appear\\W+in\\W+all\\W+copies\\W+and\\W+derivatives"
                     "\\W+This\\W+software\\W+is\\W+provided\\W+as\\W+is\\W+without\\W+express\\W+or\\W+implied\\W+warranty"
                     , boost::regex::perl | boost::regex::icase)
         ,
         boost::regex(generic_author_sig, boost::regex::perl | boost::regex::icase)
         ,
         generic_author_format
         ,
         "Old style Boost licence #10"
         ,
         "<P>Copyright <I>Author Date</I>. All rights reserved.</P>"
         "<P>Permission to use, copy, modify, and distribute this software for any "
         "purpose is hereby granted without fee, provided that this copyright and "
         "permissions notice appear in all copies and derivatives.</P>"
         "<P>This software is provided \"as is\" without express or implied warranty.</P>"
       )
      ,
      licence_info( boost::regex("This\\W+material\\W+is\\W+provided\\W+as\\W+is\\W+with\\W+absolutely\\W+no\\W+warranty\\W+expressed"
                     "\\W+or\\W+implied\\W+Any\\W+use\\W+is\\W+at\\W+your\\W+own\\W+risk"
                     "\\W+Permission\\W+to\\W+use\\W+or\\W+copy\\W+this\\W+software\\W+for\\W+any\\W+purpose\\W+is\\W+hereby\\W+granted"
                     "\\W+without\\W+fee\\W+provided\\W+the\\W+above\\W+notices\\W+are\\W+retained\\W+on\\W+all\\W+copies"
                     "\\W+Permission\\W+to\\W+modify\\W+the\\W+code\\W+and\\W+to\\W+distribute\\W+modified\\W+code\\W+is\\W+granted"
                     "\\W+provided\\W+the\\W+above\\W+notices\\W+are\\W+retained\\W+and\\W+a\\W+notice\\W+that\\W+the\\W+code\\W+was"
                     "\\W+modified\\W+is\\W+included\\W+with\\W+the\\W+above\\W+copyright\\W+notice"
                     , boost::regex::perl | boost::regex::icase)
         ,
         boost::regex(generic_author_sig, boost::regex::perl | boost::regex::icase)
         ,
         generic_author_format
         ,
         "Old style Boost licence #11"
         ,
         "<P>This material is provided \"as is\", with absolutely no warranty expressed "
         "or implied. Any use is at your own risk.</P>"
         "<P>Permission to use or copy this software for any purpose is hereby granted "
         "without fee, provided the above notices are retained on all copies. "
         "Permission to modify the code and to distribute modified code is granted, "
         "provided the above notices are retained, and a notice that the code was "
         "modified is included with the above copyright notice.</P>"
       )
      ,
      licence_info( boost::regex("Permission\\W+to\\W+copy\\W+use\\W+and\\W+distribute\\W+this\\W+software\\W+is\\W+granted\\W+provided"
                     "\\W+that\\W+this\\W+copyright\\W+notice\\W+appears\\W+in\\W+all\\W+copies"
                     "\\W+Permission\\W+to\\W+modify\\W+the\\W+code\\W+and\\W+to\\W+distribute\\W+modified\\W+code\\W+is\\W+granted"
                     "\\W+provided\\W+that\\W+this\\W+copyright\\W+notice\\W+appears\\W+in\\W+all\\W+copies\\W+and\\W+a\\W+notice"
                     "\\W+that\\W+the\\W+code\\W+was\\W+modified\\W+is\\W+included\\W+with\\W+the\\W+copyright\\W+notice"
                     "\\W+This\\W+software\\W+is\\W+provided\\W+as\\W+is\\W+without\\W+express\\W+or\\W+implied\\W+warranty\\W+and"
                     "\\W+with\\W+no\\W+claim\\W+as\\W+to\\W+its\\W+suitability\\W+for\\W+any\\W+purpose"
                     , boost::regex::perl | boost::regex::icase)
         ,
         boost::regex(generic_author_sig, boost::regex::perl | boost::regex::icase)
         ,
         generic_author_format
         ,
         "Old style Boost licence #12"
         ,
         "<P>Copyright (C) <I>Date Author</I></P><P>Permission to copy, use, and distribute this software is granted, provided "
         "that this copyright notice appears in all copies.<BR>"
         "Permission to modify the code and to distribute modified code is granted, "
         "provided that this copyright notice appears in all copies, and a notice "
         "that the code was modified is included with the copyright notice.</P>"
         "<P>This software is provided \"as is\" without express or implied warranty, and "
         "with no claim as to its suitability for any purpose.</P>"
       )
      ,
      licence_info( boost::regex("Permission\\W+to\\W+copy\\W+and\\W+use\\W+this\\W+software\\W+is\\W+granted"
                                 "\\W+provided\\W+this\\W+copyright\\W+notice\\W+appears\\W+in\\W+all\\W+copies"
                                 "\\W+Permission\\W+to\\W+modify\\W+the\\W+code\\W+and\\W+to\\W+distribute\\W+modified\\W+code\\W+is\\W+granted"
                                 "\\W+provided\\W+this\\W+copyright\\W+notice\\W+appears\\W+in\\W+all\\W+copies\\W+and\\W+a\\W+notice"
                                 "\\W+that\\W+the\\W+code\\W+was\\W+modified\\W+is\\W+included\\W+with\\W+the\\W+copyright\\W+notice"
                                 "\\W+This\\W+software\\W+is\\W+provided\\W+as\\W+is\\W+without\\W+express\\W+or\\W+implied\\W+warranty"
                                 "\\W+and\\W+with\\W+no\\W+claim\\W+as\\W+to\\W+its\\W+suitability\\W+for\\W+any\\W+purpose"
                     , boost::regex::perl | boost::regex::icase)
         ,
         boost::regex(generic_author_sig, boost::regex::perl | boost::regex::icase)
         ,
         generic_author_format
         ,
         "Old style Boost licence #13"
         ,
         "<P>Copyright (C) <I>Date Author</I></P>"
         "<P>Permission to copy and use this software is granted, "
         "provided this copyright notice appears in all copies. "
         "Permission to modify the code and to distribute modified code is granted, "
         "provided this copyright notice appears in all copies, and a notice "
         "that the code was modified is included with the copyright notice.</P>"
         "<P>This software is provided \"as is\" without express or implied warranty, "
         "and with no claim as to its suitability for any purpose.</P>"
       )
      ,
   };
   return std::pair<const licence_info*, int>(licences, sizeof(licences)/sizeof(licences[0]));
}

std::string format_authors_name(const std::string& name)
{
   // put name into a consistent format, so that we don't get too much
   // of a proliferation of names (lots of versions of the same basic form).

   static const boost::regex e("(^)?[^-(<a-zA-ZА-ю]+(([(<].*)?$)?");
   static const char* formatter = "(?1:(?2: ))";

   return boost::regex_replace(name, e, formatter, boost::match_default | boost::format_all);
}
