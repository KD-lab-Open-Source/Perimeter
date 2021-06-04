#if !defined(AFX_GRCUSTOMSETTINGSDLG_H__3A703316_E231_42C8_B6AB_E3D366768CE5__INCLUDED_)
#define AFX_GRCUSTOMSETTINGSDLG_H__3A703316_E231_42C8_B6AB_E3D366768CE5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// GrCustomSettingsDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// GrCustomSettingsDlg dialog

#include "Settings/GraphSettingsSet.h"

class GrCustomSettingsDlg : public CDialog
{
// Construction
public:
	GrCustomSettingsDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(GrCustomSettingsDlg)
	enum { IDD = IDD_DIALOG_CUSTOM_GRAPHICS };
	//}}AFX_DATA

	void reloadCotrolCaption(UINT ctrlId, LPCTSTR ctrlPostfix);

	void setGraphicSet(GraphSettingsSet const& set);
	GraphSettingsSet const& getGraphicSet() const;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(GrCustomSettingsDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(GrCustomSettingsDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP();
	GraphSettingsSet graphSettingsSet_;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GRCUSTOMSETTINGSDLG_H__3A703316_E231_42C8_B6AB_E3D366768CE5__INCLUDED_)
