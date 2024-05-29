#include "StdAfx.h"
#include "GameShell.h"
#include "PerimeterShellUI.h"
#include "MessageBox.h"
#include "MultiplayerCommon.h"
#include "MainMenu.h"
#include "ServerList.h"
#include "GameContent.h"
#include "codepages/codepages.h"

///Command line stuff

CommandLineData cmdLineData;

int exitCmdLine( float, float ) {
    gameShell->terminate();
    return 1;
}

int startCmdlineQuant(float, float ) {
    if (menuChangingDone) {
        gameShell->prepareNetClient();
        
        NetAddress conn;
        if (cmdLineData.address.empty() && cmdLineData.roomID != 0) {
            getPrimaryNetRelayAddress(cmdLineData.address);
        }
        if (cmdLineData.address.empty()
        || !NetAddress::resolve(conn, cmdLineData.address, cmdLineData.addressDefaultPort)) {
            setMessageBoxTextID("Interface.Menu.Messages.IPEmpty");
            showMessageBoxButtons();
            return 0;
        }
        
        //This forces initializing stuff required by game
        _shellIconManager.initialMenu = SQSH_MM_MULTIPLAYER_LOBBY_SCR;
        if (cmdLineData.server) {
            loadMultiplayerList();
            if (multiplayerMaps.empty()) {
                setMessageBoxTextID("Interface.Menu.Messages.UnknownError");
                showMessageBoxButtons();
            } else {
                setMessageBoxTextID("Interface.Menu.Messages.Creating");
                std::string gameName;
                if (cmdLineData.roomName.empty()) {
                    gameName = cmdLineData.playerName;
                } else {
                    gameName = cmdLineData.roomName;
                }
                MissionDescription* mission;
                if (cmdLineData.save.empty()) {
                    std::string missionName = std::string("RESOURCE/MULTIPLAYER/") + multiplayerMaps[0].missionName();
                    mission = new MissionDescription(missionName.c_str(), GT_MULTI_PLAYER_CREATE);
                } else {
                    std::string missionName = std::string("RESOURCE/SAVES/MULTIPLAYER/") + cmdLineData.save;
                    mission = new MissionDescription(missionName.c_str(), GT_MULTI_PLAYER_LOAD);
                    mission->loadIntoMemory();
                }
                _shellIconManager.initialMenu = SQSH_MM_MULTIPLAYER_LOBBY_SCR;
                gameShell->getNetClient()->CreateGame(cmdLineData.publicHost, conn, gameName, mission, cmdLineData.playerName, cmdLineData.password);
                showMessageBoxButtons();
            }
        } else if (cmdLineData.roomID != 0) {
            gameShell->getNetClient()->JoinPublicRoomGame(conn, cmdLineData.roomID, cmdLineData.playerName, cmdLineData.password);
            showMessageBoxButtons();
        } else {
            gameShell->getNetClient()->JoinDirectGame(conn, cmdLineData.playerName, cmdLineData.password);
            showMessageBoxButtons();
        }
        return 0;
    }
    return 1;
}

void GameShell::startCmdline(const CommandLineData& data) {
    _bCursorVisible = 1;
    _WaitCursor();
    cmdLineData = data;
    
    putStringSettings(regLanName, cmdLineData.playerName);
    _shellIconManager.LoadControlsGroup(SHELL_LOAD_GROUP_MENU);
    CEditWindow* input = (CEditWindow*)_shellIconManager.GetWnd(SQSH_MM_MULTIPLAYER_NAME_INPUT);
    input->SetText(cmdLineData.playerName.c_str());

    setupOkMessageBox(exitCmdLine, 0, qdTextDB::instance().getText("Interface.Menu.Messages.Connecting"), MBOX_EXIT, false);
    prepareForInGameMenu();
    showMessageBox();

    _shellIconManager.AddDynamicHandler(startCmdlineQuant, CBCODE_QUANT);
}

/// Game hosts listing

GameInfo selectedGame = {};

bool hasGameListSelection() {
    return !selectedGame.gameHost.empty();
}

void selectGameInfo(const GameInfo* game) {
    if (game) {
        selectedGame = *game;
    } else {
        selectedGame = GameInfo();
    }
}

