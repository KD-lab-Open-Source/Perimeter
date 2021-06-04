// Boost.Function library

//  Copyright Doug Gregor 2001-2003. Use, modification and
//  distribution is subject to the Boost Software License, Version
//  1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

// For more information, see http://www.boost.org

#include <boost/test/minimal.hpp>
#include <cassert>
#include <functional>
#include <boost/function.hpp>

using namespace std;
using namespace boost;

static int alloc_count = 0;
static int dealloc_count = 0;

template<typename T>
struct counting_allocator : public std::allocator<T>
{
  template<typename U>
  struct rebind
  {
    typedef counting_allocator<U> other;
  };


  T* allocate(std::size_t n)
  {
    alloc_count++;
    return std::allocator<T>::allocate(n);
  }

  void deallocate(T* p, std::size_t n)
  {
    dealloc_count++;
    std::allocator<T>::deallocate(p, n);
  }
};

static int do_minus(int x, int y) { return x-y; }

struct DoNothing
{
  void operator()() const {}
};

static void do_nothing() {}

int
test_main(int, char*[])
{
  function2<int, int, int, counting_allocator<int> > f;
  f = plus<int>();
  f.clear();
  BOOST_TEST(alloc_count == 1);
  BOOST_TEST(dealloc_count == 1);

  alloc_count = 0;
  dealloc_count = 0;
  f = &do_minus;
  f.clear();

  function0<void, counting_allocator<int> > fv;
  alloc_count = 0;
  dealloc_count = 0;
  fv = DoNothing();
  fv.clear();
  BOOST_TEST(alloc_count == 1);
  BOOST_TEST(dealloc_count == 1);

  alloc_count = 0;
  dealloc_count = 0;
  fv = &do_nothing;
  fv.clear();

  return 0;
}
