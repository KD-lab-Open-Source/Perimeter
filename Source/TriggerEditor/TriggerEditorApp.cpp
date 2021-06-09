// TriggerEditor.cpp : Defines the initialization routines for the DLL.
//

#include "StdAfx.h"
#include "TriggerEditorApp.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//
//	Note!
//
//		If this DLL is dynamically linked against the MFC
//		DLLs, any functions exported from this DLL which
//		call into MFC must have the AFX_MANAGE_STATE macro
//		added at the very beginning of the function.
//
//		For example:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// normal function body here
//		}
//
//		It is very important that this macro appear in each
//		function, prior to any calls into MFC.  This means that
//		it must appear as the first statement within the 
//		function, even before any object variable declarations
//		as their constructors may generate calls into the MFC
//		DLL.
//
//		Please see MFC Technical Notes 33 and 58 for additional
//		details.
//

/////////////////////////////////////////////////////////////////////////////
// CTriggerEditorApp

BEGIN_MESSAGE_MAP(CTriggerEditorApp, CWinApp)
	//{{AFX_MSG_MAP(CTriggerEditorApp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTriggerEditorApp construction

CTriggerEditorApp::CTriggerEditorApp()
{
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CTriggerEditorApp object

//CTriggerEditorApp theApp;


BOOL CTriggerEditorApp::InitInstance()
{
	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

	SetRegistryKey(_T("Codemasters\\Perimeter"));
	return TRUE;
}


