// Copyright David Abrahams 2002. Permission to copy, use,
// modify, sell and distribute this software is granted provided this
// copyright notice appears in all copies. This software is provided
// "as is" without express or implied warranty, and with no claim as
// to its suitability for any purpose.

// embedded_hello -- A simple Boost.Python embedding example -- by
// Dirk Gerrits

#include <boost/python.hpp>
#include <boost/scoped_ptr.hpp>
#include <iostream>
#include <stdexcept>
#include <cassert>

namespace python = boost::python;

// An abstract base class
class Base : public boost::noncopyable
{
public:
    virtual ~Base() {};

    virtual std::string hello() = 0;
};

// C++ derived class
class CppDerived : public Base
{
public:
    virtual ~CppDerived() {}

    std::string hello()
    {
        return "Hello from C++!";
    }
};

// Familiar Boost.Python wrapper class for Base
struct BaseWrap : public Base
{
    BaseWrap(PyObject* self_)
        : self(self_) {}

    std::string hello() { return python::call_method<std::string>(self, "hello"); }

    PyObject* self;
};

// Pack the Base class wrapper into a module
BOOST_PYTHON_MODULE(embedded_hello)
{
    python::class_<Base, BaseWrap, boost::noncopyable>("Base")
        ;

}


void test()
{
//- INITIALIZATION -----------------------------------------------------------//

    // Register the module with the interpreter
    if (PyImport_AppendInittab("embedded_hello", initembedded_hello) == -1)
        throw std::runtime_error("Failed to add embedded_hello to the interpreter's "
                                 "builtin modules");

    // Initialize the interpreter
    Py_Initialize();

    // Retrieve the main module
    python::object main_module = python::extract<python::object>(
        PyImport_AddModule("__main__")
    )();

    // Retrieve the main module's namespace
    python::object main_namespace(main_module.attr("__dict__"));

    // Define the derived class in Python.
    // (You'll normally want to put this in a .py file.)
    python::handle<> result(
        PyRun_String(
        "from embedded_hello import *        \n"
        "class PythonDerived(Base):          \n"
        "    def hello(self):                \n"
        "        return 'Hello from Python!' \n",
        Py_file_input, main_namespace.ptr(), main_namespace.ptr())
        );
    // Result is not needed
    result.reset();

    // Extract the raw Python object representing the just defined derived class
    python::handle<> class_ptr(
        PyRun_String("PythonDerived\n", Py_eval_input,
                     main_namespace.ptr(), main_namespace.ptr()) );

    // Wrap the raw Python object in a Boost.Python object
    python::object PythonDerived(class_ptr);

//- MAIN PROGRAM -------------------------------------------------------------//

    // Creating and using instances of the C++ class is as easy as always.
    CppDerived cpp;
    std::cout << cpp.hello() << std::endl;

    // But now creating and using instances of the Python class is almost
    // as easy!
    python::object py_base = PythonDerived();
    Base& py = python::extract<Base&>(py_base)();
    std::cout << py.hello() << std::endl;
}

void
test_tutorial()
{
    using namespace boost::python;

    object main_module(
        handle<>(borrowed(PyImport_AddModule("__main__"))));

    object main_namespace = main_module.attr("__dict__");

    handle<>(PyRun_String(

        "hello = file('hello.txt', 'w')\n"
        "hello.write('Hello world!')\n"
        "hello.close()"

      , Py_file_input
      , main_namespace.ptr()
      , main_namespace.ptr())
    );
}

void
test_tutorial2()
{
    using namespace boost::python;

    object main_module(
        handle<>(borrowed(PyImport_AddModule("__main__"))));

    object main_namespace = main_module.attr("__dict__");

    handle<>(PyRun_String(

        "result = 5 ** 2"

        , Py_file_input
        , main_namespace.ptr()
        , main_namespace.ptr())
    );

    int five_squared = extract<int>(main_namespace["result"]);
    assert(five_squared == 25);

    object result(handle<>(
        PyRun_String("5 ** 2"
            , Py_eval_input
            , main_namespace.ptr()
            , main_namespace.ptr()))
    );

    int five_squared2 = extract<int>(result);
    assert(five_squared2 == 25);
}

int main()
{
    if (python::handle_exception(test))
    {
        if (PyErr_Occurred())
            PyErr_Print();
        return 1;
    }

    if (python::handle_exception(test_tutorial))
    {
        if (PyErr_Occurred())
            PyErr_Print();
        return 1;
    }

    if (python::handle_exception(test_tutorial2))
    {
        if (PyErr_Occurred())
            PyErr_Print();
        return 1;
    }

    return 0;
}
#include "module_tail.cpp"
