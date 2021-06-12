#if !defined(AFX_GRAPHSETTINGSPAGE_H__63469A7F_ED16_419E_939F_628D9AFB0804__INCLUDED_)
#define AFX_GRAPHSETTINGSPAGE_H__63469A7F_ED16_419E_939F_628D9AFB0804__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// graphsettingspage.h : header file
//

#include "SettingFactory/ISettingFactory.h"
#include <list>
#include "BasePage.h"
#include "Settings/GraphSettingsSet.h"
/////////////////////////////////////////////////////////////////////////////
// GraphSettingsPage dialog

class GraphSettingsPage : public BasePage
{
//	DECLARE_DYNCREATE(GraphSettingsPage)

// Construction
public:
	GraphSettingsPage(ISettingFactory* factory);
	~GraphSettingsPage();

// Dialog Data
	//{{AFX_DATA(GraphSettingsPage)
	enum { IDD = IDD_PROPPAGE_GRAPH };
	CComboBox	presetWnd_;
	//}}AFX_DATA

	virtual LPCTSTR getLocaleKeyPostfix() const;
// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(GraphSettingsPage)
	public:
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	void reloadLanguage();
// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(GraphSettingsPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeComboPreset();
	afx_msg void OnButtonCustomGraph();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP();
	void loadPresetList(LocaleMngr const& mngr);
	//! Сравнивает пресеты с текущим и возвращает константу для combobox'a
	int comparePresets() const;
	void copyPreset(int presetNum);
private:
	std::list<ISetting*> settingsList_;
	GraphSettingsSet currentGraphSet_;
	GraphSettingsSet fastGraphSet_;
	GraphSettingsSet performanceGraphSet_;
	GraphSettingsSet maxGraphSet_;
//	GraphSettingsSet extremaleGraphSet_;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GRAPHSETTINGSPAGE_H__63469A7F_ED16_419E_939F_628D9AFB0804__INCLUDED_)
