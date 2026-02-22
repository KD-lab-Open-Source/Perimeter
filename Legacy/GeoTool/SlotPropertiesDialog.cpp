// SlotPropertiesDialog.cpp : implementation file
//

#include "stdafx.h"
#include <StratumXLocation3D.hpp>
#include <StratumYLocation3D.hpp>
#include <StratumZLocation3D.hpp>
#include <FlatModelXTexture3D.hpp>
#include <FlatModelYTexture3D.hpp>
#include <FlatModelZTexture3D.hpp>
#include <Woods.hpp>
#include <Sands.hpp>
#include <Texture3DFactory.hpp>
#include <Interpolator3DFactory.hpp>
#include <Colorizer3DFactory.hpp>

#include "ActionNode.h"
#include "TextureNode.h"
#include "ModelNode.h"
#include "LocationNode.h"
#include "GeoTool.h"
#include "SlotPropertiesDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSlotPropertiesDialog dialog


CSlotPropertiesDialog::CSlotPropertiesDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CSlotPropertiesDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSlotPropertiesDialog)
	scaleXValue = 0.0f;
	fromXValue = 0.0f;
	fromYValue = 0.0f;
	fromZValue = 0.0f;
	gradValue = 0.0f;
	persistenceValue = 0.0f;
	scaleYValue = 0.0f;
	scaleZValue = 0.0f;
	shiftXValue = 0.0f;
	shiftYValue = 0.0f;
	shiftZValue = 0.0f;
	stepValue = 0.0f;
	toXValue = 0.0f;
	toYValue = 0.0f;
	toZValue = 0.0f;
	octaveCountValue = 0;
	brickXValue = 0.0f;
	brickYValue = 0.0f;
	brickZValue = 0.0f;
	farBoundValue = 0.0f;
	gapXValue = 0.0f;
	gapYValue = 0.0f;
	gapZValue = 0.0f;
	nearBoundValue = 0.0f;
	fadeValue = 0.0f;
	fadingValue = 0.0f;
	radiusValue = 0.0f;
	//}}AFX_DATA_INIT

}


void CSlotPropertiesDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSlotPropertiesDialog)
	DDX_Control(pDX, IDC_STATIC_TEXTURE, textureGroupBox);
	DDX_Control(pDX, IDC_STATIC_LOCATION, locationGroupBox);
	DDX_Control(pDX, IDC_STATIC_PLANE_IMAGE3, locationPlaneLabel);
	DDX_Control(pDX, IDC_EDIT_RADIUS, radiusEdit);
	DDX_Control(pDX, IDC_PREVIEW_IMAGE, previewLabel);
	DDX_Control(pDX, IDC_STATIC_PLANE_IMAGE2, planeLabel);
	DDX_Control(pDX, IDC_STATIC_PLANE_IMAGE, plane2Label);
	DDX_Control(pDX, IDC_STATIC_SHIFT_IMAGE, shiftLabel);
	DDX_Control(pDX, IDC_STATIC_SCALE_IMAGE, scaleLabel);
	DDX_Control(pDX, IDC_EDIT_FADING, fadingEdit);
	DDX_Control(pDX, IDC_STATIC_FADING, fadingLabel);
	DDX_Control(pDX, IDC_STATIC_FADE, fadeLabel);
	DDX_Control(pDX, IDC_EDIT_FADE, fadeEdit);
	DDX_Control(pDX, IDC_STATIC_RADIUS, radiusLabel);
	DDX_Control(pDX, IDC_STATIC_NEAR_BOUND, nearBoundLabel);
	DDX_Control(pDX, IDC_STATIC_GAP, gapLabel);
	DDX_Control(pDX, IDC_STATIC_FAR_BOUND, farBoundLabel);
	DDX_Control(pDX, IDC_STATIC_CENTER, centerLabel);
	DDX_Control(pDX, IDC_STATIC_BRICK_Z, brickZLabel);
	DDX_Control(pDX, IDC_STATIC_BRICK_Y, brickYLabel);
	DDX_Control(pDX, IDC_STATIC_BRICK_X, brickXLabel);
	DDX_Control(pDX, IDC_STATIC_BRICK, brickLabel);
	DDX_Control(pDX, IDC_EDIT_GAP_Z, gapZEdit);
	DDX_Control(pDX, IDC_EDIT_GAP_Y, gapYEdit);
	DDX_Control(pDX, IDC_EDIT_GAP_X, gapXEdit);
	DDX_Control(pDX, IDC_EDIT_NEAR_BOUND, nearBoundEdit);
	DDX_Control(pDX, IDC_EDIT_FAR_BOUND, farBoundEdit);
	DDX_Control(pDX, IDC_EDIT_BRICK_Z, brickZEdit);
	DDX_Control(pDX, IDC_EDIT_BRICK_Y, brickYEdit);
	DDX_Control(pDX, IDC_EDIT_BRICK_X, brickXEdit);
	DDX_Control(pDX, IDC_CHECK_IS_BRICK, isBrickCheck);
	DDX_Control(pDX, IDC_EDIT_PERSISTENCE, persistenceEdit);
	DDX_Control(pDX, IDC_STATIC_PERSISTENCE, persistenceLabel);
	DDX_Control(pDX, IDC_STATIC_OCTAVE_COUNT, octaveCountLabel);
	DDX_Control(pDX, IDC_EDIT_OCTAVE_COUNT, octaveCountEdit);
	DDX_Control(pDX, IDC_RADIO_PLANE_X, planeXRadio);
	DDX_Control(pDX, IDC_RADIO_PLANE_Y, planeYRadio);
	DDX_Control(pDX, IDC_RADIO_PLANE_Z, planeZRadio);
	DDX_Control(pDX, IDC_RADIO_PLANE_X2, plane2XRadio);
	DDX_Control(pDX, IDC_RADIO_PLANE_Y2, plane2YRadio);
	DDX_Control(pDX, IDC_RADIO_PLANE_Z2, plane2ZRadio);
	DDX_Control(pDX, IDC_RADIO_PLANE_LOCATION_X, locationPlaneXRadio);
	DDX_Control(pDX, IDC_RADIO_PLANE_LOCATION_Y, locationPlaneYRadio);
	DDX_Control(pDX, IDC_RADIO_PLANE_LOCATION_Z, locationPlaneZRadio);
	DDX_Control(pDX, IDC_TREE, mainTree);
	DDX_Control(pDX, IDC_STATIC_TO, toLabel);
	DDX_Control(pDX, IDC_STATIC_STEP, stepLabel);
	DDX_Control(pDX, IDC_STATIC_SHIFT_Z, shiftZLabel);
	DDX_Control(pDX, IDC_STATIC_SHIFT_Y, shiftYLabel);
	DDX_Control(pDX, IDC_STATIC_SHIFT_X, shiftXLabel);
	DDX_Control(pDX, IDC_STATIC_RING_COUNT, ringCountLabel);
	DDX_Control(pDX, IDC_STATIC_GRADATION, gradLabel);
	DDX_Control(pDX, IDC_STATIC_FROM_Z, fromZLabel);
	DDX_Control(pDX, IDC_STATIC_FROM_Y, fromYLabel);
	DDX_Control(pDX, IDC_STATIC_FROM_X, fromXLabel);
	DDX_Control(pDX, IDC_STATIC_FROM, fromLabel);
	DDX_Control(pDX, IDC_STATIC_FG, fgColorLabel);
	DDX_Control(pDX, IDC_STATIC_BG, bgColorLabel);
	DDX_Control(pDX, IDC_EDIT_TO_Z, toZEdit);
	DDX_Control(pDX, IDC_EDIT_TO_Y, toYEdit);
	DDX_Control(pDX, IDC_EDIT_TO_X, toXEdit);
	DDX_Control(pDX, IDC_EDIT_STEP, stepEdit);
	DDX_Control(pDX, IDC_EDIT_SHIFT_Z, shiftZEdit);
	DDX_Control(pDX, IDC_EDIT_SHIFT_Y, shiftYEdit);
	DDX_Control(pDX, IDC_EDIT_SHIFT_X, shiftXEdit);
	DDX_Control(pDX, IDC_EDIT_SCALE_Z, scaleZEdit);
	DDX_Control(pDX, IDC_EDIT_SCALE_Y, scaleYEdit);
	DDX_Control(pDX, IDC_EDIT_SCALE_X, scaleXEdit);
	DDX_Control(pDX, IDC_EDIT_GRADATION, gradEdit);
	DDX_Control(pDX, IDC_EDIT_FROM_Z, fromZEdit);
	DDX_Control(pDX, IDC_EDIT_FROM_Y, fromYEdit);
	DDX_Control(pDX, IDC_EDIT_FROM_X, fromXEdit);
	DDX_Control(pDX, IDC_CHECK_BACK, backInterpolatorCheck);
	DDX_Control(pDX, IDC_BUTTON_FG, fgColorButton);
	DDX_Control(pDX, IDC_BUTTON_BG, bgColorButton);
	DDX_Text(pDX, IDC_EDIT_SCALE_X, scaleXValue);
	DDX_Text(pDX, IDC_EDIT_FROM_X, fromXValue);
	DDX_Text(pDX, IDC_EDIT_FROM_Y, fromYValue);
	DDX_Text(pDX, IDC_EDIT_FROM_Z, fromZValue);
	DDX_Text(pDX, IDC_EDIT_GRADATION, gradValue);
	DDX_Text(pDX, IDC_EDIT_PERSISTENCE, persistenceValue);
	DDV_MinMaxFloat(pDX, persistenceValue, -1.f, 1.f);
	DDX_Text(pDX, IDC_EDIT_SCALE_Y, scaleYValue);
	DDX_Text(pDX, IDC_EDIT_SCALE_Z, scaleZValue);
	DDX_Text(pDX, IDC_EDIT_SHIFT_X, shiftXValue);
	DDX_Text(pDX, IDC_EDIT_SHIFT_Y, shiftYValue);
	DDX_Text(pDX, IDC_EDIT_SHIFT_Z, shiftZValue);
	DDX_Text(pDX, IDC_EDIT_STEP, stepValue);
	DDX_Text(pDX, IDC_EDIT_TO_X, toXValue);
	DDX_Text(pDX, IDC_EDIT_TO_Y, toYValue);
	DDX_Text(pDX, IDC_EDIT_TO_Z, toZValue);
	DDX_Text(pDX, IDC_EDIT_OCTAVE_COUNT, octaveCountValue);
	DDV_MinMaxInt(pDX, octaveCountValue, 1, 4);
		DDX_Text(pDX, IDC_EDIT_BRICK_X, brickXValue);
		DDX_Text(pDX, IDC_EDIT_BRICK_Y, brickYValue);
		DDX_Text(pDX, IDC_EDIT_BRICK_Z, brickZValue);
		DDX_Text(pDX, IDC_EDIT_FAR_BOUND, farBoundValue);
		DDX_Text(pDX, IDC_EDIT_GAP_X, gapXValue);
		DDX_Text(pDX, IDC_EDIT_GAP_Y, gapYValue);
		DDX_Text(pDX, IDC_EDIT_GAP_Z, gapZValue);
		DDX_Text(pDX, IDC_EDIT_NEAR_BOUND, nearBoundValue);
	DDX_Text(pDX, IDC_EDIT_FADE, fadeValue);
	DDX_Text(pDX, IDC_EDIT_FADING, fadingValue);
	DDX_Text(pDX, IDC_EDIT_RADIUS, radiusValue);
	//}}AFX_DATA_MAP
