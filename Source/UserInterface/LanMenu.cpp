
#include "StdAfx.h"
#include "Config.h"
#include "Runtime.h"
#include "terra.h"
#include "GenericControls.h"
#include "Universe.h"
#include "UniverseInterface.h"
#include "GameShell.h"
#include "PerimeterShellUI.h"
#include "Controls.h"
#include "MainMenu.h"

#include "BGScene.h"
#include "../HT/ht.h"

#include "MessageBox.h"

extern LogStream fout;
extern MpegSound gb_Music;

extern bool menuChangingDone;

extern vector<MissionDescription> multiplayerMaps;
extern MissionDescription missionToExec;
extern BGScene bgScene;

extern char _bCursorVisible;

CommandLineData cmdLineData;

CreateNetCenterWayType way;

string gameSpyRoomName = "";

string messageBoxText;
bool connRestored;

//unsigned int selectedLanGameID = 0;
GUID selectedLanGameID;
extern void checkMissionDescription(int index, vector<MissionDescription>& mVect);
extern string getItemTextFromBase(const char *keyStr);

string formatGameInfo(sGameHostInfo* info, bool oneLine) {
	if (info) {
		if (info->gameName.empty()) {
			if (oneLine) {
				return "&FF0000" + info->hostName;
			} else {
				return qdTextDB::instance().getText("Interface.Tips.MultiplayerInfoServerEmpty");
			}
		} else {
			static char cbTemp[1024];
			if (oneLine) {
				string name = info->gameName + "&FFFFFF";
				if (gameShell->getNetClient()->workMode == PNetCenter::PNCWM_ONLINE_P2P) {
					name += ":" + info->hostName + ":";
				}
				_shellIconManager.FormatMessageText(
					"<MultiplayerInfoOneLine>",
					cbTemp,
					name.c_str(),
					info->gameStatusInfo.currrentPlayers,
					info->gameStatusInfo.maximumPlayers,
					info->gameStatusInfo.ping,
					info->gameStatusInfo.flag_gameRun ? qdTextDB::instance().getText("Interface.Tips.MultiplayerInfoStarted") : "");
			} else {
				_shellIconManager.FormatMessageText(
					"<MultiplayerInfo>",
					cbTemp,
					(info->gameName + "&FFFFFF").c_str(),
					info->gameStatusInfo.currrentPlayers,
					info->gameStatusInfo.maximumPlayers,
					info->gameStatusInfo.ping,
					info->hostName.c_str(),
					info->gameStatusInfo.flag_gameRun ? qdTextDB::instance().getText("Interface.Tips.MultiplayerInfoStarted") : "");
			}
			return cbTemp;
		}
	} else {
		return "";
	}
}

//lan menu
STARFORCE_API_NEW void onMMGameList(CShellWindow* pWnd, InterfaceEventCode code, int param) {
	if (code == EVENT_CREATEWND) {
//		CListBoxWindow* list = dynamic_cast<CListBoxWindow*>(_shellIconManager.GetWnd(SQSH_MM_GAME_LIST));
		CListBoxWindow* list = (CListBoxWindow*)pWnd;
		list->NewItem(1);
		list->Clear();
	} else if (code == EVENT_DRAWWND) {
//		CListBoxWindow* listBox = dynamic_cast<CListBoxWindow*>(_shellIconManager.GetWnd(SQSH_MM_GAME_LIST));
		CListBoxWindow* listBox = (CListBoxWindow*)pWnd;
		listBox->Clear();

//		gameShell->NetClient->gameHostList;

//		typedef  LanGameInfo;
		if (gameShell->getNetClient()->getGameHostList().size() > 0) {

			vector<sGameHostInfo*>::iterator it;

			sGameHostInfo* selectedGame = gameShell->getNetClient()->getGameHostList().front();
			int selectIndex = 0;
			int i = 0;
			for (it = gameShell->getNetClient()->getGameHostList().begin(); it != gameShell->getNetClient()->getGameHostList().end(); it++, i++) {
				string name = formatGameInfo(*it, true);
				listBox->AddString( name.c_str(), 0 );
				if ((*it)->gameHostGUID == selectedLanGameID) {
					selectedGame = *it;
					selectIndex = i;
				}
			}
			listBox->SetCurSel(selectIndex);
			if (selectIndex == 0) {
				selectedLanGameID = selectedGame->gameHostGUID;
			}
			((CShowMapWindow*)_shellIconManager.GetWnd(SQSH_MM_LAN_GAME_MAP))->setWorldID( selectedGame->gameStatusInfo.worldID );
			((CTextWindow*)_shellIconManager.GetWnd(SQSH_MM_LAN_GAME_MAP_DESCR_TXT))->setText( formatGameInfo(selectedGame, false) );
			((CShellPushButton*)_shellIconManager.GetWnd(SQSH_MM_JOIN_BTN))->Enable(!selectedGame->gameName.empty());
		} else {
			((CShowMapWindow*)_shellIconManager.GetWnd(SQSH_MM_LAN_GAME_MAP))->setWorldID( -1 );
			((CTextWindow*)_shellIconManager.GetWnd(SQSH_MM_LAN_GAME_MAP_DESCR_TXT))->setText( "" );
			((CShellPushButton*)_shellIconManager.GetWnd(SQSH_MM_JOIN_BTN))->Enable(0);
		}
	} else if ( code == EVENT_PRESSED && intfCanHandleInput() ) {
		CListBoxWindow* listBox = (CListBoxWindow*)_shellIconManager.GetWnd(SQSH_MM_GAME_LIST);
		int pos = listBox->GetCurSel();
		if (pos >= 0 && pos < gameShell->getNetClient()->getGameHostList().size()) {
			vector<sGameHostInfo*>::iterator it = gameShell->getNetClient()->getGameHostList().begin();
			advance(it, pos);
			selectedLanGameID = (*it)->gameHostGUID;

			((CShowMapWindow*)_shellIconManager.GetWnd(SQSH_MM_LAN_GAME_MAP))->setWorldID( (*it)->gameStatusInfo.worldID );
			((CTextWindow*)_shellIconManager.GetWnd(SQSH_MM_LAN_GAME_MAP_DESCR_TXT))->setText( formatGameInfo(*it, false) );
		}
	}		
}

