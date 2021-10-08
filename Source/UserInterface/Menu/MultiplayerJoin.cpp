#include "StdAfx.h"
#include "MainMenu.h"
#include "GameShell.h"
#include "PerimeterShellUI.h"
#include "MessageBox.h"
#include "MultiplayerCommon.h"


/// Join game to lobby handler

int toLobbyQuant( float, float ) {
    if (menuChangingDone) {
        if (gameShell->isStartedWithMainmenu()) {
            //Since we don't know if we come from LIST or JOIN screen, use LAST_VISIBLE
            _shellIconManager.SwitchMenuScreens(LAST_VISIBLE, SQSH_MM_MULTIPLAYER_LOBBY_SCR);
        } else {
            _shellIconManager.SwitchMenuScreens(-1, SQSH_MM_MULTIPLAYER_LOBBY_SCR);
            _shellIconManager.SetModalWnd(0);
        }
        return 0;
    }
    return 1;
}


int hideBoxToLobbyQuant( float, float ) {
    if (menuChangingDone) {
        hideMessageBox();
        _shellIconManager.AddDynamicHandler( toLobbyQuant, CBCODE_QUANT );
        return 0;
    }
    return 1;
}

///Join handling

void GameShell::callBack_JoinGameReturnCode(e_JoinGameReturnCode retCode) {
    switch (retCode) {
        case JG_RC_OK:
            _shellIconManager.AddDynamicHandler( hideBoxToLobbyQuant, CBCODE_QUANT );
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
        case JG_RC_GAME_NOT_EQUAL_CONTENT_ERR:
            setMessageBoxTextID("Interface.Menu.Messages.Multiplayer.IncorrectContent");
            showMessageBoxButtons();
            break;
        case JG_RC_PASSWORD_ERR:
            setMessageBoxTextID("Interface.Menu.Messages.Multiplayer.IncorrectPassword");
            showMessageBoxButtons();
            break;
        default:
            xassert(0);
            setMessageBoxTextID("Interface.Menu.Messages.UnknownError");
            showMessageBoxButtons();
    }
}

int multiplayerJoinInterruptHandler( float, float ) {
    //interrupt connecting
    gameShell->getNetClient()->FinishGame();
    gameShell->getNetClient()->StartFindHost();
    hideMessageBox();
    return 1;
}

int joinHostHandler( float, float ) {
    if (menuChangingDone) {
        CEditWindow* addressInput = dynamic_cast<CEditWindow*>(_shellIconManager.GetWnd(SQSH_MM_MULTIPLAYER_JOIN_IP_INPUT));
        NetAddress conn;
        bool resolveFailed = !NetAddress::resolve(conn, addressInput->getText());

        if (resolveFailed) {
            setMessageBoxTextID("Interface.Menu.Messages.WrongIPAdress");
            showMessageBoxButtons();
        } else {
            CEditWindow* playerNameInput = dynamic_cast<CEditWindow*>(_shellIconManager.GetWnd(SQSH_MM_MULTIPLAYER_NAME_INPUT));
            std::string playerName = playerNameInput->GetText();
            CEditWindow* passwordInput = dynamic_cast<CEditWindow*>(_shellIconManager.GetWnd(SQSH_MM_MULTIPLAYER_HOST_PASSWORD_INPUT));
            std::string password = passwordInput->getText();
            gameShell->getNetClient()->JoinGame(conn, playerName, password);
        }
        return 0;
    }
    return 1;
}

void onMMMultiplayerJoinNextBtn(CShellWindow* pWnd, InterfaceEventCode code, int param) {
    if( code == EVENT_UNPRESSED && intfCanHandleInput() ) {
        CEditWindow* input = (CEditWindow*)_shellIconManager.GetWnd(SQSH_MM_MULTIPLAYER_NAME_INPUT);
        CEditWindow* ipInput = (CEditWindow*)_shellIconManager.GetWnd(SQSH_MM_MULTIPLAYER_JOIN_IP_INPUT);
        if (input->getText().empty()) {
            setupOkMessageBox(nullptr, 0, qdTextDB::instance().getText("Interface.Menu.Messages.NameEmpty"), MBOX_OK);
            showMessageBox();
		} else if (ipInput->getText().empty()) {
			setupOkMessageBox(nullptr, 0, qdTextDB::instance().getText("Interface.Menu.Messages.IPEmpty"), MBOX_OK);
			showMessageBox();
        } else {
            putStringSettings(regLanName, input->getText());
            
            setupOkMessageBox(multiplayerJoinInterruptHandler, 0, qdTextDB::instance().getText("Interface.Menu.Messages.Connecting"), MBOX_BACK, false);
            showMessageBox();

            _shellIconManager.AddDynamicHandler( joinHostHandler, CBCODE_QUANT );
        }
    }
}