/*=============================================================================
    Copyright (c) 2002-2003 Martin Wille
    http://spirit.sourceforge.net/

    Use, modification and distribution is subject to the Boost Software
    License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#include <iostream>
#include <cstring>
#include <cassert>

// This test program only includes the epsilon.hpp header from Spirit
#include <boost/spirit/core/composite/epsilon.hpp>
#include <boost/test/included/unit_test_framework.hpp>
#include "impl/var.hpp"
#include "impl/util.ipp"

namespace ut = boost::unit_test_framework;
using namespace test;

static boost::spirit::parse_info<char const *> pi;

////////////////////////////////////////////////
// These macros are used with BOOST_CHECK
#define matches (pi.hit)
#define full_match (pi.hit && pi.full)
#define partial_match (pi.hit && !pi.full)
#define no_match (!pi.hit && !pi.full)
#define zero_length_match (pi.length == 0)
#define stop_equals_start (pi.stop == s)

template<typename ParserT>
static void
parse(char const *s, ParserT const &p, bool match)
{
    using namespace std;
    pi = boost::spirit::parse(s, s+std::strlen(s), p);
    if (match)
    {
        BOOST_CHECK(matches);
        BOOST_CHECK(zero_length_match);
        BOOST_CHECK(stop_equals_start);
    }
    else
    {
        BOOST_CHECK(no_match);
    }
}

static char const empty[] = "";
static char const not_empty[] = "asdfgh";

////////////////////////////////////////////////
// Test wether epsilon_p/eps_p work as
// primitive parsers
static void
epsilon_as_primitive()
{
    // This test case also is a compile time check wether
    // both eps_p and epsilon_p are present.

    parse(empty, boost::spirit::epsilon_p, true);
    BOOST_CHECK(full_match);
    parse(not_empty, boost::spirit::epsilon_p, true);
    BOOST_CHECK(partial_match);

    parse(empty, boost::spirit::eps_p, true);
    BOOST_CHECK(full_match);
    parse(not_empty, boost::spirit::eps_p, true);
    BOOST_CHECK(partial_match);
}

////////////////////////////////////////////////
// Test wether epsilon_p/eps_p work correctly as
// a parser generator for creating parsers from
// functors
static void
epsilon_as_parser_generator_for_functors()
{
    bool       flag = false;
    parse(empty, boost::spirit::epsilon_p(var(flag)), flag);
    BOOST_CHECK(no_match);

    flag = true;
    parse(empty, boost::spirit::epsilon_p(var(flag)), flag);
    BOOST_CHECK(full_match);
}

////////////////////////////////////////////////
// Test wether epsilon_p/eps_p work correctly as
// a parser generator for creating parsers from
// other parsers
static void
epsilon_as_parser_generator_for_parsers()
{
    // This test case uses a parser created by epsilon_p
    // as body-parser for another invokation of epsilon_p

    bool        flag = false;
    parse(empty, boost::spirit::epsilon_p(
            boost::spirit::epsilon_p(var(flag))), flag);
    BOOST_CHECK(no_match);

    flag = true;
    parse(empty, boost::spirit::epsilon_p(
            boost::spirit::epsilon_p(var(flag))), flag);
    BOOST_CHECK(full_match);
}

////////////////////////////////////////////////
// Test wether epsilon_p/eps_p support negation
static void
negation_operator_for_epsilon()
{
    bool       flag = false;
    parse(empty, ~boost::spirit::epsilon_p(var(flag)), !flag);
    BOOST_CHECK(full_match);
    parse(empty, ~~boost::spirit::epsilon_p(var(flag)), flag);
    BOOST_CHECK(no_match);

    flag = true;
    parse(empty, ~boost::spirit::epsilon_p(var(flag)), !flag);
    BOOST_CHECK(no_match);
    parse(empty, ~~boost::spirit::epsilon_p(var(flag)), flag);
    BOOST_CHECK(full_match);
}

////////////////////////////////////////////////
// Definition of the test suite
ut::test_suite*
init_unit_test_suite( int argc, char* argv[] )
{
    test::init(argc, argv);
    test::banner("epsilon_tests");

    ut::test_suite* test= BOOST_TEST_SUITE( "spirit::epsilon tests" );

    test->add(BOOST_TEST_CASE(epsilon_as_primitive));
    test->add(BOOST_TEST_CASE(epsilon_as_parser_generator_for_functors));
    test->add(BOOST_TEST_CASE(epsilon_as_parser_generator_for_parsers));
    test->add(BOOST_TEST_CASE(negation_operator_for_epsilon));

    return test;
}
