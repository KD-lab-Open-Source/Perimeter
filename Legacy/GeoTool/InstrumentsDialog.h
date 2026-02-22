#if !defined(AFX_INSTRUMENTSDIALOG_H__114900DD_B992_4384_9A76_68D5EAD734AE__INCLUDED_)
#define AFX_INSTRUMENTSDIALOG_H__114900DD_B992_4384_9A76_68D5EAD734AE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// InstrumentsDialog.h : header file
//

#include "InputHandler.h"
#include "Runtime.h"

/////////////////////////////////////////////////////////////////////////////
// CInstrumentsDialog dialog


class CInstrumentsDialog : public CDialog, public InputHandler
{
// Construction
public:

	typedef enum {
		DIG_INSTRUMENT,
		POINTER_INSTRUMENT
	} InstrumentType;

	CInstrumentsDialog(CWnd* pParent = NULL);   // standard constructor

	void handleInput(Operator* op, float deltaT);

	void setInstrumentSelected(InstrumentType type);

// Dialog Data
	//{{AFX_DATA(CInstrumentsDialog)
	enum { IDD = IDD_INSTRUMENTS };
	CButton	arrowButton;
	CButton	digButton;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CInstrumentsDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	void setInstrumentChecked(int checked, InstrumentType type);
	void updateTexture(Operator* op);
	void inputDisabled(Operator* op);
	void handlePointerInput(Operator* op, float deltaT);
	void handleDigInput(Operator* op, float deltaT);

	InstrumentType selectedInstrument;
	bool textureUpdating;
	bool needUpdateTexture;
	bool isAdjusting;

	// Generated message map functions
	//{{AFX_MSG(CInstrumentsDialog)
	afx_msg void OnPitBrush();
	afx_msg void OnPitBrushBig();
	afx_msg void OnArrowButton();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_INSTRUMENTSDIALOG_H__114900DD_B992_4384_9A76_68D5EAD734AE__INCLUDED_)
