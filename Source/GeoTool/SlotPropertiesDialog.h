#if !defined(AFX_SLOTPROPERTIESDIALOG_H__464C62C0_A86E_4890_9D23_C749697DF733__INCLUDED_)
#define AFX_SLOTPROPERTIESDIALOG_H__464C62C0_A86E_4890_9D23_C749697DF733__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SlotPropertiesDialog.h : header file
//
#include <TextureNode.h>
#include <Empty.hpp>
#include <ExtendedWorldLocation3D.hpp>
#include <Clear3D.hpp>
#include <Primitive3D.hpp>
#include <FlatModelTexture3D.hpp>
#include <StratumLocation3D.hpp>
#include <SphereLocation3D.hpp>
#include <LayingLocation3D.hpp>
#include <XMLUtils.hpp>
#include "Slot.h"
#include "PreviewDialog.h"
#include "TreeManager.h"

/////////////////////////////////////////////////////////////////////////////
// CSlotPropertiesDialog dialog

class CSlotPropertiesDialog : public CDialog
{
// Construction
public:
	CSlotPropertiesDialog(CWnd* pParent = NULL);   // standard constructor

	bool updateLocationControls(tx3d::Location3D* location);
	void updateStratumControls(tx3d::StratumLocation3D* stratumLocation);
	void updateSphereControls(tx3d::SphereLocation3D* sphereLocation);
	void updateLayingControls(tx3d::LayingLocation3D* layingLocation);
	void updateWorldControls(tx3d::WorldLocation3D* worldLocation);
	void updateExtendedWorldControls(tx3d::ExtendedWorldLocation3D* xWorldLocation);

	void onActionSelected(ActionNode::ActionType type);

	bool isLocationSelected() {
		return locationSelected;
	}
// Dialog Data

	//{{AFX_DATA(CSlotPropertiesDialog)
	enum { IDD = IDD_SLOT_PROPERTIES };
	CButton	textureGroupBox;
	CButton	locationGroupBox;
	CStatic	locationPlaneLabel;
	CEdit	radiusEdit;
	CStatic	previewLabel;
	CStatic	planeLabel;
	CStatic	plane2Label;
	CStatic	shiftLabel;
	CStatic	scaleLabel;
	CEdit	fadingEdit;
	CStatic	fadingLabel;
	CStatic	fadeLabel;
	CEdit	fadeEdit;
	CStatic	radiusLabel;
	CStatic	nearBoundLabel;
	CStatic	gapLabel;
	CStatic	farBoundLabel;
	CStatic	centerLabel;
	CStatic	brickZLabel;
	CStatic	brickYLabel;
	CStatic	brickXLabel;
	CStatic	brickLabel;
	CEdit	gapZEdit;
	CEdit	gapYEdit;
	CEdit	gapXEdit;
	CEdit	nearBoundEdit;
	CEdit	farBoundEdit;
	CEdit	brickZEdit;
	CEdit	brickYEdit;
	CEdit	brickXEdit;
	CButton	isBrickCheck;
	CEdit	persistenceEdit;
	CStatic	persistenceLabel;
	CStatic	octaveCountLabel;
	CEdit	octaveCountEdit;
	CButton	planeXRadio;
	CButton	planeYRadio;
	CButton	planeZRadio;
	CButton	plane2XRadio;
	CButton	plane2YRadio;
	CButton	plane2ZRadio;
	CButton	locationPlaneXRadio;
	CButton	locationPlaneYRadio;
	CButton	locationPlaneZRadio;
	CTreeCtrl	mainTree;
	CStatic	toLabel;
	CStatic	stepLabel;
	CStatic	shiftZLabel;
	CStatic	shiftYLabel;
	CStatic	shiftXLabel;
	CStatic	ringCountLabel;
	CStatic	gradLabel;
	CStatic	fromZLabel;
	CStatic	fromYLabel;
	CStatic	fromXLabel;
	CStatic	fromLabel;
	CStatic	fgColorLabel;
	CStatic	bgColorLabel;
	CEdit	toZEdit;
	CEdit	toYEdit;
	CEdit	toXEdit;
	CEdit	stepEdit;
	CEdit	shiftZEdit;
	CEdit	shiftYEdit;
	CEdit	shiftXEdit;
	CEdit	scaleZEdit;
	CEdit	scaleYEdit;
	CEdit	scaleXEdit;
	CEdit	gradEdit;
	CEdit	fromZEdit;
	CEdit	fromYEdit;
	CEdit	fromXEdit;
	CButton	backInterpolatorCheck;
	CButton	fgColorButton;
	CButton	bgColorButton;
	float	scaleXValue;
	float	fromXValue;
	float	fromYValue;
	float	fromZValue;
	float	gradValue;
	float	persistenceValue;
	float	scaleYValue;
	float	scaleZValue;
	float	shiftXValue;
	float	shiftYValue;
	float	shiftZValue;
	float	stepValue;
	float	toXValue;
	float	toYValue;
	float	toZValue;
	int		octaveCountValue;
	float	brickXValue;
	float	brickYValue;
	float	brickZValue;
	float	farBoundValue;
	float	gapXValue;
	float	gapYValue;
	float	gapZValue;
	float	nearBoundValue;
	float	fadeValue;
	float	fadingValue;
	float	radiusValue;
	//}}AFX_DATA