int exitCreating( float, float ) {
	hideMessageBox();
	return 1;
}

int createQuant( float, float ) {
	if (menuChangingDone) {
		if (multiplayerMaps.empty()) {
			loadMapVector(multiplayerMaps, "RESOURCE\\MULTIPLAYER\\", "RESOURCE\\MULTIPLAYER\\*.spg");
		}
		if (multiplayerMaps.empty()) {
			setMessageBoxTextID("Interface.Menu.Messages.UnknownError");
			showMessageBoxButtons();
		} else {
			CEditWindow* input = (CEditWindow*)_shellIconManager.GetWnd(SQSH_MM_LAN_PLAYER_NAME_INPUT);
			string gameName;
			if (gameSpyRoomName.empty()) {
				gameName += input->GetText();
			} else {
				gameName += gameSpyRoomName;
			}
			string missionName = string("RESOURCE\\MULTIPLAYER\\") + multiplayerMaps[0].missionName();
			gameShell->getNetClient()->CreateGame(gameName.c_str(), missionName.c_str(), input->GetText(), BELLIGERENT_EXODUS0, 0, 1, cmdLineData.password.c_str() );
		}
/*		
		CListBoxWindow* list = (CListBoxWindow*)_shellIconManager.GetWnd(SQSH_MM_LAN_MAP_LIST);
		CEditWindow* input = (CEditWindow*)_shellIconManager.GetWnd(SQSH_MM_LAN_PLAYER_NAME_INPUT);
		string gameName;
		if (gameSpyRoomName.empty()) {
			gameName+=input->GetText();
		} else {
			gameName+=gameSpyRoomName;
		}
		string missionName = string("RESOURCE\\MULTIPLAYER\\") + multiplayerMaps[list->GetCurSel()].missionName();
		gameShell->getNetClient()->CreateGame(gameName.c_str(), missionName.c_str(), input->GetText(), BELLIGERENT_EXODUS0, 0, 1, cmdLineData.password.c_str() );
*/
		return 0;
	}
	return 1;
}

void onMMLanCreateGameButton(CShellWindow* pWnd, InterfaceEventCode code, int param) {
	if( code == EVENT_UNPRESSED && intfCanHandleInput() ) {
		CEditWindow* input = (CEditWindow*)_shellIconManager.GetWnd(SQSH_MM_LAN_PLAYER_NAME_INPUT);
		if (input->getText().empty()) {
			setupOkMessageBox(0, 0, qdTextDB::instance().getText("Interface.Menu.Messages.NameEmpty"), MBOX_OK);
			showMessageBox();
		} else {
			putStringToReg(mainCurrUserRegFolder, regLanName, input->getText());
			setupOkMessageBox(exitCreating, 0, qdTextDB::instance().getText("Interface.Menu.Messages.Creating"), MBOX_EXIT, false);
			showMessageBox();
			_shellIconManager.AddDynamicHandler( createQuant, CBCODE_QUANT );
//			_shellIconManager.SwitchMenuScreens(pWnd->m_pParent->ID, SQSH_MM_CREATE_GAME_SCR);
		}
	}
}

int toLobbyQuant( float, float ) {
	if (menuChangingDone) {
		if (way == COMMAND_LINE_LOBBY) {
			_shellIconManager.SwitchMenuScreens(-1, SQSH_MM_LOBBY_SCR);
			_shellIconManager.SetModalWnd(0);
		} else {
//			fout < "toLobbyQuant SwitchMenuScreens(SQSH_MM_LAN_SCR, SQSH_MM_LOBBY_SCR)\n";
			_shellIconManager.SwitchMenuScreens(SQSH_MM_LAN_SCR, SQSH_MM_LOBBY_SCR);
		}
		return 0;
	}
	return 1;
}

int fromCreateToLobbyQuant( float, float ) {
	if (menuChangingDone) {
		if (way == COMMAND_LINE_CREATE_GAME) {
			_shellIconManager.SwitchMenuScreens(-1, SQSH_MM_LOBBY_SCR);
			_shellIconManager.SetModalWnd(0);
		} else {
			_shellIconManager.SwitchMenuScreens(SQSH_MM_LAN_SCR, SQSH_MM_LOBBY_SCR);
//			_shellIconManager.SwitchMenuScreens(SQSH_MM_CREATE_GAME_SCR, SQSH_MM_LOBBY_SCR);
		}
		return 0;
	}
	return 1;
}

int toLanQuant( float, float ) {
	if (menuChangingDone) {
		hideMessageBox();
//		fout < "toLanQuant AddDynamicHandler(toLobbyQuant)\n";
		_shellIconManager.AddDynamicHandler( toLobbyQuant, CBCODE_QUANT );
		return 0;
	}
	return 1;
}

int hideBoxToLobbyQuant( float, float ) {
	if (menuChangingDone) {
		hideMessageBox();
		_shellIconManager.AddDynamicHandler( fromCreateToLobbyQuant, CBCODE_QUANT );
		return 0;
	}
	return 1;
}

