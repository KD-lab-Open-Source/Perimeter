
//  (C) Copyright Rani Sharoni,Robert Ramey and Pavel Vozenilek 2004. 
//  Use, modification and distribution are subject to the 
//  Boost Software License, Version 1.0. (See accompanying file 
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)


#include "test.hpp"
#include "check_integral_constant.hpp"
#ifdef TEST_STD
#  include <type_traits>
#else
#  include <boost/type_traits/is_abstract.hpp>
#endif


struct TestA {};
struct TestB { virtual void foo(void) = 0; };
struct TestC { private: virtual void foo(void) = 0; };
struct TestD : TestA {};
struct TestE : TestB {};
struct TestF : TestC {};
struct TestG : TestB { virtual void foo(void) {} };
struct TestH : TestC { private: virtual void foo(void) {} };
struct TestI : TestB, TestC {};
struct TestJ : TestI { virtual void foo(void) {} };
struct TestK : TestB { virtual void foo(void); virtual void foo2(void) = 0; };
struct TestL : TestK { virtual void foo2(void) {} };
struct TestM : virtual TestB {};
struct TestN : virtual TestC {};
struct TestO : TestM, TestN {};
struct TestP : TestO { virtual void foo(void) {} };
struct TestQ : TestB { virtual void foo(void) = 0; };
struct TestR : TestC { private: virtual void foo(void) = 0; };
struct TestS { virtual void foo(void) {} };
struct TestT { virtual ~TestT(void) {} virtual void foo(void) {} };
struct TestU : TestT { virtual void foo(void) = 0; };
struct TestV : TestT { virtual void foo(void) {} };
struct TestW { virtual void foo1(void) = 0; virtual void foo2(void) = 0; };
struct TestX : TestW { virtual void foo1(void) {}  virtual void foo2(void) {} };
struct TestY { virtual ~TestY(void) = 0; };
struct TestZ { virtual ~TestZ(void) = 0; }; TestZ::~TestZ(void) {}
struct TestAA : TestZ { virtual ~TestAA(void) = 0; }; TestAA::~TestAA(void) {}
struct TestAB : TestAA { virtual ~TestAB(void) {} }; 
struct TestAC { virtual void foo(void) = 0; }; void TestAC::foo(void) {}
struct TestAD : TestAC {};
struct TestAE : TestAD { virtual void foo() {} };
struct TestAF : TestAD { virtual void foo(); }; void TestAF::foo(void) {}
struct TestAG : virtual TestA {};


TT_TEST_BEGIN(is_abstract)

BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_abstract<TestA>::value), false);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_abstract<TestB>::value), true);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_abstract<TestC>::value), true);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_abstract<TestD>::value), false);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_abstract<TestE>::value), true);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_abstract<TestF>::value), true);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_abstract<TestG>::value), false);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_abstract<TestH>::value), false);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_abstract<TestI>::value), true);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_abstract<TestJ>::value), false); // only one method implemented!
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_abstract<TestK>::value), true);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_abstract<TestL>::value), false);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_abstract<TestM>::value), true);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_abstract<TestN>::value), true);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_abstract<TestO>::value), true);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_abstract<TestP>::value), false); // ???
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_abstract<TestQ>::value), true);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_abstract<TestR>::value), true);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_abstract<TestS>::value), false);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_abstract<TestT>::value), false);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_abstract<TestU>::value), true);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_abstract<TestV>::value), false);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_abstract<TestW>::value), true);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_abstract<TestX>::value), false);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_abstract<TestY>::value), true);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_abstract<TestZ>::value), true);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_abstract<TestAA>::value), true);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_abstract<TestAB>::value), false);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_abstract<TestAC>::value), true);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_abstract<TestAD>::value), true);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_abstract<TestAE>::value), false);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_abstract<TestAF>::value), false);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_abstract<TestAG>::value), false);

BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_abstract<const TestA>::value), false);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_abstract<const TestB>::value), true);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_abstract<const TestC>::value), true);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_abstract<const TestD>::value), false);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_abstract<const TestE>::value), true);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_abstract<const TestF>::value), true);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_abstract<const TestG>::value), false);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_abstract<const TestH>::value), false);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_abstract<const TestI>::value), true);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_abstract<const TestJ>::value), false); // only one method implemented!
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_abstract<const TestK>::value), true);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_abstract<const TestL>::value), false);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_abstract<const TestM>::value), true);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_abstract<const TestN>::value), true);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_abstract<const TestO>::value), true);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_abstract<const TestP>::value), false); // ???
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_abstract<const TestQ>::value), true);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_abstract<const TestR>::value), true);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_abstract<const TestS>::value), false);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_abstract<const TestT>::value), false);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_abstract<const TestU>::value), true);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_abstract<const TestV>::value), false);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_abstract<const TestW>::value), true);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_abstract<const TestX>::value), false);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_abstract<const TestY>::value), true);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_abstract<const TestZ>::value), true);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_abstract<const TestAA>::value), true);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_abstract<const TestAB>::value), false);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_abstract<const TestAC>::value), true);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_abstract<const TestAD>::value), true);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_abstract<const TestAE>::value), false);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_abstract<const TestAF>::value), false);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_abstract<const TestAG>::value), false);

BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_abstract<volatile TestA>::value), false);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_abstract<volatile TestB>::value), true);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_abstract<volatile TestC>::value), true);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_abstract<volatile TestD>::value), false);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_abstract<volatile TestE>::value), true);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_abstract<volatile TestF>::value), true);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_abstract<volatile TestG>::value), false);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_abstract<volatile TestH>::value), false);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_abstract<volatile TestI>::value), true);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_abstract<volatile TestJ>::value), false); // only one method implemented!
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_abstract<volatile TestK>::value), true);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_abstract<volatile TestL>::value), false);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_abstract<volatile TestM>::value), true);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_abstract<volatile TestN>::value), true);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_abstract<volatile TestO>::value), true);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_abstract<volatile TestP>::value), false); // ???
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_abstract<volatile TestQ>::value), true);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_abstract<volatile TestR>::value), true);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_abstract<volatile TestS>::value), false);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_abstract<volatile TestT>::value), false);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_abstract<volatile TestU>::value), true);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_abstract<volatile TestV>::value), false);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_abstract<volatile TestW>::value), true);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_abstract<volatile TestX>::value), false);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_abstract<volatile TestY>::value), true);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_abstract<volatile TestZ>::value), true);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_abstract<volatile TestAA>::value), true);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_abstract<volatile TestAB>::value), false);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_abstract<volatile TestAC>::value), true);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_abstract<volatile TestAD>::value), true);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_abstract<volatile TestAE>::value), false);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_abstract<volatile TestAF>::value), false);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_abstract<volatile TestAG>::value), false);

BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_abstract<volatile const TestA>::value), false);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_abstract<volatile const TestB>::value), true);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_abstract<volatile const TestC>::value), true);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_abstract<volatile const TestD>::value), false);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_abstract<volatile const TestE>::value), true);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_abstract<volatile const TestF>::value), true);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_abstract<volatile const TestG>::value), false);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_abstract<volatile const TestH>::value), false);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_abstract<volatile const TestI>::value), true);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_abstract<volatile const TestJ>::value), false); // only one method implemented!
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_abstract<volatile const TestK>::value), true);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_abstract<volatile const TestL>::value), false);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_abstract<volatile const TestM>::value), true);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_abstract<volatile const TestN>::value), true);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_abstract<volatile const TestO>::value), true);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_abstract<volatile const TestP>::value), false); // ???
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_abstract<volatile const TestQ>::value), true);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_abstract<volatile const TestR>::value), true);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_abstract<volatile const TestS>::value), false);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_abstract<volatile const TestT>::value), false);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_abstract<volatile const TestU>::value), true);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_abstract<volatile const TestV>::value), false);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_abstract<volatile const TestW>::value), true);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_abstract<volatile const TestX>::value), false);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_abstract<volatile const TestY>::value), true);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_abstract<volatile const TestZ>::value), true);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_abstract<volatile const TestAA>::value), true);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_abstract<volatile const TestAB>::value), false);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_abstract<volatile const TestAC>::value), true);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_abstract<volatile const TestAD>::value), true);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_abstract<volatile const TestAE>::value), false);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_abstract<volatile const TestAF>::value), false);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_abstract<volatile const TestAG>::value), false);

BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_abstract<TestA&>::value), false);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_abstract<TestB&>::value), false);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_abstract<TestC&>::value), false);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_abstract<TestD&>::value), false);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_abstract<TestE&>::value), false);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_abstract<TestF&>::value), false);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_abstract<TestG&>::value), false);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_abstract<TestH&>::value), false);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_abstract<TestI&>::value), false);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_abstract<TestJ&>::value), false); // only one method implemented!
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_abstract<TestK&>::value), false);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_abstract<TestL&>::value), false);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_abstract<TestM&>::value), false);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_abstract<TestN&>::value), false);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_abstract<TestO&>::value), false);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_abstract<TestP&>::value), false); // ???
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_abstract<TestQ&>::value), false);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_abstract<TestR&>::value), false);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_abstract<TestS&>::value), false);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_abstract<TestT&>::value), false);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_abstract<TestU&>::value), false);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_abstract<TestV&>::value), false);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_abstract<TestW&>::value), false);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_abstract<TestX&>::value), false);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_abstract<TestY&>::value), false);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_abstract<TestZ&>::value), false);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_abstract<TestAA&>::value), false);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_abstract<TestAB&>::value), false);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_abstract<TestAC&>::value), false);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_abstract<TestAD&>::value), false);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_abstract<TestAE&>::value), false);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_abstract<TestAF&>::value), false);
BOOST_CHECK_INTEGRAL_CONSTANT((::tt::is_abstract<TestAG&>::value), false);

TT_TEST_END







