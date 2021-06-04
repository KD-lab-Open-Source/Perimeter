//  Copyright (c) 2003 boost.org
//
//  Permission to copy, use, modify, sell and distribute this software
//  is granted provided this copyright notice appears in all copies.
//  This software is provided "as is" without express or implied
//  warranty, and with no claim as to its suitability for any purpose.
//
//  http://www.boost.org
// 

// This file will declare 'foo' is 'MACROS' is NOT defined.

#ifndef MACROS
void
# ifdef _WIN32
__declspec(dllexport)
# endif 
foo() {}
#endif

# ifdef _WIN32
int __declspec(dllexport) force_implib_creation;
# endif 
