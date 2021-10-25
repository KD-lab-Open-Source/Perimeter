
#include "StdAfx.h"
#include "Config.h"
#include "terra.h"
#include "Universe.h"
#include "GameShell.h"
#include "PerimeterShellUI.h"
#include "MainMenu.h"

#include "BGScene.h"

#include "BelligerentSelect.h"
#include "GameContent.h"
#include "MultiplayerCommon.h"

extern BGScene bgScene;

extern std::string getItemTextFromBase(const char *keyStr);

void fillMultiplayerLobbyList() {
    CListBoxWindow* list = (CListBoxWindow*)_shellIconManager.GetWnd(SQSH_MM_LOBBY_MAP_LIST);
    list->NewItem(1);
    list->Clear();
    for (int i = 0; i < multiplayerMaps.size(); i++) {
        std::string name = "MapNames.";
        name += multiplayerMaps[i].missionName();
        name = qdTextDB::instance().getText(name.c_str());
        if (name.empty()) {
            name = multiplayerMaps[i].missionName();
        }
        list->AddString(name.c_str(), 0);
    }
    if (multiplayerMaps.empty()) {
        list->SetCurSel(-1);
    } else {
        list->SetCurSel(0);
    }
}

void onMMMultiplayerGameSpeedSlider(CShellWindow* pWnd, InterfaceEventCode code, int param) {
    if ( code == EVENT_CREATEWND ) {
        CSliderWindow *pSlider = (CSliderWindow*) pWnd;
        pSlider->pos = (1.0f - 0.2f) / 4.8f;
    }
}

void onMMLobbyMapList(CShellWindow* pWnd, InterfaceEventCode code, int param) {
	if ( code == EVENT_PRESSED && intfCanHandleInput() ) {
		CListBoxWindow* list = (CListBoxWindow*)pWnd;
		if (param >= 0 && param < multiplayerMaps.size()) {
			checkMissionDescription(param, multiplayerMaps);
			std::string missionName = std::string("RESOURCE\\MULTIPLAYER\\") + multiplayerMaps[param].missionName();
			gameShell->getNetClient()->changeMap(missionName.c_str());
		}
/*
		int pos = list->GetCurSel();
		if (pos >= 0 && pos < multiplayerMaps.size()) {
			checkMissionDescription(pos, multiplayerMaps);
			string missionName = string("RESOURCE\\MULTIPLAYER\\") + multiplayerMaps[pos].missionName();
			gameShell->getNetClient()->changeMap(missionName.c_str());

		}
*/
	}		
}

void setFrm(CComboWindow* combo, int number) {
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
        if (unavailableContentBelligerent(currMission.playersData[number].belligerent, terGameContentSelect)) {
            combo->pos = 0;
            gameShell->getNetClient()->changePlayerBelligerent(number, BELLIGERENT_EXODUS0);
        } else {
            combo->pos = getSelectableBelligerentIndex(currMission.playersData[number].belligerent);
        }
	} else {
		combo->Show(false);
	}
}

void onMMLobbyFrmButton(CShellWindow* pWnd, InterfaceEventCode code, int param) {
    setupFrameButton(pWnd, code, pWnd->ID - SQSH_MM_LOBBY_PLAYER1_FRM_BTN, true);
}

