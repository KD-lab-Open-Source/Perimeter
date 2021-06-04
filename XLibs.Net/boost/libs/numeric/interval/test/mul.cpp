/* Boost test/mul.cpp
 * test multiplication, division, square and square root on some intervals
 *
 * Copyright Guillaume Melquiond 2002-2003
 * Permission to use, copy, modify, sell, and distribute this software
 * is hereby granted without fee provided that the above copyright notice
 * appears in all copies and that both that copyright notice and this
 * permission notice appear in supporting documentation.
 *
 * None of the above authors nor Polytechnic University make any
 * representation about the suitability of this software for any
 * purpose. It is provided "as is" without express or implied warranty.
 *
 * $Id: mul.cpp,v 1.8 2004/02/26 18:26:59 eric_niebler Exp $
 */

#include <boost/numeric/interval.hpp>
#include <boost/test/minimal.hpp>
#include <boost/minmax.hpp>
#include "bugs.hpp"

typedef boost::numeric::interval<double> I;

static double min BOOST_PREVENT_MACRO_SUBSTITUTION (double a, double b, double c, double d) {
  return boost::std_min(boost::std_min(a, b), boost::std_min(c, d));
}

static double max BOOST_PREVENT_MACRO_SUBSTITUTION (double a, double b, double c, double d) {
  return boost::std_max(boost::std_max(a, b), boost::std_max(c, d));
}

static bool test_mul(double al, double au, double bl, double bu) {
  I a(al, au), b(bl, bu);
  I c = a * b;
  return c.lower() == (min)(al*bl, al*bu, au*bl, au*bu)
      && c.upper() == (max)(al*bl, al*bu, au*bl, au*bu);
}

static bool test_mul1(double ac, double bl, double bu) {
  I a(ac), b(bl, bu);
  I c = ac * b;
  I d = b * ac;
  I e = a * b;
  return equal(c, d) && equal(d, e);
}

static bool test_div(double al, double au, double bl, double bu) {
  I a(al, au), b(bl, bu);
  I c = a / b;
  return c.lower() == (min)(al/bl, al/bu, au/bl, au/bu)
      && c.upper() == (max)(al/bl, al/bu, au/bl, au/bu);
}

static bool test_div1(double al, double au, double bc) {
  I a(al, au), b(bc);
  I c = a / bc;
  I d = a / b;
  return equal(c, d);
}

static bool test_div2(double ac, double bl, double bu) {
  I a(ac), b(bl, bu);
  I c = ac / b;
  I d = a / b;
  return equal(c, d);
}

static bool test_square(double al, double au) {
  I a(al, au);
  I b = square(a);
  I c = a * a;
  return b.upper() == c.upper() &&
         (b.lower() == c.lower() || (c.lower() <= 0 && b.lower() == 0));
}

static bool test_sqrt(double al, double au) {
  I a(al, au);
  I b = square(sqrt(a));
  return subset(abs(a), b);
}

int test_main(int, char*[]) {
  BOOST_CHECK(test_mul(2, 3, 5, 7));
  BOOST_CHECK(test_mul(2, 3, -5, 7));
  BOOST_CHECK(test_mul(2, 3, -7, -5));
  BOOST_CHECK(test_mul(-2, 3, 5, 7));
  BOOST_CHECK(test_mul(-2, 3, -5, 7));
  BOOST_CHECK(test_mul(-2, 3, -7, -5));
  BOOST_CHECK(test_mul(-3, -2, 5, 7));
  BOOST_CHECK(test_mul(-3, -2, -5, 7));
  BOOST_CHECK(test_mul(-3, -2, -7, -5));

  BOOST_CHECK(test_mul1(3, 5, 7));
  BOOST_CHECK(test_mul1(3, -5, 7));
  BOOST_CHECK(test_mul1(3, -7, -5));
  BOOST_CHECK(test_mul1(-3, 5, 7));
  BOOST_CHECK(test_mul1(-3, -5, 7));
  BOOST_CHECK(test_mul1(-3, -7, -5));

  BOOST_CHECK(test_div(30, 42, 2, 3));
  BOOST_CHECK(test_div(30, 42, -3, -2));
  BOOST_CHECK(test_div(-30, 42, 2, 3));
  BOOST_CHECK(test_div(-30, 42, -3, -2));
  BOOST_CHECK(test_div(-42, -30, 2, 3));
  BOOST_CHECK(test_div(-42, -30, -3, -2));

  BOOST_CHECK(test_div1(30, 42, 3));
  BOOST_CHECK(test_div1(30, 42, -3));
  BOOST_CHECK(test_div1(-30, 42, 3));
  BOOST_CHECK(test_div1(-30, 42, -3));
  BOOST_CHECK(test_div1(-42, -30, 3));
  BOOST_CHECK(test_div1(-42, -30, -3));

  BOOST_CHECK(test_div2(30, 2, 3));
  BOOST_CHECK(test_div2(30, -3, -2));
  BOOST_CHECK(test_div2(-30, 2, 3));
  BOOST_CHECK(test_div2(-30, -3, -2));

  BOOST_CHECK(test_square(2, 3));
  BOOST_CHECK(test_square(-2, 3));
  BOOST_CHECK(test_square(-3, 2));

  BOOST_CHECK(test_sqrt(2, 3));
  BOOST_CHECK(test_sqrt(5, 7));
  BOOST_CHECK(test_sqrt(-1, 2));

# ifdef __BORLANDC__
  ::detail::ignore_warnings();
# endif
  return 0;
}
