/*=============================================================================
    Copyright (C) 2003 Martin Wille
    http://spirit.sourceforge.net/

    Use, modification and distribution is subject to the Boost Software
    License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/

// Nota bene: the actual locking is _not_ tested here!

#include <iostream>
#include <boost/config.hpp>

void banner()
{
    std::cout << "/////////////////////////////////////////////////////////\n";
    std::cout << "\n";
    std::cout << "          scoped_lock test\n";
    std::cout << "\n";
    std::cout << "/////////////////////////////////////////////////////////\n";
    std::cout << "\n";
}

#if defined(DONT_HAVE_BOOST) || !defined(BOOST_HAS_THREADS)
// if boost libraries are not available we have to skip the tests
int
main()
{
    banner();
    std::cout << "Test skipped (Boost libaries not available)\n";
    return 0;
}
#else

#include <boost/thread/mutex.hpp>
#include <boost/spirit/core.hpp>
#include <boost/spirit/utility/scoped_lock.hpp>
#include <cassert>

int
main()
{
    banner();

    using boost::spirit::rule;
    using boost::spirit::scoped_lock_d;
    using boost::spirit::parse_info;
    using boost::spirit::parse;
    using boost::mutex;

    mutex m;
    rule<> r = scoped_lock_d(m)['x'];
    parse_info<> pi = parse("x", r);
    assert(pi.hit);
    assert(pi.full);

    std::cout  << "Test concluded successfully\n";

    return 0;
}

#endif // defined(DONT_HAVE_BOOST)
