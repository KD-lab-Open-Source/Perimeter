#!/usr/bin/python

#  Copyright (C) Vladimir Prus 2003. Permission to copy, use, modify, sell and
#  distribute this software is granted provided this copyright notice appears in
#  all copies. This software is provided "as is" without express or implied
#  warranty, and with no claim as to its suitability for any purpose.


from BoostBuild import Tester, List


t = Tester()

# Attempt to declare a generator for creating OBJ from RC files.
# That generator should be considered together with standard
# CPP->OBJ generators and successfully create the target.
# Since we don't have RC compiler everywhere, we fake the action.
# The resulting OBJ will be unusable, but it must be created.

t.write("project-root.jam", """ 
import rc ; 
""")

t.write("rc.jam", """ 
import type ;
import generators ;
import print ;

type.register RC : rc ;

rule resource-compile ( targets * : sources * : properties * )
{
    print.output $(targets[1]) ;
    print.text "rc-object" ;
}

generators.register-standard rc.resource-compile : RC : OBJ ;

""")

t.write("Jamfile", """ 
exe hello : hello.cpp r.rc ; 
""")

t.write("hello.cpp", """ 
int main()
{
    return 0;
} 
""")

t.write("r.rc", """ 
""")

t.run_build_system(status=1, stderr=None)
t.expect_content("bin/$toolset/debug/r.obj", "rc-object\n")

t.cleanup()



