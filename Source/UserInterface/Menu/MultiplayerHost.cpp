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
        if (!portInput->getText().empty()) {
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
            std::string missionName = std::string("RESOURCE\\MULTIPLAYER\\") + multiplayerMaps[0].missionName();
            gameShell->getNetClient()->CreateGame(conn, gameName, missionName, playerName, password);
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
        pCombo->Array.emplace_back(getItemTextFromBase("Public Server").c_str() );
        pCombo->size = pCombo->Array.size();
        pCombo->pos = 0;
    }
}

/// List and button handlers

std::vector<MissionDescription> multiplayerSaves;

void fillMultiplayerHostList() {
    CListBoxWindow* list = (CListBoxWindow*)_shellIconManager.GetWnd(SQSH_MM_MULTIPLAYER_HOST_LIST);
    list->NewItem(1);
    list->Clear();
    std::string first = qdTextDB::instance().getText("Interface.Menu.Multiplayer.StartNewGame");
    list->AddString(first.c_str(), 0);
    list->SetCurSel(0);
    if (multiplayerSaves.empty()) {
        clearMapDescWnd(SQSH_MM_MULTIPLAYER_HOST_MAP, SQSH_MM_MULTIPLAYER_HOST_MAP_DESCR_TXT, -1);
    } else {
        setupMapDescWnd(0, multiplayerSaves, SQSH_MM_MULTIPLAYER_HOST_MAP, SQSH_MM_MULTIPLAYER_HOST_MAP_DESCR_TXT, -1);
    }
}

void onListSelect() {
    int pos = ((CListBoxWindow*) _shellIconManager.GetWnd(SQSH_MM_MULTIPLAYER_HOST_LIST))->GetCurSel();
    if (0 <= pos) {
        CEditWindow* input = (CEditWindow*)_shellIconManager.GetWnd(SQSH_MM_MULTIPLAYER_NAME_INPUT);
        if (input->getText().empty()) {
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

void onMMMultiplayerHostMapList(CShellWindow* pWnd, InterfaceEventCode code, int param) {
    if ( code == EVENT_PRESSED && intfCanHandleInput() ) {
        CListBoxWindow* list = (CListBoxWindow*)_shellIconManager.GetWnd(SQSH_MM_MULTIPLAYER_HOST_LIST);
        int pos = list->GetCurSel() - 1;
        if (0 <= pos && pos < multiplayerMaps.size()) {
            setupMapDescWnd(pos - 1, multiplayerSaves, SQSH_MM_MULTIPLAYER_HOST_MAP, SQSH_MM_MULTIPLAYER_HOST_MAP_DESCR_TXT, -1);
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

