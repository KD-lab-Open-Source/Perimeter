#!/usr/bin/python

from BoostBuild import Tester, List
import os
from string import strip

t = Tester()

# First check some startup
t.set_tree("direct-request-test")
t.run_build_system(extra_args="define=MACROS")

t.expect_addition("bin/$toolset/debug/" 
                  * (List("a.obj b.obj b.dll a.exe")))

# When building debug version, the 'define' still applies
t.rm("bin")
t.run_build_system(extra_args="debug define=MACROS")
t.expect_addition("bin/$toolset/debug/" 
                  * (List("a.obj b.obj b.dll a.exe")))

# When building release version, the 'define' should not
# apply: we'll have direct build request 'release <define>MACROS'
# and real build properties 'debug'.
t.copy("Jamfile2", "Jamfile")
t.copy("b_inverse.cpp", "b.cpp")
t.rm("bin")
t.run_build_system(extra_args="release define=MACROS")


# Regression test: direct build request was not working
# when there's more than one level of 'build-project'

t.rm(".")
t.write('project-root.jam', '')
t.write('Jamfile', 'build-project a ;')
t.write('a/Jamfile', 'build-project b ;')
t.write('a/b/Jamfile', '')

t.run_build_system("release")





t.cleanup()
