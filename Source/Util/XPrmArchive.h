#ifndef __XPRM_ARCHIVE_H__
#define __XPRM_ARCHIVE_H__
#pragma once

#include <vector>
#include "Handle.h"
#include "Serialization.h"
#include "SerializationImpl.h"

//Forward declaration for XPrmOArchive
class XPrmIArchive;

class XPrmOArchive
{
public:
	XPrmOArchive(const char* fname);
	~XPrmOArchive();

	void open(const char* fname);
	bool close();  // true if there were changes, so file was updated

	int type() const {
		return ARCHIVE_TEXT;
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
    XPrmOArchive& operator<<(const T & t){
        return (*this) & t;
    }

    template<class T>
    XPrmOArchive& operator&(const ObjectWrapper<T> & t)
    {
		openNode(t.name());

        typedef typename WrapperTraits<T>::unwrapped_type U;
        using namespace SerializationHelpers;
		using SerializationHelpers::Identity;

        Select<IsPrimitive<U>,
            Identity<save_primitive_impl<U> >,
            Select<boost::is_pointer<U>, 
                Identity<save_pointer_impl<U> >,
                Select<boost::is_array<U>,
                    Identity<save_array_impl<U> >,
                    Identity<save_non_primitive_impl<U> >
                >
            >
        >::type::invoke(*this, WrapperTraits<T>::unwrap (t.value()));

		closeNode(t.name());
		return *this;
    }

	// To simulate sub-blocks
	bool openBlock(const char* name, const char* nameAlt) {
		//openNode(name);
		//openBracket();
		return true;
	}

	void closeBlock() {
		//closeBracket();
		//closeNode("");
	}

private:
	XBuffer buffer_;
	string offset_;
	string fileName_;

	///////////////////////////////////
	void saveString(const char* value) {
		buffer_ < value;
	}
	void saveStringEnclosed(const char* value);

	void openNode(const char* name) {
		if(name)
			buffer_ < offset_.c_str() < name < " = ";
	}

	void closeNode(const char* name) {
		if(name)
			buffer_ < ";\r\n";
	}

	void openBracket() {
		buffer_ < "{\r\n";
		offset_ += "\t";
	}
	void closeBracket() {
		offset_.pop_back();
		buffer_ < offset_.c_str() < "}";
	}

	void openCollection(int counter){
		openBracket();
		buffer_ < offset_.c_str() <= counter < ";\r\n";
	}
	template<class T>
	void saveElement(const T& t) {
		buffer_ < offset_.c_str();
		(*this) & WRAP_NAME(t, 0);
		buffer_ < ",\r\n";
	}
	void closeCollection(bool erasePrevComma) {
		if(erasePrevComma){
			buffer_ -= 3;
			buffer_ < "\r\n";
		}
		closeBracket();
	}

    template<class T>
    struct save_primitive_impl {
        static void invoke(XPrmOArchive & ar, const T & t){
        	ar & t;
        }
    };

    template<class T>
    struct save_non_primitive_impl {
        static void invoke(XPrmOArchive & ar, const T & t){
			if(!SuppressBracket<T>::value)
				ar.openBracket();
            const_cast<T&>(t).serialize(ar);
			if(!SuppressBracket<T>::value)
				ar.closeBracket();
        }
    };

	template<class T>
    struct save_pointer_impl {
        static void invoke(XPrmOArchive& ar, const T & t){
			ar.savePointer(t);
		}
    };

	template<class T>
    struct save_array_impl {
        static void invoke(XPrmOArchive& ar, const T & t){
			int count = sizeof(t) / (
				static_cast<const char *>(static_cast<const void *>(&t[1]))
				- static_cast<const char *>(static_cast<const void *>(&t[0]))
			);
			ar.openCollection(count);
			for(int i = 0; i < count; ++i){
				ar.saveElement(t[i]);
			}
			ar.closeCollection(count);
		}
    };

	template<class T>
    XPrmOArchive& operator&(const T& value) {
		buffer_ <= value;
		return *this;
	}

	template<class T>
	void savePointer(const T* t)
    {
		if(!t){
			buffer_ < "0";
			return;
		}
		const char *name = get_type_id<T>().c_str();
		saveStringEnclosed(name);
		buffer_ < " ";
		ClassDescriptor<typename boost::remove_const<T>::type, XPrmOArchive, XPrmIArchive>::instance().find(name).save(*this, t);
	}

	template<class T, class A>
	XPrmOArchive& operator&(const std::vector<T, A>& cont){
		typename vector<T, A>::const_iterator i;
		openCollection(cont.size());
		FOR_EACH(cont, i)
			saveElement(*i);
		closeCollection(!cont.empty());
		return *this;
	}

	template<class T, class A>
	XPrmOArchive& operator&(const std::list<T, A>& cont){
		typename list<T, A>::const_iterator i;
		openCollection(cont.size());
		FOR_EACH(cont, i)
			saveElement(*i);
		closeCollection(!cont.empty());
		return *this;
	}

	template<class T1, class T2>
	XPrmOArchive& operator&(const std::pair<T1, T2>& t)
	{
		openBracket();
		(*this) & WRAP_NAME(t.first, "first");
		(*this) & WRAP_NAME(t.second, "second");
		closeBracket();
		return *this;
	}

	template<class T>
	XPrmOArchive& operator&(const ShareHandle<T>& t)
	{
		return *this & WRAP_NAME(t.get(), 0);
	}

    template<class Enum>
	XPrmOArchive& operator&(const EnumWrapper<Enum>& t)
    {
		const EnumDescriptor<Enum>& descriptor = getEnumDescriptor(Enum(0));
		saveString(descriptor.name(t.value()));
		return *this;
	}

    template<class Enum, class Value>
	XPrmOArchive& operator&(const BitVector<Enum, Value>& t)
    {
		const EnumDescriptor<Enum>& descriptor = getEnumDescriptor(Enum(0));
		saveString(descriptor.nameCombination(t.value()).c_str());
		return *this;
	}

	XPrmOArchive& operator&(const PrmString& t)
    {
		saveStringEnclosed(t);
		return *this;
	}

	XPrmOArchive& operator&(const CustomString& t)
    {
		saveStringEnclosed(t);
		return *this;
	}

	XPrmOArchive& operator&(const ComboListString& t)
    {
		saveStringEnclosed(t);
		return *this;
	}

	XPrmOArchive& operator&(const string& str) {
		saveStringEnclosed(str.c_str());
		return *this;
	}

    XPrmOArchive& operator&(const bool& value) {
		buffer_ < (value ? "true" : "false");
		return *this;
	}
};

class XPrmIArchive 
{
public:
	XPrmIArchive(const char* fname = 0);
	~XPrmIArchive();

