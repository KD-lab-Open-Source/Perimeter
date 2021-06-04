
#include "StdAfx.h"
#include "Config.h"
#include "Runtime.h"
#include "Terra.h"
#include "GenericControls.h"
#include "Universe.h"
#include "UniverseInterface.h"
#include "GameShell.h"
#include "PerimeterShellUI.h"
#include "Controls.h"

#include "BGScene.h"
#include "..\ht\ht.h"

extern MpegSound gb_Music;

extern bool menuChangingDone;

extern vector<MissionDescription> multiplayerMaps;
extern MissionDescription missionToExec;
extern BGScene bgScene;

//unsigned int selectedLanGameID = 0;
//GUID selectedLanGameID;
void checkMissionDescription(int index, vector<MissionDescription>& mVect);

//online menu
void onMMOnlineGameList(CShellWindow* pWnd, InterfaceEventCode code, int param) {
	if (code == EVENT_CREATEWND) {
		CListBoxWindow* list = (CListBoxWindow*)pWnd;
		list->NewItem(1);
		list->Clear();
	} else if (code == EVENT_DRAWWND) {
		CListBoxWindow* listBox = (CListBoxWindow*)pWnd;
		listBox->Clear();

/*
		if (gameShell->getNetClient()->getGameHostList().size() > 0) {
			list<sGameHostInfo*>::iterator it;

			int selectIndex = 0;
			int i = 0;
			for (it = gameShell->getNetClient()->getGameHostList().begin(); it != gameShell->getNetClient()->getGameHostList().end(); it++, i++) {
				listBox->AddString( (*it)->hostName.c_str(), 0 );
				if ((*it)->gameHostID == selectedLanGameID) {
					selectIndex = i;
				}
			}
			listBox->SetCurSel(selectIndex);
			if (selectIndex == 0) {
				selectedLanGameID = gameShell->getNetClient()->getGameHostList().front()->gameHostID;
			}
			((CShellPushButton*)_shellIconManager.GetWnd(SQSH_MM_JOIN_BTN))->Enable(TRUE);
		} else {
			((CShellPushButton*)_shellIconManager.GetWnd(SQSH_MM_JOIN_BTN))->Enable(0);
		}
*/
	} else if (code == EVENT_PRESSED) {
/*
		CListBoxWindow* listBox = (CListBoxWindow*)_shellIconManager.GetWnd(SQSH_MM_GAME_LIST);
		int pos = listBox->GetCurSel();
		if (pos >= 0 && pos < gameShell->getNetClient()->getGameHostList().size()) {
			list<sGameHostInfo*>::iterator it = gameShell->getNetClient()->getGameHostList().begin();
			advance(it, pos);
			selectedLanGameID = (*it)->gameHostID;
		}
*/
	}		
}
void onMMOnlineCreateGameButton(CShellWindow* pWnd, InterfaceEventCode code, int param) {
	if( code == EVENT_UNPRESSED && !bgScene.isPlaying() ) {
		_shellIconManager.SwitchMenuScreens(pWnd->m_pParent->ID, SQSH_MM_CREATE_ONLINE_GAME_SCR);
	}
}
void onMMOnlineJoinButton(CShellWindow* pWnd, InterfaceEventCode code, int param) {
	if( code == EVENT_UNPRESSED && !bgScene.isPlaying() ) {
		CEditWindow* input = (CEditWindow*)_shellIconManager.GetWnd(SQSH_MM_ONLINE_PLAYER_NAME_INPUT);
//		gameShell->getNetClient()->JoinGame(selectedLanGameID, input->GetText(), BELLIGERENT_EXODUS0, 0);
//		_shellIconManager.SwitchMenuScreens(pWnd->m_pParent->ID, SQSH_MM_ONLINE_LOBBY_SCR);
	}
}
void onMMOnlineBackButton(CShellWindow* pWnd, InterfaceEventCode code, int param) {
	if( code == EVENT_UNPRESSED && !bgScene.isPlaying() ) {
		_shellIconManager.SwitchMenuScreens( pWnd->m_pParent->ID, SQSH_MM_START_SCR );
	}
}

