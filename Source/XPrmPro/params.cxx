
//////////////////////////////////////////////////////////////////////////////////////////////
//	XScript definition
//	Section: XRealPrm
//	Number of variables: 14
//	This is computer generated code, don't try to change it
//////////////////////////////////////////////////////////////////////////////////////////////
#include <typeinfo>
#include "XPrmProInternal.h"


#ifndef _XPRM_ENUM_Mode
#define _XPRM_ENUM_Mode

struct EnumMode : EnumDescriptor<Mode>
{
	EnumMode();
};

const EnumDescriptor<Mode>& getEnumDescriptor(Mode type)
{
	static EnumMode descriptor;
	return descriptor;
}

EnumMode::EnumMode()
: EnumDescriptor<Mode>("Mode")
{
	add(MODE_1, "MODE_1", "Мода 1");
	add(MODE_2, "MODE_2", "Мода 2");
}

#endif //_XPRM_ENUM_Mode


#ifndef _XPRM_ENUM_AAAA_Mode
#define _XPRM_ENUM_AAAA_Mode

struct EnumAAAAMode : EnumDescriptor<AAAA::Mode>
{
	EnumAAAAMode();
};

const EnumDescriptor<AAAA::Mode>& getEnumDescriptor(AAAA::Mode type)
{
	static EnumAAAAMode descriptor;
	return descriptor;
}

EnumAAAAMode::EnumAAAAMode()
: EnumDescriptor<AAAA::Mode>("Mode")
{
	add(AAAA::MODE_1, "MODE_1", "Мода 1");
	add(AAAA::MODE_2, "MODE_2", "Мода 2");
}

#endif //_XPRM_ENUM_AAAA_Mode

AAAA::AAAA()
{
	internalColor_ = 0;
}

void AAAA::save(ScriptParser& parser) const 
{
	parser << OPEN_BRACKET;
	parser << OFFSET << "internalColor_" << ASSIGNMENT << internalColor_ << SEMICOLON;
	parser << CLOSE_BRACKET;
}

inline ScriptParser& operator<<(ScriptParser& parser, const AAAA& aAAAVar)
{ 
	aAAAVar.save(parser);
	return parser;
}

void AAAA::load(ScriptParser& parser)
{
	parser >> OPEN_BRACKET;
	SmartName smartNameVar("AAAA");
	parser >> smartNameVar;
	string smartNamePrev = smartNameVar;
	for(;;){
		if(smartNameVar == "internalColor_")
			parser >> ASSIGNMENT >> internalColor_ >> SEMICOLON >> smartNameVar;
		if(smartNameVar == "}")
			break;
		if(smartNameVar == smartNamePrev)
			parser >> SKIP_VALUE >> SEMICOLON >> smartNameVar;
		else
			smartNamePrev = smartNameVar;
	}
}

inline ScriptParser& operator>>(ScriptParser& parser, AAAA& aAAAVar)
{ 
	aAAAVar.load(parser);
	return parser;
}

TreeNode* AAAA::edit(const char* treeNodeName) const
{
	TreeNode* treeNode = new TreeNode(treeNodeName ? treeNodeName : "", !useAlternativeNames() ? "AAAA" : "AAAA", "AAAA", TreeNode::STATIC);
	treeNode->setColor(internalColor_);
	return treeNode;
}

inline TreeNode* createNode(const char* name, const AAAA& aAAAVar)
{ 
	return aAAAVar.edit(name);
}

void AAAA::apply(const TreeNode* treeNode)
{
	TreeNode::const_iterator i = treeNode->children().begin();
	internalColor_ = treeNode->color();
}

inline void applyNode(AAAA& aAAAVar, const TreeNode* treeNode)
{ 
	aAAAVar.apply(treeNode);
}

Condition::Condition()
{
	internalColor_ = 0;
	state_ = false;
}

void Condition::save(ScriptParser& parser) const 
{
	parser << OPEN_BRACKET;
	parser << OFFSET << "internalColor_" << ASSIGNMENT << internalColor_ << SEMICOLON;
	parser << OFFSET << "state_" << ASSIGNMENT << state_ << SEMICOLON;
	parser << CLOSE_BRACKET;
}

inline ScriptParser& operator<<(ScriptParser& parser, const Condition& conditionVar)
{ 
	conditionVar.save(parser);
	return parser;
}

void Condition::load(ScriptParser& parser)
{
	parser >> OPEN_BRACKET;
	SmartName smartNameVar("Condition");
	parser >> smartNameVar;
	string smartNamePrev = smartNameVar;
	for(;;){
		if(smartNameVar == "internalColor_")
			parser >> ASSIGNMENT >> internalColor_ >> SEMICOLON >> smartNameVar;
		if(smartNameVar == "state_")
			parser >> ASSIGNMENT >> state_ >> SEMICOLON >> smartNameVar;
		if(smartNameVar == "}")
			break;
		if(smartNameVar == smartNamePrev)
			parser >> SKIP_VALUE >> SEMICOLON >> smartNameVar;
		else
			smartNamePrev = smartNameVar;
	}
}

inline ScriptParser& operator>>(ScriptParser& parser, Condition& conditionVar)
{ 
	conditionVar.load(parser);
	return parser;
}

TreeNode* Condition::edit(const char* treeNodeName) const
{
	TreeNode* treeNode = new TreeNode(treeNodeName ? treeNodeName : "", !useAlternativeNames() ? "Condition" : "Не выполняется никогда (для выключения триггеров)", "Condition", TreeNode::STATIC);
	treeNode->setColor(internalColor_);
	treeNode->push_back(createNode(!useAlternativeNames() ? "state_" : "Текущее состояние_", state_));
	return treeNode;
}

inline TreeNode* createNode(const char* name, const Condition& conditionVar)
{ 
	return conditionVar.edit(name);
}

void Condition::apply(const TreeNode* treeNode)
{
	TreeNode::const_iterator i = treeNode->children().begin();
	internalColor_ = treeNode->color();
	applyNode(state_, *i);
	++i;
}

inline void applyNode(Condition& conditionVar, const TreeNode* treeNode)
{ 
	conditionVar.apply(treeNode);
}

inline void savePointer(ScriptParser& parser, const Condition* conditionVar)
{
	parser << typeid(*conditionVar).name() << SPACE << *conditionVar;
}

inline void loadPointer(ScriptParser& parser, Condition*& conditionVar)
{
	SmartName typeName("Condition");
	parser >> typeName;
	if(typeName == typeid(Condition).name())
		conditionVar = new Condition;
	else if(typeName == typeid(ConditionSwitcher).name())
		conditionVar = new ConditionSwitcher;
	else if(typeName == typeid(ConditionAnd).name())
		conditionVar = new ConditionAnd;
	else if(typeName == typeid(ConditionOr).name())
		conditionVar = new ConditionOr;
	else if(typeName == typeid(ConditionNot).name())
		conditionVar = new ConditionNot;
	else
		xassertStr(typeName == "" && "XPrmPro: Unknown type will be ignored", (typeName.parentName() + "::" + typeName).c_str());
	
	if(conditionVar)
		parser >> *conditionVar;
	else
		parser >> SKIP_VALUE;
}

