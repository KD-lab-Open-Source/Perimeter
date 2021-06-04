#
# auto generated makefile for VC6 compiler
#
# usage:
# make
#   brings libraries up to date
# make install
#   brings libraries up to date and copies binaries to your VC6 /lib and /bin directories (recomended)
#

#
# Add additional compiler options here:
#
CXXFLAGS=
#
# Add additional include directories here:
#
INCLUDES=
#
# add additional linker flags here:
#
XLFLAGS=
#
# add additional static-library creation flags here:
#
XSFLAGS=

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF "$(MSVCDIR)" == ""
!ERROR Variable MSVCDIR not set.
!ENDIF


ALL_HEADER= ../../../boost/regex/config.hpp ../../../boost/regex/pattern_except.hpp ../../../boost/regex/regex_traits.hpp ../../../boost/regex/user.hpp ../../../boost/regex/v3/cregex.hpp ../../../boost/regex/v3/fileiter.hpp ../../../boost/regex/v3/instances.hpp ../../../boost/regex/v3/regex.hpp ../../../boost/regex/v3/regex_compile.hpp ../../../boost/regex/v3/regex_cstring.hpp ../../../boost/regex/v3/regex_format.hpp ../../../boost/regex/v3/regex_fwd.hpp ../../../boost/regex/v3/regex_kmp.hpp ../../../boost/regex/v3/regex_library_include.hpp ../../../boost/regex/v3/regex_match.hpp ../../../boost/regex/v3/regex_raw_buffer.hpp ../../../boost/regex/v3/regex_split.hpp ../../../boost/regex/v3/regex_stack.hpp ../../../boost/regex/v3/regex_synch.hpp ../../../boost/regex/v3/regex_traits.hpp ../../../boost/regex/v4/basic_regex.hpp ../../../boost/regex/v4/char_regex_traits.hpp ../../../boost/regex/v4/cregex.hpp ../../../boost/regex/v4/fileiter.hpp ../../../boost/regex/v4/instances.hpp ../../../boost/regex/v4/iterator_category.hpp ../../../boost/regex/v4/iterator_traits.hpp ../../../boost/regex/v4/match_flags.hpp ../../../boost/regex/v4/match_results.hpp ../../../boost/regex/v4/mem_block_cache.hpp ../../../boost/regex/v4/perl_matcher.hpp ../../../boost/regex/v4/perl_matcher_common.hpp ../../../boost/regex/v4/perl_matcher_non_recursive.hpp ../../../boost/regex/v4/perl_matcher_recursive.hpp ../../../boost/regex/v4/regbase.hpp ../../../boost/regex/v4/regex.hpp ../../../boost/regex/v4/regex_compile.hpp ../../../boost/regex/v4/regex_cstring.hpp ../../../boost/regex/v4/regex_format.hpp ../../../boost/regex/v4/regex_fwd.hpp ../../../boost/regex/v4/regex_grep.hpp ../../../boost/regex/v4/regex_iterator.hpp ../../../boost/regex/v4/regex_kmp.hpp ../../../boost/regex/v4/regex_match.hpp ../../../boost/regex/v4/regex_merge.hpp ../../../boost/regex/v4/regex_raw_buffer.hpp ../../../boost/regex/v4/regex_replace.hpp ../../../boost/regex/v4/regex_search.hpp ../../../boost/regex/v4/regex_split.hpp ../../../boost/regex/v4/regex_stack.hpp ../../../boost/regex/v4/regex_synch.hpp ../../../boost/regex/v4/regex_token_iterator.hpp ../../../boost/regex/v4/regex_traits.hpp ../../../boost/regex/v4/states.hpp ../../../boost/regex/v4/sub_match.hpp ../../../boost/regex/config/borland.hpp ../../../boost/regex/config/cstring.hpp ../../../boost/regex/config/cwchar.hpp ../../../boost/regex/config/regex_library_include.hpp

all : main_dir libboost_regex-vc7-s-1_31_dir ./vc7/libboost_regex-vc7-s-1_31.lib libboost_regex-vc7-mt-s-1_31_dir ./vc7/libboost_regex-vc7-mt-s-1_31.lib libboost_regex-vc7-sgd-1_31_dir ./vc7/libboost_regex-vc7-sgd-1_31.lib libboost_regex-vc7-mt-sgd-1_31_dir ./vc7/libboost_regex-vc7-mt-sgd-1_31.lib boost_regex-vc7-mt-gd-1_31_dir ./vc7/boost_regex-vc7-mt-gd-1_31.lib boost_regex-vc7-mt-1_31_dir ./vc7/boost_regex-vc7-mt-1_31.lib libboost_regex-vc7-mt-1_31_dir ./vc7/libboost_regex-vc7-mt-1_31.lib libboost_regex-vc7-mt-gd-1_31_dir ./vc7/libboost_regex-vc7-mt-gd-1_31.lib

clean :  libboost_regex-vc7-s-1_31_clean libboost_regex-vc7-mt-s-1_31_clean libboost_regex-vc7-sgd-1_31_clean libboost_regex-vc7-mt-sgd-1_31_clean boost_regex-vc7-mt-gd-1_31_clean boost_regex-vc7-mt-1_31_clean libboost_regex-vc7-mt-1_31_clean libboost_regex-vc7-mt-gd-1_31_clean

install : all
	copy vc7\libboost_regex-vc7-s-1_31.lib "$(MSVCDIR)\lib"
	copy vc7\libboost_regex-vc7-mt-s-1_31.lib "$(MSVCDIR)\lib"
	copy vc7\libboost_regex-vc7-sgd-1_31.lib "$(MSVCDIR)\lib"
	copy vc7\libboost_regex-vc7-sgd-1_31.pdb "$(MSVCDIR)\lib"
	copy vc7\libboost_regex-vc7-mt-sgd-1_31.lib "$(MSVCDIR)\lib"
	copy vc7\libboost_regex-vc7-mt-sgd-1_31.pdb "$(MSVCDIR)\lib"
	copy vc7\boost_regex-vc7-mt-gd-1_31.lib "$(MSVCDIR)\lib"
	copy vc7\boost_regex-vc7-mt-gd-1_31.dll "$(MSVCDIR)\bin"
	copy vc7\boost_regex-vc7-mt-gd-1_31.pdb "$(MSVCDIR)\lib"
	copy vc7\boost_regex-vc7-mt-1_31.lib "$(MSVCDIR)\lib"
	copy vc7\boost_regex-vc7-mt-1_31.dll "$(MSVCDIR)\bin"
	copy vc7\libboost_regex-vc7-mt-1_31.lib "$(MSVCDIR)\lib"
	copy vc7\libboost_regex-vc7-mt-gd-1_31.lib "$(MSVCDIR)\lib"
	copy vc7\libboost_regex-vc7-mt-gd-1_31.pdb "$(MSVCDIR)\lib"

main_dir :
	@if not exist "vc7\$(NULL)" mkdir vc7


########################################################
#
# section for libboost_regex-vc7-s-1_31.lib
#
########################################################
vc7/libboost_regex-vc7-s-1_31/c_regex_traits.obj: ../src/c_regex_traits.cpp $(ALL_HEADER)
	cl $(INCLUDES) /c /nologo /ML /W3 /GX /O2 /GB /GF /Gy /I..\..\..\ /DWIN32 /DNDEBUG /D_MBCS /D_LIB /FD /Zc:wchar_t  $(CXXFLAGS) -Y- -Fo./vc7/libboost_regex-vc7-s-1_31/ -Fdvc7/libboost_regex-vc7-s-1_31.pdb ../src/c_regex_traits.cpp

vc7/libboost_regex-vc7-s-1_31/c_regex_traits_common.obj: ../src/c_regex_traits_common.cpp $(ALL_HEADER)
	cl $(INCLUDES) /c /nologo /ML /W3 /GX /O2 /GB /GF /Gy /I..\..\..\ /DWIN32 /DNDEBUG /D_MBCS /D_LIB /FD /Zc:wchar_t  $(CXXFLAGS) -Y- -Fo./vc7/libboost_regex-vc7-s-1_31/ -Fdvc7/libboost_regex-vc7-s-1_31.pdb ../src/c_regex_traits_common.cpp

vc7/libboost_regex-vc7-s-1_31/cpp_regex_traits.obj: ../src/cpp_regex_traits.cpp $(ALL_HEADER)
	cl $(INCLUDES) /c /nologo /ML /W3 /GX /O2 /GB /GF /Gy /I..\..\..\ /DWIN32 /DNDEBUG /D_MBCS /D_LIB /FD /Zc:wchar_t  $(CXXFLAGS) -Y- -Fo./vc7/libboost_regex-vc7-s-1_31/ -Fdvc7/libboost_regex-vc7-s-1_31.pdb ../src/cpp_regex_traits.cpp

vc7/libboost_regex-vc7-s-1_31/cregex.obj: ../src/cregex.cpp $(ALL_HEADER)
	cl $(INCLUDES) /c /nologo /ML /W3 /GX /O2 /GB /GF /Gy /I..\..\..\ /DWIN32 /DNDEBUG /D_MBCS /D_LIB /FD /Zc:wchar_t  $(CXXFLAGS) -Y- -Fo./vc7/libboost_regex-vc7-s-1_31/ -Fdvc7/libboost_regex-vc7-s-1_31.pdb ../src/cregex.cpp

vc7/libboost_regex-vc7-s-1_31/fileiter.obj: ../src/fileiter.cpp $(ALL_HEADER)
	cl $(INCLUDES) /c /nologo /ML /W3 /GX /O2 /GB /GF /Gy /I..\..\..\ /DWIN32 /DNDEBUG /D_MBCS /D_LIB /FD /Zc:wchar_t  $(CXXFLAGS) -Y- -Fo./vc7/libboost_regex-vc7-s-1_31/ -Fdvc7/libboost_regex-vc7-s-1_31.pdb ../src/fileiter.cpp

vc7/libboost_regex-vc7-s-1_31/instances.obj: ../src/instances.cpp $(ALL_HEADER)
	cl $(INCLUDES) /c /nologo /ML /W3 /GX /O2 /GB /GF /Gy /I..\..\..\ /DWIN32 /DNDEBUG /D_MBCS /D_LIB /FD /Zc:wchar_t  $(CXXFLAGS) -Y- -Fo./vc7/libboost_regex-vc7-s-1_31/ -Fdvc7/libboost_regex-vc7-s-1_31.pdb ../src/instances.cpp

vc7/libboost_regex-vc7-s-1_31/posix_api.obj: ../src/posix_api.cpp $(ALL_HEADER)
	cl $(INCLUDES) /c /nologo /ML /W3 /GX /O2 /GB /GF /Gy /I..\..\..\ /DWIN32 /DNDEBUG /D_MBCS /D_LIB /FD /Zc:wchar_t  $(CXXFLAGS) -Y- -Fo./vc7/libboost_regex-vc7-s-1_31/ -Fdvc7/libboost_regex-vc7-s-1_31.pdb ../src/posix_api.cpp

