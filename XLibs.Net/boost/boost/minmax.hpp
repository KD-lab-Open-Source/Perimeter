//  (C) Copyright Eric Niebler 2004.
//  Use, modification and distribution are subject to the 
//  Boost Software License, Version 1.0. (See accompanying file 
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

/*
 Revision history:
   25 February 2004
      Initial version.
*/

#ifndef BOOST_MINMAX_HPP
#define BOOST_MINMAX_HPP

#include <algorithm> // for std::min and std::max
#include <boost/config.hpp>

namespace boost
{

template< typename T >
inline T const & std_min( T const & a, T const & b )
{
    using std::min;

    return min BOOST_PREVENT_MACRO_SUBSTITUTION ( a, b );
}

template< typename T >
inline T const & std_max( T const & a, T const & b )
{
    using std::max;

    return max BOOST_PREVENT_MACRO_SUBSTITUTION ( a, b );
}

// Overloads for unsigned long to work around a bug in the Borland headers
#ifdef __BORLANDC__
inline unsigned long const & std_min( unsigned long const & a, unsigned long const & b )
{
    return a < b ? a : b;
}

inline unsigned long const & std_max( unsigned long const & a, unsigned long const & b )
{
    return a > b ? a : b;
}
#endif

}

#endif // BOOST_MINMAX_HPP