/*	if (shouldValidate) {
	} else {
		brickXValue = editToFloat(brickXEdit);
		brickYValue = editToFloat(brickYEdit);
		brickZValue = editToFloat(brickZEdit);
		gapXValue = editToFloat(gapXEdit);
		gapYValue = editToFloat(gapYEdit);
		gapZValue = editToFloat(gapZEdit);
		nearBoundValue = editToFloat(nearBoundEdit);
		farBoundValue = editToFloat(farBoundEdit);
		radiusValue = editToFloat(radiusEdit);
	}*/
}


BEGIN_MESSAGE_MAP(CSlotPropertiesDialog, CDialog)
	//{{AFX_MSG_MAP(CSlotPropertiesDialog)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BUTTON_FG, OnButtonFg)
	ON_BN_CLICKED(IDC_BUTTON_BG, OnButtonBg)
	ON_NOTIFY(NM_DBLCLK, IDC_TREE, OnDblclkTree)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE, OnSelchangedTree)
	ON_BN_CLICKED(IDC_CHECK_IS_BRICK, OnUpdateLocationControls)
	ON_NOTIFY(NM_RCLICK, IDC_TREE, OnRclickTree)
	ON_EN_CHANGE(IDC_EDIT_BRICK_X, OnUpdateLocationControls)
	ON_EN_CHANGE(IDC_EDIT_BRICK_Y, OnUpdateLocationControls)
	ON_EN_CHANGE(IDC_EDIT_BRICK_Z, OnUpdateLocationControls)
	ON_EN_CHANGE(IDC_EDIT_FAR_BOUND, OnUpdateLocationControls)
	ON_EN_CHANGE(IDC_EDIT_GAP_X, OnUpdateLocationControls)
	ON_EN_CHANGE(IDC_EDIT_GAP_Y, OnUpdateLocationControls)
	ON_EN_CHANGE(IDC_EDIT_GAP_Z, OnUpdateLocationControls)
	ON_EN_CHANGE(IDC_EDIT_NEAR_BOUND, OnUpdateLocationControls)
	ON_BN_CLICKED(IDC_RADIO_PLANE_LOCATION_X, OnUpdateLocationControls)
	ON_BN_CLICKED(IDC_RADIO_PLANE_LOCATION_Y, OnUpdateLocationControls)
	ON_BN_CLICKED(IDC_RADIO_PLANE_LOCATION_Z, OnUpdateLocationControls)
	ON_EN_CHANGE(IDC_EDIT_RADIUS, OnUpdateLocationControls)
	ON_COMMAND(ID_ADD_MODEL_TX, OnAddModelTx)
	ON_COMMAND(ID_DELETE_MODEL_TX, OnDeleteModelTx)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSlotPropertiesDialog message handlers

