/*=============================================================================
    Copyright (c) 2002-2003 Martin Wille
    http://spirit.sourceforge.net/

    Use, modification and distribution is subject to the Boost Software
    License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
// vi:ts=4:sw=4:et
// Tests for boost::spirit::if_p
// [28-Dec-2002]
////////////////////////////////////////////////////////////////////////////////
#define qDebug 0
#include <iostream>
#include <cstring>
#if qDebug
#define BOOST_SPIRIT_DEBUG
#endif
#include <boost/spirit/core.hpp>
#include <boost/spirit/dynamic/if.hpp>
#include <boost/spirit/actor/assign_actor.hpp>
#include <boost/ref.hpp>

namespace local
{
    template <typename T>
    struct var_wrapper
        : public ::boost::reference_wrapper<T>
    {
        typedef ::boost::reference_wrapper<T> parent;

        explicit inline var_wrapper(T& t) : parent(t) {}

        inline T& operator()() const { return parent::get(); }
    };

    template <typename T>
    inline var_wrapper<T>
    var(T& t)
    {
        return var_wrapper<T>(t);
    }
}

typedef ::boost::spirit::rule<> rule_t;
typedef ::boost::spirit::rule<boost::spirit::no_actions_scanner<>::type >
    no_actions_rule_t;

unsigned int test_count = 0;
unsigned int error_count = 0;

unsigned int number_result;
static const unsigned int kError = 999;
static const bool good = true;
static const bool bad = false;

rule_t hex_prefix;
no_actions_rule_t oct_prefix;
rule_t hex_rule, oct_rule, dec_rule;

rule_t auto_number_rule;
rule_t hex_or_dec_number_rule;

void
test_number(char const *s, unsigned int wanted, rule_t const &r)
{
    using namespace std;
    ++test_count;

    number_result = wanted-1;
    ::boost::spirit::parse_info<> m = ::boost::spirit::parse(s, s+strlen(s), r);

    bool result = wanted == kError?(m.full?bad:good): (number_result==wanted);

    if (m.full && (m.length != strlen(s)))
        result = bad;


    if (result==good)
        cout << "PASSED";
    else
    {
        ++error_count;
        cout << "FAILED";
    }

    cout << ": \"" << s << "\" ==> ";
    if (number_result==wanted-1)
        cout << "<error>";
    else
        cout << number_result;

    cout << "\n";
}

int
main()
{
    using namespace std;
    using ::boost::spirit::if_p;
    using ::boost::spirit::uint_p;
    using ::boost::spirit::oct_p;
    using ::boost::spirit::hex_p;
    using ::boost::spirit::str_p;
    using ::boost::spirit::ch_p;
    using ::boost::spirit::assign_a;

    cout << "/////////////////////////////////////////////////////////\n";
    cout << "\n";
    cout << "          if_p test\n";
    cout << "\n";
    cout << "/////////////////////////////////////////////////////////\n";
    cout << "\n";

    bool    as_hex;

#if qDebug
    BOOST_SPIRIT_DEBUG_RULE(hex_prefix);
    BOOST_SPIRIT_DEBUG_RULE(hex_rule);
    BOOST_SPIRIT_DEBUG_RULE(oct_prefix);
    BOOST_SPIRIT_DEBUG_RULE(oct_rule);
    BOOST_SPIRIT_DEBUG_RULE(dec_rule);
    BOOST_SPIRIT_DEBUG_RULE(auto_number_rule);
    BOOST_SPIRIT_DEBUG_RULE(hex_or_dec_number_rule);
#endif

    hex_prefix = str_p("0x");
    oct_prefix = ch_p('0');

    hex_rule = hex_p[assign_a(number_result)];
    oct_rule = oct_p[assign_a(number_result)];
    dec_rule = uint_p[assign_a(number_result)];

    auto_number_rule =
        if_p(hex_prefix)
            [hex_rule]
        .else_p
        [
            if_p(::boost::spirit::eps_p(oct_prefix))
                [oct_rule]
            .else_p
                [dec_rule]
        ];

    hex_or_dec_number_rule =
        if_p(local::var(as_hex))[hex_prefix>>hex_rule].else_p[dec_rule];

    cout << "auto:\n";
    test_number("",   kError, auto_number_rule);
    test_number("0",       0, auto_number_rule);
    test_number("1",       1, auto_number_rule);
    test_number("00",      0, auto_number_rule);
    test_number("0x", kError, auto_number_rule);
    test_number("0x0",     0, auto_number_rule);
    test_number("0755",  493, auto_number_rule);
    test_number("0x100", 256, auto_number_rule);

    cout << "\ndecimal:\n";
    as_hex = false;
    test_number("",      kError, hex_or_dec_number_rule);
    test_number("100",      100, hex_or_dec_number_rule);
    test_number("0x100", kError, hex_or_dec_number_rule);
    test_number("0xff",  kError, hex_or_dec_number_rule);

    cout << "\nhexadecimal:\n";
    as_hex = true;
    test_number("",      kError, hex_or_dec_number_rule);
    test_number("0x100",    256, hex_or_dec_number_rule);
    test_number("0xff",     255, hex_or_dec_number_rule);

    //////////////////////////////////
    // tests for if_p without else-parser
    cout << "\nno-else:\n";
    rule_t r = if_p(::boost::spirit::eps_p('0'))[oct_rule];

    test_number("0", 0, r);

    ++test_count;
    ::boost::spirit::parse_info<> m = ::boost::spirit::parse("", r);
    if (!m.hit || !m.full || m.length!=0)
    {
        std::cout << "FAILED: \"\" ==> <error>\n";
        ++error_count;
    }
    else
        std::cout << "PASSED: \"\" ==> <empty match>\n";

    ++test_count;
    m = ::boost::spirit::parse("junk", r);
    if (!m.hit || m.full || m.length!=0)
    {
        std::cout << "FAILED: \"junk\" ==> <error>\n";
        ++error_count;
    }
    else
        std::cout << "PASSED: \"junk\" ==> <empty match>\n";


    //////////////////////////////////
    // report results
    std::cout << "\n    ";
    if (error_count==0)
        cout << "All " << test_count << " if_p-tests passed.\n"
             << "Test concluded successfully\n";
    else
        cout << error_count << " of " << test_count << " if_p-tests failed\n"
             << "Test failed\n";

    return error_count!=0;
}
