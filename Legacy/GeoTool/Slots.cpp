// Slots.cpp : implementation file
//

#include "stdafx.h"
#include "LocationNode.h"
#include "GeoTool.h"
#include "Slots.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSlots dialog


CSlots::CSlots(CWnd* pParent /*=NULL*/)
	: CDialog(CSlots::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSlots)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CSlots::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSlots)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSlots, CDialog)
	//{{AFX_MSG_MAP(CSlots)
	ON_COMMAND(ID_PAINTER_SLOT, OnPainterSlot)
	ON_COMMAND(ID_MIXER_SLOT, OnMixerSlot)
	ON_COMMAND(ID_CLEAR_SLOT, OnClearSlot)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSlots message handlers

BOOL CSlots::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	buttonCount = 0;
	lastID = 4;
	init(8);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CSlots::init(int btnCount) {
	selectedButton = addNewButton();
	for (int i = 1; i < btnCount; i++) {
		addNewButton();
	}
	selectedButton->SetCheck(1);
	theApp.slotPropertiesDialog.slotChecked
		(
			selectedButton->getSlot(),
			selectedButton->isMixerButton(),
			selectedButton->getTextureItem(),
			selectedButton->getLocationItem()
		);
	slotMenu.init();
}

CSlotButton* CSlots::addNewButton() {
	buttonCount++;
	sRect wndRect;
	GetWindowRect(&wndRect);
	wndRect.right = 
		  wndRect.left
		+ GetSystemMetrics(SM_CXEDGE) * 2
		+ BUTTON_WIDTH * buttonCount
		+ 1;
	wndRect.bottom =
		  wndRect.top
		+ GetSystemMetrics(SM_CYEDGE) * 2
		+ GetSystemMetrics(SM_CYSMCAPTION) + BUTTON_HEIGHT
		+ 1;
	MoveWindow(&wndRect);
	CSlotButton* btn = new CSlotButton();
	btn->Create(
		"Empty",
//		WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | BS_PUSHLIKE | BS_LEFT, 
//		WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | BS_LEFT | BS_OWNERDRAW, 
		WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, 
		CRect(BUTTON_WIDTH * (buttonCount - 1), 0, BUTTON_WIDTH * buttonCount, BUTTON_HEIGHT),
		this,
		lastID++);
	btn->SetFont(this->GetFont());
	buttons.push_back(btn);
	return btn;
}

void CSlots::locationSelected(tx3d::Location3D* location, HTREEITEM locationItem) {
	selectedButton->getSlot()->setLocation(location, theApp.runtime->getScene());
	selectedButton->setLocationItem(locationItem);
	LocationNode::updateObject(
		selectedButton->getSlot()->getLocation(),
		selectedButton->getSlot()->getLocationObject() );
	setColor(selectedButton, true);
}

void CSlots::textureSelected(tx3d::Texture3D* texture, HTREEITEM textureItem) {
	selectedButton->getSlot()->setTexture(texture);
	selectedButton->setTextureItem(textureItem);
}

ActionNode::ActionType CSlots::getSelectedActionType() {
	if (selectedButton->getSlot()) {
		return selectedButton->isMixerButton() ? ActionNode::MIX : ActionNode::PAINT;
	} else {
		return ActionNode::EMPTY;
	}
}

void CSlots::actionSelected(ActionNode::ActionType actionType) {
	switch (actionType) {
		case ActionNode::EMPTY:
			if (selectedButton->isMixerButton()) {
				slotManager.removeMixer(selectedButton->getSlot());
			} else {
				slotManager.removePainter(selectedButton->getSlot());
			}
			selectedButton->setSlot(0);
			selectedButton->setMixer(false);
			selectedButton->setTextureItem(NULL);
			selectedButton->setLocationItem(NULL);
			break;
		case ActionNode::PAINT:
			if (selectedButton->getSlot()) {
				slotManager.moveToPainters(selectedButton->getSlot());
			} else {
				selectedButton->setSlot(slotManager.addNewPainter( theApp.runtime->getScene() ));
				selectedButton->setTextureItem(
					theApp.slotPropertiesDialog.treeManager->getItemForTexture(selectedButton->getSlot()->getTexture()) );
				selectedButton->setLocationItem(
					theApp.slotPropertiesDialog.treeManager->getItemForLocation(selectedButton->getSlot()->getLocation()) );
			}
			selectedButton->setMixer(false);
			break;
		case ActionNode::MIX:
			if (selectedButton->getSlot()) {
				slotManager.moveToMixers(selectedButton->getSlot());
			} else {
				selectedButton->setSlot(slotManager.addNewMixer( theApp.runtime->getScene() ));
				selectedButton->setTextureItem(
					theApp.slotPropertiesDialog.treeManager->getItemForTexture(selectedButton->getSlot()->getTexture()) );
				selectedButton->setLocationItem(
					theApp.slotPropertiesDialog.treeManager->getItemForLocation(selectedButton->getSlot()->getLocation()) );
			}
			selectedButton->setMixer(true);
			break;
	}
}

bool CSlots::isSelectedSlotMixer() {
	return selectedButton->isMixerButton();
}

Slot* CSlots::getSelectedSlot() {
	return selectedButton->getSlot();
}

