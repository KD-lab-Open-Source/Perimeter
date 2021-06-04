/*=============================================================================
    Copyright (c) 1998-2003 Joel de Guzman
    http://spirit.sourceforge.net/

    Use, modification and distribution is subject to the Boost Software
    License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#include <iostream>
#include <cassert>
#include <string>

using namespace std;

#include <boost/spirit/core.hpp>
using namespace boost::spirit;

///////////////////////////////////////////////////////////////////////////////
//
//  Match tests
//
///////////////////////////////////////////////////////////////////////////////
struct X {};
struct Y { Y(int) {} }; // not default constructible
struct Z { Z(double n):n(n){} double n; }; // implicitly convertible from double

void
match_tests()
{
    match<> m0;
    assert(!m0.has_valid_attribute());

    match<int> m1(m0);
    m1.value(123);
    assert(m1.has_valid_attribute());
    assert(m1.value() == 123);

    match<double> m2(m1);
    assert(m2.has_valid_attribute());
    assert(m1.value() == int(m2.value()));
    m2.value(456);

    m0 = m0; // match<nil> = match<nil>
    m0 = m1; // match<nil> = match<int>
    m0 = m2; // match<nil> = match<double>
    m1 = m0; // match<int> = match<nil>
    assert(!m1);
    assert(!m1.has_valid_attribute());

    m1 = m1; // match<int> = match<int>
    m1.value(int(m2.value()));
    assert(m1.has_valid_attribute());
    assert(m1.value() == int(m2.value()));

    m2.value(123.456);
    match<Z> mz(m2); // copy from match<double>
    mz = m2; // assign from match<double>
    assert(mz.value().n == 123.456);

    m1.value(123);
    m2 = m0;
    assert(!m2);
    assert(!m2.has_valid_attribute());

    m2 = m1; // match<double> = match<int>
    assert(m2.has_valid_attribute());
    assert(m1.value() == int(m2.value()));
    m2 = m2; // match<double> = match<double>

    cout << "sizeof(int) == " << sizeof(int) << '\n';
    cout << "sizeof(match<>) == " << sizeof(m0) << '\n';
    cout << "sizeof(match<int>) == " << sizeof(m1) << '\n';
    cout << "sizeof(match<double>) == " << sizeof(m2) << '\n';

    match<int&> mr;
    assert(!mr.has_valid_attribute());

    match<int&> mrr(4);
    assert(!mrr.has_valid_attribute());

    int ri = 3;
    match<int&> mr2(4, ri);
    assert(mr2.has_valid_attribute());
    mr = mr2;
    assert(mr.has_valid_attribute());

    match<int&> mr3(mrr);
    assert(!mr3.has_valid_attribute());
    mr2 = mr3;
    assert(!mr2.has_valid_attribute());

    match<X> mx;
    m1 = mx;
    m0 = mx;
    assert(!mx.has_valid_attribute());
    assert(!m0.has_valid_attribute());
    assert(!m1.has_valid_attribute());

    match<Y> my;
    assert(!my.has_valid_attribute());

    match<std::string> ms;
    assert(!ms.has_valid_attribute());
    ms.value("Kimpo Ponchwayla");
    assert(ms.has_valid_attribute());
    assert(ms.value() == "Kimpo Ponchwayla");
    ms = match<>();
    assert(!ms.has_valid_attribute());

    // let's try a match with a reference:
    int i;
    match<int&> mr4(4, i);
    assert(mr4.has_valid_attribute());
    mr4.value(3);
    assert(mr4.value() == 3);
    assert(i == 3);
    (void)i;

    int x = 456;
    match<int&> mref(1, x);
    assert(mref.value() == 456);
    mref.value(123);
    assert(mref.value() == 123);
    x = mref.value();
    assert(x == 123);
    mref.value() = 986;
    assert(x == 986);
}

///////////////////////////////////////////////////////////////////////////////
//
//  Match Policy tests
//
///////////////////////////////////////////////////////////////////////////////
void
match_policy_tests()
{
    match<>         m0;
    match<int>      m1;
    match<double>   m2;
    match_policy    mp;

    m0 = mp.no_match();     assert(!m0);
    m1 = mp.no_match();     assert(!m1);
    m0 = mp.empty_match();  assert(m0);
    m2 = mp.empty_match();  assert(m2);

    m1 = mp.create_match(5, 100, 0, 0);
    m2 = mp.create_match(5, 10.5, 0, 0);

    mp.concat_match(m1, m2);
    assert(m1.length() == 10);
}

///////////////////////////////////////////////////////////////////////////////
//
//  Main
//
///////////////////////////////////////////////////////////////////////////////
int
main()
{
    match_tests();
    match_policy_tests();
    cout << "Tests concluded successfully\n";
    return 0;
}

