/*=============================================================================
    Copyright (C) 1999-2003 Jaakko Järvi
    Copyright (c) 2003 Joel de Guzman

    Use, modification and distribution is subject to the Boost Software
    License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#include <boost/test/minimal.hpp>
#include <boost/spirit/fusion/sequence/tuple.hpp>
#include <boost/spirit/fusion/sequence/get.hpp>

namespace
{
    // classes with different kinds of conversions
    class AA {};
    class BB : public AA {};
    struct CC { CC() {} CC(const BB&) {} };
    struct DD { operator CC() const { return CC(); }; };
}

int
test_main(int, char *[])
{
    using namespace boost::fusion;

    tuple<int, char> t1(4, 'a');
    tuple<int, char> t2(5, 'b');
    t2 = t1;
    BOOST_TEST(get<0>(t1) == get<0>(t2));
    BOOST_TEST(get<1>(t1) == get<1>(t2));

    tuple<long, std::string> t3(2, "a");
    t3 = t1;
    BOOST_TEST((double)get<0>(t1) == get<0>(t3));
    BOOST_TEST(get<1>(t1) == get<1>(t3)[0]);

    // testing copy and assignment with implicit conversions
    // between elements testing tie

    tuple<char, BB*, BB, DD> t;
    tuple<int, AA*, CC, CC> a(t);
    a = t;

    int i; char c; double d;
    tuple<int&, char&, double&>(i, c, d) = tuple<int, char, double>(1, 'a', 5.5);

    BOOST_TEST(i==1);
    BOOST_TEST(c=='a');
    BOOST_TEST(d>5.4 && d<5.6);

    return 0;
}