void GameShell::callBack_JoinGameReturnCode(e_JoinGameReturnCode retCode) {
	switch (retCode) {
		case JG_RC_OK:
//			fout < "callBack_JoinGameReturnCode AddDynamicHandler(toLanQuant)\n";
			_shellIconManager.AddDynamicHandler( toLanQuant, CBCODE_QUANT );
			break;
		case JG_RC_CONNECTION_ERR:
			setMessageBoxTextID("Interface.Menu.Messages.Multiplayer.ConnectionFailed");
			showMessageBoxButtons();
			break;
		case JG_RC_GAME_IS_RUN_ERR:
			setMessageBoxTextID("Interface.Menu.Messages.Multiplayer.AlreadyRun");
			showMessageBoxButtons();
			break;
		case JG_RC_GAME_IS_FULL_ERR:
			setMessageBoxTextID("Interface.Menu.Messages.Multiplayer.ServerFull");
			showMessageBoxButtons();
			break;
		case JG_RC_GAME_NOT_EQUAL_VERSION_ERR:
			setMessageBoxTextID("Interface.Menu.Messages.Multiplayer.IncorrectVersion");
			showMessageBoxButtons();
			break;
		case JG_RC_GAMESPY_CONNECTION_ERR:
			setMessageBoxTextID("Interface.Menu.Messages.Multiplayer.GameSpyConnectionFailed");
			showMessageBoxButtons();
			break;
		case JG_RC_GAMESPY_PASSWORD_ERR:
			setMessageBoxTextID("Interface.Menu.Messages.Multiplayer.IncorrectPassword");
			showMessageBoxButtons();
			break;
		default:
			xassert(0);
			setMessageBoxTextID("Interface.Menu.Messages.UnknownError");
			showMessageBoxButtons();
	}
}

void GameShell::callBack_CreateGameReturnCode(e_CreateGameReturnCode retCode) {
	switch (retCode) {
		case CG_RC_OK:
			_shellIconManager.AddDynamicHandler( hideBoxToLobbyQuant, CBCODE_QUANT );
			break;
		case CG_RC_CREATE_HOST_ERR:
			setMessageBoxTextID("Interface.Menu.Messages.Multiplayer.CreateFailed");
			showMessageBoxButtons();
			break;
		default:
			xassert(0);
			setMessageBoxTextID("Interface.Menu.Messages.UnknownError");
			showMessageBoxButtons();
	}
}

int interruptConnecting( float, float ) {
	//interrupt connecting
	gameShell->getNetClient()->FinishGame();
	gameShell->getNetClient()->StartFindHost();
	hideMessageBox();
	return 1;
}

int exitCreatingNetCenter( float, float ) {
	gameShell->stopNetClient();
	hideMessageBox();
	return 1;
}

int joinQuant( float, float ) {
	if (menuChangingDone) {
		CEditWindow* input = (CEditWindow*)_shellIconManager.GetWnd(SQSH_MM_LAN_PLAYER_NAME_INPUT);
		gameShell->getNetClient()->JoinGame(selectedLanGameID, input->GetText(), BELLIGERENT_EXODUS0, 0);
		return 0;
	}
	return 1;
}

void onMMJoinButton(CShellWindow* pWnd, InterfaceEventCode code, int param) {
	if( code == EVENT_UNPRESSED && intfCanHandleInput() ) {
		CEditWindow* input = (CEditWindow*)_shellIconManager.GetWnd(SQSH_MM_LAN_PLAYER_NAME_INPUT);
		if (input->getText().empty()) {
			setupOkMessageBox(0, 0, qdTextDB::instance().getText("Interface.Menu.Messages.NameEmpty"), MBOX_OK);
			showMessageBox();
		} else {
			putStringToReg(mainCurrUserRegFolder, regLanName, input->getText());

//			setupOkMessageBox(interruptConnecting, 0, qdTextDB::instance().getText("Interface.Menu.Messages.Connecting"), MBOX_EXIT);
			setupOkMessageBox(interruptConnecting, 0, qdTextDB::instance().getText("Interface.Menu.Messages.Connecting"), MBOX_EXIT, false);
			showMessageBox();

			_shellIconManager.AddDynamicHandler( joinQuant, CBCODE_QUANT );

//			gameShell->getNetClient()->JoinGame(selectedLanGameID, input->GetText(), BELLIGERENT_EXODUS0, 0);
//			_shellIconManager.SwitchMenuScreens(pWnd->m_pParent->ID, SQSH_MM_LOBBY_SCR);
		}

	}
}
void onMMLanBackButton(CShellWindow* pWnd, InterfaceEventCode code, int param) {
	if( code == EVENT_UNPRESSED && intfCanHandleInput() ) {
		if (gameShell->isStartedWithMainmenu()) {
			CEditWindow* input = (CEditWindow*)_shellIconManager.GetWnd(SQSH_MM_LAN_PLAYER_NAME_INPUT);
			if (!input->getText().empty()) {
				putStringToReg(mainCurrUserRegFolder, regLanName, input->getText());
			}
			_shellIconManager.SwitchMenuScreens( pWnd->m_pParent->ID, SQSH_MM_START_SCR );
		} else {
//			_shellIconManager.SwitchMenuScreens(pWnd->m_pParent->ID, SHOW_LAST_SPLASH);
			gameShell->GameContinue = 0;
		}
	}
}

//create game menu
void onMMLanMapList(CShellWindow* pWnd, InterfaceEventCode code, int param) {
	if ( code == EVENT_PRESSED && intfCanHandleInput() ) {
		CListBoxWindow* list = (CListBoxWindow*)_shellIconManager.GetWnd(SQSH_MM_LAN_MAP_LIST);
		int pos = list->GetCurSel();
		if (pos >= 0 && pos < multiplayerMaps.size()) {
			checkMissionDescription(pos, multiplayerMaps);
			((CShowMapWindow*)_shellIconManager.GetWnd(SQSH_MM_CREATE_GAME_MAP))->setWorldID( multiplayerMaps[pos].worldID() );
			((CTextWindow*)_shellIconManager.GetWnd(SQSH_MM_CREATE_GAME_MAP_DESCR_TXT))->setText( multiplayerMaps[pos].missionDescription() );
		}
	} else if ( code == EVENT_DOUBLECLICK && intfCanHandleInput() ) {
		int pos = ((CListBoxWindow*)_shellIconManager.GetWnd(SQSH_MM_LAN_MAP_LIST))->GetCurSel();
		if (pos != -1) {

//			setupOkMessageBox(exitCreating, 0, qdTextDB::instance().getText("Interface.Menu.Messages.Creating"), MBOX_EXIT);
			setupOkMessageBox(exitCreating, 0, qdTextDB::instance().getText("Interface.Menu.Messages.Creating"), MBOX_EXIT, false);
			showMessageBox();

			_shellIconManager.AddDynamicHandler( createQuant, CBCODE_QUANT );
		}
	} else if (code == EVENT_DRAWWND) {
		int pos = ((CListBoxWindow*)_shellIconManager.GetWnd(SQSH_MM_LAN_MAP_LIST))->GetCurSel();
		_shellIconManager.GetWnd(SQSH_MM_CREATE_BTN)->Enable(pos != -1);
	}		
}

