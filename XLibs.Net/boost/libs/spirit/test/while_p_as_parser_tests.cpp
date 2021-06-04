/*=============================================================================
    Copyright (c) 2003 Martin Wille
    http://spirit.sourceforge.net/

    Use, modification and distribution is subject to the Boost Software
    License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#include <boost/spirit/core.hpp>
#include <boost/spirit/dynamic/while.hpp>

extern bool fun();

struct ftor
{
    bool operator()() const;
};

int
main()
{
    //////////////////////////////////
    // compile time check wether as_parser<> works for while_p

    ::boost::spirit::rule<> r;

    r = ::boost::spirit::while_p('-')['-'];
    r = ::boost::spirit::while_p("-")["-"];

    r = ::boost::spirit::while_p(&fun)["foo"];
    r = ::boost::spirit::while_p(ftor())["foo"];

    r = ::boost::spirit::while_p(r)[r];

    r = ::boost::spirit::do_p['-'].while_p('-');
    r = ::boost::spirit::do_p["-"].while_p("-");

    r = ::boost::spirit::do_p["foo"].while_p(&fun);
    r = ::boost::spirit::do_p["foo"].while_p(ftor());

    r = ::boost::spirit::do_p[r].while_p(r);
}
