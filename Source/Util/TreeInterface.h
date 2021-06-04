#ifndef __TREE_INTERFACE_H__
#define __TREE_INTERFACE_H__

#include "Handle.h"
#include "XUtil.h"

#ifdef _USRDLL
#define DLL_API __declspec(dllexport)
#else //_USRDLL
#define DLL_API __declspec(dllimport)
#endif//_USRDLL

#ifndef FOR_EACH
#define FOR_EACH(list, iterator) \
for(iterator = (list).begin(); iterator != (list).end(); ++iterator)
#endif

#include <vector>
#include <list>
#include <string>

class TreeNode;
typedef ShareHandle<TreeNode> TreeNodePtr;

class TreeNode : public ShareHandleBase
{
public:
	enum EditType {
		STATIC,		// Просто текст. Редактировать нельзя
		EDIT,		// Просто текст. Редактировать можно
		COMBO,		// Одно из значений из comboList(), разделенных |
		COMBO_MULTI, // Несколько значений из comboList(), также разделенных |, может быть пусто
		VECTOR,		 // value не меняется, но можно удалять и 
					//	добавлять в список, инициализируя значением defaultTreeNode()
		POLYMORPHIC, // Инициализировать defaultTreeNode(value from comboList())
		CUSTOM,		// Вместо обычного редактирования вызывается customValue() для получения значения
	};

	typedef const TreeNode* (*TreeNodeFunc)(int typeIndex);
	typedef const char* (*CustomValueFunc)(HWND hWndParent, const char* initialString);

	typedef std::list<TreeNodePtr> List;
	typedef List::iterator iterator;
	typedef List::const_iterator const_iterator;
	typedef std::string string;

	//-----------------------------------------------------
	TreeNode():
	parent_(0)
	,customValueFunc_(0)
	,defaultTreeNodeFunc_(0)
	,defaultTreeNode_(0)
	,editType_(STATIC)
	,hidden_(false) {}

	explicit TreeNode(const char* name) 
		: name_(name), value_(""), type_(""), editType_(STATIC),  
		defaultTreeNode_(0), defaultTreeNodeFunc_(0), customValueFunc_(0),
		parent_(0), expanded_(false), color_(0), hidden_(false) {}

	TreeNode& operator=(const TreeNode& node)
	{
		editType_ = node.editType();
		name_ = node.name();
		value_ = node.value();
		comboList_ = node.comboList();
		type_ = node.type();
		defaultTreeNode_ = node.defaultTreeNode_;
		defaultTreeNodeFunc_ = node.defaultTreeNodeFunc_;
		customValueFunc_ = node.customValueFunc_;
		expanded_ = node.expanded();
		hidden_ = node.hidden();
		color_ = node.color();

		children_.clear();
		List::const_iterator i = node.children().begin(), e = node.children().end();
		for(; i != e; ++i)
		{
			TreeNodePtr ptr(new TreeNode());
			push_back(ptr);
			*(ptr.get()) = *((*i).get());
		}
		return *this;
	}

	//-----------------------------------------------------
	const List& children() const { return children_; }

	void push_back(TreeNodePtr const& treeNode) { 
		children_.push_back(treeNode); 
		treeNode->setParent(this); 
	}
	void insert(iterator before, TreeNodePtr const& ptr)
	{
		children_.insert(before, ptr);
		ptr->setParent(this);
	}

	void erase(iterator i){
		(*i)->setParent(NULL);
		children_.erase(i);
	}

	iterator begin(){
		return children_.begin();
	}
	iterator end(){
		return children_.end();
	}
	const_iterator begin() const{
		return children_.begin();
	}
	const_iterator end() const{
		return children_.end();
	}
	const TreeNodePtr front() const {
		return children_.front();
	}
	const TreeNodePtr back() const {
		return children_.back();
	}
	bool empty() const{
		return children_.empty();
	}
	size_t size() const{
		return children_.size();
	}
	void clear() {
		iterator i = begin(), e = end();
		for(; i != e; ++i)
			(*i)->setParent(NULL);
		children_.clear();
	}

	const TreeNode* parent() const { return parent_; }
	void setParent(const TreeNode* parent) { parent_ = parent; }

	//-----------------------------------------------------
	const string& name() const { return name_; }
	void setName(const char* name) { name_ = name; }

	const string& value() const { return value_; }
	void setValue(const char* value) { value_ = value; }