void setSlot(CComboWindow* combo, int number) {
	MissionDescription& currMission = gameShell->getNetClient()->getCurrentMissionDescription();
	if ( currMission.playerAmountScenarioMax > number ) {
		combo->Show(true);
		combo->Enable(currMission.activePlayerID == currMission.playersData[0].playerID);
		switch (currMission.playersData[number].realPlayerType) {
			case REAL_PLAYER_TYPE_OPEN:
				combo->pos = 0;
				break;
			case REAL_PLAYER_TYPE_CLOSE:
				combo->pos = 2;
				break;
			case REAL_PLAYER_TYPE_AI:
				{
					switch (currMission.playersData[number].difficulty) {
						case DIFFICULTY_EASY:
							combo->pos = 3;
							break;
						case DIFFICULTY_NORMAL:
							combo->pos = 4;
							break;
						case DIFFICULTY_HARD:
							combo->pos = 5;
							break;
					}
				}
				break;
			case REAL_PLAYER_TYPE_PLAYER:
				combo->pos = 1;
				break;
		}
//		combo->pos = currMission.playersData[number].realPlayerType;
	} else {
		combo->Show(false);
	}
}
void setupSlot(CComboWindow* combo, int number, bool direction) {
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
			gameShell->getNetClient()->changeRealPlayerType(number, REAL_PLAYER_TYPE_OPEN);
			break;
		case 1:
			gameShell->getNetClient()->changeRealPlayerType(number, direction ? REAL_PLAYER_TYPE_CLOSE : REAL_PLAYER_TYPE_OPEN);
			break;
		case 2:
			gameShell->getNetClient()->changeRealPlayerType(number, REAL_PLAYER_TYPE_CLOSE);
			break;
		case 3:
			gameShell->getNetClient()->changeRealPlayerType(number, REAL_PLAYER_TYPE_AI);
			gameShell->getNetClient()->changePlayerDifficulty(number, DIFFICULTY_EASY);
			break;
		case 4:
			gameShell->getNetClient()->changeRealPlayerType(number, REAL_PLAYER_TYPE_AI);
			gameShell->getNetClient()->changePlayerDifficulty(number, DIFFICULTY_NORMAL);
			break;
		case 5:
			gameShell->getNetClient()->changeRealPlayerType(number, REAL_PLAYER_TYPE_AI);
			gameShell->getNetClient()->changePlayerDifficulty(number, DIFFICULTY_HARD);
			break;
	}
//	gameShell->getNetClient()->changeRealPlayerType(number, RealPlayerType(newPos));
}
void setupSlotButton(CShellWindow* pWnd, InterfaceEventCode code, int number) {
	if( code == EVENT_CREATEWND ) {
		CComboWindow *pCombo = (CComboWindow*) pWnd;
		pCombo->Array.push_back( getItemTextFromBase("Open").c_str() );
		pCombo->Array.push_back( getItemTextFromBase("Player").c_str() );
		pCombo->Array.push_back( getItemTextFromBase("Closed").c_str() );
		pCombo->Array.push_back( getItemTextFromBase("AI (Easy)").c_str() );
		pCombo->Array.push_back( getItemTextFromBase("AI (Normal)").c_str() );
        pCombo->Array.push_back( getItemTextFromBase("AI (Hard)").c_str() );
		pCombo->size = pCombo->Array.size();
		pCombo->pos = 0;
	} else if (code == EVENT_UNPRESSED) {
		setupSlot((CComboWindow*) pWnd, number, true);
	} else if (code == EVENT_RUNPRESSED) {
		setupSlot((CComboWindow*) pWnd, number, false);
	}
}
void onMMLobbySlotButton(CShellWindow* pWnd, InterfaceEventCode code, int param) {
	setupSlotButton(pWnd, code, pWnd->ID - SQSH_MM_LOBBY_PLAYER1_SLOT_BTN);
}

void setClan(CComboWindow* combo, int number) {
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
		combo->pos = currMission.playersData[number].clan;
	} else {
		combo->Show(false);
	}
}
void setupClan(CComboWindow* combo, int number, bool direction) {
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
	gameShell->getNetClient()->changePlayerClan(number, newPos);
//	gameShell->getNetClient()->changeRealPlayerType(number, RealPlayerType(newPos));
}
void setupClanButton(CShellWindow* pWnd, InterfaceEventCode code, int number) {
	if( code == EVENT_CREATEWND ) {
		CComboWindow *pCombo = (CComboWindow*) pWnd;
		std::string clan = getItemTextFromBase("Clan");
		char buff[30 + 1];
		for (int i = 0; i < NETWORK_PLAYERS_MAX; i++) {
			sprintf(buff, "%d", (i + 1));
			pCombo->Array.push_back( (clan + buff).c_str() );
		}
		pCombo->size = NETWORK_PLAYERS_MAX;
		pCombo->pos = 0;
	} else if (code == EVENT_UNPRESSED) {
		setupClan((CComboWindow*) pWnd, number, true);
	} else if (code == EVENT_RUNPRESSED) {
		setupClan((CComboWindow*) pWnd, number, false);
	}
}

void onMMLobbyClanButton(CShellWindow* pWnd, InterfaceEventCode code, int param) {
	setupClanButton(pWnd, code, pWnd->ID - SQSH_MM_LOBBY_PLAYER1_CLAN_BTN);
}

