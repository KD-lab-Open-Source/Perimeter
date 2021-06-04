
# The files in this directory implement the read_graphviz() functions
# declared in <boost/graph/graphviz.hpp>. There are both directed
# and undirected graph versions of these functions. To use both, you
# will need to compile the code generated from graphviz_parser.y twice,
# once with -DGRAPHVIZ_DIRECTED=0 and once with -DGRAPHVIZ_DIRECTED=1.
#
# This Makefile will most likely *not* work on your system.
# We have not yet had time to create a portable Makefile.
# This is the Makefile we currently use.
#

BOOST 		= ../../..

CXX		= g++ -ftemplate-depth-50
LEX		= flex
YACC		= bison
MV		= /bin/mv

EXTRAFLAGS 	= -g 

INCLUDES	= -I$(BOOST)

CXXFLAGS	= $(INCLUDES) $(EXTRAFLAGS)

LDFLAAGS	= 

LIBS		=

AR		= ar


default: libbgl-viz.a libbgl-viz.so

OBJS = graphviz_graph_lex.o graphviz_digraph_lex.o graphviz_digraph_parser.o graphviz_graph_parser.o

libbgl-viz.a: $(OBJS)
	$(AR) -rc libbgl-viz.a $(OBJS)

GRAPH_SONAME_VERSION = 1

libbgl-viz.so: $(OBJS)
	$(CXX) -shared -Wl,-soname -Wl,$@.$(GRAPH_SONAME_VERSION) -o $@.$(GRAPH_SONAME_VERSION) $(OBJS)
	ln -s $@.$(GRAPH_SONAME_VERSION) $@ 

# this next part is a bit strange. We compile graphviz_parser.cpp twice.
# Once with for undirected graphs with GRAPHVIZ_DIRECTED=0
# and once for directed graphs with GRAPHVIZ_DIRECTED=1.

graphviz_graph_parser.o: graphviz_graph_parser.cpp
	$(CXX) -DGRAPHVIZ_DIRECTED=0 $(CXXFLAGS) -c graphviz_graph_parser.cpp -o graphviz_graph_parser.o

graphviz_digraph_parser.o: graphviz_digraph_parser.cpp
	$(CXX) -DGRAPHVIZ_DIRECTED=1 $(CXXFLAGS) -c graphviz_digraph_parser.cpp -o graphviz_digraph_parser.o

graphviz_graph_lex.o: graphviz_graph_lex.cpp graphviz_graph_parser.hpp
	$(CXX) -DGRAPHVIZ_DIRECTED=0 $(CXXFLAGS) -c graphviz_graph_lex.cpp

graphviz_digraph_lex.o: graphviz_digraph_lex.cpp graphviz_digraph_parser.hpp
	$(CXX) -DGRAPHVIZ_DIRECTED=1 $(CXXFLAGS) -c graphviz_digraph_lex.cpp

graphviz_graph_lex.cpp: graphviz_lex.ll
	$(LEX) -Pbgl_undir_ -ographviz_graph_lex.cpp graphviz_lex.ll

graphviz_digraph_lex.cpp: graphviz_lex.ll
	$(LEX) -Pbgl_dir_ -ographviz_digraph_lex.cpp graphviz_lex.ll

graphviz_graph_parser.cpp graphviz_graph_parser.hpp: graphviz_parser.yy
	$(YACC) -p bgl_undir_ -d -v -o graphviz_graph_parser.cpp graphviz_parser.yy

graphviz_digraph_parser.cpp graphviz_digraph_parser.hpp: graphviz_parser.yy
	$(YACC) -p bgl_dir_ -d -v -o graphviz_digraph_parser.cpp graphviz_parser.yy


clean:
	/bin/rm -rf	*.o *.a *.so* *.output graphviz_digraph_parser.cpp graphviz_digraph_parser.hpp graphviz_graph_parser.cpp graphviz_graph_parser.hpp graphviz_digraph_lex.cpp graphviz_graph_lex.cpp
