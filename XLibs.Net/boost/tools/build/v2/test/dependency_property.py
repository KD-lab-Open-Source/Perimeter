#!/usr/bin/python

#  Copyright (C) Vladimir Prus 2003. Permission to copy, use, modify, sell and
#  distribute this software is granted provided this copyright notice appears in
#  all copies. This software is provided "as is" without express or implied
#  warranty, and with no claim as to its suitability for any purpose.

# Regression test: virtual targets with different dependency properties
# were considered different by 'virtual-target.register', but the code
# which determined target paths ignored dependency properties --- so both
# targets used to be placed to the same location.

from BoostBuild import Tester, List
from string import find


t = Tester()

t.write("project-root.jam", "")
t.write("Jamfile", """
lib foo : foo.cpp ;
exe hello : hello.cpp ;
exe hello2 : hello.cpp : <library>foo ;
""")
t.write("hello.cpp", """
int main()
{
    return 0;
}

""")
t.write("foo.cpp", """
#ifdef _WIN32
__declspec(dllexport)
#endif
void foo() {}
""")

t.run_build_system("--no-error-backtrace", status=1)
t.fail_test(find(t.stdout(), "Duplicate name of actual target") == -1)

t.cleanup()
