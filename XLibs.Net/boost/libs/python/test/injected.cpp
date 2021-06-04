// Copyright David Abrahams 2003. Permission to copy, use,
// modify, sell and distribute this software is granted provided this
// copyright notice appears in all copies. This software is provided
// "as is" without express or implied warranty, and with no claim as
// to its suitability for any purpose.

#include <boost/python/module.hpp>
#include <boost/python/class.hpp>
#include "test_class.hpp"
#include <memory>
#include <boost/shared_ptr.hpp>
#include <boost/python/make_constructor.hpp>

using namespace boost::python;

typedef test_class<> X;

X* empty() { return new X(1000); }

std::auto_ptr<X> sum(int a, int b) { return std::auto_ptr<X>(new X(a+b)); }

boost::shared_ptr<X> product(int a, int b, int c)
{
    return boost::shared_ptr<X>(new X(a*b*c));
}

BOOST_PYTHON_MODULE(injected_ext)
{
    class_<X>("X", init<int>())
        .def("__init__", make_constructor(empty))
        .def("__init__", make_constructor(sum))
        .def("__init__", make_constructor(product))
        .def("value", &X::value)
        ;
}
