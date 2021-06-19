#ifndef __MESSAGEBOX_H__
#define __MESSAGEBOX_H__

typedef int(*DYNCALLBACK)(float,float);

enum OKTEXTID {
	MBOX_OK,
	MBOX_EXIT,
	MBOX_BACK
};

void setupYesNoMessageBox( DYNCALLBACK yesAction, float actionParam, const std::string& mess );
void setupOkMessageBox(	DYNCALLBACK action, float actionParam, const std::string& mess, OKTEXTID okBtnTextID, bool showButtons = true );

void setMessageBoxTextID(const char* textID);

void prepareMessageBoxToShow();

void showMessageBoxButtons();
void updateMessageBoxButtonsVisibility();

void showMessageBox();
void hideMessageBox();

#endif //__MESSAGEBOX_H__
