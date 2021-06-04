#!/usr/bin/python

#  Copyright (C) Vladimir Prus 2003. Permission to copy, use, modify, sell and
#  distribute this software is granted provided this copyright notice appears in
#  all copies. This software is provided "as is" without express or implied
#  warranty, and with no claim as to its suitability for any purpose.

from BoostBuild import Tester, List


t = Tester(pass_toolset=0)

# Test that 

t.write("code", """
module a {
    rule r1 ( )
    {
        ECHO R1 ;
    }
}
module a2 {
    rule r2 ( )
    {
        ECHO R2 ;
    }
}
IMPORT a2 : r2 : : a2.r2 ;

module b {
    IMPORT_MODULE a : b ;

    rule test
    {
        # Call rule visible via IMPORT_MODULE
        a.r1 ;
        # Call rule in global scope
        a2.r2 ;
    }
}

IMPORT b : test : : test ;
test ;

module c {
    rule test
    {
        ECHO CTEST ;
    }
}    

IMPORT_MODULE c : ;
c.test ;

actions do-nothing { }
do-nothing all ;

""")

t.run_build_system("-fcode", stdout="""R1
R2
CTEST
""")

t.cleanup()
