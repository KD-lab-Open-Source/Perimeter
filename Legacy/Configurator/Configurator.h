// Configurator.h : main header file for the CONFIGURATOR application
//

#if !defined(AFX_CONFIGURATOR_H__CADE5B57_426B_4A68_B74D_A1C2D279A2C1__INCLUDED_)
#define AFX_CONFIGURATOR_H__CADE5B57_426B_4A68_B74D_A1C2D279A2C1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CConfiguratorApp:
// See Configurator.cpp for the implementation of this class
//

class CConfiguratorApp : public CWinApp
{
public:
	CConfiguratorApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CConfiguratorApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CConfiguratorApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CONFIGURATOR_H__CADE5B57_426B_4A68_B74D_A1C2D279A2C1__INCLUDED_)
