// Boost.Signals library

// Copyright Doug Gregor 2001-2004. Use, modification and
// distribution is subject to the Boost Software License, Version
// 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

// For more information, see http://www.boost.org

#define BOOST_SIGNALS_SOURCE

#include <boost/signals/connection.hpp>
#include <cassert>

namespace boost {
  namespace BOOST_SIGNALS_NAMESPACE {

    void
    connection::add_bound_object(const BOOST_SIGNALS_NAMESPACE::detail::bound_object& b)
    {
      assert(con.get() != 0);
      con->bound_objects.push_back(b);
    }


    void connection::disconnect() const
    {
      if (this->connected()) {
        // Make sure we have a reference to the basic_connection object,
        // because 'this' may disappear
        shared_ptr<detail::basic_connection> local_con = con;

        void (*signal_disconnect)(void*, void*) = local_con->signal_disconnect;

        // Note that this connection no longer exists
        // Order is important here: we could get into an infinite loop if this
        // isn't cleared before we try the disconnect.
        local_con->signal_disconnect = 0;

        // Disconnect signal
        signal_disconnect(local_con->signal, local_con->signal_data);

        // Disconnect all bound objects
        typedef std::list<BOOST_SIGNALS_NAMESPACE::detail::bound_object>::iterator iterator;
        for (iterator i = local_con->bound_objects.begin();
             i != local_con->bound_objects.end(); ++i) {
          assert(i->disconnect != 0);
          i->disconnect(i->obj, i->data);
        }
      }
    }
  } // end namespace boost
} // end namespace boost

#ifndef BOOST_MSVC
// Explicit instantiations to keep everything in the library
template class std::list<boost::BOOST_SIGNALS_NAMESPACE::detail::bound_object>;
#endif
