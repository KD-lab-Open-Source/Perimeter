#!/usr/bin/python

#  Copyright (C) Vladimir Prus 2003. Permission to copy, use, modify, sell and
#  distribute this software is granted provided this copyright notice appears in
#  all copies. This software is provided "as is" without express or implied
#  warranty, and with no claim as to its suitability for any purpose.

from BoostBuild import Tester, List

t = Tester()

t.write("project-root.jam", "")
t.write("Jamfile", """ 
alias everything : [ exe a : a.cpp ] ; 
""")

t.write("a.cpp", """ 
int main()
{
    return 0;
}

""")

t.run_build_system()
t.expect_addition("bin/$toolset/debug/everything..a.exe")
t.rm("bin/$toolset/debug/everything..a.exe")

t.run_build_system("everything..a")
t.expect_addition("bin/$toolset/debug/everything..a.exe")

t.rm("bin")

# Now check that inline targets with the same name but
# present in different places are not confused between
# each other, and with top-level targets.
t.write("Jamfile", """
exe a : a.cpp ;
alias everything : [ exe a : a.cpp ] ;
alias everything2 : [ exe a : a.cpp ] ; 
""")

t.run_build_system()
t.expect_addition("bin/$toolset/debug/a.exe")
t.expect_addition("bin/$toolset/debug/everything..a.exe")
t.expect_addition("bin/$toolset/debug/everything2..a.exe")


t.cleanup()

