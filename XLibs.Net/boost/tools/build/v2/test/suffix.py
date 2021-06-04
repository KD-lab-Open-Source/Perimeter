#!/usr/bin/python

#  Copyright (C) Vladimir Prus 2003. Permission to copy, use, modify, sell and
#  distribute this software is granted provided this copyright notice appears in
#  all copies. This software is provided "as is" without express or implied
#  warranty, and with no claim as to its suitability for any purpose.

from BoostBuild import Tester, List

t = Tester()

#  Regression test: when staging V2 used to change suffixes on targets
#  corresponding to real files.
t.write("Jamfile", """ 
import type : register ;
register A : a1 a2 a3 ;

stage a : a.a3 ; 
""")

t.write("project-root.jam", "")
t.write("a.a3", "")

t.run_build_system()
t.expect_addition("a/a.a3");

# Regression test: we should be able to specify empty suffix for
# derived target type, even if base type has non-empty suffix.
t.write("a.cpp", "")

t.write("suffixes.jam", """ 
import type ;
import generators ;
import common ;

type.register First : first : ;
type.register Second : "" : First : main ;

generators.register-standard $(__name__).second : CPP : Second ;

rule second
{
    TOUCH on $(<) = [ common.file-creation-command ] ;
}

actions second
{
    $(TOUCH) $(<)
}

""")

t.write("project-root.jam", """ 
import suffixes ; 
""")

t.write("Jamfile", """ 
second a : a.cpp ; 
""")

t.run_build_system()
t.expect_addition("bin/$toolset/debug/a")


t.cleanup()
