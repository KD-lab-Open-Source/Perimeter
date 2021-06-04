//  Boost checked_delete test program  ---------------------------------------//

//  Copyright Beman Dawes 2001.
//  See accompanying license for terms and conditions of use.

//  See http://www.boost.org/libs/utility for documentation.

//  Revision History
//  21 May 01  Initial version (Beman Dawes)

#include <boost/checked_delete.hpp>  // for checked_delete

//  This program demonstrates compiler errors when trying to delete an
//  incomplete type.

namespace
{
    class Incomplete;
}

int main()
{
    Incomplete * p = 0;
    boost::checked_delete(p);          // should cause compile time error
    boost::checked_array_delete(p);    // should cause compile time error
    return 0;
}   // main
