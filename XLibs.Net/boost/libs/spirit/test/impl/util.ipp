/*=============================================================================
    Copyright (c) 2003 Martin Wille
    http://spirit.sourceforge.net/

    Use, modification and distribution is subject to the Boost Software
    License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#ifndef BOOST_SPIRIT_TEST_IMPL_UTIL_IPP
#define BOOST_SPIRIT_TEST_IMPL_UTIL_IPP

#include <cstdlib>
#include <cstring>

namespace test
{

bool    verbose_runtests; // set to true when started by
                          // libs/spirit/tests/runtests.sh

void init(int /*argc*/, char* /*argv*/[])
{
    using namespace std;
    verbose_runtests = !!getenv("VERBOSE_RUNTESTS");
}

void banner(char const *text)
{
    if (!verbose_runtests)
        return;

    using namespace std;

    int len = strlen(text);
    std::cout << "/////////////////////////////////////////////////////////\n";
    len = (57-len)/2;
    while(len-->0) std::cout << ' ';
    std::cout << text << "\n";
    std::cout << "/////////////////////////////////////////////////////////\n";
}

}
#endif // BOOST_SPIRIT_TEST_IMPL_UTIL_IPP
