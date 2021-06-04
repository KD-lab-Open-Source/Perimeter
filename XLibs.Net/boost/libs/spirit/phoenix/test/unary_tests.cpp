/*=============================================================================
    Phoenix v1.2
    Copyright (c) 2001-2003 Joel de Guzman

    Use, modification and distribution is subject to the Boost Software
    License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#include <iostream>
#include <cassert>

#define PHOENIX_LIMIT 15
#include <boost/spirit/phoenix/primitives.hpp>
#include <boost/spirit/phoenix/operators.hpp>

using namespace phoenix;
using namespace std;

///////////////////////////////////////////////////////////////////////////////
int
main()
{
    int i1 = 1, i = 5;

///////////////////////////////////////////////////////////////////////////////
//
//  Unary operators
//
///////////////////////////////////////////////////////////////////////////////
    assert((!val(true))() == false);
    assert((-val(1))() == -1);
    assert((+val(1))() == +1);
    assert((~val(1))() == ~1);
    assert(*(&arg1)(i1) == *(&i1));
    assert((&arg1)(i1) == &i1);

    assert((*val(&i1))() == *(&i1));
    assert((*&arg1)(i1) == *(&i1));
    assert((++var(i))() == 6);
    assert((--var(i))() == 5);
    assert((var(i)++)() == 5);
    assert(i == 6);
    assert((var(i)--)() == 6);
    assert(i == 5);

    cout << "///////////////////////////////////////////////////////////////////////////////\n";
    cout << "\t\tTests concluded\n";
    cout << "\t\tSUCCESS!!!\n";
    cout << "///////////////////////////////////////////////////////////////////////////////\n";
}
