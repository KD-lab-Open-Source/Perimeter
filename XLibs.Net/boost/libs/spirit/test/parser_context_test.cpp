/*=============================================================================
    Copyright (c) 2003 Vaclav Vesely
    http://spirit.sourceforge.net/

    Use, modification and distribution is subject to the Boost Software
    License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#include <boost/test/included/unit_test_framework.hpp>
#include <boost/spirit/core.hpp>
#include <boost/spirit/actor/assign_actor.hpp>

using namespace boost;
using namespace spirit;
using namespace unit_test_framework;

//-----------------------------------------------------------------------------

void attr_parser_context_test()
{
    rule<parser_context<int> > int_rule = int_p;
    int result(0);
    parse("123", int_rule[assign_a(result)]); 
    BOOST_CHECK(result == 123);
}

//-----------------------------------------------------------------------------

test_suite* init_unit_test_suite(int /*argc*/, char* /*argv*/[])
{
    test_suite* test = BOOST_TEST_SUITE("attr_parser_context test");

    test->add(BOOST_TEST_CASE(&attr_parser_context_test));

    return test;
}

//-----------------------------------------------------------------------------
