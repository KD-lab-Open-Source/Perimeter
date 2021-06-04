#!/usr/bin/python

#  Copyright (C) Vladimir Prus 2003. Permission to copy, use, modify, sell and
#  distribute this software is granted provided this copyright notice appears in
#  all copies. This software is provided "as is" without express or implied
#  warranty, and with no claim as to its suitability for any purpose.

#  Test that we can access project-root attributes from Jamfiles.
from BoostBuild import Tester, List

# Create a temporary working directory
t = Tester()

t.write("Jamfile", """
local l = [ project-root get-location ] ;
ECHO XXX $(l) ;
""")

t.write("project-root.jam", "")
        
t.run_build_system(stdout="XXX .\n")

t.cleanup()
