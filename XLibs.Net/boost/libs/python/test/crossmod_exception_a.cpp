// Copyright (C) 2003 Rational Discovery LLC
// Permission to copy, use, modify, sell and distribute this software
// is granted provided this copyright notice appears in all
// copies. This software is provided "as is" without express or
// implied warranty, and with no claim as to its suitability for any
// purpose.

#include <boost/python.hpp>

namespace python = boost::python;

void tossit(){
  PyErr_SetString(PyExc_IndexError,"a-blah!");
  throw python::error_already_set();
}

BOOST_PYTHON_MODULE(crossmod_exception_a)
{
    python::def("tossit",tossit);
}
