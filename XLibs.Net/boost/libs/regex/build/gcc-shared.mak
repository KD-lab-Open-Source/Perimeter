#
# auto generated makefile for gcc compiler
#
# usage:
# make
#   brings libraries up to date
# make clean
#   deletes temporary object files (but not archives).
#

#
# the following environment variables are recognised:
# CXXFLAGS= extra compiler options - note applies to all build variants
# INCLUDES= additional include directories
# LDFLAGS=  additional linker options
# LIBS=     additional library files

# compiler:
CXX=g++
LINKER=g++ -shared

#
# compiler options for release build:
#
C1=-c -O2 -I../../../ -fPIC
#
# compiler options for debug build:
#
C2=-c -g -I../../../ -fPIC


ALL_HEADER= ../../../boost/regex/config.hpp ../../../boost/regex/pattern_except.hpp ../../../boost/regex/regex_traits.hpp ../../../boost/regex/user.hpp ../../../boost/regex/v3/cregex.hpp ../../../boost/regex/v3/fileiter.hpp ../../../boost/regex/v3/instances.hpp ../../../boost/regex/v3/regex.hpp ../../../boost/regex/v3/regex_compile.hpp ../../../boost/regex/v3/regex_cstring.hpp ../../../boost/regex/v3/regex_format.hpp ../../../boost/regex/v3/regex_fwd.hpp ../../../boost/regex/v3/regex_kmp.hpp ../../../boost/regex/v3/regex_library_include.hpp ../../../boost/regex/v3/regex_match.hpp ../../../boost/regex/v3/regex_raw_buffer.hpp ../../../boost/regex/v3/regex_split.hpp ../../../boost/regex/v3/regex_stack.hpp ../../../boost/regex/v3/regex_synch.hpp ../../../boost/regex/v3/regex_traits.hpp ../../../boost/regex/v4/basic_regex.hpp ../../../boost/regex/v4/char_regex_traits.hpp ../../../boost/regex/v4/cregex.hpp ../../../boost/regex/v4/fileiter.hpp ../../../boost/regex/v4/instances.hpp ../../../boost/regex/v4/iterator_category.hpp ../../../boost/regex/v4/iterator_traits.hpp ../../../boost/regex/v4/match_flags.hpp ../../../boost/regex/v4/match_results.hpp ../../../boost/regex/v4/mem_block_cache.hpp ../../../boost/regex/v4/perl_matcher.hpp ../../../boost/regex/v4/perl_matcher_common.hpp ../../../boost/regex/v4/perl_matcher_non_recursive.hpp ../../../boost/regex/v4/perl_matcher_recursive.hpp ../../../boost/regex/v4/regbase.hpp ../../../boost/regex/v4/regex.hpp ../../../boost/regex/v4/regex_compile.hpp ../../../boost/regex/v4/regex_cstring.hpp ../../../boost/regex/v4/regex_format.hpp ../../../boost/regex/v4/regex_fwd.hpp ../../../boost/regex/v4/regex_grep.hpp ../../../boost/regex/v4/regex_iterator.hpp ../../../boost/regex/v4/regex_kmp.hpp ../../../boost/regex/v4/regex_match.hpp ../../../boost/regex/v4/regex_merge.hpp ../../../boost/regex/v4/regex_raw_buffer.hpp ../../../boost/regex/v4/regex_replace.hpp ../../../boost/regex/v4/regex_search.hpp ../../../boost/regex/v4/regex_split.hpp ../../../boost/regex/v4/regex_stack.hpp ../../../boost/regex/v4/regex_synch.hpp ../../../boost/regex/v4/regex_token_iterator.hpp ../../../boost/regex/v4/regex_traits.hpp ../../../boost/regex/v4/states.hpp ../../../boost/regex/v4/sub_match.hpp ../../../boost/regex/config/borland.hpp ../../../boost/regex/config/cstring.hpp ../../../boost/regex/config/cwchar.hpp ../../../boost/regex/config/regex_library_include.hpp

all : gcc  gcc gcc/boost_regex-gcc-1_31_shared ./gcc/libboost_regex-gcc-1_31.so gcc gcc/boost_regex-gcc-d-1_31_shared ./gcc/libboost_regex-gcc-d-1_31.so

gcc :
	mkdir -p gcc

clean :  boost_regex-gcc-1_31_clean boost_regex-gcc-d-1_31_clean

install : all



