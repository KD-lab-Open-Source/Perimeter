//////////////////////////////////////////////
//		Библиотека типов 
//////////////////////////////////////////////
#ifndef __TYPE_LIBRARY_H__
#define __TYPE_LIBRARY_H__

#include <map>
#include "Serialization.h"
#include "SerializationMacro.h"

// string adaptor
inline const std::string key2String(const std::string& data) {
	return data;
}

inline void setKeyC(std::string& data, const char* str) {
	data = str;
}

// EnumWrapper adaptor
template<class Enum>
inline const std::string key2String(const EnumWrapper<Enum>& data) {
	return getEnumDescriptor(Enum(0)).nameAlt(data.value());
}

template<class Enum>
inline void setKeyC(EnumWrapper<Enum>& data, const char* str) {
	data.value() = getEnumDescriptor(Enum(0)).keyByNameAlt(str);
}


template<class Key, class Type, bool enableSelfReference = false>
class TypeLibrary
{
public:
	typedef std::map<Key, ShareHandle<Type> > Map;

	class Reference 
	{
	public:
		Reference(const Type* type = 0) : type_(type) {}
		
		~Reference() {
			if(type_ && !type_->decrRef()) 
				delete type_; 
		}

		void setKey(const Key& key) {
			key_ = key;
			if(!enableSelfReference)
				set(instance().find(key_)); 
		}

		void set(const Type* type) { 
			if(type_ && !type_->decrRef() && type_ != type) 
				delete type_;
			type_ = type; 
			if(type_) 
				type_->addRef(); 
		}

		const Type* get() const {
			return enableSelfReference ? instance().find(key_) : type_;
		}
		
		const Type* operator->() const { return get(); }
		const Type& operator*() const { return *get(); }
		operator const Type* () const { return get(); }

        SERIALIZE(ar) {
			if (ar.type() & ARCHIVE_EDIT) {
				ComboListString comboStr(instance().comboList(), key2String(key_).c_str());
				ar & TRANSLATE_NAME(comboStr, 0, 0);
				if(ar.isInput()) {
                    setKeyC(key_, comboStr);
                }
			} else {
                ar & WRAP_NAME(key_, !SuppressBracket<Reference>::value ? "key" : 0);
            }

			if(ar.isInput()) {
                setKey(key_);
            }
        }

	private:
		Key key_;
		const Type* type_;
        //ComboListString comboStr_;
	};

	const Type* find(const Key& key) {
		typename Map::const_iterator i = map_.find(key);
		return i != map_.end() ? i->second() : nullptr;
	}

    SERIALIZE(ar) {
		typedef std::list<typename Map::value_type> List;
		List tmpStorage;
		if (ar.isOutput()) {
			for (auto& i : map_) {
                tmpStorage.push_back(i);
            }
		} else {
            map_.clear();
        }

        ar & TRANSLATE_NAME(tmpStorage, "types", "Список");
		
		if (ar.isInput()) {
			comboList_.clear();
			for (auto& i : tmpStorage) {
				map_.insert(i);
				if (!comboList_.empty()) {
                    comboList_ += "|";
                }
				comboList_ += key2String(i.first).c_str();
			}
		}
	}

	void add(const Key& key, ShareHandle<Type> type) {
		map_.insert_or_assign(key, type);
	}

    void clear() {
        map_.clear();
    }

	Map& map() {
		return map_;
	}

	const char* comboList() const {
		return comboList_.c_str();
	}

    typedef typename Map::iterator iterator;
    iterator begin() noexcept { return map_.begin(); }
    iterator end() noexcept { return map_.end(); }

	static TypeLibrary& instance() {
		return SingletonPrm<TypeLibrary>::instance();
	}

private:
	Map map_;
	std::string comboList_;
};

//////////////////////////////////////////////////////////////
// editName решает две задачи - специализация StringTable
// (разные таблици должны быть разного типа) и ввод имени для
// редактирования
//////////////////////////////////////////////////////////////
template<const char editName[]>
class StringTable
{
public:
	typedef std::vector<std::string> Strings;

	class Reference 
	{
	public:
		Reference() {
			key_ = 0;
		}

		int key() const {
			return key_;
		}

		void setKey(int key) { 
			key_ = key;
		}

		operator const char*() const { 
			return instance().find(*this); 
		}

		bool operator==(const Reference& rhs) const {
			return key_ == rhs.key_;
		}

		bool operator<(const Reference& rhs) const {
			return key_ < rhs.key_;
		}

        SERIALIZE(ar) {
			ComboListString comboStr(instance().comboList_.c_str(), (const char*)(*this));
			ar & TRANSLATE_NAME(comboStr, nullptr, nullptr);
			if (ar.isInput()) {
                *this = instance().find(comboStr);
            }
		}

	private:
		Reference(int key) : key_(key) {}

		int key_;

		friend StringTable;
	};

	const char* find(const Reference& reference) {
		xassert(reference.key_ < strings_.size());
		return strings_[reference.key_].c_str();
	}

	const Reference find(const char* name) {
		Strings::iterator i;		
		FOR_EACH(strings_, i)
			if(*i == name)
				return Reference(i - strings_.begin());
		strings_.push_back(name);
		if(!comboList_.empty())
			comboList_ += "|";
		comboList_ += name;
		return Reference(strings_.size() - 1);
	}

    SERIALIZE(ar) {
		ar & TRANSLATE_NAME(strings_, "strings", editName);
		if (ar.isInput()) {
			comboList_ = "";
			for (auto& i : strings_) {
				if (!comboList_.empty()) {
                    comboList_ += "|";
                }
				comboList_ += i;
			}
		}
	}

	const Strings& strings() const {
		return strings_;
	}

	static StringTable& instance() {
		return SingletonPrm<StringTable>::instance();
	}

private:
	Strings strings_;
	std::string comboList_;
};

#endif //__TYPE_LIBRARY_H__
