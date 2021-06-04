// Copyright Thomas Witt 2003. Permission to copy, use,
// modify, sell and distribute this software is granted provided this
// copyright notice appears in all copies. This software is provided
// "as is" without express or implied warranty, and with no claim as
// to its suitability for any purpose.
#include <boost/iterator/indirect_iterator.hpp>
#include <boost/iterator/reverse_iterator.hpp>
#include <boost/concept_check.hpp>
#include <boost/cstdlib.hpp>
#include <list>

int main()
{
  {
    typedef boost::reverse_iterator<std::list<int*>::iterator>   rev_iter;
    typedef boost::indirect_iterator<std::list<int*>::iterator>  ind_iter;

    ind_iter() == rev_iter();
  }

  return boost::exit_success;
}