const TreeNode* treeNodeSelectorCondition(int typeIndex)
{
	xassert(typeIndex >= 0 && typeIndex < 5);
	
	static TreeNode* treeNodes[6] = {
		createNode(0, Condition()),
		createNode(0, ConditionSwitcher()),
		createNode(0, ConditionAnd()),
		createNode(0, ConditionOr()),
		createNode(0, ConditionNot()),
		0
	};
	
	return treeNodes[typeIndex];
}

inline TreeNode* createNode(const char* name, const ShareHandle<Condition>& conditionVar)
{
	if(!name)
		name = "";
	
	static string comboList;
	static string comboListAlt;
	static bool inited = false;
	if(!inited){
		inited = true;
		comboList += "Condition";
		comboListAlt += "Не выполняется никогда (для выключения триггеров)";
		comboList += "|";
		comboList += "ConditionSwitcher";
		comboListAlt += "|";
		comboListAlt += "И/ИЛИ";
		comboList += "|";
		comboList += "ConditionAnd";
		comboListAlt += "|";
		comboListAlt += "И";
		comboList += "|";
		comboList += "ConditionOr";
		comboListAlt += "|";
		comboListAlt += "ИЛИ";
		comboList += "|";
		comboList += "ConditionNot";
		comboListAlt += "|";
		comboListAlt += "НЕ";
	}
	TreeNode* treeNode = new TreeNode(name, "", "Condition*", TreeNode::POLYMORPHIC, !useAlternativeNames() ? comboList.c_str() : comboListAlt.c_str(), 0, &treeNodeSelectorCondition);
	if(conditionVar){
		ShareHandle<TreeNode> tmpNode = createNode("", *conditionVar);
		treeNode->setValue(tmpNode->value().c_str());
		treeNode->setColor(tmpNode->color());
		TreeNode::const_iterator i;
		FOR_EACH(tmpNode->children(), i)
			treeNode->push_back(*i);
	}
	return treeNode;
}

inline void applyNode(ShareHandle<Condition>& conditionVar, const TreeNode* treeNode)
{
	string typeName = treeNode->value();
	if(typeName != ""){
		if(conditionVar)
			conditionVar = 0;
		if(typeName == (!useAlternativeNames() ? "Condition" : "Не выполняется никогда (для выключения триггеров)"))
			conditionVar = new Condition;
		else if(typeName == (!useAlternativeNames() ? "ConditionSwitcher" : "И/ИЛИ"))
			conditionVar = new ConditionSwitcher;
		else if(typeName == (!useAlternativeNames() ? "ConditionAnd" : "И"))
			conditionVar = new ConditionAnd;
		else if(typeName == (!useAlternativeNames() ? "ConditionOr" : "ИЛИ"))
			conditionVar = new ConditionOr;
		else if(typeName == (!useAlternativeNames() ? "ConditionNot" : "НЕ"))
			conditionVar = new ConditionNot;
		else
			xassert(0);
		applyNode(*conditionVar, treeNode);
	}
	else
		conditionVar = 0;
}


#ifndef _XPRM_ENUM_ConditionNode_Type
#define _XPRM_ENUM_ConditionNode_Type

struct EnumConditionNodeType : EnumDescriptor<ConditionNode::Type>
{
	EnumConditionNodeType();
};

const EnumDescriptor<ConditionNode::Type>& getEnumDescriptor(ConditionNode::Type type)
{
	static EnumConditionNodeType descriptor;
	return descriptor;
}

EnumConditionNodeType::EnumConditionNodeType()
: EnumDescriptor<ConditionNode::Type>("Type")
{
	add(ConditionNode::NORMAL, "NORMAL", "да");
	add(ConditionNode::INVERTED, "INVERTED", "НЕ");
}

#endif //_XPRM_ENUM_ConditionNode_Type

ConditionNode::ConditionNode()
{
	type = NORMAL;
	condition = 0;
}

void ConditionNode::save(ScriptParser& parser) const 
{
	parser << OPEN_BRACKET;
	parser << OFFSET << "type" << ASSIGNMENT << type << SEMICOLON;
	parser << OFFSET << "condition" << ASSIGNMENT << condition << SEMICOLON;
	parser << CLOSE_BRACKET;
}

inline ScriptParser& operator<<(ScriptParser& parser, const ConditionNode& conditionNodeVar)
{ 
	conditionNodeVar.save(parser);
	return parser;
}

void ConditionNode::load(ScriptParser& parser)
{
	parser >> OPEN_BRACKET;
	SmartName smartNameVar("ConditionNode");
	parser >> smartNameVar;
	string smartNamePrev = smartNameVar;
	for(;;){
		if(smartNameVar == "type")
			parser >> ASSIGNMENT >> type >> SEMICOLON >> smartNameVar;
		if(smartNameVar == "condition")
			parser >> ASSIGNMENT >> condition >> SEMICOLON >> smartNameVar;
		if(smartNameVar == "}")
			break;
		if(smartNameVar == smartNamePrev)
			parser >> SKIP_VALUE >> SEMICOLON >> smartNameVar;
		else
			smartNamePrev = smartNameVar;
	}
}

inline ScriptParser& operator>>(ScriptParser& parser, ConditionNode& conditionNodeVar)
{ 
	conditionNodeVar.load(parser);
	return parser;
}

TreeNode* ConditionNode::edit(const char* treeNodeName) const
{
	TreeNode* treeNode = new TreeNode(treeNodeName ? treeNodeName : "", !useAlternativeNames() ? "ConditionNode" : "", "ConditionNode", TreeNode::STATIC);
	treeNode->push_back(createNode(!useAlternativeNames() ? "type" : "Тип", type));
	treeNode->push_back(createNode(!useAlternativeNames() ? "condition" : "Условие", condition));
	return treeNode;
}

inline TreeNode* createNode(const char* name, const ConditionNode& conditionNodeVar)
{ 
	return conditionNodeVar.edit(name);
}

void ConditionNode::apply(const TreeNode* treeNode)
{
	TreeNode::const_iterator i = treeNode->children().begin();
	applyNode(type, *i);
	++i;
	applyNode(condition, *i);
	++i;
}

inline void applyNode(ConditionNode& conditionNodeVar, const TreeNode* treeNode)
{ 
	conditionNodeVar.apply(treeNode);
}


#ifndef _XPRM_ENUM_ConditionSwitcher_Type
#define _XPRM_ENUM_ConditionSwitcher_Type

struct EnumConditionSwitcherType : EnumDescriptor<ConditionSwitcher::Type>
{
	EnumConditionSwitcherType();
};

const EnumDescriptor<ConditionSwitcher::Type>& getEnumDescriptor(ConditionSwitcher::Type type)
{
	static EnumConditionSwitcherType descriptor;
	return descriptor;
}

EnumConditionSwitcherType::EnumConditionSwitcherType()
: EnumDescriptor<ConditionSwitcher::Type>("Type")
{
	add(ConditionSwitcher::AND, "AND", "И");
	add(ConditionSwitcher::OR, "OR", "ИЛИ");
}

#endif //_XPRM_ENUM_ConditionSwitcher_Type

ConditionSwitcher::ConditionSwitcher()
{
	type = AND;
}

