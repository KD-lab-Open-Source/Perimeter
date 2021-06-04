#!/usr/bin/python

#  Copyright (C) Vladimir Prus 2003. Permission to copy, use, modify, sell and
#  distribute this software is granted provided this copyright notice appears in
#  all copies. This software is provided "as is" without express or implied
#  warranty, and with no claim as to its suitability for any purpose.

#  Test that the user can define his own rule that will call builtin main
#  target rule and that this will work.

from BoostBuild import Tester, List

t = Tester()

t.write("Jamfile", """ my-test : test.cpp ;


""")

t.write("test.cpp", """ 
int main()
{
    return 0;
}

""")

t.write("project-root.jam", """ using testing ;

rule my-test ( name ? : sources + )
{
    {
        name ?= test ;
        unit-test $(name) : $(sources) ; # /site-config//cppunit /util//testMain ;
    }   
}

IMPORT $(__name__) : my-test : : my-test ;


""")

t.run_build_system()
t.expect_addition("bin/$toolset/debug/test.passed")

t.cleanup()

