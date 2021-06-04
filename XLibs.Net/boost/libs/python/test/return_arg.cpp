// Copyright David Abrahams and Nikolay Mladenov 2003. Permission to
// copy, use, modify, sell and distribute this software is granted
// provided this copyright notice appears in all copies. This software
// is provided "as is" without express or implied warranty, and with
// no claim as to its suitability for any purpose.

#include <boost/python/module.hpp>
#include <boost/python/class.hpp>
#include <boost/python/def.hpp>
#include <boost/python/return_arg.hpp>

struct Widget
{
    Widget()
      : sensitive_(true)
    {}
    
    bool get_sensitive() const
    {
        return sensitive_;
    }
    
    void set_sensitive(bool s)
    {
        this->sensitive_ = s;
    }
    
 private:
   bool sensitive_;
};

struct Label : Widget
{
    Label() {}

    std::string  get_label() const
    {
        return label_;
    }

    void set_label(const std::string &l)
    {
        label_ = l;
    }

 private:
   std::string label_;
};

void return_arg_f(boost::python::object) {}

using namespace boost::python;
BOOST_PYTHON_MODULE(return_arg_ext)
{
   class_<Widget>("Widget")
      .def("sensitive", &Widget::get_sensitive)
      .def("sensitive", &Widget::set_sensitive, return_self<>())
      ;

   class_<Label, bases<Widget> >("Label")
      .def("label", &Label::get_label)//,return_arg<0>()) //error(s)
      .def("label", &Label::set_label, return_self<>())
      ;

   def("return_arg", return_arg_f, return_arg<1>());
}

#include "module_tail.cpp"
