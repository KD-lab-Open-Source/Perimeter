// (C) Copyright Jeremy Siek 2000. Permission to copy, use, modify, sell and
// distribute this software is granted provided this copyright notice appears
// in all copies. This software is provided "as is" without express or implied
// warranty, and with no claim as to its suitability for any purpose.


#include <boost/config.hpp>
#include <algorithm>
#include <iostream>
#include <iterator>
#include <vector>
#include <boost/iterator/counting_iterator.hpp>
#include <boost/iterator/indirect_iterator.hpp>

int main(int, char*[])
{
  // Example of using counting_iterator_generator
  std::cout << "counting from 0 to 4:" << std::endl;
  boost::counting_iterator<int> first(0), last(4);
  std::copy(first, last, std::ostream_iterator<int>(std::cout, " "));
  std::cout << std::endl;

  // Example of using make_counting_iterator()
  std::cout << "counting from -5 to 4:" << std::endl;
  std::copy(boost::make_counting_iterator(-5),
            boost::make_counting_iterator(5),
            std::ostream_iterator<int>(std::cout, " "));
  std::cout << std::endl;

  // Example of using counting iterator to create an array of pointers.
#if !BOOST_WORKAROUND(__BORLANDC__, BOOST_TESTED_AT(0x551))
  const
#endif 
      int N = 7;
  std::vector<int> numbers;
  // Fill "numbers" array with [0,N)
  std::copy(
      boost::make_counting_iterator(0)
      , boost::make_counting_iterator(N)
      , std::back_inserter(numbers));

  std::vector<std::vector<int>::iterator> pointers;

  // Use counting iterator to fill in the array of pointers.
  // causes an ICE with MSVC6
  std::copy(boost::make_counting_iterator(numbers.begin()),
            boost::make_counting_iterator(numbers.end()),
            std::back_inserter(pointers));

  // Use indirect iterator to print out numbers by accessing
  // them through the array of pointers.
  std::cout << "indirectly printing out the numbers from 0 to " 
            << N << std::endl;
  std::copy(boost::make_indirect_iterator(pointers.begin()),
            boost::make_indirect_iterator(pointers.end()),
            std::ostream_iterator<int>(std::cout, " "));
  std::cout << std::endl;
  
  return 0;
}
