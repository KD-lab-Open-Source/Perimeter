#!/usr/bin/python

from BoostBuild import Tester
t = Tester()

# Test that use requirements on main target work
t.write("project-root.jam", "import gcc ;")

t.write(
    "Jamfile",
"""
    lib b : b.cpp : <link>shared:<define>SHARED_B
    : : <define>FOO <link>shared:<define>SHARED_B
    ;
    exe a : a.cpp b ;
""")

t.write(
    "b.cpp",
"""
void
#if defined(_WIN32) && defined(SHARED_B)
__declspec(dllexport)
#endif
foo() {}\n
""")

t.write(
    "a.cpp",
"""
#ifdef FOO
void
# if defined(_WIN32) && defined(SHARED_B)
__declspec(dllexport)
# endif
foo() {}
#endif

int main() { foo(); }
""")

t.run_build_system()

t.run_build_system("--clean")

# Test that use requirements on main target work, when they are referred using
# 'dependency' features.
t.write("project-root.jam", "import gcc ;")

t.write(
    "Jamfile", 
"""
    lib b : b.cpp : <link>shared:<define>SHARED_B
    : : <define>FOO <link>shared:<define>SHARED_B
    ;
    exe a : a.cpp : <use>b ;
""")

t.write(
    "b.cpp",
"""
void
#if defined(_WIN32) && defined(SHARED_B)
__declspec(dllexport)
#endif
foo() {}
""")

t.write(
    "a.cpp",
"""
#ifdef FOO
int main() { return 0; }
#endif
""")

t.run_build_system()

t.run_build_system("--clean")


# Test that use requirement on project work
t.write("Jamfile", "exe a : a.cpp lib//b ;")

t.write(
    "lib/Jamfile", 
"""
project
   : requirements <link>shared:<define>SHARED_B
   : usage-requirements <define>FOO <link>shared:<define>SHARED_B
    ;
lib b : b.cpp ;
""")

t.write(
    "lib/b.cpp", 
"""
void
#if defined(_WIN32) && defined(SHARED_B)
__declspec(dllexport)
#endif
foo() {}\n
""")

t.run_build_system()

# Test that use requirements are inherited correctly

t.write("Jamfile", "exe a : a.cpp lib/1//b ;")

t.write(
    "a.cpp", 
"""
#if defined(FOO) && defined(ZOO)
void foo() {}
#endif

int main() { foo(); }
""")

t.write(
    "lib/Jamfile", 
"""
project
   : requirements
   : usage-requirements <define>FOO
    ;
""")

t.write(
    "lib/1/Jamfile", 
"""
project
   : requirements <link>shared:<define>SHARED_B
   : usage-requirements <define>ZOO <link>shared:<define>SHARED_B 
   ;
lib b : b.cpp ;
""")

t.write(
    "lib/1/b.cpp", 
"""
void
#if defined(_WIN32) && defined(SHARED_B)
__declspec(dllexport)
#endif
foo() {}\n
""")

t.run_build_system()
t.run_build_system("--clean")

# Test that we correctly handle dependency features
# in use requirements on target

t.write(
    "Jamfile", 
"""
    lib b : b.cpp : <link>shared:<define>SHARED_B 
    : : <define>FOO <link>shared:<define>SHARED_B
    ;
    
    # Here's the test: we should correctly
    # handle dependency feature and get
    # use requirements from 'b'.
    lib c : c.cpp : <link>shared:<define>SHARED_C : : <library>b ;
    
    # This will build only if <define>FOO
    # was propagated from 'c'.
    exe a : a.cpp c ;
""")

t.write(
    "a.cpp", 
"""
#ifdef FOO
void
# if defined(_WIN32) && defined(SHARED_B)
__declspec(dllexport)
# endif
foo();
#endif

int main() { foo(); }
""")

t.write(
    "c.cpp",
"""
int 
#if defined(_WIN32) && defined(SHARED_C)
__declspec(dllexport)
#endif
must_export_something;
""")

t.run_build_system()
t.run_build_system("--clean")

# Test correct handling of dependency features in 
# project requirements.
t.write(
    "Jamfile",
"""
    exe a : a.cpp lib1//c ;
""")

t.write(
    "lib1/Jamfile",
"""
    project
    : requirements <link>shared:<define>SHARED_C
    : usage-requirements <library>../lib2//b <link>shared:<define>SHARED_C
    ;
    
    lib c : c.cpp ;    
""")

t.write(
    "lib1/c.cpp", 
"""
int 
#if defined(_WIN32) && defined(SHARED_C)
__declspec(dllexport)
#endif
must_export_something;
""")

t.write(
    "lib2/Jamfile", 
"""
    lib b : b.cpp : <link>shared:<define>SHARED_B
    : : <define>FOO <link>shared:<define>SHARED_B ;
""")

t.copy("b.cpp", "lib2/b.cpp")

t.run_build_system()

# Test that dependency feature in use requirements are built
# with the correct properties
t.rm(".")

t.write(
    "Jamfile", 
""" 
lib main : main.cpp : <use>libs//lib1 : : <library>libs//lib1 ; 
exe hello : hello.cpp main : ;
""")

t.write(
    "main.cpp", 
""" 
void
#if defined(_WIN32) && defined(SHARED_LIB1)
__declspec(dllimport)
#endif
foo();

int main() { foo(); return 0; } 
""")

t.write("hello.cpp", "\n")

t.write(
    "project-root.jam",
""" 
import gcc ; 
""")

t.write(
    "libs/a.cpp", 
""" 
void
#if defined(_WIN32) && defined(SHARED_LIB1)
__declspec(dllexport)
#endif
foo() {} 
""")

# This library should be build with the same properties as
# 'main'. There were a bug when they were generated with 
# empty properties, and there were ambiguity between variants.
t.write(
    "libs/Jamfile", 
""" 
lib lib1 : a_d.cpp : <variant>debug <link>shared:<define>SHARED_LIB1
: : <link>shared:<define>SHARED_LIB1 ;

lib lib1 : a.cpp : <variant>release <link>shared:<define>SHARED_LIB1
: : <link>shared:<define>SHARED_LIB1 ; 
""")

t.write(
    "libs/a_d.cpp", 
""" 
void
#if defined(_WIN32) && defined(SHARED_LIB1)
__declspec(dllexport)
#endif
foo() {} 
""")

t.run_build_system("link=static")
t.expect_addition("libs/bin/$toolset/debug/link-static/a_d.obj")

t.cleanup()
