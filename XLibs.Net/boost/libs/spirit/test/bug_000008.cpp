/*=============================================================================
    Copyright (c) 2003 Martin Wille
    http://spirit.sourceforge.net/

    Use, modification and distribution is subject to the Boost Software
    License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/

  // see http://article.gmane.org/gmane.comp.parsers.spirit.general/4575
  // or https://sf.net/mailarchive/forum.php?thread_id=2692308&forum_id=1595
  // for a description of the bug being tested for by this program
  //
  // the problem should be solved with version 1.3 of phoenix/closures.hpp

#if defined(BOOST_SPIRIT_DEBUG) && defined(__GNUC__) && defined(__WIN32__)
// It seems that MinGW has some problems with threads and iostream ?
// This code crashes MinGW when BOOST_SPIRIT_DEBUG is defined. The reason
// is beyond me. Disable BOOST_SPIRIT_DEBUG for now.
#undef BOOST_SPIRIT_DEBUG
#endif

#include <iostream>
#include <boost/config.hpp>
#include <boost/test/included/unit_test_framework.hpp>
#include "impl/util.ipp"

namespace ut = boost::unit_test_framework;
using namespace test;

static char const test_banner_name[]="bug_000008 (closure MT) test";
static char const test_suite_name[]="spirit::bug_000008";

#if defined(DONT_HAVE_BOOST) || !defined(BOOST_HAS_THREADS)
// we end here if we can't do multithreading
static void skipped()
{
    if (test::verbose_runtests)
        std::cout << "skipped\n";
}

ut::test_suite*
init_unit_test_suite( int argc, char* argv[] )
{
    test::init(argc, argv);
    test::banner(test_banner_name);
    ut::test_suite* test= BOOST_TEST_SUITE(test_suite_name);
    test->add(BOOST_TEST_CASE(skipped));
    return test;
}

#else
// the real MT stuff

#undef BOOST_SPIRIT_THREADSAFE
#define BOOST_SPIRIT_THREADSAFE
#undef PHOENIX_THREADSAFE
#define PHOENIX_THREADSAFE

#include <boost/spirit/core.hpp>
#include <boost/spirit/attribute/closure.hpp>
#include <boost/thread.hpp>

static const int number_of_calls_to_parse_per_thread=20000;

struct test_closure
    : boost::spirit::closure<test_closure, char const*>
{
    member1 b;
};

struct test_grammar
    : boost::spirit::grammar<test_grammar, test_closure::context_t>
{
    test_grammar() {}

    template <typename ScannerT>
    struct definition
    {
        definition(test_grammar const &self)
        {
            using namespace phoenix;
            rule = boost::spirit::epsilon_p[self.b = arg1];
        }

        boost::spirit::rule<ScannerT> const &start() const { return rule; }

        boost::spirit::rule<ScannerT> rule;
    };
};

test_grammar const g;

void
in_thread(void)
{
    char const text[]="foo";
    for(int i=0; i<number_of_calls_to_parse_per_thread; ++i)
    {
        boost::spirit::parse(&text[0], text+sizeof(text), g);
    }
}

void
bug_000008()
{
    boost::thread t1(in_thread);
    boost::thread t2(in_thread);
    boost::thread t3(in_thread);
    boost::thread t4(in_thread);

    t1.join();
    t2.join();
    t3.join();
    t4.join();
}

ut::test_suite *
init_unit_test_suite( int argc, char *argv[] )
{
    test::init(argc, argv);
    test::banner(test_banner_name);

    ut::test_suite *suite = BOOST_TEST_SUITE(test_suite_name);

    suite->add(BOOST_TEST_CASE(bug_000008));
    return suite;
}

#endif
