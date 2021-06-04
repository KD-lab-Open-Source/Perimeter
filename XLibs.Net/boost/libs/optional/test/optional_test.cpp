// Copyright (C) 2003, Fernando Luis Cacciola Carballal.
//
// Use, modification, and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/lib/optional for documentation.
//
// You are welcome to contact the author at:
//  fernando_cacciola@hotmail.com
//
#include<iostream>
#include<stdexcept>
#include<string>

#define BOOST_ENABLE_ASSERT_HANDLER

#include "boost/optional.hpp"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "boost/detail/none.hpp"

#include "boost/test/minimal.hpp"

#include "optional_test_common.cpp"

void test_implicit_construction ( optional<double> opt, double v, double z )
{
  check_value(opt,v,z);
}

void test_implicit_construction ( optional<X> opt, X const& v, X const& z )
{
  check_value(opt,v,z);
}

void test_default_implicit_construction ( double, optional<double> opt )
{
  BOOST_CHECK(!opt);
}

void test_default_implicit_construction ( X const&, optional<X> opt )
{
  BOOST_CHECK(!opt);
}

//
// Basic test.
// Check ordinary functionality:
//   Initialization, assignment, comparison and value-accessing.
//
template<class T>
void test_basics( T const* )
{
  TRACE( std::endl << BOOST_CURRENT_FUNCTION  );

  T z(0);

  T a(1);

  // Default construction.
  // 'def' state is Uninitialized.
  // T::T() is not called (and it is not even defined)
  optional<T> def ;
  check_uninitialized(def);

  // Implicit construction
  // The first parameter is implicitely converted to optional<T>(a);
  test_implicit_construction(a,a,z);
  
  // Direct initialization.
  // 'oa' state is Initialized with 'a'
  // T::T( T const& x ) is used.
  set_pending_copy( ARG(T) ) ;
  optional<T> oa ( a ) ;
  check_is_not_pending_copy( ARG(T) );
  check_initialized(oa);
  check_value(oa,a,z);

  T b(2);

  optional<T> ob ;

  // Value-Assignment upon Uninitialized optional.
  // T::T ( T const& x ) is used.
  set_pending_copy( ARG(T) ) ;
  ob = a ;
  check_is_not_pending_copy( ARG(T) ) ;
  check_initialized(ob);
  check_value(ob,a,z);

  // Value-Assignment upon Initialized optional.
  // T::T ( T const& x ) is used
  set_pending_dtor( ARG(T) ) ;
  set_pending_copy( ARG(T) ) ;
  ob = b ;
  check_is_not_pending_dtor( ARG(T) ) ;
  check_is_not_pending_copy( ARG(T) ) ;
  check_initialized(ob);
  check_value(ob,b,z);

  // Assignment initialization.
  // T::T ( T const& x ) is used to copy new value.
  set_pending_copy( ARG(T) ) ;
  optional<T> const oa2 ( oa ) ;
  check_is_not_pending_copy( ARG(T) ) ;
  check_initialized_const(oa2);
  check_value_const(oa2,a,z);

  // Assignment
  // T::~T() is used to destroy previous value in ob.
  // T::T ( T const& x ) is used to copy new value.
  set_pending_dtor( ARG(T) ) ;
  set_pending_copy( ARG(T) ) ;
  oa = ob ;
  check_is_not_pending_dtor( ARG(T) ) ;
  check_is_not_pending_copy( ARG(T) ) ;
  check_initialized(oa);
  check_value(oa,b,z);

  // Uninitializing Assignment upon Initialized Optional
  // T::~T() is used to destroy previous value in oa.
  set_pending_dtor( ARG(T) ) ;
  set_pending_copy( ARG(T) ) ;
  oa = def ;
  check_is_not_pending_dtor( ARG(T) ) ;
  check_is_pending_copy    ( ARG(T) ) ;
  check_uninitialized(oa);

  // Uninitializing Assignment upon Uninitialized Optional
  // (Dtor is not called this time)
  set_pending_dtor( ARG(T) ) ;
  set_pending_copy( ARG(T) ) ;
  oa = def ;
  check_is_pending_dtor( ARG(T) ) ;
  check_is_pending_copy( ARG(T) ) ;
  check_uninitialized(oa);

  // Deinitialization of Initialized Optional
  // T::~T() is used to destroy previous value in ob.
  set_pending_dtor( ARG(T) ) ;
  ob.reset();
  check_is_not_pending_dtor( ARG(T) ) ;
  check_uninitialized(ob);

  // Deinitialization of Uninitialized Optional
  // (Dtor is not called this time)
  set_pending_dtor( ARG(T) ) ;
  ob.reset();
  check_is_pending_dtor( ARG(T) ) ;
  check_uninitialized(ob);
}

