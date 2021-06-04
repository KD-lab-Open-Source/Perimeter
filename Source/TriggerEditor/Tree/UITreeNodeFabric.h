// UITreeNodeFabric.h: interface for the UITreeNodeFabric class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_UITREENODEFABRIC_H__7B4D4985_90C8_4F60_BC2C_94E359F5FEAA__INCLUDED_)
#define AFX_UITREENODEFABRIC_H__7B4D4985_90C8_4F60_BC2C_94E359F5FEAA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <memory>

interface IUITreeNode;
class UITreeNodeFabric  
{
	UITreeNodeFabric();
	~UITreeNodeFabric();
public:
	static std::auto_ptr<IUITreeNode> create(std::string const& action, 
											int ordinalNumber);
};

#endif // !defined(AFX_UITREENODEFABRIC_H__7B4D4985_90C8_4F60_BC2C_94E359F5FEAA__INCLUDED_)
