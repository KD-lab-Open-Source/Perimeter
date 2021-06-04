// SettingsSheet.h : header file
//
// This class defines custom modal property sheet 
// SettingsSheet.
 
#ifndef __SETTINGSSHEET_H__
#define __SETTINGSSHEET_H__

#include "MiscSettingsPage.h"
#include "graphsettingspage.h"
#include "soundsettingspage.h"
#include "GamePropertyPage.h"
#include "aboutpage.h"

/////////////////////////////////////////////////////////////////////////////
// SettingsSheet

class SettingsSheet : public CPropertySheet
{
	DECLARE_DYNAMIC(SettingsSheet)

// Construction
public:
	SettingsSheet(CWnd* pWndParent = NULL);

// Attributes
public:
	MiscSettingsPage miscSettingsPage_;
	GraphSettingsPage graphSettingsPage_;
	SoundSettingsPage soundSettingsPage_;
	GamePropertyPage gamePropertyPage_;
	AboutPage		 aboutPage_;
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SettingsSheet)
	public:
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL

	void onChangeLanguage(CString const& newLanguageName);
// Implementation
public:
	virtual ~SettingsSheet();

	void reloadCaptions();
// Generated message map functions
protected:
	//{{AFX_MSG(SettingsSheet)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP();
	void loadLanguage();
protected:
};

/////////////////////////////////////////////////////////////////////////////

#endif	// __SETTINGSSHEET_H__
