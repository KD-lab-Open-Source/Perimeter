// ConfiguratorDlg.h : header file
//

#if !defined(AFX_CONFIGURATORDLG_H__8B48B866_F951_4EC6_9E8F_1CDB56EEA19A__INCLUDED_)
#define AFX_CONFIGURATORDLG_H__8B48B866_F951_4EC6_9E8F_1CDB56EEA19A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CConfiguratorDlg dialog

class CConfiguratorDlg : public CDialog
{
// Construction
public:
	CConfiguratorDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CConfiguratorDlg)
	enum { IDD = IDD_CONFIGURATOR_DIALOG };
	CComboBox	resolutionWnd_;
	CComboBox	languageWnd_;
	int		multiProc_;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CConfiguratorDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL


	void loadLanguages();
	void loadResolutions();
	void readCurrentLanguage();
	void readCurrentResolution();
	void readMultiproc();

	CString readLanguageValue();

	void saveOptions();
	void saveCurrentLanguage();
	void saveResolution();
	void saveMultiproc();

	void writeLanguageValue(CString const& language);
	void writeMultiproc(int value);
// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CConfiguratorDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CONFIGURATORDLG_H__8B48B866_F951_4EC6_9E8F_1CDB56EEA19A__INCLUDED_)
