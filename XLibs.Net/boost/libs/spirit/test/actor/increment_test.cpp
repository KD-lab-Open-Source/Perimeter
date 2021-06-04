/*=============================================================================
    Copyright (c) 2003 Jonathan de Halleux (dehalleux@pelikhan.com)
    http://spirit.sourceforge.net/

    Use, modification and distribution is subject to the Boost Software
    License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/

///////////////////////////////////////////////////////////////////////////////
// Test suite for increment_actor
///////////////////////////////////////////////////////////////////////////////

#include <boost/spirit.hpp>
#include <boost/test/unit_test.hpp>

#include <boost/spirit/actor/increment_actor.hpp>

void increment_test()
{
    using namespace boost::unit_test_framework;
    using namespace boost::spirit;

    BOOST_MESSAGE("increment_test");

    const char* cp = "63";
    const char* cp_first = cp;
    const char* cp_last = cp + strlen(cp);
    int h=127;
    int hm=h;

    scanner<char const*> scan( cp_first, cp_last );
    match<> hit;

    hit = int_p[ increment_a(hm)].parse(scan);
    BOOST_CHECK(hit);
    BOOST_CHECK_EQUAL(scan.first, scan.last);

    ++h;
    BOOST_CHECK_EQUAL( hm,h);
}

void increment_action_test(boost::unit_test_framework::test_suite* test)
{
    test->add( BOOST_TEST_CASE(&increment_test) );
}

