#!/usr/bin/python

# This tests the "existing" and "updated" modifiers on actions.

import BoostBuild
from string import strip, replace

t = BoostBuild.Tester(pass_toolset=0)

code = """

DEPENDS all : a ;
ALWAYS a ;
NOTFILE a ;

actions existing make-a
{
    echo $(>) > list
}
make-a a : a-1 a-2 a-3 ;
DEPENDS a : a-1 a-2 a-3 ;
NOCARE a-1 a-2 ;

actions make-a3
{
   echo foo > $(<)
}
make-a3 a-3 ;
"""

t.write("file.jam", code)
t.write("a-1", "")

t.run_build_system("-ffile.jam")
t.fail_test(strip(t.read("list")) != "a-1")
t.rm(["a-3", "list"])

code = replace(code, "existing", "updated")
t.write("file.jam", code)
t.run_build_system("-ffile.jam")
t.fail_test(strip(t.read("list")) != "a-3")

code = replace(code, "updated", "existing updated")
t.write("file.jam", code)
t.run_build_system("-ffile.jam")
t.fail_test(strip(t.read("list")) != "a-1 a-3")









t.cleanup()
