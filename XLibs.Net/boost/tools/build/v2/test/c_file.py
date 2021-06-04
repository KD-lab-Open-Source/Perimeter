#!/usr/bin/python

#  Copyright (C) Vladimir Prus 2003. Permission to copy, use, modify, sell and
#  distribute this software is granted provided this copyright notice appears in
#  all copies. This software is provided "as is" without express or implied
#  warranty, and with no claim as to its suitability for any purpose.

#  Test that C files are compiled by C compiler
from BoostBuild import Tester, List

t = Tester()

t.write("project-root.jam", "")
t.write("Jamfile", """
project ;

exe hello : hello.cpp a.c ;
""")
t.write("hello.cpp", """
extern "C" int foo();
int main() { return foo(); }
""")
t.write("a.c", """
// This won't compile unless in C mode
int foo()
{
    int new = 0;
    new = (new+1)*7;
    return new;
}
""")
t.run_build_system()
t.expect_addition("bin/$toolset/debug/hello.exe")

t.cleanup()
