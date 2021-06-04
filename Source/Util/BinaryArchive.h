/*
	Бинарный архив. 

В отличии от XPrmArchive, BinaryArchive представляют
собой просто бинарный поток. В начале - заголовок:

	"BinX" + int(version)

Версии необходимы для ручного контроля конвертации 
старых данных. Для этого во все архивы введена функция
laterThen(version) ("позже, чем"): true, если версия файла
больше, чем указанная (для всех других потоков - всегда true).

	// v1
	ar & WRAP_OBJECT(obj1);
	ar & WRAP_OBJECT(obj2);
	ar & WRAP_OBJECT(obj3);

	// v2
	if(!ar.laterThan(1))
		ar & WRAP_OBJECT(obj1); // старый объект прочитается только в v1
	ar & WRAP_OBJECT(obj2);
	ar & WRAP_OBJECT(obj3);
	if(ar.laterThan(1))
		ar & WRAP_OBJECT(obj4); // новый объект прочитается в v2, запишется и отредактируется везде

EnumWrapper и BitVector пишутся по значению,
а не по именам. При изменении значений enum'ов 
конвертировать данные вручную.

Вместо имен классов пишется 4-байтный хеш. Теоретически может возникнуть совпадение,
стоит предупреждение, выход - поменять чуть-чуть имя класса.

*/

#ifndef __BINARY_ARCHIVE_H__
#define __BINARY_ARCHIVE_H__
#pragma once

#include <vector>
#include "Handle.h"
#include "Serialization.h"
#include "SerializationImpl.h"

inline unsigned int stringHash(const char *str)
{
	unsigned int h = 0;
	while(*str)
		h = 5*h + *(str++);
	return h;
}

class BinaryOArchive 
{
public:
	BinaryOArchive(const char* fname, int version = 0);
	~BinaryOArchive();

	void open(const char* fname, int version = 0); 
	bool close();  // true if there were changes, so file was updated

	int type() const {
		return ARCHIVE_BINARY;
	}

	bool isInput() const {
		return false;
	}

	bool isOutput() const {
		return true;
	}

	bool laterThan(int version) const {
		return true;
	}

	template<class T>
    BinaryOArchive& operator<<(const T & t){
        return (*this) & t;
    }

    template<class T>
    BinaryOArchive& operator&(const ObjectWrapper<T> & t)
    {
        typedef WrapperTraits<T>::unwrapped_type U;

		using namespace SerializationHelpers;
		using SerializationHelpers::Identity;

        Select<IsPrimitive<U>,
            Identity<save_primitive_impl<U> >,
			Select<is_pointer<U>, 
				Identity<save_pointer_impl<U> >,
				Select<is_array<U>, 
					Identity<save_array_impl<U> >,
					Identity<save_non_primitive_impl<U> >
				>
			>
        >::type::invoke(*this, WrapperTraits<T>::unwrap (t.value()));
		return *this;
    }

	// To simulate sub-blocks
	bool openBlock(const char* name, const char* nameAlt) {
		return true;
	}

	void closeBlock() {
	}

private:
	XBuffer buffer_;
	string fileName_;

	///////////////////////////////////
	void saveString(const char* value) {
		if(value)
			buffer_ < char(1) < value < char(0);
		else
			buffer_ < char(0);
	}

	void openCollection(int counter){
		buffer_ < counter;
	}

    template<class T>
    struct save_primitive_impl {
        static void invoke(BinaryOArchive & ar, const T & t){
        	ar & t;
        }
    };

    template<class T>
    struct save_non_primitive_impl {
        static void invoke(BinaryOArchive & ar, const T & t){
            const_cast<T&>(t).serialize(ar);
        }
    };

	template<class T>
    struct save_pointer_impl {
        static void invoke(BinaryOArchive& ar, const T & t){
			ar.savePointer(t);
		}
    };

	template<class T>
    struct save_array_impl {
        static void invoke(BinaryOArchive& ar, const T & t){
			int count = sizeof(t) / (
				static_cast<const char *>(static_cast<const void *>(&t[1])) 
				- static_cast<const char *>(static_cast<const void *>(&t[0]))
			);
			ar.openCollection(count);
			for(int i = 0; i < count; ++i)
				ar & WRAP_NAME(t[i], 0);
		}
    };

	template<class T>
    BinaryOArchive& operator&(const T& value) {
		buffer_ < value;
		return *this;
	}

	template<class T>
	void savePointer(const T* t)
    {
		if(!t){
			buffer_ < stringHash("");
			return;
		}
		const char* name = typeid(*t).name();
		buffer_ < stringHash(name);
		BinaryClassDescriptor<remove_const<T>::type>::instance().find(name).save(*this, t);
	}

