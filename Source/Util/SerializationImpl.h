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

//Serialization code for xmath.h

template<class Archive>
void Vect2f::serialize(Archive& ar) {
    if((ar.type() & (ARCHIVE_EDIT | ARCHIVE_BINARY)) || ar.isOutput()){
        ar & makeObjectWrapper(x, "x", "&x");
        ar & makeObjectWrapper(y, "y", "&y");
    }
    else
        convertT(ar, *this);
}

template<class Archive>
void Vect2i::serialize(Archive& ar) {
    if((ar.type() & (ARCHIVE_EDIT | ARCHIVE_BINARY)) || ar.isOutput()){
        ar & makeObjectWrapper(x, "x", "&x");
        ar & makeObjectWrapper(y, "y", "&y");
    }
    else
        convertT(ar, *this);
}

template<class Archive>
void Vect2s::serialize(Archive& ar) {
    ar & makeObjectWrapper(x, "x", "&x");
    ar & makeObjectWrapper(y, "y", "&y");
}

template<class Archive>
void Vect3f::serialize(Archive& ar) {
    if((ar.type() & (ARCHIVE_EDIT | ARCHIVE_BINARY)) || ar.isOutput()){
        ar & makeObjectWrapper(x, "x", "&x");
        ar & makeObjectWrapper(y, "y", "&y");
        ar & makeObjectWrapper(z, "z", "&z");
    }
    else
        convertT(ar, *this);
}

template<class Archive>
void Vect3d::serialize(Archive& ar) {
    ar & makeObjectWrapper(x, "x", "&x");
    ar & makeObjectWrapper(y, "y", "&y");
    ar & makeObjectWrapper(z, "z", "&z");
}

template<class Archive>
void Mat3f::serialize(Archive& ar) {
    ar & makeObjectWrapper(xrow(), "xrow", "xrow");
    ar & makeObjectWrapper(yrow(), "yrow", "yrow");
    ar & makeObjectWrapper(zrow(), "zrow", "zrow");
}

template<class Archive>
void Mat3d::serialize(Archive& ar) {
    ar & makeObjectWrapper(xrow(), "xrow", "xrow");
    ar & makeObjectWrapper(yrow(), "yrow", "yrow");
    ar & makeObjectWrapper(zrow(), "zrow", "zrow");
}

template<class Archive>
void MatXf::serialize(Archive& ar) {
    ar & makeObjectWrapper(rot(), "rotation", "Ориентация");
    ar & makeObjectWrapper(trans(), "position", "Позиция");
}

template<class Archive>
void MatXd::serialize(Archive& ar) {
    ar & makeObjectWrapper(rot(), "rotation", "Ориентация");
    ar & makeObjectWrapper(trans(), "position", "Позиция");
}

template<class Archive>
void QuatF::serialize(Archive& ar) {
    if((ar.type() & (ARCHIVE_EDIT | ARCHIVE_BINARY)) || ar.isOutput()){
        ar & makeObjectWrapper(s_, "s", "&s");
        ar & makeObjectWrapper(x_, "x", "&x");
        ar & makeObjectWrapper(y_, "y", "&y");
        ar & makeObjectWrapper(z_, "z", "&z");
    }
    else
        convertT(ar, *this);
}

template<class Archive>
void QuatD::serialize(Archive& ar) {
    ar & makeObjectWrapper(s_, "s", "&s");
    ar & makeObjectWrapper(x_, "x", "&x");
    ar & makeObjectWrapper(y_, "y", "&y");
    ar & makeObjectWrapper(z_, "z", "&z");
}

template<class Archive>
void Se3f::serialize(Archive& ar) {
    ar & makeObjectWrapper(rot(), "rotation", "Ориентация");
    ar & makeObjectWrapper(trans(), "position", "Позиция");
}

template<class Archive>
void Se3d::serialize(Archive& ar) {
    ar & makeObjectWrapper(rot(), "rotation", "Ориентация");
    ar & makeObjectWrapper(trans(), "position", "Позиция");
}

template<class Archive>
void Vect4f::serialize(Archive& ar) {
    ar & makeObjectWrapper(x, "x", "&x");
    ar & makeObjectWrapper(y, "y", "&y");
    ar & makeObjectWrapper(z, "z", "&z");
    ar & makeObjectWrapper(z, "w", "&w");
}

template<class Archive>
void Mat4f::serialize(Archive& ar) {
    ar & makeObjectWrapper(xrow(), "xrow", "xrow");
    ar & makeObjectWrapper(yrow(), "yrow", "yrow");
    ar & makeObjectWrapper(zrow(), "zrow", "zrow");
    ar & makeObjectWrapper(wrow(), "wrow", "wrow");
}

////////

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