//
// Test Direct Value Manipulation
//
template<class T>
void test_direct_value_manip( T const* )
{
  TRACE( std::endl << BOOST_CURRENT_FUNCTION   );
  
  T x(3);

  optional<T> const c_opt0(x) ;
  optional<T>         opt0(x);

  BOOST_CHECK( c_opt0.get().V() == x.V() ) ;
  BOOST_CHECK(   opt0.get().V() == x.V() ) ;
  
  BOOST_CHECK( c_opt0->V() == x.V() ) ;
  BOOST_CHECK(   opt0->V() == x.V() ) ;

  BOOST_CHECK( (*c_opt0).V() == x.V() ) ;
  BOOST_CHECK( (*  opt0).V() == x.V() ) ;

  T y(4);
  opt0 = y ;
  BOOST_CHECK( get(opt0).V() == y.V() ) ;
}

//
// Test Uninitialized access assert
//
template<class T>
void test_uninitialized_access( T const* )
{
  TRACE( std::endl << BOOST_CURRENT_FUNCTION   );

  optional<T> def ;

  bool passed = false ;
  try
  {
    // This should throw because 'def' is uninitialized
    T const& n = def.get() ;
    unused_variable(n);
    passed = true ;
  }
  catch (...) {}
  BOOST_CHECK(!passed);

  passed = false ;
  try
  {
    // This should throw because 'def' is uninitialized
    T const& n = *def ;
    unused_variable(n);
    passed = true ;
  }
  catch (...) {}
  BOOST_CHECK(!passed);
  
  passed = false ;
  try
  {
    T v(5) ;
    unused_variable(v);
    // This should throw because 'def' is uninitialized
    *def = v ;
    passed = true ;
  }
  catch (...) {}
  BOOST_CHECK(!passed);

  passed = false ;
  try
  {
    // This should throw because 'def' is uninitialized
    T v = *(def.operator->()) ;
    unused_variable(v);
    passed = true ;
  }
  catch (...) {}
  BOOST_CHECK(!passed);
}

#if BOOST_WORKAROUND( BOOST_INTEL_CXX_VERSION, <= 700) // Intel C++ 7.0
void prevent_buggy_optimization( bool v ) {}
#endif

//
// Test Direct Initialization of optional for a T with throwing copy-ctor.
//
template<class T>
void test_throwing_direct_init( T const* )
{
  TRACE( std::endl << BOOST_CURRENT_FUNCTION   );

  T a(6);

  int count = get_instance_count( ARG(T) ) ;

  set_throw_on_copy( ARG(T) ) ;

  bool passed = false ;
  try
  {
    // This should:
    //   Attempt to copy construct 'a' and throw.
    // 'opt' won't be constructed.
    set_pending_copy( ARG(T) ) ;

#if BOOST_WORKAROUND( BOOST_INTEL_CXX_VERSION, <= 700) // Intel C++ 7.0
    // Intel C++ 7.0 specific:
    //    For some reason, when "check_is_not_pending_copy",
    //    after the exception block is reached,
    //    X::pending_copy==true even though X's copy ctor set it to false.
    //    I guessed there is some sort of optimization bug,
    //    and it seems to be the since the following additional line just
    //    solves the problem (!?)
    prevent_buggy_optimization(X::pending_copy);
#endif

    optional<T> opt(a) ;
    passed = true ;
  }
  catch ( ... ){}

  BOOST_CHECK(!passed);
  check_is_not_pending_copy( ARG(T) );
  check_instance_count(count, ARG(T) );
}