vc7/libboost_regex-vc7-s-1_31/regex.obj: ../src/regex.cpp $(ALL_HEADER)
	cl $(INCLUDES) /c /nologo /ML /W3 /GX /O2 /GB /GF /Gy /I..\..\..\ /DWIN32 /DNDEBUG /D_MBCS /D_LIB /FD /Zc:wchar_t  $(CXXFLAGS) -Y- -Fo./vc7/libboost_regex-vc7-s-1_31/ -Fdvc7/libboost_regex-vc7-s-1_31.pdb ../src/regex.cpp

vc7/libboost_regex-vc7-s-1_31/regex_debug.obj: ../src/regex_debug.cpp $(ALL_HEADER)
	cl $(INCLUDES) /c /nologo /ML /W3 /GX /O2 /GB /GF /Gy /I..\..\..\ /DWIN32 /DNDEBUG /D_MBCS /D_LIB /FD /Zc:wchar_t  $(CXXFLAGS) -Y- -Fo./vc7/libboost_regex-vc7-s-1_31/ -Fdvc7/libboost_regex-vc7-s-1_31.pdb ../src/regex_debug.cpp

vc7/libboost_regex-vc7-s-1_31/regex_synch.obj: ../src/regex_synch.cpp $(ALL_HEADER)
	cl $(INCLUDES) /c /nologo /ML /W3 /GX /O2 /GB /GF /Gy /I..\..\..\ /DWIN32 /DNDEBUG /D_MBCS /D_LIB /FD /Zc:wchar_t  $(CXXFLAGS) -Y- -Fo./vc7/libboost_regex-vc7-s-1_31/ -Fdvc7/libboost_regex-vc7-s-1_31.pdb ../src/regex_synch.cpp

vc7/libboost_regex-vc7-s-1_31/w32_regex_traits.obj: ../src/w32_regex_traits.cpp $(ALL_HEADER)
	cl $(INCLUDES) /c /nologo /ML /W3 /GX /O2 /GB /GF /Gy /I..\..\..\ /DWIN32 /DNDEBUG /D_MBCS /D_LIB /FD /Zc:wchar_t  $(CXXFLAGS) -Y- -Fo./vc7/libboost_regex-vc7-s-1_31/ -Fdvc7/libboost_regex-vc7-s-1_31.pdb ../src/w32_regex_traits.cpp

vc7/libboost_regex-vc7-s-1_31/wide_posix_api.obj: ../src/wide_posix_api.cpp $(ALL_HEADER)
	cl $(INCLUDES) /c /nologo /ML /W3 /GX /O2 /GB /GF /Gy /I..\..\..\ /DWIN32 /DNDEBUG /D_MBCS /D_LIB /FD /Zc:wchar_t  $(CXXFLAGS) -Y- -Fo./vc7/libboost_regex-vc7-s-1_31/ -Fdvc7/libboost_regex-vc7-s-1_31.pdb ../src/wide_posix_api.cpp

vc7/libboost_regex-vc7-s-1_31/winstances.obj: ../src/winstances.cpp $(ALL_HEADER)
	cl $(INCLUDES) /c /nologo /ML /W3 /GX /O2 /GB /GF /Gy /I..\..\..\ /DWIN32 /DNDEBUG /D_MBCS /D_LIB /FD /Zc:wchar_t  $(CXXFLAGS) -Y- -Fo./vc7/libboost_regex-vc7-s-1_31/ -Fdvc7/libboost_regex-vc7-s-1_31.pdb ../src/winstances.cpp

libboost_regex-vc7-s-1_31_dir :
	@if not exist "vc7\libboost_regex-vc7-s-1_31\$(NULL)" mkdir vc7\libboost_regex-vc7-s-1_31

libboost_regex-vc7-s-1_31_clean :
	del vc7\libboost_regex-vc7-s-1_31\*.obj
	del vc7\libboost_regex-vc7-s-1_31\*.idb
	del vc7\libboost_regex-vc7-s-1_31\*.exp
	del vc7\libboost_regex-vc7-s-1_31\*.pch

./vc7/libboost_regex-vc7-s-1_31.lib : vc7/libboost_regex-vc7-s-1_31/c_regex_traits.obj vc7/libboost_regex-vc7-s-1_31/c_regex_traits_common.obj vc7/libboost_regex-vc7-s-1_31/cpp_regex_traits.obj vc7/libboost_regex-vc7-s-1_31/cregex.obj vc7/libboost_regex-vc7-s-1_31/fileiter.obj vc7/libboost_regex-vc7-s-1_31/instances.obj vc7/libboost_regex-vc7-s-1_31/posix_api.obj vc7/libboost_regex-vc7-s-1_31/regex.obj vc7/libboost_regex-vc7-s-1_31/regex_debug.obj vc7/libboost_regex-vc7-s-1_31/regex_synch.obj vc7/libboost_regex-vc7-s-1_31/w32_regex_traits.obj vc7/libboost_regex-vc7-s-1_31/wide_posix_api.obj vc7/libboost_regex-vc7-s-1_31/winstances.obj
	link -lib /nologo /out:vc7/libboost_regex-vc7-s-1_31.lib $(XSFLAGS)  vc7/libboost_regex-vc7-s-1_31/c_regex_traits.obj vc7/libboost_regex-vc7-s-1_31/c_regex_traits_common.obj vc7/libboost_regex-vc7-s-1_31/cpp_regex_traits.obj vc7/libboost_regex-vc7-s-1_31/cregex.obj vc7/libboost_regex-vc7-s-1_31/fileiter.obj vc7/libboost_regex-vc7-s-1_31/instances.obj vc7/libboost_regex-vc7-s-1_31/posix_api.obj vc7/libboost_regex-vc7-s-1_31/regex.obj vc7/libboost_regex-vc7-s-1_31/regex_debug.obj vc7/libboost_regex-vc7-s-1_31/regex_synch.obj vc7/libboost_regex-vc7-s-1_31/w32_regex_traits.obj vc7/libboost_regex-vc7-s-1_31/wide_posix_api.obj vc7/libboost_regex-vc7-s-1_31/winstances.obj

########################################################
#
# section for libboost_regex-vc7-mt-s-1_31.lib
#
########################################################
vc7/libboost_regex-vc7-mt-s-1_31/c_regex_traits.obj: ../src/c_regex_traits.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MT /W3 /GX /O2 /GB /GF /Gy /I..\..\..\ /D_MT /DWIN32 /DNDEBUG /D_MBCS /D_LIB /FD /Zc:wchar_t /c $(CXXFLAGS) -Y- -Fo./vc7/libboost_regex-vc7-mt-s-1_31/ -Fdvc7/libboost_regex-vc7-mt-s-1_31.pdb ../src/c_regex_traits.cpp

vc7/libboost_regex-vc7-mt-s-1_31/c_regex_traits_common.obj: ../src/c_regex_traits_common.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MT /W3 /GX /O2 /GB /GF /Gy /I..\..\..\ /D_MT /DWIN32 /DNDEBUG /D_MBCS /D_LIB /FD /Zc:wchar_t /c $(CXXFLAGS) -Y- -Fo./vc7/libboost_regex-vc7-mt-s-1_31/ -Fdvc7/libboost_regex-vc7-mt-s-1_31.pdb ../src/c_regex_traits_common.cpp

vc7/libboost_regex-vc7-mt-s-1_31/cpp_regex_traits.obj: ../src/cpp_regex_traits.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MT /W3 /GX /O2 /GB /GF /Gy /I..\..\..\ /D_MT /DWIN32 /DNDEBUG /D_MBCS /D_LIB /FD /Zc:wchar_t /c $(CXXFLAGS) -Y- -Fo./vc7/libboost_regex-vc7-mt-s-1_31/ -Fdvc7/libboost_regex-vc7-mt-s-1_31.pdb ../src/cpp_regex_traits.cpp

vc7/libboost_regex-vc7-mt-s-1_31/cregex.obj: ../src/cregex.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MT /W3 /GX /O2 /GB /GF /Gy /I..\..\..\ /D_MT /DWIN32 /DNDEBUG /D_MBCS /D_LIB /FD /Zc:wchar_t /c $(CXXFLAGS) -Y- -Fo./vc7/libboost_regex-vc7-mt-s-1_31/ -Fdvc7/libboost_regex-vc7-mt-s-1_31.pdb ../src/cregex.cpp

vc7/libboost_regex-vc7-mt-s-1_31/fileiter.obj: ../src/fileiter.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MT /W3 /GX /O2 /GB /GF /Gy /I..\..\..\ /D_MT /DWIN32 /DNDEBUG /D_MBCS /D_LIB /FD /Zc:wchar_t /c $(CXXFLAGS) -Y- -Fo./vc7/libboost_regex-vc7-mt-s-1_31/ -Fdvc7/libboost_regex-vc7-mt-s-1_31.pdb ../src/fileiter.cpp

vc7/libboost_regex-vc7-mt-s-1_31/instances.obj: ../src/instances.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MT /W3 /GX /O2 /GB /GF /Gy /I..\..\..\ /D_MT /DWIN32 /DNDEBUG /D_MBCS /D_LIB /FD /Zc:wchar_t /c $(CXXFLAGS) -Y- -Fo./vc7/libboost_regex-vc7-mt-s-1_31/ -Fdvc7/libboost_regex-vc7-mt-s-1_31.pdb ../src/instances.cpp

vc7/libboost_regex-vc7-mt-s-1_31/posix_api.obj: ../src/posix_api.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MT /W3 /GX /O2 /GB /GF /Gy /I..\..\..\ /D_MT /DWIN32 /DNDEBUG /D_MBCS /D_LIB /FD /Zc:wchar_t /c $(CXXFLAGS) -Y- -Fo./vc7/libboost_regex-vc7-mt-s-1_31/ -Fdvc7/libboost_regex-vc7-mt-s-1_31.pdb ../src/posix_api.cpp

vc7/libboost_regex-vc7-mt-s-1_31/regex.obj: ../src/regex.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MT /W3 /GX /O2 /GB /GF /Gy /I..\..\..\ /D_MT /DWIN32 /DNDEBUG /D_MBCS /D_LIB /FD /Zc:wchar_t /c $(CXXFLAGS) -Y- -Fo./vc7/libboost_regex-vc7-mt-s-1_31/ -Fdvc7/libboost_regex-vc7-mt-s-1_31.pdb ../src/regex.cpp

vc7/libboost_regex-vc7-mt-s-1_31/regex_debug.obj: ../src/regex_debug.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MT /W3 /GX /O2 /GB /GF /Gy /I..\..\..\ /D_MT /DWIN32 /DNDEBUG /D_MBCS /D_LIB /FD /Zc:wchar_t /c $(CXXFLAGS) -Y- -Fo./vc7/libboost_regex-vc7-mt-s-1_31/ -Fdvc7/libboost_regex-vc7-mt-s-1_31.pdb ../src/regex_debug.cpp

vc7/libboost_regex-vc7-mt-s-1_31/regex_synch.obj: ../src/regex_synch.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MT /W3 /GX /O2 /GB /GF /Gy /I..\..\..\ /D_MT /DWIN32 /DNDEBUG /D_MBCS /D_LIB /FD /Zc:wchar_t /c $(CXXFLAGS) -Y- -Fo./vc7/libboost_regex-vc7-mt-s-1_31/ -Fdvc7/libboost_regex-vc7-mt-s-1_31.pdb ../src/regex_synch.cpp