void onMMLanGameSpeedSlider(CShellWindow* pWnd, InterfaceEventCode code, int param) {
	if ( code == EVENT_CREATEWND ) {
		CSliderWindow *pSlider = (CSliderWindow*) pWnd;
		pSlider->pos = (1.0f - 0.2f) / 4.8f;
	}
}

void onMMCreateButton(CShellWindow* pWnd, InterfaceEventCode code, int param) {
	if( code == EVENT_UNPRESSED && intfCanHandleInput() ) {
//		setupOkMessageBox(exitCreating, 0, qdTextDB::instance().getText("Interface.Menu.Messages.Creating"), MBOX_EXIT);
		setupOkMessageBox(exitCreating, 0, qdTextDB::instance().getText("Interface.Menu.Messages.Creating"), MBOX_EXIT, false);
		showMessageBox();
		_shellIconManager.AddDynamicHandler( createQuant, CBCODE_QUANT );
	}
}
void onMMCreateLanBackButton(CShellWindow* pWnd, InterfaceEventCode code, int param) {
	if( code == EVENT_UNPRESSED && intfCanHandleInput() ) {
		if (gameShell->isStartedWithMainmenu()) {
			_shellIconManager.SwitchMenuScreens( pWnd->m_pParent->ID, SQSH_MM_LAN_SCR );
		} else {
//			_shellIconManager.SwitchMenuScreens(pWnd->m_pParent->ID, SHOW_LAST_SPLASH);
			gameShell->GameContinue = 0;
		}
	}
}

//lobby
void onMMLobbyMapList(CShellWindow* pWnd, InterfaceEventCode code, int param) {
	if ( code == EVENT_PRESSED && intfCanHandleInput() ) {
		CListBoxWindow* list = (CListBoxWindow*)pWnd;
		if (param >= 0 && param < multiplayerMaps.size()) {
			checkMissionDescription(param, multiplayerMaps);
			string missionName = string("RESOURCE\\MULTIPLAYER\\") + multiplayerMaps[param].missionName();
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
		switch (currMission.playersData[number].belligerent) {
			case BELLIGERENT_EXODUS0:
			case BELLIGERENT_EXODUS1:
			case BELLIGERENT_EXODUS2:
			case BELLIGERENT_EXODUS3:
			case BELLIGERENT_EXODUS4:
				combo->pos = 0;
				break;
			case BELLIGERENT_HARKBACKHOOD0:
			case BELLIGERENT_HARKBACKHOOD1:
				#if defined(_DEMO_) || defined(_PERIMETER_ADDON_)
					combo->pos = 2;
				#else
					combo->pos = 0;
					gameShell->getNetClient()->changePlayerBelligerent(number, BELLIGERENT_EXODUS0);
				#endif
				break;
			case BELLIGERENT_EMPIRE0:
			case BELLIGERENT_EMPIRE1:
			case BELLIGERENT_EMPIRE2:
			case BELLIGERENT_EMPIRE3:
			case BELLIGERENT_EMPIRE4:
			default:
				combo->pos = 1;
		}
	} else {
		combo->Show(false);
	}
}
void setupFrm(CComboWindow* combo, int number, bool direction) {
	terBelligerent curBelligerent = BELLIGERENT_EXODUS0;
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
#ifndef _DEMO_
		case 2:
			curBelligerent = BELLIGERENT_HARKBACKHOOD0;
			break;
#endif
	}
	gameShell->getNetClient()->changePlayerBelligerent(number, curBelligerent);
}
void setupFrmButton(CShellWindow* pWnd, InterfaceEventCode code, int number) {
	if( code == EVENT_CREATEWND ) {
		CComboWindow *pCombo = (CComboWindow*) pWnd;
		pCombo->Array.push_back( getItemTextFromBase("Exodus").c_str() );
		pCombo->Array.push_back( getItemTextFromBase("Empire").c_str() );
		#ifndef _DEMO_
			pCombo->Array.push_back( getItemTextFromBase("Harkback").c_str() );
		#endif
		pCombo->size = pCombo->Array.size();
		pCombo->pos = 0;
	} else if (code == EVENT_UNPRESSED) {
		setupFrm((CComboWindow*) pWnd, number, true);
	} else if (code == EVENT_RUNPRESSED) {
		setupFrm((CComboWindow*) pWnd, number, false);
	}
}
void onMMLobbyFrmButton(CShellWindow* pWnd, InterfaceEventCode code, int param) {
	setupFrmButton(pWnd, code, pWnd->ID - SQSH_MM_LOBBY_PLAYER1_FRM_BTN);
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
#ifndef _DEMO_
						case DIFFICULTY_HARD:
							combo->pos = 5;
							break;
#endif
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
#ifndef _DEMO_
		case 5:
			gameShell->getNetClient()->changeRealPlayerType(number, REAL_PLAYER_TYPE_AI);
			gameShell->getNetClient()->changePlayerDifficulty(number, DIFFICULTY_HARD);
			break;
#endif
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
		#ifndef _DEMO_
			pCombo->Array.push_back( getItemTextFromBase("AI (Hard)").c_str() );
		#endif
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
		string clan = getItemTextFromBase("Clan");
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
			newPos = combo->size - 1;
		}
	}
	gameShell->getNetClient()->changePlayerColor(number, newPos);
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
		btn->setText(string(currMission.playersData[number].name()));
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
int getMultiplayerMapNumber(const string& saveName) {
	for (int i = 0, s = multiplayerMaps.size(); i < s; i++) {
		if (saveName == multiplayerMaps[i].saveName()) {
			return i;
		}
	}
	return -1;
}
void onMMLobby(CShellWindow* pWnd, InterfaceEventCode code, int param) {
	if (code == EVENT_DRAWWND) {
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
			_shellIconManager.SwitchMenuScreens( pWnd->m_pParent->ID, SQSH_MM_LAN_SCR );
		} else {
			gameShell->GameContinue = 0;
		}
	}
}
int SwitchLanToLoadQuant( float, float ) {
	if (menuChangingDone) {
		gameShell->currentSingleProfile.setLastGameType(UserSingleProfile::LAN);

		gb_Music.FadeVolume(_fEffectButtonTotalTime*0.001f);
		_shellIconManager.SwitchMenuScreens(SQSH_MM_LOBBY_SCR, SQSH_MM_LOADING_MISSION_SCR);
		return 0;
	}
	return 1;
}

