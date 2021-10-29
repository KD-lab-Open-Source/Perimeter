#include "StdAfx.h"
#include "../Resource.h"
#include "TETreeManager.h"
#include "TETreeDlg.h"
#include "TETreeNotifyListener.h"

TETreeManager::TETreeManager(void)
{
}

TETreeManager::~TETreeManager(void)
{
}

bool TETreeManager::create(CFrameWnd* pParent, uint32_t dwStyle)
{
	parentFrame_ = pParent;

	treeDlg_.reset(new TETreeDlg(parentFrame_));
	if (!treeDlg_)
		return false;

	if (!dialogBar_.Create(pParent, treeDlg_.get(), dwStyle, TETreeDlg::IDD))
		return false;

	dialogBar_.EnableDocking(CBRS_ALIGN_ANY);
//	pParent->DockControlBar(&dialogBar_, AFX_IDW_DOCKBAR_LEFT);
	return true;
}

void TETreeManager::dockBar(uint32_t dockBarID)
{
	parentFrame_->DockControlBar(&dialogBar_, dockBarID);
}

bool TETreeManager::show(eShowHide e)
{
	if (e == SH_SHOW)
		parentFrame_->ShowControlBar(&dialogBar_, TRUE, FALSE); 
	else 
		parentFrame_->ShowControlBar(&dialogBar_, FALSE, FALSE); 
	return true;
}

bool TETreeManager::isVisible() const {
	return (IsWindowVisible(dialogBar_.m_hWnd) == TRUE);
}

xTreeListCtrl& TETreeManager::getTreeListCtrl() const
{
	return treeDlg_->getTreeCtrl();
}

void TETreeManager::setTETreeNotifyListener(TETreeNotifyListener* ptl)
{
	treeDlg_->setTETreeNotifyListener(ptl);
}