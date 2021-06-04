/********************************************************************
	created:	2002/08/30
	created:	30:8:2002   14:30
	filename: 	itr_traits_spec.hpp
	file base:	itr_traits_spec
	file ext:	hpp
	Pewerd by:	Илюха
	
	purpose:	обработка проблемы частичной специализации темплэйтов
				для свойств итераторов
*********************************************************************/

#ifndef __ITER_TRAITS_SPEC_HPP
#define __ITER_TRAITS_SPEC_HPP

#include <iterator>
#if _MSC_VER <= 1300
#define BROKEN_COMPILER_ITER_TRAITS_PTR_SPECIALIZATION_RANK1(_Tp)			\
template <> struct iterator_traits< _Tp* > {								\
	typedef random_access_iterator_tag iterator_category;					\
	typedef _Tp                         value_type;							\
	typedef ptrdiff_t                   difference_type;					\
	typedef _Tp*                        pointer;							\
	typedef _Tp&                        reference;							\
};																			\
/**/

#define BROKEN_COMPILER_ITER_TRAITS_PTR_SPECIALIZATION_RANK2(_Tp)				\
	BROKEN_COMPILER_ITER_TRAITS_PTR_SPECIALIZATION_RANK1(_Tp)					\
	BROKEN_COMPILER_ITER_TRAITS_PTR_SPECIALIZATION_RANK1(_Tp const)				\
	BROKEN_COMPILER_ITER_TRAITS_PTR_SPECIALIZATION_RANK1(_Tp volatile)			\
	BROKEN_COMPILER_ITER_TRAITS_PTR_SPECIALIZATION_RANK1(_Tp const volatile)	\
/**/

#if !defined _STLP_NO_NAMESPACES 
#define BEGIN_NAMESPACE_STD namespace std{
#define END_NAMESPACE_STD }
#else
#define BEGIN_NAMESPACE_STD
#define END_NAMESPACE_STD 
#endif 																			

#define BROKEN_COMPILER_ITER_TRAITS_PTR_SPECIALIZATION(_Tp)						\
	BEGIN_NAMESPACE_STD															\
	BROKEN_COMPILER_ITER_TRAITS_PTR_SPECIALIZATION_RANK2(_Tp)					\
	BROKEN_COMPILER_ITER_TRAITS_PTR_SPECIALIZATION_RANK2(_Tp*)					\
	BROKEN_COMPILER_ITER_TRAITS_PTR_SPECIALIZATION_RANK2(_Tp const *)			\
	BROKEN_COMPILER_ITER_TRAITS_PTR_SPECIALIZATION_RANK2(_Tp volatile *)		\
	BROKEN_COMPILER_ITER_TRAITS_PTR_SPECIALIZATION_RANK2(_Tp const volatile *)	\
	END_NAMESPACE_STD															\
/**/	
#else  //_MSC_VER <= 1300

#define BROKEN_COMPILER_ITER_TRAITS_PTR_SPECIALIZATION(_Tp)	

#endif

#if _MSC_VER <= 1300
#define BROKEN_COMPILER_PTR_ITER_TRAITS_SPECIALIZATION_RANK1(_Tp)				\
template <> struct pointer_iterator_traits< _Tp* >								\
{																				\
	typedef remove_const< _Tp >::type value_type;								\
	typedef _Tp* pointer;														\
	typedef _Tp& reference;														\
	typedef std::random_access_iterator_tag iterator_category;					\
	typedef std::ptrdiff_t difference_type;										\
};																				\

#define BROKEN_COMPILER_PTR_ITER_TRAITS_SPECIALIZATION_RANK2(_Tp)				\
	BROKEN_COMPILER_PTR_ITER_TRAITS_SPECIALIZATION_RANK1(_Tp)					\
	BROKEN_COMPILER_PTR_ITER_TRAITS_SPECIALIZATION_RANK1(_Tp const)				\
	BROKEN_COMPILER_PTR_ITER_TRAITS_SPECIALIZATION_RANK1(_Tp volatile)			\
	BROKEN_COMPILER_PTR_ITER_TRAITS_SPECIALIZATION_RANK1(_Tp const volatile)	\
	/**/

#define BROKEN_COMPILER_PTR_ITER_TRAITS_SPECIALIZATION(_Tp)						\
namespace boost{																\
namespace detail{																\
	BROKEN_COMPILER_PTR_ITER_TRAITS_SPECIALIZATION_RANK2(_Tp)					\
	BROKEN_COMPILER_PTR_ITER_TRAITS_SPECIALIZATION_RANK2(_Tp*)					\
	BROKEN_COMPILER_PTR_ITER_TRAITS_SPECIALIZATION_RANK2(_Tp const *)			\
	BROKEN_COMPILER_PTR_ITER_TRAITS_SPECIALIZATION_RANK2(_Tp volatile *)		\
	BROKEN_COMPILER_PTR_ITER_TRAITS_SPECIALIZATION_RANK2(_Tp const volatile *)	\
}																				\
}																				\
/**/								

#else //_MSC_VER <= 1300

#define BROKEN_COMPILER_PTR_ITER_TRAITS_SPECIALIZATION(_Tp)	

#endif // 

BROKEN_COMPILER_ITER_TRAITS_PTR_SPECIALIZATION(bool)
BROKEN_COMPILER_ITER_TRAITS_PTR_SPECIALIZATION(char)
BROKEN_COMPILER_ITER_TRAITS_PTR_SPECIALIZATION(signed   char)
BROKEN_COMPILER_ITER_TRAITS_PTR_SPECIALIZATION(unsigned char)
BROKEN_COMPILER_ITER_TRAITS_PTR_SPECIALIZATION(signed   short)
BROKEN_COMPILER_ITER_TRAITS_PTR_SPECIALIZATION(unsigned short)
BROKEN_COMPILER_ITER_TRAITS_PTR_SPECIALIZATION(signed   int)
BROKEN_COMPILER_ITER_TRAITS_PTR_SPECIALIZATION(unsigned int)
BROKEN_COMPILER_ITER_TRAITS_PTR_SPECIALIZATION(signed   long)
BROKEN_COMPILER_ITER_TRAITS_PTR_SPECIALIZATION(unsigned long)
BROKEN_COMPILER_ITER_TRAITS_PTR_SPECIALIZATION(float)
BROKEN_COMPILER_ITER_TRAITS_PTR_SPECIALIZATION(double)
BROKEN_COMPILER_ITER_TRAITS_PTR_SPECIALIZATION(long double)

#endif//#ifndef __ITER_TRAITS_SPEC_HPP