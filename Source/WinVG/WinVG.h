// WinVG.h : main header file for the WINVG application
//

#if !defined(AFX_WINVG_H__17B7B26D_2EFD_49CF_AEF2_861E2D89B4F3__INCLUDED_)
#define AFX_WINVG_H__17B7B26D_2EFD_49CF_AEF2_861E2D89B4F3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CWinVGApp:
// See WinVG.cpp for the implementation of this class
//

class CWinVGApp : public CWinApp
{
public:
	CWinVGApp();

	void SetActive(BOOL b){bActive=b;};
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWinVGApp)
	public:
	virtual BOOL InitInstance();
	virtual BOOL OnIdle(LONG lCount);
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CWinVGApp)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
protected:
	BOOL bActive;
};

extern CWinVGApp theApp;
/////////////////////////////////////////////////////////////////////////////
inline int GetRegistryInt(LPCTSTR lpszEntry, int nDefault )
{return theApp.GetProfileInt("WinVG",lpszEntry,nDefault);}

inline BOOL WriteRegistryInt(LPCTSTR lpszEntry, int nValue)
{return theApp.WriteProfileInt("WinVG",lpszEntry,nValue );}

inline CString GetRegistryString(LPCTSTR lpszEntry,LPCTSTR lpszDefault = NULL)
{return theApp.GetProfileString("WinVG",lpszEntry,lpszDefault);}

inline BOOL WriteRegistryString(LPCTSTR lpszEntry,LPCTSTR lpszValue)
{return theApp.WriteProfileString("WinVG",lpszEntry,lpszValue);}

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WINVG_H__17B7B26D_2EFD_49CF_AEF2_861E2D89B4F3__INCLUDED_)
