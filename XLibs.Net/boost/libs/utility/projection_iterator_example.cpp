// (C) Copyright Jeremy Siek 2000. Permission to copy, use, modify, sell and
// distribute this software is granted provided this copyright notice appears
// in all copies. This software is provided "as is" without express or implied
// warranty, and with no claim as to its suitability for any purpose.

#include <boost/config.hpp>
#include <list>
#include <iostream>
#include <iterator>
#include <algorithm>
#include <string>
#include <boost/iterator/transform_iterator.hpp>

struct personnel_record {
  personnel_record(std::string n, int id) : m_name(n), m_ID(id) { }
  std::string m_name;
  int m_ID;
};

struct select_name {
  typedef personnel_record argument_type;
  typedef std::string const& result_type;
  const std::string& operator()(const personnel_record& r) const {
    return r.m_name;
  }
  std::string& operator()(personnel_record& r) const {
    return r.m_name;
  }
};

struct select_ID {
  typedef personnel_record argument_type;
  typedef int& result_type;
  const int& operator()(const personnel_record& r) const {
    return r.m_ID;
  }
  int& operator()(personnel_record& r) const {
    return r.m_ID;
  }
};

int main(int, char*[])
{
  std::list<personnel_record> personnel_list;

  personnel_list.push_back(personnel_record("Barney", 13423));
  personnel_list.push_back(personnel_record("Fred", 12343));
  personnel_list.push_back(personnel_record("Wilma", 62454));
  personnel_list.push_back(personnel_record("Betty", 20490));

  // Example of using transform_iterator to print out the names in the
  // personnel list using a projection.

  boost::transform_iterator<
      select_name
    , std::list<personnel_record>::iterator
#ifdef BOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION
    , std::string
#endif 
  >
    personnel_first(personnel_list.begin()),
    personnel_last(personnel_list.end());

  std::copy(personnel_first, personnel_last,
            std::ostream_iterator<std::string>(std::cout, "\n"));
  std::cout << std::endl;
  
  // Example of using transform_iterator with const_iterators to
  // assign new ID numbers to the personnel.
  
  boost::transform_iterator<
      select_ID, std::list<personnel_record>::iterator
 > ID_first(personnel_list.begin()),
    ID_last(personnel_list.end());

  int new_id = 0;
  while (ID_first != ID_last) {
    *ID_first = new_id++;
    ++ID_first;
  }

    boost::transform_iterator<
      select_ID, std::list<personnel_record>::const_iterator, int const&
    >
    const_ID_first(personnel_list.begin()),
    const_ID_last(personnel_list.end());

  std::copy(const_ID_first, const_ID_last,
            std::ostream_iterator<int>(std::cout, " "));
  std::cout << std::endl;
  std::cout << std::endl;

#ifndef BOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION
  // Example of using make_const_projection_iterator()
  // to print out the names in the personnel list again.
  std::copy(
      boost::make_transform_iterator<select_name>(personnel_list.begin())
    , boost::make_transform_iterator<select_name>(personnel_list.end())
    , std::ostream_iterator<std::string>(std::cout, "\n"));
#endif
  
  return 0;
}
