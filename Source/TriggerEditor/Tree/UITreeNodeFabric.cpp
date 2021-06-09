// UITreeNodeFabric.cpp: implementation of the UITreeNodeFabric class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "UITreeNodeFabric.h"
#include "TreeNodes/UITreeNode.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

UITreeNodeFabric::UITreeNodeFabric()
{

}

UITreeNodeFabric::~UITreeNodeFabric()
{

}

std::auto_ptr<IUITreeNode> UITreeNodeFabric::create(std::string const& action, 
													int ordinalNumber)
{
	return std::auto_ptr<IUITreeNode>(new UITreeNode(action, ordinalNumber));
}
