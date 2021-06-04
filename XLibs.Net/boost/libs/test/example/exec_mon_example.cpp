//  (C) Copyright Gennadiy Rozental 2003.
//  Use, modification, and distribution are subject to the 
//  Boost Software License, Version 1.0. (See accompanying file 
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org/libs/test for the library home page.

#include <boost/test/execution_monitor.hpp>

#include <iostream>

struct my_exception1
{
    explicit    my_exception1( int res_code ) : m_res_code( res_code ) {}

    int         m_res_code;
};

struct my_exception2
{
    explicit    my_exception2( int res_code ) : m_res_code( res_code ) {}

    int         m_res_code;
};

struct dangerous_call_monitor : boost::execution_monitor
{
    explicit dangerous_call_monitor( int argc ) : m_argc( argc ) {}

    virtual int function()
    {
        // here we perform some operation under monitoring that could throw my_exception
        if( m_argc < 2 )
            throw my_exception1( 23 );
        if( m_argc > 3 )
            throw my_exception2( 45 );
        else if( m_argc > 2 )
            throw "too many args";

        return 1;
    }

    int m_argc;
};

void translate_my_exception1( my_exception1 const& ex )
{
    std::cout << "Caught my_exception1(" << ex.m_res_code << ")"<< std::endl;   
}

void translate_my_exception2( my_exception2 const& ex )
{
    std::cout << "Caught my_exception2(" << ex.m_res_code << ")"<< std::endl;   
}

int
main( int argc , char *[] )
{ 
    dangerous_call_monitor the_monitor( argc );

    the_monitor.register_exception_translator<my_exception1>( &translate_my_exception1 );
    the_monitor.register_exception_translator<my_exception2>( &translate_my_exception2 );

    try {
        the_monitor.execute();
    }
    catch ( boost::execution_exception const& ex ) {
        std::cout << "Caught exception: " << ex.what() << std::endl;
    }

    return 0;
}

// EOF
