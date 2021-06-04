/*=============================================================================
    Copyright (c) 2003 Jonathan de Halleux (dehalleux@pelikhan.com)
    http://spirit.sourceforge.net/

    Use, modification and distribution is subject to the Boost Software
    License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/

///////////////////////////////////////////////////////////////////////////////
// Test suite for assign_actor
///////////////////////////////////////////////////////////////////////////////

#include <boost/spirit.hpp>
#include <boost/test/unit_test.hpp>

#include <boost/spirit/actor/assign_actor.hpp>

void assign_test()
{
    using namespace boost::unit_test_framework;
    using namespace boost::spirit;

    const char* cp = "63";
    const char* cp_first = cp;
    const char* cp_last = cp + strlen(cp);
    int h=127;
    int hm=h;

    scanner<char const*> scan( cp_first, cp_last );
    match<> hit;

    hit = int_p[ assign_a(hm)].parse(scan);
    BOOST_CHECK(hit);
    BOOST_CHECK_EQUAL(scan.first, scan.last);

    h=63;
    BOOST_CHECK_EQUAL( hm,h);
}

void assign_test_ref()
{
    using namespace boost::unit_test_framework;
    using namespace boost::spirit;

    const char* cp = "63";
    const char* cp_first = cp;
    const char* cp_last = cp + strlen(cp);
    int h=127;
    int hm=63;

    scanner<char const*> scan( cp_first, cp_last );
    match<> hit;

    hit = int_p[ assign_a(h,hm)].parse(scan);
    BOOST_CHECK(hit);
    BOOST_CHECK_EQUAL(scan.first, scan.last);

    BOOST_CHECK_EQUAL( hm,h);
}

void assign_action_test(boost::unit_test_framework::test_suite* test)
{
    test->add( BOOST_TEST_CASE(&assign_test) );
    test->add( BOOST_TEST_CASE(&assign_test_ref) );
}

