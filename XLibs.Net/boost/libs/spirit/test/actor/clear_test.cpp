/*=============================================================================
    Copyright (c) 2003 Jonathan de Halleux (dehalleux@pelikhan.com)
    http://spirit.sourceforge.net/

    Use, modification and distribution is subject to the Boost Software
    License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/

///////////////////////////////////////////////////////////////////////////////
// Test suite for clear_actor
///////////////////////////////////////////////////////////////////////////////

#include <boost/spirit.hpp>
#include <vector>
#include <boost/spirit/actor/clear_actor.hpp>

#include <boost/test/unit_test.hpp>


void clear_test()
{
    using namespace boost::unit_test_framework;
    using namespace boost::spirit;

    BOOST_MESSAGE("clear_test");

    const char* cp = "63";
    const char* cp_first = cp;
    const char* cp_last = cp + strlen(cp);
    std::vector<int> c;
    c.push_back(1);

    scanner<char const*> scan( cp_first, cp_last );
    match<> hit;

    hit = int_p[ clear_a(c)].parse(scan);
    BOOST_CHECK(hit);
    BOOST_CHECK_EQUAL(scan.first, scan.last);
    BOOST_CHECK( c.empty() );
    scan.first = cp;
    c.push_back(1);

    hit = str_p("63")[ clear_a(c)].parse(scan);
    BOOST_CHECK(hit);
    BOOST_CHECK_EQUAL(scan.first, scan.last);
    BOOST_CHECK( c.empty() );
}

void clear_action_test(boost::unit_test_framework::test_suite* test)
{
    test->add( BOOST_TEST_CASE(&clear_test) );
}

