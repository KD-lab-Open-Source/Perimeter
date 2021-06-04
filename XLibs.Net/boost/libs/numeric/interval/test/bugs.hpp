/* Boost test/bugs.hpp
 * Handles namespace resolution quirks in older compilers and braindead
 * warnings [Herve, June 3rd 2003]
 *
 * Copyright Herve Bronnimann2002-2003
 * Permission to use, copy, modify, sell, and distribute this software
 * is hereby granted without fee provided that the above copyright
 * notice appears in all copies and that both that copyright notice and
 * this permission notice appear in supporting documentation.
 *
 * None of the above authors nor Polytechnic University make any
 * representation about the suitability of this software for any
 * purpose. It is provided "as is" without express or implied warranty.
 *
 * $Id: bugs.hpp,v 1.4 2003/06/04 16:54:59 gmelquio Exp $
 */

#include <boost/config.hpp>

// Borland compiler complains about unused variables a bit easily and
// incorrectly

#ifdef __BORLANDC__
namespace detail {

  template <class T> inline void ignore_unused_variable_warning(const T&) { }

  inline void ignore_warnings() {
#   ifdef BOOST_NUMERIC_INTERVAL_CONSTANTS_HPP
    using namespace boost::numeric::interval_lib::constants;
    ignore_unused_variable_warning( pi_f_l );
    ignore_unused_variable_warning( pi_f_u );
    ignore_unused_variable_warning( pi_d_l );
    ignore_unused_variable_warning( pi_d_u );
#   endif
  }

}
#endif

// Some compilers are broken with respect to name resolution

#if defined(BOOST_NO_ARGUMENT_DEPENDENT_LOOKUP) || defined( __BORLANDC__)

using namespace boost;
using namespace numeric;
using namespace interval_lib;

#endif
