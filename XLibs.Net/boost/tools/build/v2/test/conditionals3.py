#!/usr/bin/python

#  Copyright (C) Vladimir Prus 2003. Permission to copy, use, modify, sell and
#  distribute this software is granted provided this copyright notice appears in
#  all copies. This software is provided "as is" without express or implied
#  warranty, and with no claim as to its suitability for any purpose.

#  Test that conditional properties work, even if property is free, and
#  value includes colon.
from BoostBuild import Tester, List


t = Tester()

# Create the needed files
t.write("project-root.jam", "")
t.write("Jamfile", """
exe hello : hello.cpp : <variant>debug:<define>CLASS=Foo::Bar ;
""")
t.write("hello.cpp", """
namespace Foo { class Bar { } ; }
int main()
{
    CLASS c;
    return 0;
}

""")

# Don't check stderr, which can include warning about unused 'c'.
t.run_build_system(stdout=None, stderr=None)
t.expect_addition("bin/$toolset/debug/hello.exe")

t.cleanup()
