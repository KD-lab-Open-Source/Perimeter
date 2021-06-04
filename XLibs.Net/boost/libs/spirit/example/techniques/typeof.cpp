/*=============================================================================
    Copyright (c) 2002-2003 Joel de Guzman
    http://spirit.sourceforge.net/

    Use, modification and distribution is subject to the Boost Software
    License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/

// *** See the section "typeof" in chapter "Techniques" of
// *** the Spirit documentation for information regarding
// *** this snippet.

#ifdef __MWERKS__
#define typeof __typeof__
#endif

#if !defined(__MWERKS__) && !defined(__GNUC__)
#error "typeof not supported by your compiler"
#endif

#include <iostream>
#include <boost/spirit/core.hpp>

using namespace boost::spirit;

#define RULE(name, definition) typeof(definition) name = definition

int
main()
{
    RULE(
        skipper,
        (       space_p
            |   "//" >> *(anychar_p - '\n') >> '\n'
            |   "/*" >> *(anychar_p - "*/") >> "*/"
        )
    );

    bool success = parse(
        "/*this is a comment*/\n//this is a c++ comment\n\n",
        *skipper).full;
    assert(success);
    std::cout << "SUCCESS!!!\n";
    return 0;
}

