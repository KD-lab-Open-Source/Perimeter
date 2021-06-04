// Copyright David Abrahams 2003. Permission to copy, use,
// modify, sell and distribute this software is granted provided this
// copyright notice appears in all copies. This software is provided
// "as is" without express or implied warranty, and with no claim as
// to its suitability for any purpose.

#include <boost/implicit_cast.hpp>
#include <boost/type.hpp>

#define BOOST_INCLUDE_MAIN
#include <boost/test/test_tools.hpp>

using boost::implicit_cast;

struct foo
{
    explicit foo(char const*) {}
};

int test_main(int, char*[])
{
    foo x = implicit_cast<foo>("foobar");
}