vc7/libboost_regex-vc7-mt-s-1_31/w32_regex_traits.obj: ../src/w32_regex_traits.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MT /W3 /GX /O2 /GB /GF /Gy /I..\..\..\ /D_MT /DWIN32 /DNDEBUG /D_MBCS /D_LIB /FD /Zc:wchar_t /c $(CXXFLAGS) -Y- -Fo./vc7/libboost_regex-vc7-mt-s-1_31/ -Fdvc7/libboost_regex-vc7-mt-s-1_31.pdb ../src/w32_regex_traits.cpp

vc7/libboost_regex-vc7-mt-s-1_31/wide_posix_api.obj: ../src/wide_posix_api.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MT /W3 /GX /O2 /GB /GF /Gy /I..\..\..\ /D_MT /DWIN32 /DNDEBUG /D_MBCS /D_LIB /FD /Zc:wchar_t /c $(CXXFLAGS) -Y- -Fo./vc7/libboost_regex-vc7-mt-s-1_31/ -Fdvc7/libboost_regex-vc7-mt-s-1_31.pdb ../src/wide_posix_api.cpp

vc7/libboost_regex-vc7-mt-s-1_31/winstances.obj: ../src/winstances.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MT /W3 /GX /O2 /GB /GF /Gy /I..\..\..\ /D_MT /DWIN32 /DNDEBUG /D_MBCS /D_LIB /FD /Zc:wchar_t /c $(CXXFLAGS) -Y- -Fo./vc7/libboost_regex-vc7-mt-s-1_31/ -Fdvc7/libboost_regex-vc7-mt-s-1_31.pdb ../src/winstances.cpp

libboost_regex-vc7-mt-s-1_31_dir :
	@if not exist "vc7\libboost_regex-vc7-mt-s-1_31\$(NULL)" mkdir vc7\libboost_regex-vc7-mt-s-1_31

libboost_regex-vc7-mt-s-1_31_clean :
	del vc7\libboost_regex-vc7-mt-s-1_31\*.obj
	del vc7\libboost_regex-vc7-mt-s-1_31\*.idb
	del vc7\libboost_regex-vc7-mt-s-1_31\*.exp
	del vc7\libboost_regex-vc7-mt-s-1_31\*.pch

./vc7/libboost_regex-vc7-mt-s-1_31.lib : vc7/libboost_regex-vc7-mt-s-1_31/c_regex_traits.obj vc7/libboost_regex-vc7-mt-s-1_31/c_regex_traits_common.obj vc7/libboost_regex-vc7-mt-s-1_31/cpp_regex_traits.obj vc7/libboost_regex-vc7-mt-s-1_31/cregex.obj vc7/libboost_regex-vc7-mt-s-1_31/fileiter.obj vc7/libboost_regex-vc7-mt-s-1_31/instances.obj vc7/libboost_regex-vc7-mt-s-1_31/posix_api.obj vc7/libboost_regex-vc7-mt-s-1_31/regex.obj vc7/libboost_regex-vc7-mt-s-1_31/regex_debug.obj vc7/libboost_regex-vc7-mt-s-1_31/regex_synch.obj vc7/libboost_regex-vc7-mt-s-1_31/w32_regex_traits.obj vc7/libboost_regex-vc7-mt-s-1_31/wide_posix_api.obj vc7/libboost_regex-vc7-mt-s-1_31/winstances.obj
	link -lib /nologo /out:vc7/libboost_regex-vc7-mt-s-1_31.lib $(XSFLAGS)  vc7/libboost_regex-vc7-mt-s-1_31/c_regex_traits.obj vc7/libboost_regex-vc7-mt-s-1_31/c_regex_traits_common.obj vc7/libboost_regex-vc7-mt-s-1_31/cpp_regex_traits.obj vc7/libboost_regex-vc7-mt-s-1_31/cregex.obj vc7/libboost_regex-vc7-mt-s-1_31/fileiter.obj vc7/libboost_regex-vc7-mt-s-1_31/instances.obj vc7/libboost_regex-vc7-mt-s-1_31/posix_api.obj vc7/libboost_regex-vc7-mt-s-1_31/regex.obj vc7/libboost_regex-vc7-mt-s-1_31/regex_debug.obj vc7/libboost_regex-vc7-mt-s-1_31/regex_synch.obj vc7/libboost_regex-vc7-mt-s-1_31/w32_regex_traits.obj vc7/libboost_regex-vc7-mt-s-1_31/wide_posix_api.obj vc7/libboost_regex-vc7-mt-s-1_31/winstances.obj

########################################################
#
# section for libboost_regex-vc7-sgd-1_31.lib
#
########################################################
vc7/libboost_regex-vc7-sgd-1_31/c_regex_traits.obj: ../src/c_regex_traits.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MLd /W3 /Gm /GX /Zi /Od /I..\..\..\ /DWIN32 /D_DEBUG /D_MBCS /D_LIB /FD /GX /RTC1 /Zc:wchar_t /c  $(CXXFLAGS) -Y- -Fo./vc7/libboost_regex-vc7-sgd-1_31/ -Fdvc7/libboost_regex-vc7-sgd-1_31.pdb ../src/c_regex_traits.cpp

vc7/libboost_regex-vc7-sgd-1_31/c_regex_traits_common.obj: ../src/c_regex_traits_common.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MLd /W3 /Gm /GX /Zi /Od /I..\..\..\ /DWIN32 /D_DEBUG /D_MBCS /D_LIB /FD /GX /RTC1 /Zc:wchar_t /c  $(CXXFLAGS) -Y- -Fo./vc7/libboost_regex-vc7-sgd-1_31/ -Fdvc7/libboost_regex-vc7-sgd-1_31.pdb ../src/c_regex_traits_common.cpp

vc7/libboost_regex-vc7-sgd-1_31/cpp_regex_traits.obj: ../src/cpp_regex_traits.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MLd /W3 /Gm /GX /Zi /Od /I..\..\..\ /DWIN32 /D_DEBUG /D_MBCS /D_LIB /FD /GX /RTC1 /Zc:wchar_t /c  $(CXXFLAGS) -Y- -Fo./vc7/libboost_regex-vc7-sgd-1_31/ -Fdvc7/libboost_regex-vc7-sgd-1_31.pdb ../src/cpp_regex_traits.cpp

vc7/libboost_regex-vc7-sgd-1_31/cregex.obj: ../src/cregex.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MLd /W3 /Gm /GX /Zi /Od /I..\..\..\ /DWIN32 /D_DEBUG /D_MBCS /D_LIB /FD /GX /RTC1 /Zc:wchar_t /c  $(CXXFLAGS) -Y- -Fo./vc7/libboost_regex-vc7-sgd-1_31/ -Fdvc7/libboost_regex-vc7-sgd-1_31.pdb ../src/cregex.cpp

vc7/libboost_regex-vc7-sgd-1_31/fileiter.obj: ../src/fileiter.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MLd /W3 /Gm /GX /Zi /Od /I..\..\..\ /DWIN32 /D_DEBUG /D_MBCS /D_LIB /FD /GX /RTC1 /Zc:wchar_t /c  $(CXXFLAGS) -Y- -Fo./vc7/libboost_regex-vc7-sgd-1_31/ -Fdvc7/libboost_regex-vc7-sgd-1_31.pdb ../src/fileiter.cpp

vc7/libboost_regex-vc7-sgd-1_31/instances.obj: ../src/instances.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MLd /W3 /Gm /GX /Zi /Od /I..\..\..\ /DWIN32 /D_DEBUG /D_MBCS /D_LIB /FD /GX /RTC1 /Zc:wchar_t /c  $(CXXFLAGS) -Y- -Fo./vc7/libboost_regex-vc7-sgd-1_31/ -Fdvc7/libboost_regex-vc7-sgd-1_31.pdb ../src/instances.cpp

vc7/libboost_regex-vc7-sgd-1_31/posix_api.obj: ../src/posix_api.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MLd /W3 /Gm /GX /Zi /Od /I..\..\..\ /DWIN32 /D_DEBUG /D_MBCS /D_LIB /FD /GX /RTC1 /Zc:wchar_t /c  $(CXXFLAGS) -Y- -Fo./vc7/libboost_regex-vc7-sgd-1_31/ -Fdvc7/libboost_regex-vc7-sgd-1_31.pdb ../src/posix_api.cpp

vc7/libboost_regex-vc7-sgd-1_31/regex.obj: ../src/regex.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MLd /W3 /Gm /GX /Zi /Od /I..\..\..\ /DWIN32 /D_DEBUG /D_MBCS /D_LIB /FD /GX /RTC1 /Zc:wchar_t /c  $(CXXFLAGS) -Y- -Fo./vc7/libboost_regex-vc7-sgd-1_31/ -Fdvc7/libboost_regex-vc7-sgd-1_31.pdb ../src/regex.cpp

vc7/libboost_regex-vc7-sgd-1_31/regex_debug.obj: ../src/regex_debug.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MLd /W3 /Gm /GX /Zi /Od /I..\..\..\ /DWIN32 /D_DEBUG /D_MBCS /D_LIB /FD /GX /RTC1 /Zc:wchar_t /c  $(CXXFLAGS) -Y- -Fo./vc7/libboost_regex-vc7-sgd-1_31/ -Fdvc7/libboost_regex-vc7-sgd-1_31.pdb ../src/regex_debug.cpp

vc7/libboost_regex-vc7-sgd-1_31/regex_synch.obj: ../src/regex_synch.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MLd /W3 /Gm /GX /Zi /Od /I..\..\..\ /DWIN32 /D_DEBUG /D_MBCS /D_LIB /FD /GX /RTC1 /Zc:wchar_t /c  $(CXXFLAGS) -Y- -Fo./vc7/libboost_regex-vc7-sgd-1_31/ -Fdvc7/libboost_regex-vc7-sgd-1_31.pdb ../src/regex_synch.cpp

vc7/libboost_regex-vc7-sgd-1_31/w32_regex_traits.obj: ../src/w32_regex_traits.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MLd /W3 /Gm /GX /Zi /Od /I..\..\..\ /DWIN32 /D_DEBUG /D_MBCS /D_LIB /FD /GX /RTC1 /Zc:wchar_t /c  $(CXXFLAGS) -Y- -Fo./vc7/libboost_regex-vc7-sgd-1_31/ -Fdvc7/libboost_regex-vc7-sgd-1_31.pdb ../src/w32_regex_traits.cpp

vc7/libboost_regex-vc7-sgd-1_31/wide_posix_api.obj: ../src/wide_posix_api.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MLd /W3 /Gm /GX /Zi /Od /I..\..\..\ /DWIN32 /D_DEBUG /D_MBCS /D_LIB /FD /GX /RTC1 /Zc:wchar_t /c  $(CXXFLAGS) -Y- -Fo./vc7/libboost_regex-vc7-sgd-1_31/ -Fdvc7/libboost_regex-vc7-sgd-1_31.pdb ../src/wide_posix_api.cpp

vc7/libboost_regex-vc7-sgd-1_31/winstances.obj: ../src/winstances.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MLd /W3 /Gm /GX /Zi /Od /I..\..\..\ /DWIN32 /D_DEBUG /D_MBCS /D_LIB /FD /GX /RTC1 /Zc:wchar_t /c  $(CXXFLAGS) -Y- -Fo./vc7/libboost_regex-vc7-sgd-1_31/ -Fdvc7/libboost_regex-vc7-sgd-1_31.pdb ../src/winstances.cpp

