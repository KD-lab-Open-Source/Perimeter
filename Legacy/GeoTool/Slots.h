#if !defined(AFX_SLOTS_H__385294E3_A1B7_4084_9802_9308C12F2E48__INCLUDED_)
#define AFX_SLOTS_H__385294E3_A1B7_4084_9802_9308C12F2E48__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Slots.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSlots dialog
#include "LocationUpdater.h"
#include "SlotMenu.h"
#include "SlotManager.h"
#include "SlotButton.h"

class CSlots : public CDialog, public LocationUpdater
{
// Construction
public:
	CSlots(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSlots)
	enum { IDD = IDD_SLOTS };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	CSlotButton* addNewButton();

	void textureSelected(tx3d::Texture3D* texture, HTREEITEM textureItem);
	void locationSelected(tx3d::Location3D* location, HTREEITEM locationItem);
	void actionSelected(ActionNode::ActionType actionType);

	ActionNode::ActionType getSelectedActionType();
	HTREEITEM getSelectedSlotTextureItem();
	HTREEITEM getSelectedSlotLocationItem();
	bool isSelectedSlotMixer();
	Slot* getSelectedSlot();

	void init(int buttonCount);

	CSlotButton* getSelectedButton();
	void setSelectedButton(CSlotButton* btn);
	void showPopup(CSlotButton* btn, CPoint point);

	bool updateSelectedSlot();
	tx3d::Texture3D* assembleTexture();
	bool parseTexture(tx3d::Texture3D* tx);

	void update(const Vect3f& point, const Vect3f& direction, const Vect3f& direction2, ConstraintType type);
	void setLocationsVisible(bool visible);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSlots)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	static void setColor(CSlotButton* btn, bool selected);

	SlotManager slotManager;
	CSlotButton* selectedButton;

	CSlotMenu slotMenu;

	list<CSlotButton*> buttons;
	int buttonCount;
	int lastID;

	static const int BUTTON_WIDTH;
	static const int BUTTON_HEIGHT;

	// Generated message map functions
	//{{AFX_MSG(CSlots)
	virtual BOOL OnInitDialog();
	afx_msg void OnPainterSlot();
	afx_msg void OnMixerSlot();
	afx_msg void OnClearSlot();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SLOTS_H__385294E3_A1B7_4084_9802_9308C12F2E48__INCLUDED_)