int exitQuant( float, float ) {
	if (menuChangingDone) {
		//meassge to NetCenter
//		gameShell->getNetClient()->FinishGame();
//		gameShell->getNetClient()->DisconnectAndStartFindHost();
		exitToInterfaceMessage(0);
		return 0;
	}
	return 1;
}

int returnToLanScreenQuant( float, float ) {
	if (menuChangingDone) {
		_shellIconManager.SwitchMenuScreens( LAST_VISIBLE, SQSH_MM_LAN_SCR );
		return 0;
	}
	return 1;
}

int exitToLanScreenAction( float, float ) {
	if (!gameShell->isStartedWithMainmenu()) {
		gameShell->GameContinue = 0;
	} else if (universe()) {
		hideMessageBox();
		_shellIconManager.AddDynamicHandler( exitQuant, CBCODE_QUANT );
//		HTManager::instance()->GameClose();
//		_shellIconManager.LoadControlsGroup(SHELL_LOAD_GROUP_MENU);
//		_shellIconManager.SwitchMenuScreens(-1, SQSH_MM_LAN_SCR);
	} else {
		hideMessageBox();
		_shellIconManager.AddDynamicHandler( returnToLanScreenQuant, CBCODE_QUANT );
	}
	return 1;
}

int finishAndGoToLanQuant( float, float ) {
	if (menuChangingDone) {
		setupOkMessageBox(exitToLanScreenAction, 0, qdTextDB::instance().getText("Interface.Menu.Messages.CantFindMap"), MBOX_EXIT);
		gameShell->getNetClient()->FinishGame();
		gameShell->getNetClient()->StartFindHost();
		showMessageBox();
		return 0;
	}
	return 1;
}

void GameShell::LANGameStart(const MissionDescription& mission) {
//	fout < "LANGameStart()\n";
	if ( !isWorldIDValid(mission.worldID()) || getMultiplayerMapNumber(mission.saveName()) == -1 ) {
		_shellIconManager.AddDynamicHandler(finishAndGoToLanQuant, CBCODE_QUANT);
/*
		setupOkMessageBox(exitToLanScreenAction, 0, qdTextDB::instance().getText("Interface.Menu.Messages.CantFindMap"), MBOX_EXIT);
		gameShell->getNetClient()->FinishGame();
		gameShell->getNetClient()->StartFindHost();
		showMessageBox();
*/
	} else {
		missionToExec = mission;
		_shellIconManager.AddDynamicHandler(SwitchLanToLoadQuant, CBCODE_QUANT);
	}
/*
	missionToExec = mission;

	currentSingleProfile.setLastGameType(UserSingleProfile::LAN);

	gb_Music.FadeVolume(_fEffectButtonTotalTime*0.001f);
	_shellIconManager.SwitchMenuScreens(SQSH_MM_LOBBY_SCR, SQSH_MM_LOADING_MISSION_SCR);
*/
}

//////////show/hide////////////////////

int exitAction( float, float ) {
	hideMessageBox();
	_shellIconManager.AddDynamicHandler( exitQuant, CBCODE_QUANT );
	return 1;
}

int showMessageInGameQuant( float, float ) {
	if (menuChangingDone) {
		xassert(_shellIconManager.GetWnd(SQSH_MM_SUBMIT_DIALOG_SCR));
		if (_shellIconManager.GetWnd(SQSH_MM_SUBMIT_DIALOG_SCR)) {
			setupOkMessageBox(exitAction, 0, messageBoxText, MBOX_EXIT);
			gameShell->prepareForInGameMenu();
			showMessageBox();
		}
		return 0;
	}
	return 1;
}

void GameShell::showConnectFailedInGame(const string& playerList) {
	messageBoxText = qdTextDB::instance().getText("Interface.Menu.Messages.Multiplayer.WaitingForPlayers") + playerList;
	_shellIconManager.AddDynamicHandler(showMessageInGameQuant, CBCODE_QUANT);
}

int hideMessageInGameQuant( float, float ) {
	if (menuChangingDone) {
		xassert(_shellIconManager.GetWnd(SQSH_MM_SUBMIT_DIALOG_SCR));
		if (_shellIconManager.GetWnd(SQSH_MM_SUBMIT_DIALOG_SCR)) {
			hideMessageBox();
		}
		if (!connRestored) {
			_shellIconManager.AddDynamicHandler( exitQuant, CBCODE_QUANT );
		}
		return 0;
	}
	return 1;
}

void GameShell::hideConnectFailedInGame(bool connectionRestored) {
//	if ( _shellIconManager.HasDynamicHandler(showMessageInGameQuant, CBCODE_QUANT) ) {
//		_shellIconManager.DelDynamicHandler(showMessageInGameQuant, CBCODE_QUANT);
//	} else {
//	}
	connRestored = connectionRestored;
	_shellIconManager.AddDynamicHandler( hideMessageInGameQuant, CBCODE_QUANT );
}

//////////cmd///////////

int exitCmdLine( float, float ) {
	gameShell->GameContinue = 0;
	return 1;
}