void ConditionSwitcher::save(ScriptParser& parser) const 
{
	parser << OPEN_BRACKET;
	parser << OFFSET << "internalColor_" << ASSIGNMENT << internalColor_ << SEMICOLON;
	parser << OFFSET << "state_" << ASSIGNMENT << state_ << SEMICOLON;
	parser << OFFSET << "type" << ASSIGNMENT << type << SEMICOLON;
	parser << OFFSET << "conditions" << ASSIGNMENT << conditions << SEMICOLON;
	parser << CLOSE_BRACKET;
}

inline ScriptParser& operator<<(ScriptParser& parser, const ConditionSwitcher& conditionSwitcherVar)
{ 
	conditionSwitcherVar.save(parser);
	return parser;
}

void ConditionSwitcher::load(ScriptParser& parser)
{
	parser >> OPEN_BRACKET;
	SmartName smartNameVar("Condition");
	parser >> smartNameVar;
	string smartNamePrev = smartNameVar;
	for(;;){
		if(smartNameVar == "internalColor_")
			parser >> ASSIGNMENT >> internalColor_ >> SEMICOLON >> smartNameVar;
		if(smartNameVar == "state_")
			parser >> ASSIGNMENT >> state_ >> SEMICOLON >> smartNameVar;
		if(smartNameVar == "type")
			parser >> ASSIGNMENT >> type >> SEMICOLON >> smartNameVar;
		if(smartNameVar == "conditions")
			parser >> ASSIGNMENT >> conditions >> SEMICOLON >> smartNameVar;
		if(smartNameVar == "}")
			break;
		if(smartNameVar == smartNamePrev)
			parser >> SKIP_VALUE >> SEMICOLON >> smartNameVar;
		else
			smartNamePrev = smartNameVar;
	}
}

inline ScriptParser& operator>>(ScriptParser& parser, ConditionSwitcher& conditionSwitcherVar)
{ 
	conditionSwitcherVar.load(parser);
	return parser;
}

TreeNode* ConditionSwitcher::edit(const char* treeNodeName) const
{
	TreeNode* treeNode = new TreeNode(treeNodeName ? treeNodeName : "", !useAlternativeNames() ? "ConditionSwitcher" : "И/ИЛИ", "ConditionSwitcher", TreeNode::STATIC);
	treeNode->setColor(internalColor_);
	treeNode->push_back(createNode(!useAlternativeNames() ? "state_" : "Текущее состояние_", state_));
	treeNode->push_back(createNode(!useAlternativeNames() ? "type" : "Тип", type));
	treeNode->push_back(createNode(!useAlternativeNames() ? "conditions" : "Условия", conditions));
	return treeNode;
}

inline TreeNode* createNode(const char* name, const ConditionSwitcher& conditionSwitcherVar)
{ 
	return conditionSwitcherVar.edit(name);
}

void ConditionSwitcher::apply(const TreeNode* treeNode)
{
	TreeNode::const_iterator i = treeNode->children().begin();
	internalColor_ = treeNode->color();
	applyNode(state_, *i);
	++i;
	applyNode(type, *i);
	++i;
	applyNode(conditions, *i);
	++i;
}

inline void applyNode(ConditionSwitcher& conditionSwitcherVar, const TreeNode* treeNode)
{ 
	conditionSwitcherVar.apply(treeNode);
}

ConditionAnd::ConditionAnd()
{
}

void ConditionAnd::save(ScriptParser& parser) const 
{
	parser << OPEN_BRACKET;
	parser << OFFSET << "internalColor_" << ASSIGNMENT << internalColor_ << SEMICOLON;
	parser << OFFSET << "state_" << ASSIGNMENT << state_ << SEMICOLON;
	parser << OFFSET << "conditions" << ASSIGNMENT << conditions << SEMICOLON;
	parser << CLOSE_BRACKET;
}

inline ScriptParser& operator<<(ScriptParser& parser, const ConditionAnd& conditionAndVar)
{ 
	conditionAndVar.save(parser);
	return parser;
}

void ConditionAnd::load(ScriptParser& parser)
{
	parser >> OPEN_BRACKET;
	SmartName smartNameVar("Condition");
	parser >> smartNameVar;
	string smartNamePrev = smartNameVar;
	for(;;){
		if(smartNameVar == "internalColor_")
			parser >> ASSIGNMENT >> internalColor_ >> SEMICOLON >> smartNameVar;
		if(smartNameVar == "state_")
			parser >> ASSIGNMENT >> state_ >> SEMICOLON >> smartNameVar;
		if(smartNameVar == "conditions")
			parser >> ASSIGNMENT >> conditions >> SEMICOLON >> smartNameVar;
		if(smartNameVar == "}")
			break;
		if(smartNameVar == smartNamePrev)
			parser >> SKIP_VALUE >> SEMICOLON >> smartNameVar;
		else
			smartNamePrev = smartNameVar;
	}
}

inline ScriptParser& operator>>(ScriptParser& parser, ConditionAnd& conditionAndVar)
{ 
	conditionAndVar.load(parser);
	return parser;
}

TreeNode* ConditionAnd::edit(const char* treeNodeName) const
{
	TreeNode* treeNode = new TreeNode(treeNodeName ? treeNodeName : "", !useAlternativeNames() ? "ConditionAnd" : "И", "ConditionAnd", TreeNode::STATIC);
	treeNode->setColor(internalColor_);
	treeNode->push_back(createNode(!useAlternativeNames() ? "state_" : "Текущее состояние_", state_));
	treeNode->push_back(createNode(!useAlternativeNames() ? "conditions" : "И", conditions));
	return treeNode;
}

inline TreeNode* createNode(const char* name, const ConditionAnd& conditionAndVar)
{ 
	return conditionAndVar.edit(name);
}

void ConditionAnd::apply(const TreeNode* treeNode)
{
	TreeNode::const_iterator i = treeNode->children().begin();
	internalColor_ = treeNode->color();
	applyNode(state_, *i);
	++i;
	applyNode(conditions, *i);
	++i;
}

inline void applyNode(ConditionAnd& conditionAndVar, const TreeNode* treeNode)
{ 
	conditionAndVar.apply(treeNode);
}

ConditionOr::ConditionOr()
{
}

void ConditionOr::save(ScriptParser& parser) const 
{
	parser << OPEN_BRACKET;
	parser << OFFSET << "internalColor_" << ASSIGNMENT << internalColor_ << SEMICOLON;
	parser << OFFSET << "state_" << ASSIGNMENT << state_ << SEMICOLON;
	parser << OFFSET << "conditions" << ASSIGNMENT << conditions << SEMICOLON;
	parser << CLOSE_BRACKET;
}

inline ScriptParser& operator<<(ScriptParser& parser, const ConditionOr& conditionOrVar)
{ 
	conditionOrVar.save(parser);
	return parser;
}

