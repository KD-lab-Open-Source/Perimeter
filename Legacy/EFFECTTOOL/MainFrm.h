// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINFRM_H__83DD7B06_F1A4_4905_AAA7_218DD0FE5100__INCLUDED_)
#define AFX_MAINFRM_H__83DD7B06_F1A4_4905_AAA7_218DD0FE5100__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DlgBar.h"
#include "D3DWindow.h"
#include "EffectToolDoc.h"

class CMainFrame : public CFrameWnd
{
	CEffectToolDoc* GetDocument();
	CD3DWindow*     GetD3DWindow();
	
	void GetValue(float& f, int idc);
	void SetValue(float f, int idc);
	void UpdateControlBar();

protected:
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)


	void LoadFromReg();
public:	
	void SaveToReg();
	friend class COptTree;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFrame)
	//}}AFX_VIRTUAL

public:
	virtual ~CMainFrame();

	void SetFps(float fps);
protected:  // control bar embedded members
	CToolBar    m_wndToolBar;
//	CToolBar    m_wndToolBarTools;
	CStatusBar  m_wndStatusBar;

	CSplitterWnd m_wndSplitter;
	CSplitterWnd m_wndSplitter1;
	CSplitterWnd m_wndSplitter2;

	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	//{{AFX_MSG(CMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnUpdateTools(CCmdUI* pCmdUI);
	afx_msg void OnToolsBsDir();
	afx_msg void OnToolsDir();
	afx_msg void OnToolsPos();
	afx_msg void OnToolsFullScreen();
	afx_msg void OnChangeEditRate();
	afx_msg void OnActivateApp(BOOL bActive, DWORD hTask);
	//}}AFX_MSG
	LRESULT OnUpdateBar(WPARAM wParam, LPARAM lParam);
	void OnUpdateSplineEdit(CCmdUI* pCmdUI);
	void OnSplineEdit();
	void OnUpdateToolsOpt();
	void OnUpdateToolsDir(CCmdUI* pCmdUI);
	void OnUpdateToolsInt(CCmdUI* pCmdUI);
	void OnUpdateToolsOptEnb(CCmdUI* pCmdUI);
	void OnUpdateForAll(CCmdUI* pCmdUI);
	void OnTools();
	void OnFindCenter();
	DECLARE_MESSAGE_MAP()
public:
	void SplineEndEdit();
	afx_msg void OnClose();
};
extern CMainFrame* mf;

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINFRM_H__83DD7B06_F1A4_4905_AAA7_218DD0FE5100__INCLUDED_)
