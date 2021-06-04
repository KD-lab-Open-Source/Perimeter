//  (C) Copyright Gennadiy Rozental 2003.
//  Use, modification, and distribution are subject to the 
//  Boost Software License, Version 1.0. (See accompanying file 
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org/libs/test for the library home page.
//
//  File        : $RCSfile: prg_exec_fail4.cpp,v $
//
//  Version     : $Revision: 1.4 $
//
//  Description : test the Execution Monitor logic regards cpp_main return values
// ***************************************************************************
#include <stdexcept>

int cpp_main( int, char* [] )  // note the name
{
    return -1;
}

//____________________________________________________________________________//

// ***************************************************************************
//  Revision History :
//  
//  $Log: prg_exec_fail4.cpp,v $
//  Revision 1.4  2003/12/01 00:42:38  rogeeff
//  prerelease cleaning
//

// ***************************************************************************

// EOF
