#!/usr/bin/python

#  Copyright (C) Vladimir Prus 2003. Permission to copy, use, modify, sell and
#  distribute this software is granted provided this copyright notice appears in
#  all copies. This software is provided "as is" without express or implied
#  warranty, and with no claim as to its suitability for any purpose.

#  Test the unit_test rule
from BoostBuild import Tester, List

t = Tester()

# Create the needed files
t.write("project-root.jam", """
using testing ;
""")
t.write("Jamfile", """
lib helper : helper.cpp ;
unit-test test : test.cpp : <source>helper ;
""")
t.write("test.cpp", """
void helper();
int main()
{
    helper();
    return 0;
}
""")

t.write("helper.cpp", """
void
#if defined(_WIN32)
__declspec(dllexport)
#endif
helper() {}
""")

t.run_build_system("link=static")
t.expect_addition("bin/$toolset/debug/link-static/test.passed")




t.cleanup()
