/*=============================================================================
    Copyright (C) 1999-2003 Jaakko Jдrvi
    Copyright (c) 2003 Joel de Guzman

    Use, modification and distribution is subject to the Boost Software
    License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#include <boost/test/minimal.hpp>
#include <boost/ref.hpp>
#include <boost/spirit/fusion/sequence/tuple.hpp>
#include <boost/spirit/fusion/sequence/get.hpp>
#include <boost/spirit/fusion/sequence/make_tuple.hpp>
#include <boost/spirit/fusion/sequence/equal_to.hpp>

namespace
{
    // something to prevent warnings for unused variables
    template<class T> void dummy(const T&) {}

    class A {};
    class B {};
}

void make_tuple_test() {}

int
test_main(int, char *[])
{
    using namespace boost::fusion;

    {
        tuple<int, char> t1 = make_tuple(5, 'a');
        BOOST_TEST(get<0>(t1) == 5);
        BOOST_TEST(get<1>(t1) == 'a');

        tuple<int, std::string> t2;
        t2 = make_tuple((short int)2, std::string("Hi"));
        BOOST_TEST(get<0>(t2) == 2);
        BOOST_TEST(get<1>(t2) == "Hi");
    }

    {   // This test was previously disallowed for non-PTS compilers.
        A a = A(); B b;
        const A ca = a;
        make_tuple(boost::cref(a), b);
        make_tuple(boost::ref(a), b);
        make_tuple(boost::ref(a), boost::cref(b));
        make_tuple(boost::ref(ca));
    }

    {   //  the result of make_tuple is assignable: $$$ LATER $$$
        BOOST_TEST(make_tuple(2, 4, 6) ==
            (make_tuple(1, 2, 3) = make_tuple(2, 4, 6)));
    }

    {   // This test was previously disallowed for non-PTS compilers.
        make_tuple("Donald", "Daisy"); // should work;
    //  std::make_pair("Doesn't","Work"); // fails
    }

#if !BOOST_WORKAROUND(BOOST_MSVC, <= 1200)
    {   // This test works on VC7 now, only VC6 fails
        // You can store a reference to a function in a tuple
        tuple<void(&)()> adf(make_tuple_test);
        dummy(adf); // avoid warning for unused variable
    }
#endif

    {
        //  But make_tuple doesn't work
        //  with function references, since it creates a const
        //  qualified function type

    //  make_tuple(make_tuple_test);
    }

    {
        // With function pointers, make_tuple works just fine
        // This test works on Borland now

        make_tuple(&make_tuple_test);
    }

// NOTE:
//
// wrapping the function reference with ref helps on gcc 2.95.2.
// on edg 2.43. it results in a catastrophic error?

// make_tuple(ref(foo3));

// It seems that edg can't use implicitly the ref's conversion operator, e.g.:
// typedef void (&func_t) (void);
// func_t fref = static_cast<func_t>(ref(make_tuple_test)); // works fine
// func_t fref = ref(make_tuple_test);                      // error

// This is probably not a very common situation, so currently
// I don't know how which compiler is right (JJ)

    return 0;
}