void ConditionOr::load(ScriptParser& parser)
{
	parser >> OPEN_BRACKET;
	SmartName smartNameVar("Condition");
	parser >> smartNameVar;
	string smartNamePrev = smartNameVar;
	for(;;){
		if(smartNameVar == "internalColor_")
			parser >> ASSIGNMENT >> internalColor_ >> SEMICOLON >> smartNameVar;
		if(smartNameVar == "state_")
			parser >> ASSIGNMENT >> state_ >> SEMICOLON >> smartNameVar;
		if(smartNameVar == "conditions")
			parser >> ASSIGNMENT >> conditions >> SEMICOLON >> smartNameVar;
		if(smartNameVar == "}")
			break;
		if(smartNameVar == smartNamePrev)
			parser >> SKIP_VALUE >> SEMICOLON >> smartNameVar;
		else
			smartNamePrev = smartNameVar;
	}
}

inline ScriptParser& operator>>(ScriptParser& parser, ConditionOr& conditionOrVar)
{ 
	conditionOrVar.load(parser);
	return parser;
}

TreeNode* ConditionOr::edit(const char* treeNodeName) const
{
	TreeNode* treeNode = new TreeNode(treeNodeName ? treeNodeName : "", !useAlternativeNames() ? "ConditionOr" : "ИЛИ", "ConditionOr", TreeNode::STATIC);
	treeNode->setColor(internalColor_);
	treeNode->push_back(createNode(!useAlternativeNames() ? "state_" : "Текущее состояние_", state_));
	treeNode->push_back(createNode(!useAlternativeNames() ? "conditions" : "ИЛИ", conditions));
	return treeNode;
}

inline TreeNode* createNode(const char* name, const ConditionOr& conditionOrVar)
{ 
	return conditionOrVar.edit(name);
}

void ConditionOr::apply(const TreeNode* treeNode)
{
	TreeNode::const_iterator i = treeNode->children().begin();
	internalColor_ = treeNode->color();
	applyNode(state_, *i);
	++i;
	applyNode(conditions, *i);
	++i;
}

inline void applyNode(ConditionOr& conditionOrVar, const TreeNode* treeNode)
{ 
	conditionOrVar.apply(treeNode);
}

ConditionNot::ConditionNot()
{
	condition = 0;
}

void ConditionNot::save(ScriptParser& parser) const 
{
	parser << OPEN_BRACKET;
	parser << OFFSET << "internalColor_" << ASSIGNMENT << internalColor_ << SEMICOLON;
	parser << OFFSET << "state_" << ASSIGNMENT << state_ << SEMICOLON;
	parser << OFFSET << "condition" << ASSIGNMENT << condition << SEMICOLON;
	parser << CLOSE_BRACKET;
}

inline ScriptParser& operator<<(ScriptParser& parser, const ConditionNot& conditionNotVar)
{ 
	conditionNotVar.save(parser);
	return parser;
}

void ConditionNot::load(ScriptParser& parser)
{
	parser >> OPEN_BRACKET;
	SmartName smartNameVar("Condition");
	parser >> smartNameVar;
	string smartNamePrev = smartNameVar;
	for(;;){
		if(smartNameVar == "internalColor_")
			parser >> ASSIGNMENT >> internalColor_ >> SEMICOLON >> smartNameVar;
		if(smartNameVar == "state_")
			parser >> ASSIGNMENT >> state_ >> SEMICOLON >> smartNameVar;
		if(smartNameVar == "condition")
			parser >> ASSIGNMENT >> condition >> SEMICOLON >> smartNameVar;
		if(smartNameVar == "}")
			break;
		if(smartNameVar == smartNamePrev)
			parser >> SKIP_VALUE >> SEMICOLON >> smartNameVar;
		else
			smartNamePrev = smartNameVar;
	}
}

inline ScriptParser& operator>>(ScriptParser& parser, ConditionNot& conditionNotVar)
{ 
	conditionNotVar.load(parser);
	return parser;
}

TreeNode* ConditionNot::edit(const char* treeNodeName) const
{
	TreeNode* treeNode = new TreeNode(treeNodeName ? treeNodeName : "", !useAlternativeNames() ? "ConditionNot" : "НЕ", "ConditionNot", TreeNode::STATIC);
	treeNode->setColor(internalColor_);
	treeNode->push_back(createNode(!useAlternativeNames() ? "state_" : "Текущее состояние_", state_));
	treeNode->push_back(createNode(!useAlternativeNames() ? "condition" : "НЕ", condition));
	return treeNode;
}

inline TreeNode* createNode(const char* name, const ConditionNot& conditionNotVar)
{ 
	return conditionNotVar.edit(name);
}

void ConditionNot::apply(const TreeNode* treeNode)
{
	TreeNode::const_iterator i = treeNode->children().begin();
	internalColor_ = treeNode->color();
	applyNode(state_, *i);
	++i;
	applyNode(condition, *i);
	++i;
}

inline void applyNode(ConditionNot& conditionNotVar, const TreeNode* treeNode)
{ 
	conditionNotVar.apply(treeNode);
}


#ifndef _XPRM_ENUM_Node_terBelligerent
#define _XPRM_ENUM_Node_terBelligerent

struct EnumNodeterBelligerent : EnumDescriptor<Node::terBelligerent>
{
	EnumNodeterBelligerent();
};

const EnumDescriptor<Node::terBelligerent>& getEnumDescriptor(Node::terBelligerent type)
{
	static EnumNodeterBelligerent descriptor;
	return descriptor;
}

EnumNodeterBelligerent::EnumNodeterBelligerent()
: EnumDescriptor<Node::terBelligerent>("terBelligerent")
{
	add(Node::BELLIGERENT_EXODUS, "BELLIGERENT_EXODUS", "Исходники");
	add(Node::BELLIGERENT_HARKBACKHOOD0, "BELLIGERENT_HARKBACKHOOD0", "Возвратники 1");
	add(Node::BELLIGERENT_HARKBACKHOOD1, "BELLIGERENT_HARKBACKHOOD1", "Возвратники 2");
	add(Node::BELLIGERENT_EMPIRE0, "BELLIGERENT_EMPIRE0", "BELLIGERENT_EMPIRE0");
	add(Node::BELLIGERENT_EMPIRE1, "BELLIGERENT_EMPIRE1", "BELLIGERENT_EMPIRE1");
	add(Node::BELLIGERENT_EMPIRE2, "BELLIGERENT_EMPIRE2", "BELLIGERENT_EMPIRE2");
	add(Node::BELLIGERENT_EMPIRE3, "BELLIGERENT_EMPIRE3", "BELLIGERENT_EMPIRE3");
	add(Node::BELLIGERENT_EMPIRE4, "BELLIGERENT_EMPIRE4", "BELLIGERENT_EMPIRE4");
	add(Node::BELLIGERENT_EMPIRE5, "BELLIGERENT_EMPIRE5", "BELLIGERENT_EMPIRE5");
	add(Node::BELLIGERENT_EMPIRE6, "BELLIGERENT_EMPIRE6", "BELLIGERENT_EMPIRE6");
	add(Node::BELLIGERENT_EMPIRE7, "BELLIGERENT_EMPIRE7", "BELLIGERENT_EMPIRE7");
	add(Node::BELLIGERENT_EMPIRE8, "BELLIGERENT_EMPIRE8", "BELLIGERENT_EMPIRE8");
	add(Node::BELLIGERENT_EMPIRE9, "BELLIGERENT_EMPIRE9", "BELLIGERENT_EMPIRE9");
	add(Node::BELLIGERENT_EMPIRE10, "BELLIGERENT_EMPIRE10", "BELLIGERENT_EMPIRE10");
	add(Node::BELLIGERENT_EMPIRE11, "BELLIGERENT_EMPIRE11", "BELLIGERENT_EMPIRE11");
	add(Node::BELLIGERENT_EMPIRE12, "BELLIGERENT_EMPIRE12", "BELLIGERENT_EMPIRE12");
	add(Node::BELLIGERENT_EMPIRE13, "BELLIGERENT_EMPIRE13", "BELLIGERENT_EMPIRE13");
	add(Node::BELLIGERENT_EMPIRE14, "BELLIGERENT_EMPIRE14", "BELLIGERENT_EMPIRE14");
	add(Node::BELLIGERENT_EMPIRE15, "BELLIGERENT_EMPIRE15", "BELLIGERENT_EMPIRE15");
	add(Node::BELLIGERENT_EMPIRE16, "BELLIGERENT_EMPIRE16", "BELLIGERENT_EMPIRE16");
	add(Node::BELLIGERENT_EMPIRE17, "BELLIGERENT_EMPIRE17", "BELLIGERENT_EMPIRE17");
	add(Node::BELLIGERENT_EMPIRE18, "BELLIGERENT_EMPIRE18", "BELLIGERENT_EMPIRE18");
	add(Node::BELLIGERENT_EMPIRE19, "BELLIGERENT_EMPIRE19", "BELLIGERENT_EMPIRE19");
	add(Node::BELLIGERENT_EMPIRE20, "BELLIGERENT_EMPIRE20", "BELLIGERENT_EMPIRE20");
}

