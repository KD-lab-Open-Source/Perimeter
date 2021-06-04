#ifndef __EDIT_ARCHIVE_H__
#define __EDIT_ARCHIVE_H__
#pragma once

#include "Serialization.h"
#include "TreeInterface.h"
#include "SerializationImpl.h"

class EditOArchive 
{
public:
	EditOArchive();
	~EditOArchive();

	void setTranslatedOnly(bool translatedOnly) {
		translatedOnly_ = translatedOnly;
	}

	const TreeNode* rootNode() const { 
		xassert(!nesting_);
		return rootNode_;
	}

	int type() const {
		return ARCHIVE_EDIT;
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
    EditOArchive & operator<<(const T& t){
        return (*this) & t;
    }

    template<class T>
    EditOArchive& operator&(const ObjectWrapper<T> & t)
    {
		openNode(t.name(), t.nameAlt());

		setType<T>();

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
        >::type::invoke(*this, WrapperTraits<T>::unwrap(t.value()));
        
		closeNode(t.name());
		return *this;
    }

	// To simulate sub-blocks
	bool openBlock(const char* name, const char* nameAlt) {
		openNode(name, nameAlt);
		return true;
	}

	void closeBlock() {
		closeNode("");
	}

private:
	ShareHandle<TreeNode> rootNode_;
	TreeNode* currentNode_;
	bool translatedOnly_;
	int nesting_;
	static int typeIDs_;

    ////////////////////////////////////////
	void openNode(const char* name, const char* nameAlt) 
	{
		++nesting_;
		if(name){
			if(nameAlt)
				name = nameAlt;
			if(*name == '_')
				++name;
			TreeNode* node = new TreeNode(name);       
			if((!nameAlt || *nameAlt == '_') && translatedOnly_)
				node->setHidden();
			currentNode_->push_back(node);
			currentNode_ = node;
		}
	}

	void closeNode(const char* name) 
	{
		--nesting_;
		if(name && currentNode_->parent())
			currentNode_ = const_cast<TreeNode*>(currentNode_->parent());
	}

	template<class T>
	void setType(const char* prefix = ""){
		static int id = ++typeIDs_;
		XBuffer buf;
		buf < prefix <= id;
		currentNode_->setType(buf);
	}

	template<class T>
	void setDefaultTreeNode(TreeNode::EditType editType) {
    	static bool inited;
		static TreeNodePtr treeNode_;
		if(!inited){
			inited = true;
			T t;
			EditOArchive archive;
			archive << WRAP_NAME(t, 0);
			treeNode_ = const_cast<TreeNode*>(archive.rootNode());
		}
		if(treeNode_){
			currentNode_->setEditType(editType);
			currentNode_->setTreeNode(treeNode_);
		}
		else
			currentNode_->setEditType(TreeNode::STATIC);
	}

	template<class T>
    struct save_primitive_impl {
        static void invoke(EditOArchive & ar, const T & t){
        	ar & t;
        }
    };

    template<class T>
    struct save_non_primitive_impl {
        static void invoke(EditOArchive & ar, const T& t){
            const_cast<T&>(t).serialize(ar);
        }
    };

	template<class T>
    struct save_pointer_impl {
        static void invoke(EditOArchive& ar, const T & t){
			ar.savePointer(t);
		}
    };

	template<class T>
    struct save_array_impl {
        static void invoke(EditOArchive& ar, const T & t){
			int count = sizeof(t) / (
				static_cast<const char *>(static_cast<const void *>(&t[1])) 
				- static_cast<const char *>(static_cast<const void *>(&t[0]))
			);
			ar.setType<T>();
//			ar.setDefaultTreeNode<T>(TreeNode::STATIC);
			for(int i = 0; i < count; ++i){
				XBuffer buf;
				buf < "[" <= i < "]";
				ar & TRANSLATE_NAME(t[i], buf, buf);
			}
		}
    };

	template<class T>
    EditOArchive& operator&(const T& value)
    {
        XBuffer buf; buf <= value; 
		currentNode_->setValue(buf);
		currentNode_->setEditType(TreeNode::EDIT);
		return *this;
	}

	template<class T>
	void savePointer(const T* t)
    {
		currentNode_->setComboList(TreeNode::POLYMORPHIC, EditClassDescriptor<T>::instance().comboListAlt().c_str());
		typedef EditClassDescriptor<remove_const<T>::type> Descriptor;
		currentNode_->setTreeNodeFunc(Descriptor::instance().treeNodeFunc);
		if(!t)
			return;
		const char* name = typeid(*t).name();
		Descriptor::SerializerBase& serializer = Descriptor::instance().find(name);
		serializer.save(*this, t);		
		currentNode_->setValue(serializer.nameAlt());
		//node->setType((node->type() + "*").c_str());
	}

	template<class T, class A>
	EditOArchive& operator&(const vector<T, A>& cont)
	{
		setType<T>();
		setDefaultTreeNode<T>(TreeNode::STATIC);
		int i = 0;
		vector<T, A>::const_iterator it;
		FOR_EACH(cont, it){
			XBuffer buf;
			buf < "[" <= i++ < "]";
			(*this) & TRANSLATE_NAME(*it, buf, buf);
		}
		return *this;
	}

	template<class T, class A>
	EditOArchive& operator&(const list<T, A>& cont)
	{
		setType<T>();
		setDefaultTreeNode<T>(TreeNode::STATIC);
		int i = 0;
		list<T, A>::const_iterator it;
		FOR_EACH(cont, it){
			XBuffer buf;
			buf < "[" <= i++ < "]";
			(*this) & TRANSLATE_NAME(*it, buf, buf);
		}
		return *this;
	}

	template<class T1, class T2>
	EditOArchive& operator&(const std::pair<T1, T2>& t)
	{
		(*this) & TRANSLATE_NAME(t.first, "first", "&Ключ");
		(*this) & TRANSLATE_NAME(t.second, "second", "Значение");

		string name = currentNode_->name();
		TreeNode::const_iterator i;
		FOR_EACH(currentNode_->front()->children(), i)
			if(!(*i)->name().empty() && (*i)->name()[0] == '&'){
				if(!(*i)->value().empty()){
					if(!name.empty())
						name += ", ";
					name += (*i)->value();
				}
			}
		currentNode_->setName(name.c_str());

		return *this;
	}
    
	template<class T>
	EditOArchive& operator&(const ShareHandle<T>& t)
	{
		return *this & WRAP_NAME(t.get(), 0);
	}

    template<class Enum>
	EditOArchive& operator&(const EnumWrapper<Enum>& t)
    {
		const EnumDescriptor<Enum>& descriptor = getEnumDescriptor(Enum(0));
		currentNode_->setValue(descriptor.nameAlt(t.value()));
		currentNode_->setType(descriptor.typeName());
		currentNode_->setComboList(TreeNode::COMBO, descriptor.comboListAlt());
		return *this;
	}

    template<class Enum, class Value>
	EditOArchive& operator&(const BitVector<Enum, Value>& t)
    {
		const EnumDescriptor<Enum>& descriptor = getEnumDescriptor(Enum(0));
		currentNode_->setValue(descriptor.nameAltCombination(t.value()).c_str());
		XBuffer buf;
		buf < "bv of " < descriptor.typeName();
		currentNode_->setType(buf);
		currentNode_->setComboList(TreeNode::COMBO_MULTI, descriptor.comboListAlt());
		return *this;
	}

	EditOArchive& operator&(const PrmString& t)
    {
		currentNode_->setValue(t ? t : "\\0");
		currentNode_->setType("PrmString");
		currentNode_->setEditType(TreeNode::EDIT);
		return *this;
	}

	EditOArchive& operator&(const CustomString& t)
    {
		currentNode_->setValue(t);
		currentNode_->setType("CustomString");
		currentNode_->setCustomValueFunc(t.customValueFunc());
		return *this;
	}

	EditOArchive& operator&(const ComboListString& t)
    {
		currentNode_->setValue(t);
		currentNode_->setType("ComboListString");
		currentNode_->setComboList(TreeNode::COMBO, t.comboList());
		return *this;
	}

	EditOArchive& operator&(const string& value)
    {
        currentNode_->setValue(value.c_str());
		currentNode_->setEditType(TreeNode::EDIT);
		return *this;
	}

	EditOArchive& operator&(const bool& value)
    {
		currentNode_->setValue(value ? "Да" : "Нет");
		currentNode_->setComboList(TreeNode::COMBO, "Да|Нет");
		return *this;
	}
};


class EditIArchive 
{
public:
	EditIArchive();
	EditIArchive(const TreeNode* node);
	EditIArchive(const EditOArchive& eoa);
	~EditIArchive();

