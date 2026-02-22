#if !defined(AFX_LOADMAPDIALOG_H__8F994217_C8D3_4BA8_B6DE_50664C577B88__INCLUDED_)
#define AFX_LOADMAPDIALOG_H__8F994217_C8D3_4BA8_B6DE_50664C577B88__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// LoadMapDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CLoadMapDialog dialog

class CLoadMapDialog : public CDialog
{
// Construction
public:
	CLoadMapDialog(CWnd* pParent = NULL);   // standard constructor

	int getSelectedMap();

// Dialog Data
	//{{AFX_DATA(CLoadMapDialog)
	enum { IDD = IDD_LOAD_MAP };
	CListBox	mapList;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLoadMapDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	
	int selectedMap;
	// Generated message map functions
	//{{AFX_MSG(CLoadMapDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchange();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LOADMAPDIALOG_H__8F994217_C8D3_4BA8_B6DE_50664C577B88__INCLUDED_)
