#!/usr/bin/python

# Test that default build clause actually has any effect.

from BoostBuild import Tester, List
t = Tester()

t.write("project-root.jam", "import gcc ;")
t.write("Jamfile", "exe a : a.cpp : : debug release ;")
t.write("a.cpp", "int main() { return 0; }\n")

t.run_build_system()
t.expect_addition("bin/$toolset/debug/a.exe")
t.expect_addition("bin/$toolset/release/a.exe")

# Check that explictly-specified build variant supresses
# default-build
t.rm("bin")
t.run_build_system("release")
t.expect_addition(List("bin/$toolset/release/") * "a.exe a.obj")
t.expect_nothing_more()

# Now check that we can specify explicit build request and
# default-build will be combined with it
t.run_build_system("optimization=space")
t.expect_addition("bin/$toolset/debug/optimization-space/a.exe")
t.expect_addition("bin/$toolset/release/optimization-space/a.exe")

# Test that default-build must be identical in all alternatives. Error case.
t.write("Jamfile", """
exe a : a.cpp : : debug ;
exe a : b.cpp : : ;
""")
expected="""error: default build must be identical in all alternatives
main target is ./a
with
differing from previous default build <variant>debug

"""
t.run_build_system("-n --no-error-backtrace", status=1, stdout=expected)

# Test that default-build must be identical in all alternatives. No Error case, empty default build.
t.write("Jamfile", """
exe a : a.cpp : <variant>debug ;
exe a : b.cpp : <variant>release ;
""")
t.run_build_system("-n --no-error-backtrace", status=0)


# Now try a harder example: default build which contains <define>
# should cause <define> to be present when "b" is compiled.
# This happens only of "build-project b" is placed first.
t.write("Jamfile", """
    project 
        : default-build <define>FOO 
    ;

    build-project a ;
    build-project b ;   
""")

t.write("a/Jamfile", """
    exe a : a.cpp ../b//b ;
""")
t.write("a/a.cpp", """
#ifdef _WIN32
__declspec(dllimport)
#endif
void foo();
int main() { foo(); return 0; }
""")

t.write("b/Jamfile", """
    lib b : b.cpp ;
""")
t.write("b/b.cpp", """
#ifdef FOO
#ifdef _WIN32
__declspec(dllexport)
#endif
void foo() {}
#endif
""")

t.run_build_system()

t.cleanup()
