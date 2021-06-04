#if !defined(AFX_ChooseName_H__80408421_6F3E_46FA_BD1E_A93574B3480D__INCLUDED_)
#define AFX_ChooseName_H__80408421_6F3E_46FA_BD1E_A93574B3480D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ChooseName.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// ChooseName dialog

#include <utility>

class ChooseName : public CDialog
{
// Construction
public:
	ChooseName(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(ChooseName)
	enum { IDD = IDD_DLG_NAME };
	CString	m_strName;
	//}}AFX_DATA

	void set_str(const CString &str){
		m_strName = str;
	}
	
	const CString& get_str(){
		return m_strName;
	}
	
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(ChooseName)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(ChooseName)
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
std::pair<const CString, bool> AskUserForName(LPCTSTR lpszDef);

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ChooseName_H__80408421_6F3E_46FA_BD1E_A93574B3480D__INCLUDED_)
