/*=============================================================================
    Copyright (c) 2003 Martin Wille
    http://spirit.sourceforge.net/

    Use, modification and distribution is subject to the Boost Software
    License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#include <iostream>
#include <boost/config.hpp>
#include <boost/test/included/unit_test_framework.hpp>
#include "impl/util.ipp"

namespace ut = boost::unit_test_framework;
using namespace test;

static char const test_banner_name[]="grammar tests (MT)";
static char const test_suite_name[]="spirit::grammar_tests (MT)";

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

#include <boost/thread/thread.hpp>
#include <boost/spirit/core/non_terminal/grammar.hpp>
#include <boost/spirit/core/non_terminal/rule.hpp>
#include <boost/spirit/core/composite/epsilon.hpp>
#include <boost/thread/xtime.hpp>
#include <boost/ref.hpp>

static boost::mutex simple_mutex;
static int simple_definition_count = 0;

struct simple : public boost::spirit::grammar<simple>
{
    template <typename ScannerT>
    struct definition
    {
        definition(simple const& /*self*/)
        {
            top = boost::spirit::epsilon_p;
            boost::mutex::scoped_lock lock(simple_mutex);
            simple_definition_count++;
        }

        boost::spirit::rule<ScannerT> top;
        boost::spirit::rule<ScannerT> const &start() const { return top; }
    };
};

struct count_guard
{
    count_guard(int &c) : counter(c) {}
    ~count_guard() { counter = 0; }
private:
    int &counter;
};

static void
milli_sleep(unsigned long milliseconds)
{
    static long const nanoseconds_per_second = 1000L*1000L*1000L;
    boost::xtime xt;
    boost::xtime_get(&xt, boost::TIME_UTC);
    xt.nsec+=1000*1000*milliseconds;
    while (xt.nsec > nanoseconds_per_second)
    {
        xt.nsec -= nanoseconds_per_second;
        xt.sec++;
    }
        
    boost::thread::sleep(xt);
}

static void
nap()
{
    // this function is called by various threads to ensure
    // that thread lifetime actually overlap
    milli_sleep(300);
}

template <typename GrammarT>
static void
make_definition(GrammarT &g)
{
    char const *text="blah";
    boost::spirit::scanner<> s(text, text+4);

    g.parse(s);
}

template <typename GrammarT>
static void
make_definition3(GrammarT &g)
{
    char const *text="blah";
    boost::spirit::scanner<> s(text, text+4);

    g.parse(s);
    nap();
    g.parse(s);
    g.parse(s);
}
////////////////////////////////////////////////////////////////////////////////
#define exactly_one_instance_created simple_definition_count == 1
#define exactly_two_instances_created simple_definition_count == 2
#define exactly_four_instances_created simple_definition_count == 4
#define exactly_eight_instances_created simple_definition_count == 8

////////////////////////////////////////////////////////////////////////////////
static void
multiple_attempts_to_instantiate_a_definition_from_a_single_thread()
{
    // checks wether exactly one definition per grammar
    // object is created

    count_guard guard(simple_definition_count);

    simple simple1_p;
    simple simple2_p;

    make_definition(simple1_p);
    make_definition(simple1_p);
    make_definition(simple1_p);

    BOOST_CHECK(exactly_one_instance_created);

    make_definition(simple2_p);
    make_definition(simple2_p);
    make_definition(simple2_p);

    BOOST_CHECK(exactly_two_instances_created);
}

////////////////////////////////////////////////////////////////////////////////
struct single_grammar_object_task
{
    void operator()() const
    {
        make_definition3(simple1_p);
    };

    simple simple1_p;
};

static void
single_local_grammar_object_multiple_threads()
{
    // check wether independent definition objects are
    // created
    count_guard guard(simple_definition_count);
    single_grammar_object_task task1, task2, task3, task4;

    boost::thread t1(boost::ref(task1));
    boost::thread t2(boost::ref(task2));
    boost::thread t3(boost::ref(task3));
    boost::thread t4(boost::ref(task4));

    t1.join();
    t2.join();
    t3.join();
    t4.join();

    BOOST_CHECK(exactly_four_instances_created);
}

////////////////////////////////////////////////////////////////////////////////
struct two_grammar_objects_task
{
    void operator()() const
    {
        make_definition3(simple1_p);
        make_definition3(simple2_p);
    };

    simple simple1_p;
    simple simple2_p;
};

static void
multiple_local_grammar_objects_multiple_threads()
{
    // check wether exactly one definition per thread 
    // and per grammar object is created
    count_guard guard(simple_definition_count);
    two_grammar_objects_task task1, task2, task3, task4;

    boost::thread t1(boost::ref(task1));
    boost::thread t2(boost::ref(task2));
    boost::thread t3(boost::ref(task3));
    boost::thread t4(boost::ref(task4));

    t1.join();
    t2.join();
    t3.join();
    t4.join();

    BOOST_CHECK(exactly_eight_instances_created);
}

////////////////////////////////////////////////////////////////////////////////
static simple global_simple1_p;

struct single_global_grammar_object_task
{
    void operator()() const
    {
        make_definition3(global_simple1_p);
    };
};

static void
single_global_grammar_object_multiple_threads()
{
    // check wether exactly one definition per thread is
    // created
    count_guard guard(simple_definition_count);
    single_global_grammar_object_task task1, task2, task3, task4;

    boost::thread t1(boost::ref(task1));
    boost::thread t2(boost::ref(task2));
    boost::thread t3(boost::ref(task3));
    boost::thread t4(boost::ref(task4));

    t1.join();
    t2.join();
    t3.join();
    t4.join();

    BOOST_CHECK(exactly_four_instances_created);
}

////////////////////////////////////////////////////////////////////////////////
static simple global_simple2_p;
static simple global_simple3_p;

struct multiple_global_grammar_objects_task
{
    void operator()() const
    {
        make_definition3(global_simple2_p);
        make_definition3(global_simple3_p);
    };
};

static void
multiple_global_grammar_objects_multiple_threads()
{
    // check wether exactly one definition per thread 
    // and per grammar object is created
    count_guard guard(simple_definition_count);
    multiple_global_grammar_objects_task task1, task2, task3, task4;

    boost::thread t1(boost::ref(task1));
    boost::thread t2(boost::ref(task2));
    boost::thread t3(boost::ref(task3));
    boost::thread t4(boost::ref(task4));

    t1.join();
    t2.join();
    t3.join();
    t4.join();

    BOOST_CHECK(exactly_eight_instances_created);
}
////////////////////////////////////////////////////////////////////////////////
ut::test_suite *
init_unit_test_suite( int argc, char *argv[] )
{
    test::init(argc, argv);
    test::banner(test_banner_name);

    ut::test_suite *suite = BOOST_TEST_SUITE(test_suite_name);

    suite->add(BOOST_TEST_CASE(multiple_attempts_to_instantiate_a_definition_from_a_single_thread));
    suite->add(BOOST_TEST_CASE(single_local_grammar_object_multiple_threads));
    suite->add(BOOST_TEST_CASE(multiple_local_grammar_objects_multiple_threads));
    suite->add(BOOST_TEST_CASE(single_global_grammar_object_multiple_threads));
    suite->add(BOOST_TEST_CASE(multiple_global_grammar_objects_multiple_threads));

    return suite;
}

////////////////////////////////////////////////////////////////////////////////

static boost::spirit::parse_info<char const *> pi;

////////////////////////////////////////////////
// These macros are used with BOOST_CHECK



#endif // MT mode
