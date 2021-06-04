// (C) Copyright Jeremy Siek 2000. Permission to copy, use, modify, sell and
// distribute this software is granted provided this copyright notice appears
// in all copies. This software is provided "as is" without express or implied
// warranty, and with no claim as to its suitability for any purpose.

#include <boost/config.hpp>
#include <vector>
#include <iostream>
#include <iterator>
#include <functional>
#include <algorithm>
#include <boost/iterator/indirect_iterator.hpp>

int main(int, char*[])
{
  char characters[] = "abcdefg";
  const int N = sizeof(characters)/sizeof(char) - 1; // -1 since characters has a null char
  char* pointers_to_chars[N];                        // at the end.
  for (int i = 0; i < N; ++i)
    pointers_to_chars[i] = &characters[i];

  // Example of using indirect_iterator_generator
  
  boost::indirect_iterator<char**, char>
    indirect_first(pointers_to_chars), indirect_last(pointers_to_chars + N);

  std::copy(indirect_first, indirect_last, std::ostream_iterator<char>(std::cout, ","));
  std::cout << std::endl;
  

  // Example of using indirect_iterator_pair_generator

  char mutable_characters[N];
  char* pointers_to_mutable_chars[N];
  for (int j = 0; j < N; ++j)
    pointers_to_mutable_chars[j] = &mutable_characters[j];

  boost::indirect_iterator<char* const*> mutable_indirect_first(pointers_to_mutable_chars),
    mutable_indirect_last(pointers_to_mutable_chars + N);
  boost::indirect_iterator<char* const*, char const> const_indirect_first(pointers_to_chars),
    const_indirect_last(pointers_to_chars + N);

  std::transform(const_indirect_first, const_indirect_last,
                 mutable_indirect_first, std::bind1st(std::plus<char>(), 1));

  std::copy(mutable_indirect_first, mutable_indirect_last,
            std::ostream_iterator<char>(std::cout, ","));
  std::cout << std::endl;

  
  // Example of using make_indirect_iterator()

  std::copy(boost::make_indirect_iterator(pointers_to_chars), 
            boost::make_indirect_iterator(pointers_to_chars + N),
            std::ostream_iterator<char>(std::cout, ","));
  std::cout << std::endl;
  
  return 0;
}
