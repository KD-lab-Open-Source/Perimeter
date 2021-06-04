// Example of using the filter iterator adaptor from
// boost/iterator_adaptors.hpp.

//  (C) Copyright Jeremy Siek 1999. Permission to copy, use, modify,
//  sell and distribute this software is granted provided this
//  copyright notice appears in all copies. This software is provided
//  "as is" without express or implied warranty, and with no claim as
//  to its suitability for any purpose.

#include <boost/config.hpp>
#include <algorithm>
#include <functional>
#include <iostream>
#include <boost/iterator/filter_iterator.hpp>

struct is_positive_number {
  bool operator()(int x) { return 0 < x; }
};

int main()
{
  int numbers_[] = { 0, -1, 4, -3, 5, 8, -2 };
  const int N = sizeof(numbers_)/sizeof(int);
  
  typedef int* base_iterator;
  base_iterator numbers(numbers_);
  
  // Example using make_filter_iterator()
  std::copy(boost::make_filter_iterator<is_positive_number>(numbers, numbers + N),
            boost::make_filter_iterator<is_positive_number>(numbers + N, numbers + N),
            std::ostream_iterator<int>(std::cout, " "));
  std::cout << std::endl;

  // Example using filter_iterator
  typedef boost::filter_iterator<is_positive_number, base_iterator>
    FilterIter;
  
  is_positive_number predicate;
  FilterIter filter_iter_first(predicate, numbers, numbers + N);
  FilterIter filter_iter_last(predicate, numbers + N, numbers + N);

  std::copy(filter_iter_first, filter_iter_last, std::ostream_iterator<int>(std::cout, " "));
  std::cout << std::endl;

  // Another example using make_filter_iterator()
  std::copy(
      boost::make_filter_iterator(
          std::bind2nd(std::greater<int>(), -2)
        , numbers, numbers + N)
            
    , boost::make_filter_iterator(
          std::bind2nd(std::greater<int>(), -2)
        , numbers + N, numbers + N)
      
    , std::ostream_iterator<int>(std::cout, " ")
  );
  
  std::cout << std::endl;
  
  
  return 0;
}
