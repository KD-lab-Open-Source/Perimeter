/*=============================================================================
    Copyright (c) 2004 João Abecasis
    Copyright (c) 2004 Joel de Guzman
    http://spirit.sourceforge.net/

    Use, modification and distribution is subject to the Boost Software
    License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#include <boost/spirit/core.hpp>
#include <boost/spirit/dynamic.hpp>
#include <boost/spirit/phoenix.hpp>
#include <boost/test/included/unit_test_framework.hpp>

#include <iostream>
#include <string>

using namespace boost::spirit;
using namespace boost::unit_test_framework;
using namespace phoenix;

void negated_eps_test()
{
    using std::cout;
    using std::endl;
    using std::string;

    bool f = false;

    rule<> start =
    while_p(~eps_p(anychar_p[var(f) = true]))
    [
        anychar_p
    ];

    parse("This goes to prove my point.", start);
    BOOST_CHECK(f == false);
}

test_suite*
init_unit_test_suite(int /*argc*/, char* /*argv*/[])
{
    test_suite* test = BOOST_TEST_SUITE("negated eps test");
    test->add(BOOST_TEST_CASE(&negated_eps_test));
    return test;
}