//create online game menu
void onMMOnlineMapList(CShellWindow* pWnd, InterfaceEventCode code, int param) {
	if (code == EVENT_PRESSED) {
		CListBoxWindow* list = (CListBoxWindow*)_shellIconManager.GetWnd(SQSH_MM_ONLINE_MAP_LIST);
		int pos = list->GetCurSel();
		if (pos >= 0 && pos < multiplayerMaps.size()) {
			checkMissionDescription(pos, multiplayerMaps);
			((CShowMapWindow*)_shellIconManager.GetWnd(SQSH_MM_CREATE_ONLINE_GAME_MAP))->setWorldID( multiplayerMaps[pos].worldID() );
			((CTextWindow*)_shellIconManager.GetWnd(SQSH_MM_CREATE_ONLINE_GAME_MAP_DESCR_TXT))->setText( multiplayerMaps[pos].missionDescription() );
		}
	}		
}

void onMMOnlineGameSpeedSlider(CShellWindow* pWnd, InterfaceEventCode code, int param) {
	if ( code == EVENT_CREATEWND ) {
		CSliderWindow *pSlider = (CSliderWindow*) pWnd;
		pSlider->pos = (1.0f - 0.2f) / 4.8f;
	}
}

void onMMOnlineCreateButton(CShellWindow* pWnd, InterfaceEventCode code, int param) {
	if( code == EVENT_UNPRESSED && !bgScene.isPlaying() ) {
/*
		CSliderWindow *pSlider = (CSliderWindow*) _shellIconManager.GetWnd(SQSH_MM_ONLINE_GAME_SPEED_SLIDER);
		CListBoxWindow* list = (CListBoxWindow*)_shellIconManager.GetWnd(SQSH_MM_ONLINE_MAP_LIST);
		CEditWindow* input = (CEditWindow*)_shellIconManager.GetWnd(SQSH_MM_ONLINE_PLAYER_NAME_INPUT);
		string gameName="Игра от:";
		gameName+=input->GetText();
		gameShell->getNetClient()->CreateGame(gameName.c_str(), strcat("RESOURCE\\MULTIPLAYER\\", multiplayerMaps[list->GetCurSel()].missionName()), input->GetText(), BELLIGERENT_EXODUS0, 0, 0.2f + pSlider->pos * 4.8f);
		_shellIconManager.SwitchMenuScreens(pWnd->m_pParent->ID, SQSH_MM_ONLINE_LOBBY_SCR);
*/
	}
}

//online lobby

void setOnlineFrm(CComboWindow* combo, int number) {
	MissionDescription& currMission = gameShell->getNetClient()->getCurrentMissionDescription();
	if (
			currMission.playerAmountScenarioMax > number
		&&	currMission.playersData[number].realPlayerType != REAL_PLAYER_TYPE_OPEN
		&&	currMission.playersData[number].realPlayerType != REAL_PLAYER_TYPE_CLOSE
		) {

		combo->Show(true);
		combo->Enable(
					currMission.activePlayerID == currMission.playersData[number].playerID
				||	(currMission.activePlayerID == currMission.playersData[0].playerID && currMission.playersData[number].realPlayerType == REAL_PLAYER_TYPE_AI)
			);
		switch (currMission.playersData[number].belligerent) {
			case BELLIGERENT_EXODUS0:
			case BELLIGERENT_EXODUS1:
				combo->pos = 0;
				break;
			case BELLIGERENT_HARKBACKHOOD0:
			case BELLIGERENT_HARKBACKHOOD1:
				combo->pos = 2;
				break;
			case BELLIGERENT_EMPIRE0:
			case BELLIGERENT_EMPIRE1:
			default:
				combo->pos = 1;
		}
	} else {
		combo->Show(false);
	}
}

void setupOnlineFrm(CComboWindow* combo, int number, bool direction) {
	terBelligerent curBelligerent;
	int newPos = combo->pos;
	if (direction) {
		newPos++;
		if (newPos >= combo->size) {
			newPos = 0;
		}
	} else {
		newPos--;
		if (newPos < 0) {
			newPos = combo->size - 1;
		}
	}
	switch (newPos) {
		case 0:
			curBelligerent = BELLIGERENT_EXODUS0;
			break;
		case 1:
			curBelligerent = BELLIGERENT_EMPIRE0;
			break;
		case 2:
			curBelligerent = BELLIGERENT_HARKBACKHOOD0;
			break;
	}
//	gameShell->getNetClient()->changePlayerBelligerent(number, curBelligerent);
}

