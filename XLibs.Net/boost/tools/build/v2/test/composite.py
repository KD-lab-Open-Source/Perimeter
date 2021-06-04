#!/usr/bin/python

#  Copyright (C) Vladimir Prus 2003. Permission to copy, use, modify, sell and
#  distribute this software is granted provided this copyright notice appears in
#  all copies. This software is provided "as is" without express or implied
#  warranty, and with no claim as to its suitability for any purpose.

#  Test that composite properties are handled correctly.
from BoostBuild import Tester, List

t = Tester()

t.write("project-root.jam", "")
t.write("Jamfile", """
exe hello : hello.cpp : <variant>release ;
""")
t.write("hello.cpp", """
int main()
{
    return 0;
}

""")

t.run_build_system()

t.expect_addition("bin/$toolset/release/hello.exe")


t.cleanup()
