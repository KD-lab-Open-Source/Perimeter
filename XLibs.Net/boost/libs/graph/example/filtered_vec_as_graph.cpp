//=======================================================================
// Copyright 2001 University of Notre Dame.
// Author: Jeremy G. Siek
//
// This file is part of the Boost Graph Library
//
// You should have received a copy of the License Agreement for the
// Boost Graph Library along with the software; see the file LICENSE.
// If not, contact Office of Research, University of Notre Dame, Notre
// Dame, IN 46556.
//
// Permission to modify the code and to distribute modified code is
// granted, provided the text of this NOTICE is retained, a notice that
// the code was modified is included with the above COPYRIGHT NOTICE and
// with the COPYRIGHT NOTICE in the LICENSE file, and that the LICENSE
// file is distributed with the modified code.
//
// LICENSOR MAKES NO REPRESENTATIONS OR WARRANTIES, EXPRESS OR IMPLIED.
// By way of example, but not limitation, Licensor MAKES NO
// REPRESENTATIONS OR WARRANTIES OF MERCHANTABILITY OR FITNESS FOR ANY
// PARTICULAR PURPOSE OR THAT THE USE OF THE LICENSED SOFTWARE COMPONENTS
// OR DOCUMENTATION WILL NOT INFRINGE ANY PATENTS, COPYRIGHTS, TRADEMARKS
// OR OTHER RIGHTS.
//=======================================================================

/* 
   Sample output:

   filtered out-edges:
   A --> 
   B --> 
   C --> E 
   D --> E 
   E --> 
 */

#include <boost/config.hpp>
#include <iostream>
#include <boost/graph/vector_as_graph.hpp>
#include <boost/graph/filtered_graph.hpp>
#include <boost/graph/graph_utility.hpp>

struct constant_target {
  constant_target() { }
  constant_target(int t) : target(t) { }
  bool operator()(const std::pair<int,int>& e) const {
    return e.second == target;
  }
  int target;
};


int main()
{
  using namespace boost;
  
  enum { A, B, C, D, E, N };
  const char* name = "ABCDE";
  typedef std::vector < std::list < int > > Graph;
  Graph g(N);
  g[A].push_back(B);
  g[A].push_back(C);
  g[C].push_back(D);
  g[C].push_back(E);
  g[D].push_back(E);
  g[E].push_back(C);

  constant_target filter(E);
  filtered_graph<Graph, constant_target> fg(g, filter);

  std::cout << "filtered out-edges:" << std::endl;
  print_graph(fg, name);
  
  return 0;
}
