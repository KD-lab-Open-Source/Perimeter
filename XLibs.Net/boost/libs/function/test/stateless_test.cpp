// Boost.Function library

//  Copyright Doug Gregor 2001-2003. Use, modification and
//  distribution is subject to the Boost Software License, Version
//  1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

// For more information, see http://www.boost.org

#include <boost/test/minimal.hpp>
#include <boost/function.hpp>
#include <stdexcept>

struct stateless_integer_add {
  int operator()(int x, int y) const { return x+y; }

  void* operator new(std::size_t, stateless_integer_add*)
  {
    throw std::runtime_error("Cannot allocate a stateless_integer_add");
  }

  void operator delete(void*, stateless_integer_add*) throw()
  {
  }
};

namespace boost {
  template<>
  struct is_stateless<stateless_integer_add> {
    BOOST_STATIC_CONSTANT(bool, value = true);
  };
}

int test_main(int, char*[])
{
  boost::function2<int, int, int> f;
  f = stateless_integer_add();

  return 0;
}