	bool open(const char* fname);  // true if file exists
	void close();
	bool findSection(const char* sectionName);

	int type() const {
		return ARCHIVE_TEXT;
	}

	bool isInput() const {
		return true;
	}

	bool isOutput() const {
		return false;
	}

	bool laterThan(int version) const {
		return true;
	}

	template<class T>
    XPrmIArchive& operator>>(T& t){
		return (*this) & t;
    }

	template<class T>
    XPrmIArchive& operator&(const ObjectWrapper<T>& t)
    {
		if(!openNode(t.name()))
			return *this;

        typedef typename WrapperTraits<T>::unwrapped_type U;
        using namespace SerializationHelpers;
		using SerializationHelpers::Identity;

        Select<IsPrimitive<U>,
            Identity<load_primitive_impl<U> >,
            Select<boost::is_pointer<U>, 
                Identity<load_pointer_impl<U> >,
                Select<boost::is_array<U>, 
                    Identity<load_array_impl<U> >,
                    Identity<load_non_primitive_impl<U> >
                >
            >
        >::type::invoke(*this, WrapperTraits<T>::unwrap (t.value()));

		closeNode(t.name());
		return *this;
	}

	// To simulate sub-blocks
	bool openBlock(const char* name, const char* nameAlt) {
		//if(openNode(name)){
		//	openBracket();
		//	return true;
		//}
		//return false;
		return true;
	}

	void closeBlock() {
		//closeBracket();
		//closeNode("");
	}

	XBuffer& buffer() {
		return buffer_;
	}

private:
	string fileName_;
	XBuffer buffer_;
	char replaced_symbol;
	int putTokenOffset_;
	vector<int> readingStarts_;