#endif //_XPRM_ENUM_Node_terBelligerent

Node::Node()
{
	aaaa = BELLIGERENT_EXODUS;
	xxx = 0;
	flags = BELLIGERENT_EXODUS | BELLIGERENT_EMPIRE10;
}

void Node::save(ScriptParser& parser) const 
{
	parser << OPEN_BRACKET;
	parser << OFFSET << "aaaa" << ASSIGNMENT << aaaa << SEMICOLON;
	parser << OFFSET << "xxx" << ASSIGNMENT << xxx << SEMICOLON;
	parser << OFFSET << "flags" << ASSIGNMENT << flags << SEMICOLON;
	parser << CLOSE_BRACKET;
}

inline ScriptParser& operator<<(ScriptParser& parser, const Node& nodeVar)
{ 
	nodeVar.save(parser);
	return parser;
}

void Node::load(ScriptParser& parser)
{
	parser >> OPEN_BRACKET;
	SmartName smartNameVar("Node");
	parser >> smartNameVar;
	string smartNamePrev = smartNameVar;
	for(;;){
		if(smartNameVar == "aaaa")
			parser >> ASSIGNMENT >> aaaa >> SEMICOLON >> smartNameVar;
		if(smartNameVar == "xxx")
			parser >> ASSIGNMENT >> xxx >> SEMICOLON >> smartNameVar;
		if(smartNameVar == "flags")
			parser >> ASSIGNMENT >> flags >> SEMICOLON >> smartNameVar;
		if(smartNameVar == "}")
			break;
		if(smartNameVar == smartNamePrev)
			parser >> SKIP_VALUE >> SEMICOLON >> smartNameVar;
		else
			smartNamePrev = smartNameVar;
	}
}

inline ScriptParser& operator>>(ScriptParser& parser, Node& nodeVar)
{ 
	nodeVar.load(parser);
	return parser;
}

TreeNode* Node::edit(const char* treeNodeName) const
{
	TreeNode* treeNode = new TreeNode(treeNodeName ? treeNodeName : "", !useAlternativeNames() ? "Node" : "Узел", "Node", TreeNode::STATIC);
	treeNode->push_back(createNode("aaaa", aaaa));
	if(useAlternativeNames())
		treeNode->children().back()->setHidden();
	treeNode->push_back(createNode(!useAlternativeNames() ? "xxx" : "муму", xxx));
	treeNode->push_back(createNode("flags", flags));
	if(useAlternativeNames())
		treeNode->children().back()->setHidden();
	return treeNode;
}

inline TreeNode* createNode(const char* name, const Node& nodeVar)
{ 
	return nodeVar.edit(name);
}

void Node::apply(const TreeNode* treeNode)
{
	TreeNode::const_iterator i = treeNode->children().begin();
	applyNode(aaaa, *i);
	++i;
	applyNode(xxx, *i);
	++i;
	applyNode(flags, *i);
	++i;
}

inline void applyNode(Node& nodeVar, const TreeNode* treeNode)
{ 
	nodeVar.apply(treeNode);
}

inline void savePointer(ScriptParser& parser, const Node* nodeVar)
{
	parser << typeid(*nodeVar).name() << SPACE << *nodeVar;
}

inline void loadPointer(ScriptParser& parser, Node*& nodeVar)
{
	SmartName typeName("Node");
	parser >> typeName;
	if(typeName == typeid(Node).name())
		nodeVar = new Node;
	else if(typeName == typeid(NodeA).name())
		nodeVar = new NodeA;
	else
		xassertStr(typeName == "" && "XPrmPro: Unknown type will be ignored", (typeName.parentName() + "::" + typeName).c_str());
	
	if(nodeVar)
		parser >> *nodeVar;
	else
		parser >> SKIP_VALUE;
}

const TreeNode* treeNodeSelectorNode(int typeIndex)
{
	xassert(typeIndex >= 0 && typeIndex < 2);
	
	static TreeNode* treeNodes[3] = {
		createNode(0, Node()),
		createNode(0, NodeA()),
		0
	};
	
	return treeNodes[typeIndex];
}

inline TreeNode* createNode(const char* name, const ShareHandle<Node>& nodeVar)
{
	if(!name)
		name = "";
	
	static string comboList;
	static string comboListAlt;
	static bool inited = false;
	if(!inited){
		inited = true;
		comboList += "Node";
		comboListAlt += "Узел";
		comboList += "|";
		comboList += "NodeA";
		comboListAlt += "|";
		comboListAlt += "Узел А";
	}
	TreeNode* treeNode = new TreeNode(name, "", "Node*", TreeNode::POLYMORPHIC, !useAlternativeNames() ? comboList.c_str() : comboListAlt.c_str(), 0, &treeNodeSelectorNode);
	if(nodeVar){
		ShareHandle<TreeNode> tmpNode = createNode("", *nodeVar);
		treeNode->setValue(tmpNode->value().c_str());
		treeNode->setColor(tmpNode->color());
		TreeNode::const_iterator i;
		FOR_EACH(tmpNode->children(), i)
			treeNode->push_back(*i);
	}
	return treeNode;
}

inline void applyNode(ShareHandle<Node>& nodeVar, const TreeNode* treeNode)
{
	string typeName = treeNode->value();
	if(typeName != ""){
		if(nodeVar)
			nodeVar = 0;
		if(typeName == (!useAlternativeNames() ? "Node" : "Узел"))
			nodeVar = new Node;
		else if(typeName == (!useAlternativeNames() ? "NodeA" : "Узел А"))
			nodeVar = new NodeA;
		else
			xassert(0);
		applyNode(*nodeVar, treeNode);
	}
	else
		nodeVar = 0;
}

