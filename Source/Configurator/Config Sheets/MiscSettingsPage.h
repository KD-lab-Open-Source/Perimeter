// MiscSettingsPage.h : header file
//

#ifndef __MISCSETTINGSPAGE_H__
#define __MISCSETTINGSPAGE_H__

//#include "Settings/ISetting.h"
#include "BasePage.h"
#include "SettingFactory/ISettingFactory.h"

#include <list>


class LanguageSetting;
/////////////////////////////////////////////////////////////////////////////
// MiscSettingsPage dialog

class MiscSettingsPage : public BasePage
{
//	DECLARE_DYNCREATE(MiscSettingsPage)

// Construction
public:
	MiscSettingsPage(ISettingFactory* factory);
	~MiscSettingsPage();

	LPCTSTR MiscSettingsPage::getLocaleKeyPostfix() const;
// Dialog Data
	//{{AFX_DATA(MiscSettingsPage)
	enum { IDD = IDD_PROPPAGE_MISC };
	CComboBox	languageWnd_;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(MiscSettingsPage)
	public:
	virtual BOOL OnApply();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void reloadLanguage();
	// Generated message map functions
	//{{AFX_MSG(MiscSettingsPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeComboLanguage();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP();

	LanguageSetting* languageSetting_;
};



#endif // __MISCSETTINGSPAGE_H__