LocalizedText formatGameInfoList(const GameInfo& info) {
    //TODO No localized text allowed here since the text must be in info.gameName's locale, remove this when UTF8
    const std::string& locale = info.gameName.locale;
    const char* textColor;
    if (info.gameClosed || info.gameStarted || info.gameVersion != currentShortVersion) {
        textColor = "&FF2222";
    } else if (info.hasPassword) {
        textColor = "&66DDFF";
    } else {
        textColor = "&FFFFFF";
    }
    std::string text = textColor;
    text += convertToCodepage(info.gameName.text.c_str(), locale);
    text += textColor;
    text += " (" + std::to_string(info.currentPlayers);
    text += "/" + std::to_string(info.maximumPlayers);
    text += " - " + std::to_string(info.ping) + " ms)";
    return LocalizedText(text, locale);
}

std::string formatGameInfoWindow(const GameInfo& info) {
    std::string text;
    if (info.hasPassword) {
        text += "&66DDFF";
        text += qdTextDB::instance().getText("Interface.Tips.Multiplayer.HasPassword");
        text += "\n";
    }
    if (info.gameStarted) {
        text += "&FF2222";
        text += qdTextDB::instance().getText("Interface.Tips.Multiplayer.GameStarted");
        text += "\n";
    }

    //Show content name, only the first of array
    std::vector<GAME_CONTENT> gameContent = getGameContentEnums(selectedGame.gameContent);
    if (!gameContent.empty()) {
        GAME_CONTENT content = gameContent.front();
        if (terGameContentSelect & content) {
            text += "&FFFFFF";
        } else {
            text += "&FF2222";
        }
        text += qdTextDB::instance().getText("Interface.Tips.Multiplayer.Game");
        text += ":\n";
        text += getGameContentName(content);
    }
    
    //Show version
    text += "&FFFFFF - ";
    if (info.gameVersion == currentShortVersion) {
        text += "&FFFFFF";
    } else {
        text += "&FF2222";
    }
    text += info.gameVersion + "\n";
    
    //Show map
    text += "&FFFFFF";
    text += qdTextDB::instance().getText("Interface.Tips.Multiplayer.Map");
    text += ": ";
    text += getMapName(info.scenario.c_str());
    text += "\n";
    
    //Show players
    if (info.currentPlayers < info.maximumPlayers) {
        text += "&FFFFFF";
    } else {
        text += "&FF2222";
    }
    text += qdTextDB::instance().getText("Interface.Tips.Multiplayer.CurrentPlayers");
    text += ": " + std::to_string(info.currentPlayers);
    text += " / " + std::to_string(info.maximumPlayers);
    
    //Show ping
    text += "\n&FFFFFF";
    text += qdTextDB::instance().getText("Interface.Tips.Multiplayer.Ping");
    text += ": " + std::to_string(info.ping) + "\n";
    
    return text;
}

bool canJoinSelectedGame() {
    return hasGameListSelection()
    && !selectedGame.gameClosed && !selectedGame.gameStarted
    && (selectedGame.currentPlayers < selectedGame.maximumPlayers)
    && selectedGame.gameVersion == currentShortVersion
    ;
}

void updateMultiplayerListUI() {
    bool hasSelection = hasGameListSelection();
    
    //Set map window
    int mapPos = -1;
    if (hasSelection) {
        for (uint32_t i = 0, s = multiplayerMaps.size(); i < s; i++) {
            if (selectedGame.scenario == multiplayerMaps[i].worldName()) {
                mapPos = static_cast<int>(i);
            }
        }
    }
    if (0 <= mapPos) {
        setupMapDescWnd(mapPos, multiplayerMaps, SQSH_MM_MULTIPLAYER_LIST_MAP, -1, -1, GT_MULTI_PLAYER_CREATE);
    } else {
        ((CShowMapWindow*)_shellIconManager.GetWnd(SQSH_MM_MULTIPLAYER_LIST_MAP))->setWorldID( hasSelection ? -2 : -1 );
    }
    
    //Set server description
    dynamic_cast<CTextWindow*>(_shellIconManager.GetWnd(SQSH_MM_MULTIPLAYER_LIST_MAP_DESCR_TXT))->setText(
            hasSelection ? formatGameInfoWindow(selectedGame) : ""
    );
    
    //Set join enable button state
    ((CShellPushButton*)_shellIconManager.GetWnd(SQSH_MM_MULTIPLAYER_LIST_JOIN_BTN))->Enable(canJoinSelectedGame());
}

