/*=============================================================================
    Copyright (c) 2003 Joel de Guzman

    Use, modification and distribution is subject to the Boost Software
    License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#include <boost/test/minimal.hpp>
#include <boost/spirit/fusion/sequence/tuple.hpp>
#include <boost/spirit/fusion/sequence/io.hpp>
#include <boost/spirit/fusion/sequence/generate.hpp>
#include <boost/spirit/fusion/sequence/equal_to.hpp>
#include <boost/spirit/fusion/sequence/make_tuple.hpp>
#include <boost/spirit/fusion/sequence/joint_view.hpp>
#include <boost/spirit/fusion/sequence/get.hpp>
#include <boost/mpl/vector_c.hpp>

struct X
{
    operator char const*() const
    {
        return "<X-object>";
    }
};

int
test_main(int, char*[])
{
    using namespace boost::fusion;

    std::cout << tuple_open('[');
    std::cout << tuple_close(']');
    std::cout << tuple_delimiter(", ");

/// Testing joint_view

    {
        tuple<int> t1(3);
        tuple<X> t2;
        typedef joint_view<tuple<int>, tuple<X> > view_type;
        view_type view(t1, t2);

        std::cout << view << std::endl;
        BOOST_TEST((view == make_tuple(3, X())));
    }

    {
        tuple<int, char> t1(3, 'x');
        tuple<X> t2;
        typedef joint_view<tuple<int, char>, tuple<X> > view_type;
        view_type view(t1, t2);
        std::cout << view << std::endl;
        BOOST_TEST((view == make_tuple(3, 'x', X())));

        *begin(view) = 4;
        BOOST_TEST(get<0>(t1) == 4);
    }

    {
        tuple<int, char> t1(3, 'x');
        tuple<X, int> t2;
        typedef joint_view<tuple<int, char>, tuple<X, int> > view_type;
        view_type view(t1, t2);
        std::cout << view << std::endl;
        BOOST_TEST((view == make_tuple(3, 'x', X(), 0)));
    }

    {
        typedef tuple<int> t1_type;
        t1_type t1(777);
        typedef tuple<int, char, double> t2_type;
        t2_type t2(1, 'x', 3.3);

        {
            typedef joint_view<t1_type, t2_type> view_type;
            view_type view(t1, t2);
            std::cout << view << std::endl;
            BOOST_TEST((view == make_tuple(777, 1, 'x', 3.3)));
        }

        {
            typedef joint_view<t2_type, t1_type> view_type;
            view_type view(t2, t1);
            std::cout << view << std::endl;
            BOOST_TEST((view == make_tuple(1, 'x', 3.3, 777)));
        }

        {
            typedef joint_view<t2_type, t1_type> jv_type;
            typedef joint_view<jv_type, jv_type> jv2_type;

            jv_type jv(t2, t1);
            jv2_type jv2(jv, jv);

            std::cout << jv << std::endl;
            std::cout << jv2 << std::endl;

            BOOST_TEST(jv2
                == make_tuple(1, 'x', 3.3, 777, 1, 'x', 3.3, 777));
        }

        {
            typedef joint_view<t2_type, t1_type> jt_type;
            typedef joint_view<t1_type, t2_type> jv2_type;
            typedef joint_view<jt_type, jv2_type> jv3_type;

            jt_type jt(t2, t1);
            jv2_type jv2(t1, t2);
            jv3_type jv3(jt, jv2);

            std::cout << jt << std::endl;
            std::cout << jv2 << std::endl;
            std::cout << jv3 << std::endl;

            BOOST_TEST(jv3
                == make_tuple(1, 'x', 3.3, 777, 777, 1, 'x', 3.3));
        }

        {
            typedef joint_view<tuple<>, t1_type> jt_type;
            tuple<> empty;
            jt_type jt(empty, t1);
            std::cout << generate(jt) << std::endl;
            BOOST_TEST(generate(jt) == make_tuple(777));
        }

        {
            typedef joint_view<t1_type, tuple<> > jt_type;
            tuple<> empty;
            jt_type jt(t1, empty);
            std::cout << generate(jt) << std::endl;
            BOOST_TEST(generate(jt) == make_tuple(777));
        }

        {
            typedef joint_view<tuple<>, tuple<> > jt_type;
            tuple<> empty;
            jt_type jt(empty, empty);
            std::cout << generate(jt) << std::endl;
            BOOST_TEST(generate(jt) == make_tuple());
        }
    }

    return 0;
}

