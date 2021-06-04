#!/usr/bin/python

#  Copyright (C) Vladimir Prus 2003. Permission to copy, use, modify, sell and
#  distribute this software is granted provided this copyright notice appears in
#  all copies. This software is provided "as is" without express or implied
#  warranty, and with no claim as to its suitability for any purpose.

#  Test that features with default values are always present 
#  in build properties of any target.

from BoostBuild import Tester, List

t = Tester()

# Declare *non-propagated* feature foo.
t.write("project-root.jam", """
import feature : feature ;

feature foo : on off ;
""")

# Note that '<foo>on' won't be propagated
# to 'd/l'.
t.write("Jamfile", """
exe hello : hello.cpp d//l ;
""")
t.write("hello.cpp", """
#ifdef _WIN32
__declspec(dllimport)
#endif
void foo();
int main()
{
   foo();
   return 1;
}

""")

t.write("d/Jamfile", """
lib l : l.cpp : <foo>on:<define>FOO ;
""")
t.write("d/l.cpp", """
#ifdef _WIN32
__declspec(dllexport)
#endif
#ifdef FOO
void foo() {}
#endif

""")

t.run_build_system()

t.expect_addition("bin/$toolset/debug/hello.exe")

t.cleanup()
