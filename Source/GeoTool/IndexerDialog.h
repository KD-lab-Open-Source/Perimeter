#if !defined(AFX_INDEXERDIALOG_H__D33DDC19_F817_4D75_B712_B699FF8F0C37__INCLUDED_)
#define AFX_INDEXERDIALOG_H__D33DDC19_F817_4D75_B712_B699FF8F0C37__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// IndexerDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CIndexerDialog dialog

class CIndexerDialog : public CDialog
{
// Construction
public:
	CIndexerDialog(CWnd* pParent = NULL);   // standard constructor

	tx3d::Vector3D getStep();
	tx3d::Vector3D getFrom();
	tx3d::Vector3D getTo();
	float getTreshold();

// Dialog Data
	//{{AFX_DATA(CIndexerDialog)
	enum { IDD = IDD_INDEXER_DIALOG };
	int		indexerFromX;
	int		indexerFromY;
	int		indexerFromZ;
	int		indexerStepX;
	int		indexerStepY;
	int		indexerStepZ;
	int		indexerToX;
	int		indexerToY;
	int		indexerToZ;
	float	indexerTreshold;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIndexerDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CIndexerDialog)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_INDEXERDIALOG_H__D33DDC19_F817_4D75_B712_B699FF8F0C37__INCLUDED_)
