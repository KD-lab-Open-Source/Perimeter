#include "stdafx.h"
#include "resource.h"
#include "triggerproflist.h"
#include "Custom Controls/CoolDialogBar.h"
#include "TriggerDbgDlg.h"

TriggerProfList::TriggerProfList(void):
	m_pFrameWnd(NULL)
{
}

TriggerProfList::~TriggerProfList(void)
{
}

void TriggerProfList::setTriggerChainProfiler(ITriggerChainProfiler* ptrProf){
	m_ptrTrigDbgDlg->setTriggerChainProfiler(ptrProf);
}

bool TriggerProfList::create(CFrameWnd* pParent, DWORD dwStyle)
{
	m_pFrameWnd = pParent;

	m_ptrTrigDbgDlg.reset(new TriggerDbgDlg);
	if (!m_ptrTrigDbgDlg.get())
		return false;

	m_ptrDlgBar.reset(new CCoolDialogBar);
	if (!m_ptrDlgBar.get())
		return false;
	
	if (!m_ptrDlgBar->Create(pParent, m_ptrTrigDbgDlg.get(), 
		dwStyle, TriggerDbgDlg::IDD))
		return false;

	m_ptrDlgBar->EnableDocking(CBRS_ALIGN_ANY);
	return true;
}

bool TriggerProfList::show() const 
{
	m_pFrameWnd->ShowControlBar(m_ptrDlgBar.get(), TRUE, FALSE); 
	m_ptrTrigDbgDlg->OnShow();
	return true;
}

bool TriggerProfList::hide() const
{
	m_pFrameWnd->ShowControlBar(m_ptrDlgBar.get(), FALSE, FALSE); 
	m_ptrTrigDbgDlg->OnHide();
	return true;
}

bool TriggerProfList::isVisible() const {
	return IsWindowVisible(m_ptrDlgBar->m_hWnd);
}

bool TriggerProfList::load()
{
	return m_ptrTrigDbgDlg->Load();
}

bool TriggerProfList::next() const
{
	return m_ptrTrigDbgDlg->Next();
}

bool TriggerProfList::prev() const
{
	return m_ptrTrigDbgDlg->Prev();
}

bool TriggerProfList::activate()
{
	if (m_ptrTrigDbgDlg->Activate())
		return load();
	return false;
}

void TriggerProfList::dockBar(UINT uiDocBarId)
{
	m_pFrameWnd->DockControlBar(m_ptrDlgBar.get(), uiDocBarId);
}

void TriggerProfList::setTriggerEditorView(ITriggerEditorView* ptrTEView)
{
	m_ptrTrigDbgDlg->setTriggerEditorView(ptrTEView);
}

bool TriggerProfList::canBeActivated() const{
	return m_ptrTrigDbgDlg->canBeActivated();
}
