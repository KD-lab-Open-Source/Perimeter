#include "StdAfx.h"
#include "MainMenu.h"
#include "GameShell.h"
#include "PerimeterShellUI.h"
#include "MessageBox.h"
#include "MultiplayerCommon.h"

///Command line stuff

CommandLineData cmdLineData;

int exitCmdLine( float, float ) {
    gameShell->terminate();
    return 1;
}

int startCmdlineQuant(float, float ) {
    if (menuChangingDone) {
        gameShell->createNetClient();
        
        auto client = gameShell->getNetClient();
        client->publicServerHost = cmdLineData.publicHost;

        NetAddress conn;
        if (!NetAddress::resolve(conn, cmdLineData.address)) {
            setMessageBoxTextID("Interface.Menu.Messages.IPEmpty");
            showMessageBoxButtons();
        } else {
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
                    gameShell->getNetClient()->CreateGame(conn, gameName, mission, cmdLineData.playerName, cmdLineData.password);
                    showMessageBoxButtons();
                }
            } else {
                gameShell->getNetClient()->JoinGame(conn, cmdLineData.playerName, cmdLineData.password);
                showMessageBoxButtons();
            }
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

NetAddress selectedHost;

std::string formatGameInfo(const GameHostInfo& info, bool oneLine) {
    if (info.gameName.empty()) {
        if (oneLine) {
            return "&FF0000" + info.hostName;
        } else {
            return qdTextDB::instance().getText("Interface.Tips.MultiplayerInfoServerEmpty");
        }
    } else {
        static char cbTemp[1024];
        if (oneLine) {
            std::string name = info.gameName + "&FFFFFF";
            if (!info.hostName.empty()) {
                name += ":" + info.hostName + ":";
            }
            _shellIconManager.FormatMessageText(
                    "<MultiplayerInfoOneLine>",
                    cbTemp,
                    name.c_str(),
                    info.gameStatusInfo.currrentPlayers,
                    info.gameStatusInfo.maximumPlayers,
                    info.gameStatusInfo.ping,
                    info.gameStatusInfo.flag_gameRun ? qdTextDB::instance().getText("Interface.Tips.MultiplayerInfoStarted") : "");
        } else {
            _shellIconManager.FormatMessageText(
                    "<MultiplayerInfo>",
                    cbTemp,
                    (info.gameName + "&FFFFFF").c_str(),
                    info.gameStatusInfo.currrentPlayers,
                    info.gameStatusInfo.maximumPlayers,
                    info.gameStatusInfo.ping,
                    info.hostName.c_str(),
                    info.gameStatusInfo.flag_gameRun ? qdTextDB::instance().getText("Interface.Tips.MultiplayerInfoStarted") : "");
        }
        return cbTemp;
    }
}

void onMMMultiplayerListGameList(CShellWindow* pWnd, InterfaceEventCode code, int param) {
    if (code == EVENT_CREATEWND) {
//		CListBoxWindow* list = dynamic_cast<CListBoxWindow*>(_shellIconManager.GetWnd(SQSH_MM_GAME_LIST));
        CListBoxWindow* list = (CListBoxWindow*)pWnd;
        list->NewItem(1);
        list->Clear();
    } else if (code == EVENT_DRAWWND) {
//		CListBoxWindow* listBox = dynamic_cast<CListBoxWindow*>(_shellIconManager.GetWnd(SQSH_MM_GAME_LIST));
        CListBoxWindow* listBox = (CListBoxWindow*)pWnd;
        listBox->Clear();

        auto hostList = gameShell->getNetClient()->getGameHostList();
        if (!hostList.empty()) {

            GameHostInfo& selectedGame = hostList.front();
            int selectIndex = 0;
            int i = 0;
            for (auto it = hostList.begin(); it != hostList.end(); it++, i++) {
                std::string name = formatGameInfo(*it, true);
                listBox->AddString( name.c_str(), 0 );
                if ((*it).gameHost.crc() == selectedHost.crc()) {
                    selectedGame = *it;
                    selectIndex = i;
                }
            }
            listBox->SetCurSel(selectIndex);
            if (selectIndex == 0) {
                selectedHost = selectedGame.gameHost;
            }
            int id = getMultiplayerMapNumber(selectedGame.gameStatusInfo.world);
            ((CShowMapWindow*)_shellIconManager.GetWnd(SQSH_MM_MULTIPLAYER_LIST_MAP))->setWorldID(id);
            ((CTextWindow*)_shellIconManager.GetWnd(SQSH_MM_MULTIPLAYER_LIST_MAP_DESCR_TXT))->setText(formatGameInfo(selectedGame, false) );
            ((CShellPushButton*)_shellIconManager.GetWnd(SQSH_MM_MULTIPLAYER_LIST_JOIN_BTN))->Enable(!selectedGame.gameName.empty());
        } else {
            ((CShowMapWindow*)_shellIconManager.GetWnd(SQSH_MM_MULTIPLAYER_LIST_MAP))->setWorldID(-1 );
            ((CTextWindow*)_shellIconManager.GetWnd(SQSH_MM_MULTIPLAYER_LIST_MAP_DESCR_TXT))->setText("" );
            ((CShellPushButton*)_shellIconManager.GetWnd(SQSH_MM_MULTIPLAYER_LIST_JOIN_BTN))->Enable(0);
        }
    } else if ( code == EVENT_PRESSED && intfCanHandleInput() ) {
        CListBoxWindow* listBox = (CListBoxWindow*)_shellIconManager.GetWnd(SQSH_MM_MULTIPLAYER_LIST_GAME_LIST);
        int pos = listBox->GetCurSel();
        if (pos >= 0 && pos < gameShell->getNetClient()->getGameHostList().size()) {
            std::vector<GameHostInfo>::iterator it = gameShell->getNetClient()->getGameHostList().begin();
            advance(it, pos);
            selectedHost = (*it).gameHost;
            
            int id = getMultiplayerMapNumber((*it).gameStatusInfo.world);
            ((CShowMapWindow*)_shellIconManager.GetWnd(SQSH_MM_MULTIPLAYER_LIST_MAP))->setWorldID(id);
            ((CTextWindow*)_shellIconManager.GetWnd(SQSH_MM_MULTIPLAYER_LIST_MAP_DESCR_TXT))->setText(formatGameInfo(*it, false) );
        }
    }
}

///Join handler

extern int multiplayerJoinBackHandler(float, float);

int joinQuant( float, float ) {
    if (menuChangingDone) {
        CEditWindow* input = (CEditWindow*)_shellIconManager.GetWnd(SQSH_MM_MULTIPLAYER_NAME_INPUT);
        gameShell->getNetClient()->JoinGame(selectedHost, input->GetText());
        return 0;
    }
    return 1;
}

void onMMMultiplayerListJoinButton(CShellWindow* pWnd, InterfaceEventCode code, int param) {
    if( code == EVENT_UNPRESSED && intfCanHandleInput() ) {
        CEditWindow* input = (CEditWindow*)_shellIconManager.GetWnd(SQSH_MM_MULTIPLAYER_NAME_INPUT);
        if (input->isEmptyText()) {
            setupOkMessageBox(nullptr, 0, qdTextDB::instance().getText("Interface.Menu.Messages.NameEmpty"), MBOX_OK);
            showMessageBox();
        } else {
            putStringSettings(regLanName, input->getText());

            setupOkMessageBox(multiplayerJoinBackHandler, 0, qdTextDB::instance().getText("Interface.Menu.Messages.Connecting"), MBOX_EXIT, false);
            showMessageBox();

            _shellIconManager.AddDynamicHandler( joinQuant, CBCODE_QUANT );
        }
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
            setupOkMessageBox(0, 0, qdTextDB::instance().getText("Interface.Menu.Messages.NameEmpty"), MBOX_OK);
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
