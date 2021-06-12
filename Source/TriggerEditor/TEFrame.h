#if !defined(AFX_TEFRAME_H__89496F98_054A_46A7_94DE_E0CC73AB9027__INCLUDED_)
#define AFX_TEFRAME_H__89496F98_054A_46A7_94DE_E0CC73AB9027__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TEFrame.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// TEFrame frame

class TETreeManager;
class ScaleMgr;
class TriggerEditorView;
class TriggerProfList;
class TriggerEditor;

#include <boost/scoped_ptr.hpp>

class TEFrame : public CFrameWnd
{
	DECLARE_DYNCREATE(TEFrame)
protected:

// Attributes
public:
	TEFrame();           
	TEFrame(TriggerEditor* triggerEditor);           
	virtual ~TEFrame();

// Operations
public:
	void setParent(HWND hparent);
	HWND getParent() const;

	void setTETreeManager(TETreeManager* mngr);
	TETreeManager* getTETreeManager() const;

	TriggerEditorView* addTriggerEditorView();

	TriggerProfList* getTriggerProfList() const;
	//! \brief Для установки размеров окна view. Вызывается один раз в начале, 
	//! чтобы сделать это когда нужно мне
	void updateViewSize();
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(TEFrame)
	public:
	virtual BOOL OnCmdMsg(UINT nID, int nCode, 
							void* pExtra, 
							AFX_CMDHANDLERINFO* pHandlerInfo);
	protected:
	virtual void PostNcDestroy();
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(TEFrame)
	afx_msg void OnClose();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnFileSave();
	afx_msg void onViewProfiler();
	afx_msg void onUpdateViewProfiler(CCmdUI* pCmdUI);
	afx_msg void onViewToolbar();
	afx_msg void onUpdateViewToolbar(CCmdUI* pCmdUI);
	afx_msg void onViewTree();
	afx_msg void onUpdateViewTree(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP();

	//! После создания выполняет настройки необъодимые для корректной работы дерева
	bool initTETreeManager();
private:
	//! Окно, поверх которого открывается окно редактора триггеров
	HWND hParentWnd_;
	//! Дерево редактора триггеров
	TETreeManager* treeManager_;
	//! Управление масштабом
	boost::scoped_ptr<ScaleMgr> scaleMgr_;
	//! Статус бар
	CStatusBar wndStatusBar_;
	//! Окно, в котором отображается поле редактора триггеров
	TriggerEditorView* triggerEditorView_;

	TriggerEditor* triggerEditor_;

	//! Отладчик, триггеров
	boost::scoped_ptr<TriggerProfList> triggerProfList_;

	CToolBar mainToolBar_;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TEFRAME_H__89496F98_054A_46A7_94DE_E0CC73AB9027__INCLUDED_)