libboost_regex-vc7-sgd-1_31_dir :
	@if not exist "vc7\libboost_regex-vc7-sgd-1_31\$(NULL)" mkdir vc7\libboost_regex-vc7-sgd-1_31

libboost_regex-vc7-sgd-1_31_clean :
	del vc7\libboost_regex-vc7-sgd-1_31\*.obj
	del vc7\libboost_regex-vc7-sgd-1_31\*.idb
	del vc7\libboost_regex-vc7-sgd-1_31\*.exp
	del vc7\libboost_regex-vc7-sgd-1_31\*.pch

./vc7/libboost_regex-vc7-sgd-1_31.lib : vc7/libboost_regex-vc7-sgd-1_31/c_regex_traits.obj vc7/libboost_regex-vc7-sgd-1_31/c_regex_traits_common.obj vc7/libboost_regex-vc7-sgd-1_31/cpp_regex_traits.obj vc7/libboost_regex-vc7-sgd-1_31/cregex.obj vc7/libboost_regex-vc7-sgd-1_31/fileiter.obj vc7/libboost_regex-vc7-sgd-1_31/instances.obj vc7/libboost_regex-vc7-sgd-1_31/posix_api.obj vc7/libboost_regex-vc7-sgd-1_31/regex.obj vc7/libboost_regex-vc7-sgd-1_31/regex_debug.obj vc7/libboost_regex-vc7-sgd-1_31/regex_synch.obj vc7/libboost_regex-vc7-sgd-1_31/w32_regex_traits.obj vc7/libboost_regex-vc7-sgd-1_31/wide_posix_api.obj vc7/libboost_regex-vc7-sgd-1_31/winstances.obj
	link -lib /nologo /out:vc7/libboost_regex-vc7-sgd-1_31.lib $(XSFLAGS)  vc7/libboost_regex-vc7-sgd-1_31/c_regex_traits.obj vc7/libboost_regex-vc7-sgd-1_31/c_regex_traits_common.obj vc7/libboost_regex-vc7-sgd-1_31/cpp_regex_traits.obj vc7/libboost_regex-vc7-sgd-1_31/cregex.obj vc7/libboost_regex-vc7-sgd-1_31/fileiter.obj vc7/libboost_regex-vc7-sgd-1_31/instances.obj vc7/libboost_regex-vc7-sgd-1_31/posix_api.obj vc7/libboost_regex-vc7-sgd-1_31/regex.obj vc7/libboost_regex-vc7-sgd-1_31/regex_debug.obj vc7/libboost_regex-vc7-sgd-1_31/regex_synch.obj vc7/libboost_regex-vc7-sgd-1_31/w32_regex_traits.obj vc7/libboost_regex-vc7-sgd-1_31/wide_posix_api.obj vc7/libboost_regex-vc7-sgd-1_31/winstances.obj

########################################################
#
# section for libboost_regex-vc7-mt-sgd-1_31.lib
#
########################################################
vc7/libboost_regex-vc7-mt-sgd-1_31/c_regex_traits.obj: ../src/c_regex_traits.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MTd /W3 /Gm /GX /Zi /Od /I..\..\..\ /DWIN32 /D_MT /D_DEBUG /D_MBCS /D_LIB /FD /GX /RTC1 /Zc:wchar_t /c $(CXXFLAGS) -Y- -Fo./vc7/libboost_regex-vc7-mt-sgd-1_31/ -Fdvc7/libboost_regex-vc7-mt-sgd-1_31.pdb ../src/c_regex_traits.cpp

vc7/libboost_regex-vc7-mt-sgd-1_31/c_regex_traits_common.obj: ../src/c_regex_traits_common.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MTd /W3 /Gm /GX /Zi /Od /I..\..\..\ /DWIN32 /D_MT /D_DEBUG /D_MBCS /D_LIB /FD /GX /RTC1 /Zc:wchar_t /c $(CXXFLAGS) -Y- -Fo./vc7/libboost_regex-vc7-mt-sgd-1_31/ -Fdvc7/libboost_regex-vc7-mt-sgd-1_31.pdb ../src/c_regex_traits_common.cpp

vc7/libboost_regex-vc7-mt-sgd-1_31/cpp_regex_traits.obj: ../src/cpp_regex_traits.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MTd /W3 /Gm /GX /Zi /Od /I..\..\..\ /DWIN32 /D_MT /D_DEBUG /D_MBCS /D_LIB /FD /GX /RTC1 /Zc:wchar_t /c $(CXXFLAGS) -Y- -Fo./vc7/libboost_regex-vc7-mt-sgd-1_31/ -Fdvc7/libboost_regex-vc7-mt-sgd-1_31.pdb ../src/cpp_regex_traits.cpp

vc7/libboost_regex-vc7-mt-sgd-1_31/cregex.obj: ../src/cregex.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MTd /W3 /Gm /GX /Zi /Od /I..\..\..\ /DWIN32 /D_MT /D_DEBUG /D_MBCS /D_LIB /FD /GX /RTC1 /Zc:wchar_t /c $(CXXFLAGS) -Y- -Fo./vc7/libboost_regex-vc7-mt-sgd-1_31/ -Fdvc7/libboost_regex-vc7-mt-sgd-1_31.pdb ../src/cregex.cpp

vc7/libboost_regex-vc7-mt-sgd-1_31/fileiter.obj: ../src/fileiter.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MTd /W3 /Gm /GX /Zi /Od /I..\..\..\ /DWIN32 /D_MT /D_DEBUG /D_MBCS /D_LIB /FD /GX /RTC1 /Zc:wchar_t /c $(CXXFLAGS) -Y- -Fo./vc7/libboost_regex-vc7-mt-sgd-1_31/ -Fdvc7/libboost_regex-vc7-mt-sgd-1_31.pdb ../src/fileiter.cpp

vc7/libboost_regex-vc7-mt-sgd-1_31/instances.obj: ../src/instances.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MTd /W3 /Gm /GX /Zi /Od /I..\..\..\ /DWIN32 /D_MT /D_DEBUG /D_MBCS /D_LIB /FD /GX /RTC1 /Zc:wchar_t /c $(CXXFLAGS) -Y- -Fo./vc7/libboost_regex-vc7-mt-sgd-1_31/ -Fdvc7/libboost_regex-vc7-mt-sgd-1_31.pdb ../src/instances.cpp

vc7/libboost_regex-vc7-mt-sgd-1_31/posix_api.obj: ../src/posix_api.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MTd /W3 /Gm /GX /Zi /Od /I..\..\..\ /DWIN32 /D_MT /D_DEBUG /D_MBCS /D_LIB /FD /GX /RTC1 /Zc:wchar_t /c $(CXXFLAGS) -Y- -Fo./vc7/libboost_regex-vc7-mt-sgd-1_31/ -Fdvc7/libboost_regex-vc7-mt-sgd-1_31.pdb ../src/posix_api.cpp

vc7/libboost_regex-vc7-mt-sgd-1_31/regex.obj: ../src/regex.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MTd /W3 /Gm /GX /Zi /Od /I..\..\..\ /DWIN32 /D_MT /D_DEBUG /D_MBCS /D_LIB /FD /GX /RTC1 /Zc:wchar_t /c $(CXXFLAGS) -Y- -Fo./vc7/libboost_regex-vc7-mt-sgd-1_31/ -Fdvc7/libboost_regex-vc7-mt-sgd-1_31.pdb ../src/regex.cpp

vc7/libboost_regex-vc7-mt-sgd-1_31/regex_debug.obj: ../src/regex_debug.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MTd /W3 /Gm /GX /Zi /Od /I..\..\..\ /DWIN32 /D_MT /D_DEBUG /D_MBCS /D_LIB /FD /GX /RTC1 /Zc:wchar_t /c $(CXXFLAGS) -Y- -Fo./vc7/libboost_regex-vc7-mt-sgd-1_31/ -Fdvc7/libboost_regex-vc7-mt-sgd-1_31.pdb ../src/regex_debug.cpp

vc7/libboost_regex-vc7-mt-sgd-1_31/regex_synch.obj: ../src/regex_synch.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MTd /W3 /Gm /GX /Zi /Od /I..\..\..\ /DWIN32 /D_MT /D_DEBUG /D_MBCS /D_LIB /FD /GX /RTC1 /Zc:wchar_t /c $(CXXFLAGS) -Y- -Fo./vc7/libboost_regex-vc7-mt-sgd-1_31/ -Fdvc7/libboost_regex-vc7-mt-sgd-1_31.pdb ../src/regex_synch.cpp

vc7/libboost_regex-vc7-mt-sgd-1_31/w32_regex_traits.obj: ../src/w32_regex_traits.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MTd /W3 /Gm /GX /Zi /Od /I..\..\..\ /DWIN32 /D_MT /D_DEBUG /D_MBCS /D_LIB /FD /GX /RTC1 /Zc:wchar_t /c $(CXXFLAGS) -Y- -Fo./vc7/libboost_regex-vc7-mt-sgd-1_31/ -Fdvc7/libboost_regex-vc7-mt-sgd-1_31.pdb ../src/w32_regex_traits.cpp

vc7/libboost_regex-vc7-mt-sgd-1_31/wide_posix_api.obj: ../src/wide_posix_api.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MTd /W3 /Gm /GX /Zi /Od /I..\..\..\ /DWIN32 /D_MT /D_DEBUG /D_MBCS /D_LIB /FD /GX /RTC1 /Zc:wchar_t /c $(CXXFLAGS) -Y- -Fo./vc7/libboost_regex-vc7-mt-sgd-1_31/ -Fdvc7/libboost_regex-vc7-mt-sgd-1_31.pdb ../src/wide_posix_api.cpp

vc7/libboost_regex-vc7-mt-sgd-1_31/winstances.obj: ../src/winstances.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MTd /W3 /Gm /GX /Zi /Od /I..\..\..\ /DWIN32 /D_MT /D_DEBUG /D_MBCS /D_LIB /FD /GX /RTC1 /Zc:wchar_t /c $(CXXFLAGS) -Y- -Fo./vc7/libboost_regex-vc7-mt-sgd-1_31/ -Fdvc7/libboost_regex-vc7-mt-sgd-1_31.pdb ../src/winstances.cpp

libboost_regex-vc7-mt-sgd-1_31_dir :
	@if not exist "vc7\libboost_regex-vc7-mt-sgd-1_31\$(NULL)" mkdir vc7\libboost_regex-vc7-mt-sgd-1_31

libboost_regex-vc7-mt-sgd-1_31_clean :
	del vc7\libboost_regex-vc7-mt-sgd-1_31\*.obj
	del vc7\libboost_regex-vc7-mt-sgd-1_31\*.idb
	del vc7\libboost_regex-vc7-mt-sgd-1_31\*.exp
	del vc7\libboost_regex-vc7-mt-sgd-1_31\*.pch

