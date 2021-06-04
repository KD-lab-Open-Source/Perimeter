/*=============================================================================
    Copyright (c) 2003 Vaclav Vesely
    http://spirit.sourceforge.net/

    Use, modification and distribution is subject to the Boost Software
    License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#include <boost/spirit/core.hpp>
#include <boost/spirit/actor/assign_actor.hpp>
#include <boost/spirit/dynamic/lazy.hpp>
#include <boost/spirit/phoenix/primitives.hpp>
#include <boost/spirit/phoenix/operators.hpp>
#include <boost/test/included/unit_test_framework.hpp>

using namespace boost;
using namespace unit_test_framework;
using namespace spirit;
using namespace phoenix;

//-----------------------------------------------------------------------------

void lazy_p_test()
{
    int result;
    BOOST_CHECK(parse("123", lazy_p(val(int_p))[assign_a(result)]).full);
    BOOST_CHECK_EQUAL(result, 123);
}

//-----------------------------------------------------------------------------

test_suite* init_unit_test_suite(int /*argc*/, char* /*argv*/[])
{
    test_suite* test = BOOST_TEST_SUITE("lazy test");

    test->add(BOOST_TEST_CASE(&lazy_p_test));

    return test;
}

//-----------------------------------------------------------------------------