########################################################
#
# section for libboost_regex-gcc-1_31.a
#
########################################################
gcc/boost_regex-gcc-1_31_shared/c_regex_traits.o: ../src/c_regex_traits.cpp $(ALL_HEADER)
	$(CXX) $(INCLUDES) -o gcc/boost_regex-gcc-1_31_shared/c_regex_traits.o $(C1) $(CXXFLAGS) ../src/c_regex_traits.cpp

gcc/boost_regex-gcc-1_31_shared/c_regex_traits_common.o: ../src/c_regex_traits_common.cpp $(ALL_HEADER)
	$(CXX) $(INCLUDES) -o gcc/boost_regex-gcc-1_31_shared/c_regex_traits_common.o $(C1) $(CXXFLAGS) ../src/c_regex_traits_common.cpp

gcc/boost_regex-gcc-1_31_shared/cpp_regex_traits.o: ../src/cpp_regex_traits.cpp $(ALL_HEADER)
	$(CXX) $(INCLUDES) -o gcc/boost_regex-gcc-1_31_shared/cpp_regex_traits.o $(C1) $(CXXFLAGS) ../src/cpp_regex_traits.cpp

gcc/boost_regex-gcc-1_31_shared/cregex.o: ../src/cregex.cpp $(ALL_HEADER)
	$(CXX) $(INCLUDES) -o gcc/boost_regex-gcc-1_31_shared/cregex.o $(C1) $(CXXFLAGS) ../src/cregex.cpp

gcc/boost_regex-gcc-1_31_shared/fileiter.o: ../src/fileiter.cpp $(ALL_HEADER)
	$(CXX) $(INCLUDES) -o gcc/boost_regex-gcc-1_31_shared/fileiter.o $(C1) $(CXXFLAGS) ../src/fileiter.cpp

gcc/boost_regex-gcc-1_31_shared/instances.o: ../src/instances.cpp $(ALL_HEADER)
	$(CXX) $(INCLUDES) -o gcc/boost_regex-gcc-1_31_shared/instances.o $(C1) $(CXXFLAGS) ../src/instances.cpp

gcc/boost_regex-gcc-1_31_shared/posix_api.o: ../src/posix_api.cpp $(ALL_HEADER)
	$(CXX) $(INCLUDES) -o gcc/boost_regex-gcc-1_31_shared/posix_api.o $(C1) $(CXXFLAGS) ../src/posix_api.cpp

gcc/boost_regex-gcc-1_31_shared/regex.o: ../src/regex.cpp $(ALL_HEADER)
	$(CXX) $(INCLUDES) -o gcc/boost_regex-gcc-1_31_shared/regex.o $(C1) $(CXXFLAGS) ../src/regex.cpp

gcc/boost_regex-gcc-1_31_shared/regex_debug.o: ../src/regex_debug.cpp $(ALL_HEADER)
	$(CXX) $(INCLUDES) -o gcc/boost_regex-gcc-1_31_shared/regex_debug.o $(C1) $(CXXFLAGS) ../src/regex_debug.cpp

gcc/boost_regex-gcc-1_31_shared/regex_synch.o: ../src/regex_synch.cpp $(ALL_HEADER)
	$(CXX) $(INCLUDES) -o gcc/boost_regex-gcc-1_31_shared/regex_synch.o $(C1) $(CXXFLAGS) ../src/regex_synch.cpp

gcc/boost_regex-gcc-1_31_shared/w32_regex_traits.o: ../src/w32_regex_traits.cpp $(ALL_HEADER)
	$(CXX) $(INCLUDES) -o gcc/boost_regex-gcc-1_31_shared/w32_regex_traits.o $(C1) $(CXXFLAGS) ../src/w32_regex_traits.cpp

gcc/boost_regex-gcc-1_31_shared/wide_posix_api.o: ../src/wide_posix_api.cpp $(ALL_HEADER)
	$(CXX) $(INCLUDES) -o gcc/boost_regex-gcc-1_31_shared/wide_posix_api.o $(C1) $(CXXFLAGS) ../src/wide_posix_api.cpp

gcc/boost_regex-gcc-1_31_shared/winstances.o: ../src/winstances.cpp $(ALL_HEADER)
	$(CXX) $(INCLUDES) -o gcc/boost_regex-gcc-1_31_shared/winstances.o $(C1) $(CXXFLAGS) ../src/winstances.cpp

gcc/boost_regex-gcc-1_31_shared : 
	mkdir -p gcc/boost_regex-gcc-1_31_shared