./vc7/libboost_regex-vc7-mt-sgd-1_31.lib : vc7/libboost_regex-vc7-mt-sgd-1_31/c_regex_traits.obj vc7/libboost_regex-vc7-mt-sgd-1_31/c_regex_traits_common.obj vc7/libboost_regex-vc7-mt-sgd-1_31/cpp_regex_traits.obj vc7/libboost_regex-vc7-mt-sgd-1_31/cregex.obj vc7/libboost_regex-vc7-mt-sgd-1_31/fileiter.obj vc7/libboost_regex-vc7-mt-sgd-1_31/instances.obj vc7/libboost_regex-vc7-mt-sgd-1_31/posix_api.obj vc7/libboost_regex-vc7-mt-sgd-1_31/regex.obj vc7/libboost_regex-vc7-mt-sgd-1_31/regex_debug.obj vc7/libboost_regex-vc7-mt-sgd-1_31/regex_synch.obj vc7/libboost_regex-vc7-mt-sgd-1_31/w32_regex_traits.obj vc7/libboost_regex-vc7-mt-sgd-1_31/wide_posix_api.obj vc7/libboost_regex-vc7-mt-sgd-1_31/winstances.obj
	link -lib /nologo /out:vc7/libboost_regex-vc7-mt-sgd-1_31.lib $(XSFLAGS)  vc7/libboost_regex-vc7-mt-sgd-1_31/c_regex_traits.obj vc7/libboost_regex-vc7-mt-sgd-1_31/c_regex_traits_common.obj vc7/libboost_regex-vc7-mt-sgd-1_31/cpp_regex_traits.obj vc7/libboost_regex-vc7-mt-sgd-1_31/cregex.obj vc7/libboost_regex-vc7-mt-sgd-1_31/fileiter.obj vc7/libboost_regex-vc7-mt-sgd-1_31/instances.obj vc7/libboost_regex-vc7-mt-sgd-1_31/posix_api.obj vc7/libboost_regex-vc7-mt-sgd-1_31/regex.obj vc7/libboost_regex-vc7-mt-sgd-1_31/regex_debug.obj vc7/libboost_regex-vc7-mt-sgd-1_31/regex_synch.obj vc7/libboost_regex-vc7-mt-sgd-1_31/w32_regex_traits.obj vc7/libboost_regex-vc7-mt-sgd-1_31/wide_posix_api.obj vc7/libboost_regex-vc7-mt-sgd-1_31/winstances.obj

########################################################
#
# section for boost_regex-vc7-mt-gd-1_31.lib
#
########################################################
vc7/boost_regex-vc7-mt-gd-1_31/c_regex_traits.obj: ../src/c_regex_traits.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MDd /W3 /Gm /GX /Zi /Od /I../../../ /D_DEBUG /DBOOST_REGEX_DYN_LINK /DWIN32 /D_WINDOWS /D_MBCS /DUSRDLL /FD /GX /RTC1 /Zc:wchar_t /c $(CXXFLAGS) -Y- -Fo./vc7/boost_regex-vc7-mt-gd-1_31/ -Fdvc7/boost_regex-vc7-mt-gd-1_31.pdb ../src/c_regex_traits.cpp

vc7/boost_regex-vc7-mt-gd-1_31/c_regex_traits_common.obj: ../src/c_regex_traits_common.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MDd /W3 /Gm /GX /Zi /Od /I../../../ /D_DEBUG /DBOOST_REGEX_DYN_LINK /DWIN32 /D_WINDOWS /D_MBCS /DUSRDLL /FD /GX /RTC1 /Zc:wchar_t /c $(CXXFLAGS) -Y- -Fo./vc7/boost_regex-vc7-mt-gd-1_31/ -Fdvc7/boost_regex-vc7-mt-gd-1_31.pdb ../src/c_regex_traits_common.cpp

vc7/boost_regex-vc7-mt-gd-1_31/cpp_regex_traits.obj: ../src/cpp_regex_traits.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MDd /W3 /Gm /GX /Zi /Od /I../../../ /D_DEBUG /DBOOST_REGEX_DYN_LINK /DWIN32 /D_WINDOWS /D_MBCS /DUSRDLL /FD /GX /RTC1 /Zc:wchar_t /c $(CXXFLAGS) -Y- -Fo./vc7/boost_regex-vc7-mt-gd-1_31/ -Fdvc7/boost_regex-vc7-mt-gd-1_31.pdb ../src/cpp_regex_traits.cpp

vc7/boost_regex-vc7-mt-gd-1_31/cregex.obj: ../src/cregex.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MDd /W3 /Gm /GX /Zi /Od /I../../../ /D_DEBUG /DBOOST_REGEX_DYN_LINK /DWIN32 /D_WINDOWS /D_MBCS /DUSRDLL /FD /GX /RTC1 /Zc:wchar_t /c $(CXXFLAGS) -Y- -Fo./vc7/boost_regex-vc7-mt-gd-1_31/ -Fdvc7/boost_regex-vc7-mt-gd-1_31.pdb ../src/cregex.cpp

vc7/boost_regex-vc7-mt-gd-1_31/fileiter.obj: ../src/fileiter.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MDd /W3 /Gm /GX /Zi /Od /I../../../ /D_DEBUG /DBOOST_REGEX_DYN_LINK /DWIN32 /D_WINDOWS /D_MBCS /DUSRDLL /FD /GX /RTC1 /Zc:wchar_t /c $(CXXFLAGS) -Y- -Fo./vc7/boost_regex-vc7-mt-gd-1_31/ -Fdvc7/boost_regex-vc7-mt-gd-1_31.pdb ../src/fileiter.cpp

vc7/boost_regex-vc7-mt-gd-1_31/instances.obj: ../src/instances.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MDd /W3 /Gm /GX /Zi /Od /I../../../ /D_DEBUG /DBOOST_REGEX_DYN_LINK /DWIN32 /D_WINDOWS /D_MBCS /DUSRDLL /FD /GX /RTC1 /Zc:wchar_t /c $(CXXFLAGS) -Y- -Fo./vc7/boost_regex-vc7-mt-gd-1_31/ -Fdvc7/boost_regex-vc7-mt-gd-1_31.pdb ../src/instances.cpp

vc7/boost_regex-vc7-mt-gd-1_31/posix_api.obj: ../src/posix_api.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MDd /W3 /Gm /GX /Zi /Od /I../../../ /D_DEBUG /DBOOST_REGEX_DYN_LINK /DWIN32 /D_WINDOWS /D_MBCS /DUSRDLL /FD /GX /RTC1 /Zc:wchar_t /c $(CXXFLAGS) -Y- -Fo./vc7/boost_regex-vc7-mt-gd-1_31/ -Fdvc7/boost_regex-vc7-mt-gd-1_31.pdb ../src/posix_api.cpp

vc7/boost_regex-vc7-mt-gd-1_31/regex.obj: ../src/regex.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MDd /W3 /Gm /GX /Zi /Od /I../../../ /D_DEBUG /DBOOST_REGEX_DYN_LINK /DWIN32 /D_WINDOWS /D_MBCS /DUSRDLL /FD /GX /RTC1 /Zc:wchar_t /c $(CXXFLAGS) -Y- -Fo./vc7/boost_regex-vc7-mt-gd-1_31/ -Fdvc7/boost_regex-vc7-mt-gd-1_31.pdb ../src/regex.cpp

vc7/boost_regex-vc7-mt-gd-1_31/regex_debug.obj: ../src/regex_debug.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MDd /W3 /Gm /GX /Zi /Od /I../../../ /D_DEBUG /DBOOST_REGEX_DYN_LINK /DWIN32 /D_WINDOWS /D_MBCS /DUSRDLL /FD /GX /RTC1 /Zc:wchar_t /c $(CXXFLAGS) -Y- -Fo./vc7/boost_regex-vc7-mt-gd-1_31/ -Fdvc7/boost_regex-vc7-mt-gd-1_31.pdb ../src/regex_debug.cpp

vc7/boost_regex-vc7-mt-gd-1_31/regex_synch.obj: ../src/regex_synch.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MDd /W3 /Gm /GX /Zi /Od /I../../../ /D_DEBUG /DBOOST_REGEX_DYN_LINK /DWIN32 /D_WINDOWS /D_MBCS /DUSRDLL /FD /GX /RTC1 /Zc:wchar_t /c $(CXXFLAGS) -Y- -Fo./vc7/boost_regex-vc7-mt-gd-1_31/ -Fdvc7/boost_regex-vc7-mt-gd-1_31.pdb ../src/regex_synch.cpp

vc7/boost_regex-vc7-mt-gd-1_31/w32_regex_traits.obj: ../src/w32_regex_traits.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MDd /W3 /Gm /GX /Zi /Od /I../../../ /D_DEBUG /DBOOST_REGEX_DYN_LINK /DWIN32 /D_WINDOWS /D_MBCS /DUSRDLL /FD /GX /RTC1 /Zc:wchar_t /c $(CXXFLAGS) -Y- -Fo./vc7/boost_regex-vc7-mt-gd-1_31/ -Fdvc7/boost_regex-vc7-mt-gd-1_31.pdb ../src/w32_regex_traits.cpp

vc7/boost_regex-vc7-mt-gd-1_31/wide_posix_api.obj: ../src/wide_posix_api.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MDd /W3 /Gm /GX /Zi /Od /I../../../ /D_DEBUG /DBOOST_REGEX_DYN_LINK /DWIN32 /D_WINDOWS /D_MBCS /DUSRDLL /FD /GX /RTC1 /Zc:wchar_t /c $(CXXFLAGS) -Y- -Fo./vc7/boost_regex-vc7-mt-gd-1_31/ -Fdvc7/boost_regex-vc7-mt-gd-1_31.pdb ../src/wide_posix_api.cpp

vc7/boost_regex-vc7-mt-gd-1_31/winstances.obj: ../src/winstances.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MDd /W3 /Gm /GX /Zi /Od /I../../../ /D_DEBUG /DBOOST_REGEX_DYN_LINK /DWIN32 /D_WINDOWS /D_MBCS /DUSRDLL /FD /GX /RTC1 /Zc:wchar_t /c $(CXXFLAGS) -Y- -Fo./vc7/boost_regex-vc7-mt-gd-1_31/ -Fdvc7/boost_regex-vc7-mt-gd-1_31.pdb ../src/winstances.cpp

boost_regex-vc7-mt-gd-1_31_dir :
	@if not exist "vc7\boost_regex-vc7-mt-gd-1_31\$(NULL)" mkdir vc7\boost_regex-vc7-mt-gd-1_31

boost_regex-vc7-mt-gd-1_31_clean :
	del vc7\boost_regex-vc7-mt-gd-1_31\*.obj
	del vc7\boost_regex-vc7-mt-gd-1_31\*.idb
	del vc7\boost_regex-vc7-mt-gd-1_31\*.exp
	del vc7\boost_regex-vc7-mt-gd-1_31\*.pch

