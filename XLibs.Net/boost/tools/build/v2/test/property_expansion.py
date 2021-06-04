#!/usr/bin/python

#  Copyright (C) Vladimir Prus 2003. Permission to copy, use, modify, sell and
#  distribute this software is granted provided this copyright notice appears in
#  all copies. This software is provided "as is" without express or implied
#  warranty, and with no claim as to its suitability for any purpose.

from BoostBuild import Tester, List

# Test that free property inside 
t = Tester()

t.write("project-root.jam", "")
t.write("Jamfile", """
project ;

variant debug-AA : debug : <define>AA ;

alias all : hello ;
exe hello : hello.cpp ;
explicit hello ;
""")
t.write("hello.cpp", """
#ifdef AA
int main()
{
    return 0;
}
#endif
""")

t.run_build_system("debug-AA")


t.cleanup()