boost_regex-gcc-1_31_clean :
	rm -f gcc/boost_regex-gcc-1_31_shared/*.o

./gcc/libboost_regex-gcc-1_31.so : gcc/boost_regex-gcc-1_31_shared/c_regex_traits.o gcc/boost_regex-gcc-1_31_shared/c_regex_traits_common.o gcc/boost_regex-gcc-1_31_shared/cpp_regex_traits.o gcc/boost_regex-gcc-1_31_shared/cregex.o gcc/boost_regex-gcc-1_31_shared/fileiter.o gcc/boost_regex-gcc-1_31_shared/instances.o gcc/boost_regex-gcc-1_31_shared/posix_api.o gcc/boost_regex-gcc-1_31_shared/regex.o gcc/boost_regex-gcc-1_31_shared/regex_debug.o gcc/boost_regex-gcc-1_31_shared/regex_synch.o gcc/boost_regex-gcc-1_31_shared/w32_regex_traits.o gcc/boost_regex-gcc-1_31_shared/wide_posix_api.o gcc/boost_regex-gcc-1_31_shared/winstances.o
	$(LINKER) -o gcc/libboost_regex-gcc-1_31.so $(LDFLAGS)  gcc/boost_regex-gcc-1_31_shared/c_regex_traits.o gcc/boost_regex-gcc-1_31_shared/c_regex_traits_common.o gcc/boost_regex-gcc-1_31_shared/cpp_regex_traits.o gcc/boost_regex-gcc-1_31_shared/cregex.o gcc/boost_regex-gcc-1_31_shared/fileiter.o gcc/boost_regex-gcc-1_31_shared/instances.o gcc/boost_regex-gcc-1_31_shared/posix_api.o gcc/boost_regex-gcc-1_31_shared/regex.o gcc/boost_regex-gcc-1_31_shared/regex_debug.o gcc/boost_regex-gcc-1_31_shared/regex_synch.o gcc/boost_regex-gcc-1_31_shared/w32_regex_traits.o gcc/boost_regex-gcc-1_31_shared/wide_posix_api.o gcc/boost_regex-gcc-1_31_shared/winstances.o $(LIBS)

########################################################
#
# section for libboost_regex-gcc-d-1_31.a
#
########################################################
gcc/boost_regex-gcc-d-1_31_shared/c_regex_traits.o: ../src/c_regex_traits.cpp $(ALL_HEADER)
	$(CXX) $(INCLUDES) -o gcc/boost_regex-gcc-d-1_31_shared/c_regex_traits.o $(C2) $(CXXFLAGS) ../src/c_regex_traits.cpp

gcc/boost_regex-gcc-d-1_31_shared/c_regex_traits_common.o: ../src/c_regex_traits_common.cpp $(ALL_HEADER)
	$(CXX) $(INCLUDES) -o gcc/boost_regex-gcc-d-1_31_shared/c_regex_traits_common.o $(C2) $(CXXFLAGS) ../src/c_regex_traits_common.cpp

gcc/boost_regex-gcc-d-1_31_shared/cpp_regex_traits.o: ../src/cpp_regex_traits.cpp $(ALL_HEADER)
	$(CXX) $(INCLUDES) -o gcc/boost_regex-gcc-d-1_31_shared/cpp_regex_traits.o $(C2) $(CXXFLAGS) ../src/cpp_regex_traits.cpp

gcc/boost_regex-gcc-d-1_31_shared/cregex.o: ../src/cregex.cpp $(ALL_HEADER)
	$(CXX) $(INCLUDES) -o gcc/boost_regex-gcc-d-1_31_shared/cregex.o $(C2) $(CXXFLAGS) ../src/cregex.cpp

gcc/boost_regex-gcc-d-1_31_shared/fileiter.o: ../src/fileiter.cpp $(ALL_HEADER)
	$(CXX) $(INCLUDES) -o gcc/boost_regex-gcc-d-1_31_shared/fileiter.o $(C2) $(CXXFLAGS) ../src/fileiter.cpp

gcc/boost_regex-gcc-d-1_31_shared/instances.o: ../src/instances.cpp $(ALL_HEADER)
	$(CXX) $(INCLUDES) -o gcc/boost_regex-gcc-d-1_31_shared/instances.o $(C2) $(CXXFLAGS) ../src/instances.cpp

gcc/boost_regex-gcc-d-1_31_shared/posix_api.o: ../src/posix_api.cpp $(ALL_HEADER)
	$(CXX) $(INCLUDES) -o gcc/boost_regex-gcc-d-1_31_shared/posix_api.o $(C2) $(CXXFLAGS) ../src/posix_api.cpp

gcc/boost_regex-gcc-d-1_31_shared/regex.o: ../src/regex.cpp $(ALL_HEADER)
	$(CXX) $(INCLUDES) -o gcc/boost_regex-gcc-d-1_31_shared/regex.o $(C2) $(CXXFLAGS) ../src/regex.cpp

gcc/boost_regex-gcc-d-1_31_shared/regex_debug.o: ../src/regex_debug.cpp $(ALL_HEADER)
	$(CXX) $(INCLUDES) -o gcc/boost_regex-gcc-d-1_31_shared/regex_debug.o $(C2) $(CXXFLAGS) ../src/regex_debug.cpp

gcc/boost_regex-gcc-d-1_31_shared/regex_synch.o: ../src/regex_synch.cpp $(ALL_HEADER)
	$(CXX) $(INCLUDES) -o gcc/boost_regex-gcc-d-1_31_shared/regex_synch.o $(C2) $(CXXFLAGS) ../src/regex_synch.cpp

gcc/boost_regex-gcc-d-1_31_shared/w32_regex_traits.o: ../src/w32_regex_traits.cpp $(ALL_HEADER)
	$(CXX) $(INCLUDES) -o gcc/boost_regex-gcc-d-1_31_shared/w32_regex_traits.o $(C2) $(CXXFLAGS) ../src/w32_regex_traits.cpp

gcc/boost_regex-gcc-d-1_31_shared/wide_posix_api.o: ../src/wide_posix_api.cpp $(ALL_HEADER)
	$(CXX) $(INCLUDES) -o gcc/boost_regex-gcc-d-1_31_shared/wide_posix_api.o $(C2) $(CXXFLAGS) ../src/wide_posix_api.cpp

gcc/boost_regex-gcc-d-1_31_shared/winstances.o: ../src/winstances.cpp $(ALL_HEADER)
	$(CXX) $(INCLUDES) -o gcc/boost_regex-gcc-d-1_31_shared/winstances.o $(C2) $(CXXFLAGS) ../src/winstances.cpp

gcc/boost_regex-gcc-d-1_31_shared : 
	mkdir -p gcc/boost_regex-gcc-d-1_31_shared

boost_regex-gcc-d-1_31_clean :
	rm -f gcc/boost_regex-gcc-d-1_31_shared/*.o

./gcc/libboost_regex-gcc-d-1_31.so : gcc/boost_regex-gcc-d-1_31_shared/c_regex_traits.o gcc/boost_regex-gcc-d-1_31_shared/c_regex_traits_common.o gcc/boost_regex-gcc-d-1_31_shared/cpp_regex_traits.o gcc/boost_regex-gcc-d-1_31_shared/cregex.o gcc/boost_regex-gcc-d-1_31_shared/fileiter.o gcc/boost_regex-gcc-d-1_31_shared/instances.o gcc/boost_regex-gcc-d-1_31_shared/posix_api.o gcc/boost_regex-gcc-d-1_31_shared/regex.o gcc/boost_regex-gcc-d-1_31_shared/regex_debug.o gcc/boost_regex-gcc-d-1_31_shared/regex_synch.o gcc/boost_regex-gcc-d-1_31_shared/w32_regex_traits.o gcc/boost_regex-gcc-d-1_31_shared/wide_posix_api.o gcc/boost_regex-gcc-d-1_31_shared/winstances.o
	$(LINKER) -o gcc/libboost_regex-gcc-d-1_31.so $(LDFLAGS)  gcc/boost_regex-gcc-d-1_31_shared/c_regex_traits.o gcc/boost_regex-gcc-d-1_31_shared/c_regex_traits_common.o gcc/boost_regex-gcc-d-1_31_shared/cpp_regex_traits.o gcc/boost_regex-gcc-d-1_31_shared/cregex.o gcc/boost_regex-gcc-d-1_31_shared/fileiter.o gcc/boost_regex-gcc-d-1_31_shared/instances.o gcc/boost_regex-gcc-d-1_31_shared/posix_api.o gcc/boost_regex-gcc-d-1_31_shared/regex.o gcc/boost_regex-gcc-d-1_31_shared/regex_debug.o gcc/boost_regex-gcc-d-1_31_shared/regex_synch.o gcc/boost_regex-gcc-d-1_31_shared/w32_regex_traits.o gcc/boost_regex-gcc-d-1_31_shared/wide_posix_api.o gcc/boost_regex-gcc-d-1_31_shared/winstances.o $(LIBS)

