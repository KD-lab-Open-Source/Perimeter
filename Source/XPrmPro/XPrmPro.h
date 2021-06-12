#ifndef __XPRMPRO_H__
#define __XPRMPRO_H__

#include <Handle.h>

class ScriptParser;
class TreeNode;

#ifndef _FINAL_VERSION_
#define xassertStr(exp, str) { string s = #exp; s += "\n"; s += str; xxassert(exp,s.c_str()); }
#else
#define xassertStr(exp, str) 
#endif


template<class Enum>
class EnumWrapper
{
public:
	EnumWrapper() {}
	EnumWrapper(Enum value) : value_(value) {}
	operator Enum() const { return value_; }

private:
	Enum value_;
};

template<class T, int size>
class ArrayWrapper
{
public:
	//ArrayWrapper() { memset(array_, 0, sizeof(array_)); }
	ArrayWrapper() { for(int i = 0; i < size; i++) array_[i] = T(); }
	void set(const T& value) { for(int i = 0; i < size; i++) array_[i] = value; }
	
	operator T*() { return array_; }
	operator const T*() const { return array_; }
	
//	T& operator[](int i) { return array_[i]; }
//	const T& operator[](int i) const { return array_[i]; }

private:
	T array_[size];
};

template<class Enum>
class BitVector
{
public:
	BitVector(int value = 0) : value_(value) {}
	operator int() const { return value_; }

private:
	int value_;
};

class PrmString
{
public:
	PrmString(const char* value = 0) { (*this) = value; }
	PrmString& operator=(const char* value) { if(value) { value_ = value; zeroPointer_ = false; } else { value_ = ""; zeroPointer_ = true; } return *this; }
	PrmString& operator=(const string& value) { value_ = value; zeroPointer_ = false; return *this; }
	operator const char*() const { return !zeroPointer_ ? value_.c_str() : 0; }
	
private:
	string value_;
	bool zeroPointer_;
};

typedef const char* (*CustomValueFunc)(HWND hWndParent, const class TreeNode* treeNode);

class CustomString
{
public:
	CustomString(const char* value = 0, CustomValueFunc customValueFunc = 0) : customValueFunc_(customValueFunc) { (*this) = value; }
	CustomString& operator=(const char* value) { if(value) { value_ = value; zeroPointer_ = false; } else { value_ = ""; zeroPointer_ = true; } return *this; }
	CustomString& operator=(const string& value) { value_ = value; zeroPointer_ = false; return *this; }
	operator const char*() const { return !zeroPointer_ ? value_.c_str() : 0; }
	
	CustomValueFunc customValueFunc() const { return customValueFunc_; }
		
private:
	string value_;
	bool zeroPointer_;
	CustomValueFunc customValueFunc_;
};

template<class Enum>
class EnumDescriptor
{
public:
	EnumDescriptor(const char* typeName) : typeName_(typeName) {}

	const char* name(Enum key) const {
		KeyToName::const_iterator i = keyToName.find(key);
		if(i != keyToName.end())
			return i->second.c_str();
		xassert(0);
		return "";
	}

	const char* nameAlt(Enum key) const {
		KeyToName::const_iterator i = keyToNameAlt.find(key);
		if(i != keyToNameAlt.end())
			return i->second.c_str();
		xassert(0);
		return "";
	}

	Enum keyByName(const char* name) const {
		NameToKey::const_iterator i = nameToKey.find(name);
		if(i != nameToKey.end())
			return (Enum)i->second;
		xassertStr(!strlen(name) && "Unknown enum identificator will be ignored: ", (typeName_ + "::" + name).c_str());
		return (Enum)0;
	}

	Enum keyByNameAlt(const char* nameAlt) const {
		NameToKey::const_iterator i = nameAltToKey.find(nameAlt);
		if(i != nameAltToKey.end())
			return (Enum)i->second;
		xassertStr(!strlen(nameAlt) && "Unknown enum identificator will be ignored: ", (typeName_ + "::" + nameAlt).c_str());
		return (Enum)0;
	}

	string nameCombination(int bitVector) const {
		string value;
		KeyToName::const_iterator i;
		FOR_EACH(keyToName, i)
			if(bitVector & i->first){
				if(!value.empty())
					value += " | ";
				value += i->second;
			}
		return value;
	}

	string nameAltCombination(int bitVector) const {
		string value;
		KeyToName::const_iterator i;
		FOR_EACH(keyToNameAlt, i)
			if(bitVector & i->first){
				if(!value.empty())
					value += " | ";
				value += i->second;
			}
		return value;
	}


	const char* comboList() const { return comboList_.c_str(); }
	const char* comboListAlt() const { return comboListAlt_.c_str(); }

	const char* typeName() const { return typeName_.c_str(); }
	Enum defaultValue() const { xassert(!keyToName.empty()); return (Enum)keyToName.begin()->first; }

protected:
	void add(Enum key, const char* name, const char* nameAlt){
		keyToName[key] = name;
		keyToNameAlt[key] = nameAlt;
		nameToKey[name] = key;
		nameAltToKey[nameAlt] = key;
		if(!comboList_.empty()){
			comboList_ += "|";
			comboListAlt_ += "|";
		}
		comboList_ += name;
		comboListAlt_ += nameAlt;
	}

private:
	typedef hash_map<int, string> KeyToName;
	typedef hash_map<string, int> NameToKey;

	KeyToName keyToName;
	KeyToName keyToNameAlt;
	NameToKey nameToKey;
	NameToKey nameAltToKey;

	string comboList_;
	string comboListAlt_;

	string typeName_;
};


template<class Enum>
const char* getEnumName(const Enum& key)
{
	return getEnumDescriptor(Enum()).name(key);
}

template<class Enum>
const char* getEnumNameAlt(const Enum& key)
{
	return getEnumDescriptor(Enum()).nameAlt(key);
}

#endif //__XPRMPRO_H__