int cmdLineCreateHost( float, float ) {
	if (menuChangingDone) {
		gameSpyRoomName = cmdLineData.roomName;
		way = COMMAND_LINE_CREATE_GAME;
		gameShell->createNetClient(
			cmdLineData.p2p ? PNetCenter::PNCWM_ONLINE_P2P : PNetCenter::PNCWM_ONLINE_GAMESPY,
			cmdLineData.name.c_str(),
			cmdLineData.ip.c_str(),
			cmdLineData.password.c_str() );
//		startWithScreen(SQSH_MM_CREATE_GAME_SCR);
		return 0;
	}
	return 1;
}

int cmdLineJoin( float, float ) {
	if (menuChangingDone) {
		way = COMMAND_LINE_LOBBY;
		gameShell->createNetClient(
			cmdLineData.p2p ? PNetCenter::PNCWM_ONLINE_P2P : PNetCenter::PNCWM_ONLINE_GAMESPY,
			cmdLineData.name.c_str(),
			cmdLineData.ip.c_str(),
			cmdLineData.password.c_str() );
		selectedLanGameID = cmdLineData.gameID;
		return 0;
	}
	return 1;
}

int fromBoxToMultiplayerQuant( float, float ) {
	if (menuChangingDone) {
		switchToMultiplayer(way);
		return 0;
	}
	return 1;
}

int hideBoxNetCenterQuant( float, float ) {
	if (menuChangingDone) {
		if (way == COMMAND_LINE_LOBBY || way == COMMAND_LINE_CREATE_GAME) {
			switchToMultiplayer(way);
		} else {
			hideMessageBox();
			_shellIconManager.AddDynamicHandler( fromBoxToMultiplayerQuant, CBCODE_QUANT );
		}
		return 0;
	}
	return 1;
}

void GameShell::startOnline(CommandLineData data) {
	_bCursorVisible = 1;
	_WaitCursor();
	cmdLineData = data;
	putStringToReg(mainCurrUserRegFolder, regLanName, data.name);
	_shellIconManager.LoadControlsGroup(SHELL_LOAD_GROUP_MENU);
	CEditWindow* input = (CEditWindow*)_shellIconManager.GetWnd(SQSH_MM_LAN_PLAYER_NAME_INPUT);
	input->SetText(data.name.c_str());

//	setupOkMessageBox(exitCmdLine, 0, qdTextDB::instance().getText("Interface.Menu.Messages.Connecting"), MBOX_EXIT);
	setupOkMessageBox(exitCmdLine, 0, qdTextDB::instance().getText("Interface.Menu.Messages.Connecting"), MBOX_EXIT, false);
	prepareForInGameMenu();
	showMessageBox();

	if (data.host) {
		_shellIconManager.AddDynamicHandler( cmdLineCreateHost, CBCODE_QUANT );
	} else {
		_shellIconManager.AddDynamicHandler( cmdLineJoin, CBCODE_QUANT );
	}
}

void GameShell::callBack_NetCenterConstructorReturnCode(e_NetCenterConstructorReturnCode retCode) {
	switch (retCode) {
		case NCC_RC_OK:
			_shellIconManager.AddDynamicHandler( hideBoxNetCenterQuant, CBCODE_QUANT );
			break;
		case NCC_RC_GAMESPY_CONNECT_ERR:
			setMessageBoxTextID("Interface.Menu.Messages.Multiplayer.GameSpyConnectionFailed");
			showMessageBoxButtons();
			break;
		case NCC_RC_GAMESPY_NICK_ERR:
			setMessageBoxTextID("Interface.Menu.Messages.Multiplayer.NickError");
			stopNetClient();
			showMessageBoxButtons();
			break;
		case NCC_RC_DIRECTPLAY_INIT_ERR:
			setMessageBoxTextID("Interface.Menu.Messages.Multiplayer.DirectPlayInitError");
			showMessageBoxButtons();
			break;
		default:
			xassert(0);
			setMessageBoxTextID("Interface.Menu.Messages.UnknownError");
			showMessageBoxButtons();
	}
}

STARFORCE_API void switchToMultiplayer(CreateNetCenterWayType way) {
	switch (way) {
		case COMMAND_LINE_CREATE_GAME:
			{
//				_shellIconManager.SwitchMenuScreens(-1, SQSH_MM_CREATE_GAME_SCR);
//				_shellIconManager.SetModalWnd(0);
				if(multiplayerMaps.empty()){
					loadMapVector(multiplayerMaps, "RESOURCE\\MULTIPLAYER\\", "RESOURCE\\MULTIPLAYER\\*.spg");
				}
				if (multiplayerMaps.empty()) {
					setMessageBoxTextID("Interface.Menu.Messages.UnknownError");
					showMessageBoxButtons();
				} else {
					setMessageBoxTextID("Interface.Menu.Messages.Creating");
					string gameName;
					if (gameSpyRoomName.empty()) {
						gameName = cmdLineData.name;
					} else {
						gameName = gameSpyRoomName;
					}
					string missionName = string("RESOURCE\\MULTIPLAYER\\") + multiplayerMaps[0].missionName();
					gameShell->getNetClient()->CreateGame(gameName.c_str(), missionName.c_str(), cmdLineData.name.c_str(), BELLIGERENT_EXODUS0, 0, 1, cmdLineData.password.c_str() );
				}
			}
			break;
		case COMMAND_LINE_LOBBY:
			gameShell->getNetClient()->JoinGame(cmdLineData.ip.c_str(), cmdLineData.name.c_str(), BELLIGERENT_EXODUS0, 0, cmdLineData.password.c_str());
			showMessageBoxButtons();
			break;
		case MENU_LAN:
			_shellIconManager.SwitchMenuScreens(SQSH_MM_START_SCR, SQSH_MM_LAN_SCR);
			break;
		case MENU_ONLINE:
			_shellIconManager.SwitchMenuScreens(SQSH_MM_NAME_INPUT_SCR, SQSH_MM_LAN_SCR);
			break;
		default:
			xassert(0);
			_shellIconManager.SwitchMenuScreens(SQSH_MM_NAME_INPUT_SCR, SQSH_MM_LAN_SCR);
	}
}

