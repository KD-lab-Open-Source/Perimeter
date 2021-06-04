// Boost.Function library

//  Copyright Doug Gregor 2004. Use, modification and
//  distribution is subject to the Boost Software License, Version
//  1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include <boost/test/minimal.hpp>
#include <boost/function.hpp>
#include <boost/ref.hpp>

static int forty_two() { return 42; }

struct Seventeen
{
  int operator()() const { return 17; }
};

struct ReturnInt
{
  explicit ReturnInt(int value) : value(value) {}

  int operator()() const { return value; }

  int value;
};

bool operator==(const ReturnInt& x, const ReturnInt& y)
{ return x.value == y.value; }

bool operator!=(const ReturnInt& x, const ReturnInt& y)
{ return x.value != y.value; }

static void target_test()
{
  boost::function0<int> f;

  f = &forty_two;
  BOOST_TEST(*f.target<int (*)()>() == &forty_two);
  BOOST_TEST(!f.target<Seventeen>());

  f = Seventeen();
  BOOST_TEST(!f.target<int (*)()>());
  BOOST_TEST(f.target<Seventeen>());

  Seventeen this_seventeen;
  f = boost::ref(this_seventeen);
  BOOST_TEST(!f.target<int (*)()>());
  BOOST_TEST(f.target<Seventeen>());
  BOOST_TEST(f.target<Seventeen>() == &this_seventeen);
}

static void equal_test()
{
  boost::function0<int> f;

  f = &forty_two;
  BOOST_TEST(f == &forty_two);
  BOOST_TEST(&forty_two == f);
  BOOST_TEST(f != ReturnInt(17));
  BOOST_TEST(ReturnInt(17) != f);
  BOOST_TEST(f.contains(&forty_two));

  f = ReturnInt(17);
  BOOST_TEST(f != &forty_two);
  BOOST_TEST(&forty_two != f);
  BOOST_TEST(f == ReturnInt(17));
  BOOST_TEST(ReturnInt(17) == f);
  BOOST_TEST(f != ReturnInt(16));
  BOOST_TEST(ReturnInt(16) != f);
  BOOST_TEST(f.contains(ReturnInt(17)));

#if !defined(BOOST_FUNCTION_NO_FUNCTION_TYPE_SYNTAX)
  boost::function<int(void)> g;

  g = &forty_two;
  BOOST_TEST(g == &forty_two);
  BOOST_TEST(&forty_two == g);
  BOOST_TEST(g != ReturnInt(17));
  BOOST_TEST(ReturnInt(17) != g);

  g = ReturnInt(17);
  BOOST_TEST(g != &forty_two);
  BOOST_TEST(&forty_two != g);
  BOOST_TEST(g == ReturnInt(17));
  BOOST_TEST(ReturnInt(17) == g);
  BOOST_TEST(g != ReturnInt(16));
  BOOST_TEST(ReturnInt(16) != g);
#endif
}

static void ref_equal_test()
{
  {
    ReturnInt ri(17);
    boost::function0<int> f = boost::ref(ri);

    // References and values are equal
    BOOST_TEST(f == boost::ref(ri));
    BOOST_TEST(f == ri);
    BOOST_TEST(boost::ref(ri) == f);
    BOOST_TEST(ri == f);
    BOOST_TEST(!(f != boost::ref(ri)));
    BOOST_TEST(!(f != ri));
    BOOST_TEST(!(boost::ref(ri) != f));
    BOOST_TEST(!(ri != f));

    // Values equal, references inequal
    ReturnInt ri2(17);
    BOOST_TEST(f == ri2);
    BOOST_TEST(f != boost::ref(ri2));
    BOOST_TEST(ri2 == f);
    BOOST_TEST(boost::ref(ri2) != f);
    BOOST_TEST(!(f != ri2));
    BOOST_TEST(!(f == boost::ref(ri2)));
    BOOST_TEST(!(ri2 != f));
    BOOST_TEST(!(boost::ref(ri2) == f));
  }

#if !defined(BOOST_FUNCTION_NO_FUNCTION_TYPE_SYNTAX)
  {
    ReturnInt ri(17);
    boost::function<int(void)> f = boost::ref(ri);

    // References and values are equal
    BOOST_TEST(f == boost::ref(ri));
    BOOST_TEST(f == ri);
    BOOST_TEST(boost::ref(ri) == f);
    BOOST_TEST(ri == f);
    BOOST_TEST(!(f != boost::ref(ri)));
    BOOST_TEST(!(f != ri));
    BOOST_TEST(!(boost::ref(ri) != f));
    BOOST_TEST(!(ri != f));

    // Values equal, references inequal
    ReturnInt ri2(17);
    BOOST_TEST(f == ri2);
    BOOST_TEST(f != boost::ref(ri2));
    BOOST_TEST(ri2 == f);
    BOOST_TEST(boost::ref(ri2) != f);
    BOOST_TEST(!(f != ri2));
    BOOST_TEST(!(f == boost::ref(ri2)));
    BOOST_TEST(!(ri2 != f));
    BOOST_TEST(!(boost::ref(ri2) == f));
  }
#endif
}

int test_main(int, char*[])
{
  target_test();
  equal_test();
  ref_equal_test();

  return 0;
}