void setHC(CComboWindow* combo, int number) {
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

		int handicap = currMission.playersData[number].handicap;
		if (handicap > 70) {
			combo->pos = 0;
		} else if (handicap > 50) {
			combo->pos = 1;
		} else if (handicap > 30) {
			combo->pos = 2;
		} else {
			combo->pos = 3;
		}
	} else {
		combo->Show(false);
	}
}
void setupHC(CComboWindow* combo, int number, bool direction) {
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
	int handicap = 100;
	switch (newPos) {
		case 0:
			handicap = 100;
			break;
		case 1:
			handicap = 70;
			break;
		case 2:
			handicap = 50;
			break;
		case 3:
			handicap = 30;
			break;
	}
	gameShell->getNetClient()->changePlayerHandicap(number, handicap);
}
void setupHCButton(CShellWindow* pWnd, InterfaceEventCode code, int number) {
	if( code == EVENT_CREATEWND ) {
		CComboWindow *pCombo = (CComboWindow*) pWnd;
		pCombo->Array.push_back( "100%" );
		pCombo->Array.push_back( "70%" );
		pCombo->Array.push_back( "50%" );
		pCombo->Array.push_back( "30%" );
		pCombo->size = 4;
		pCombo->pos = 0;
	} else if (code == EVENT_UNPRESSED) {
		setupHC((CComboWindow*) pWnd, number, true);
	} else if (code == EVENT_RUNPRESSED) {
		setupHC((CComboWindow*) pWnd, number, false);
	}
}

void onMMLobbyHCButton(CShellWindow* pWnd, InterfaceEventCode code, int param) {
	setupHCButton(pWnd, code, pWnd->ID - SQSH_MM_LOBBY_PLAYER1_HC_BTN);
}