NodeA::NodeA()
{
	fff = 1;
	pos = Vect3f :: ZERO;
}

void NodeA::save(ScriptParser& parser) const 
{
	parser << OPEN_BRACKET;
	parser << OFFSET << "aaaa" << ASSIGNMENT << aaaa << SEMICOLON;
	parser << OFFSET << "xxx" << ASSIGNMENT << xxx << SEMICOLON;
	parser << OFFSET << "flags" << ASSIGNMENT << flags << SEMICOLON;
	parser << OFFSET << "fff" << ASSIGNMENT << fff << SEMICOLON;
	parser << OFFSET << "pos" << ASSIGNMENT << pos << SEMICOLON;
	parser << CLOSE_BRACKET;
}

inline ScriptParser& operator<<(ScriptParser& parser, const NodeA& nodeAVar)
{ 
	nodeAVar.save(parser);
	return parser;
}

void NodeA::load(ScriptParser& parser)
{
	parser >> OPEN_BRACKET;
	SmartName smartNameVar("Node");
	parser >> smartNameVar;
	string smartNamePrev = smartNameVar;
	for(;;){
		if(smartNameVar == "aaaa")
			parser >> ASSIGNMENT >> aaaa >> SEMICOLON >> smartNameVar;
		if(smartNameVar == "xxx")
			parser >> ASSIGNMENT >> xxx >> SEMICOLON >> smartNameVar;
		if(smartNameVar == "flags")
			parser >> ASSIGNMENT >> flags >> SEMICOLON >> smartNameVar;
		if(smartNameVar == "fff")
			parser >> ASSIGNMENT >> fff >> SEMICOLON >> smartNameVar;
		if(smartNameVar == "pos")
			parser >> ASSIGNMENT >> pos >> SEMICOLON >> smartNameVar;
		if(smartNameVar == "}")
			break;
		if(smartNameVar == smartNamePrev)
			parser >> SKIP_VALUE >> SEMICOLON >> smartNameVar;
		else
			smartNamePrev = smartNameVar;
	}
}

inline ScriptParser& operator>>(ScriptParser& parser, NodeA& nodeAVar)
{ 
	nodeAVar.load(parser);
	return parser;
}

TreeNode* NodeA::edit(const char* treeNodeName) const
{
	TreeNode* treeNode = new TreeNode(treeNodeName ? treeNodeName : "", !useAlternativeNames() ? "NodeA" : "Узел А", "NodeA", TreeNode::STATIC);
	treeNode->push_back(createNode("aaaa", aaaa));
	if(useAlternativeNames())
		treeNode->children().back()->setHidden();
	treeNode->push_back(createNode(!useAlternativeNames() ? "xxx" : "муму", xxx));
	treeNode->push_back(createNode("flags", flags));
	if(useAlternativeNames())
		treeNode->children().back()->setHidden();
	treeNode->push_back(createNode("fff", fff));
	if(useAlternativeNames())
		treeNode->children().back()->setHidden();
	treeNode->push_back(createNode("pos", pos));
	if(useAlternativeNames())
		treeNode->children().back()->setHidden();
	return treeNode;
}

inline TreeNode* createNode(const char* name, const NodeA& nodeAVar)
{ 
	return nodeAVar.edit(name);
}

void NodeA::apply(const TreeNode* treeNode)
{
	TreeNode::const_iterator i = treeNode->children().begin();
	applyNode(aaaa, *i);
	++i;
	applyNode(xxx, *i);
	++i;
	applyNode(flags, *i);
	++i;
	applyNode(fff, *i);
	++i;
	applyNode(pos, *i);
	++i;
}

inline void applyNode(NodeA& nodeAVar, const TreeNode* treeNode)
{ 
	nodeAVar.apply(treeNode);
}

Record::Record()
{
	node = 0;
	valueBool = false;
	valueInt = 1;
	valueFloat = 1;
	valueDouble = 1;
	valueString = "";
	pos = Vect3f :: ZERO;
	ori = QuatF :: ID;
	rot = Mat3f :: ID;
	rotX = MatXf :: ID;
}

void Record::save(ScriptParser& parser) const 
{
	parser << OPEN_BRACKET;
	parser << OFFSET << "node" << ASSIGNMENT << node << SEMICOLON;
	parser << OFFSET << "nodes" << ASSIGNMENT << nodes << SEMICOLON;
	parser << OFFSET << "valueBool" << ASSIGNMENT << valueBool << SEMICOLON;
	parser << OFFSET << "valueInt" << ASSIGNMENT << valueInt << SEMICOLON;
	parser << OFFSET << "valueFloat" << ASSIGNMENT << valueFloat << SEMICOLON;
	parser << OFFSET << "valueDouble" << ASSIGNMENT << valueDouble << SEMICOLON;
	parser << OFFSET << "valueString" << ASSIGNMENT << valueString << SEMICOLON;
	parser << OFFSET << "arrayInt" << ASSIGNMENT << arrayInt << SEMICOLON;
	parser << OFFSET << "pos" << ASSIGNMENT << pos << SEMICOLON;
	parser << OFFSET << "ori" << ASSIGNMENT << ori << SEMICOLON;
	parser << OFFSET << "rot" << ASSIGNMENT << rot << SEMICOLON;
	parser << OFFSET << "rotX" << ASSIGNMENT << rotX << SEMICOLON;
	parser << CLOSE_BRACKET;
}

inline ScriptParser& operator<<(ScriptParser& parser, const Record& recordVar)
{ 
	recordVar.save(parser);
	return parser;
}

void Record::load(ScriptParser& parser)
{
	parser >> OPEN_BRACKET;
	SmartName smartNameVar("Record");
	parser >> smartNameVar;
	string smartNamePrev = smartNameVar;
	for(;;){
		if(smartNameVar == "node")
			parser >> ASSIGNMENT >> node >> SEMICOLON >> smartNameVar;
		if(smartNameVar == "nodes")
			parser >> ASSIGNMENT >> nodes >> SEMICOLON >> smartNameVar;
		if(smartNameVar == "valueBool")
			parser >> ASSIGNMENT >> valueBool >> SEMICOLON >> smartNameVar;
		if(smartNameVar == "valueInt")
			parser >> ASSIGNMENT >> valueInt >> SEMICOLON >> smartNameVar;
		if(smartNameVar == "valueFloat")
			parser >> ASSIGNMENT >> valueFloat >> SEMICOLON >> smartNameVar;
		if(smartNameVar == "valueDouble")
			parser >> ASSIGNMENT >> valueDouble >> SEMICOLON >> smartNameVar;
		if(smartNameVar == "valueString")
			parser >> ASSIGNMENT >> valueString >> SEMICOLON >> smartNameVar;
		if(smartNameVar == "arrayInt")
			parser >> ASSIGNMENT >> arrayInt >> SEMICOLON >> smartNameVar;
		if(smartNameVar == "pos")
			parser >> ASSIGNMENT >> pos >> SEMICOLON >> smartNameVar;
		if(smartNameVar == "ori")
			parser >> ASSIGNMENT >> ori >> SEMICOLON >> smartNameVar;
		if(smartNameVar == "rot")
			parser >> ASSIGNMENT >> rot >> SEMICOLON >> smartNameVar;
		if(smartNameVar == "rotX")
			parser >> ASSIGNMENT >> rotX >> SEMICOLON >> smartNameVar;
		if(smartNameVar == "}")
			break;
		if(smartNameVar == smartNamePrev)
			parser >> SKIP_VALUE >> SEMICOLON >> smartNameVar;
		else
			smartNamePrev = smartNameVar;
	}
}

