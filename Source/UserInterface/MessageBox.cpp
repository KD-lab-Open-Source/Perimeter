
#include "StdAfx.h"
#include "Config.h"
#include "Runtime.h"
#include "terra.h"
#include "GenericControls.h"
#include "Universe.h"
#include "UniverseInterface.h"
#include "GameShell.h"
#include "PerimeterShellUI.h"
#include "MainMenu.h"
#include "MessageBox.h"

DYNCALLBACK submitAction = 0;
float submitParam = 0;
bool showNoBtn = true;

bool showBtns = true;

void setupYesNoMessageBox(DYNCALLBACK yesAction, float actionParam, const std::string& mess) {
	submitAction = yesAction;
	submitParam = actionParam;
	showNoBtn = true;
	showBtns = true;
	((CTextWindow*)_shellIconManager.GetWnd(SQSH_MM_SUBMIT_TXT))->setText( mess );
}

void setupOkMessageBox(DYNCALLBACK action, float actionParam, const std::string& mess, OKTEXTID okBtnTextID, bool showButtons) {
	submitAction = action;
	submitParam = actionParam;
	showNoBtn = false;
	showBtns = showButtons;
	((CTextWindow*)_shellIconManager.GetWnd(SQSH_MM_SUBMIT_TXT))->setText( mess );
	std::string okTextID;
	switch (okBtnTextID) {
		case MBOX_OK:
			okTextID = "Interface.Menu.ButtonLabels.OK";
			break;
		case MBOX_EXIT:
			okTextID = "Interface.Menu.ButtonLabels.EXIT";
			break;
		case MBOX_BACK:
			okTextID = "Interface.Menu.ButtonLabels.BACK";
			break;
	}
	((CShellPushButton*) _shellIconManager.GetWnd(SQSH_MM_SUBMIT_OK_BTN))->setText(qdTextDB::instance().getText(okTextID.c_str()));
}

void setMessageBoxTextID(const char* textID) {
	((CTextWindow*)_shellIconManager.GetWnd(SQSH_MM_SUBMIT_TXT))->setText( qdTextDB::instance().getText(textID) );
}

void prepareMessageBoxToShow() {
	updateMessageBoxButtonsVisibility();
	_shellIconManager.SetModalWnd(SQSH_MM_SUBMIT_DIALOG_SCR);
}

void showMessageBoxButtons() {
	showBtns = true;
	updateMessageBoxButtonsVisibility();
}

void updateMessageBoxButtonsVisibility() {
	_shellIconManager.GetWnd(SQSH_MM_SUBMIT_NO_BTN)->Show( showBtns && showNoBtn );
	_shellIconManager.GetWnd(SQSH_MM_SUBMIT_NO_BORDER)->Show( showBtns && showNoBtn );
	_shellIconManager.GetWnd(SQSH_MM_SUBMIT_YES_BTN)->Show( showBtns && showNoBtn );
	_shellIconManager.GetWnd(SQSH_MM_SUBMIT_YES_BORDER)->Show( showBtns && showNoBtn );
	_shellIconManager.GetWnd(SQSH_MM_SUBMIT_OK_BTN)->Show( showBtns && !showNoBtn );
	_shellIconManager.GetWnd(SQSH_MM_SUBMIT_OK_BORDER)->Show( showBtns && !showNoBtn );
}

void showMessageBox() {
    xassert(!((CTextWindow*)_shellIconManager.GetWnd(SQSH_MM_SUBMIT_TXT))->getText().empty());
	_shellIconManager.SwitchMenuScreens( FAKE, SQSH_MM_SUBMIT_DIALOG_SCR );
}

void hideMessageBox() {
	_shellIconManager.SwitchMenuScreens( SQSH_MM_SUBMIT_DIALOG_SCR, FAKE );
}

//background control
void onMMSubmitDialogBackground(CShellWindow* pWnd, InterfaceEventCode code, int param) {
    if( code == EVENT_SHOWWND ) {
        //Pick the current active screen if any, if is a CMultiTexWindow check if it has any bg and if it has then
        // not show one from dialog to avoid duplicate bg's
        bool showBackground = true;
        int id = _shellIconManager.getVisibleMenuScr();
        CMultiTexWindow* current_scr = 0 < id ? dynamic_cast<CMultiTexWindow*>(_shellIconManager.GetWnd(id)) : nullptr;
        if (current_scr) {
            showBackground = current_scr->m_hTexture2 == nullptr
                          && current_scr->m_hTexture3 == nullptr;
        }
        if (!showBackground) {
            pWnd->Show(false);
        }
    }
}

//submit dialog
void onMMSubmitYesButton(CShellWindow* pWnd, InterfaceEventCode code, int param) {
	if( code == EVENT_UNPRESSED ) {
		int res = 0;
		if (submitAction) {
			res = submitAction(submitParam, 0);
		}
		if (!res) {
			hideMessageBox();
		}
	}
}

void onMMSubmitNoButton(CShellWindow* pWnd, InterfaceEventCode code, int param) {
	if( code == EVENT_UNPRESSED ) {
		hideMessageBox();
	}
}

