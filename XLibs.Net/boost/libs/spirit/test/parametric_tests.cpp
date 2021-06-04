/*=============================================================================
    Copyright (c) 2001-2003 Joel de Guzman
    Copyright (c)      2003 Martin Wille
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
#include <boost/spirit/attribute/parametric.hpp>
#include <boost/spirit/phoenix/primitives.hpp>
#include <boost/spirit/phoenix/operators.hpp>
using namespace boost::spirit;
using namespace phoenix;

#include <boost/test/included/unit_test_framework.hpp>
#include "impl/util.ipp"

namespace ut = boost::unit_test_framework;

///////////////////////////////////////////////////////////////////////////////
//
//  Parametric tests
//
///////////////////////////////////////////////////////////////////////////////

template <typename T>
static unsigned
length(T const *p)
{
    unsigned result = 0;
    while (*p++)
        ++result;
    return result;
}

template <typename T>
bool
is_equal(T const* a, T const* b)
{
    while (*a && *b)
        if (*a++ != *b++)
            return false;
    return true;
}

typedef rule< scanner<wchar_t const *> > wrule_t;

void
narrow_f_ch_p()
{
    char ch;
    rule<> r = anychar_p[var(ch) = arg1] >> *f_ch_p(const_(ch));
    parse_info<char const*> pi;

    pi = parse("aaaaaaaaa", r);
    BOOST_CHECK(pi.hit);
    BOOST_CHECK(pi.full);

    pi = parse("aaaaabaaa", r);
    BOOST_CHECK(pi.hit);
    BOOST_CHECK(!pi.full);
    BOOST_CHECK(is_equal(pi.stop, "baaa"));
}

void
wide_f_ch_p()
{
    wchar_t ch;
    wrule_t r = anychar_p[var(ch) = arg1] >> *f_ch_p(const_(ch));
    parse_info<wchar_t const*> pi;

    pi = parse(L"aaaaaaaaa", r);
    BOOST_CHECK(pi.hit);
    BOOST_CHECK(pi.full);

    pi = parse(L"aaaaabaaa", r);
    BOOST_CHECK(pi.hit);
    BOOST_CHECK(!pi.full);
    BOOST_CHECK(is_equal(pi.stop, L"baaa"));
}

void
narrow_f_range_p()
{
    char from = 'a';
    char to = 'z';

    parse_info<char const*> pi;

    rule<> r2 = *f_range_p(const_(from), const_(to));
    pi = parse("abcdefghijklmnopqrstuvwxyz", r2);
    BOOST_CHECK(pi.hit);
    BOOST_CHECK(pi.full);

    pi = parse("abcdefghijklmnopqrstuvwxyz123", r2);
    BOOST_CHECK(pi.hit);
    BOOST_CHECK(!pi.full);
    BOOST_CHECK(is_equal(pi.stop, "123"));
}

void
wide_f_range_p()
{
    wchar_t from = L'a';
    wchar_t to = L'z';

    parse_info<wchar_t const*> pi;

    wrule_t r2 = *f_range_p(const_(from), const_(to));
    pi = parse(L"abcdefghijklmnopqrstuvwxyz", r2);
    BOOST_CHECK(pi.hit);
    BOOST_CHECK(pi.full);

    pi = parse(L"abcdefghijklmnopqrstuvwxyz123", r2);
    BOOST_CHECK(pi.hit);
    BOOST_CHECK(!pi.full);
    BOOST_CHECK(is_equal(pi.stop, L"123"));
}

void
narrow_f_str_p()
{
    parse_info<char const*> pi;

    char const* start = "kim";
    char const* end = start + length(start);
    rule<> r3 = +f_str_p(const_(start), const_(end));

    pi = parse("kimkimkimkimkimkimkimkimkim", r3);
    BOOST_CHECK(pi.hit);
    BOOST_CHECK(pi.full);

    pi = parse("kimkimkimkimkimkimkimkimkimmama", r3);
    BOOST_CHECK(pi.hit);
    BOOST_CHECK(!pi.full);
    BOOST_CHECK(is_equal(pi.stop, "mama"));

    pi = parse("joel", r3);
    BOOST_CHECK(!pi.hit);
}

void
wide_f_str_p()
{
    parse_info<wchar_t const*> pi;

    wchar_t const* start = L"kim";
    wchar_t const* end = start + length(start);
    wrule_t r3 = +f_str_p(const_(start), const_(end));

    pi = parse(L"kimkimkimkimkimkimkimkimkim", r3);
    BOOST_CHECK(pi.hit);
    BOOST_CHECK(pi.full);

    pi = parse(L"kimkimkimkimkimkimkimkimkimmama", r3);
    BOOST_CHECK(pi.hit);
    BOOST_CHECK(!pi.full);
    BOOST_CHECK(is_equal(pi.stop, L"mama"));

    pi = parse(L"joel", r3);
    BOOST_CHECK(!pi.hit);
}

///////////////////////////////////////////////////////////////////////////////
//
//  test suite
//
///////////////////////////////////////////////////////////////////////////////
ut::test_suite *
init_unit_test_suite(int argc, char *argv[])
{
    test::init(argc, argv);
    test::banner("parametric tests");
    ut::test_suite *suite = BOOST_TEST_SUITE("spirit::parametric_tests");

    suite->add(BOOST_TEST_CASE(narrow_f_ch_p));
    suite->add(BOOST_TEST_CASE(wide_f_ch_p));
    suite->add(BOOST_TEST_CASE(narrow_f_range_p));
    suite->add(BOOST_TEST_CASE(wide_f_range_p));
    suite->add(BOOST_TEST_CASE(narrow_f_str_p));
    suite->add(BOOST_TEST_CASE(wide_f_str_p));

    return suite;
}