//
// Test Value Assignment to an Uninitialized optional for a T with a throwing copy-ctor
//
template<class T>
void test_throwing_val_assign_on_uninitialized( T const* )
{
  TRACE( std::endl << BOOST_CURRENT_FUNCTION   );

  T a(7);

  int count = get_instance_count( ARG(T) ) ;

  set_throw_on_copy( ARG(T) ) ;

  optional<T> opt ;

  bool passed = false ;
  try
  {
    // This should:
    //   Attempt to copy construct 'a' and throw.
    //   opt should be left uninitialized.
    set_pending_copy( ARG(T) ) ;
    opt.reset( a );
    passed = true ;
  }
  catch ( ... ) {}

  BOOST_CHECK(!passed);

  check_is_not_pending_copy( ARG(T) );
  check_instance_count(count, ARG(T) );
  check_uninitialized(opt);
}

//
// Test Value Reset on an Initialized optional for a T with a throwing copy-ctor
//
template<class T>
void test_throwing_val_assign_on_initialized( T const* )
{
  TRACE( std::endl << BOOST_CURRENT_FUNCTION   );

  T z(0);
  T a(8);
  T b(9);

  int count = get_instance_count( ARG(T) ) ;

  reset_throw_on_copy( ARG(T) ) ;

  optional<T> opt ( b ) ;
  ++ count ;

  check_instance_count(count, ARG(T) );

  check_value(opt,b,z);

  set_throw_on_copy( ARG(T) ) ;

  bool passed = false ;
  try
  {
    // This should:
    //   Attempt to copy construct 'a' and throw.
    //   opt should be left uninitialized (even though it was initialized)
    set_pending_dtor( ARG(T) ) ;
    set_pending_copy( ARG(T) ) ;
    opt.reset ( a ) ;
    passed = true ;
  }
  catch ( ... ) {}

  BOOST_CHECK(!passed);

  -- count ;
  
  check_is_not_pending_dtor( ARG(T) );
  check_is_not_pending_copy( ARG(T) );
  check_instance_count(count, ARG(T) );
  check_uninitialized(opt);
}

//
// Test Copy Initialization from an Initialized optional for a T with a throwing copy-ctor
//
template<class T>
void test_throwing_copy_initialization( T const* )
{
  TRACE( std::endl << BOOST_CURRENT_FUNCTION   );

  T z(0);
  T a(10);

  reset_throw_on_copy( ARG(T) ) ;

  optional<T> opt (a);

  int count = get_instance_count( ARG(T) ) ;

  set_throw_on_copy( ARG(T) ) ;

  bool passed = false ;
  try
  {
    // This should:
    //   Attempt to copy construct 'opt' and throw.
    //   opt1 won't be constructed.
    set_pending_copy( ARG(T) ) ;
    optional<T> opt1 = opt ;
    passed = true ;
  }
  catch ( ... ) {}

  BOOST_CHECK(!passed);

  check_is_not_pending_copy( ARG(T) );
  check_instance_count(count, ARG(T) );

  // Nothing should have happened to the source optional.
  check_initialized(opt);
  check_value(opt,a,z);
}

