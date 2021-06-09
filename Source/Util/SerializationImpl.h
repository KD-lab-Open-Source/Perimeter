#ifndef __SERIALIZATION_IMPL_H__
#define __SERIALIZATION_IMPL_H__

#include <boost/type_traits/is_pointer.hpp>
#include <boost/type_traits/is_array.hpp>
#include <boost/type_traits/is_fundamental.hpp>
#include <boost/mpl/equal_to.hpp>
/*
#include <boost/mpl/apply_if.hpp>
#include <boost/mpl/identity.hpp>
*/
#include "SerializationHelpers.h"
#include <boost/mpl/int.hpp>
#include <boost/type_traits/remove_const.hpp>

#include "RangedWrapper.h"

using namespace boost;

#ifdef _MSC_VER
namespace std {
template<class T, class A> class list;
template<class T, class A> class vector;
template<class T1, class T2> struct pair;
}
#endif

template<class T>
struct WrapperTraits {
    typedef T unwrapped_type;
    static T& unwrap (T& _data) {
        return _data;
    }
};

template<class T>
struct WrapperTraits< RangedWrapper<T> > {
    typedef T unwrapped_type;
    static T& unwrap (RangedWrapper<T>& _data) {
        return _data.value ();
    }
};


template<class T>
struct IsPrimitive
{
	enum { value = ::boost::is_fundamental<T>::value };
};

template<class T, class A> struct IsPrimitive<vector<T, A> > {  
	enum { value = true };  
};

template<class T, class A> struct IsPrimitive<list<T, A> > {  
	enum { value = true };  
};

template<class T> struct IsPrimitive<ShareHandle<T> > {  
	enum { value = true };  
};

template<class T> struct IsPrimitive<EnumWrapper<T> > {  
	enum { value = true };  
};

template<class T, class V> struct IsPrimitive<BitVector<T, V> > {  
	enum { value = true };  
};

template<> struct IsPrimitive<std::string> {  
	enum { value = true };  
};

template<> struct IsPrimitive<PrmString> {  
	enum { value = true };  
};

template<> struct IsPrimitive<CustomString> {  
	enum { value = true };  
};

template<> struct IsPrimitive<ComboListString> {  
	enum { value = true };  
};

template<class T1, class T2> struct IsPrimitive<std::pair<T1, T2> > {  
	enum { value = true };  
};


template<class T>
struct SerializationDefaultValue {
	static T get() {
		return T();
	}
};

template<class T>
struct SerializationDefaultValue<T*> {
	static T* get() {
		return 0;
	}
};

template<class Archive, class T>
void convertT(Archive& ar, T& t) {}


#endif //__SERIALIZATION_IMPL_H__


#ifdef REGISTER_CLASS
#undef REGISTER_CLASS
#undef REGISTER_CLASS_EDIT
#undef REGISTER_CLASS_XPRM
#undef REGISTER_CLASS_BINARY
#undef REGISTER_CLASS_CONTAINER
#endif

#ifdef __EDIT_ARCHIVE_H__ 
#define REGISTER_CLASS_EDIT(baseClass, derivedClass, classNameAlt) \
	EditClassDescriptor<baseClass>::EditSerializer<derivedClass> serializerEdit##baseClass##derivedClass(classNameAlt); 
#else
#define REGISTER_CLASS_EDIT(baseClass, derivedClass, classNameAlt) 
#endif 

#ifdef __XPRM_ARCHIVE_H__ 
#define REGISTER_CLASS_XPRM(baseClass, derivedClass, classNameAlt) \
	ClassDescriptor<baseClass, XPrmOArchive, XPrmIArchive>::Serializer<derivedClass> serializerXPrm##baseClass##derivedClass(classNameAlt); 
#else
#define REGISTER_CLASS_XPRM(baseClass, derivedClass, classNameAlt) 
#endif 

#ifdef __BINARY_ARCHIVE_H__ 
#define REGISTER_CLASS_BINARY(baseClass, derivedClass, classNameAlt) \
	BinaryClassDescriptor<baseClass>::BinarySerializer<derivedClass> serializerBinary##baseClass##derivedClass; 
#else
#define REGISTER_CLASS_BINARY(baseClass, derivedClass, classNameAlt) 
#endif 

#ifdef __ARCHIVE_CONTAINER_H_INCLUDED__
#define REGISTER_CLASS_CONTAINER(baseClass, derivedClass, classNameAlt) \
    ArchiveContainerClassDescriptor<baseClass>::ArchiveContainerSerializer<derivedClass> serializerContainer##baseClass##derivedClass(classNameAlt);
#else
#define REGISTER_CLASS_CONTAINER(baseClass, derivedClass, classNameAlt)
#endif

#define REGISTER_CLASS(baseClass, derivedClass, classNameAlt)  \
	REGISTER_CLASS_EDIT(baseClass, derivedClass, classNameAlt) \
	REGISTER_CLASS_CONTAINER(baseClass, derivedClass, classNameAlt) \
	REGISTER_CLASS_XPRM(baseClass, derivedClass, classNameAlt) \
	REGISTER_CLASS_BINARY(baseClass, derivedClass, classNameAlt)