//name input
////////create online net center//////////
int createOnlineNetCenter( float, float ) {
	if (menuChangingDone) {
		CEditWindow* ipInput = (CEditWindow*)_shellIconManager.GetWnd(SQSH_MM_IP_INPUT);
		if ( !checkInetAddress(ipInput->getText().c_str()) ) {
			setMessageBoxTextID("Interface.Menu.Messages.Multiplayer.WrongIPAdress");
			showMessageBoxButtons();
		} else {
			CEditWindow* input = (CEditWindow*)_shellIconManager.GetWnd(SQSH_MM_PLAYER_NAME_INPUT);
			CComboWindow* pCombo = (CComboWindow*) _shellIconManager.GetWnd(SQSH_MM_CONNECTION_TYPE_COMBO);
			way = MENU_ONLINE;
			gameShell->createNetClient(
				pCombo->pos ? PNetCenter::PNCWM_ONLINE_GAMESPY : PNetCenter::PNCWM_ONLINE_P2P,
				input->getText().c_str(),
				ipInput->getText().c_str() );
			_shellIconManager.GetWnd(SQSH_MM_LAN_PLAYER_NAME_INPUT)->Enable(false);
		}
		return 0;
	}
	return 1;
}
void onMMApplyNameBtn(CShellWindow* pWnd, InterfaceEventCode code, int param) {
	if( code == EVENT_UNPRESSED && intfCanHandleInput() ) {
		CEditWindow* input = (CEditWindow*)_shellIconManager.GetWnd(SQSH_MM_PLAYER_NAME_INPUT);
		CEditWindow* ipInput = (CEditWindow*)_shellIconManager.GetWnd(SQSH_MM_IP_INPUT);
		CComboWindow* pCombo = (CComboWindow*) _shellIconManager.GetWnd(SQSH_MM_CONNECTION_TYPE_COMBO);
		if (input->getText().empty()) {

			setupOkMessageBox(0, 0, qdTextDB::instance().getText("Interface.Menu.Messages.NameEmpty"), MBOX_OK);
			showMessageBox();
/*
		} else if (!pCombo->pos && ipInput->getText().empty()) {

			setupOkMessageBox(0, 0, qdTextDB::instance().getText("Interface.Menu.Messages.IPEmpty"), MBOX_OK);
			showMessageBox();
*/
		} else {
			putStringToReg(mainCurrUserRegFolder, regLanName, input->getText());
			//TODO: put ipInput->getText() if pCombo->pos

//			setupOkMessageBox(exitCreatingNetCenter, 0, qdTextDB::instance().getText("Interface.Menu.Messages.Connecting"), MBOX_BACK);
			setupOkMessageBox(exitCreatingNetCenter, 0, qdTextDB::instance().getText("Interface.Menu.Messages.Connecting"), MBOX_BACK, false);
			showMessageBox();

			_shellIconManager.AddDynamicHandler( createOnlineNetCenter, CBCODE_QUANT );
//			switchToMultiplayer(MENU_ONLINE);
//			gameShell->createNetClient(
//				pCombo->pos ? PNetCenter::PNCWM_ONLINE_P2P : PNetCenter::PNCWM_ONLINE_GAMESPY,
//				input->getText().c_str(),
//				ipInput->getText().c_str() );
//			switchToMultiplayerMenu(pWnd, true);
		}
	}
}

void onMMConnectionTypeCombo(CShellWindow* pWnd, InterfaceEventCode code, int param) {
	if( code == EVENT_CREATEWND ) {
		CComboWindow *pCombo = (CComboWindow*) pWnd;
		pCombo->Array.push_back( getItemTextFromBase("Peer2Peer").c_str() );
		pCombo->Array.push_back( getItemTextFromBase("GameSpy").c_str() );
		pCombo->size = 2;
		pCombo->pos = 0;
//	} else if ( (code == EVENT_UNPRESSED || code == EVENT_RUNPRESSED) && intfCanHandleInput() )	{
//		CComboWindow *pCombo = (CComboWindow*) pWnd;
//		_shellIconManager.GetWnd(SQSH_MM_IP_BTN)->Enable(pCombo->pos);
//		_shellIconManager.GetWnd(SQSH_MM_IP_INPUT)->Enable(pCombo->pos);
	}
}

////////create lan net center//////////
int createLanNetCenter( float, float ) {
	if (menuChangingDone) {
		way = MENU_LAN;
		gameShell->createNetClient(PNetCenter::PNCWM_LAN);
		_shellIconManager.GetWnd(SQSH_MM_LAN_PLAYER_NAME_INPUT)->Enable(true);
		return 0;
	}
	return 1;
}

void onMMLanButton(CShellWindow* pWnd, InterfaceEventCode code, int param) {
#ifndef _SINGLE_DEMO_
	if( code == EVENT_UNPRESSED && intfCanHandleInput() ) {

//		setupOkMessageBox(exitCreatingNetCenter, 0, qdTextDB::instance().getText("Interface.Menu.Messages.Connecting"), MBOX_BACK);
		setupOkMessageBox(exitCreatingNetCenter, 0, qdTextDB::instance().getText("Interface.Menu.Messages.Connecting"), MBOX_BACK, false);
		showMessageBox();

		_shellIconManager.AddDynamicHandler( createLanNetCenter, CBCODE_QUANT );
//		switchToMultiplayer(MENU_LAN);
//		gameShell->createNetClient(PNetCenter::PNCWM_LAN);
//		switchToMultiplayerMenu(pWnd, false);
	}
#endif //_SINGLE_DEMO_
}

//////abnormalNetCenterTermination/////

int returnToStartScreenQuant( float, float ) {
	if (menuChangingDone) {
		_shellIconManager.SwitchMenuScreens( LAST_VISIBLE, SQSH_MM_START_SCR );
		return 0;
	}
	return 1;
}