BOOL CSlotPropertiesDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	locationSelected = false;
	currentTx = 0;
	currentLc = 0;
	controlsIsUpdating = false;
	shouldValidate = true;

	treeManager = new TreeManager(&mainTree);
	treeManager->fillTree();
	initColorButtons();
	colorChooser.m_cc.Flags |= CC_FULLOPEN | CC_RGBINIT;

	VERIFY( modelMenu.LoadMenu(IDR_MODELMENU) );

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CSlotPropertiesDialog::initColorButtons() {

	CBitmap fgBitmap;
	fgBitmap.LoadBitmap(IDB_COLORPICKER);
	fgColorButton.SetBitmap(fgBitmap);
	fgBitmap.Detach();

	CBitmap bgBitmap;
	bgBitmap.LoadBitmap(IDB_COLORPICKER);
	bgColorButton.SetBitmap(bgBitmap);

	bits = new unsigned char[32 * 16 * 4];
	bgBitmap.GetBitmapBits(32 * 16 * 4, bits);

	bgBitmap.Detach();


	updateFgColorButton();
	updateBgColorButton();
}

void CSlotPropertiesDialog::OnButtonFg() 
{
	chooseColor(&fgColorButton, &fgColor);
}

void CSlotPropertiesDialog::OnButtonBg() 
{
	chooseColor(&bgColorButton, &bgColor);
}

void CSlotPropertiesDialog::chooseColor(CButton* btn, tx3d::Vector3D* color) {
	colorChooser.m_cc.rgbResult = 
		RGB(
			tx3d::Texture3DUtils::round(255 * color->x),
			tx3d::Texture3DUtils::round(255 * color->y),
			tx3d::Texture3DUtils::round(255 * color->z)
			);

	if (colorChooser.DoModal() == IDOK) {
		COLORREF choosedColor = colorChooser.GetColor();
		color->x = GetRValue(choosedColor) / 255.0f;
		color->y = GetGValue(choosedColor) / 255.0f;
		color->z = GetBValue(choosedColor) / 255.0f;
		updateColorButton(btn, color);
	}
}

void CSlotPropertiesDialog::updateFgColorButton() {
	updateColorButton(&fgColorButton, &fgColor);
}

void CSlotPropertiesDialog::updateBgColorButton() {
	updateColorButton(&bgColorButton, &bgColor);
}

void CSlotPropertiesDialog::updateColorButton(CButton* btn, tx3d::Vector3D* color) {
	for (int y = 32 * 4; y < 32 * 15 * 4; y += 32 * 4) {
		for (int x = 4; x < 31 * 4; x += 4) {
			bits[y + x] = tx3d::Texture3DUtils::round(255 * color->z);
			bits[y + x + 1] = tx3d::Texture3DUtils::round(255 * color->y);
			bits[y + x + 2] = tx3d::Texture3DUtils::round(255 * color->x);
			bits[y + x + 3] = 0;
		}
	}
	CBitmap* temp;
	temp = CBitmap::FromHandle(btn->GetBitmap());
	temp->SetBitmapBits(32 * 16 * 4, bits);
	btn->Invalidate();
}

void CSlotPropertiesDialog::OnDestroy() 
{
	CDialog::OnDestroy();

	delete [] bits;	
	delete treeManager;
}

void CSlotPropertiesDialog::clearTextureControls() {
	floatToEdit(0.0f, &shiftXEdit);
	floatToEdit(0.0f, &shiftYEdit);
	floatToEdit(0.0f, &shiftZEdit);
	floatToEdit(0.0f, &scaleXEdit);
	floatToEdit(0.0f, &scaleYEdit);
	floatToEdit(0.0f, &scaleZEdit);

	floatToEdit(0.0f, &fromXEdit);
	floatToEdit(0.0f, &fromYEdit);
	floatToEdit(0.0f, &fromZEdit);
	floatToEdit(0.0f, &toXEdit);
	floatToEdit(0.0f, &toYEdit);
	floatToEdit(0.0f, &toZEdit);

	floatToEdit(0.0f, &stepEdit);
	floatToEdit(0.0f, &gradEdit);

	floatToEdit(0.0f, &persistenceEdit);
	intToEdit(1, &octaveCountEdit);
	plane2XRadio.SetCheck(0);
	plane2YRadio.SetCheck(0);
	plane2ZRadio.SetCheck(0);
	planeXRadio.SetCheck(0);
	planeYRadio.SetCheck(0);
	planeZRadio.SetCheck(0);
}


//---Visibility-Textures--------------

void CSlotPropertiesDialog::setFGColorVisible(int cmdShow) {
	fgColorButton.ShowWindow(cmdShow);
	fgColorLabel.ShowWindow(cmdShow);
}

void CSlotPropertiesDialog::setBGColorVisible(int cmdShow) {
	bgColorButton.ShowWindow(cmdShow);
	bgColorLabel.ShowWindow(cmdShow);
}

void CSlotPropertiesDialog::setShiftScaleVisible(int cmdShow) {
	shiftLabel.ShowWindow(cmdShow);
	shiftXEdit.ShowWindow(cmdShow);
	shiftXLabel.ShowWindow(cmdShow);
	shiftYEdit.ShowWindow(cmdShow);
	shiftYLabel.ShowWindow(cmdShow);
	shiftZEdit.ShowWindow(cmdShow);
	shiftZLabel.ShowWindow(cmdShow);
	scaleLabel.ShowWindow(cmdShow);
	scaleXEdit.ShowWindow(cmdShow);
	scaleYEdit.ShowWindow(cmdShow);
	scaleZEdit.ShowWindow(cmdShow);
}

void CSlotPropertiesDialog::setFromToVisible(int cmdShow, bool fromOnly) {
	fromLabel.ShowWindow(cmdShow);
	fromXEdit.ShowWindow(cmdShow);
	fromXLabel.ShowWindow(cmdShow);
	fromYEdit.ShowWindow(cmdShow);
	fromYLabel.ShowWindow(cmdShow);
	fromZEdit.ShowWindow(cmdShow);
	fromZLabel.ShowWindow(cmdShow);
	if (fromOnly) {
		toLabel.ShowWindow(SW_HIDE);
		toXEdit.ShowWindow(SW_HIDE);
		toYEdit.ShowWindow(SW_HIDE);
		toZEdit.ShowWindow(SW_HIDE);
	} else {
		toLabel.ShowWindow(cmdShow);
		toXEdit.ShowWindow(cmdShow);
		toYEdit.ShowWindow(cmdShow);
		toZEdit.ShowWindow(cmdShow);
	}
}

void CSlotPropertiesDialog::setWoodPropsVisible(int cmdShow) {
	ringCountLabel.ShowWindow(cmdShow);
	stepEdit.ShowWindow(cmdShow);
}

void CSlotPropertiesDialog::setSandPropsVisible(int cmdShow) {
	stepEdit.ShowWindow(cmdShow);
	stepLabel.ShowWindow(cmdShow);
	gradEdit.ShowWindow(cmdShow);
	gradLabel.ShowWindow(cmdShow);
}

void CSlotPropertiesDialog::setPlaneVisible(int cmdShow) {
	planeLabel.ShowWindow(cmdShow);
	planeXRadio.ShowWindow(cmdShow);
	planeYRadio.ShowWindow(cmdShow);
	planeZRadio.ShowWindow(cmdShow);
}

