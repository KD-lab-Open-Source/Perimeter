#!/usr/bin/python

# Test conditional properties

from BoostBuild import Tester, List
import os
from string import strip

t = Tester()

# Arrange a project which will build only if
# 'a.cpp' is compiled with "STATIC" define.
t.write("project-root.jam", "import gcc ;")
t.write("a.cpp", """
#ifdef STATIC
int main() {  return 0; }
#endif
""")
t.write("Jamfile", "exe a : a.cpp : <link>static:<define>STATIC ;")
t.run_build_system("link=static")
t.expect_addition("bin/$toolset/debug/link-static/a.exe")

t.write("Jamfile", """
project : requirements <link>static:<define>STATIC ;
exe a : a.cpp ;
""")
t.rm("bin")
t.run_build_system("link=static")
t.expect_addition("bin/$toolset/debug/link-static/a.exe")

# Regression test for a bug found by Ali Azarbayejani.
# Conditionals inside usage requirement were not evaluated.
# This breaks 

t.write("Jamfile", """
lib l : l.cpp : : : <link>static:<define>STATIC ;
exe a : a.cpp l ;
""")
t.write("l.cpp", "")

t.rm("bin")
t.run_build_system("link=static")
t.expect_addition("bin/$toolset/debug/link-static/a.exe")



t.cleanup()
