// InstrumentsDialog.cpp : implementation file
//

#include "stdafx.h"
#include "GeoTool.h"
#include "InstrumentsDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CInstrumentsDialog dialog


CInstrumentsDialog::CInstrumentsDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CInstrumentsDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CInstrumentsDialog)
	//}}AFX_DATA_INIT
}


void CInstrumentsDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CInstrumentsDialog)
	DDX_Control(pDX, IDC_ARROW_BUTTON, arrowButton);
	DDX_Control(pDX, IDC_PIT_BRUSH, digButton);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CInstrumentsDialog, CDialog)
	//{{AFX_MSG_MAP(CInstrumentsDialog)
	ON_BN_CLICKED(IDC_PIT_BRUSH, OnPitBrush)
	ON_BN_CLICKED(IDC_PIT_BRUSH_BIG, OnPitBrushBig)
	ON_BN_CLICKED(IDC_ARROW_BUTTON, OnArrowButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInstrumentsDialog message handlers

void CInstrumentsDialog::OnPitBrush() 
{
	theApp.setBrush(VMapOperator::PIT_BRUSH);
	selectedInstrument = DIG_INSTRUMENT;
}

void CInstrumentsDialog::OnPitBrushBig() 
{
	theApp.setBrush(VMapOperator::PIT_BRUSH_BIG);
	selectedInstrument = DIG_INSTRUMENT;
}

void CInstrumentsDialog::OnArrowButton() 
{
	selectedInstrument = POINTER_INSTRUMENT;
}

void CInstrumentsDialog::setInstrumentSelected(InstrumentType type) {
	if (selectedInstrument != type) {
		setInstrumentChecked(0, selectedInstrument);
		selectedInstrument = type;
		setInstrumentChecked(1, selectedInstrument);
	}
}

void CInstrumentsDialog::setInstrumentChecked(int checked, InstrumentType type) {
	switch (type) {
		case POINTER_INSTRUMENT:
			arrowButton.SetCheck(checked);
			break;
		case DIG_INSTRUMENT:
			digButton.SetCheck(checked);
			break;
	}
}

BOOL CInstrumentsDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();

	selectedInstrument = POINTER_INSTRUMENT;
	textureUpdating = false;
	needUpdateTexture = true;
	isAdjusting = false;

	CBitmap bmp;
	bmp.LoadBitmap(IDB_PIT_BRUSH);
	static_cast<CButton*>(GetDlgItem(IDC_PIT_BRUSH))->SetBitmap(bmp);
	bmp.Detach();

	bmp.LoadBitmap(IDB_PIT_BRUSH_BIG);
	static_cast<CButton*>(GetDlgItem(IDC_PIT_BRUSH_BIG))->SetBitmap(bmp);
	bmp.Detach();

	bmp.LoadBitmap(IDB_ARROW);
	CButton* btn = static_cast<CButton*>(GetDlgItem(IDC_ARROW_BUTTON));
	btn->SetBitmap(bmp);
	bmp.Detach();
	btn->SetCheck(1);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CInstrumentsDialog::handleInput(Operator* op, float deltaT) {
	switch (selectedInstrument) {
		case POINTER_INSTRUMENT:
			handlePointerInput(op, deltaT);
			break;
		case DIG_INSTRUMENT:
			handleDigInput(op, deltaT);
			break;
	}
}
void CInstrumentsDialog::inputDisabled(Operator* op) {
	needUpdateTexture = true;
}

void CInstrumentsDialog::updateTexture(Operator* op) {
	Vect2f p;
	if ( op->getViewportPos(&p) ) {
		if (!textureUpdating) {
			textureUpdating = true;
			if (theApp.slotsDialog.updateSelectedSlot()) {
				CWaitCursor wait;
				op->setTexture( theApp.slotsDialog.assembleTexture() );
				needUpdateTexture = false;
			}
			textureUpdating = false;
		}
	}
}

void CInstrumentsDialog::handlePointerInput(Operator* op, float deltaT) {
	if (theApp.slotPropertiesDialog.isLocationSelected()) {
		if( GetAsyncKeyState(VK_LBUTTON) < 0 ) {
			if (needUpdateTexture) {
				updateTexture(op);
			} else {
				isAdjusting = true;
				Vect3f p;
				Vect3f dir;
				Vect3f dir2;
				if ( op->cursorTrace(&p, &dir, &dir2) ) {
					if ( GetAsyncKeyState(VK_CONTROL) < 0 ) {
						theApp.slotsDialog.update(p, dir, dir2, LocationUpdater::SCALE);
					} else if ( GetAsyncKeyState(VK_SHIFT) < 0 ) {
						theApp.slotsDialog.update(p, dir, dir2, LocationUpdater::SHIFT_ZY);
					} else {
						theApp.slotsDialog.update(p, dir, dir2, LocationUpdater::SHIFT_XY);
					}
				}
			}
		} else {
			if (isAdjusting) {
				isAdjusting = false;
				updateTexture(op);
			}
		}
	} else {
		Vect2f p;
		if( op->getViewportPos(&p) && (GetAsyncKeyState(VK_LBUTTON) < 0) ) {
			if (!textureUpdating) {
				textureUpdating = true;
				if (theApp.slotsDialog.updateSelectedSlot()) {
					CWaitCursor wait;
					tx3d::Texture3D* tx = theApp.slotsDialog.assembleTexture();
					if (!tx) {
						// set new painter slot with selected texture
					}
					op->setTexture( tx );
				}
				textureUpdating = false;
			}
		}
	}
}

void CInstrumentsDialog::handleDigInput(Operator* op, float deltaT) {
	if( GetAsyncKeyState(VK_LBUTTON) < 0) {	
		if ( GetAsyncKeyState(VK_CONTROL) < 0) {
			Vect3f v;
			if (op->cursorTrace(&v)) {
				op->shiftExactColorMap(&v);
			}
		} else {
			Vect3f v;
			if (op->cursorTrace(&v)) {
				float factor = deltaT / 400.0f;
				op->getVMapOperator()->dig( v, (GetAsyncKeyState(VK_SHIFT) < 0) ? -factor : factor );
			}
		}
	}
}
