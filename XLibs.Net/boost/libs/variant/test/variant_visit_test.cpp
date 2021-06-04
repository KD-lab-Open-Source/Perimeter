//-----------------------------------------------------------------------------
// boost-libs variant/test/variant_visit_test.cpp source file
// See http://www.boost.org for updates, documentation, and revision history.
//-----------------------------------------------------------------------------
//
// Copyright (c) 2003
// Eric Friedman
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee, 
// provided that the above copyright notice appears in all copies and 
// that both the copyright notice and this permission notice appear in 
// supporting documentation. No representations are made about the 
// suitability of this software for any purpose. It is provided "as is" 
// without express or implied warranty.

#include "boost/variant/variant.hpp"
#include "boost/variant/apply_visitor.hpp"
#include "boost/variant/static_visitor.hpp"
#include "boost/test/minimal.hpp"

#include "boost/mpl/bool.hpp"
#include "boost/mpl/and.hpp"
#include "boost/type_traits/is_same.hpp"

struct udt1
{
};

struct udt2
{
};

template <typename T>
class unary_check_content_type
    : public boost::static_visitor<bool>
{
public:

    // not recommended design, but simplifies workarounds:

    template <typename U>
    bool operator()(U&) const
    {
        return ::boost::is_same<T,U>::value;
    }

};

template <typename T1, typename T2>
class binary_check_content_type
    : public boost::static_visitor<bool>
{
public:

    // not recommended design, but simplifies workarounds:

    template <typename U1, typename U2>
    bool operator()(U1&, U2&) const
    {
        return ::boost::mpl::and_<
              boost::is_same<T1,U1>
            , boost::is_same<T2,U2>
            >::value;
    }

};

template <typename Checker, typename Variant>
inline void unary_test(Variant& var, Checker* = 0)
{
    Checker checker;
    const Checker& const_checker = checker;

    // standard tests

    BOOST_CHECK( boost::apply_visitor(checker, var) );
    BOOST_CHECK( boost::apply_visitor(const_checker, var) );
    BOOST_CHECK( boost::apply_visitor(Checker(), var) );

    // delayed tests

    BOOST_CHECK( boost::apply_visitor(checker)(var) );
    BOOST_CHECK( boost::apply_visitor(const_checker)(var) );
}

template <typename Checker, typename Variant1, typename Variant2>
inline void binary_test(Variant1& var1, Variant2& var2, Checker* = 0)
{
    Checker checker;
    const Checker& const_checker = checker;

    // standard tests

    BOOST_CHECK( boost::apply_visitor(checker, var1, var2) );
    BOOST_CHECK( boost::apply_visitor(const_checker, var1, var2) );
    BOOST_CHECK( boost::apply_visitor(Checker(), var1, var2) );

    // delayed tests

    BOOST_CHECK( boost::apply_visitor(checker)(var1, var2) );
    BOOST_CHECK( boost::apply_visitor(const_checker)(var1, var2) );
}

int test_main(int , char* [])
{
    typedef boost::variant<udt1,udt2> var_t;
    udt1 u1;
    var_t var1(u1);
    udt2 u2;
    var_t var2(u2);

    const var_t& cvar1 = var1;
    const var_t& cvar2 = var2;

    //
    // unary tests
    //

    typedef unary_check_content_type<udt1> check1_t;
    typedef unary_check_content_type<const udt1> check1_const_t;
    typedef unary_check_content_type<udt2> check2_t;
    typedef unary_check_content_type<const udt2> check2_const_t;

    unary_test< check1_t       >(var1);
    unary_test< check1_const_t >(cvar1);

    unary_test< check2_t       >(var2);
    unary_test< check2_const_t >(cvar2);

    //
    // binary tests
    //

    typedef binary_check_content_type<udt1,udt2> check12_t;
    typedef binary_check_content_type<const udt1, const udt2> check12_const_t;
    typedef binary_check_content_type<udt2,udt1> check21_t;
    typedef binary_check_content_type<const udt2, const udt1> check21_const_t;

    binary_test< check12_t       >(var1,var2);
    binary_test< check12_const_t >(cvar1,cvar2);

    binary_test< check21_t       >(var2,var1);
    binary_test< check21_const_t >(cvar2,cvar1);

    return boost::exit_success;
}
