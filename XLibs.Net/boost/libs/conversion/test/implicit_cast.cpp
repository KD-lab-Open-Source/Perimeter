// Copyright David Abrahams 2003. Permission to copy, use,
// modify, sell and distribute this software is granted provided this
// copyright notice appears in all copies. This software is provided
// "as is" without express or implied warranty, and with no claim as
// to its suitability for any purpose.

#include <boost/implicit_cast.hpp>
#include <cassert>
#include <boost/type.hpp>

using boost::implicit_cast;
using boost::type;

template <class T>
type<T> check_return(T) { return type<T>(); }

struct foo
{
    foo(char const*) {}
    operator long() const { return 0; }
};

typedef type<long> long_type;
typedef type<foo> foo_type;

int main()
{
    type<long> x = check_return(boost::implicit_cast<long>(1));
    assert(boost::implicit_cast<long>(1) == 1L);

    type<foo> f = check_return(boost::implicit_cast<foo>("hello"));
    type<long> z = check_return(boost::implicit_cast<long>(foo("hello")));
    return 0;
}