void CSlotPropertiesDialog::setFlatPropsVisible(int cmdShow) {
	plane2Label.ShowWindow(cmdShow);
	plane2XRadio.ShowWindow(cmdShow);
	plane2YRadio.ShowWindow(cmdShow);
	plane2ZRadio.ShowWindow(cmdShow);
}

void CSlotPropertiesDialog::setPrimitivePropsVisible(int cmdShow) {
	backInterpolatorCheck.ShowWindow(cmdShow);
	octaveCountEdit.ShowWindow(cmdShow);
	octaveCountLabel.ShowWindow(cmdShow);
	persistenceEdit.ShowWindow(cmdShow);
	persistenceLabel.ShowWindow(cmdShow);
}

void CSlotPropertiesDialog::hideAllTextureControls() {
	previewLabel.ShowWindow(SW_HIDE);
	textureGroupBox.ShowWindow(SW_HIDE);
	setFromToVisible(SW_HIDE, false);
	setFGColorVisible(SW_HIDE);
	setBGColorVisible(SW_HIDE);
	setPlaneVisible(SW_HIDE);
	setFlatPropsVisible(SW_HIDE);
	setShiftScaleVisible(SW_HIDE);
	setPrimitivePropsVisible(SW_HIDE);
	setWoodPropsVisible(SW_HIDE);
	setSandPropsVisible(SW_HIDE);
}


//---Visibility-Locations--------------

void CSlotPropertiesDialog::setLayingPropsVisible(int cmdShow) {
	brickLabel.ShowWindow(cmdShow);
	brickXEdit.ShowWindow(cmdShow);
	brickXLabel.ShowWindow(cmdShow);
	brickYEdit.ShowWindow(cmdShow);
	brickYLabel.ShowWindow(cmdShow);
	brickZEdit.ShowWindow(cmdShow);
	brickZLabel.ShowWindow(cmdShow);
	gapLabel.ShowWindow(cmdShow);
	gapXEdit.ShowWindow(cmdShow);
	gapYEdit.ShowWindow(cmdShow);
	gapZEdit.ShowWindow(cmdShow);

	isBrickCheck.ShowWindow(cmdShow);
}

void CSlotPropertiesDialog::setStratumPropsVisible(int cmdShow) {
	nearBoundEdit.ShowWindow(cmdShow);
	nearBoundLabel.ShowWindow(cmdShow);
	farBoundEdit.ShowWindow(cmdShow);
	farBoundLabel.ShowWindow(cmdShow);

	locationPlaneLabel.ShowWindow(cmdShow);
	locationPlaneXRadio.ShowWindow(cmdShow);
	locationPlaneYRadio.ShowWindow(cmdShow);
	locationPlaneZRadio.ShowWindow(cmdShow);
}

void CSlotPropertiesDialog::setWorldPropsVisible(int cmdShow) {
	fadeLabel.ShowWindow(cmdShow);
	fadeEdit.ShowWindow(cmdShow);
}

void CSlotPropertiesDialog::setExtendedWorldPropsVisible(int cmdShow) {
	fadingLabel.ShowWindow(cmdShow);
	fadingEdit.ShowWindow(cmdShow);
}

void CSlotPropertiesDialog::setSpherePropsVisible(int cmdShow) {
	centerLabel.ShowWindow(cmdShow);
	brickXEdit.ShowWindow(cmdShow);
	brickXLabel.ShowWindow(cmdShow);
	brickYEdit.ShowWindow(cmdShow);
	brickYLabel.ShowWindow(cmdShow);
	brickZEdit.ShowWindow(cmdShow);
	brickZLabel.ShowWindow(cmdShow);

	radiusLabel.ShowWindow(cmdShow);
	radiusEdit.ShowWindow(cmdShow);
}

void CSlotPropertiesDialog::hideAllLocationControls() {
	locationGroupBox.ShowWindow(SW_HIDE);
	setExtendedWorldPropsVisible(SW_HIDE);
	setWorldPropsVisible(SW_HIDE);
	setLayingPropsVisible(SW_HIDE);
	setSpherePropsVisible(SW_HIDE);
	setStratumPropsVisible(SW_HIDE);
}


//---Parsing-Textures--------------

void CSlotPropertiesDialog::emptyToControls(tx3d::Empty* empty, bool isMixer) {
	bgColor = empty->bgColor;
	fgColor = empty->bgColor + empty->diffColor;

	if (isMixer) {

		setBGColorVisible(SW_HIDE);
		setFGColorVisible(SW_HIDE);
		setFromToVisible(SW_SHOW);

		floatToEdit(bgColor.x, &fromXEdit);
		floatToEdit(bgColor.y, &fromYEdit);
		floatToEdit(bgColor.z, &fromZEdit);
		floatToEdit(fgColor.x, &toXEdit);
		floatToEdit(fgColor.y, &toYEdit);
		floatToEdit(fgColor.z, &toZEdit);

	} else {

		setBGColorVisible(SW_SHOW);
		setFGColorVisible(SW_SHOW);
		setFromToVisible(SW_HIDE);

		updateFgColorButton();
		updateBgColorButton();

	}
}

void CSlotPropertiesDialog::colorizerToControls(tx3d::Colorizer3D* colorizer, bool isMixer) {
	tx3d::Empty* empty = dynamic_cast<tx3d::Empty*>(colorizer);
	if (empty) {
		emptyToControls(empty, isMixer);
		tx3d::Wood* wood = dynamic_cast<tx3d::Wood*>(colorizer);
		char end = *(colorizer->getType().end() - 1);
		if (wood) {
			setSandPropsVisible(SW_HIDE);
			setWoodPropsVisible(SW_SHOW);
			setPlaneVisible(SW_HIDE);
			floatToEdit(wood->ringCount, &stepEdit);
		} else if (colorizer->getType() == tx3d::Sand::TYPE_NAME) {
			tx3d::Sand* sand = dynamic_cast<tx3d::Sand*>(colorizer);
			setWoodPropsVisible(SW_HIDE);
			setSandPropsVisible(SW_SHOW);
			setPlaneVisible(SW_HIDE);
			floatToEdit(sand->step, &stepEdit);
			floatToEdit(sand->grad, &gradEdit);
		} else if (end == 'X' || end == 'Y' || end == 'Z') {
			setWoodPropsVisible(SW_HIDE);
			setSandPropsVisible(SW_HIDE);
			setPlaneVisible(SW_SHOW);
			switch (end) {
				case 'X':
					planeXRadio.SetCheck(1);
					break;
				case 'Y':
					planeYRadio.SetCheck(1);
					break;
				case 'Z':
					planeZRadio.SetCheck(1);
					break;
			}
		} else {
			setWoodPropsVisible(SW_HIDE);
			setSandPropsVisible(SW_HIDE);
			setPlaneVisible(SW_HIDE);
		}
	} else {
		setBGColorVisible(SW_HIDE);
		setFGColorVisible(SW_HIDE);
		setFromToVisible(SW_HIDE);
	}
}

