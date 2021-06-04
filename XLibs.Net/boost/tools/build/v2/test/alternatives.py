#!/usr/bin/python

# Test main target alternatives.

from BoostBuild import Tester
from string import find
t = Tester()


# Test that basic alternatives selection works.
t.write("project-root.jam", " ")
t.write("Jamfile", """

exe a : a_empty.cpp ;
exe a : a.cpp : <variant>release ;
""")
t.write("a_empty.cpp", "")
t.write("a.cpp", "int main() { return 0; }\n")

t.run_build_system("release")
t.expect_addition("bin/$toolset/release/a.exe")

# Test that alternative selection works for ordinary
# properties, in particular user-defined.
t.write("project-root.jam", " ")
t.write("Jamfile", """

import feature ;
feature.feature X : off on : propagated ;

exe a : b.cpp ;
exe a : a.cpp : <X>on ;
""")
t.write("b.cpp", "int main() { return 0; }\n")

t.rm("bin")

t.run_build_system()
t.expect_addition("bin/$toolset/debug/b.obj")

t.run_build_system("X=on")
t.expect_addition("bin/$toolset/debug/X-on/a.obj")

t.rm("bin")

# Test that everything works ok even with default
# build.

t.write("Jamfile", """

exe a : a_empty.cpp : <variant>release ;
exe a : a.cpp : <variant>debug ;
""")

t.run_build_system()
t.expect_addition("bin/$toolset/debug/a.exe")

# Test that only properties which are in build request
# matters when selection alternative. IOW, alternative
# with <variant>release is better than one with
# <variant>debug when building release version.
t.write("Jamfile", """

exe a : a_empty.cpp : <variant>debug ;
exe a : a.cpp : <variant>release ;
""")

t.run_build_system("release")
t.expect_addition("bin/$toolset/release/a.exe")

# Test that free properties do not matter. We really don't
# want <cxxflags> property in build request to affect
# alternative selection.
t.write("Jamfile", """
exe a : a_empty.cpp : <variant>debug <define>FOO <include>BAR ;
exe a : a.cpp : <variant>release ;
""")

t.rm("bin/$toolset/release/a.exe")
t.run_build_system("release define=FOO")
t.expect_addition("bin/$toolset/release/a.exe")

# Test that abibuity is reported correctly
t.write("Jamfile", """
exe a : a_empty.cpp ;
exe a : a.cpp ;
""")
t.run_build_system("--no-error-backtrace", status=1)
t.fail_test(find(t.stdout(), "because no best-matching alternative could be found") == -1)

# Another ambiguity test: two matches properties in one alternative are
# neither better nor worse than a single one in another alternative.
t.write("Jamfile", """
exe a : a_empty.cpp : <optimization>off <profiling>off ;
exe a : a.cpp : <debug-symbols>on ;
""")

t.run_build_system("--no-error-backtrace", status=1)
t.fail_test(find(t.stdout(), "because no best-matching alternative could be found") == -1)



# Test that we can have alternative without sources
t.write("Jamfile", """
alias specific-sources ;
import feature ;
feature.extend os : MAGIC ;
alias specific-sources : b.cpp : <os>MAGIC ;
exe a : a.cpp specific-sources ;
""")
t.rm("bin")
t.run_build_system()

           
t.cleanup()        
