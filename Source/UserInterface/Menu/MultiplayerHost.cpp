#include "StdAfx.h"
#include "MainMenu.h"
#include "GameShell.h"
#include "PerimeterShellUI.h"
#include "MessageBox.h"
#include "MultiplayerCommon.h"


/// Create host to lobby transition

int fromCreateToLobbyQuant( float, float ) {
    if (menuChangingDone) {
        if (gameShell->isStartedWithMainmenu()) {
            _shellIconManager.SwitchMenuScreens(SQSH_MM_MULTIPLAYER_HOST_SCR, SQSH_MM_MULTIPLAYER_LOBBY_SCR);
        } else {
            _shellIconManager.SwitchMenuScreens(-1, SQSH_MM_MULTIPLAYER_LOBBY_SCR);
        }
        return 0;
    }
    return 1;
}

int createHostHideBoxToLobbyQuant( float, float ) {
    if (menuChangingDone) {
        hideMessageBox();
        _shellIconManager.AddDynamicHandler( fromCreateToLobbyQuant, CBCODE_QUANT );
        return 0;
    }
    return 1;
}

void GameShell::callBack_CreateGameReturnCode(e_CreateGameReturnCode retCode) {
    switch (retCode) {
        case CG_RC_OK:
            _shellIconManager.AddDynamicHandler( createHostHideBoxToLobbyQuant, CBCODE_QUANT );
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

int creatingHostDialogQuant(float, float ) {
    if (menuChangingDone) {
        loadMultiplayerList();

        CEditWindow* portInput = dynamic_cast<CEditWindow*>(_shellIconManager.GetWnd(SQSH_MM_MULTIPLAYER_HOST_PORT_INPUT));
        NetAddress conn;
        std::string address = PERIMETER_IP_HOST_DEFAULT;
        if (!portInput->isEmptyText()) {
            address += ":" + portInput->getText();
        }
        bool resolveFailed = !NetAddress::resolve(conn, address);

        if (multiplayerMaps.empty()) {
            setMessageBoxTextID("Interface.Menu.Messages.UnknownError");
            showMessageBoxButtons();
        } else if (resolveFailed) {
            setMessageBoxTextID("Interface.Menu.Messages.WrongIPPort");
            showMessageBoxButtons();
        } else {
            CEditWindow* playerNameInput = dynamic_cast<CEditWindow*>(_shellIconManager.GetWnd(SQSH_MM_MULTIPLAYER_NAME_INPUT));
            std::string playerName = playerNameInput->GetText();
            CEditWindow* gameNameInput = dynamic_cast<CEditWindow*>(_shellIconManager.GetWnd(SQSH_MM_MULTIPLAYER_HOST_NAME_INPUT));
            std::string gameName = gameNameInput->getText();
            CEditWindow* passwordInput = dynamic_cast<CEditWindow*>(_shellIconManager.GetWnd(SQSH_MM_MULTIPLAYER_HOST_PASSWORD_INPUT));
            std::string password = passwordInput->getText();

            putStringSettings("HostName", gameName);
            putStringSettings("HostPort", std::to_string(conn.port()));
            
            //Load multiplayer save or use first multiplayer map
            CListBoxWindow* list = (CListBoxWindow*)_shellIconManager.GetWnd(SQSH_MM_MULTIPLAYER_HOST_LIST);
            int pos = list->GetCurSel() - 1;
            MissionDescription* mission;
            if (0 <= pos && pos < multiplayerSaves.size()) {
                std::string missionName = std::string("RESOURCE/SAVES/MULTIPLAYER/") + multiplayerSaves[pos].missionName();
                mission = new MissionDescription(missionName.c_str(), GT_MULTI_PLAYER_LOAD);
                mission->loadIntoMemory();
            } else {
                std::string missionName = std::string("RESOURCE/MULTIPLAYER/") + multiplayerMaps[0].missionName();
                mission = new MissionDescription(missionName.c_str(), GT_MULTI_PLAYER_CREATE);
            }

            gameShell->getNetClient()->CreateGame(conn, gameName, mission, playerName, password);
        }
        return 0;
    }
    return 1;
}

/// Other UI handlers

void onMMMultiplayerHostTypeCombo(CShellWindow* pWnd, InterfaceEventCode code, int param) {
    CComboWindow *pCombo = (CComboWindow*) pWnd;
    if (code == EVENT_CREATEWND) {
        pCombo->Array.emplace_back(getItemTextFromBase("Private Server").c_str() );
        //TODO enable this once implemented the public listing
        //pCombo->Array.emplace_back(getItemTextFromBase("Public Server").c_str() );
        pCombo->size = pCombo->Array.size();
        pCombo->pos = 0;
    }
}

/// List and button handlers

void fillMultiplayerHostList() {
    if (multiplayerSaves.empty()) {
        loadMultiplayerList();
    }
    CListBoxWindow* list = (CListBoxWindow*)_shellIconManager.GetWnd(SQSH_MM_MULTIPLAYER_HOST_LIST);
    list->NewItem(1);
    list->Clear();
    std::string first = qdTextDB::instance().getText("Interface.Menu.Multiplayer.StartNewGame");
    list->AddString(first.c_str(), 0);
    for (int i = 0; i < multiplayerSaves.size(); i++) {
        std::string name = multiplayerSaves[i].missionName();
        list->AddString(name.c_str(), 0);
    }
    list->SetCurSel(0);
    clearMapDescWnd(SQSH_MM_MULTIPLAYER_HOST_MAP, SQSH_MM_MULTIPLAYER_HOST_MAP_DESCR_TXT, -1);
}

void onListSelect() {
    int pos = ((CListBoxWindow*) _shellIconManager.GetWnd(SQSH_MM_MULTIPLAYER_HOST_LIST))->GetCurSel();
    if (0 <= pos) {
        CEditWindow* input = (CEditWindow*)_shellIconManager.GetWnd(SQSH_MM_MULTIPLAYER_NAME_INPUT);
        if (input->isEmptyText()) {
            setupOkMessageBox(nullptr, 0, qdTextDB::instance().getText("Interface.Menu.Messages.NameEmpty"), MBOX_OK);
            showMessageBox();
        } else {
            putStringSettings(regLanName, input->getText());
            setupOkMessageBox(nullptr, 0, qdTextDB::instance().getText("Interface.Menu.Messages.Creating"),
                              MBOX_EXIT, false);
            showMessageBox();
            _shellIconManager.AddDynamicHandler(creatingHostDialogQuant, CBCODE_QUANT);
        }
    }
}

int delMultiSaveGameAction(float, float) {
    CListBoxWindow* list = dynamic_cast<CListBoxWindow*>(_shellIconManager.GetWnd(SQSH_MM_MULTIPLAYER_HOST_LIST));
    int pos = list->GetCurSel() - 1;
    if (0 <= pos && pos < multiplayerSaves.size()) {
        gameShell->currentSingleProfile.deleteSave(multiplayerSaves[pos].savePathContent());
        multiplayerSaves.clear();
        loadMultiplayerList();
        fillMultiplayerHostList();
    }
    return 0;
}

void onMMMultiplayerHostDelButton(CShellWindow* pWnd, InterfaceEventCode code, int param) {
    if( code == EVENT_UNPRESSED && intfCanHandleInput() && param == VK_LBUTTON) {
        CListBoxWindow* list = dynamic_cast<CListBoxWindow*>(_shellIconManager.GetWnd(SQSH_MM_MULTIPLAYER_HOST_LIST));
        int pos = list->GetCurSel() - 1;
        if (0 <= pos && pos < multiplayerSaves.size()) {
            std::string text = qdTextDB::instance().getText("Interface.Menu.Messages.Confirmations.DeleteSave");
            std::string saveName = multiplayerSaves[pos].missionName();
            char* mess = new char[text.length() + saveName.length()];
            sprintf(mess, text.c_str(), saveName.c_str());
            setupYesNoMessageBox(delMultiSaveGameAction, 0, mess);
            delete [] mess;
            showMessageBox();
        }
    } else if (code == EVENT_DRAWWND) {
        CListBoxWindow* list = dynamic_cast<CListBoxWindow*>(_shellIconManager.GetWnd(SQSH_MM_MULTIPLAYER_HOST_LIST));
        pWnd->Enable( list->GetCurSel() >= 1 );
    }
}

void onMMMultiplayerHostMapList(CShellWindow* pWnd, InterfaceEventCode code, int param) {
    if ( code == EVENT_PRESSED && intfCanHandleInput() ) {
        CListBoxWindow* list = (CListBoxWindow*)_shellIconManager.GetWnd(SQSH_MM_MULTIPLAYER_HOST_LIST);
        int pos = list->GetCurSel() - 1;
        if (0 <= pos && pos < multiplayerSaves.size()) {
            setupMapDescWnd(pos, multiplayerSaves, SQSH_MM_MULTIPLAYER_HOST_MAP, SQSH_MM_MULTIPLAYER_HOST_MAP_DESCR_TXT, -1, GT_MULTI_PLAYER_LOAD);
        } else {
            clearMapDescWnd(SQSH_MM_MULTIPLAYER_HOST_MAP, SQSH_MM_MULTIPLAYER_HOST_MAP_DESCR_TXT, -1);
        }
    } else if ( code == EVENT_DOUBLECLICK && intfCanHandleInput() ) {
        onListSelect();
    } else if (code == EVENT_DRAWWND) {
        int pos = ((CListBoxWindow*)_shellIconManager.GetWnd(SQSH_MM_MULTIPLAYER_HOST_LIST))->GetCurSel();
        _shellIconManager.GetWnd(SQSH_MM_MULTIPLAYER_HOST_NEXT_BTN)->Enable(0 <= pos);
    }
}

void onMMMultiplayerHostNextButton(CShellWindow* pWnd, InterfaceEventCode code, int param) {
    if( code == EVENT_UNPRESSED && intfCanHandleInput() ) {
        onListSelect();
    }
}

void onMMMultiplayerHostBackButton(CShellWindow* pWnd, InterfaceEventCode code, int param) {
    if( code == EVENT_UNPRESSED && intfCanHandleInput() ) {
        if (gameShell->isStartedWithMainmenu()) {
            _shellIconManager.SwitchMenuScreens( pWnd->m_pParent->ID, SQSH_MM_MULTIPLAYER_LIST_SCR );
        } else {
//			_shellIconManager.SwitchMenuScreens(pWnd->m_pParent->ID, SHOW_LAST_SPLASH);
            gameShell->terminate();
        }
    }
}