	template<class T, class A>
	BinaryOArchive& operator&(const std::vector<T, A>& cont){
		vector<T, A>::const_iterator i;
		openCollection(cont.size());
		FOR_EACH(cont, i)
			(*this) & WRAP_NAME(*i, 0);
		return *this;
	}

	template<class T, class A>
	BinaryOArchive& operator&(const std::list<T, A>& cont){
		list<T, A>::const_iterator i;
		openCollection(cont.size());
		FOR_EACH(cont, i)
			(*this) & WRAP_NAME(*i, 0);
		return *this;
	}

	template<class T1, class T2>
	BinaryOArchive& operator&(const std::pair<T1, T2>& t)
	{
		(*this) & WRAP_NAME(t.first, "first");
		(*this) & WRAP_NAME(t.second, "second");
		return *this;
	}

	template<class T>
	BinaryOArchive& operator&(const ShareHandle<T>& t)
	{
		return *this & WRAP_NAME(t.get(), 0);
	}

    template<class Enum>
	BinaryOArchive& operator&(const EnumWrapper<Enum>& t)
    {
		buffer_.write(t.value());
		return *this;
	}

    template<class Enum, class Value>
	BinaryOArchive& operator&(const BitVector<Enum, Value>& t)
    {
		buffer_.write(t.value());
		return *this;
	}

	BinaryOArchive& operator&(const PrmString& t)
    {
		saveString(t);
		return *this;
	}

	BinaryOArchive& operator&(const CustomString& t)
    {
		saveString(t);
		return *this;
	}

	BinaryOArchive& operator&(const ComboListString& t)
    {
		saveString(t);
		return *this;
	}

	BinaryOArchive& operator&(const string& str) { 
		saveString(str.c_str()); 
		return *this;
	}

    BinaryOArchive& operator&(const bool& value) {
		buffer_.write(value);
		return *this;
	}
};


class BinaryIArchive 
{
public:
	BinaryIArchive(const char* fname = 0);
	~BinaryIArchive();

	bool open(const char* fname);  // true if file exists
	void close();

	int type() const {
		return ARCHIVE_BINARY;
	}

	bool isInput() const {
		return true;
	}

	bool isOutput() const {
		return false;
	}

	bool laterThan(int version) const {
		return version_ > version;
	}

	template<class T>
    BinaryIArchive& operator>>(T& t){
		return (*this) & t;
    }

	template<class T>
    BinaryIArchive& operator&(const ObjectWrapper<T>& t)
    {
        typedef WrapperTraits<T>::unwrapped_type U;
		using namespace SerializationHelpers;
		using SerializationHelpers::Identity;

        Select<IsPrimitive<U>,
            Identity<load_primitive_impl<U> >,
			Select<is_pointer<U>, 
				Identity<load_pointer_impl<U> >,
				Select<is_array<U>, 
					Identity<load_array_impl<U> >,
					Identity<load_non_primitive_impl<U> >
				>
			>
        >::type::invoke(*this, WrapperTraits<T>::unwrap (t.value()));

		return *this;
	}

	// To simulate sub-blocks
	bool openBlock(const char* name, const char* nameAlt) {
		return true;
	}

	void closeBlock() {
	}

private:
	string fileName_;
	XBuffer buffer_;
	int version_;

	/////////////////////////////////////
	bool loadString(string& value); // false if zero string should be loaded

	int openCollection(){
		int counter;
		buffer_ > counter;
		return counter;
	}

    template<class T>
    struct load_primitive_impl {
        static void invoke(BinaryIArchive & ar, T & t){
        	ar & t;
        }
    };

    template<class T>
    struct load_non_primitive_impl {
        static void invoke(BinaryIArchive & ar, T & t){
            t.serialize(ar);
        }
    };

	template<class T>
    struct load_pointer_impl {
        static void invoke(BinaryIArchive& ar, T & t){
			ar.loadPointer(t);
		}
    };

	template<class T>
    struct load_array_impl {
        static void invoke(BinaryIArchive& ar, T & t){
			int count = sizeof(t) / (
				static_cast<const char *>(static_cast<const void *>(&t[1])) 
				- static_cast<const char *>(static_cast<const void *>(&t[0]))
			);
			int countOld = ar.openCollection();
			if(count < countOld){
				xassert(0 && "Array size too short");
				ErrH.Abort("Array size too short");
			}
			for(int i = 0; i < count; ++i)
				ar & WRAP_NAME(t[i], 0);
		}
    };

    template<class T>
	BinaryIArchive& operator&(T& value){
		buffer_ > value;
		return *this;
	}