./vc7/boost_regex-vc7-mt-gd-1_31.lib : vc7/boost_regex-vc7-mt-gd-1_31/c_regex_traits.obj vc7/boost_regex-vc7-mt-gd-1_31/c_regex_traits_common.obj vc7/boost_regex-vc7-mt-gd-1_31/cpp_regex_traits.obj vc7/boost_regex-vc7-mt-gd-1_31/cregex.obj vc7/boost_regex-vc7-mt-gd-1_31/fileiter.obj vc7/boost_regex-vc7-mt-gd-1_31/instances.obj vc7/boost_regex-vc7-mt-gd-1_31/posix_api.obj vc7/boost_regex-vc7-mt-gd-1_31/regex.obj vc7/boost_regex-vc7-mt-gd-1_31/regex_debug.obj vc7/boost_regex-vc7-mt-gd-1_31/regex_synch.obj vc7/boost_regex-vc7-mt-gd-1_31/w32_regex_traits.obj vc7/boost_regex-vc7-mt-gd-1_31/wide_posix_api.obj vc7/boost_regex-vc7-mt-gd-1_31/winstances.obj
	link kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /incremental:yes /pdb:"vc7/boost_regex-vc7-mt-gd-1_31.pdb" /debug /machine:I386 /out:"vc7/boost_regex-vc7-mt-gd-1_31.dll" /implib:"vc7/boost_regex-vc7-mt-gd-1_31.lib" /LIBPATH:$(STLPORT_PATH)\lib $(XLFLAGS)  vc7/boost_regex-vc7-mt-gd-1_31/c_regex_traits.obj vc7/boost_regex-vc7-mt-gd-1_31/c_regex_traits_common.obj vc7/boost_regex-vc7-mt-gd-1_31/cpp_regex_traits.obj vc7/boost_regex-vc7-mt-gd-1_31/cregex.obj vc7/boost_regex-vc7-mt-gd-1_31/fileiter.obj vc7/boost_regex-vc7-mt-gd-1_31/instances.obj vc7/boost_regex-vc7-mt-gd-1_31/posix_api.obj vc7/boost_regex-vc7-mt-gd-1_31/regex.obj vc7/boost_regex-vc7-mt-gd-1_31/regex_debug.obj vc7/boost_regex-vc7-mt-gd-1_31/regex_synch.obj vc7/boost_regex-vc7-mt-gd-1_31/w32_regex_traits.obj vc7/boost_regex-vc7-mt-gd-1_31/wide_posix_api.obj vc7/boost_regex-vc7-mt-gd-1_31/winstances.obj

########################################################
#
# section for boost_regex-vc7-mt-1_31.lib
#
########################################################
vc7/boost_regex-vc7-mt-1_31/c_regex_traits.obj: ../src/c_regex_traits.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MD /W3 /GX /O2 /GB /GF /Gy /I../../../ /DBOOST_REGEX_DYN_LINK /DNDEBUG /DWIN32 /D_WINDOWS /D_MBCS /D_USRDLL /FD /Zc:wchar_t /c $(CXXFLAGS) -Y- -Fo./vc7/boost_regex-vc7-mt-1_31/ -Fdvc7/boost_regex-vc7-mt-1_31.pdb ../src/c_regex_traits.cpp

vc7/boost_regex-vc7-mt-1_31/c_regex_traits_common.obj: ../src/c_regex_traits_common.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MD /W3 /GX /O2 /GB /GF /Gy /I../../../ /DBOOST_REGEX_DYN_LINK /DNDEBUG /DWIN32 /D_WINDOWS /D_MBCS /D_USRDLL /FD /Zc:wchar_t /c $(CXXFLAGS) -Y- -Fo./vc7/boost_regex-vc7-mt-1_31/ -Fdvc7/boost_regex-vc7-mt-1_31.pdb ../src/c_regex_traits_common.cpp

vc7/boost_regex-vc7-mt-1_31/cpp_regex_traits.obj: ../src/cpp_regex_traits.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MD /W3 /GX /O2 /GB /GF /Gy /I../../../ /DBOOST_REGEX_DYN_LINK /DNDEBUG /DWIN32 /D_WINDOWS /D_MBCS /D_USRDLL /FD /Zc:wchar_t /c $(CXXFLAGS) -Y- -Fo./vc7/boost_regex-vc7-mt-1_31/ -Fdvc7/boost_regex-vc7-mt-1_31.pdb ../src/cpp_regex_traits.cpp

vc7/boost_regex-vc7-mt-1_31/cregex.obj: ../src/cregex.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MD /W3 /GX /O2 /GB /GF /Gy /I../../../ /DBOOST_REGEX_DYN_LINK /DNDEBUG /DWIN32 /D_WINDOWS /D_MBCS /D_USRDLL /FD /Zc:wchar_t /c $(CXXFLAGS) -Y- -Fo./vc7/boost_regex-vc7-mt-1_31/ -Fdvc7/boost_regex-vc7-mt-1_31.pdb ../src/cregex.cpp

vc7/boost_regex-vc7-mt-1_31/fileiter.obj: ../src/fileiter.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MD /W3 /GX /O2 /GB /GF /Gy /I../../../ /DBOOST_REGEX_DYN_LINK /DNDEBUG /DWIN32 /D_WINDOWS /D_MBCS /D_USRDLL /FD /Zc:wchar_t /c $(CXXFLAGS) -Y- -Fo./vc7/boost_regex-vc7-mt-1_31/ -Fdvc7/boost_regex-vc7-mt-1_31.pdb ../src/fileiter.cpp

vc7/boost_regex-vc7-mt-1_31/instances.obj: ../src/instances.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MD /W3 /GX /O2 /GB /GF /Gy /I../../../ /DBOOST_REGEX_DYN_LINK /DNDEBUG /DWIN32 /D_WINDOWS /D_MBCS /D_USRDLL /FD /Zc:wchar_t /c $(CXXFLAGS) -Y- -Fo./vc7/boost_regex-vc7-mt-1_31/ -Fdvc7/boost_regex-vc7-mt-1_31.pdb ../src/instances.cpp

vc7/boost_regex-vc7-mt-1_31/posix_api.obj: ../src/posix_api.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MD /W3 /GX /O2 /GB /GF /Gy /I../../../ /DBOOST_REGEX_DYN_LINK /DNDEBUG /DWIN32 /D_WINDOWS /D_MBCS /D_USRDLL /FD /Zc:wchar_t /c $(CXXFLAGS) -Y- -Fo./vc7/boost_regex-vc7-mt-1_31/ -Fdvc7/boost_regex-vc7-mt-1_31.pdb ../src/posix_api.cpp

vc7/boost_regex-vc7-mt-1_31/regex.obj: ../src/regex.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MD /W3 /GX /O2 /GB /GF /Gy /I../../../ /DBOOST_REGEX_DYN_LINK /DNDEBUG /DWIN32 /D_WINDOWS /D_MBCS /D_USRDLL /FD /Zc:wchar_t /c $(CXXFLAGS) -Y- -Fo./vc7/boost_regex-vc7-mt-1_31/ -Fdvc7/boost_regex-vc7-mt-1_31.pdb ../src/regex.cpp

vc7/boost_regex-vc7-mt-1_31/regex_debug.obj: ../src/regex_debug.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MD /W3 /GX /O2 /GB /GF /Gy /I../../../ /DBOOST_REGEX_DYN_LINK /DNDEBUG /DWIN32 /D_WINDOWS /D_MBCS /D_USRDLL /FD /Zc:wchar_t /c $(CXXFLAGS) -Y- -Fo./vc7/boost_regex-vc7-mt-1_31/ -Fdvc7/boost_regex-vc7-mt-1_31.pdb ../src/regex_debug.cpp

vc7/boost_regex-vc7-mt-1_31/regex_synch.obj: ../src/regex_synch.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MD /W3 /GX /O2 /GB /GF /Gy /I../../../ /DBOOST_REGEX_DYN_LINK /DNDEBUG /DWIN32 /D_WINDOWS /D_MBCS /D_USRDLL /FD /Zc:wchar_t /c $(CXXFLAGS) -Y- -Fo./vc7/boost_regex-vc7-mt-1_31/ -Fdvc7/boost_regex-vc7-mt-1_31.pdb ../src/regex_synch.cpp

vc7/boost_regex-vc7-mt-1_31/w32_regex_traits.obj: ../src/w32_regex_traits.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MD /W3 /GX /O2 /GB /GF /Gy /I../../../ /DBOOST_REGEX_DYN_LINK /DNDEBUG /DWIN32 /D_WINDOWS /D_MBCS /D_USRDLL /FD /Zc:wchar_t /c $(CXXFLAGS) -Y- -Fo./vc7/boost_regex-vc7-mt-1_31/ -Fdvc7/boost_regex-vc7-mt-1_31.pdb ../src/w32_regex_traits.cpp

vc7/boost_regex-vc7-mt-1_31/wide_posix_api.obj: ../src/wide_posix_api.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MD /W3 /GX /O2 /GB /GF /Gy /I../../../ /DBOOST_REGEX_DYN_LINK /DNDEBUG /DWIN32 /D_WINDOWS /D_MBCS /D_USRDLL /FD /Zc:wchar_t /c $(CXXFLAGS) -Y- -Fo./vc7/boost_regex-vc7-mt-1_31/ -Fdvc7/boost_regex-vc7-mt-1_31.pdb ../src/wide_posix_api.cpp

vc7/boost_regex-vc7-mt-1_31/winstances.obj: ../src/winstances.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MD /W3 /GX /O2 /GB /GF /Gy /I../../../ /DBOOST_REGEX_DYN_LINK /DNDEBUG /DWIN32 /D_WINDOWS /D_MBCS /D_USRDLL /FD /Zc:wchar_t /c $(CXXFLAGS) -Y- -Fo./vc7/boost_regex-vc7-mt-1_31/ -Fdvc7/boost_regex-vc7-mt-1_31.pdb ../src/winstances.cpp

boost_regex-vc7-mt-1_31_dir :
	@if not exist "vc7\boost_regex-vc7-mt-1_31\$(NULL)" mkdir vc7\boost_regex-vc7-mt-1_31

boost_regex-vc7-mt-1_31_clean :
	del vc7\boost_regex-vc7-mt-1_31\*.obj
	del vc7\boost_regex-vc7-mt-1_31\*.idb
	del vc7\boost_regex-vc7-mt-1_31\*.exp
	del vc7\boost_regex-vc7-mt-1_31\*.pch

./vc7/boost_regex-vc7-mt-1_31.lib : vc7/boost_regex-vc7-mt-1_31/c_regex_traits.obj vc7/boost_regex-vc7-mt-1_31/c_regex_traits_common.obj vc7/boost_regex-vc7-mt-1_31/cpp_regex_traits.obj vc7/boost_regex-vc7-mt-1_31/cregex.obj vc7/boost_regex-vc7-mt-1_31/fileiter.obj vc7/boost_regex-vc7-mt-1_31/instances.obj vc7/boost_regex-vc7-mt-1_31/posix_api.obj vc7/boost_regex-vc7-mt-1_31/regex.obj vc7/boost_regex-vc7-mt-1_31/regex_debug.obj vc7/boost_regex-vc7-mt-1_31/regex_synch.obj vc7/boost_regex-vc7-mt-1_31/w32_regex_traits.obj vc7/boost_regex-vc7-mt-1_31/wide_posix_api.obj vc7/boost_regex-vc7-mt-1_31/winstances.obj
	link kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /incremental:yes /pdb:"vc7/boost_regex-vc7-mt-1_31.pdb" /debug /machine:I386 /out:"vc7/boost_regex-vc7-mt-1_31.dll" /implib:"vc7/boost_regex-vc7-mt-1_31.lib" /LIBPATH:$(STLPORT_PATH)\lib $(XLFLAGS)  vc7/boost_regex-vc7-mt-1_31/c_regex_traits.obj vc7/boost_regex-vc7-mt-1_31/c_regex_traits_common.obj vc7/boost_regex-vc7-mt-1_31/cpp_regex_traits.obj vc7/boost_regex-vc7-mt-1_31/cregex.obj vc7/boost_regex-vc7-mt-1_31/fileiter.obj vc7/boost_regex-vc7-mt-1_31/instances.obj vc7/boost_regex-vc7-mt-1_31/posix_api.obj vc7/boost_regex-vc7-mt-1_31/regex.obj vc7/boost_regex-vc7-mt-1_31/regex_debug.obj vc7/boost_regex-vc7-mt-1_31/regex_synch.obj vc7/boost_regex-vc7-mt-1_31/w32_regex_traits.obj vc7/boost_regex-vc7-mt-1_31/wide_posix_api.obj vc7/boost_regex-vc7-mt-1_31/winstances.obj

