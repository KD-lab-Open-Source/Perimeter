#!/usr/bin/python

#  Copyright (C) Vladimir Prus 2003. Permission to copy, use, modify, sell and
#  distribute this software is granted provided this copyright notice appears in
#  all copies. This software is provided "as is" without express or implied
#  warranty, and with no claim as to its suitability for any purpose.

#  Test for the regression testing framework.
from BoostBuild import Tester, List

# Create a temporary working directory
t = Tester()

t.write("c.cpp", "")

t.write("r.cpp", """

void helper();

#include <iostream>
int main(int ac, char* av[])
{
    helper();

    for (int i = 1; i < ac; ++i)
       std::cout << av[i] << '\\n';
    return 0;
} 
""")

t.write("c-f.cpp", """ 
int 
""")

t.write("r-f.cpp", """ 
int main()
{
    return 1;
} 
""")


t.write("Jamfile", """ 
import testing ;

compile c.cpp ;
compile-fail c-f.cpp ;
run r.cpp libs//helper : foo bar ;
run-fail r-f.cpp ;

""")

t.write("libs/Jamfile", """
lib helper : helper.cpp ;
""")

t.write("libs/helper.cpp", """
void
#if defined(_WIN32)
__declspec(dllexport)
#endif
helper() {}

""")

t.write("project-root.jam", "")

# First test that when outcomes are expected, all .test files are created.
t.run_build_system("hardcode-dll-paths=true", stderr=None, status=None)
t.expect_addition("bin/c.test/$toolset/debug/c.test")
t.expect_addition("bin/c-f.test/$toolset/debug/c-f.test")
t.expect_addition("bin/r.test/$toolset/debug/r.test")
t.expect_addition("bin/r-f.test/$toolset/debug/r-f.test")

# Make sure args are handled.
t.expect_content("bin/r.test/$toolset/debug/r.output",
                 "foo\nbar\n")

# Test that input file is handled as well.
t.write("r.cpp", """
#include <iostream>
#include <fstream>
int main(int ac, char* av[])
{
    for (int i = 1; i < ac; ++i) {
        std::ifstream ifs(av[i]);
        std::cout << ifs.rdbuf();
    }

    return 0;
} 
""")

t.write("dir/input.txt", "test input")

t.write("Jamfile", """ 
import testing ;

compile c.cpp ;
compile-fail c-f.cpp ;
run r.cpp : : dir/input.txt ;
run-fail r-f.cpp ;

""")

t.run_build_system("hardcode-dll-paths=true")
t.expect_content("bin/r.test/$toolset/debug/r.output",
                 "test input")

# Make sure test failures are detected. Reverse expectation and see
# if .test files are created or not.
t.write("Jamfile", """ 
import testing ;

compile-fail c.cpp ;
compile c-f.cpp ;
run-fail r.cpp : : dir/input.txt ;
run r-f.cpp ;

""")

t.touch(List("c.cpp c-f.cpp r.cpp r-f.cpp"))

t.run_build_system("hardcode-dll-paths=true", stderr=None, status=1)
t.expect_removal("bin/c.test/$toolset/debug/c.test")
t.expect_removal("bin/c-f.test/$toolset/debug/c-f.test")
t.expect_removal("bin/r.test/$toolset/debug/r.test")
t.expect_removal("bin/r-f.test/$toolset/debug/r-f.test")

t.cleanup()