	template<class T>
	void loadPointer(T*& t)
    {
		unsigned int hash;
		buffer_ > hash;
		if(hash == stringHash("")){
			if(t){
				delete t;
				t = 0;
			}
			return;
		}
		typedef BinaryClassDescriptor<remove_const<T>::type> Descriptor;
		if(t){
			if(hash == stringHash(typeid(*t).name())){
				Descriptor::instance().findByHash(hash).load(*this, t);
				return;
			}
			else{
				delete t;
				t = 0;
			}
		}
        Descriptor::instance().findByHash(hash).create(*this, t);
	}

	template<class T, class A>
	BinaryIArchive& operator&(vector<T, A>& cont)
	{
		int count = openCollection();
		if(count != cont.size()){
			cont.clear();
			cont.reserve(count);
			while(count--){
				cont.push_back(SerializationDefaultValue<T>::get());
				(*this) & WRAP_NAME(cont.back(), 0);
			}
		}
		else{
			vector<T, A>::iterator i;
			FOR_EACH(cont, i)
				(*this) & WRAP_NAME(*i, 0);
		}
		return *this;
	}

	template<class T, class A>
	BinaryIArchive& operator&(list<T, A>& cont)
	{
		int count = openCollection();
		if(count != cont.size()){
			cont.clear();
			while(count--){
				cont.push_back(SerializationDefaultValue<T>::get());
				(*this) & WRAP_NAME(cont.back(), 0);
			}
		}
		else{
			list<T, A>::iterator i;
			FOR_EACH(cont, i)
				(*this) & WRAP_NAME(*i, 0);
		}
		return *this;
	}

	template<class T1, class T2>
	BinaryIArchive& operator&(std::pair<T1, T2>& t)
	{
		(*this) & WRAP_NAME(t.first, "first");
		(*this) & WRAP_NAME(t.second, "second");
		return *this;
	}

	template<class T>
	BinaryIArchive& operator&(ShareHandle<T>& t)
	{
		T* ptr = t;
		if(ptr){
			ptr->addRef();
			t = 0;
			ptr->decrRef();
		}
		(*this) & WRAP_NAME(ptr, 0);
		t = ptr;
		return *this;
	}

	template<class Enum>
	BinaryIArchive& operator&(EnumWrapper<Enum>& t)
	{
		buffer_.read(t.value());
		return *this;
	}

	template<class Enum, class Value>			 
	BinaryIArchive& operator&(BitVector<Enum, Value>& t)
	{
		buffer_.read(t.value());
		return *this;
	}

	BinaryIArchive& operator&(PrmString& t)
	{
		string str;
		if(loadString(str))
			t = str;
		else
			t = 0;
		return *this;
	}

	BinaryIArchive& operator&(CustomString& t)
	{
		string str;
		loadString(str);
		t = str;
		return *this;
	}

	BinaryIArchive& operator&(ComboListString& t)
	{
		string str;
		loadString(str);
		t = str;
		return *this;
	}

	BinaryIArchive& operator&(string& value){
		loadString(value);
		return *this;
	}

	BinaryIArchive& operator&(bool& value){
		buffer_.read(value);
		return *this;
	}
};


template<class Base>
class BinaryClassDescriptor : public ClassDescriptor<Base, BinaryOArchive, BinaryIArchive>
{
public:
	template<class Derived>
	struct BinarySerializer : ClassDescriptor<Base, BinaryOArchive, BinaryIArchive>::Serializer<Derived>
	{
		BinarySerializer() {
			instance().add(*this, typeid(Derived).name());
		}
	};

	void add(SerializerBase& serializer, const char* name) {
		ClassDescriptor<Base, BinaryOArchive, BinaryIArchive>::add(serializer, name, 0);
		unsigned int hash = stringHash(name);
		if(mapHash_.find(hash) != mapHash_.end())
			ErrH.Abort("BinaryClassDescriptor: coincidence of hash code", XERR_USER, 0, name); 
        mapHash_[hash] = &serializer;
	}

	SerializerBase& findByHash(unsigned int hash) {
		MapHash::iterator i = mapHash_.find(hash);
		if(i == mapHash_.end()){
			xassert(0 && "Unregistered class");
			ErrH.Abort("BinaryClassDescriptor: Unregistered class");
		}
		return *i->second;
	}

	static BinaryClassDescriptor& instance() {
		return Singleton<BinaryClassDescriptor>::instance();
	}

private:
	typedef map<unsigned int, SerializerBase*> MapHash;
	MapHash mapHash_;
};


#endif //__BINARY_ARCHIVE_H__
