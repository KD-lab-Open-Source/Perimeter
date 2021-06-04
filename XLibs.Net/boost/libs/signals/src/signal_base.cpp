// Boost.Signals library

// Copyright Doug Gregor 2001-2004. Use, modification and
// distribution is subject to the Boost Software License, Version
// 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

// For more information, see http://www.boost.org

#define BOOST_SIGNALS_SOURCE

#include <boost/signals/detail/signal_base.hpp>
#include <cassert>

namespace boost {
  namespace BOOST_SIGNALS_NAMESPACE {
    namespace detail {
      signal_base_impl::signal_base_impl(const compare_type& comp,
                                         const any& combiner)
        : call_depth(0),
          slots_(comp),
          combiner_(combiner)
      {
        flags.delayed_disconnect = false;
        flags.clearing = false;
      }

      signal_base_impl::~signal_base_impl()
      {
        // Set the "clearing" flag to ignore extraneous disconnect requests,
        // because all slots will be disconnected on destruction anyway.
        flags.clearing = true;
      }

      void signal_base_impl::disconnect_all_slots()
      {
        // Do nothing if we're already clearing the slot list
        if (flags.clearing)
          return;

        if (call_depth == 0) {
          // Clearing the slot list will disconnect all slots automatically
          temporarily_set_clearing set_clearing(this);
          slots_.clear();
        }
        else {
          // We can't actually remove elements from the slot list because there
          // are still iterators into the slot list that must not be
          // invalidated by this operation. So just disconnect each slot
          // without removing it from the slot list. When the call depth does
          // reach zero, the call list will be cleared.
          flags.delayed_disconnect = true;
          temporarily_set_clearing set_clearing(this);
          for (iterator i = slots_.begin(); i != slots_.end(); ++i) {
            i->first.disconnect();
          }
        }
      }

      connection
      signal_base_impl::
        connect_slot(const any& slot,
                     const any& name,
                     const std::vector<const trackable*>& bound_objects,
                     connect_position at)
      {
        // Allocate storage for a new basic_connection object to represent the
        // connection
        basic_connection* con = new basic_connection();

        // Create a new connection handle object and place the basic_connection
        // object we just created under its control. Note that the "reset"
        // routine will delete con if allocation throws.
        connection slot_connection;
        slot_connection.reset(con);

        // Allocate storage for an iterator that will hold the point of
        // insertion of the slot into the list. This is used to later remove
        // the slot when it is disconnected.
        std::auto_ptr<iterator> saved_iter(new iterator());

        // Add the slot to the list.
        iterator pos = slots_.insert(name, slot_connection, slot, at);

        // Make the copy of the connection in the list disconnect when it is
        // destroyed
        pos->first.set_controlling();

        // The assignment operation here absolutely must not throw, which
        // intuitively makes sense (because any container's insert method
        // becomes impossible to use in an exception-safe manner without this
        // assumption), but doesn't appear to be mentioned in the standard.
        *saved_iter = pos;

        // Fill out the connection object appropriately. None of these
        // operations can throw
        con->signal = this;
        con->signal_data = saved_iter.release();
        con->signal_disconnect = &signal_base_impl::slot_disconnected;

        // If an exception is thrown the connection will automatically be
        // disconnected.
        scoped_connection safe_connection = slot_connection;

        // Connect each of the bound objects
        for(std::vector<const trackable*>::const_iterator i =
              bound_objects.begin();
            i != bound_objects.end();
            ++i) {
          // Notify the object that the signal is connecting to it by passing
          // it a copy of the connection. If the connection
          // should throw, the scoped connection safe_connection will
          // disconnect the connection completely.
          bound_object binding;
          (*i)->signal_connected(slot_connection, binding);

          // This will notify the bound object that the connection just made
          // should be disconnected if an exception is thrown before the
          // end of this iteration
          auto_disconnect_bound_object disconnector(binding);

          // Add the binding to the list of bindings for the connection.
          con->bound_objects.push_back(binding);

          // The connection object now knows about the bound object, so if an
          // exception is thrown later the connection object will notify the
          // bound object of the disconnection automatically
          disconnector.release();
        }

        // No exceptions will be thrown past this point, and we must not
        // disconnect the connection now
        safe_connection.release();

        return slot_connection;
      }

      bool signal_base_impl::empty() const
      {
        // Disconnected slots may still be in the list of slots if
        //   a) this is called while slots are being invoked (call_depth > 0)
        //   b) an exception was thrown in remove_disconnected_slots
        for (iterator i = slots_.begin(); i != slots_.end(); ++i) {
          if (i->first.connected())
            return false;
        }

        return true;
      }

      std::size_t signal_base_impl::num_slots() const
      {
        // Disconnected slots may still be in the list of slots if
        //   a) this is called while slots are being invoked (call_depth > 0)
        //   b) an exception was thrown in remove_disconnected_slots
        std::size_t count = 0;
        for (iterator i = slots_.begin(); i != slots_.end(); ++i) {
          if (i->first.connected())
            ++count;
        }
        return count;
      }

      void signal_base_impl::disconnect(const any& group)
      { slots_.disconnect(group); }

      void signal_base_impl::slot_disconnected(void* obj, void* data)
      {
        signal_base_impl* self = reinterpret_cast<signal_base_impl*>(obj);

        // We won't need the slot iterator after this
        std::auto_ptr<iterator> slot(reinterpret_cast<iterator*>(data));

        // If we're flags.clearing, we don't bother updating the list of slots
        if (!self->flags.clearing) {
          // If we're in a call, note the fact that a slot has been deleted so
          // we can come back later to remove the iterator
          if (self->call_depth > 0) {
            self->flags.delayed_disconnect = true;
          }
          else {
            // Just remove the slot now, it's safe
            self->slots_.erase(*slot);
          }
        }
      }

      void signal_base_impl::remove_disconnected_slots() const
      { slots_.remove_disconnected_slots(); }

      call_notification::
        call_notification(const shared_ptr<signal_base_impl>& b) :
          impl(b)
      {
        // A call will be made, so increment the call depth as a notification
        impl->call_depth++;
      }

      call_notification::~call_notification()
      {
        impl->call_depth--;

        // If the call depth is zero and we have some slots that have been
        // disconnected during the calls, remove those slots from the list
        if (impl->call_depth == 0 &&
            impl->flags.delayed_disconnect) {
          impl->remove_disconnected_slots();
          impl->flags.delayed_disconnect = false;
        }
      }

    signal_base::signal_base(const compare_type& comp, const any& combiner)
      : impl()
    {
      impl.reset(new signal_base_impl(comp, combiner));
    }

    signal_base::~signal_base()
    {
    }

    } // namespace detail
  } // namespace BOOST_SIGNALS_NAMESPACE
} // namespace boost