inline ScriptParser& operator>>(ScriptParser& parser, Record& recordVar)
{ 
	recordVar.load(parser);
	return parser;
}

TreeNode* Record::edit(const char* treeNodeName) const
{
	TreeNode* treeNode = new TreeNode(treeNodeName ? treeNodeName : "", !useAlternativeNames() ? "Record" : "Record", "Record", TreeNode::STATIC);
	treeNode->push_back(createNode("node", node));
	if(useAlternativeNames())
		treeNode->children().back()->setHidden();
	treeNode->push_back(createNode("nodes", nodes));
	if(useAlternativeNames())
		treeNode->children().back()->setHidden();
	treeNode->push_back(createNode("valueBool", valueBool));
	if(useAlternativeNames())
		treeNode->children().back()->setHidden();
	treeNode->push_back(createNode("valueInt", valueInt));
	if(useAlternativeNames())
		treeNode->children().back()->setHidden();
	treeNode->push_back(createNode("valueFloat", valueFloat));
	if(useAlternativeNames())
		treeNode->children().back()->setHidden();
	treeNode->push_back(createNode("valueDouble", valueDouble));
	if(useAlternativeNames())
		treeNode->children().back()->setHidden();
	treeNode->push_back(createNode("valueString", valueString));
	if(useAlternativeNames())
		treeNode->children().back()->setHidden();
	treeNode->push_back(createNode("arrayInt", arrayInt));
	if(useAlternativeNames())
		treeNode->children().back()->setHidden();
	treeNode->push_back(createNode("pos", pos));
	if(useAlternativeNames())
		treeNode->children().back()->setHidden();
	treeNode->push_back(createNode("ori", ori));
	if(useAlternativeNames())
		treeNode->children().back()->setHidden();
	treeNode->push_back(createNode("rot", rot));
	if(useAlternativeNames())
		treeNode->children().back()->setHidden();
	treeNode->push_back(createNode("rotX", rotX));
	if(useAlternativeNames())
		treeNode->children().back()->setHidden();
	return treeNode;
}

inline TreeNode* createNode(const char* name, const Record& recordVar)
{ 
	return recordVar.edit(name);
}

void Record::apply(const TreeNode* treeNode)
{
	TreeNode::const_iterator i = treeNode->children().begin();
	applyNode(node, *i);
	++i;
	applyNode(nodes, *i);
	++i;
	applyNode(valueBool, *i);
	++i;
	applyNode(valueInt, *i);
	++i;
	applyNode(valueFloat, *i);
	++i;
	applyNode(valueDouble, *i);
	++i;
	applyNode(valueString, *i);
	++i;
	applyNode(arrayInt, *i);
	++i;
	applyNode(pos, *i);
	++i;
	applyNode(ori, *i);
	++i;
	applyNode(rot, *i);
	++i;
	applyNode(rotX, *i);
	++i;
}

inline void applyNode(Record& recordVar, const TreeNode* treeNode)
{ 
	recordVar.apply(treeNode);
}

MegaData::MegaData()
{
	modelName = CustomString("jdif", getModelName);
}

void MegaData::save(ScriptParser& parser) const 
{
	parser << OPEN_BRACKET;
	parser << OFFSET << "records" << ASSIGNMENT << records << SEMICOLON;
	parser << OFFSET << "mode" << ASSIGNMENT << mode << SEMICOLON;
	parser << OFFSET << "modelName" << ASSIGNMENT << modelName << SEMICOLON;
	parser << CLOSE_BRACKET;
}

inline ScriptParser& operator<<(ScriptParser& parser, const MegaData& megaDataVar)
{ 
	megaDataVar.save(parser);
	return parser;
}

void MegaData::load(ScriptParser& parser)
{
	parser >> OPEN_BRACKET;
	SmartName smartNameVar("MegaData");
	parser >> smartNameVar;
	string smartNamePrev = smartNameVar;
	for(;;){
		if(smartNameVar == "records")
			parser >> ASSIGNMENT >> records >> SEMICOLON >> smartNameVar;
		if(smartNameVar == "mode")
			parser >> ASSIGNMENT >> mode >> SEMICOLON >> smartNameVar;
		if(smartNameVar == "modelName")
			parser >> ASSIGNMENT >> modelName >> SEMICOLON >> smartNameVar;
		if(smartNameVar == "}")
			break;
		if(smartNameVar == smartNamePrev)
			parser >> SKIP_VALUE >> SEMICOLON >> smartNameVar;
		else
			smartNamePrev = smartNameVar;
	}
}

inline ScriptParser& operator>>(ScriptParser& parser, MegaData& megaDataVar)
{ 
	megaDataVar.load(parser);
	return parser;
}

TreeNode* MegaData::edit(const char* treeNodeName) const
{
	TreeNode* treeNode = new TreeNode(treeNodeName ? treeNodeName : "", !useAlternativeNames() ? "MegaData" : "MegaData", "MegaData", TreeNode::STATIC);
	treeNode->push_back(createNode("records", records));
	if(useAlternativeNames())
		treeNode->children().back()->setHidden();
	treeNode->push_back(createNode("mode", mode));
	if(useAlternativeNames())
		treeNode->children().back()->setHidden();
	treeNode->push_back(createNode(!useAlternativeNames() ? "modelName" : "Имя модели", modelName));
	return treeNode;
}

inline TreeNode* createNode(const char* name, const MegaData& megaDataVar)
{ 
	return megaDataVar.edit(name);
}

void MegaData::apply(const TreeNode* treeNode)
{
	TreeNode::const_iterator i = treeNode->children().begin();
	applyNode(records, *i);
	++i;
	applyNode(mode, *i);
	++i;
	applyNode(modelName, *i);
	++i;
}

inline void applyNode(MegaData& megaDataVar, const TreeNode* treeNode)
{ 
	megaDataVar.apply(treeNode);
}

XRealPrm::XRealPrm()
{
	condition = 0;
	belligerent = Node :: BELLIGERENT_HARKBACKHOOD0;
	bitFlag = MODE_1 | MODE_2;
	node_ = 0;
	node__ = 0;
	scol.set("sajfd");
}

