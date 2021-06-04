#!/usr/bin/python

#  Copyright (C) Vladimir Prus 2003. Permission to copy, use, modify, sell and
#  distribute this software is granted provided this copyright notice appears in
#  all copies. This software is provided "as is" without express or implied
#  warranty, and with no claim as to its suitability for any purpose.

from BoostBuild import Tester, List
from string import find

t = Tester()


t.write("project-root.jam", "")
t.write("Jamfile", """ 

lib main : main.cpp l ;
lib l : l.cpp main ; 
""")

t.write("main.cpp", "")
t.write("l.cpp", "")

t.run_build_system("--no-error-backtrace", status=1)
t.fail_test(find(t.stdout(), "error: Recursion in main target references") == -1)
t.fail_test(find(t.stdout(), "./main ./l ./main") == -1)

t.cleanup()
