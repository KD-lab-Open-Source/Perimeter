/*=============================================================================
    Copyright (C) 1999-2003 Jaakko Järvi
    Copyright (c) 2003 Joel de Guzman

    Use, modification and distribution is subject to the Boost Software
    License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#include <boost/test/minimal.hpp>
#include <boost/spirit/fusion/sequence/tuple.hpp>
#include <boost/spirit/fusion/sequence/get.hpp>

namespace
{
    // something to prevent warnings for unused variables
    template<class T> void dummy(const T&) {}

    // no public default constructor
    class foo
    {
    public:

        explicit foo(int v) : val(v) {}

        bool operator==(const foo& other) const
        {
            return val == other.val;
        }

    private:

        foo() {}
        int val;
    };

    // another class without a public default constructor
    class no_def_constructor
    {
        no_def_constructor() {}

    public:

        no_def_constructor(std::string) {}
    };
}

int
test_main(int, char *[])
{
    using namespace boost::fusion;

    tuple<int> t1;
    BOOST_TEST(get<0>(t1) == int());

    tuple<float> t2(5.5f);
    BOOST_TEST(get<0>(t2) > 5.4f && get<0>(t2) < 5.6f);

    tuple<foo> t3(foo(12));
    BOOST_TEST(get<0>(t3) == foo(12));

    tuple<double> t4(t2);
    BOOST_TEST(get<0>(t4) > 5.4 && get<0>(t4) < 5.6);

    tuple<int, float> t5;
    BOOST_TEST(get<0>(t5) == int());
    BOOST_TEST(get<1>(t5) == float());

    tuple<int, float> t6(12, 5.5f);
    BOOST_TEST(get<0>(t6) == 12);
    BOOST_TEST(get<1>(t6) > 5.4f && get<1>(t6) < 5.6f);

    tuple<int, float> t7(t6);
    BOOST_TEST(get<0>(t7) == 12);
    BOOST_TEST(get<1>(t7) > 5.4f && get<1>(t7) < 5.6f);

    tuple<long, double> t8(t6);
    BOOST_TEST(get<0>(t8) == 12);
    BOOST_TEST(get<1>(t8) > 5.4f && get<1>(t8) < 5.6f);

    dummy
    (
        tuple<no_def_constructor, no_def_constructor, no_def_constructor>(
            std::string("Jaba"),        // ok, since the default
            std::string("Daba"),        // constructor is not used
            std::string("Doo")
        )
    );

    dummy(tuple<int, double>());
    dummy(tuple<int, double>(1,3.14));

//  dummy(tuple<double&>());            // should fail, no defaults for references
//  dummy(tuple<const double&>());      // likewise

    {   //  JDG 10-21-2003: previously not availiable without
        //  partial template specialization. Now OK with VC6/7

        double dd = 5;
        dummy(tuple<double&>(dd)); // ok
        dummy(tuple<const double&>(dd+3.14)); // ok, but dangerous
    }

//  dummy(tuple<double&>(dd+3.14));     // should fail,
//                                      // temporary to non-const reference
    return 0;
}
