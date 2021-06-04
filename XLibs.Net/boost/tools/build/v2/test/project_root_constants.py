#!/usr/bin/python

#  Copyright (C) Vladimir Prus 2003. Permission to copy, use, modify, sell and
#  distribute this software is granted provided this copyright notice appears in
#  all copies. This software is provided "as is" without express or implied
#  warranty, and with no claim as to its suitability for any purpose.

from BoostBuild import Tester, List
from string import find

# Create a temporary working directory
t = Tester()

# Create the needed files
t.write("project-root.jam", """
constant FOO : foobar ;
ECHO $(FOO) ;
""")
t.write("Jamfile", """
""")

t.run_build_system()
t.fail_test(find(t.stdout(), "foobar") == -1)

# Regression test: when absolute paths were passed to path-constant rule,
# Boost.Build failed to recognize path as absolute and prepended current dir.
t.write("project-root.jam", """
import path ;
local here = [ path.native [ path.pwd ] ] ;
path-constant HERE : $(here) ;
if $(HERE) != $(here) 
{
    ECHO "PWD           =" $(here) ;
    ECHO "path constant =" $(HERE) ;
    EXIT ;
}
""")
t.write("Jamfile", "")

t.run_build_system()


t.cleanup()
