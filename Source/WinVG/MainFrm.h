// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINFRM_H__378C9D5C_3E28_40E8_84E9_C1FE6F876BEA__INCLUDED_)
#define AFX_MAINFRM_H__378C9D5C_3E28_40E8_84E9_C1FE6F876BEA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CMainFrame : public CFrameWnd
{
	friend class CWinVGView;
protected: // create from serialization only
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// Attributes
public:
	string effect_directory;

	void UpdateList();
	void UnknownSelect();
// Operations
public:
	void LoadRegistry();
	void SaveRegistry();
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFrame)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	int			m_bPressBBox	: 1;
	int			m_bPressBound	: 1;
	int			m_bPressLogic	: 1;
	int			m_bPressObject	: 1;
	int			m_bInit			: 1;

	CStatusBar  m_wndStatusBar;
	bool scale_normal;
protected:
	CToolBar    m_wndToolBar;

	CSplitterWnd m_Splitter0,m_Splitter1;

	bool use_shadow;
	void CheckControlSwitch();
	void AddListBox();
	void AddDirectoryBox();

	CComboBox list_box;
	CComboBox directory_box;
// Generated message map functions
protected:
	//{{AFX_MSG(CMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnButtonBbox();
	afx_msg void OnButtonBound();
	afx_msg void OnButtonLogic();
	afx_msg void OnButtonObject();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnButtonLighting();
	afx_msg void OnMenuControlCamera();
	afx_msg void OnMenuControlObject();
	afx_msg void OnMenuControlDirlight();
	afx_msg void OnButtonColor();
	afx_msg void OnButtonBkcolor();
	afx_msg void OnDebugMemory();
	afx_msg void OnNumObject();
	afx_msg void OnUpdateNumObject(CCmdUI* pCmdUI);
	afx_msg void OnHologram();
	afx_msg void OnUpdateHologram(CCmdUI* pCmdUI);
	afx_msg void OnScalenormal();
	afx_msg void OnUpdateScalenormal(CCmdUI* pCmdUI);
	afx_msg void OnButtonDebugShadowVolume();
	afx_msg void OnUpdateButtonDebugShadowVolume(CCmdUI* pCmdUI);
	afx_msg void OnButtonShadow();
	afx_msg void OnUpdateButtonShadow(CCmdUI* pCmdUI);
	afx_msg void OnEnableBump();
	afx_msg void OnUpdateEnableBump(CCmdUI* pCmdUI);
	afx_msg void OnEffectDirectory();
	afx_msg void OnSelchangeAnimationList();
	afx_msg void OnFileSettexturedirectory();
	afx_msg void OnModelInfo();
	afx_msg void OnSelchangeDirectoryBox();
	afx_msg void OnEnableAnisotropic();
	afx_msg void OnUpdateEnableAnisotropic(CCmdUI* pCmdUI);
	afx_msg void OnButtonBasement();
	afx_msg void OnUpdateButtonBasement(CCmdUI* pCmdUI);
	afx_msg void OnScreenShoot();
	afx_msg void OnEnableTilemap();
	afx_msg void OnUpdateEnableTilemap(CCmdUI* pCmdUI);
	//}}AFX_MSG
	afx_msg void OnActivateApp(BOOL bActive, DWORD hTask);
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
extern CMainFrame* gb_FrameWnd;

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINFRM_H__378C9D5C_3E28_40E8_84E9_C1FE6F876BEA__INCLUDED_)
