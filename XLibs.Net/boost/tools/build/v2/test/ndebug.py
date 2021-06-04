#!/usr/bin/python

#  Copyright (C) Vladimir Prus 2003. Permission to copy, use, modify, sell and
#  distribute this software is granted provided this copyright notice appears in
#  all copies. This software is provided "as is" without express or implied
#  warranty, and with no claim as to its suitability for any purpose.

#  Test that building with optimization brings NDEBUG define, and, more
#  importantly, that dependency targets are built with NDEBUG as well,
#  even if they are not directly requested.


from BoostBuild import Tester, List


t = Tester()

t.write("project-root.jam", "")
t.write("Jamfile", """
exe hello : hello.cpp lib//lib1 ;
""")
t.write("hello.cpp", """
#ifdef NDEBUG
void foo();
int main()
{
    foo();
    return 0;
}
#endif
""")
t.write("lib/Jamfile", """
lib lib1 : lib1.cpp ;        
""")
t.write("lib/lib1.cpp", """
#ifdef NDEBUG
void foo() {}
#endif
""")

# 'release' builds should get NDEBUG define
# use static linking to avoid messing with
# imports/exports on windows.
t.run_build_system("link=static release")


t.cleanup()
