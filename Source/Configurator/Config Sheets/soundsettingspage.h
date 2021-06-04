#if !defined(AFX_SOUNDSETTINGSPAGE_H__582A8E2E_FF39_4871_825F_878AC24E1679__INCLUDED_)
#define AFX_SOUNDSETTINGSPAGE_H__582A8E2E_FF39_4871_825F_878AC24E1679__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// soundsettingspage.h : header file
//

#include "SettingFactory/ISettingFactory.h"
#include "BasePage.h"
/////////////////////////////////////////////////////////////////////////////
// SoundSettingsPage dialog

class SoundSettingsPage : public BasePage
{
// Construction
public:
	SoundSettingsPage(ISettingFactory* factory);
	~SoundSettingsPage();

	virtual LPCTSTR getLocaleKeyPostfix() const;
// Dialog Data
	//{{AFX_DATA(SoundSettingsPage)
	enum { IDD = IDD_PROPPAGE_SOUND };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(SoundSettingsPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(SoundSettingsPage)
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP();

	void reloadLanguage();

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SOUNDSETTINGSPAGE_H__582A8E2E_FF39_4871_825F_878AC24E1679__INCLUDED_)
