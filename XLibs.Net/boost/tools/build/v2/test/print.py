#!/usr/bin/python

from BoostBuild import Tester, List

t = Tester()

t.write("project-root.jam", "import gcc ;")
t.write("Jamfile", """
import print ;
print.output foo ;
print.text \\\"Something\\\" ;
DEPENDS all : foo ;
ALWAYS foo ;
""")

t.run_build_system()
t.expect_content("foo", """\"Something\"
""")

t.write("Jamfile", """
import print ;
print.output foo ;
print.text \\\"Somethingelse\\\" ;
DEPENDS all : foo ;
ALWAYS foo ;
""")

t.run_build_system()
t.expect_content("foo", """\"Something\"
\"Somethingelse\"
""")

t.write("Jamfile", """
import print ;
print.output foo ;
print.text \\\"Different\\\" : true ;
DEPENDS all : foo ;
ALWAYS foo ;
""")

t.run_build_system()
t.expect_content("foo", """\"Different\"
""")

t.cleanup()
