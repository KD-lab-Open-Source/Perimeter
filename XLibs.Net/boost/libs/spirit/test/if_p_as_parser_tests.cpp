/*=============================================================================
    Copyright (c) 2003 Martin Wille
    http://spirit.sourceforge.net/

    Use, modification and distribution is subject to the Boost Software
    License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#include <boost/spirit/core.hpp>
#include <boost/spirit/dynamic/if.hpp>

extern bool fun();

struct ftor
{
    bool operator()() const;
};

int
main()
{
    //////////////////////////////////
    // compile time check wether as_parser<> works for if_p

    ::boost::spirit::rule<> r;

    r = ::boost::spirit::if_p('-')['-'];
    r = ::boost::spirit::if_p("-")["-"];
    r = ::boost::spirit::if_p('-')['-'].else_p['-'];
    r = ::boost::spirit::if_p("-")["-"].else_p["-"];
    
    r = ::boost::spirit::if_p(&fun)["foo"];
    r = ::boost::spirit::if_p(ftor())["foo"];
    r = ::boost::spirit::if_p(&fun)["foo"].else_p["bar"];
    r = ::boost::spirit::if_p(ftor())["foo"].else_p["bar"];

    r = ::boost::spirit::if_p(r)[r];
    r = ::boost::spirit::if_p(r)[r].else_p[r];
}
