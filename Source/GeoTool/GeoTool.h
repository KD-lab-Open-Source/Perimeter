// GeoTool.h : main header file for the GEOTOOL application
//

#if !defined(AFX_GEOTOOL_H__0EEA8F57_2E88_40A2_8CC2_1F5FAD2A74EB__INCLUDED_)
#define AFX_GEOTOOL_H__0EEA8F57_2E88_40A2_8CC2_1F5FAD2A74EB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols
#include "Runtime.h"       // main symbols
#include "InstrumentsDialog.h"
#include "SlotPropertiesDialog.h"
#include "Slots.h"
#include "IndexerDialog.h"
#include "LoadMapDialog.h"

/////////////////////////////////////////////////////////////////////////////
// CGeoToolApp:
// See GeoTool.cpp for the implementation of this class
//

class CGeoToolApp : public CWinApp
{
public:

	Runtime* runtime;
	CInstrumentsDialog instrumentsDialog;
	CSlotPropertiesDialog slotPropertiesDialog;
	CSlots slotsDialog;
	CLoadMapDialog loadMapDialog;

	CGeoToolApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGeoToolApp)
	public:
	virtual BOOL InitInstance();
	virtual BOOL OnIdle(LONG lCount);
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation
	void setBrush(VMapOperator::BrushType type);

public:
	//{{AFX_MSG(CGeoToolApp)
	afx_msg void OnAppAbout();
	afx_msg void OnLoadMap();
	afx_msg void OnRemoveDamLayer();
	afx_msg void OnPlaneDamLayer();
	afx_msg void OnPlaneDamLocal();
	afx_msg void OnRemoveDamLocal();
	afx_msg void OnShowSlotProperties();
	afx_msg void OnUpdateShowSlotProperties(CCmdUI* pCmdUI);
	afx_msg void OnShowPreview();
	afx_msg void OnUpdateShowPreview(CCmdUI* pCmdUI);
	afx_msg void OnShowSlots();
	afx_msg void OnUpdateShowSlots(CCmdUI* pCmdUI);
	afx_msg void OnLoadTexture();
	afx_msg void OnSaveTexture();
	afx_msg void OnToggleIndexed();
	afx_msg void OnShowInstruments();
	afx_msg void OnUpdateShowInstruments(CCmdUI* pCmdUI);
	afx_msg void OnBtnZoom();
	afx_msg void OnButtonzero();
	afx_msg void OnPublish();
	afx_msg void OnButtonZshiftMinus();
	afx_msg void OnButtonZshiftPlus();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

extern CGeoToolApp theApp;

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GEOTOOL_H__0EEA8F57_2E88_40A2_8CC2_1F5FAD2A74EB__INCLUDED_)
