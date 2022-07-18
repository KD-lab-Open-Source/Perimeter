#include "StdAfx.h"

#include "EditArchive.h"

////////////////////////////////////////
int EditOArchive::typeIDs_;

EditOArchive::EditOArchive()
{
	currentNode_ = rootNode_ = new TreeNode("");
	translatedOnly_ = true;
	nesting_ = 0;
}

EditOArchive::~EditOArchive()
{
}

////////////////////////////////////////
EditIArchive::EditIArchive()
{
	currentNode_ = parentNode_ = 0;
	translatedOnly_ = true;
}

EditIArchive::EditIArchive(const TreeNode* node)
{
	currentNode_ = parentNode_ = 0;
	setRootNode(node);
	translatedOnly_ = true;
}

EditIArchive::EditIArchive(const EditOArchive& eoa)
{
	currentNode_ = parentNode_ = 0;
	setRootNode(eoa.rootNode());
	translatedOnly_ = true;
}

EditIArchive::~EditIArchive()
{
}

////////////////////////////////////////
EditArchive::EditArchive()
: hwnd_(nullptr),
treeControlSetup_(0, 0, 800, 1000, "treeControlSetup")
{
	outputNode_ = 0;
}

EditArchive::EditArchive(void* hwnd, const TreeControlSetup& treeControlSetup)
: hwnd_(hwnd),
treeControlSetup_(treeControlSetup)
{
	outputNode_ = 0;
}


EditArchive::~EditArchive()
{
	clear();
}

void EditArchive::setTranslatedOnly(bool translatedOnly)
{
	EditOArchive::setTranslatedOnly(translatedOnly);
	EditIArchive::setTranslatedOnly(translatedOnly);
}

bool EditArchive::edit()
{
	outputNode_ = 
		treeControlEdit(rootNode(), hwnd_, treeControlSetup_);
	setRootNode(outputNode_);
	return outputNode_;
}

void EditArchive::clear()
{
	if(outputNode_)
		treeControlFree(outputNode_);
	outputNode_ = 0;
}


