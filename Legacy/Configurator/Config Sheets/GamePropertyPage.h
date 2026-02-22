#if !defined(AFX_GAMEPROPERTYPAGE_H__4CAFD977_022F_47DD_99A3_039826235CC7__INCLUDED_)
#define AFX_GAMEPROPERTYPAGE_H__4CAFD977_022F_47DD_99A3_039826235CC7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// GamePropertyPage.h : header file
//

#include "SettingFactory/ISettingFactory.h"
#include "BasePage.h"
/////////////////////////////////////////////////////////////////////////////
// GamePropertyPage dialog

class GamePropertyPage : public BasePage
{
// Construction
public:
	GamePropertyPage(ISettingFactory* factory);
	~GamePropertyPage();

// Dialog Data
	//{{AFX_DATA(GamePropertyPage)
	enum { IDD = IDD_PROPPAGE_GAME };
	//}}AFX_DATA

	virtual LPCTSTR getLocaleKeyPostfix() const;
// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(GamePropertyPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	virtual void reloadLanguage();
// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(GamePropertyPage)
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GAMEPROPERTYPAGE_H__4CAFD977_022F_47DD_99A3_039826235CC7__INCLUDED_)
