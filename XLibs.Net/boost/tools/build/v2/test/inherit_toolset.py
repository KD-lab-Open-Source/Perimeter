#!/usr/bin/python

#  Copyright (C) Vladimir Prus 2003. Permission to copy, use, modify, sell and
#  distribute this software is granted provided this copyright notice appears in
#  all copies. This software is provided "as is" without express or implied
#  warranty, and with no claim as to its suitability for any purpose.

from BoostBuild import Tester, List
from string import find

t = Tester(pass_toolset=0)

t.write("a.cpp", """ 
""")

t.write("yfc1.jam", """ 
import toolset ;
import generators ;

toolset.register yfc1 ;

rule init ( )
{
}

generators.register-standard yfc1.compile : CPP : OBJ : <toolset>yfc1 ;
generators.register-standard yfc1.link : OBJ : EXE : <toolset>yfc1 ;

actions compile
{
    yfc1-compile 
}

actions link
{
    yfc1-link
}


""")

t.write("yfc2.jam", """ 
import toolset ;

toolset.register yfc2 ;
toolset.inherit yfc2 : yfc1 ;

rule init ( )
{
}

actions link
{
    yfc2-link
} 
""")

t.write("Jamfile", """ 
exe a : a.cpp ; 
""")

t.write("project-root.jam", """
using yfc1 ;
""")

t.run_build_system("-n -d2 yfc1")
t.fail_test(find(t.stdout(), "yfc1-link") == -1)

# Make sure we don't have to explicit 'use' yfc1.
t.write("project-root.jam", """
using yfc2 ;
""")

t.run_build_system("-n -d2 yfc2")
t.fail_test(find(t.stdout(), "yfc2-link") == -1)

t.cleanup()

