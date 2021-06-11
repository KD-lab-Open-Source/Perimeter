#include "StdAfx.h"
#include "TreeInterface.h"

extern "C" TreeNode const* treeControlEdit(const TreeNode * treeNode,
	HWND hwnd, TreeControlSetup & treeControlSetup)
{
	return NULL;
}
// Очищает статические данные
extern "C" void treeControlFree(TreeNode const*)
{

}