	void setRootNode(const TreeNode* node){
		currentNode_ = parentNode_ = node;
	}

	void setTranslatedOnly(bool translatedOnly) {
		translatedOnly_ = translatedOnly;
	}

	int type() const {
		return ARCHIVE_EDIT;
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
    EditIArchive& operator>>(T & t){
        return (*this) & t;
    }

    template<class T>
    EditIArchive& operator&(const ObjectWrapper<T>& t)
    {				   
		openNode(t.name());

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
		>::type::invoke(*this, WrapperTraits<T>::unwrap(t.value()));
		return *this;
	}

	bool openBlock(const char* name, const char* nameAlt) {
		openNode(name);
		return true;
	}

	void closeBlock() {
	}

private:
	const TreeNode* parentNode_;
	const TreeNode* currentNode_;
	bool translatedOnly_;

	/////////////////////////////////////
	void openNode(const char* name) 
	{
		if(name){
			if(!currentNode_->empty()){
				parentNode_ = currentNode_;
				currentNode_ = currentNode_->front();
			}
			else{
				while(currentNode_ != parentNode_){
					TreeNode::const_iterator i = find(parentNode_->begin(), parentNode_->end(), currentNode_);
					if(++i != parentNode_->end()){
						currentNode_ = *i;
						break;
					}
					else{
						currentNode_ = parentNode_;
						parentNode_ = parentNode_->parent();
					}
				}
			}
		}
	}

    template<class T>
    struct load_primitive_impl {
        static void invoke(EditIArchive & ar, T & t){
        	ar & t;
        }
    };

    template<class T>
    struct load_non_primitive_impl {
        static void invoke(EditIArchive & ar, T & t){
            t.serialize(ar);
        }
    };

	template<class T>
    struct load_pointer_impl {
        static void invoke(EditIArchive& ar, T & t){
			ar.loadPointer(t);
		}
    };

	template<class T>
    struct load_array_impl {
        static void invoke(EditIArchive& ar, T & t){
			int count = sizeof(t) / (
				static_cast<const char *>(static_cast<const void *>(&t[1])) 
				- static_cast<const char *>(static_cast<const void *>(&t[0]))
			);
			for(int i = 0; i < count; ++i)
				ar & TRANSLATE_NAME(t[i], "", "");
		}
    };

	template<class T>
	EditIArchive& operator&(T& value){
		currentNode_->valueBuffer() >= value;
		return *this;
	}

	template<class T>
	void loadPointer(T*& t)
    {
		const string& typeName = currentNode_->value();
		if(typeName.empty()){
			if(t){
				delete t;
				t = 0;
			}
			return;
		}
		typedef EditClassDescriptor<remove_const<T>::type> Descriptor;
		if(t){
			if(typeName == Descriptor::instance().nameAlt(*t)){
				Descriptor::instance().findAlt(typeName.c_str()).load(*this, t);
				return;
			}
			else{
				delete t;
				t = 0;
			}
		}
		Descriptor::instance().findAlt(typeName.c_str()).create(*this, t);
	}

	template<class T, class A>
	EditIArchive& operator&(vector<T, A>& cont)
    {
		int count = currentNode_->size();
		if(count != cont.size()){
			cont.clear();
			cont.reserve(count);
			while(count--){
				cont.push_back(SerializationDefaultValue<T>::get());
				(*this) & TRANSLATE_NAME(cont.back(), "", "");
			}
		}
		else{
			vector<T, A>::iterator i;
			FOR_EACH(cont, i)
				(*this) & TRANSLATE_NAME(*i, "", "");
		}
		return *this;
    }

	template<class T, class A>
	EditIArchive& operator&(list<T, A>& cont)
    {
		int count = currentNode_->size();
		if(count != cont.size()){
			cont.clear();
			while(count--){
				cont.push_back(SerializationDefaultValue<T>::get());
				(*this) & TRANSLATE_NAME(cont.back(), "", "");
			}
		} 
		else{
			list<T, A>::iterator i;
			FOR_EACH(cont, i)
				(*this) & TRANSLATE_NAME(*i, "", "");
		}
		return *this;
    }

	template<class T1, class T2>
	EditIArchive& operator&(std::pair<T1, T2>& t)
	{
		(*this) & TRANSLATE_NAME(t.first, "first", "Ключ");
		(*this) & TRANSLATE_NAME(t.second, "second", "Значение");
		return *this;
	}

	template<class T>
	EditIArchive& operator&(ShareHandle<T>& t)
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
	EditIArchive& operator&(EnumWrapper<Enum>& t)
	{
		const EnumDescriptor<Enum>& descriptor = getEnumDescriptor(Enum(0));
		const char* str = currentNode_->value().c_str();
		t.value() = descriptor.keyByNameAlt(currentNode_->value().c_str());
		return *this;
	}

	template<class Enum, class Value>
	EditIArchive& operator&(BitVector<Enum, Value>& t)
	{
		const EnumDescriptor<Enum>& descriptor = getEnumDescriptor(Enum(0));

		XBuffer valueBuffer = currentNode_->valueBuffer();
		
		t.value() = (Value)0;
		for(;;){
			string name = getEnumToken(valueBuffer);
			if(name == "")
				break;
			t.value() |= descriptor.keyByNameAlt(name.c_str());
		}
		return *this;
	}

	EditIArchive& operator&(PrmString& t)
	{
		if(currentNode_->value() != "\\0")
			t = currentNode_->value();
		else
			t = 0;
		return *this;
	}

	EditIArchive& operator&(CustomString& t)
	{
		t = currentNode_->value();
		return *this;
	}

	EditIArchive& operator&(ComboListString& t)
	{
		t = currentNode_->value();
		return *this;
	}

	EditIArchive& operator&(string& value){
		value = currentNode_->value();
		return *this;
	}

	EditIArchive& operator&(bool& value){
		value = currentNode_->value() == "Да";
		return *this;
	}
};


class EditArchive : public EditOArchive, public EditIArchive
{
public:
	EditArchive();
	EditArchive(HWND hwnd, const TreeControlSetup& treeControlSetup);
	~EditArchive();