	const std::string& type() const { return type_; } // Для проверки при копировании
	void setType(const char* type) { type_ = type; }

	EditType editType() const { return editType_; }
	void setEditType(EditType editType) { editType_ = editType; }

	const char* comboList() const { return comboList_.c_str(); }
	void setComboList(EditType editType, const char* comboList) 
	{
		editType_ = editType; 
		comboList_ = comboList; 
	}

	void setTreeNode(TreeNode* treeNode)
	{
		editType_ = VECTOR;
		defaultTreeNode_ = treeNode;
	}

	void setTreeNodeFunc(TreeNodeFunc func)
	{
		editType_ = POLYMORPHIC;
		defaultTreeNodeFunc_ = func;
	}

	void setCustomValueFunc(CustomValueFunc func)
	{
		editType_ = CUSTOM;
		customValueFunc_ = func;
	}

	void setHidden() { hidden_ = true; }
	bool hidden() const { return hidden_; }
    	
	const TreeNode* defaultTreeNode() const { return defaultTreeNode_; }
	const TreeNode* defaultTreeNode(int typeIndex) const { 
		return defaultTreeNodeFunc_(typeIndex); 
	}
    
	const char* customValue(HWND hWndParent) const { 
		return customValueFunc_ ? customValueFunc_(hWndParent, value().c_str()) : 0; 
	}

	string itemName() const { 
		string postfix;
		const_iterator i;
		FOR_EACH(children(), i)
			if(!(*i)->name().empty() && (*i)->name()[0] == '&'){
				if(!(*i)->value().empty()){
					if(!postfix.empty())
						postfix += ", ";
					postfix += (*i)->value();
				}
			}

		if(name().empty())
			return postfix;

		string result = name()[0] != '&' ? name().c_str() : name().c_str() + 1;
		if(postfix.empty())
			return result;
		return result + ": " + postfix;
	}

	XBuffer valueBuffer() const { return XBuffer((void*)value_.c_str(), value_.size()); }

	//-----------------------------------------------------
	bool expanded() const { return expanded_; }
	void setExpanded(bool expanded) { expanded_ = expanded;	}
	
	unsigned int color() const { return color_;	}
	void setColor(unsigned int color) { color_ = color;	}

	TreeNode* findChild(const string& childName, const string& childType, const string& parentType) {
		if(name() == childName && type() == childType && parent() && parent()->type() == parentType)
			return this;
		iterator i;
		FOR_EACH(children_, i){
			TreeNode* node = (*i)->findChild(childName, childType, parentType);
			if(node)
				return node;
		}
		return 0;
	}

	//-----------------------------------------------------
	static bool isExtensible(EditType type){ return type == VECTOR; }
	
private:
	List children_;
	const TreeNode* parent_;
	EditType editType_;
	std::string name_;
	std::string value_;
	std::string type_;
	std::string comboList_;
	const TreeNode* defaultTreeNode_;
	TreeNodeFunc defaultTreeNodeFunc_;
	CustomValueFunc customValueFunc_;
	bool expanded_;
	bool hidden_;
	unsigned int color_;
};


struct TreeControlSetup
{
	RECT window;
	bool expandAllNodes_;

	TreeControlSetup(int left, int top, int right, int bottom, const char* configName, bool expandAllNodes = false) {
		expandAllNodes_ = expandAllNodes;
		window.left = left; 
		window.top = top; 
		window.right = right; 
		window.bottom = bottom; 
		configName_ = configName;
		assert(configName_);
	}

	~TreeControlSetup() {
	}

	char const* getConfigName() const {
		return configName_;
	}

private:
	const char* configName_;
};


#ifndef _FINAL_VERSION_

// Копирует дерево, возвращает указатель на статические данные, либо 0.
extern "C" DLL_API TreeNode const* treeControlEdit(const TreeNode* treeNode, 
										 HWND hwnd, TreeControlSetup& treeControlSetup);
// Очищает статические данные
extern "C" DLL_API void treeControlFree(TreeNode const* );

#ifndef _USRDLL
#pragma comment(lib,"ScriptEditor.lib")
#endif//_USERDLL

#else // _FINAL_VERSION_

inline TreeNode const* treeControlEdit(const TreeNode* treeNode, HWND hwnd, TreeControlSetup& treeControlSetup){ return 0; }
inline void treeControlFree(TreeNode const* ){}

#endif // _FINAL_VERSION_

#endif //__TREE_INTERFACE_H__