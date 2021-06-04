#!/usr/bin/python

# Test the 'make' rule

from BoostBuild import Tester
from string import find

t = Tester(pass_toolset=1)

t.write("project-root.jam", "")
t.write("Jamfile", """

rule creator ( string targets * : sources * : * )
{
    STRING on $(targets) = $(string) ;
    creator2 $(targets) : $(sources) ;
}

actions creator2
{
    echo $(STRING) > $(<)
}

make foo.bar : : creator foobar ;
""")

t.run_build_system()
t.expect_addition("bin/$toolset/debug/foo.bar")
t.fail_test(find(t.read("bin/$toolset/debug/foo.bar"), "foobar") == -1)

# Regression test. Make sure that if main target requested two times,
# and build request differ only in incidental properties, the main target
# if created only once. The bug was discovered by Kirill Lapshin.

t.write("Jamfile", """ 
# Make sure that incidental property does not
# cause second creation of 'hello1.cpp'.
exe a : dir//hello1.cpp ;
exe b : dir//hello1.cpp/<hardcode-dll-paths>true ; 
""")

t.write("project-root.jam", "")

t.write("dir/Jamfile", """ 
import common ;
make hello1.cpp : hello.cpp : common.copy ;

""")

t.write("dir/hello.cpp", """
int main()
{
    return 1;
}
""")
t.run_build_system("-d2")
t.fail_test(t.stdout().count("common.copy") != 1)

t.cleanup()