	void setTranslatedOnly(bool translatedOnly);
    
	bool edit();
	void clear();

	template<class T>
	bool edit(T& t, const char* name = 0) { 
    	static_cast<EditOArchive&>(*this) << WRAP_NAME(t, 0);
		if(name) // если редактируется указатель, то имя конфликтует с его типом
			const_cast<TreeNode*>(rootNode())->setValue(name);
		if(edit()){
			static_cast<EditIArchive&>(*this) >> WRAP_NAME(t, 0);
			clear();
			return true;
		}
		return false;
	}

private:
	const TreeNode* outputNode_;

	HWND hwnd_;
	TreeControlSetup treeControlSetup_;
};

template<class Base>
class EditClassDescriptor : public ClassDescriptor<Base, EditOArchive, EditIArchive>
{
public:
	template<class Derived>
	struct EditSerializer : ClassDescriptor<Base, EditOArchive, EditIArchive>::Serializer<Derived>
	{
		EditSerializer(const char* nameAlt) {
			nameAlt_ = nameAlt;
			instance().add(*this, typeid(Derived).name(), nameAlt);
		}

		const TreeNode* treeNode() {
			if(!treeNode_){ 
				Derived* t = static_cast<Derived*>(ObjectCreator<Base, Derived>::create());
				EditOArchive archive;
				archive << WRAP_NAME(*t, 0);
				treeNode_ = const_cast<TreeNode*>(archive.rootNode());
				treeNode_->setValue(nameAlt_);
				delete t;
			}
			return treeNode_;
		}