HTREEITEM CSlots::getSelectedSlotTextureItem() {
	return selectedButton->getTextureItem();
}

HTREEITEM CSlots::getSelectedSlotLocationItem() {
	return selectedButton->getLocationItem();
}

CSlotButton* CSlots::getSelectedButton() {
	return selectedButton;
}

void CSlots::setSelectedButton(CSlotButton* btn) {
	if (btn != selectedButton) {
		selectedButton->SetCheck(0);
		btn->SetCheck(1);
		setColor(selectedButton, false);
		setColor(btn, true);
		selectedButton = btn;
		theApp.slotPropertiesDialog.slotChecked
			(
				selectedButton->getSlot(),
				selectedButton->isMixerButton(),
				selectedButton->getTextureItem(),
				selectedButton->getLocationItem()
			);
		Invalidate();
	}
}

void CSlots::setColor(CSlotButton* btn, bool selected) {
	if ( btn->getSlot() && btn->getSlot()->getLocationObject() ) {
		LocationNode::setObjectSelected(btn->getSlot()->getLocationObject(), selected);
	}
}

bool CSlots::updateSelectedSlot() {
	return theApp.slotPropertiesDialog.slotUnchecking
						(
							selectedButton->getSlot(),
							selectedButton->isMixerButton()
						);
}

void CSlots::update(const Vect3f& point, const Vect3f& direction, const Vect3f& direction2, ConstraintType type) {
	Slot* selectedSlot = selectedButton->getSlot();
	if (selectedSlot &&	LocationNode::update(
							selectedSlot->getLocation(),
							selectedSlot->getLocationObject(),
							point,
							direction,
							direction2,
							type ) 
		) {

		theApp.slotPropertiesDialog.updateLocationControls(selectedSlot->getLocation());
	}
}

void CSlots::setLocationsVisible(bool visible) {
	list <CSlotButton*>::iterator iter;
	for ( iter = buttons.begin(); iter != buttons.end( ); iter++ ) {
		if ( (*iter)->getSlot() ) {
			cObjectNodeRoot* locationObject = (*iter)->getSlot()->getLocationObject();
			if (locationObject) {
				if (visible) {
					locationObject->ClearAttr(ATTRUNKOBJ_IGNORE);
				} else {
					locationObject->SetAttr(ATTRUNKOBJ_IGNORE);
				}
			}
		}
	}
	if (visible) {
		setColor(selectedButton, true);
	}
}

tx3d::Texture3D* CSlots::assembleTexture() {
	return slotManager.assembleTexture();
}

bool CSlots::parseTexture(tx3d::Texture3D* tx) {
	if ( !slotManager.parseTexture(tx, theApp.runtime->getScene()) ) {
		return false;
	}
	int count = buttons.size() - slotManager.getPainterCount() - slotManager.getMixerCount();
	if (count < 0) {
		for (int i = 0; i < count; i++) {
			addNewButton();
		}
	}
	list <CSlotButton*>::iterator it = buttons.begin();
	selectedButton->SetCheck(0);
	selectedButton = *it;
	selectedButton->SetCheck(1);
	for (int i = 0; i < slotManager.getPainterCount(); i++) {
		(*it)->setSlot(slotManager.getPainter(i));
		(*it)->setMixer(false);
		(*it)->setLocationItem(
			theApp.slotPropertiesDialog.treeManager->getItemForLocation((*it)->getSlot()->getLocation()) );
		(*it)->setTextureItem(
			theApp.slotPropertiesDialog.treeManager->getItemForTexture((*it)->getSlot()->getTexture()) );
		it++;
	}
	for (i = 0; i < slotManager.getMixerCount(); i++) {
		(*it)->setSlot(slotManager.getMixer(i));
		(*it)->setMixer(true);
		(*it)->setLocationItem(
			theApp.slotPropertiesDialog.treeManager->getItemForLocation((*it)->getSlot()->getLocation()) );
		(*it)->setTextureItem(
			theApp.slotPropertiesDialog.treeManager->getItemForTexture((*it)->getSlot()->getTexture()) );
		it++;
	}
	for (; it != buttons.end(); it++) {
		(*it)->setSlot(0);
		(*it)->setMixer(false);
		(*it)->setTextureItem(NULL);
		(*it)->setLocationItem(NULL);
	}

	setColor(selectedButton, false);
	theApp.slotPropertiesDialog.slotChecked
		(
			selectedButton->getSlot(),
			selectedButton->isMixerButton(),
			selectedButton->getTextureItem(),
			selectedButton->getLocationItem()
		);
	
	return true;
}


void CSlots::showPopup(CSlotButton* btn, CPoint point) {
	setSelectedButton(btn);
	btn->ClientToScreen(&point);
	slotMenu.TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON, point.x, point.y, this);
}

const int CSlots::BUTTON_WIDTH = 84;
const int CSlots::BUTTON_HEIGHT = 24;

void CSlots::OnPainterSlot() 
{
	theApp.slotPropertiesDialog.onActionSelected(ActionNode::PAINT);
}

void CSlots::OnMixerSlot() 
{
	theApp.slotPropertiesDialog.onActionSelected(ActionNode::MIX);
}

void CSlots::OnClearSlot() 
{
	theApp.slotPropertiesDialog.onActionSelected(ActionNode::EMPTY);
}