ScriptParser& operator<<(ScriptParser& parser, const XRealPrm& xRealPrmVar)
{
	parser << "XRealPrm" << ASSIGNMENT << OPEN_BRACKET;
	parser << OFFSET << "mmmm" << ASSIGNMENT << xRealPrmVar.mmmm << SEMICOLON;
	parser << OFFSET << "condition" << ASSIGNMENT << xRealPrmVar.condition << SEMICOLON;
	parser << OFFSET << "strings" << ASSIGNMENT << xRealPrmVar.strings << SEMICOLON;
	parser << OFFSET << "belligerent" << ASSIGNMENT << xRealPrmVar.belligerent << SEMICOLON;
	parser << OFFSET << "bitFlag" << ASSIGNMENT << xRealPrmVar.bitFlag << SEMICOLON;
	parser << OFFSET << "node_" << ASSIGNMENT << xRealPrmVar.node_ << SEMICOLON;
	parser << OFFSET << "node__" << ASSIGNMENT << xRealPrmVar.node__ << SEMICOLON;
	parser << OFFSET << "nodesVect" << ASSIGNMENT << xRealPrmVar.nodesVect << SEMICOLON;
	parser << OFFSET << "ConditionsVectPtr" << ASSIGNMENT << xRealPrmVar.ConditionsVectPtr << SEMICOLON;
	parser << OFFSET << "data" << ASSIGNMENT << xRealPrmVar.data << SEMICOLON;
	parser << OFFSET << "node" << ASSIGNMENT << xRealPrmVar.node << SEMICOLON;
	parser << OFFSET << "colors" << ASSIGNMENT << xRealPrmVar.colors << SEMICOLON;
	parser << OFFSET << "icolors" << ASSIGNMENT << xRealPrmVar.icolors << SEMICOLON;
	parser << OFFSET << "scol" << ASSIGNMENT << xRealPrmVar.scol << SEMICOLON;
	parser << CLOSE_BRACKET << SEMICOLON;
	return parser;
}

ScriptParser& operator>>(ScriptParser& parser, XRealPrm& xRealPrmVar)
{
	parser >> "XRealPrm" >> ASSIGNMENT >> OPEN_BRACKET;
	SmartName smartNameVar("XRealPrm");
	parser >> smartNameVar;
	string smartNamePrev = smartNameVar;
	for(;;){
		if(smartNameVar == "mmmm")
			parser >> ASSIGNMENT >> xRealPrmVar.mmmm >> SEMICOLON >> smartNameVar;
		if(smartNameVar == "condition")
			parser >> ASSIGNMENT >> xRealPrmVar.condition >> SEMICOLON >> smartNameVar;
		if(smartNameVar == "strings")
			parser >> ASSIGNMENT >> xRealPrmVar.strings >> SEMICOLON >> smartNameVar;
		if(smartNameVar == "belligerent")
			parser >> ASSIGNMENT >> xRealPrmVar.belligerent >> SEMICOLON >> smartNameVar;
		if(smartNameVar == "bitFlag")
			parser >> ASSIGNMENT >> xRealPrmVar.bitFlag >> SEMICOLON >> smartNameVar;
		if(smartNameVar == "node_")
			parser >> ASSIGNMENT >> xRealPrmVar.node_ >> SEMICOLON >> smartNameVar;
		if(smartNameVar == "node__")
			parser >> ASSIGNMENT >> xRealPrmVar.node__ >> SEMICOLON >> smartNameVar;
		if(smartNameVar == "nodesVect")
			parser >> ASSIGNMENT >> xRealPrmVar.nodesVect >> SEMICOLON >> smartNameVar;
		if(smartNameVar == "ConditionsVectPtr")
			parser >> ASSIGNMENT >> xRealPrmVar.ConditionsVectPtr >> SEMICOLON >> smartNameVar;
		if(smartNameVar == "data")
			parser >> ASSIGNMENT >> xRealPrmVar.data >> SEMICOLON >> smartNameVar;
		if(smartNameVar == "node")
			parser >> ASSIGNMENT >> xRealPrmVar.node >> SEMICOLON >> smartNameVar;
		if(smartNameVar == "colors")
			parser >> ASSIGNMENT >> xRealPrmVar.colors >> SEMICOLON >> smartNameVar;
		if(smartNameVar == "icolors")
			parser >> ASSIGNMENT >> xRealPrmVar.icolors >> SEMICOLON >> smartNameVar;
		if(smartNameVar == "scol")
			parser >> ASSIGNMENT >> xRealPrmVar.scol >> SEMICOLON >> smartNameVar;
		if(smartNameVar == "}")
			break;
		if(smartNameVar == smartNamePrev)
			parser >> SKIP_VALUE >> SEMICOLON >> smartNameVar;
		else
			smartNamePrev = smartNameVar;
	}
	parser >> SEMICOLON;
	return parser;
}

TreeNode* XRealPrm::edit() const
{
	TreeNode* treeNode = new TreeNode("XRealPrm", "", "XRealPrm");
	treeNode->push_back(createNode("mmmm", mmmm));
	if(useAlternativeNames())
		treeNode->children().back()->setHidden();
	treeNode->push_back(createNode(!useAlternativeNames() ? "condition" : "Условие", condition));
	treeNode->push_back(createNode("strings", strings));
	if(useAlternativeNames())
		treeNode->children().back()->setHidden();
	treeNode->push_back(createNode("belligerent", belligerent));
	if(useAlternativeNames())
		treeNode->children().back()->setHidden();
	treeNode->push_back(createNode("bitFlag", bitFlag));
	if(useAlternativeNames())
		treeNode->children().back()->setHidden();
	treeNode->push_back(createNode("node_", node_));
	if(useAlternativeNames())
		treeNode->children().back()->setHidden();
	treeNode->push_back(createNode("node__", node__));
	if(useAlternativeNames())
		treeNode->children().back()->setHidden();
	treeNode->push_back(createNode("nodesVect", nodesVect));
	if(useAlternativeNames())
		treeNode->children().back()->setHidden();
	treeNode->push_back(createNode("ConditionsVectPtr", ConditionsVectPtr));
	if(useAlternativeNames())
		treeNode->children().back()->setHidden();
	treeNode->push_back(createNode("data", data));
	if(useAlternativeNames())
		treeNode->children().back()->setHidden();
	treeNode->push_back(createNode("node", node));
	if(useAlternativeNames())
		treeNode->children().back()->setHidden();
	treeNode->push_back(createNode("colors", colors));
	if(useAlternativeNames())
		treeNode->children().back()->setHidden();
	treeNode->push_back(createNode("icolors", icolors));
	if(useAlternativeNames())
		treeNode->children().back()->setHidden();
	treeNode->push_back(createNode("scol", scol));
	if(useAlternativeNames())
		treeNode->children().back()->setHidden();
	return treeNode;
}

void XRealPrm::apply(const TreeNode* treeNode)
{
	TreeNode::const_iterator i = treeNode->children().begin();
	applyNode(mmmm, *i);
	++i;
	applyNode(condition, *i);
	++i;
	applyNode(strings, *i);
	++i;
	applyNode(belligerent, *i);
	++i;
	applyNode(bitFlag, *i);
	++i;
	applyNode(node_, *i);
	++i;
	applyNode(node__, *i);
	++i;
	applyNode(nodesVect, *i);
	++i;
	applyNode(ConditionsVectPtr, *i);
	++i;
	applyNode(data, *i);
	++i;
	applyNode(node, *i);
	++i;
	applyNode(colors, *i);
	++i;
	applyNode(icolors, *i);
	++i;
	applyNode(scol, *i);
	++i;
}

//////////////////////////////////////////////////////////////////////////////////////////////
//	XScript end: XRealPrm
//////////////////////////////////////////////////////////////////////////////////////////////