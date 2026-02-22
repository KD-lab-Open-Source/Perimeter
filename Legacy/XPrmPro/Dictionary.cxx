
//////////////////////////////////////////////////////////////////////////////////////////////
//	XScript definition
//	Section: DictionaryPrm
//	Number of variables: 1
//	This is computer generated code, don't try to change it
//////////////////////////////////////////////////////////////////////////////////////////////
#include <typeinfo>
#include "XPrmProInternal.h"

DictionaryRecord::DictionaryRecord()
{
	parentName = "";
	oldName = "";
	newName = "";
}

void DictionaryRecord::save(ScriptParser& parser) const 
{
	parser << OPEN_BRACKET;
	parser << OFFSET << "parentName" << ASSIGNMENT << parentName << SEMICOLON;
	parser << OFFSET << "oldName" << ASSIGNMENT << oldName << SEMICOLON;
	parser << OFFSET << "newName" << ASSIGNMENT << newName << SEMICOLON;
	parser << CLOSE_BRACKET;
}

inline ScriptParser& operator<<(ScriptParser& parser, const DictionaryRecord& dictionaryRecordVar)
{ 
	dictionaryRecordVar.save(parser);
	return parser;
}

void DictionaryRecord::load(ScriptParser& parser)
{
	parser >> OPEN_BRACKET;
	SmartName smartNameVar("DictionaryRecord");
	parser >> smartNameVar;
	string smartNamePrev = smartNameVar;
	for(;;){
		if(smartNameVar == "parentName")
			parser >> ASSIGNMENT >> parentName >> SEMICOLON >> smartNameVar;
		if(smartNameVar == "oldName")
			parser >> ASSIGNMENT >> oldName >> SEMICOLON >> smartNameVar;
		if(smartNameVar == "newName")
			parser >> ASSIGNMENT >> newName >> SEMICOLON >> smartNameVar;
		if(smartNameVar == "}")
			break;
		if(smartNameVar == smartNamePrev)
			parser >> SKIP_VALUE >> SEMICOLON >> smartNameVar;
		else
			smartNamePrev = smartNameVar;
	}
}

inline ScriptParser& operator>>(ScriptParser& parser, DictionaryRecord& dictionaryRecordVar)
{ 
	dictionaryRecordVar.load(parser);
	return parser;
}

TreeNode* DictionaryRecord::edit(const char* treeNodeName) const
{
	TreeNode* treeNode = new TreeNode(treeNodeName ? treeNodeName : "", !useAlternativeNames() ? "DictionaryRecord" : "DictionaryRecord", "DictionaryRecord", TreeNode::STATIC);
	treeNode->push_back(createNode("parentName", parentName));
	if(useAlternativeNames())
		treeNode->children().back()->setHidden();
	treeNode->push_back(createNode("oldName", oldName));
	if(useAlternativeNames())
		treeNode->children().back()->setHidden();
	treeNode->push_back(createNode("newName", newName));
	if(useAlternativeNames())
		treeNode->children().back()->setHidden();
	return treeNode;
}

inline TreeNode* createNode(const char* name, const DictionaryRecord& dictionaryRecordVar)
{ 
	return dictionaryRecordVar.edit(name);
}

void DictionaryRecord::apply(const TreeNode* treeNode)
{
	TreeNode::const_iterator i = treeNode->children().begin();
	applyNode(parentName, *i);
	++i;
	applyNode(oldName, *i);
	++i;
	applyNode(newName, *i);
	++i;
}

inline void applyNode(DictionaryRecord& dictionaryRecordVar, const TreeNode* treeNode)
{ 
	dictionaryRecordVar.apply(treeNode);
}

DictionaryPrm::DictionaryPrm()
{
}

ScriptParser& operator<<(ScriptParser& parser, const DictionaryPrm& dictionaryPrmVar)
{
	parser << "DictionaryPrm" << ASSIGNMENT << OPEN_BRACKET;
	parser << OFFSET << "dictionaryRecords" << ASSIGNMENT << dictionaryPrmVar.dictionaryRecords << SEMICOLON;
	parser << CLOSE_BRACKET << SEMICOLON;
	return parser;
}

ScriptParser& operator>>(ScriptParser& parser, DictionaryPrm& dictionaryPrmVar)
{
	parser >> "DictionaryPrm" >> ASSIGNMENT >> OPEN_BRACKET;
	SmartName smartNameVar("DictionaryPrm");
	parser >> smartNameVar;
	string smartNamePrev = smartNameVar;
	for(;;){
		if(smartNameVar == "dictionaryRecords")
			parser >> ASSIGNMENT >> dictionaryPrmVar.dictionaryRecords >> SEMICOLON >> smartNameVar;
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

TreeNode* DictionaryPrm::edit() const
{
	TreeNode* treeNode = new TreeNode("DictionaryPrm", "", "DictionaryPrm");
	treeNode->push_back(createNode("dictionaryRecords", dictionaryRecords));
	if(useAlternativeNames())
		treeNode->children().back()->setHidden();
	return treeNode;
}

void DictionaryPrm::apply(const TreeNode* treeNode)
{
	TreeNode::const_iterator i = treeNode->children().begin();
	applyNode(dictionaryRecords, *i);
	++i;
}

//////////////////////////////////////////////////////////////////////////////////////////////
//	XScript end: DictionaryPrm
//////////////////////////////////////////////////////////////////////////////////////////////