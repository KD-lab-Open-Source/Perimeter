
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

//We can only show 4 simultaneous player in UI, so we use paging to allow seeing more
int lobbyPlayersPage = -1;

extern std::string getItemTextFromBase(const char *keyStr);

bool isEditAllowed(const MissionDescription& mission, int number) {
    if (gameShell->getNetClient()->isSaveGame()) {
        return false;
    }
    return mission.activePlayerID == mission.playersData[number].playerID
           || (gameShell->getNetClient()->isHost() && mission.playersData[number].realPlayerType == REAL_PLAYER_TYPE_AI);
}

///Updates buttons and page text controls, also ensures page is not out of bounds
void updateLobbyPageControls() {
    const MissionDescription& mission = gameShell->getNetClient()->getLobbyMissionDescription();
    int maxPage = (mission.playersAmountScenarioMax() - 1) / UI_PLAYERS_MAX;
    if (lobbyPlayersPage < 0) {
        lobbyPlayersPage = 0;
        for (int i = 0; i < mission.playerAmountScenarioMax; ++i) {
            if (mission.playersData[i].playerID == mission.activePlayerID) {
                lobbyPlayersPage = i / UI_PLAYERS_MAX;
                break;
            }
        }
    } else if (lobbyPlayersPage >= maxPage) {
        lobbyPlayersPage = maxPage;
    }

    CShellPushButton* prev_btn = ((CShellPushButton*) _shellIconManager.GetWnd(SQSH_MM_LOBBY_PLAYER_PAGE_PREV_BTN));
    prev_btn->Show(0 < lobbyPlayersPage);
    CShellPushButton* next_btn = ((CShellPushButton*) _shellIconManager.GetWnd(SQSH_MM_LOBBY_PLAYER_PAGE_NEXT_BTN));
    next_btn->Show(lobbyPlayersPage < maxPage);
    CTextWindow* textWindow = ((CTextWindow*) _shellIconManager.GetWnd(SQSH_MM_LOBBY_PLAYER_PAGE_TXT));
    textWindow->Show(0 < maxPage);
    if (maxPage) {
        std::string pagetext = std::to_string(lobbyPlayersPage + 1) + " / " + std::to_string(maxPage + 1);
        textWindow->SetText(pagetext.c_str());
    }
}

bool showPlayerControls(const MissionDescription& mission, int number) {
    return 0 <= number && number < mission.playerAmountScenarioMax
    && (mission.gameType_ == GT_MULTI_PLAYER_LOAD || mission.playersData[number].realPlayerType != REAL_PLAYER_TYPE_OPEN)
    && mission.playersData[number].realPlayerType != REAL_PLAYER_TYPE_CLOSE;
}

void setupMultiplayerLobby() {
    CListBoxWindow* list = (CListBoxWindow*)_shellIconManager.GetWnd(SQSH_MM_LOBBY_MAP_LIST);
    list->NewItem(1);
    list->Clear();
    if (!gameShell->getNetClient()->isSaveGame()) {
        for (int i = 0; i < multiplayerMaps.size(); i++) {
            std::string name = getMapName(multiplayerMaps[i].missionName().c_str());
            list->AddString(name.c_str(), 0);
        }
    }
    if (multiplayerMaps.empty()) {
        list->SetCurSel(-1);
    } else {
        list->SetCurSel(0);
    }

    ((ChatWindow*)_shellIconManager.GetWnd(SQSH_MM_LOBBY_CHAT_TEXT))->Clear();
}

void onMMMultiplayerGameSpeedSlider(CShellWindow* pWnd, InterfaceEventCode code, int param) {
    if ( code == EVENT_CREATEWND ) {
        CSliderWindow *pSlider = (CSliderWindow*) pWnd;
        pSlider->pos = (1.0f - 0.2f) / 4.8f;
    }
}

void onMMLobbyMapList(CShellWindow* pWnd, InterfaceEventCode code, int param) {
	if ( code == EVENT_PRESSED && intfCanHandleInput() ) {
		if (param >= 0 && param < multiplayerMaps.size()) {
			checkMissionDescription(param, multiplayerMaps, GT_MULTI_PLAYER_CREATE);
			std::string missionName = std::string("RESOURCE\\MULTIPLAYER\\") + multiplayerMaps[param].missionName();
			gameShell->getNetClient()->changeMap(missionName.c_str());
		}
	}		
}

