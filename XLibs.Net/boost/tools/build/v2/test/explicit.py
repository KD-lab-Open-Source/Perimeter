#!/usr/bin/python

#  Copyright (C) Vladimir Prus 2003. Permission to copy, use, modify, sell and
#  distribute this software is granted provided this copyright notice appears in
#  all copies. This software is provided "as is" without express or implied
#  warranty, and with no claim as to its suitability for any purpose.

#  This file is template for Boost.Build tests. It creates a simple
#  project that builds one exe from one source, and checks that the exe
#  is really created.
from BoostBuild import Tester, List

t = Tester()

t.write("project-root.jam", "")
t.write("Jamfile", """

exe hello : hello.cpp ;
exe hello2 : hello.cpp ;

explicit hello2 ;
""")
t.write("hello.cpp", """
int main()
{
    return 0;
}

""")

t.run_build_system()
t.ignore("*.tds")
t.expect_addition(List("bin/$toolset/debug/hello") * [".exe", ".obj"])
t.expect_nothing_more()

t.run_build_system("hello2")
t.expect_addition("bin/$toolset/debug/hello2.exe")


t.cleanup()
