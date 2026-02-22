#if !defined(AFX_TEXTUREDIRECTORY_H__AA790369_1853_4CD9_9CB2_56BB45B08861__INCLUDED_)
#define AFX_TEXTUREDIRECTORY_H__AA790369_1853_4CD9_9CB2_56BB45B08861__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TextureDirectory.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTextureDirectory dialog

class CTextureDirectory : public CDialog
{
// Construction
public:
	CTextureDirectory(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CTextureDirectory)
	enum { IDD = IDD_TEXTURE_DIRECTORY };
	CComboBox	directory;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTextureDirectory)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTextureDirectory)
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TEXTUREDIRECTORY_H__AA790369_1853_4CD9_9CB2_56BB45B08861__INCLUDED_)