void CSlotPropertiesDialog::clearToControls(tx3d::Clear3D* clearTx, bool isMixer) {
	if (isMixer) {
		setFromToVisible(SW_SHOW, true);
		setFGColorVisible(SW_HIDE);
		floatToEdit(clearTx->color.x, &fromXEdit);
		floatToEdit(clearTx->color.y, &fromYEdit);
		floatToEdit(clearTx->color.z, &fromZEdit);
	} else {
		setFGColorVisible(SW_SHOW);
		setFromToVisible(SW_HIDE, false);
		fgColor = clearTx->color;
		updateFgColorButton();
	}

	setBGColorVisible(SW_HIDE);
	setPlaneVisible(SW_HIDE);
	setFlatPropsVisible(SW_HIDE);
	setShiftScaleVisible(SW_HIDE);
	setPrimitivePropsVisible(SW_HIDE);
	setWoodPropsVisible(SW_HIDE);
	setSandPropsVisible(SW_HIDE);
}

void CSlotPropertiesDialog::primitiveToControls(tx3d::Primitive3D* primitiveTx, bool isMixer) {
	setFlatPropsVisible(SW_HIDE);

	colorizerToControls(primitiveTx->getColorizer(), isMixer);

	setShiftScaleVisible(SW_SHOW);
	floatToEdit(primitiveTx->shift.x, &shiftXEdit);
	floatToEdit(primitiveTx->shift.y, &shiftYEdit);
	floatToEdit(primitiveTx->shift.z, &shiftZEdit);
	floatToEdit(primitiveTx->scale.x, &scaleXEdit);
	floatToEdit(primitiveTx->scale.y, &scaleYEdit);
	floatToEdit(primitiveTx->scale.z, &scaleZEdit);

	setPrimitivePropsVisible(SW_SHOW);
	backInterpolatorCheck.SetCheck(
		primitiveTx->interpolator->getType()[0] == 'B' ? 1 : 0 );

	floatToEdit(primitiveTx->persistence, &persistenceEdit);
	intToEdit(primitiveTx->octaveCount, &octaveCountEdit);
}

void CSlotPropertiesDialog::flatModelToControls(tx3d::FlatModelTexture3D* flatTx, bool isMixer) {
	setFlatPropsVisible(SW_SHOW);
	char end = *(flatTx->getType().end() - 1);
	switch (end) {
		case 'X':
			plane2XRadio.SetCheck(1);
			break;
		case 'Y':
			plane2YRadio.SetCheck(1);
			break;
		case 'Z':
			plane2ZRadio.SetCheck(1);
			break;
	}

	setShiftScaleVisible(SW_SHOW);
	floatToEdit(flatTx->shift.x, &shiftXEdit);
	floatToEdit(flatTx->shift.y, &shiftYEdit);
	floatToEdit(flatTx->shift.z, &shiftZEdit);
	floatToEdit(flatTx->scale.x, &scaleXEdit);
	floatToEdit(flatTx->scale.y, &scaleYEdit);
	floatToEdit(flatTx->scale.z, &scaleZEdit);

	setBGColorVisible(SW_HIDE);
	setFGColorVisible(SW_HIDE);
	setFromToVisible(SW_HIDE);
	setWoodPropsVisible(SW_HIDE);
	setSandPropsVisible(SW_HIDE);
	setPlaneVisible(SW_HIDE);
	setPrimitivePropsVisible(SW_HIDE);
}

bool CSlotPropertiesDialog::textureToControls(tx3d::Texture3D* tx, bool isMixer) {
	theApp.slotsDialog.setLocationsVisible(false);
	locationSelected = false;

	previewLabel.ShowWindow(SW_SHOW);
	textureGroupBox.ShowWindow(SW_SHOW);
	hideAllLocationControls();
	clearTextureControls();
	tx3d::Clear3D* clearTx = dynamic_cast<tx3d::Clear3D*>(tx);
	if (clearTx) {
		clearToControls(clearTx, isMixer);
		return true;
	}
	tx3d::Primitive3D* primitiveTx = dynamic_cast<tx3d::Primitive3D*>(tx);
	if (primitiveTx) {
		primitiveToControls(primitiveTx, isMixer);
		return true;
	}
	tx3d::FlatModelTexture3D* flatTx = dynamic_cast<tx3d::FlatModelTexture3D*>(tx);
	if (flatTx) {
		flatModelToControls(flatTx, isMixer);
		return true;
	}
	return false;
}


//---Parsing-Locations--------------

bool CSlotPropertiesDialog::locationToControls(tx3d::Location3D* location) {
	theApp.slotsDialog.setLocationsVisible(true);
	locationSelected = true;

	controlsIsUpdating = true;

	locationGroupBox.ShowWindow(SW_SHOW);
	hideAllTextureControls();
	clearLocationControls();
	tx3d::ExtendedWorldLocation3D* xWorldLoc = dynamic_cast<tx3d::ExtendedWorldLocation3D*>(location);
	if (xWorldLoc) {
		extendedWorldToControls(xWorldLoc);
	} else {
		setExtendedWorldPropsVisible(SW_HIDE);
	}
	tx3d::WorldLocation3D* worldLoc = dynamic_cast<tx3d::WorldLocation3D*>(location);
	if (worldLoc) {
		worldToControls(worldLoc);
		if (worldLoc->getType() == WorldLocation3D::TYPE_NAME) {
			controlsIsUpdating = false;
			return true;
		}
	}
	tx3d::StratumLocation3D* stratumLoc = dynamic_cast<tx3d::StratumLocation3D*>(location);
	if (stratumLoc) {
		stratumToControls(stratumLoc);
		controlsIsUpdating = false;
		return true;
	}
	tx3d::SphereLocation3D* sphereLoc = dynamic_cast<tx3d::SphereLocation3D*>(location);
	if (sphereLoc) {
		sphereToControls(sphereLoc);
		controlsIsUpdating = false;
		return true;
	}
	tx3d::LayingLocation3D* layingLoc = dynamic_cast<tx3d::LayingLocation3D*>(location);
	if (layingLoc) {
		layingToControls(layingLoc);
		controlsIsUpdating = false;
		return true;
	}
	controlsIsUpdating = false;
	return false;
}

void CSlotPropertiesDialog::extendedWorldToControls(
			tx3d::ExtendedWorldLocation3D* xWorldLocation) {

	setExtendedWorldPropsVisible(SW_SHOW);

	updateExtendedWorldControls(xWorldLocation);
}

void CSlotPropertiesDialog::worldToControls(tx3d::WorldLocation3D* worldLocation) {
	setSpherePropsVisible(SW_HIDE);
	setLayingPropsVisible(SW_HIDE);
	setStratumPropsVisible(SW_HIDE);
	setWorldPropsVisible(SW_SHOW);

	updateWorldControls(worldLocation);
}

void CSlotPropertiesDialog::stratumToControls(tx3d::StratumLocation3D* stratumLocation) {
	setSpherePropsVisible(SW_HIDE);
	setLayingPropsVisible(SW_HIDE);
	setStratumPropsVisible(SW_SHOW);

	updateStratumControls(stratumLocation);
}

void CSlotPropertiesDialog::sphereToControls(tx3d::SphereLocation3D* sphereLocation) {
	setLayingPropsVisible(SW_HIDE);
	setStratumPropsVisible(SW_HIDE);
	setSpherePropsVisible(SW_SHOW);

	updateSphereControls(sphereLocation);
}