	/////////////////////////////////////
	const char* getToken();
	void releaseToken();
	void putToken();
	void skipValue();

	void passString(const char* value);
	bool loadString(string& value); // false if zero string should be loaded
	int line() const;

	bool openNode(const char* name) 
	{
		if(name){
			int pass = 0;
			for(;;){
				const char* str = getToken();
				string token = str ? str : "}"; // to simulate end of block when end of file
				if(!str)
					token = "}";
				releaseToken();
				if(token == name)
					break;
				if(token == "}"){
					if(pass++ == 2){
						putToken();
						return false;
					}
					else
						buffer_.set(readingStarts_.back());
				}
				else{
					passString("=");
					skipValue();
					passString(";");
				}
			}

			passString("=");
		}
		return true;
	}

	void closeNode(const char* name) 
	{
		if(name)
			passString(";");
	}

	void openBracket() {
		passString("{");
	}
	void closeBracket() {
		passString("}");
	}

	void openStructure() {
		openBracket();
		readingStarts_.push_back(buffer_.offset);
	}
	void closeStructure() {
		readingStarts_.pop_back();
		for(;;){
			string token;
			loadString(token);
			if(token == "}"){
				break;
			}
			else{
				passString("=");
				skipValue();
				passString(";");
			}
		}
	}

	int openCollection(){
		openBracket();
		int counter;
		buffer_ >= counter;
		passString(";");
		return counter;
	}
	template<class T>
	void loadElement(T& t) {
		(*this) & WRAP_NAME(t, 0);
		string name;
		loadString(name);
		if(name != ",")
			putToken();
	}

    template<class T>
    struct load_primitive_impl {
        static void invoke(XPrmIArchive & ar, T & t){
        	ar & t;
        }
    };

    template<class T>
    struct load_non_primitive_impl {
        static void invoke(XPrmIArchive& ar, T& t){
			if(!SuppressBracket<T>::value)
				ar.openStructure();
            t.serialize(ar);
			if(!SuppressBracket<T>::value)
				ar.closeStructure();
        }
    };

	template<class T>
    struct load_pointer_impl {
        static void invoke(XPrmIArchive& ar, T & t){
			ar.loadPointer(t);
		}
    };

	template<class T>
    struct load_array_impl {
        static void invoke(XPrmIArchive& ar, T & t){
			int count = sizeof(t) / (
				static_cast<const char *>(static_cast<const void *>(&t[1])) 
				- static_cast<const char *>(static_cast<const void *>(&t[0]))
			);
			int countOld = ar.openCollection();
			if(count < countOld){
				xassert(0 && "Array size too short");
				ErrH.Abort("Array size too short");
			}
			for(int i = 0; i < countOld; ++i)
				ar.loadElement(t[i]);
			ar.closeBracket();
		}
    };

    template<class T>
	XPrmIArchive& operator&(T& value){
		buffer_ >= value;
		return *this;
	}

	template<class T>
	void loadPointer(T*& t)
    {
		string typeName;
		loadString(typeName);
		if(typeName == "0"){
			if(t){
				delete t;
				t = 0;
			}
			return;
		}
		typedef ClassDescriptor<typename boost::remove_const<T>::type, XPrmOArchive, XPrmIArchive> Descriptor;
		if(t){
			if(typeName == get_type_id<T>().c_str()){
				Descriptor::instance().find(typeName.c_str()).load(*this, t);
				return;
			}
			else{
				delete t;
				t = 0;
			}
		}
		Descriptor::instance().find(typeName.c_str()).create(*this, t);
	}

	template<class T, class A>
	XPrmIArchive& operator&(vector<T, A>& cont)
	{
		int count = openCollection();
		if(count != cont.size()){
			cont.clear();
			cont.reserve(count);
			while(count--){
				cont.push_back(SerializationDefaultValue<T>::get());
				loadElement(cont.back());
			}
		}
		else{
			typename vector<T, A>::iterator i;
			FOR_EACH(cont, i)
				loadElement(*i);
		}
		closeBracket();
		return *this;
	}

