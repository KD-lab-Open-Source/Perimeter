// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__0DF779EF_6694_4FDC_A90A_95940C9ADEBD__INCLUDED_)
#define AFX_STDAFX_H__0DF779EF_6694_4FDC_A90A_95940C9ADEBD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#pragma warning(disable : 4786)
#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
//#include <afxdisp.h>        // MFC Automation classes
//#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

CString const iniFileName(_T(".\\Perimeter.ini"));
CString const mainIniFileName(_T(".\\Config.ini"));
DWORD const INI_READ_BUFFER = 2048;

#endif // !defined(AFX_STDAFX_H__0DF779EF_6694_4FDC_A90A_95940C9ADEBD__INCLUDED_)