//
// Test Assignment to an Uninitialized optional from an Initialized optional
// for a T with a throwing copy-ctor
//
template<class T>
void test_throwing_assign_to_uninitialized( T const* )
{
  TRACE( std::endl << BOOST_CURRENT_FUNCTION   );

  T z(0);
  T a(11);

  reset_throw_on_copy( ARG(T) ) ;

  optional<T> opt0 ;
  optional<T> opt1(a) ;

  int count = get_instance_count( ARG(T) ) ;

  set_throw_on_copy( ARG(T) ) ;

  bool passed = false ;
  try
  {
    // This should:
    //   Attempt to copy construct 'opt1.value()' into opt0 and throw.
    //   opt0 should be left uninitialized.
    set_pending_copy( ARG(T) ) ;
    opt0 = opt1 ;
    passed = true ;
  }
  catch ( ... ) {}

  BOOST_CHECK(!passed);

  check_is_not_pending_copy( ARG(T) );
  check_instance_count(count, ARG(T) );
  check_uninitialized(opt0);
}

//
// Test Assignment to an Initialized optional from an Initialized optional
// for a T with a throwing copy-ctor
//
template<class T>
void test_throwing_assign_to_initialized( T const* )
{
  TRACE( std::endl << BOOST_CURRENT_FUNCTION   );

  T z(0);
  T a(12);
  T b(13);

  reset_throw_on_copy( ARG(T) ) ;

  optional<T> opt0(a) ;
  optional<T> opt1(b) ;

  int count = get_instance_count( ARG(T) ) ;

  set_throw_on_copy( ARG(T) ) ;

  bool passed = false ;
  try
  {
    // This should:
    //   Attempt to copy construct 'opt1.value()' into opt0 and throw.
    //   opt0 should be left unmodified or uninitialized
    set_pending_dtor( ARG(T) ) ;
    set_pending_copy( ARG(T) ) ;
    opt0 = opt1 ;
    passed = true ;
  }
  catch ( ... ) {}

  BOOST_CHECK(!passed);

  // opt0 was left uninitialized
  -- count ;
  check_is_not_pending_dtor( ARG(T) );
  check_is_not_pending_copy( ARG(T) );
  check_instance_count(count, ARG(T) );
  check_uninitialized(opt0);
}

//
// Test swap in a no-throwing case
//
template<class T>
void test_no_throwing_swap( T const* )
{
  TRACE( std::endl << BOOST_CURRENT_FUNCTION   );
  
  T z(0);
  T a(14);
  T b(15);

  reset_throw_on_copy( ARG(T) ) ;

  optional<T> def0 ;
  optional<T> def1 ;
  optional<T> opt0(a) ;
  optional<T> opt1(b) ;

  int count = get_instance_count( ARG(T) ) ;

  swap(def0,def1);
  check_uninitialized(def0);
  check_uninitialized(def1);

  swap(def0,opt0);
  check_uninitialized(opt0);
  check_initialized(def0);
  check_value(def0,a,z);

  // restore def0 and opt0
  swap(def0,opt0);

  swap(opt0,opt1);
  check_instance_count(count, ARG(T) );
  check_initialized(opt0);
  check_initialized(opt1);
  check_value(opt0,b,z);
  check_value(opt1,a,z);
}

//
// Test swap in a throwing case
//
template<class T>
void test_throwing_swap( T const* )
{
  TRACE( std::endl << BOOST_CURRENT_FUNCTION   );
  
  T a(16);
  T b(17);

  reset_throw_on_copy( ARG(T) ) ;

  optional<T> opt0(a) ;
  optional<T> opt1(b) ;

  set_throw_on_copy( ARG(T) ) ;

  //
  // Case 1: Both Initialized.
  //
  bool passed = false ;
  try
  {
    // This should attempt to swap optionals and fail at swap(X&,X&).
    swap(opt0,opt1);

    passed = true ;
  }
  catch ( ... ) {}

  BOOST_CHECK(!passed);

  // Assuming swap(T&,T&) has at least the basic guarantee, these should hold.
  BOOST_CHECK( ( !opt0 || ( !!opt0 && ( ( *opt0 == a ) || ( *opt0 == b ) ) ) ) ) ;
  BOOST_CHECK( ( !opt1 || ( !!opt1 && ( ( *opt1 == a ) || ( *opt1 == b ) ) ) ) ) ;

  //
  // Case 2: Only one Initialized.
  //
  reset_throw_on_copy( ARG(T) ) ;

  opt0.reset();
  opt1.reset(a);

  set_throw_on_copy( ARG(T) ) ;

  passed = false ;
  try
  {
    // This should attempt to swap optionals and fail at opt0.reset(*opt1)
    // opt0 should be left uninitialized and opt1 unchanged.
    swap(opt0,opt1);

    passed = true ;
  }
  catch ( ... ) {}

  BOOST_CHECK(!passed);

  check_uninitialized(opt0);
  check_initialized(opt1);
  check_value(opt1,a,b);
}

