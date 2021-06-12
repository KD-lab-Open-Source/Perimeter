
//////////////////////////////////////////////////////////////////////////////////////////////
//	XScript declaration
//	Section: XRealPrm
//	Number of variables: 14
//	This is computer generated code, don't try to change it
//////////////////////////////////////////////////////////////////////////////////////////////
#ifndef _XPRM_SECTION_XRealPrm
#define _XPRM_SECTION_XRealPrm

#include "XPrmPro.h"

enum MMode {
	MMODE_1 = 1,
	MMODE_2
};

struct AAAA {
	int internalColor_;
	enum Mode {
		MODE_1 = 1,
		MODE_2
	};
	
	
	AAAA();
	TreeNode* edit(const char* treeNodeName) const;
	void apply(const TreeNode* treeNode);
	void save(ScriptParser& parser) const;
	void load(ScriptParser& parser);
};

struct Condition : ShareHandleBase {
	int internalColor_;
	bool state_;
	
	Condition();
	virtual ~Condition() {}
	virtual TreeNode* edit(const char* treeNodeName) const;
	virtual void apply(const TreeNode* treeNode);
	virtual void save(ScriptParser& parser) const;
	virtual void load(ScriptParser& parser);
};

struct ConditionNode {
	enum Type {
		NORMAL,
		INVERTED
	};
	
	EnumWrapper<Type> type;
	ShareHandle<Condition> condition;
	
	ConditionNode();
	TreeNode* edit(const char* treeNodeName) const;
	void apply(const TreeNode* treeNode);
	void save(ScriptParser& parser) const;
	void load(ScriptParser& parser);
};

struct ConditionSwitcher : Condition {
	enum Type {
		AND,
		OR
	};
	
	EnumWrapper<Type> type;
	vector<ConditionNode> conditions;
	
	ConditionSwitcher();
	TreeNode* edit(const char* treeNodeName) const;
	void apply(const TreeNode* treeNode);
	void save(ScriptParser& parser) const;
	void load(ScriptParser& parser);
};

struct ConditionAnd : Condition {
	vector<ShareHandle<Condition> > conditions;
	
	ConditionAnd();
	TreeNode* edit(const char* treeNodeName) const;
	void apply(const TreeNode* treeNode);
	void save(ScriptParser& parser) const;
	void load(ScriptParser& parser);
};

struct ConditionOr : Condition {
	vector<ShareHandle<Condition> > conditions;
	
	ConditionOr();
	TreeNode* edit(const char* treeNodeName) const;
	void apply(const TreeNode* treeNode);
	void save(ScriptParser& parser) const;
	void load(ScriptParser& parser);
};

struct ConditionNot : Condition {
	ShareHandle<Condition> condition;
	
	ConditionNot();
	TreeNode* edit(const char* treeNodeName) const;
	void apply(const TreeNode* treeNode);
	void save(ScriptParser& parser) const;
	void load(ScriptParser& parser);
};

struct Node : ShareHandleBase {
	enum terBelligerent {
		BELLIGERENT_EXODUS,
		BELLIGERENT_HARKBACKHOOD0,
		BELLIGERENT_HARKBACKHOOD1,
		BELLIGERENT_EMPIRE0,
		BELLIGERENT_EMPIRE1,
		BELLIGERENT_EMPIRE2,
		BELLIGERENT_EMPIRE3,
		BELLIGERENT_EMPIRE4,
		BELLIGERENT_EMPIRE5,
		BELLIGERENT_EMPIRE6,
		BELLIGERENT_EMPIRE7,
		BELLIGERENT_EMPIRE8,
		BELLIGERENT_EMPIRE9,
		BELLIGERENT_EMPIRE10,
		BELLIGERENT_EMPIRE11,
		BELLIGERENT_EMPIRE12,
		BELLIGERENT_EMPIRE13,
		BELLIGERENT_EMPIRE14,
		BELLIGERENT_EMPIRE15,
		BELLIGERENT_EMPIRE16,
		BELLIGERENT_EMPIRE17,
		BELLIGERENT_EMPIRE18,
		BELLIGERENT_EMPIRE19,
		BELLIGERENT_EMPIRE20
	};
	
	EnumWrapper<terBelligerent> aaaa;
	float xxx;
	BitVector<terBelligerent> flags;
	
	Node();
	virtual ~Node() {}
	virtual TreeNode* edit(const char* treeNodeName) const;
	virtual void apply(const TreeNode* treeNode);
	virtual void save(ScriptParser& parser) const;
	virtual void load(ScriptParser& parser);
};

struct NodeA : Node {
	int fff;
	Vect3f pos;
	
	NodeA();
	TreeNode* edit(const char* treeNodeName) const;
	void apply(const TreeNode* treeNode);
	void save(ScriptParser& parser) const;
	void load(ScriptParser& parser);
};

struct Record {
	ShareHandle<Node> node;
	vector<ShareHandle<Node> > nodes;
	bool valueBool;
	int valueInt;
	float valueFloat;
	double valueDouble;
	PrmString valueString;
	vector<int> arrayInt;
	Vect3f pos;
	QuatF ori;
	Mat3f rot;
	MatXf rotX;
	
	Record();
	TreeNode* edit(const char* treeNodeName) const;
	void apply(const TreeNode* treeNode);
	void save(ScriptParser& parser) const;
	void load(ScriptParser& parser);
};

struct MegaData {
	Record records;
	vector<EnumWrapper<Mode> > mode;
	CustomString modelName;
	
	MegaData();
	TreeNode* edit(const char* treeNodeName) const;
	void apply(const TreeNode* treeNode);
	void save(ScriptParser& parser) const;
	void load(ScriptParser& parser);
};


struct XRealPrm {
	vector<EnumWrapper<AAAA::Mode> > mmmm;
	ShareHandle<Condition> condition;
	vector<PrmString> strings;
	EnumWrapper<Node::terBelligerent> belligerent;
	BitVector<Mode> bitFlag;
	ShareHandle<Node> node_;
	ShareHandle<Node> node__;
	vector<Node> nodesVect;
	vector<ShareHandle<Condition> > ConditionsVectPtr;
	MegaData data;
	Node node;
	vector<int> colors;
	ArrayWrapper<unsigned char, 10> icolors;
	ArrayWrapper<PrmString, 3> scol;
	
	XRealPrm();
	TreeNode* edit() const;
	void apply(const TreeNode* node);
	friend ScriptParser& operator<<(ScriptParser& parser, const XRealPrm& data);
	friend ScriptParser& operator>>(ScriptParser& parser, XRealPrm& data);
};

const EnumDescriptor<AAAA::Mode>& getEnumDescriptor(AAAA::Mode type);
const EnumDescriptor<ConditionNode::Type>& getEnumDescriptor(ConditionNode::Type type);
const EnumDescriptor<ConditionSwitcher::Type>& getEnumDescriptor(ConditionSwitcher::Type type);
const EnumDescriptor<Node::terBelligerent>& getEnumDescriptor(Node::terBelligerent type);
const EnumDescriptor<Mode>& getEnumDescriptor(Mode type);

#endif //_XPRM_SECTION_XRealPrm
//////////////////////////////////////////////////////////////////////////////////////////////
//	XScript end: XRealPrm
//////////////////////////////////////////////////////////////////////////////////////////////