void setupOnlineFrmButton(CShellWindow* pWnd, InterfaceEventCode code, int number) {
	if( code == EVENT_CREATEWND ) {
		CComboWindow *pCombo = (CComboWindow*) pWnd;
		pCombo->Array.push_back( "Exodus" );
		pCombo->Array.push_back( "Empire" );
		pCombo->Array.push_back( "Harkback" );
		pCombo->size = 3;
		pCombo->pos = 0;
	} else if (code == EVENT_UNPRESSED) {
		setupOnlineFrm((CComboWindow*) pWnd, number, true);
	} else if (code == EVENT_RUNPRESSED) {
		setupOnlineFrm((CComboWindow*) pWnd, number, false);
	}
}

void onMMOnlineLobbyFrmButton(CShellWindow* pWnd, InterfaceEventCode code, int param) {
	setupOnlineFrmButton(pWnd, code, pWnd->ID - SQSH_MM_LOBBY_PLAYER1_FRM_BTN);
}

void setOnlineSlot(CComboWindow* combo, int number) {
	MissionDescription& currMission = gameShell->getNetClient()->getCurrentMissionDescription();
	if ( currMission.playerAmountScenarioMax > number ) {
		combo->Show(true);
		combo->Enable(currMission.activePlayerID == currMission.playersData[0].playerID);
		combo->pos = currMission.playersData[number].realPlayerType;
	} else {
		combo->Show(false);
	}
}

void setupOnlineSlot(CComboWindow* combo, int number, bool direction) {
	int newPos = combo->pos;
	if (direction) {
		newPos++;
		if (newPos >= combo->size) {
			newPos = 0;
		}
	} else {
		newPos--;
		if (newPos < 0) {
			newPos = combo->size - 1;
		}
	}
//	gameShell->getNetClient()->changeRealPlayerType(number, RealPlayerType(newPos));
}
void setupOnlineSlotButton(CShellWindow* pWnd, InterfaceEventCode code, int number) {
	if( code == EVENT_CREATEWND ) {
		CComboWindow *pCombo = (CComboWindow*) pWnd;
		pCombo->Array.push_back( "Open" );
		pCombo->Array.push_back( "Player" );
		pCombo->Array.push_back( "Closed" );
		pCombo->Array.push_back( "AI" );
		pCombo->size = 4;
		pCombo->pos = 0;
	} else if (code == EVENT_UNPRESSED) {
		setupOnlineSlot((CComboWindow*) pWnd, number, true);
	} else if (code == EVENT_RUNPRESSED) {
		setupOnlineSlot((CComboWindow*) pWnd, number, false);
	}
}
void onMMOnlineLobbySlotButton(CShellWindow* pWnd, InterfaceEventCode code, int param) {
	setupOnlineSlotButton(pWnd, code, pWnd->ID - SQSH_MM_LOBBY_PLAYER1_SLOT_BTN);
}

void setOnlineClr(CColorComboWindow* combo, CShellWindow* bg, int number) {
	MissionDescription& currMission = gameShell->getNetClient()->getCurrentMissionDescription();
	if (
			currMission.playerAmountScenarioMax > number
		&&	currMission.playersData[number].realPlayerType != REAL_PLAYER_TYPE_OPEN
		&&	currMission.playersData[number].realPlayerType != REAL_PLAYER_TYPE_CLOSE
		) {

		bg->Show(true);
		combo->Show(true);
		combo->Enable(
					currMission.activePlayerID == currMission.playersData[number].playerID
				||	(currMission.activePlayerID == currMission.playersData[0].playerID && currMission.playersData[number].realPlayerType == REAL_PLAYER_TYPE_AI)
			);
		combo->pos = currMission.playersData[number].colorIndex;
	} else {
		bg->Show(false);
		combo->Show(false);
	}
}
void setupOnlineClr(CColorComboWindow* combo, int number, bool direction) {
	int newPos = combo->pos;
	if (direction) {
		newPos++;
		if (newPos >= combo->size) {
			newPos = 0;
		}
	} else {
		newPos--;
		if (newPos < 0) {
			newPos = combo->size - 1;
		}
	}
//	gameShell->getNetClient()->changePlayerColor(number, newPos);
}
void setupOnlineClrButton(CShellWindow* pWnd, InterfaceEventCode code, int number) {
	if (code == EVENT_UNPRESSED) {
		setupOnlineClr((CColorComboWindow*) pWnd, number, true);
	} else if (code == EVENT_RUNPRESSED) {
		setupOnlineClr((CColorComboWindow*) pWnd, number, false);
	}
}
void onMMOnlineLobbyClrButton(CShellWindow* pWnd, InterfaceEventCode code, int param) {
	setupOnlineClrButton(pWnd, code, pWnd->ID - SQSH_MM_LOBBY_PLAYER1_CLR_BTN);
}