//
// This verifies relational operators.
//
template<class T>
void test_relops( T const* )
{
  TRACE( std::endl << BOOST_CURRENT_FUNCTION   );
  
  reset_throw_on_copy( ARG(T) ) ;
  
  T v0(18);
  T v1(19);
  T v2(19);

  optional<T> def0 ;
  optional<T> def1 ;
  optional<T> opt0(v0);
  optional<T> opt1(v1);
  optional<T> opt2(v2);

  // Check identity
  BOOST_CHECK ( def0 == def0 ) ;
  BOOST_CHECK ( opt0 == opt0 ) ;
  BOOST_CHECK ( !(def0 != def0) ) ;
  BOOST_CHECK ( !(opt0 != opt0) ) ;

  // Check when both are uininitalized.
  BOOST_CHECK (   def0 == def1  ) ; // both uninitialized compare equal
  BOOST_CHECK ( !(def0 <  def1) ) ; // uninitialized is never less    than uninitialized 
  BOOST_CHECK ( !(def0 >  def1) ) ; // uninitialized is never greater than uninitialized
  BOOST_CHECK ( !(def0 != def1) ) ;
  BOOST_CHECK (   def0 <= def1  ) ; 
  BOOST_CHECK (   def0 >= def1  ) ; 

  // Check when only lhs is uninitialized.
  BOOST_CHECK (   def0 != opt0  ) ; // uninitialized is never equal to initialized
  BOOST_CHECK ( !(def0 == opt0) ) ;
  BOOST_CHECK (   def0 <  opt0  ) ; // uninitialized is always less than initialized
  BOOST_CHECK ( !(def0 >  opt0) ) ;
  BOOST_CHECK (   def0 <= opt0  ) ;
  BOOST_CHECK ( !(def0 >= opt0) ) ;

  // Check when only rhs is uninitialized.
  BOOST_CHECK (   opt0 != def0  ) ; // initialized is never equal to uninitialized
  BOOST_CHECK ( !(opt0 == def0) ) ;
  BOOST_CHECK ( !(opt0 <  def0) ) ; // initialized is never less than uninitialized
  BOOST_CHECK (   opt0 >  def0  ) ;
  BOOST_CHECK ( !(opt0 <= def0) ) ;
  BOOST_CHECK (   opt0 >= opt0  ) ;

  // If both are initialized, values are compared
  BOOST_CHECK ( opt0 != opt1 ) ;
  BOOST_CHECK ( opt1 == opt2 ) ;
  BOOST_CHECK ( opt0 <  opt1 ) ;
  BOOST_CHECK ( opt1 >  opt0 ) ;
  BOOST_CHECK ( opt1 <= opt2 ) ;
  BOOST_CHECK ( opt1 >= opt0 ) ;
}