void CSlotPropertiesDialog::layingToControls(tx3d::LayingLocation3D* layingLocation) {
	setWorldPropsVisible(SW_HIDE);
	setSpherePropsVisible(SW_HIDE);
	setStratumPropsVisible(SW_HIDE);
	setLayingPropsVisible(SW_SHOW);

	updateLayingControls(layingLocation);
}

void CSlotPropertiesDialog::clearLocationControls() {
	floatToEdit(1.0f, &fadingEdit);
	floatToEdit(0.0f, &fadeEdit);

	floatToEdit(0.0f, &brickXEdit);
	floatToEdit(0.0f, &brickYEdit);
	floatToEdit(0.0f, &brickZEdit);
	floatToEdit(0.0f, &gapXEdit);
	floatToEdit(0.0f, &gapYEdit);
	floatToEdit(0.0f, &gapZEdit);

	floatToEdit(0.0f, &farBoundEdit);
	floatToEdit(0.0f, &nearBoundEdit);
	floatToEdit(0.0f, &radiusEdit);
	locationPlaneXRadio.SetCheck(0);
	locationPlaneYRadio.SetCheck(0);
	locationPlaneZRadio.SetCheck(0);
}

bool CSlotPropertiesDialog::updateLocationControls(tx3d::Location3D* location) {
	controlsIsUpdating = true;
	tx3d::ExtendedWorldLocation3D* xWorldLoc = dynamic_cast<tx3d::ExtendedWorldLocation3D*>(location);
	if (xWorldLoc) {
		updateExtendedWorldControls(xWorldLoc);
	}
	tx3d::WorldLocation3D* worldLoc = dynamic_cast<tx3d::WorldLocation3D*>(location);
	if (worldLoc) {
		updateWorldControls(worldLoc);
		if (worldLoc->getType() == WorldLocation3D::TYPE_NAME) {
			controlsIsUpdating = false;
			return true;
		}
	}
	tx3d::StratumLocation3D* stratumLoc = dynamic_cast<tx3d::StratumLocation3D*>(location);
	if (stratumLoc) {
		updateStratumControls(stratumLoc);
		controlsIsUpdating = false;
		return true;
	}
	tx3d::SphereLocation3D* sphereLoc = dynamic_cast<tx3d::SphereLocation3D*>(location);
	if (sphereLoc) {
		updateSphereControls(sphereLoc);
		controlsIsUpdating = false;
		return true;
	}
	tx3d::LayingLocation3D* layingLoc = dynamic_cast<tx3d::LayingLocation3D*>(location);
	if (layingLoc) {
		updateLayingControls(layingLoc);
		controlsIsUpdating = false;
		return true;
	}
	controlsIsUpdating = false;
	return false;
}

void CSlotPropertiesDialog::updateWorldControls(tx3d::WorldLocation3D* worldLocation) {
	floatToEdit(worldLocation->blanking, &fadeEdit);
}

void CSlotPropertiesDialog::updateExtendedWorldControls(tx3d::ExtendedWorldLocation3D* xWorldLocation) {
	floatToEdit(xWorldLocation->fading, &fadingEdit);
}

void CSlotPropertiesDialog::updateStratumControls(tx3d::StratumLocation3D* stratumLocation) {
	char end = *(stratumLocation->getType().end() - 1);
	switch (end) {
		case 'X':
			locationPlaneXRadio.SetCheck(1);
			break;
		case 'Y':
			locationPlaneYRadio.SetCheck(1);
			break;
		case 'Z':
			locationPlaneZRadio.SetCheck(1);
			break;
	}

	floatToEdit(stratumLocation->nearBound, &nearBoundEdit);
	floatToEdit(stratumLocation->farBound, &farBoundEdit);
}

void CSlotPropertiesDialog::updateSphereControls(tx3d::SphereLocation3D* sphereLocation) {
	floatToEdit(sphereLocation->center.x, &brickXEdit);
	floatToEdit(sphereLocation->center.y, &brickYEdit);
	floatToEdit(sphereLocation->center.z, &brickZEdit);
	floatToEdit(sphereLocation->radius, &radiusEdit);
}

void CSlotPropertiesDialog::updateLayingControls(tx3d::LayingLocation3D* layingLocation) {

	floatToEdit(layingLocation->brick.x, &brickXEdit);
	floatToEdit(layingLocation->brick.y, &brickYEdit);
	floatToEdit(layingLocation->brick.z, &brickZEdit);

	floatToEdit(layingLocation->gap.x, &gapXEdit);
	floatToEdit(layingLocation->gap.y, &gapYEdit);
	floatToEdit(layingLocation->gap.z, &gapZEdit);

	isBrickCheck.SetCheck(layingLocation->isBrick);
}


//---Serializing-Textures--------------

tx3d::Colorizer3D* CSlotPropertiesDialog::planeToColorizer(string name, char plane) {
	string type = name;
	if (planeXRadio.GetCheck() == 1) {
		if (plane == 'X') {
			return 0;
		}
		type += 'X';
	} else if (planeYRadio.GetCheck() == 1) {
		if (plane == 'Y') {
			return 0;
		}
		type += 'Y';
	} else {
		if (plane == 'Z') {
			return 0;
		}
		type += 'Z';
	}
	return tx3d::Colorizer3DFactory::createColorizer3DByType(type);
}

tx3d::Colorizer3D* CSlotPropertiesDialog::controlsToColorizer(tx3d::Colorizer3D* colorizer, bool isMixer) {
	tx3d::Colorizer3D* result = 0;
	tx3d::Empty* empty = dynamic_cast<tx3d::Empty*>(colorizer);
	if (empty) {
		tx3d::Wood* wood = dynamic_cast<tx3d::Wood*>(colorizer);
		if (wood) {
			wood->ringCount = stepValue;
		} else if (colorizer->getType() == tx3d::Sand::TYPE_NAME) {
			tx3d::Sand* sand = dynamic_cast<tx3d::Sand*>(colorizer);
			sand->step = stepValue;
			sand->grad = gradValue;
		} else {
			char end = colorizer->getType()[colorizer->getType().length() - 1];
			string coreName = colorizer->getType().substr(0, colorizer->getType().length() - 1);
			result = planeToColorizer(coreName, end);
			if (result) {
				empty = dynamic_cast<tx3d::Empty*>(result);
			}
		}
		controlsToEmpty(empty, isMixer);
	}
	return result;
}

void CSlotPropertiesDialog::controlsToEmpty(tx3d::Empty* empty, bool isMixer) {
	if (isMixer) {
		empty->bgColor.x = fromXValue;
		empty->bgColor.y = fromYValue;
		empty->bgColor.z = fromZValue;
		empty->diffColor.x = toXValue - fromXValue;
		empty->diffColor.y = toYValue - fromYValue;
		empty->diffColor.z = toZValue - fromZValue;
	} else {
		empty->bgColor = bgColor;
		empty->diffColor = fgColor - empty->bgColor;
	}
}