########################################################
#
# section for libboost_regex-vc7-mt-1_31.lib
#
########################################################
vc7/libboost_regex-vc7-mt-1_31/c_regex_traits.obj: ../src/c_regex_traits.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MD /W3 /GX /O2 /GB /GF /Gy /I../../../ /DBOOST_REGEX_STATIC_LINK /DNDEBUG /DWIN32 /D_WINDOWS /D_MBCS /D_USRDLL /FD /Zc:wchar_t /c $(CXXFLAGS) -Y- -Fo./vc7/libboost_regex-vc7-mt-1_31/ -Fdvc7/libboost_regex-vc7-mt-1_31.pdb ../src/c_regex_traits.cpp

vc7/libboost_regex-vc7-mt-1_31/c_regex_traits_common.obj: ../src/c_regex_traits_common.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MD /W3 /GX /O2 /GB /GF /Gy /I../../../ /DBOOST_REGEX_STATIC_LINK /DNDEBUG /DWIN32 /D_WINDOWS /D_MBCS /D_USRDLL /FD /Zc:wchar_t /c $(CXXFLAGS) -Y- -Fo./vc7/libboost_regex-vc7-mt-1_31/ -Fdvc7/libboost_regex-vc7-mt-1_31.pdb ../src/c_regex_traits_common.cpp

vc7/libboost_regex-vc7-mt-1_31/cpp_regex_traits.obj: ../src/cpp_regex_traits.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MD /W3 /GX /O2 /GB /GF /Gy /I../../../ /DBOOST_REGEX_STATIC_LINK /DNDEBUG /DWIN32 /D_WINDOWS /D_MBCS /D_USRDLL /FD /Zc:wchar_t /c $(CXXFLAGS) -Y- -Fo./vc7/libboost_regex-vc7-mt-1_31/ -Fdvc7/libboost_regex-vc7-mt-1_31.pdb ../src/cpp_regex_traits.cpp

vc7/libboost_regex-vc7-mt-1_31/cregex.obj: ../src/cregex.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MD /W3 /GX /O2 /GB /GF /Gy /I../../../ /DBOOST_REGEX_STATIC_LINK /DNDEBUG /DWIN32 /D_WINDOWS /D_MBCS /D_USRDLL /FD /Zc:wchar_t /c $(CXXFLAGS) -Y- -Fo./vc7/libboost_regex-vc7-mt-1_31/ -Fdvc7/libboost_regex-vc7-mt-1_31.pdb ../src/cregex.cpp

vc7/libboost_regex-vc7-mt-1_31/fileiter.obj: ../src/fileiter.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MD /W3 /GX /O2 /GB /GF /Gy /I../../../ /DBOOST_REGEX_STATIC_LINK /DNDEBUG /DWIN32 /D_WINDOWS /D_MBCS /D_USRDLL /FD /Zc:wchar_t /c $(CXXFLAGS) -Y- -Fo./vc7/libboost_regex-vc7-mt-1_31/ -Fdvc7/libboost_regex-vc7-mt-1_31.pdb ../src/fileiter.cpp

vc7/libboost_regex-vc7-mt-1_31/instances.obj: ../src/instances.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MD /W3 /GX /O2 /GB /GF /Gy /I../../../ /DBOOST_REGEX_STATIC_LINK /DNDEBUG /DWIN32 /D_WINDOWS /D_MBCS /D_USRDLL /FD /Zc:wchar_t /c $(CXXFLAGS) -Y- -Fo./vc7/libboost_regex-vc7-mt-1_31/ -Fdvc7/libboost_regex-vc7-mt-1_31.pdb ../src/instances.cpp

vc7/libboost_regex-vc7-mt-1_31/posix_api.obj: ../src/posix_api.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MD /W3 /GX /O2 /GB /GF /Gy /I../../../ /DBOOST_REGEX_STATIC_LINK /DNDEBUG /DWIN32 /D_WINDOWS /D_MBCS /D_USRDLL /FD /Zc:wchar_t /c $(CXXFLAGS) -Y- -Fo./vc7/libboost_regex-vc7-mt-1_31/ -Fdvc7/libboost_regex-vc7-mt-1_31.pdb ../src/posix_api.cpp

vc7/libboost_regex-vc7-mt-1_31/regex.obj: ../src/regex.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MD /W3 /GX /O2 /GB /GF /Gy /I../../../ /DBOOST_REGEX_STATIC_LINK /DNDEBUG /DWIN32 /D_WINDOWS /D_MBCS /D_USRDLL /FD /Zc:wchar_t /c $(CXXFLAGS) -Y- -Fo./vc7/libboost_regex-vc7-mt-1_31/ -Fdvc7/libboost_regex-vc7-mt-1_31.pdb ../src/regex.cpp

vc7/libboost_regex-vc7-mt-1_31/regex_debug.obj: ../src/regex_debug.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MD /W3 /GX /O2 /GB /GF /Gy /I../../../ /DBOOST_REGEX_STATIC_LINK /DNDEBUG /DWIN32 /D_WINDOWS /D_MBCS /D_USRDLL /FD /Zc:wchar_t /c $(CXXFLAGS) -Y- -Fo./vc7/libboost_regex-vc7-mt-1_31/ -Fdvc7/libboost_regex-vc7-mt-1_31.pdb ../src/regex_debug.cpp

vc7/libboost_regex-vc7-mt-1_31/regex_synch.obj: ../src/regex_synch.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MD /W3 /GX /O2 /GB /GF /Gy /I../../../ /DBOOST_REGEX_STATIC_LINK /DNDEBUG /DWIN32 /D_WINDOWS /D_MBCS /D_USRDLL /FD /Zc:wchar_t /c $(CXXFLAGS) -Y- -Fo./vc7/libboost_regex-vc7-mt-1_31/ -Fdvc7/libboost_regex-vc7-mt-1_31.pdb ../src/regex_synch.cpp

vc7/libboost_regex-vc7-mt-1_31/w32_regex_traits.obj: ../src/w32_regex_traits.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MD /W3 /GX /O2 /GB /GF /Gy /I../../../ /DBOOST_REGEX_STATIC_LINK /DNDEBUG /DWIN32 /D_WINDOWS /D_MBCS /D_USRDLL /FD /Zc:wchar_t /c $(CXXFLAGS) -Y- -Fo./vc7/libboost_regex-vc7-mt-1_31/ -Fdvc7/libboost_regex-vc7-mt-1_31.pdb ../src/w32_regex_traits.cpp

vc7/libboost_regex-vc7-mt-1_31/wide_posix_api.obj: ../src/wide_posix_api.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MD /W3 /GX /O2 /GB /GF /Gy /I../../../ /DBOOST_REGEX_STATIC_LINK /DNDEBUG /DWIN32 /D_WINDOWS /D_MBCS /D_USRDLL /FD /Zc:wchar_t /c $(CXXFLAGS) -Y- -Fo./vc7/libboost_regex-vc7-mt-1_31/ -Fdvc7/libboost_regex-vc7-mt-1_31.pdb ../src/wide_posix_api.cpp

vc7/libboost_regex-vc7-mt-1_31/winstances.obj: ../src/winstances.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MD /W3 /GX /O2 /GB /GF /Gy /I../../../ /DBOOST_REGEX_STATIC_LINK /DNDEBUG /DWIN32 /D_WINDOWS /D_MBCS /D_USRDLL /FD /Zc:wchar_t /c $(CXXFLAGS) -Y- -Fo./vc7/libboost_regex-vc7-mt-1_31/ -Fdvc7/libboost_regex-vc7-mt-1_31.pdb ../src/winstances.cpp

libboost_regex-vc7-mt-1_31_dir :
	@if not exist "vc7\libboost_regex-vc7-mt-1_31\$(NULL)" mkdir vc7\libboost_regex-vc7-mt-1_31

libboost_regex-vc7-mt-1_31_clean :
	del vc7\libboost_regex-vc7-mt-1_31\*.obj
	del vc7\libboost_regex-vc7-mt-1_31\*.idb
	del vc7\libboost_regex-vc7-mt-1_31\*.exp
	del vc7\libboost_regex-vc7-mt-1_31\*.pch

./vc7/libboost_regex-vc7-mt-1_31.lib : vc7/libboost_regex-vc7-mt-1_31/c_regex_traits.obj vc7/libboost_regex-vc7-mt-1_31/c_regex_traits_common.obj vc7/libboost_regex-vc7-mt-1_31/cpp_regex_traits.obj vc7/libboost_regex-vc7-mt-1_31/cregex.obj vc7/libboost_regex-vc7-mt-1_31/fileiter.obj vc7/libboost_regex-vc7-mt-1_31/instances.obj vc7/libboost_regex-vc7-mt-1_31/posix_api.obj vc7/libboost_regex-vc7-mt-1_31/regex.obj vc7/libboost_regex-vc7-mt-1_31/regex_debug.obj vc7/libboost_regex-vc7-mt-1_31/regex_synch.obj vc7/libboost_regex-vc7-mt-1_31/w32_regex_traits.obj vc7/libboost_regex-vc7-mt-1_31/wide_posix_api.obj vc7/libboost_regex-vc7-mt-1_31/winstances.obj
	link -lib /nologo /out:vc7/libboost_regex-vc7-mt-1_31.lib $(XSFLAGS)  vc7/libboost_regex-vc7-mt-1_31/c_regex_traits.obj vc7/libboost_regex-vc7-mt-1_31/c_regex_traits_common.obj vc7/libboost_regex-vc7-mt-1_31/cpp_regex_traits.obj vc7/libboost_regex-vc7-mt-1_31/cregex.obj vc7/libboost_regex-vc7-mt-1_31/fileiter.obj vc7/libboost_regex-vc7-mt-1_31/instances.obj vc7/libboost_regex-vc7-mt-1_31/posix_api.obj vc7/libboost_regex-vc7-mt-1_31/regex.obj vc7/libboost_regex-vc7-mt-1_31/regex_debug.obj vc7/libboost_regex-vc7-mt-1_31/regex_synch.obj vc7/libboost_regex-vc7-mt-1_31/w32_regex_traits.obj vc7/libboost_regex-vc7-mt-1_31/wide_posix_api.obj vc7/libboost_regex-vc7-mt-1_31/winstances.obj

