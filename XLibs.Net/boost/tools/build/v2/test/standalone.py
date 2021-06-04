#!/usr/bin/python

#  Copyright (C) Vladimir Prus 2003. Permission to copy, use, modify, sell and
#  distribute this software is granted provided this copyright notice appears in
#  all copies. This software is provided "as is" without express or implied
#  warranty, and with no claim as to its suitability for any purpose.


from BoostBuild import Tester, List

t = Tester()

# Regression tests: standalone project were not able to refer to targets
# declared in themselfs!

t.write("a.cpp", """ 
int main()
{
    return 0;
}

""")

t.write("project-root.jam", """ 
import standalone ; 
""")

t.write("standalone.jam", """ 
import project ;

project.initialize $(__name__) ;
project standalone ;

local pwd = [ PWD ] ;

alias x : $(pwd)/../a.cpp ;
alias runtime : x ;

""")

t.write("sub/Jamfile", """ 
stage bin : /standalone//runtime ; 
""")

t.run_build_system(subdir="sub")
t.expect_addition("sub/bin/a.cpp")
t.cleanup()