tx3d::Texture3D* CSlotPropertiesDialog::controlsToTexture(tx3d::Texture3D* tx, bool isMixer) {
	tx3d::Primitive3D* primitive = dynamic_cast<tx3d::Primitive3D*>(tx);
	if (primitive) {
		controlsToPrimitive(primitive, isMixer);
		return 0;
	}

	tx3d::Clear3D* clear = dynamic_cast<tx3d::Clear3D*>(tx);
	if (clear) {
		controlsToClear(clear, isMixer);
		return 0;
	}

	tx3d::FlatModelTexture3D* flat = dynamic_cast<tx3d::FlatModelTexture3D*>(tx);
	if (flat) {
		return controlsToFlatModel(flat, isMixer);
	}
	return 0;
}

void CSlotPropertiesDialog::controlsToClear(tx3d::Clear3D* clearTx, bool isMixer) {
	if (isMixer) {
		clearTx->color.x = fromXValue;
		clearTx->color.y = fromYValue;
		clearTx->color.z = fromZValue;
	} else {
		clearTx->color = fgColor;
	}
}

void CSlotPropertiesDialog::controlsToPrimitive(tx3d::Primitive3D* primitiveTx, bool isMixer) {
	tx3d::Colorizer3D* newColorizer = controlsToColorizer(primitiveTx->getColorizer(), isMixer);
	if (newColorizer) {
		primitiveTx->setColorizer(newColorizer);
	}

	primitiveTx->shift.x = shiftXValue;
	primitiveTx->shift.y = shiftYValue;
	primitiveTx->shift.z = shiftZValue;
	primitiveTx->scale.x = scaleXValue;
	primitiveTx->scale.y = scaleYValue;
	primitiveTx->scale.z = scaleZValue;
	char firstChar = primitiveTx->interpolator->getType()[0];
	if (backInterpolatorCheck.GetCheck() == 1 && firstChar != 'B') {
		primitiveTx->interpolator = tx3d::Interpolator3DFactory::getFactory().getInterpolator3DByType(tx3d::BackCubicInterpolator3D::TYPE_NAME);
	} else if (backInterpolatorCheck.GetCheck() == 0 && firstChar == 'B') {
		primitiveTx->interpolator = tx3d::Interpolator3DFactory::getFactory().getInterpolator3DByType(tx3d::CubicInterpolator3D::TYPE_NAME);
	}

	primitiveTx->persistence = persistenceValue;
	primitiveTx->octaveCount = octaveCountValue;
}

tx3d::Texture3D* CSlotPropertiesDialog::controlsToFlatModel(tx3d::FlatModelTexture3D* flatTx, bool isMixer) {
	flatTx->shift.x = shiftXValue;
	flatTx->shift.y = shiftYValue;
	flatTx->shift.z = shiftZValue;
	flatTx->scale.x = scaleXValue;
	flatTx->scale.y = scaleYValue;
	flatTx->scale.z = scaleZValue;

	char end = flatTx->getType()[flatTx->getType().length() - 1];
	if (plane2XRadio.GetCheck() == 1) {
		if (end != 'X') {
			return new tx3d::FlatModelXTexture3D(flatTx);
		}
	}
	if (plane2YRadio.GetCheck() == 1) {
		if (end != 'Y') {
			return new tx3d::FlatModelYTexture3D(flatTx);
		}
	}
	if (plane2ZRadio.GetCheck() == 1) {
		if (end != 'Z') {
			return new tx3d::FlatModelZTexture3D(flatTx);
		}
	}
	return 0;
}


//---Serializing-Location--------------

tx3d::Location3D* CSlotPropertiesDialog::controlsToLocation(tx3d::Location3D* location) {
	tx3d::ExtendedWorldLocation3D* xWorldLoc = dynamic_cast<tx3d::ExtendedWorldLocation3D*>(location);
	if (xWorldLoc) {
		controlsToExtendedWorld(xWorldLoc);
	}

	tx3d::WorldLocation3D* worldLoc = dynamic_cast<tx3d::WorldLocation3D*>(location);
	if (worldLoc) {
		controlsToWorld(worldLoc);
		if (worldLoc->getType() == WorldLocation3D::TYPE_NAME) {
			return 0;
		}
	}

	tx3d::SphereLocation3D* sphereLoc = dynamic_cast<tx3d::SphereLocation3D*>(location);
	if (sphereLoc) {
		controlsToSphere(sphereLoc);
		return 0;
	}

	tx3d::LayingLocation3D* layingLoc = dynamic_cast<tx3d::LayingLocation3D*>(location);
	if (layingLoc) {
		controlsToLaying(layingLoc);
		return 0;
	}

	tx3d::StratumLocation3D* stratumLoc = dynamic_cast<tx3d::StratumLocation3D*>(location);
	if (stratumLoc) {
		return controlsToStratum(stratumLoc);
	}
	return 0;
}

void CSlotPropertiesDialog::controlsToWorld(tx3d::WorldLocation3D* worldLocation) {
	worldLocation->blanking = fadeValue;
}

void CSlotPropertiesDialog::controlsToExtendedWorld(tx3d::ExtendedWorldLocation3D* xWorldLocation) {
	xWorldLocation->fading = fadingValue;
}

void CSlotPropertiesDialog::controlsToSphere(tx3d::SphereLocation3D* sphereLocation) {
	sphereLocation->center.x = brickXValue;
	sphereLocation->center.y = brickYValue;
	sphereLocation->center.z = brickZValue;

	sphereLocation->radius = radiusValue;
}

void CSlotPropertiesDialog::controlsToLaying(tx3d::LayingLocation3D* layingLocation) {
	layingLocation->brick.x = brickXValue;
	layingLocation->brick.y = brickYValue;
	layingLocation->brick.z = brickZValue;

	layingLocation->gap.x = gapXValue;
	layingLocation->gap.y = gapYValue;
	layingLocation->gap.z = gapZValue;
	
	layingLocation->isBrick = (backInterpolatorCheck.GetCheck() == 1);
}

tx3d::Location3D* CSlotPropertiesDialog::controlsToStratum(tx3d::StratumLocation3D* stratumLocation) {
	stratumLocation->nearBound = nearBoundValue;
	stratumLocation->farBound = farBoundValue;

	char end = stratumLocation->getType()[stratumLocation->getType().length() - 1];
	if (locationPlaneXRadio.GetCheck() == 1) {
		if (end != 'X') {
			return new tx3d::StratumXLocation3D(stratumLocation);
		}
	}
	if (locationPlaneYRadio.GetCheck() == 1) {
		if (end != 'Y') {
			return new tx3d::StratumYLocation3D(stratumLocation);
		}
	}
	if (locationPlaneZRadio.GetCheck() == 1) {
		if (end != 'Z') {
			return new tx3d::StratumZLocation3D(stratumLocation);
		}
	}
	return 0;
}

//---Message-Handling--------------

