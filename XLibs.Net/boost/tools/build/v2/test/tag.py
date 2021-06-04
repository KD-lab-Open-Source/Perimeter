#!/usr/bin/python

#  Copyright (C) Pedro Ferreira 2003. Permission to copy, use, modify, sell and
#  distribute this software is granted provided this copyright notice appears in
#  all copies. This software is provided "as is" without express or implied
#  warranty, and with no claim as to its suitability for any purpose.

from BoostBuild import Tester, List

t = Tester()

t.write("project-root.jam", "")
t.write("Jamfile", """ 
local tags = <variant>debug:<tag>_d <variant>release:<tag>_r <variant>profile:<tag>_p <link>shared:<tag>s <link>static:<tag>t ;
exe a : a.cpp : $(tags) ;
lib b : a.cpp : $(tags) ;
stage c : a ;
""")

t.write("a.cpp", """ 
int main()
{
    return 0;
}

#ifdef _MSC_VER
__declspec (dllexport) void x () {} 
#endif
""")

file_list = \
List("bin/$toolset/debug/a_ds.exe") + \
List("bin/$toolset/debug/b_ds.dll") + \
List("c/a_ds.exe") + \
List("bin/$toolset/release/a_rs.exe") + \
List("bin/$toolset/release/b_rs.dll") + \
List("c/a_rs.exe") + \
List("bin/$toolset/profile/a_ps.exe") + \
List("bin/$toolset/profile/b_ps.dll") + \
List("c/a_ps.exe") + \
List("bin/$toolset/debug/link-static/a_dt.exe") + \
List("bin/$toolset/debug/link-static/b_dt.lib") + \
List("c/a_dt.exe") + \
List("bin/$toolset/release/link-static/a_rt.exe") + \
List("bin/$toolset/release/link-static/b_rt.lib") + \
List("c/a_rt.exe") + \
List("bin/$toolset/profile/link-static/a_pt.exe") + \
List("bin/$toolset/profile/link-static/b_pt.lib") + \
List("c/a_pt.exe") \

variants = "debug release profile link=static,shared"

t.run_build_system(variants)
t.expect_addition(file_list)

t.run_build_system(variants + " clean")
t.expect_removal(file_list)

t.cleanup()