void setClr(CColorComboWindow* combo, CShellWindow* bg, int number) {
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
void setupClr(CColorComboWindow* combo, int number, bool direction) {
	int newPos = combo->pos;
	if (direction) {
		newPos++;
		if (newPos >= combo->size) {
			newPos = 0;
		}
	} else {
		newPos--;
		if (newPos < 0) {
			newPos = playerAllowedColorSize - 1;
		}
	}
	gameShell->getNetClient()->changePlayerColor(number, newPos, direction);
}
void setupClrButton(CShellWindow* pWnd, InterfaceEventCode code, int number) {
	if (code == EVENT_UNPRESSED) {
		setupClr((CColorComboWindow*) pWnd, number, true);
	} else if (code == EVENT_RUNPRESSED) {
		setupClr((CColorComboWindow*) pWnd, number, false);
	}
}
void onMMLobbyClrButton(CShellWindow* pWnd, InterfaceEventCode code, int param) {
	setupClrButton(pWnd, code, pWnd->ID - SQSH_MM_LOBBY_PLAYER1_CLR_BTN);
}

void setName(CShellPushButton* btn, int number) {
	MissionDescription& currMission = gameShell->getNetClient()->getCurrentMissionDescription();
	if (
			currMission.playerAmountScenarioMax > number
		&&	currMission.playersData[number].realPlayerType != REAL_PLAYER_TYPE_OPEN
		&&	currMission.playersData[number].realPlayerType != REAL_PLAYER_TYPE_CLOSE
		&&	currMission.playersData[number].realPlayerType != REAL_PLAYER_TYPE_AI
		) {

		btn->Show(true);
		btn->Enable(false);
		btn->setText(std::string(currMission.playersData[number].name()));
	} else {
		btn->Show(false);
	}
}
void onMMLobbyNameButton(CShellWindow* pWnd, InterfaceEventCode code, int param) {
}

void setReady(CShellWindow* wnd, int number) {
	MissionDescription& currMission = gameShell->getNetClient()->getCurrentMissionDescription();
	wnd->Show (
					currMission.playerAmountScenarioMax > number
				&&	currMission.playersData[number].realPlayerType != REAL_PLAYER_TYPE_OPEN
				&&	currMission.playersData[number].realPlayerType != REAL_PLAYER_TYPE_CLOSE
				&&	(currMission.playersData[number].realPlayerType == REAL_PLAYER_TYPE_AI || currMission.playersData[number].flag_playerStartReady) );
}

void setLobbyMapListVisible(bool visible) {
	_shellIconManager.GetWnd(SQSH_MM_LOBBY_MAP_LIST)->Show(visible);
	_shellIconManager.GetWnd(SQSH_MM_LOBBY_MAP_LIST_RAMKA1)->Show(visible);
	_shellIconManager.GetWnd(SQSH_MM_LOBBY_MAP_LIST_RAMKA2)->Show(visible);
	_shellIconManager.GetWnd(SQSH_MM_LOBBY_MAP_LIST_RAMKA3)->Show(visible);
	_shellIconManager.GetWnd(SQSH_MM_LOBBY_MAP_LIST_RAMKA4)->Show(visible);
}

void onMMLobby(CShellWindow* pWnd, InterfaceEventCode code, int param) {
	if (code == EVENT_DRAWWND) {
        CPushButton* btn = reinterpret_cast<CPushButton*>(pWnd);
        btn->setText(gameShell->getNetClient()->m_GameName);
        
		MissionDescription& currMission = gameShell->getNetClient()->getCurrentMissionDescription();
//		((CPushButton*)pWnd)->setText(currMission.missionDescription());

		if (gameShell->getNetClient()->isHost()) {
			setLobbyMapListVisible(true);
			_shellIconManager.GetWnd(SQSH_MM_LOBBY_GAME_MAP)->Show(0);
			((CShowMapWindow*)_shellIconManager.GetWnd(SQSH_MM_LOBBY_HOST_GAME_MAP))->setWorldID( currMission.worldID() );
			int pos = getMultiplayerMapNumber(currMission.saveName());
			if (pos != -1) {
				((CListBoxWindow*)_shellIconManager.GetWnd(SQSH_MM_LOBBY_MAP_LIST))->SetCurSelPassive(pos);
			}
		} else {
			setLobbyMapListVisible(false);
			((CShowMapWindow*)_shellIconManager.GetWnd(SQSH_MM_LOBBY_GAME_MAP))->setWorldID( currMission.worldID() );
			_shellIconManager.GetWnd(SQSH_MM_LOBBY_HOST_GAME_MAP)->Show(0);
		}
		((CTextWindow*)_shellIconManager.GetWnd(SQSH_MM_LOBBY_GAME_MAP_DESCR_TXT))->setText( currMission.missionDescription() );
	
		for (int i = 0; i < 4; i++) {
			setFrm(((CComboWindow*)_shellIconManager.GetWnd(SQSH_MM_LOBBY_PLAYER1_FRM_BTN + i)), i);
			setClr(((CColorComboWindow*)_shellIconManager.GetWnd(SQSH_MM_LOBBY_PLAYER1_CLR_BTN + i)), _shellIconManager.GetWnd(SQSH_MM_LOBBY_PLAYER1_CLR_BG + i), i);
			setSlot(((CComboWindow*)_shellIconManager.GetWnd(SQSH_MM_LOBBY_PLAYER1_SLOT_BTN + i)), i);
			setName(((CShellPushButton*)_shellIconManager.GetWnd(SQSH_MM_LOBBY_PLAYER1_NAME_BTN + i)), i);
			setClan(((CComboWindow*)_shellIconManager.GetWnd(SQSH_MM_LOBBY_PLAYER1_CLAN_BTN + i)), i);
			setHC(((CComboWindow*)_shellIconManager.GetWnd(SQSH_MM_LOBBY_PLAYER1_HC_BTN + i)), i);
			setReady(_shellIconManager.GetWnd(SQSH_MM_LOBBY_PLAYER1_READY_BTN + i), i);
		}
	}
}

void onMMLobbyStartButton(CShellWindow* pWnd, InterfaceEventCode code, int param) {
	if (code == EVENT_DRAWWND){
		MissionDescription& currMission = gameShell->getNetClient()->getCurrentMissionDescription();
		pWnd->Enable(currMission.playersAmount() > 1);
	} else if (code == EVENT_UNPRESSED){
//		fout < "StartLoadTheGame()\n";
//		gameShell->getNetClient()->StartLoadTheGame();
		_shellIconManager.SwitchMenuScreens( READY, READY );
	}
}
void onMMLobbyBackButton(CShellWindow* pWnd, InterfaceEventCode code, int param) {
	if( code == EVENT_UNPRESSED && intfCanHandleInput() ) {
		if (gameShell->isStartedWithMainmenu()) {
			_shellIconManager.SwitchMenuScreens( pWnd->m_pParent->ID, SQSH_MM_MULTIPLAYER_LIST_SCR );
		} else {
			gameShell->GameContinue = 0;
		}
	}
}
