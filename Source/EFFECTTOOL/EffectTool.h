// EffectTool.h : main header file for the EFFECTTOOL application
//

#if !defined(AFX_EFFECTTOOL_H__63D9CD81_1175_45A6_A5EA_0B80457AC33F__INCLUDED_)
#define AFX_EFFECTTOOL_H__63D9CD81_1175_45A6_A5EA_0B80457AC33F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

#include "D3DScene.h"

/////////////////////////////////////////////////////////////////////////////
// CEffectToolApp:
// See EffectTool.cpp for the implementation of this class
//

class CEffectToolApp : public CWinApp
{
	void CheckCreateEmtrDir();

public:

	CString    m_szDir;
	CString    m_szDirEmitters;
	CD3DScene  scene;

	CEffectToolApp();
	~CEffectToolApp();

	void SetActive(bool b){bActive=b;}
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEffectToolApp)
	public:
	virtual BOOL InitInstance();
	virtual BOOL OnIdle(LONG lCount);
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CEffectToolApp)
	afx_msg void OnAppAbout();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
protected:
	bool bActive;
};

extern CEffectToolApp theApp;

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EFFECTTOOL_H__63D9CD81_1175_45A6_A5EA_0B80457AC33F__INCLUDED_)