	template<class T, class A>
	XPrmIArchive& operator&(list<T, A>& cont)
	{
		int count = openCollection();
		if(count != cont.size()){
			cont.clear();
			while(count--){
				cont.push_back(SerializationDefaultValue<T>::get());
				loadElement(cont.back());
			}
		}
		else{
            typename list<T, A>::iterator i;
			FOR_EACH(cont, i)
				loadElement(*i);
		}
		closeBracket();
		return *this;
	}

	template<class T1, class T2>
	XPrmIArchive& operator&(std::pair<T1, T2>& t)
	{
		openBracket();
		(*this) & WRAP_NAME(t.first, "first");
		(*this) & WRAP_NAME(t.second, "second");
		closeBracket();
		return *this;
	}

	template<class T>
	XPrmIArchive& operator&(ShareHandle<T>& t)
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
	XPrmIArchive& operator&(EnumWrapper<Enum>& t)
	{
		const EnumDescriptor<Enum>& descriptor = getEnumDescriptor(Enum(0));
		string str;
		loadString(str);
		t.value() = descriptor.keyByName(str.c_str());
		return *this;
	}

	template<class Enum, class Value>			 
	XPrmIArchive& operator&(BitVector<Enum, Value>& t)
	{
		const EnumDescriptor<Enum>& descriptor = getEnumDescriptor(Enum(0));
		t.value() = (Value)0;
		for(;;){
			string name;
			loadString(name);
			if(name == ";"){
				putToken();
				break;
			}
			else if(name == "|")
				continue;
			t.value() |= descriptor.keyByName(name.c_str());
		}
		return *this;
	}

	XPrmIArchive& operator&(PrmString& t)
	{
		string str;
		if(loadString(str))
			t = str;
		else
			t = 0;
		return *this;
	}

	XPrmIArchive& operator&(CustomString& t)
	{
		string str;
		loadString(str);
		t = str;
		return *this;
	}

	XPrmIArchive& operator&(ComboListString& t)
	{
		string str;
		loadString(str);
		t = str;
		return *this;
	}

	XPrmIArchive& operator&(string& value){
		loadString(value);
		return *this;
	}

	XPrmIArchive& operator&(bool& value){
		string str;
		loadString(str);
		if(str == "true")
			value = true;
		else if(str == "false")
			value = false;
		else 
			value = atoi(str.c_str());
		return *this;
	}
};

// Old data convertion
template<>
inline void convertT(XPrmIArchive& ar, Vect2i& v) {
	XBuffer& buf = ar.buffer();
	while(isspace(buf()))
		++buf;
	if(buf() == 'x'){
		--buf;
		ar & makeObjectWrapper(v.x, "x", "&x");
		ar & makeObjectWrapper(v.y, "y", "&y");
	}
	else{
		--buf;
		buf >= v;
	}
}

template<>
inline void convertT(XPrmIArchive& ar, Vect2f& v) {
	XBuffer& buf = ar.buffer();
	while(isspace(buf()))
		++buf;
	if(buf() == 'x'){
		--buf;
		ar & makeObjectWrapper(v.x, "x", "&x");
		ar & makeObjectWrapper(v.y, "y", "&y");
	}
	else{
		--buf;
		buf >= v;
	}
}

template<>
inline void convertT(XPrmIArchive& ar, Vect3f& v) {
	XBuffer& buf = ar.buffer();
	while(isspace(buf()))
		++buf;
	if(buf() == 'x'){
		--buf;
		ar & makeObjectWrapper(v.x, "x", "&x");
		ar & makeObjectWrapper(v.y, "y", "&y");
		ar & makeObjectWrapper(v.z, "z", "&z");
	}
	else{
		--buf;
		buf >= v;
	}
}

template<>
inline void convertT(XPrmIArchive& ar, QuatF& q) {
	XBuffer& buf = ar.buffer();
	while(isspace(buf()))
		++buf;
	if(buf() == 's'){
		--buf;
		ar & makeObjectWrapper(q.s(), "s", "&s");
		ar & makeObjectWrapper(q.x(), "x", "&x");
		ar & makeObjectWrapper(q.y(), "y", "&y");
		ar & makeObjectWrapper(q.z(), "z", "&z");
	}
	else{
		--buf;
		buf >= q;
	}
}

#endif //__XPRM_ARCHIVE_H__