template<class T>
void test_none( T const* )
{
  TRACE( std::endl << BOOST_CURRENT_FUNCTION   );

  using boost::none ;
  
  optional<T> def0 ;
  optional<T> def1(none) ;
  optional<T> non_def( T(1234) ) ;

  BOOST_CHECK ( def0    == none ) ;
  BOOST_CHECK ( non_def != none ) ;
  BOOST_CHECK ( !def1           ) ;

  non_def = none ;
  BOOST_CHECK ( !non_def ) ;

  test_default_implicit_construction(T(1),none);
}

void test_with_builtin_types()
{
  TRACE( std::endl << BOOST_CURRENT_FUNCTION   );
  
  test_basics( ARG(double) );
  test_uninitialized_access( ARG(double) );
  test_no_throwing_swap( ARG(double) );
  test_relops( ARG(double) ) ;
  test_none( ARG(double) ) ;
}

void test_with_class_type()
{
  TRACE( std::endl << BOOST_CURRENT_FUNCTION   );

  test_basics( ARG(X) );
  test_direct_value_manip( ARG(X) );
  test_uninitialized_access( ARG(X) );
  test_throwing_direct_init( ARG(X) );
  test_throwing_val_assign_on_uninitialized( ARG(X) );
  test_throwing_val_assign_on_initialized( ARG(X) );
  test_throwing_copy_initialization( ARG(X) );
  test_throwing_assign_to_uninitialized( ARG(X) );
  test_throwing_assign_to_initialized( ARG(X) );
  test_no_throwing_swap( ARG(X) );
  test_throwing_swap( ARG(X) );
  test_relops( ARG(X) ) ;
  test_none( ARG(X) ) ;
  BOOST_CHECK ( X::count == 0 ) ;
}

int eat ( bool ) { return 1 ; }
int eat ( char ) { return 1 ; }
int eat ( int  ) { return 1 ; }
int eat ( void const* ) { return 1 ; }

template<class T> int eat ( T ) { return 0 ; }

//
// This verifies that operator safe_bool() behaves properly.
//
template<class T>
void test_no_implicit_conversions_impl( T const& )
{
  TRACE( std::endl << BOOST_CURRENT_FUNCTION   );

  optional<T> def ;
  BOOST_CHECK ( eat(def) == 0 ) ;
}

void test_no_implicit_conversions()
{
  TRACE( std::endl << BOOST_CURRENT_FUNCTION   );

  bool b = false ;
  char c = 0 ;
  int i = 0 ;
  void const* p = 0 ;

  test_no_implicit_conversions_impl(b);
  test_no_implicit_conversions_impl(c);
  test_no_implicit_conversions_impl(i);
  test_no_implicit_conversions_impl(p);
}

struct A {} ;
void test_conversions1()
{
  TRACE( std::endl << BOOST_CURRENT_FUNCTION );

#ifndef BOOST_OPTIONAL_NO_CONVERTING_COPY_CTOR
  char c = 20 ;
  optional<char> opt0(c);
  optional<int> opt1(opt0);
  BOOST_CHECK(*opt1 == static_cast<int>(c));
#endif

#ifndef BOOST_OPTIONAL_NO_CONVERTING_ASSIGNMENT
  float f = 21.22f ;
  double d = f ;
  optional<float> opt2(f) ;
  optional<double> opt3 ;
  opt3 = opt2 ;
  BOOST_CHECK(*opt3 == d);
#endif  
}

void test_conversions2()
{
  TRACE( std::endl << BOOST_CURRENT_FUNCTION );

  char c = 20 ;
  optional<int> opt(c);
  BOOST_CHECK( get(opt) == static_cast<int>(c));

  float f = 21.22f ;
  optional<double> opt1;
  opt1 = f ;
  BOOST_CHECK(*get(&opt1) == static_cast<double>(f));
}

int test_main( int, char* [] )
{
  try
  {
    test_with_class_type();
    test_with_builtin_types();
    test_no_implicit_conversions();
    test_conversions1();
    test_conversions2();
  }
  catch ( ... )
  {
    BOOST_ERROR("Unexpected Exception caught!");
  }

  return 0;
}


