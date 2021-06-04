#!/usr/bin/python

#  Copyright (C) Vladimir Prus 2003. Permission to copy, use, modify, sell and
#  distribute this software is granted provided this copyright notice appears in
#  all copies. This software is provided "as is" without express or implied
#  warranty, and with no claim as to its suitability for any purpose.


#  Regression test: when directory of project root contained regex metacharacters,
#  Boost.Build failed to work. Bug reported by Michael Stevens

from BoostBuild import Tester, List

t = Tester()

t.write("bad[abc]dirname/Jamfile", """ 
""")

t.write("bad[abc]dirname/project-root.jam", """ 
""")

t.run_build_system(subdir="bad[abc]dirname")

t.cleanup()