		const char* nameAlt() const { 
			return nameAlt_; 
		}

	private:
		TreeNodePtr treeNode_;
		const char* nameAlt_;
	};

	void add(SerializerBase& serializer, const char* name, const char* nameAlt) {
		ClassDescriptor<Base, EditOArchive, EditIArchive>::add(serializer, name, nameAlt);
		mapAlt_[nameAlt] = &serializer;
		mapNameToNameAlt_[name] = nameAlt;
		if(!comboListAlt().empty())
			comboListAlt() += "|";
		comboListAlt() += nameAlt;
	}

	const EditClassDescriptor& operator=(const EditClassDescriptor& rhs) {
		map_ = rhs.map_;
		mapAlt_ = rhs.mapAlt_;
		mapNameToNameAlt_ = rhs.mapNameToNameAlt_;
		comboListAlt().clear();
		Map::iterator i;
		FOR_EACH(mapAlt_, i){
			if(!comboListAlt().empty())
				comboListAlt() += "|";
			comboListAlt() += i->first;
		}
		return *this;
	}

	SerializerBase& findAlt(const char* nameAlt) {
		Map::iterator i = mapAlt_.find(nameAlt);
		if(i == mapAlt_.end()){
			xassertStr(0 && "Unregistered class", nameAlt);
			ErrH.Abort("Unregistered class", XERR_USER, 0, nameAlt);
		}
		return *i->second;
	}

	template<class T>
	const char* nameAlt(const T& t) const {
		const char* name = typeid(t).name();
		map<string, string>::const_iterator i = mapNameToNameAlt_.find(name);
		if(i == mapNameToNameAlt_.end()){
			xassertStr(0 && "Unregistered class", name);
			ErrH.Abort("Unregistered class", XERR_USER, 0, name);
		}
		return i->second.c_str();
	}

	static string& comboListAlt() {
		static string comboListAlt_;
		return comboListAlt_;
	}

	static const TreeNode* treeNodeFunc(int index) {
		const char* start = comboListAlt().c_str();
		while(index--){
			while(*++start != '|');
			++start;
		}

		const char* end = start;
		while(*++end != 0 && *end != '|');

		string name(start, end);
		return instance().findAlt(name.c_str()).treeNode();
	}

	static EditClassDescriptor& instance() {
		return Singleton<EditClassDescriptor>::instance();
	}

private:
	Map mapAlt_;
	map<string, string> mapNameToNameAlt_;
};

inline string getEnumToken(XBuffer& buffer)
{
	while(buffer() == ' ' || buffer() == '|')
		++buffer;
	const char* marker = &buffer();
	while(buffer() != '|' && buffer() != 0)
		++buffer;
	string str(marker, &buffer() - marker);
	while(str.size() && str[str.size() - 1] == ' ')
		str.pop_back();
	return str;
}

#endif //__EDIT_ARCHIVE_H__
