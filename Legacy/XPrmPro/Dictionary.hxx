
//////////////////////////////////////////////////////////////////////////////////////////////
//	XScript declaration
//	Section: DictionaryPrm
//	Number of variables: 1
//	This is computer generated code, don't try to change it
//////////////////////////////////////////////////////////////////////////////////////////////
#ifndef _XPRM_SECTION_DictionaryPrm
#define _XPRM_SECTION_DictionaryPrm

#include "XPrmPro.h"

struct DictionaryRecord {
	PrmString parentName;
	PrmString oldName;
	PrmString newName;
	
	DictionaryRecord();
	TreeNode* edit(const char* treeNodeName) const;
	void apply(const TreeNode* treeNode);
	void save(ScriptParser& parser) const;
	void load(ScriptParser& parser);
};


struct DictionaryPrm {
	vector<DictionaryRecord> dictionaryRecords;
	
	DictionaryPrm();
	TreeNode* edit() const;
	void apply(const TreeNode* node);
	friend ScriptParser& operator<<(ScriptParser& parser, const DictionaryPrm& data);
	friend ScriptParser& operator>>(ScriptParser& parser, DictionaryPrm& data);
};


#endif //_XPRM_SECTION_DictionaryPrm
//////////////////////////////////////////////////////////////////////////////////////////////
//	XScript end: DictionaryPrm
//////////////////////////////////////////////////////////////////////////////////////////////