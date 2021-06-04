#!/usr/bin/python

# Test that unused sources are at least reported.

from BoostBuild import Tester
from string import find
t = Tester()

t.set_tree("unused")

t.run_build_system()
# The second invocation should do nothing, and produce
# no warning. The previous invocation might have printed
# executed actions and other things, so it's not easy
# to check if warning was issued or not.
t.run_build_system()
t.fail_test(find(t.stdout(), "warning: Unused source { b.X } in main target ./a") == -1)

t.run_build_system("-sGENERATE_ONLY_UNUSABLE=1")
t.fail_test(find(t.stdout(), "warning: Unused source { b.X } in main target ./a") == -1)

# Now check that even if main target generates nothing, its
# usage requirements are still propagated to dependents.
t.write("a.cpp","""
#ifdef FOO
int main() {}
#endif
""")
t.run_build_system("-sGENERATE_NOTHING=1")

t.cleanup()

