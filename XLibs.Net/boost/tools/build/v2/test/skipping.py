#!/usr/bin/python

#  Copyright (C) Vladimir Prus 2003. Permission to copy, use, modify, sell and
#  distribute this software is granted provided this copyright notice appears in
#  all copies. This software is provided "as is" without express or implied
#  warranty, and with no claim as to its suitability for any purpose.

#  Test that V2 does not fail gracelessy when any target is skipped.
from BoostBuild import Tester, List

# Create a temporary working directory
t = Tester()
t.write("a.cpp", """ 
int main() {} 
""")

t.write("b.cpp", """ 
int main() {} 
""")

t.write("c.cpp", """ 
int main() {} 
""")

t.write("Jamfile", """ 
import feature : feature ;

feature foo : 1 2 : link-incompatible ;

exe a : a.cpp : <foo>1 ;
exe b : b.cpp : <foo>2 ;
exe c : c.cpp ; 
""")

t.write("project-root.jam", """ 
""")

t.run_build_system("foo=1")

t.cleanup()

