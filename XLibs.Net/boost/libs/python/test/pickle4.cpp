// Example by Ralf W. Grosse-Kunstleve

/*
    This example shows how to enable pickling without using the
    pickle_suite. The pickling interface (__getinitargs__) is
    implemented in Python.

    For more information refer to boost/libs/python/doc/pickle.html.
 */

#include <boost/python/module.hpp>
#include <boost/python/class.hpp>

#include <string>

namespace {

  // A friendly class.
  class world
  {
    private:
      std::string country;
    public:
      world(const std::string& country) {
        this->country = country;
      }
      std::string greet() const { return "Hello from " + country + "!"; }
      std::string get_country() const { return country; }
  };

}

BOOST_PYTHON_MODULE(pickle4_ext)
{
  using namespace boost::python;
  class_<world>("world", init<const std::string&>())
      .enable_pickling()
      .def("greet", &world::greet)
      .def("get_country", &world::get_country)
      ;
}