int exitToStartScreenAction( float, float ) {
	if (!gameShell->isStartedWithMainmenu()) {
		gameShell->GameContinue = 0;
	} else if (universe()) {
		HTManager::instance()->GameClose();
		_shellIconManager.LoadControlsGroup(SHELL_LOAD_GROUP_MENU);
		_shellIconManager.SwitchMenuScreens(-1, SQSH_MM_START_SCR);
	} else {
		hideMessageBox();
		_shellIconManager.AddDynamicHandler( returnToStartScreenQuant, CBCODE_QUANT );
	}
	return 1;
}

int showTerminationQuant( float, float ) {
	if (menuChangingDone) {
		setupOkMessageBox(exitToStartScreenAction, 0, qdTextDB::instance().getText("Interface.Menu.Messages.UnknownError"), MBOX_EXIT);
		gameShell->prepareForInGameMenu();
		showMessageBox();
		return 0;
	}
	return 1;
}

void GameShell::abnormalNetCenterTermination() {
//	stopNetClient();
	_shellIconManager.AddDynamicHandler(showTerminationQuant, CBCODE_QUANT);
}

//////////generalErrorOccured/////////////

GameShell::GeneralErrorType gtError;

int showTerminationToLanQuant( float, float ) {
	if (menuChangingDone) {
		string textID;
		switch (gtError) {
			case GameShell::GENERAL_CONNECTION_FAILED:
				textID = "Interface.Menu.Messages.Multiplayer.ConnectionFailed";
				break;
			case GameShell::HOST_TERMINATED:
//				if (universe()) {
//					textID = "Interface.Menu.Messages.UnknownError";
//				} else {
					textID = "Interface.Menu.Messages.Multiplayer.ClientDropped";
//				}
				break;
			default:
				textID = "Interface.Menu.Messages.UnknownError";
		}


		setupOkMessageBox(exitToLanScreenAction, 0, qdTextDB::instance().getText(textID.c_str()), MBOX_EXIT);
		gameShell->prepareForInGameMenu();
//		fout < "showTerminationToLanQuant showMessageBox()\n";
		showMessageBox();
		return 0;
	}
	return 1;
}

void GameShell::generalErrorOccured(GeneralErrorType error) {
	gtError = error;
	gameShell->getNetClient()->FinishGame();
//	fout < "generalErrorOccured AddDynamicHandler(showTerminationToLanQuant)\n";
	_shellIconManager.AddDynamicHandler(showTerminationToLanQuant, CBCODE_QUANT);
}

//----------------------------
string colorComponentToString(unsigned char component) {
	char buff[5];
	sprintf(buff, "%x", component);
	if (strlen(buff) > 1) {
		return buff;
	} else {
		return string("0") + buff;
	}
}

void onMMLobbyChatInputButton(CShellWindow* pWnd, InterfaceEventCode code, int param) {
	if( code == EVENT_DOUBLECLICK && intfCanHandleInput() ) {
		MissionDescription& currMission = gameShell->getNetClient()->getCurrentMissionDescription();
		string name = currMission.getActivePlayerData().name();
		int colorIndex = currMission.getActivePlayerData().colorIndex;
		sColor4c activePlayerColor = sColor4c( sColor4f(playerColors[colorIndex].unitColor) );

		string strToSay =
				"&"
			+	colorComponentToString(activePlayerColor.r)
			+	colorComponentToString(activePlayerColor.g)
			+	colorComponentToString(activePlayerColor.b)
			+	name
			+	"&FFFFFF: "
			+	((CEditWindow*)pWnd)->getText();

//		gameShell->addStringToChatWindow(strToSay);
		gameShell->getNetClient()->chatMessage(-1, strToSay.c_str());
		((CEditWindow*)pWnd)->SetText("");
	}
}

void onMMInGameChatInputButton(CShellWindow* pWnd, InterfaceEventCode code, int param) {
	if( code == EVENT_DOUBLECLICK && intfCanHandleInput() ) {
		terPlayer* activePlayer = universe()->activePlayer();
		if (activePlayer) {
			CChatInGameEditWindow* chatInput = (CChatInGameEditWindow*) pWnd;
			string name = activePlayer->name();
			int colorIndex = activePlayer->colorIndex();
			sColor4c activePlayerColor = sColor4c( sColor4f(playerColors[colorIndex].unitColor) );

			string strToSay =
					"&"
				+	colorComponentToString(activePlayerColor.r)
				+	colorComponentToString(activePlayerColor.g)
				+	colorComponentToString(activePlayerColor.b)
				+	name
				+	"&FFFFFF"
				+	chatInput->getModePostfix()
				+	": "
				+	((CEditWindow*)pWnd)->getText();

			//gameShell->addStringToChatWindow(strToSay);
			gameShell->getNetClient()->chatMessage(chatInput->alliesOnlyMode ? activePlayer->clan() : -1, strToSay.c_str());
			chatInput->SetText("");
		}
	}
}


string toChatStr;

int addStringToChatWindowQuant( float, float ) {
	ChatWindow* chatWnd = (ChatWindow*)_shellIconManager.GetWnd(SQSH_MM_LOBBY_CHAT_TEXT);
	chatWnd->AddString(toChatStr.c_str());
	return 0;
}

int addStringToChatHintWindowQuant( float, float ) {
	_shellIconManager.addChatString(toChatStr);
	return 0;
}

void GameShell::addStringToChatWindow(int clanNum, const char* newString) {
	toChatStr = newString;
	if (_shellIconManager.GetWnd(SQSH_MM_LOBBY_CHAT_TEXT)) {
		_shellIconManager.AddDynamicHandler( addStringToChatWindowQuant, CBCODE_QUANT );
	} else {
		terPlayer* activePlayer = universe()->activePlayer();
		if (clanNum == -1 || (activePlayer && activePlayer->clan() == clanNum)) {
			_shellIconManager.AddDynamicHandler( addStringToChatHintWindowQuant, CBCODE_QUANT );
		}
	}
}