int multiplayerListJoinQuant( float, float ) {
    if (menuChangingDone) {
        if (!hasGameListSelection() || selectedGame.gameClosed) {
            int id = _shellIconManager.getVisibleMenuScr();
            if (id == SQSH_MM_MULTIPLAYER_LIST_SCR) {
                gameShell->prepareNetClient();
            } else {
                _shellIconManager.SwitchMenuScreens(LAST_VISIBLE, SQSH_MM_MULTIPLAYER_LIST_SCR);
            }
            return 0;
        }
        std::string name = dynamic_cast<CEditWindow*>(_shellIconManager.GetWnd(SQSH_MM_MULTIPLAYER_NAME_INPUT))->getText();
        std::string password;
        if (selectedGame.hasPassword) {
            //Player had to go through SQSH_MM_MULTIPLAYER_PASSWORD_SCR so pickup the password from it's input
            CEditWindow* passwordInput = dynamic_cast<CEditWindow*>(_shellIconManager.GetWnd(SQSH_MM_MULTIPLAYER_PASSWORD_PASSWORD_INPUT));
            password = passwordInput->getText();
        }
        NetAddress conn;
        if (!NetAddress::resolve(conn, selectedGame.gameHost, NET_RELAY_DEFAULT_PORT)) {
            setMessageBoxTextID("Interface.Menu.Messages.IPEmpty");
            showMessageBoxButtons();
        } else if (selectedGame.gameRoomID != 0) {
            gameShell->getNetClient()->JoinPublicRoomGame(conn, selectedGame.gameRoomID, name, password);
        } else {
            gameShell->getNetClient()->JoinDirectGame(conn, name, password);
        }
        return 0;
    }
    return 1;
}

///Code to run when game is doble clicked in list or join button is pressed
void joinSelectedGame(CShellWindow* pWnd) {
    if (!hasGameListSelection()) {
        return;
    }
    CEditWindow* input = dynamic_cast<CEditWindow*>(_shellIconManager.GetWnd(SQSH_MM_MULTIPLAYER_NAME_INPUT));
    if (input->isEmptyText()) {
        setupOkMessageBox(nullptr, 0, qdTextDB::instance().getText("Interface.Menu.Messages.NameEmpty"), MBOX_OK);
        showMessageBox();
    } else {
        putStringSettings(regLanName, input->getText());

        if (selectedGame.hasPassword) {
            CTextWindow* name_text = dynamic_cast<CTextWindow*>(_shellIconManager.GetWnd(SQSH_MM_MULTIPLAYER_PASSWORD_NAME_TXT));
            std::string text = ""; //TODO use game name when texts are UTF8
            name_text->Show(0);
            name_text->setText(text);
            _shellIconManager.SwitchMenuScreens(pWnd->m_pParent->ID, SQSH_MM_MULTIPLAYER_PASSWORD_SCR);
        } else {
            setupOkMessageBox(nullptr, 0, qdTextDB::instance().getText("Interface.Menu.Messages.Connecting"), MBOX_OK, false);
            showMessageBox();
            _shellIconManager.AddDynamicHandler( multiplayerListJoinQuant, CBCODE_QUANT );
        }
    }
}

void onMMMultiplayerListGameList(CShellWindow* pWnd, InterfaceEventCode code, int param) {
    if (code == EVENT_CREATEWND) {
        CListBoxWindow* listBox = dynamic_cast<CListBoxWindow*>(pWnd);
        listBox->NewItem(1);
        listBox->Clear();
    } else if (code == EVENT_DRAWWND) {
        CListBoxWindow* listBox = dynamic_cast<CListBoxWindow*>(pWnd);
        listBox->Clear();

        auto gameList = gameShell->getNetClient()->getGameList();
        bool hasSelection = hasGameListSelection();
        if (!gameList.empty()) {
            int selectIndex = -1;
            int i = 0;
            for (auto& game : gameList) {
                LocalizedText name = formatGameInfoList(game);
                listBox->AddLocalizedText(name, 0);
                if (hasSelection && selectIndex == -1) {
                    if (game.gameHost == selectedGame.gameHost
                    && game.gameRoomID == selectedGame.gameRoomID) {
                        selectIndex = i;
                        selectGameInfo(&game);
                    }
                }
                i++;
            }
            listBox->SetCurSel(selectIndex);
            if (hasSelection) {
                if (selectIndex == -1) {
                    //No longer in list
                    selectGameInfo(nullptr);
                }
                updateMultiplayerListUI();
            }
        } else {
            selectGameInfo(nullptr);
            updateMultiplayerListUI();
        }
    } else if ( code == EVENT_DOUBLECLICK && param == VK_LBUTTON) {
        if (canJoinSelectedGame()) {
            joinSelectedGame(pWnd);
        }
    } else if ( code == EVENT_PRESSED && intfCanHandleInput() ) {
        CListBoxWindow* listBox = (CListBoxWindow*)_shellIconManager.GetWnd(SQSH_MM_MULTIPLAYER_LIST_GAME_LIST);
        int pos = listBox->GetCurSel();
        auto gameList = gameShell->getNetClient()->getGameList();
        if (0 <= pos && pos < gameList.size()) {
            std::vector<GameInfo>::iterator it = gameList.begin();
            advance(it, pos);
            selectGameInfo(&(*it));
        } else {
            selectGameInfo(nullptr);
        }
        updateMultiplayerListUI();
    }
}