void setOnlineName(CShellPushButton* btn, int number) {
	MissionDescription& currMission = gameShell->getNetClient()->getCurrentMissionDescription();
	if (
			currMission.playerAmountScenarioMax > number
		&&	currMission.playersData[number].realPlayerType != REAL_PLAYER_TYPE_OPEN
		&&	currMission.playersData[number].realPlayerType != REAL_PLAYER_TYPE_CLOSE
		) {

		btn->Show(true);
		btn->Enable(false);
		btn->setText(string(currMission.playersData[number].name()));
	} else {
		btn->Show(false);
	}
}
void onMMOnlineLobbyNameButton(CShellWindow* pWnd, InterfaceEventCode code, int param) {
}

void onMMOnlineLobby(CShellWindow* pWnd, InterfaceEventCode code, int param) {
	if (code == EVENT_DRAWWND) {
/*
		MissionDescription& currMission = gameShell->getNetClient()->getCurrentMissionDescription();
//		((CPushButton*)pWnd)->setText(currMission.missionDescription());
		
		((CShowMapWindow*)_shellIconManager.GetWnd(SQSH_MM_LOBBY_GAME_MAP))->setWorldID( currMission.worldID() );
		((CTextWindow*)_shellIconManager.GetWnd(SQSH_MM_LOBBY_GAME_MAP_DESCR_TXT))->setText( currMission.missionDescription() );
	
		for (int i = 0; i < 4; i++) {
			setOnlineFrm(((CComboWindow*)_shellIconManager.GetWnd(SQSH_MM_LOBBY_PLAYER1_FRM_BTN + i)), i);
			setOnlineClr(((CColorComboWindow*)_shellIconManager.GetWnd(SQSH_MM_LOBBY_PLAYER1_CLR_BTN + i)), _shellIconManager.GetWnd(SQSH_MM_LOBBY_PLAYER1_CLR_BG + i), i);
			setOnlineSlot(((CComboWindow*)_shellIconManager.GetWnd(SQSH_MM_LOBBY_PLAYER1_SLOT_BTN + i)), i);
			setOnlineName(((CShellPushButton*)_shellIconManager.GetWnd(SQSH_MM_LOBBY_PLAYER1_NAME_BTN + i)), i);
		}
*/
	}
}

void onMMOnlineLobbyStartButton(CShellWindow* pWnd, InterfaceEventCode code, int param) {
	if (code == EVENT_UNPRESSED){
//		gameShell->getNetClient()->StartLoadTheGame();
	}
}
void onMMOnlineLobbyBackButton(CShellWindow* pWnd, InterfaceEventCode code, int param) {
	if( code == EVENT_UNPRESSED && !bgScene.isPlaying() ) {
		_shellIconManager.SwitchMenuScreens( pWnd->m_pParent->ID, SQSH_MM_ONLINE_SCR );
	}
}
/*
int SwitchLanToLoadQuant( float, float ) {
	if (menuChangingDone) {
		gameShell->currentSingleProfile.setLastGameType(UserSingleProfile::LAN);

		gb_Music.FadeVolume(_fEffectButtonTotalTime*0.001f);
		_shellIconManager.SwitchMenuScreens(SQSH_MM_LOBBY_SCR, SQSH_MM_LOADING_MISSION_SCR);
		return 0;
	}
	return 1;
}

void GameShell::LANGameStart(const MissionDescription& mission) {
	missionToExec = mission;
	_shellIconManager.AddDynamicHandler(SwitchLanToLoadQuant, CBCODE_QUANT);

//	missionToExec = mission;

//	currentSingleProfile.setLastGameType(UserSingleProfile::LAN);

//	gb_Music.FadeVolume(_fEffectButtonTotalTime*0.001f);
//	_shellIconManager.SwitchMenuScreens(SQSH_MM_LOBBY_SCR, SQSH_MM_LOADING_MISSION_SCR);

}
*/