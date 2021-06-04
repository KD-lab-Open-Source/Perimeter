// (C) Copyright Jeremy Siek 2000. Permission to copy, use, modify, sell and
// distribute this software is granted provided this copyright notice appears
// in all copies. This software is provided "as is" without express or implied
// warranty, and with no claim as to its suitability for any purpose.

#include <boost/config.hpp>
#include <iostream>
#include <algorithm>
#include <boost/iterator/reverse_iterator.hpp>
#include <boost/detail/iterator.hpp>

//boost::detail::iterator_traits
int main(int, char*[])
{
  char letters_[] = "hello world!";
  const int N = sizeof(letters_)/sizeof(char) - 1;
  typedef char* base_iterator;
  base_iterator letters(letters_);
  
  std::cout << "original sequence of letters:\t"
            << letters_ << std::endl;

  std::sort(letters, letters + N);

  // Use reverse_iterator_generator to print a sequence
  // of letters in reverse order.
  
  boost::reverse_iterator<base_iterator>
    reverse_letters_first(letters + N),
    reverse_letters_last(letters);

  std::cout << "letters in descending order:\t";
  std::copy(reverse_letters_first, reverse_letters_last,
            std::ostream_iterator<char>(std::cout));
  std::cout << std::endl;

  // Use make_reverse_iterator() to print the sequence
  // of letters in reverse-reverse order.

  std::cout << "letters in ascending order:\t";
  std::copy(boost::make_reverse_iterator(reverse_letters_last),
            boost::make_reverse_iterator(reverse_letters_first),
            std::ostream_iterator<char>(std::cout));
  std::cout << std::endl;

  return 0;
}