	bool slotUnchecking(Slot* slot, bool isMixer);
	void slotChecked(Slot* slot, bool isMixer, HTREEITEM texture, HTREEITEM location);

	TreeManager* treeManager;
	CMenu modelMenu;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSlotPropertiesDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	void initColorButtons();
	void updateFgColorButton();
	void updateBgColorButton();
	void updateColorButton(CButton* btn, tx3d::Vector3D* color);
	void chooseColor(CButton* btn, tx3d::Vector3D* color);
	void updatePreviewLabel(TextureNode* txNode);
	
	//---Visibility-----------------------
	void setFGColorVisible(int cmdShow);
	void setBGColorVisible(int cmdShow);
	void setShiftScaleVisible(int cmdShow);
	void setWoodPropsVisible(int cmdShow);
	void setSandPropsVisible(int cmdShow);
	void setPlaneVisible(int cmdShow);
	void setFlatPropsVisible(int cmdShow);
	void setFromToVisible(int cmdShow, bool fromOnly = false);
	void setPrimitivePropsVisible(int cmdShow);
	void hideAllTextureControls();

	void setExtendedWorldPropsVisible(int cmdShow);
	void setWorldPropsVisible(int cmdShow);
	void setLayingPropsVisible(int cmdShow);
	void setStratumPropsVisible(int cmdShow);
	void setSpherePropsVisible(int cmdShow);
	void hideAllLocationControls();

	//---Parsing--------------------------
	void clearTextureControls();

	bool textureToControls(tx3d::Texture3D* tx, bool isMixer);
	void clearToControls(tx3d::Clear3D* clearTx, bool isMixer);
	void primitiveToControls(tx3d::Primitive3D* primitiveTx, bool isMixer);
	void flatModelToControls(tx3d::FlatModelTexture3D* flatTx, bool isMixer);
	void emptyToControls(tx3d::Empty* empty, bool isMixer);
	void colorizerToControls(tx3d::Colorizer3D* colorizer, bool isMixer);

	void clearLocationControls();

	void extendedWorldToControls(tx3d::ExtendedWorldLocation3D* xWorldLocation);
	bool locationToControls(tx3d::Location3D* location);
	void worldToControls(tx3d::WorldLocation3D* worldLocation);
	void stratumToControls(tx3d::StratumLocation3D* stratumLocation);
	void sphereToControls(tx3d::SphereLocation3D* sphereLocation);
	void layingToControls(tx3d::LayingLocation3D* layingLocation);

	//---Serializing--------------------------
	tx3d::Texture3D* controlsToTexture(tx3d::Texture3D* tx, bool isMixer);
	void controlsToClear(tx3d::Clear3D* clearTx, bool isMixer);
	void controlsToPrimitive(tx3d::Primitive3D* primitiveTx, bool isMixer);
	tx3d::Texture3D* controlsToFlatModel(tx3d::FlatModelTexture3D* flatTx, bool isMixer);
	void controlsToEmpty(tx3d::Empty* empty, bool isMixer);
	tx3d::Colorizer3D* planeToColorizer(string name, char plane);
	tx3d::Colorizer3D* controlsToColorizer(tx3d::Colorizer3D* colorizer, bool isMixer);

	tx3d::Location3D* controlsToLocation(tx3d::Location3D* location);
	void controlsToWorld(tx3d::WorldLocation3D* worldLocation);
	void controlsToSphere(tx3d::SphereLocation3D* sphereLocation);
	void controlsToLaying(tx3d::LayingLocation3D* layingLocation);
	tx3d::Location3D* controlsToStratum(tx3d::StratumLocation3D* stratumLocation);
	void controlsToExtendedWorld(tx3d::ExtendedWorldLocation3D* xWorldLocation);

	//-----------------------------------------

	static void floatToEdit(float f, CEdit* edit) {
		edit->SetWindowText(tx3d::XMLUtils::floatToString(f).c_str());
	}

	static void intToEdit(int i, CEdit* edit) {
		edit->SetWindowText(tx3d::XMLUtils::intToString(i).c_str());
	}

	static float editToFloat(const CEdit& edit) {
		char floatBuffer[31];
		edit.GetWindowText(floatBuffer, 31);
		return atof(floatBuffer);
	}

	tx3d::Vector3D fgColor;
	tx3d::Vector3D bgColor;

	unsigned char* bits;

	CColorDialog colorChooser;

	bool locationSelected;

	bool controlsIsUpdating;
	bool shouldValidate;

	tx3d::Texture3D* currentTx;
	tx3d::Location3D* currentLc;

	HTREEITEM markedModelNode;

	// Generated message map functions
	//{{AFX_MSG(CSlotPropertiesDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnButtonFg();
	afx_msg void OnButtonBg();
	afx_msg void OnDblclkTree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchangedTree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnUpdateLocationControls();
	afx_msg void OnRclickTree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnAddModelTx();
	afx_msg void OnDeleteModelTx();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SLOTPROPERTIESDIALOG_H__464C62C0_A86E_4890_9D23_C749697DF733__INCLUDED_)
