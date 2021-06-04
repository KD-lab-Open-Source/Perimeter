//
// Copyright Thomas Witt 2004. Permission to copy, use,
// modify, sell and distribute this software is granted provided this
// copyright notice appears in all copies. This software is provided
// "as is" without express or implied warranty, and with no claim as
// to its suitability for any purpose.
//
#include <boost/iterator/iterator_archetypes.hpp>


int main()
{
  typedef boost::iterator_archetype<
      int
    , boost::iterator_archetypes::readable_iterator_t
    , boost::single_pass_traversal_tag
    > iter;

  // single_pass_traversal iterators are not required to be
  // default constructible
  iter it;
}
