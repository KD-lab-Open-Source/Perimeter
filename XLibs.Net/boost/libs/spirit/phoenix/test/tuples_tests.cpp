/*=============================================================================
    Phoenix v1.2
    Copyright (c) 2001-2003 Joel de Guzman

    Use, modification and distribution is subject to the Boost Software
    License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#include <iostream>
#include <string>
#include <cassert>
#include <boost/spirit/phoenix/tuples.hpp>

using namespace std;
using namespace phoenix;
using namespace phoenix;
using namespace phoenix::tuple_index_names;

int
main()
{
    {
        typedef tuple<int, char> tuple_t;
        tuple_t ttt(3, 'c');

        tuple_element<0, tuple_t>::type& e0 = ttt[_1];
        tuple_element<1, tuple_t>::type& e1 = ttt[_2];

        assert(e0 == 3);
        assert(e1 == 'c');

        cout << e0 << endl;
        cout << e1 << endl;
    }

    {
        typedef tuple<int, char, char const*> tuple_t;
        tuple_t ttt(3, 'c', "hello world");
        cout << ttt.length << endl;

        tuple_element<0, tuple_t>::type& e0 = ttt[_1];
        tuple_element<1, tuple_t>::type& e1 = ttt[_2];
        tuple_element<2, tuple_t>::type& e2 = ttt[_3];

        assert(e0 == 3);
        assert(e1 == 'c');
        assert(string(e2) == "hello world");

        cout << e0 << endl;
        cout << e1 << endl;
        cout << e2 << endl;
    }

    return 0;
}