void CSlotPropertiesDialog::OnDblclkTree(NMHDR* pNMHDR, LRESULT* pResult) 
{
	*pResult = 0;
/*
  	POINT Point;
	GetCursorPos(&Point);
	mainTree.ScreenToClient(&Point);
	HTREEITEM item = mainTree.HitTest(Point);
	
	if (item != NULL) {
		Node* node = (Node*) mainTree.GetItemData(item);
		TextureNode* txNode = 0;
		if (node) {
			txNode = dynamic_cast<TextureNode*>(node);
		}

		if (!txNode) {
			HTREEITEM selected = treeManager->getSelectedTexture();
			if (selected != NULL) {
				node = (Node*) mainTree.GetItemData(selected);
				txNode = dynamic_cast<TextureNode*>(node);
			} else {
				//update bitmap with default grays
			}
		}

		if (txNode) {
			CBitmap* bmp = CBitmap::FromHandle(previewLabel.GetBitmap());
			txNode->updateBitmap(bmp);
			previewLabel.Invalidate();
		}
	}
*/	
}

bool CSlotPropertiesDialog::slotUnchecking(Slot* slot, bool isMixer) {
	if (slot) {
		if (!UpdateData()) {
			return false;
		}
		if (treeManager->getFocusedItem() == NULL || treeManager->isTextureFocused()) {
			tx3d::Texture3D* newTx = controlsToTexture(currentTx, isMixer);
			HTREEITEM selected = treeManager->getSelectedTexture();
			if (newTx) {
				theApp.slotsDialog.textureSelected(newTx, selected);
			} else {
				theApp.slotsDialog.textureSelected(currentTx, selected);
			}
		} else {
			tx3d::Location3D* newLoc = controlsToLocation(currentLc);
			HTREEITEM selected = treeManager->getSelectedLocation();
			if (newLoc) {
				theApp.slotsDialog.locationSelected(newLoc, selected);
			} else {
				theApp.slotsDialog.locationSelected(currentLc, selected);
			}
		}
	}
	return true;
}

void CSlotPropertiesDialog::slotChecked(Slot* slot, bool isMixer, HTREEITEM texture, HTREEITEM location) {
	if (slot) {
		treeManager->setTextureSelected(texture);
		treeManager->setLocationSelected(location);
		if (treeManager->getFocusedItem() == NULL || treeManager->isTextureFocused()) {
			treeManager->setFocusedItem(texture, true);
			if (currentTx) {
				currentTx->released();
			}
			currentTx = slot->getTexture();
			currentTx->referenced();
			textureToControls(currentTx, isMixer);
			updatePreviewLabel((TextureNode*) mainTree.GetItemData(texture));
		} else {
			treeManager->setFocusedItem(location, false);
			if (currentLc) {
				currentLc->released();
			}
			currentLc = slot->getLocation();
			currentLc->referenced();
			locationToControls(currentLc);
		}
	} else {
		if (treeManager->getFocusedItem() == NULL || treeManager->isTextureFocused()) {
			treeManager->setLocationSelected(NULL);
		} else {
			treeManager->setTextureSelected(NULL);
		}
	}
}

void CSlotPropertiesDialog::OnSelchangedTree(NMHDR* pNMHDR, LRESULT* pResult) 
{
	*pResult = 0;

	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	HTREEITEM item = pNMTreeView->itemNew.hItem;
	if (item != NULL) {
		Node* node = (Node*) mainTree.GetItemData(item);
		if (node) {
			theApp.instrumentsDialog.setInstrumentSelected(CInstrumentsDialog::POINTER_INSTRUMENT);

			TextureNode* txNode = dynamic_cast<TextureNode*>(node);
			if (txNode) {
				treeManager->setFocusedItem(item, true);
				treeManager->setLocationSelected(theApp.slotsDialog.getSelectedSlotLocationItem());
				treeManager->setTextureSelected(item);

				updatePreviewLabel(txNode);
					
				if (currentTx) {
					currentTx->released();
				}

				currentTx = txNode->createTexture();

				if (currentTx) {
					currentTx->referenced();
				}

				textureToControls(currentTx, theApp.slotsDialog.isSelectedSlotMixer());
				return;
			}

			LocationNode* locNode = dynamic_cast<LocationNode*>(node);
			if (locNode) {
				treeManager->setFocusedItem(item, false);
				treeManager->setTextureSelected(theApp.slotsDialog.getSelectedSlotTextureItem());
				HTREEITEM selected = treeManager->getSelectedTexture();
				if (selected != NULL) {
					updatePreviewLabel((TextureNode*) mainTree.GetItemData(selected));
				} else {
					//update bitmap with default grays
				}

				treeManager->setLocationSelected(item);
				if (currentLc) {
					currentLc->released();
				}

				currentLc = locNode->createLocation();

				if (currentLc) {
					currentLc->referenced();
				}

				locationToControls(currentLc);

				return;
			}
		}
	}
	
}

void CSlotPropertiesDialog::OnUpdateLocationControls() 
{
/*
	if (!controlsIsUpdating) {
		shouldValidate = false;
		UpdateData();
		shouldValidate = true;
		Slot* slot = theApp.slotsDialog.getSelectedSlot();
		tx3d::Location3D* newLoc = controlsToLocation(slot->getLocation());
		if (newLoc) {
			slot->setLocation(newLoc, theApp.runtime->getScene());
		} else {
			LocationNode::updateObject(slot->getLocation(), slot->getLocationObject());
		}
	}
*/
}

void CSlotPropertiesDialog::onActionSelected(ActionNode::ActionType type) {
	if ( theApp.slotsDialog.getSelectedActionType() != type ) {

		theApp.slotsDialog.actionSelected(type);
		slotChecked
			(
				theApp.slotsDialog.getSelectedSlot(),
				theApp.slotsDialog.isSelectedSlotMixer(),
				theApp.slotsDialog.getSelectedSlotTextureItem(),
				theApp.slotsDialog.getSelectedSlotLocationItem()
			);
	}
}

void CSlotPropertiesDialog::updatePreviewLabel(TextureNode* txNode) {
	CBitmap* bmp = CBitmap::FromHandle(previewLabel.GetBitmap());
	txNode->updateBitmap(bmp);
	previewLabel.Invalidate();
}

void CSlotPropertiesDialog::OnRclickTree(NMHDR* pNMHDR, LRESULT* pResult) 
{
	*pResult = 0;
  	sPoint point;
  	sPoint pointInTree;
	GetCursorPos(&point);
	pointInTree = point;
	mainTree.ScreenToClient(&pointInTree);
	HTREEITEM item = mainTree.HitTest(pointInTree);
	
	if (item != NULL) {
		Node* node = (Node*) mainTree.GetItemData(item);
		ModelNode* modelNode = 0;
		if (node) {
			modelNode = dynamic_cast<ModelNode*>(node);
			if (modelNode) {
				markedModelNode = item;
				modelMenu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON, point.x, point.y, this);
			}
		}
	}
}

void CSlotPropertiesDialog::OnAddModelTx() 
{
	CFileDialog fileDialog(
		TRUE,
		"*.*",
		NULL,
		OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_NOCHANGEDIR,
		NULL,
		theApp.m_pMainWnd);

	if (fileDialog.DoModal() == IDOK) {
		ModelNode* node = new ModelNode(string(fileDialog.GetFileTitle()), string(fileDialog.GetPathName()));
		treeManager->insertModelNode(markedModelNode, node);
	}
}

void CSlotPropertiesDialog::OnDeleteModelTx() 
{
	treeManager->deleteModelNode(markedModelNode);
}
