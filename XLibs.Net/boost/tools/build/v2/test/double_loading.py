#!/usr/bin/python

#  Copyright (C) Vladimir Prus 2003. Permission to copy, use, modify, sell and
#  distribute this software is granted provided this copyright notice appears in
#  all copies. This software is provided "as is" without express or implied
#  warranty, and with no claim as to its suitability for any purpose.

from BoostBuild import Tester, List
import string

t = Tester()

#  Regression test for double loading of the same Jamfile.
t.write("Jamfile", """ 
build-project subdir ; 
""")

t.write("project-root.jam", """ 
""")

t.write("subdir/Jamfile", """ 
ECHO "Loaded subdir" ; 
""")

t.run_build_system(subdir="subdir")
t.fail_test(string.count(t.stdout(), "Loaded subdir") != 1)

#  Regression test for a more contrived case. The top-level
#  jamfile refers to subdir via use-project, while subdir's
#  Jamfile is being loaded. The motivation why use-project
#  referring to subprojects are usefull can be found at
#  http://article.gmane.org/gmane.comp.lib.boost.build/3906/
t.write("Jamfile", """ 
use-project /subdir : subdir ; 
""")

t.write("project-root.jam", """ 
""")

t.write("subdir/Jamfile", """ 
project subdir ; 
""")

t.run_build_system(subdir="subdir");

t.cleanup()

