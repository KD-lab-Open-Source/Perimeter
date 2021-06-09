#include "afxwin.h"
#if !defined(AFX_DLGLOADSPRITE_H__247736EA_D68D_4A92_B31C_118C665C14E4__INCLUDED_)
#define AFX_DLGLOADSPRITE_H__247736EA_D68D_4A92_B31C_118C665C14E4__INCLUDED_
#include "../Render/src/FileImage.h"
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgLoadSprite.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTextureWnd window

class CTextureWnd : public CWnd
{
	HBITMAP  m_hBmp;
	cFileImage* FileImage;
	int m_CountFrame;
	int m_CurFrame;
	bool m_bAlpha;
public:
	CTextureWnd();
	virtual ~CTextureWnd();
	void SetBitmap(HBITMAP hBmp);
	int SetFileImage(CString fName,bool alpha);
	void NextFrame();
	void UpDateBitMap();
	bool Empty(){return FileImage==NULL;}
	int GetRealyFramesCount();
	int GetFramesCount();

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTextureWnd)
	//}}AFX_VIRTUAL

	// Generated message map functions
protected:
	//{{AFX_MSG(CTextureWnd)
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnTimer(UINT nIDEvent);
};


/////////////////////////////////////////////////////////////////////////////
// CDlgLoadSprite dialog

class CDlgLoadSprite : public CFileDialog
{
	int m_intCurImage;
	bool m_bPlumeVisible;
	CString  m_str[2];
	void SetTitleImage();
public:
	CDlgLoadSprite(CWnd* pParent = NULL);

	CString GetStrTexture(){ return m_str[0];}
	void SetStrTexture(CString str){m_str[0] = str;}
	CString GetStrPlumeTexture(){return m_str[1];}
	void SetStrPlumeTexture(CString str){m_str[1] = str;}
	bool& PlumeVisible(){return m_bPlumeVisible;}
	virtual INT_PTR DoModal();
// Dialog Data
	//{{AFX_DATA(CDlgLoadSprite)
	enum { IDD = IDD_LOAD_SPRITE };
	CTextureWnd	m_Image[2];
	CStatic m_title[2];
	BOOL	m_bAlpha;
	//}}AFX_DATA
//	CTextureWnd m_wndPlumeImage;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgLoadSprite)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CDlgLoadSprite)
	afx_msg void OnAlpha();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	virtual void OnFileNameChange();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnChildActivate();
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
};
/////////////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGLOADSPRITE_H__247736EA_D68D_4A92_B31C_118C665C14E4__INCLUDED_)