########################################################
#
# section for libboost_regex-vc7-mt-gd-1_31.lib
#
########################################################
vc7/libboost_regex-vc7-mt-gd-1_31/c_regex_traits.obj: ../src/c_regex_traits.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MDd /W3 /Gm /GX /Zi /Od /I../../../ /DBOOST_REGEX_STATIC_LINK /D_DEBUG /DWIN32 /D_WINDOWS /D_MBCS /DUSRDLL /FD /GX /RTC1 /Zc:wchar_t /c $(CXXFLAGS) -Y- -Fo./vc7/libboost_regex-vc7-mt-gd-1_31/ -Fdvc7/libboost_regex-vc7-mt-gd-1_31.pdb ../src/c_regex_traits.cpp

vc7/libboost_regex-vc7-mt-gd-1_31/c_regex_traits_common.obj: ../src/c_regex_traits_common.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MDd /W3 /Gm /GX /Zi /Od /I../../../ /DBOOST_REGEX_STATIC_LINK /D_DEBUG /DWIN32 /D_WINDOWS /D_MBCS /DUSRDLL /FD /GX /RTC1 /Zc:wchar_t /c $(CXXFLAGS) -Y- -Fo./vc7/libboost_regex-vc7-mt-gd-1_31/ -Fdvc7/libboost_regex-vc7-mt-gd-1_31.pdb ../src/c_regex_traits_common.cpp

vc7/libboost_regex-vc7-mt-gd-1_31/cpp_regex_traits.obj: ../src/cpp_regex_traits.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MDd /W3 /Gm /GX /Zi /Od /I../../../ /DBOOST_REGEX_STATIC_LINK /D_DEBUG /DWIN32 /D_WINDOWS /D_MBCS /DUSRDLL /FD /GX /RTC1 /Zc:wchar_t /c $(CXXFLAGS) -Y- -Fo./vc7/libboost_regex-vc7-mt-gd-1_31/ -Fdvc7/libboost_regex-vc7-mt-gd-1_31.pdb ../src/cpp_regex_traits.cpp

vc7/libboost_regex-vc7-mt-gd-1_31/cregex.obj: ../src/cregex.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MDd /W3 /Gm /GX /Zi /Od /I../../../ /DBOOST_REGEX_STATIC_LINK /D_DEBUG /DWIN32 /D_WINDOWS /D_MBCS /DUSRDLL /FD /GX /RTC1 /Zc:wchar_t /c $(CXXFLAGS) -Y- -Fo./vc7/libboost_regex-vc7-mt-gd-1_31/ -Fdvc7/libboost_regex-vc7-mt-gd-1_31.pdb ../src/cregex.cpp

vc7/libboost_regex-vc7-mt-gd-1_31/fileiter.obj: ../src/fileiter.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MDd /W3 /Gm /GX /Zi /Od /I../../../ /DBOOST_REGEX_STATIC_LINK /D_DEBUG /DWIN32 /D_WINDOWS /D_MBCS /DUSRDLL /FD /GX /RTC1 /Zc:wchar_t /c $(CXXFLAGS) -Y- -Fo./vc7/libboost_regex-vc7-mt-gd-1_31/ -Fdvc7/libboost_regex-vc7-mt-gd-1_31.pdb ../src/fileiter.cpp

vc7/libboost_regex-vc7-mt-gd-1_31/instances.obj: ../src/instances.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MDd /W3 /Gm /GX /Zi /Od /I../../../ /DBOOST_REGEX_STATIC_LINK /D_DEBUG /DWIN32 /D_WINDOWS /D_MBCS /DUSRDLL /FD /GX /RTC1 /Zc:wchar_t /c $(CXXFLAGS) -Y- -Fo./vc7/libboost_regex-vc7-mt-gd-1_31/ -Fdvc7/libboost_regex-vc7-mt-gd-1_31.pdb ../src/instances.cpp

vc7/libboost_regex-vc7-mt-gd-1_31/posix_api.obj: ../src/posix_api.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MDd /W3 /Gm /GX /Zi /Od /I../../../ /DBOOST_REGEX_STATIC_LINK /D_DEBUG /DWIN32 /D_WINDOWS /D_MBCS /DUSRDLL /FD /GX /RTC1 /Zc:wchar_t /c $(CXXFLAGS) -Y- -Fo./vc7/libboost_regex-vc7-mt-gd-1_31/ -Fdvc7/libboost_regex-vc7-mt-gd-1_31.pdb ../src/posix_api.cpp

vc7/libboost_regex-vc7-mt-gd-1_31/regex.obj: ../src/regex.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MDd /W3 /Gm /GX /Zi /Od /I../../../ /DBOOST_REGEX_STATIC_LINK /D_DEBUG /DWIN32 /D_WINDOWS /D_MBCS /DUSRDLL /FD /GX /RTC1 /Zc:wchar_t /c $(CXXFLAGS) -Y- -Fo./vc7/libboost_regex-vc7-mt-gd-1_31/ -Fdvc7/libboost_regex-vc7-mt-gd-1_31.pdb ../src/regex.cpp

vc7/libboost_regex-vc7-mt-gd-1_31/regex_debug.obj: ../src/regex_debug.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MDd /W3 /Gm /GX /Zi /Od /I../../../ /DBOOST_REGEX_STATIC_LINK /D_DEBUG /DWIN32 /D_WINDOWS /D_MBCS /DUSRDLL /FD /GX /RTC1 /Zc:wchar_t /c $(CXXFLAGS) -Y- -Fo./vc7/libboost_regex-vc7-mt-gd-1_31/ -Fdvc7/libboost_regex-vc7-mt-gd-1_31.pdb ../src/regex_debug.cpp

vc7/libboost_regex-vc7-mt-gd-1_31/regex_synch.obj: ../src/regex_synch.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MDd /W3 /Gm /GX /Zi /Od /I../../../ /DBOOST_REGEX_STATIC_LINK /D_DEBUG /DWIN32 /D_WINDOWS /D_MBCS /DUSRDLL /FD /GX /RTC1 /Zc:wchar_t /c $(CXXFLAGS) -Y- -Fo./vc7/libboost_regex-vc7-mt-gd-1_31/ -Fdvc7/libboost_regex-vc7-mt-gd-1_31.pdb ../src/regex_synch.cpp

vc7/libboost_regex-vc7-mt-gd-1_31/w32_regex_traits.obj: ../src/w32_regex_traits.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MDd /W3 /Gm /GX /Zi /Od /I../../../ /DBOOST_REGEX_STATIC_LINK /D_DEBUG /DWIN32 /D_WINDOWS /D_MBCS /DUSRDLL /FD /GX /RTC1 /Zc:wchar_t /c $(CXXFLAGS) -Y- -Fo./vc7/libboost_regex-vc7-mt-gd-1_31/ -Fdvc7/libboost_regex-vc7-mt-gd-1_31.pdb ../src/w32_regex_traits.cpp

vc7/libboost_regex-vc7-mt-gd-1_31/wide_posix_api.obj: ../src/wide_posix_api.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MDd /W3 /Gm /GX /Zi /Od /I../../../ /DBOOST_REGEX_STATIC_LINK /D_DEBUG /DWIN32 /D_WINDOWS /D_MBCS /DUSRDLL /FD /GX /RTC1 /Zc:wchar_t /c $(CXXFLAGS) -Y- -Fo./vc7/libboost_regex-vc7-mt-gd-1_31/ -Fdvc7/libboost_regex-vc7-mt-gd-1_31.pdb ../src/wide_posix_api.cpp

vc7/libboost_regex-vc7-mt-gd-1_31/winstances.obj: ../src/winstances.cpp $(ALL_HEADER)
	cl $(INCLUDES) /nologo /MDd /W3 /Gm /GX /Zi /Od /I../../../ /DBOOST_REGEX_STATIC_LINK /D_DEBUG /DWIN32 /D_WINDOWS /D_MBCS /DUSRDLL /FD /GX /RTC1 /Zc:wchar_t /c $(CXXFLAGS) -Y- -Fo./vc7/libboost_regex-vc7-mt-gd-1_31/ -Fdvc7/libboost_regex-vc7-mt-gd-1_31.pdb ../src/winstances.cpp

libboost_regex-vc7-mt-gd-1_31_dir :
	@if not exist "vc7\libboost_regex-vc7-mt-gd-1_31\$(NULL)" mkdir vc7\libboost_regex-vc7-mt-gd-1_31

libboost_regex-vc7-mt-gd-1_31_clean :
	del vc7\libboost_regex-vc7-mt-gd-1_31\*.obj
	del vc7\libboost_regex-vc7-mt-gd-1_31\*.idb
	del vc7\libboost_regex-vc7-mt-gd-1_31\*.exp
	del vc7\libboost_regex-vc7-mt-gd-1_31\*.pch

./vc7/libboost_regex-vc7-mt-gd-1_31.lib : vc7/libboost_regex-vc7-mt-gd-1_31/c_regex_traits.obj vc7/libboost_regex-vc7-mt-gd-1_31/c_regex_traits_common.obj vc7/libboost_regex-vc7-mt-gd-1_31/cpp_regex_traits.obj vc7/libboost_regex-vc7-mt-gd-1_31/cregex.obj vc7/libboost_regex-vc7-mt-gd-1_31/fileiter.obj vc7/libboost_regex-vc7-mt-gd-1_31/instances.obj vc7/libboost_regex-vc7-mt-gd-1_31/posix_api.obj vc7/libboost_regex-vc7-mt-gd-1_31/regex.obj vc7/libboost_regex-vc7-mt-gd-1_31/regex_debug.obj vc7/libboost_regex-vc7-mt-gd-1_31/regex_synch.obj vc7/libboost_regex-vc7-mt-gd-1_31/w32_regex_traits.obj vc7/libboost_regex-vc7-mt-gd-1_31/wide_posix_api.obj vc7/libboost_regex-vc7-mt-gd-1_31/winstances.obj
	link -lib /nologo /out:vc7/libboost_regex-vc7-mt-gd-1_31.lib $(XSFLAGS)  vc7/libboost_regex-vc7-mt-gd-1_31/c_regex_traits.obj vc7/libboost_regex-vc7-mt-gd-1_31/c_regex_traits_common.obj vc7/libboost_regex-vc7-mt-gd-1_31/cpp_regex_traits.obj vc7/libboost_regex-vc7-mt-gd-1_31/cregex.obj vc7/libboost_regex-vc7-mt-gd-1_31/fileiter.obj vc7/libboost_regex-vc7-mt-gd-1_31/instances.obj vc7/libboost_regex-vc7-mt-gd-1_31/posix_api.obj vc7/libboost_regex-vc7-mt-gd-1_31/regex.obj vc7/libboost_regex-vc7-mt-gd-1_31/regex_debug.obj vc7/libboost_regex-vc7-mt-gd-1_31/regex_synch.obj vc7/libboost_regex-vc7-mt-gd-1_31/w32_regex_traits.obj vc7/libboost_regex-vc7-mt-gd-1_31/wide_posix_api.obj vc7/libboost_regex-vc7-mt-gd-1_31/winstances.obj

