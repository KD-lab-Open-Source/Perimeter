#pragma once

#include "TEEngine/Profiler/ITriggerProflist.h"
#include <memory>

class CCoolDialogBar;
class TriggerDbgDlg;
interface ITriggerEditorView;
interface ITriggerChainProfiler;

class TriggerProfList :
	public ITriggerProfList
{
public:
	TriggerProfList(void);
	~TriggerProfList(void);

	bool create(CFrameWnd* pParent, 
								DWORD dwStyle = WS_CHILD |CBRS_FLYBY | 
								CBRS_RIGHT | CBRS_SIZE_DYNAMIC);

	virtual bool show()const;
	virtual bool hide()const;
	virtual bool isVisible()const;

	virtual bool load();
	virtual bool next()const;
	virtual bool prev()const;
	virtual bool activate();
	virtual bool canBeActivated() const;

	void dockBar(UINT uiDocBarId);
	void setTriggerEditorView(ITriggerEditorView* ptrTEView);
	void setTriggerChainProfiler(ITriggerChainProfiler* ptrITriggerChainProf);

private:
	std::auto_ptr<TriggerDbgDlg> m_ptrTrigDbgDlg;
	std::auto_ptr<CCoolDialogBar> m_ptrDlgBar;
	CFrameWnd* m_pFrameWnd;
};
