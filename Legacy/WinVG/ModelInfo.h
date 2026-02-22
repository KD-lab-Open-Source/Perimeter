#if !defined(AFX_MODELINFO_H__F25C6A99_6377_4E0B_AE34_E939B5362137__INCLUDED_)
#define AFX_MODELINFO_H__F25C6A99_6377_4E0B_AE34_E939B5362137__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ModelInfo.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CModelInfo dialog

class CModelInfo : public CDialog
{
	cObjectNodeRoot* object;
// Construction
public:
	CModelInfo(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CModelInfo)
	enum { IDD = IDD_DIALOG_MODEL_INFO };
	CListBox	textures;
	CString	fname;
	int		material_num;
	int		object_num;
	//}}AFX_DATA

	void Init(cObjectNodeRoot* UObj);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CModelInfo)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CModelInfo)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MODELINFO_H__F25C6A99_6377_4E0B_AE34_E939B5362137__INCLUDED_)
