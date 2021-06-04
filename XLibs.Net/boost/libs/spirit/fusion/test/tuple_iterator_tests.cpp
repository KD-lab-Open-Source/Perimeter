/*=============================================================================
    Copyright (c) 2001-2003 Joel de Guzman

    Use, modification and distribution is subject to the Boost Software
    License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#include <iostream>
#include <boost/test/minimal.hpp>
#include <boost/type_traits/is_same.hpp>
#include <boost/spirit/fusion/sequence/tuple.hpp>
#include <boost/spirit/fusion/iterator/tuple_iterator.hpp>
#include <boost/spirit/fusion/iterator/deref.hpp>
#include <boost/spirit/fusion/iterator/next.hpp>
#include <boost/spirit/fusion/iterator/prior.hpp>
#include <boost/spirit/fusion/iterator/equal_to.hpp>
#include <boost/spirit/fusion/sequence/begin.hpp>
#include <boost/spirit/fusion/sequence/end.hpp>
#include <boost/spirit/fusion/sequence/tuple_element.hpp>
#include <boost/spirit/fusion/sequence/get.hpp>
//#include <boost/spirit/fusion/distance.hpp>
//#include <boost/spirit/fusion/advance.hpp>
//#include <boost/spirit/fusion/get_pointer.hpp>
//#include <boost/spirit/fusion/value_of.hpp>
//#include <boost/spirit/fusion/pointer_of.hpp>
//#include <boost/spirit/fusion/reference_of.hpp>
//#include <boost/spirit/fusion/is_readable.hpp>
//#include <boost/spirit/fusion/is_writable.hpp>
//#include <boost/spirit/fusion/category_of.hpp>

int
test_main(int, char*[])
{
    using namespace boost::fusion;
    using namespace boost;

/// Testing the tuple_iterator

    { // testing deref, next, prior, begin, end

        char const* s = "Hello";
        typedef tuple<int, char, double, char const*> tuple_type;
        tuple_type t(1, 'x', 3.3, s);
        tuple_iterator<0, tuple_type> i(t);

#if BOOST_WORKAROUND(BOOST_MSVC, <= 1200)
        // ?%$# VC6 I don't know why this is needed! $$$LOOK$$$
        next(i);
#endif

        BOOST_TEST(*i == 1);
        BOOST_TEST(*(next(i)) == 'x');
        BOOST_TEST(*(next(next(i))) == 3.3);
        BOOST_TEST(*(next(next(next(i)))) == s);

        next(next(next(next(i)))); // end

#ifdef FUSION_TEST_COMPILE_FAIL
        next(next(next(next(next(i))))); // past the end: must not compile
#endif

        BOOST_TEST(*(prior(next(next(next(i))))) == 3.3);
        BOOST_TEST(*(prior(prior(next(next(next(i)))))) == 'x');
        BOOST_TEST(*(prior(prior(prior(next(next(next(i))))))) == 1);

        BOOST_TEST(*begin(t) == 1);
        BOOST_TEST(*prior(end(t)) == s);

        *i = 3;
        BOOST_TEST(*i == 3);
        BOOST_TEST(*i == get<0>(t));
    }

    { // Testing const tuple and const tuple_iterator

        char const* s = "Hello";
        typedef tuple<int, char, double, char const*> const tuple_type;
        tuple_type t(1, 'x', 3.3, s);
        tuple_iterator<0, tuple_type> i(t);

        BOOST_TEST(*i == 1);
        BOOST_TEST(*(next(i)) == 'x');
        BOOST_TEST(*begin(t) == 1);
        BOOST_TEST(*prior(end(t)) == s);

#ifdef FUSION_TEST_COMPILE_FAIL
        *i = 3; // must not compile
#endif
    }

    { // Testing tuple equality

        typedef tuple<int, char, double, char const*> tuple_type;
        typedef tuple_iterator<0, tuple_type> ti1;
        typedef tuple_iterator<0, tuple_type const> ti2;
        BOOST_STATIC_ASSERT((equal_to<ti1 const, ti1>::value));
        BOOST_STATIC_ASSERT((equal_to<ti1, ti1 const>::value));
        BOOST_STATIC_ASSERT((equal_to<ti1, ti2>::value));
        BOOST_STATIC_ASSERT((equal_to<ti1 const, ti2>::value));
        BOOST_STATIC_ASSERT((equal_to<ti1, ti2 const>::value));
        BOOST_STATIC_ASSERT((equal_to<ti1 const, ti2 const>::value));
    }
//
//    { // Testing distance
//
//        typedef tuple<int, char, double, char const*> tuple_type;
//        tuple_type t(1, 'x', 3.3, "Hello");
//
//        BOOST_STATIC_ASSERT((result_of_distance<
//            tuple_iterator<0, tuple_type>
//          , tuple_iterator<4, tuple_type> >::type::value == 4));
//
//        BOOST_TEST(distance(begin(t), end(t)).value == 4);
//    }
//
//    { // Testing advance
//
//        typedef tuple<int, char, double, char const*> tuple_type;
//        tuple_type t(1, 'x', 3.3, "Hello");
//
//        BOOST_TEST(*advance<0>(begin(t)) == get<0>(t));
//        BOOST_TEST(*advance<1>(begin(t)) == get<1>(t));
//        BOOST_TEST(*advance<2>(begin(t)) == get<2>(t));
//        BOOST_TEST(*advance<3>(begin(t)) == get<3>(t));
//
//        BOOST_TEST(*advance<-1>(end(t)) == get<3>(t));
//        BOOST_TEST(*advance<-2>(end(t)) == get<2>(t));
//        BOOST_TEST(*advance<-3>(end(t)) == get<1>(t));
//        BOOST_TEST(*advance<-4>(end(t)) == get<0>(t));
//
//        BOOST_TEST(&*advance<0>(begin(t)) == &get<0>(t));
//        BOOST_TEST(&*advance<1>(begin(t)) == &get<1>(t));
//        BOOST_TEST(&*advance<2>(begin(t)) == &get<2>(t));
//        BOOST_TEST(&*advance<3>(begin(t)) == &get<3>(t));
//    }
//

#if !BOOST_WORKAROUND(__BORLANDC__, <= 0x551)
// #$%&@ Borland IS SO DUMB!!! #$%&@
    {
        typedef tuple<int, int> tuple_type;
        typedef result_of_begin<tuple_type>::type begin_type;
        typedef result_of_end<tuple_type>::type end_type;
        typedef result_of_next<begin_type>::type i1;
        typedef result_of_next<i1>::type i2;

        BOOST_STATIC_ASSERT((is_same<end_type, i2>::value));
    }
#endif

    { // Testing constructing tuples from iterators

        tuple<int, char, double> t1(1, 'x', 3.3);

        tuple<long, int, double> t2(begin(t1));
        BOOST_TEST(get<0>(t2) == get<0>(t1));
        BOOST_TEST(get<1>(t2) == get<1>(t1));
        BOOST_TEST(get<2>(t2) == get<2>(t1));

        tuple<char, double> t3(next(begin(t1)));
        BOOST_TEST(get<0>(t3) == get<1>(t1));
        BOOST_TEST(get<1>(t3) == get<2>(t1));

        tuple<double> t4(prior(end(t1)));
        BOOST_TEST(get<0>(t4) == get<2>(t1));

        tuple<char, char, char, char, char, char, char, char, char> t5;
        tuple<int, int, int, int, int, int, int, int, int> t6(begin(t5));
        (void)t6;
    }

    { // Testing tuple iterator value, reference, pointer, is_readable,
      // is_writable, result_of_deref, result_of_get_pointer

        typedef tuple<int, char&> tuple_type;
        typedef tuple_iterator<0, tuple_type> i0;
        typedef tuple_iterator<1, tuple_type> i1;
        typedef tuple_iterator<1, tuple_type const> i2;

        BOOST_STATIC_ASSERT((
            is_same<tuple_element<0, tuple_type>::type, int>::value));

        BOOST_STATIC_ASSERT((
            is_same<tuple_element<1, tuple_type>::type, char&>::value));

//        BOOST_STATIC_ASSERT((
//            is_same<category_of<i0>::type, random_access_traversal_tag>::value));
//
//        BOOST_STATIC_ASSERT(is_readable<i0>::value);
//        BOOST_STATIC_ASSERT(is_writable<i0>::value);
//        BOOST_STATIC_ASSERT(is_readable<i2>::value);
//        BOOST_STATIC_ASSERT(!is_writable<i2>::value);

        BOOST_STATIC_ASSERT((is_same<result_of_deref<i0>::type, int&>::value));
        BOOST_STATIC_ASSERT((is_same<result_of_deref<i1>::type, char&>::value));

//        BOOST_STATIC_ASSERT((is_same<result_of_get_pointer<i0>::type, int*>::value));
//        BOOST_STATIC_ASSERT((is_same<result_of_get_pointer<i1>::type, char*>::value));
//
//        BOOST_STATIC_ASSERT((is_same<value_of<i0>::type, int>::value));
//        BOOST_STATIC_ASSERT((is_same<value_of<i1>::type, char&>::value));
//
//        BOOST_STATIC_ASSERT((is_same<reference_of<i0>::type, int&>::value));
//        BOOST_STATIC_ASSERT((is_same<reference_of<i1>::type, char&>::value));
//
//        BOOST_STATIC_ASSERT((is_same<pointer_of<i0>::type, int*>::value));
//        BOOST_STATIC_ASSERT((is_same<pointer_of<i1>::type, char*>::value));
    }

    return 0;
}