void setFrm(CComboWindow* combo, int number) {
	const MissionDescription& currMission = gameShell->getNetClient()->getLobbyMissionDescription();
    if (showPlayerControls(currMission, number)) {
		combo->Show(true);
        combo->Enable(isEditAllowed(currMission, number));
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
    int number = pWnd->ID - SQSH_MM_LOBBY_PLAYER1_FRM_BTN;
    setupFrameButton(pWnd, code, UI_PLAYERS_MAX * lobbyPlayersPage + number, true);
}

void setSlot(CComboWindow* combo, int number) {
    const MissionDescription& currMission = gameShell->getNetClient()->getLobbyMissionDescription();
    const PlayerData& pd = currMission.playersData[number];
    bool isSave = gameShell->getNetClient()->isSaveGame();
	if ( 0 <= number && number < currMission.playerAmountScenarioMax) {
		combo->Show(true);
        bool isHost = gameShell->getNetClient()->isHost();
        bool slotEnable = isHost && currMission.activePlayerID != number;
        if (slotEnable && isSave
        && pd.realPlayerType != REAL_PLAYER_TYPE_OPEN
        && pd.realPlayerType != REAL_PLAYER_TYPE_PLAYER
        && pd.realPlayerType != REAL_PLAYER_TYPE_PLAYER_AI) {
            //Only kicking clients or adding/removing AI to empty player is allowed when loading a save
            slotEnable = false;
        }
        combo->Enable(slotEnable);
		switch (pd.realPlayerType) {
            case REAL_PLAYER_TYPE_OPEN:
				combo->pos = 0;
				break;
            case REAL_PLAYER_TYPE_PLAYER:
                combo->pos = 1;
                break;
			case REAL_PLAYER_TYPE_CLOSE:
				combo->pos = 2;
				break;
			case REAL_PLAYER_TYPE_AI:
				{
					switch (pd.difficulty) {
						case DIFFICULTY_EASY:
							combo->pos = 3;
							break;
						case DIFFICULTY_NORMAL:
							combo->pos = 4;
							break;
						case DIFFICULTY_HARD:
							combo->pos = 5;
							break;
                        default:
                            break;
					}
				}
				break;
            case REAL_PLAYER_TYPE_PLAYER_AI:
                combo->pos = 5;
                break;
            default:
                break;
        }
//		combo->pos = currMission.playersData[number].realPlayerType;
	} else {
		combo->Show(false);
	}
}

void setupSlot(CComboWindow* combo, int number, bool direction) {
    if (gameShell->getNetClient()->isSaveGame()) {
        const MissionDescription& currMission = gameShell->getNetClient()->getLobbyMissionDescription();
        const PlayerData& pd = currMission.playersData[number];
        switch (pd.realPlayerType) {
            case REAL_PLAYER_TYPE_OPEN:
                gameShell->getNetClient()->changeRealPlayerType(number, REAL_PLAYER_TYPE_PLAYER_AI);
                break;
            case REAL_PLAYER_TYPE_PLAYER:
            case REAL_PLAYER_TYPE_PLAYER_AI:
                gameShell->getNetClient()->changeRealPlayerType(number, REAL_PLAYER_TYPE_OPEN);
                break;
            default:
                break;
        }
    } else {
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
            default:
                break;
            case 0:
                gameShell->getNetClient()->changeRealPlayerType(number, REAL_PLAYER_TYPE_OPEN);
                break;
            case 1: //REAL_PLAYER_TYPE_PLAYER
                gameShell->getNetClient()->changeRealPlayerType(number, direction ? REAL_PLAYER_TYPE_CLOSE
                                                                                  : REAL_PLAYER_TYPE_OPEN);
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
    }
}

void onMMLobbySlotButton(CShellWindow* pWnd, InterfaceEventCode code, int param) {
    if( code == EVENT_CREATEWND ) {
        CComboWindow *pCombo = (CComboWindow*) pWnd;
        pCombo->Array.emplace_back( getItemTextFromBase("Open").c_str() );
        pCombo->Array.emplace_back( getItemTextFromBase("Player").c_str() );
        pCombo->Array.emplace_back( getItemTextFromBase("Closed").c_str() );
        pCombo->Array.emplace_back( getItemTextFromBase("AI (Easy)").c_str() );
        pCombo->Array.emplace_back( getItemTextFromBase("AI (Normal)").c_str() );
        pCombo->Array.emplace_back( getItemTextFromBase("AI (Hard)").c_str() );
        pCombo->size = pCombo->Array.size();
        pCombo->pos = 0;
    } else if (code == EVENT_UNPRESSED || code == EVENT_RUNPRESSED) {
        bool direction = code == EVENT_UNPRESSED;
        int number = pWnd->ID - SQSH_MM_LOBBY_PLAYER1_SLOT_BTN;
        setupSlot((CComboWindow*) pWnd, UI_PLAYERS_MAX * lobbyPlayersPage + number, direction);
    }
}

void setClan(CComboWindow* combo, int number) {
    const MissionDescription& currMission = gameShell->getNetClient()->getLobbyMissionDescription();
    if (showPlayerControls(currMission, number)) {
        std::string clan = getItemTextFromBase("Clan");
        if (combo->size != currMission.playerAmountScenarioMax) {
            combo->Array.clear();
            for (int i = 0; i < currMission.playerAmountScenarioMax; i++) {
                combo->Array.emplace_back(clan + std::to_string(i + 1));
            }
            combo->size = combo->Array.size();
        }
		combo->Show(true);
        combo->Enable(isEditAllowed(currMission, number));
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

void onMMLobbyClanButton(CShellWindow* pWnd, InterfaceEventCode code, int param) {
    if( code == EVENT_CREATEWND ) {
        CComboWindow *pCombo = (CComboWindow*) pWnd;
        pCombo->Array.clear();
        pCombo->size = pCombo->Array.size();
        pCombo->pos = 0;
    } else if (code == EVENT_UNPRESSED || code == EVENT_RUNPRESSED) {
        bool direction = code == EVENT_UNPRESSED;
        int number = pWnd->ID - SQSH_MM_LOBBY_PLAYER1_CLAN_BTN;
        setupClan((CComboWindow*) pWnd, UI_PLAYERS_MAX * lobbyPlayersPage + number, direction);
    }
}

void setHC(CComboWindow* combo, int number) {
	const MissionDescription& currMission = gameShell->getNetClient()->getLobbyMissionDescription();
    if (showPlayerControls(currMission, number)) {
		combo->Show(true);
        combo->Enable(isEditAllowed(currMission, number));

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

void onMMLobbyHCButton(CShellWindow* pWnd, InterfaceEventCode code, int param) {
    if( code == EVENT_CREATEWND ) {
        CComboWindow *pCombo = (CComboWindow*) pWnd;
        pCombo->Array.emplace_back( "100%" );
        pCombo->Array.emplace_back( "70%" );
        pCombo->Array.emplace_back( "50%" );
        pCombo->Array.emplace_back( "30%" );
        pCombo->size = 4;
        pCombo->pos = 0;
    } else if (code == EVENT_UNPRESSED || code == EVENT_RUNPRESSED) {
        bool direction = code == EVENT_UNPRESSED;
        int number = pWnd->ID - SQSH_MM_LOBBY_PLAYER1_HC_BTN;
        setupHC((CComboWindow*) pWnd, UI_PLAYERS_MAX * lobbyPlayersPage + number, direction);
    }
}

void setClr(CColorComboWindow* combo, CShellWindow* bg, int number) {
	const MissionDescription& currMission = gameShell->getNetClient()->getLobbyMissionDescription();
    if (showPlayerControls(currMission, number)) {
		bg->Show(true);
		combo->Show(true);
        combo->Enable(isEditAllowed(currMission, number));
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

void onMMLobbyClrButton(CShellWindow* pWnd, InterfaceEventCode code, int param) {
    if (code == EVENT_UNPRESSED || code == EVENT_RUNPRESSED) {
        bool direction = code == EVENT_UNPRESSED;
        int number = pWnd->ID - SQSH_MM_LOBBY_PLAYER1_CLR_BTN;
        setupClr((CColorComboWindow*) pWnd, UI_PLAYERS_MAX * lobbyPlayersPage + number, direction);
    }
}

void setName(CShellPushButton* btn, int number) {
	const MissionDescription& currMission = gameShell->getNetClient()->getLobbyMissionDescription();
    bool isSave = currMission.gameType_ == GT_MULTI_PLAYER_LOAD;
    const PlayerData& pd = currMission.playersData[number];
    if (showPlayerControls(currMission, number)
    && pd.realPlayerType != REAL_PLAYER_TYPE_AI) {
		btn->Show(true);
		btn->Enable(true);
        std::string text = pd.name();
        if (isSave) {
            if (pd.realPlayerType == REAL_PLAYER_TYPE_PLAYER) {
                text = pd.nameInitial() + std::string(" &FFFFFF(") + pd.name() + ")";
            }
        } else {
            text = std::string("&FFFFFF") + text;
        }
        btn->setText(text);
	} else {
		btn->Show(false);
	}
}

void setupName(CComboWindow* combo, int number) {
    gameShell->getNetClient()->changePlayerSeat(number);
}

void onMMLobbyNameButton(CShellWindow* pWnd, InterfaceEventCode code, int param) {
    if (code == EVENT_UNPRESSED || code == EVENT_RUNPRESSED) {
        int number = pWnd->ID - SQSH_MM_LOBBY_PLAYER1_NAME_BTN;
        setupName((CComboWindow*) pWnd, UI_PLAYERS_MAX * lobbyPlayersPage + number);
    }
}

void setReady(CShellWindow* wnd, int number) {
	const MissionDescription& currMission = gameShell->getNetClient()->getLobbyMissionDescription();
    bool showReady = currMission.playerAmountScenarioMax > number;
    if (showReady
    && currMission.playersData[number].realPlayerType != REAL_PLAYER_TYPE_AI
    && currMission.playersData[number].realPlayerType != REAL_PLAYER_TYPE_PLAYER_AI) {
        showReady = currMission.playersData[number].flag_playerStartReady;
    }
	wnd->Show(showReady);
}

void setLobbyMapListVisible(bool visible) {
	_shellIconManager.GetWnd(SQSH_MM_LOBBY_MAP_LIST)->Show(visible);
	_shellIconManager.GetWnd(SQSH_MM_LOBBY_MAP_LIST_RAMKA1)->Show(visible);
	_shellIconManager.GetWnd(SQSH_MM_LOBBY_MAP_LIST_RAMKA2)->Show(visible);
	_shellIconManager.GetWnd(SQSH_MM_LOBBY_MAP_LIST_RAMKA3)->Show(visible);
	_shellIconManager.GetWnd(SQSH_MM_LOBBY_MAP_LIST_RAMKA4)->Show(visible);
}

void onMMLobbyGameNameButton(CShellWindow* pWnd, InterfaceEventCode code, int param) {
	if (code == EVENT_DRAWWND) {
        CPushButton* btn = reinterpret_cast<CPushButton*>(pWnd);
        btn->setText(gameShell->getNetClient()->m_GameName);

		const MissionDescription& currMission = gameShell->getNetClient()->getLobbyMissionDescription();
        std::string missingContent = checkMissingContent(currMission);
        CTextWindow* descWnd = (CTextWindow*)_shellIconManager.GetWnd(SQSH_MM_LOBBY_GAME_MAP_DESCR_TXT);
		if (gameShell->getNetClient()->isHost() && !gameShell->getNetClient()->isSaveGame()) {
            descWnd->Show(0);
			setLobbyMapListVisible(true);
			_shellIconManager.GetWnd(SQSH_MM_LOBBY_GAME_MAP)->Show(0);
            CShowMapWindow* mapWindow = ((CShowMapWindow*)_shellIconManager.GetWnd(SQSH_MM_LOBBY_HOST_GAME_MAP));
            mapWindow->setWorldID( missingContent.empty() ? currMission.worldID() : -2 );
			int pos = getMultiplayerMapNumber(currMission.missionName());
			if (pos != -1) {
				((CListBoxWindow*)_shellIconManager.GetWnd(SQSH_MM_LOBBY_MAP_LIST))->SetCurSelPassive(pos);
			}
		} else {
			setLobbyMapListVisible(false);
            CShowMapWindow* mapWindow = ((CShowMapWindow*)_shellIconManager.GetWnd(SQSH_MM_LOBBY_GAME_MAP));
            mapWindow->setWorldID( missingContent.empty() ? currMission.worldID() : -2 );
			_shellIconManager.GetWnd(SQSH_MM_LOBBY_HOST_GAME_MAP)->Show(0);

            std::string text;
            if (missingContent.empty()) {
                text = currMission.missionName();
                /*
                std::string text = currMission.missionDescription();
                if (text.empty()) {
                    text = qdTextDB::instance().getText("Interface.Menu.Messages.Battle");
                }
                text = currMission.missionName() + "\n\n" + text;
                */
            } else {
                text = missingContent;
            }   
            descWnd->setText(text);
            descWnd->Show(1);
		}

        updateLobbyPageControls();
	
		for (int u = 0; u < UI_PLAYERS_MAX; u++) {
            int i = UI_PLAYERS_MAX * lobbyPlayersPage + u;
			setFrm(((CComboWindow*)_shellIconManager.GetWnd(SQSH_MM_LOBBY_PLAYER1_FRM_BTN + u)), i);
			setClr(
                ((CColorComboWindow*)_shellIconManager.GetWnd(SQSH_MM_LOBBY_PLAYER1_CLR_BTN + u)), 
                _shellIconManager.GetWnd(SQSH_MM_LOBBY_PLAYER1_CLR_BG + u),
                i
            );
			setSlot(((CComboWindow*)_shellIconManager.GetWnd(SQSH_MM_LOBBY_PLAYER1_SLOT_BTN + u)), i);
			setName(((CShellPushButton*)_shellIconManager.GetWnd(SQSH_MM_LOBBY_PLAYER1_NAME_BTN + u)), i);
			setClan(((CComboWindow*)_shellIconManager.GetWnd(SQSH_MM_LOBBY_PLAYER1_CLAN_BTN + u)), i);
			setHC(((CComboWindow*)_shellIconManager.GetWnd(SQSH_MM_LOBBY_PLAYER1_HC_BTN + u)), i);
			setReady(_shellIconManager.GetWnd(SQSH_MM_LOBBY_PLAYER1_READY_BTN + u), i);
		}
	}
}

void onMMLobbyStartButton(CShellWindow* pWnd, InterfaceEventCode code, int param) {
	if (code == EVENT_DRAWWND){
        const MissionDescription& currMission = gameShell->getNetClient()->getLobbyMissionDescription();
        bool enable = true;
        //Lock host enable until all slots are closed
        if (gameShell->getNetClient()->isHost() && currMission.gameType_ == GT_MULTI_PLAYER_LOAD) {
            for (int i = 0; i < currMission.playerAmountScenarioMax; ++i) {
                if (currMission.playersData[i].realPlayerType == REAL_PLAYER_TYPE_OPEN) {
                    enable = false;
                    break;
                }
            }
        }
		pWnd->Enable(enable);
	} else if (code == EVENT_UNPRESSED){
        const MissionDescription& currMission = gameShell->getNetClient()->getLobbyMissionDescription();
        bool state = !currMission.playersData[currMission.activePlayerID].flag_playerStartReady;
		gameShell->getNetClient()->StartLoadTheGame(state);
        //Set button state
        CPushButton* btn = reinterpret_cast<CPushButton*>(pWnd);
        btn->setText(qdTextDB::instance().getText(state ? "Interface.Menu.ButtonLabels.CANCEL" : "Interface.Menu.ButtonLabels.READY"));
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

void setLobbyPage(bool direction) {
    if (direction) {
        const MissionDescription& currMission = gameShell->getNetClient()->getLobbyMissionDescription();
        int maxPage = (currMission.playersAmountScenarioMax() - 1) / UI_PLAYERS_MAX;
        if (0 < maxPage && lobbyPlayersPage < maxPage) {
            lobbyPlayersPage = std::min(lobbyPlayersPage + 1, maxPage);
            updateLobbyPageControls();
        }
    } else {
        if (0 < lobbyPlayersPage) {
            lobbyPlayersPage--;
            updateLobbyPageControls();
        }
    }
}

void onMMLobbyPageNextButton(CShellWindow* pWnd, InterfaceEventCode code, int param) {
    if ((code == EVENT_UNPRESSED || code == EVENT_RUNPRESSED) && intfCanHandleInput()) {
        setLobbyPage(true);
    }
}

void onMMLobbyPagePrevButton(CShellWindow* pWnd, InterfaceEventCode code, int param) {
    if ((code == EVENT_UNPRESSED || code == EVENT_RUNPRESSED) && intfCanHandleInput()) {
        setLobbyPage(false);
    }
}

void onMMLobbyPageText(CShellWindow* pWnd, InterfaceEventCode code, int param) {
    if (code == EVENT_UNPRESSED && intfCanHandleInput()) {
        setLobbyPage(true);
    } else if (code == EVENT_RUNPRESSED && intfCanHandleInput()) {
        setLobbyPage(false);
    }
}
