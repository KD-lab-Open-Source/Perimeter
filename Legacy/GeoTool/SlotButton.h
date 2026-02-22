#if !defined(AFX_SLOTBUTTTON_H__4E0A2CDA_306B_4E11_8A86_73DCF353C1F0__INCLUDED_)
#define AFX_SLOTBUTTTON_H__4E0A2CDA_306B_4E11_8A86_73DCF353C1F0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SlotButton.h : header file
//

#include "Slot.h"

/////////////////////////////////////////////////////////////////////////////
// CSlotButton window

class CSlotButton : public CButton
{
// Construction
public:
	CSlotButton();

// Attributes
public:
	
	bool isMixerButton();
	HTREEITEM getTextureItem();
	HTREEITEM getLocationItem();
	Slot* getSlot();

	void setMixer(bool isMixer);
	void setTextureItem(HTREEITEM textureItem);
	void setLocationItem(HTREEITEM locationItem);
	void setSlot(Slot* slot);

	static const COLORREF PAINTER_BG_COLOR;
	static const COLORREF PAINTER_FG_COLOR;
	static const COLORREF MIXER_BG_COLOR;
	static const COLORREF MIXER_FG_COLOR;
	static const COLORREF PAINTER_BG_COLOR_HI;
	static const COLORREF PAINTER_FG_COLOR_HI;
	static const COLORREF MIXER_BG_COLOR_HI;
	static const COLORREF MIXER_FG_COLOR_HI;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSlotButton)
	public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDIS);
	protected:
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSlotButton();

	// Generated message map functions
protected:

	void updateText();

	bool isMixer;
	HTREEITEM textureItem;
	HTREEITEM locationItem;
	Slot* slot;

	//{{AFX_MSG(CSlotButton)
	afx_msg void OnClicked();
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SLOTBUTTTON_H__4E0A2CDA_306B_4E11_8A86_73DCF353C1F0__INCLUDED_)
