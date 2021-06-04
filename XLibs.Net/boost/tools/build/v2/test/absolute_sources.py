#!/usr/bin/python

# Test that sources with absolute names are handled OK.

from BoostBuild import Tester
t = Tester()

t.write("project-root.jam", """
path-constant TOP : . ;
""")
t.write("Jamfile", """
local pwd = [ PWD ] ;
ECHO $(pwd) XXXXX ;
exe hello : $(pwd)/hello.cpp $(TOP)/empty.cpp ;
""")
t.write("hello.cpp", "int main() { return 0; }\n")
t.write("empty.cpp", "\n")

t.run_build_system()
t.expect_addition("bin/$toolset/debug/hello.exe")

# Test a contrived case. There, absolute name is used in
# standalone project (not Jamfile). Moreover, the target with
# absolute name is returned by 'alias' and used from other project.
t.write("a.cpp", """ 
int main()
{
    return 0;
}

""")

t.write("Jamfile", """ 
exe a : /standalone//a ; 
""")

t.write("project-root.jam", """ 
import standalone ; 
""")

t.write("standalone.jam", """ 
import project ;

project.initialize $(__name__) ;
project standalone ;

local pwd = [ PWD ] ;

alias a : $(pwd)/a.cpp ;

""")
t.run_build_system()
t.expect_addition("bin/$toolset/debug/a.exe")

t.cleanup()