void onMMMultiplayerListJoinButton(CShellWindow* pWnd, InterfaceEventCode code, int param) {
    if( code == EVENT_UNPRESSED && intfCanHandleInput() ) {
        joinSelectedGame(pWnd);
    }
}


///Password screen handlers

void onMMMultiplayerPasswordNextBtn(CShellWindow* pWnd, InterfaceEventCode code, int param) {
    if( code == EVENT_UNPRESSED && intfCanHandleInput() ) {
        if (!hasGameListSelection()) {
            return;
        }
        setupOkMessageBox(nullptr, 0, qdTextDB::instance().getText("Interface.Menu.Messages.Connecting"), MBOX_OK, false);
        showMessageBox();
        _shellIconManager.AddDynamicHandler(multiplayerListJoinQuant, CBCODE_QUANT);
    }
}

///Button handlers

void onMMMultiplayerButton(CShellWindow* pWnd, InterfaceEventCode code, int param) {
    if ( code == EVENT_UNPRESSED && intfCanHandleInput() ) {
        _shellIconManager.SwitchMenuScreens(SQSH_MM_START_SCR, SQSH_MM_MULTIPLAYER_LIST_SCR);
    }
}

void onMMMultiplayerListDirectButton(CShellWindow* pWnd, InterfaceEventCode code, int param) {
    if ( code == EVENT_UNPRESSED && intfCanHandleInput() ) {
        _shellIconManager.SwitchMenuScreens(pWnd->m_pParent->ID, SQSH_MM_MULTIPLAYER_JOIN_SCR);
    }
}

void onMMMultiplayerListCreateButton(CShellWindow* pWnd, InterfaceEventCode code, int param) {
    if( code == EVENT_UNPRESSED && intfCanHandleInput() ) {
        CEditWindow* input = (CEditWindow*)_shellIconManager.GetWnd(SQSH_MM_MULTIPLAYER_NAME_INPUT);
        if (input->isEmptyText()) {
            setupOkMessageBox(nullptr, 0, qdTextDB::instance().getText("Interface.Menu.Messages.NameEmpty"), MBOX_OK);
            showMessageBox();
        } else {
            if (!input->isEmptyText()) {
                putStringSettings(regLanName, input->getText());
            }
			_shellIconManager.SwitchMenuScreens(pWnd->m_pParent->ID, SQSH_MM_MULTIPLAYER_HOST_SCR);
        }
    }
}

void onMMMultiplayerListBackButton(CShellWindow* pWnd, InterfaceEventCode code, int param) {
    if( code == EVENT_UNPRESSED && intfCanHandleInput() ) {
        if (gameShell->isStartedWithMainmenu()) {
            CEditWindow* input = (CEditWindow*)_shellIconManager.GetWnd(SQSH_MM_MULTIPLAYER_NAME_INPUT);
            if (!input->isEmptyText()) {
                putStringSettings(regLanName, input->getText());
            }
            _shellIconManager.SwitchMenuScreens( pWnd->m_pParent->ID, SQSH_MM_START_SCR );
        } else {
//			_shellIconManager.SwitchMenuScreens(pWnd->m_pParent->ID, SHOW_LAST_SPLASH);
            gameShell->GameContinue = 0;
        }
    }
}
