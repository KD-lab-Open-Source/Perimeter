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

void GameShell::callBack_JoinGameReturnCode(e_JoinGameReturnCode retCode, std::string extraInfo) {
    std::string textID;
    switch (retCode) {
        case JG_RC_OK:
            _shellIconManager.AddDynamicHandler( hideBoxToLobbyQuant, CBCODE_QUANT );
            return;
        case JG_RC_CONNECTION_ERR:
            textID = "Interface.Menu.Messages.Multiplayer.ConnectionFailed";
            break;
        case JG_RC_GAME_IS_RUN_ERR:
            textID = "Interface.Menu.Messages.Multiplayer.AlreadyRun";
            break;
        case JG_RC_GAME_IS_FULL_ERR:
            textID = "Interface.Menu.Messages.Multiplayer.ServerFull";
            break;
        case JG_RC_GAME_NOT_EQUAL_VERSION_ERR:
            textID = "Interface.Menu.Messages.Multiplayer.IncorrectVersion";
            break;
        case JG_RC_GAME_NOT_EQUAL_CONTENT_ERR:
            textID = "Interface.Menu.Messages.Multiplayer.IncorrectContent";
            break;
        case JG_RC_GAME_NOT_EQUAL_ARCH_ERR:
            textID = "Interface.Menu.Messages.Multiplayer.IncorrectArch";
            break;
        case JG_RC_PASSWORD_ERR:
            textID = "Interface.Menu.Messages.Multiplayer.IncorrectPassword";
            break;
        case JG_RC_SIGNATURE_ERR:
            textID = "Interface.Menu.Messages.Multiplayer.SignatureError";
            break;
        default:
            xassert(0);
            textID = "Interface.Menu.Messages.UnknownError";
            break;
    }
    
    std::string text = qdTextDB::instance().getText(textID.c_str());
    if (!extraInfo.empty()) {
        text += "\n\n" + extraInfo;
        printf("Connection response info: \n%s\n", text.c_str());
    }
    dynamic_cast<CTextWindow*>(_shellIconManager.GetWnd(SQSH_MM_SUBMIT_TXT))->setText(text);
    showMessageBoxButtons();
}

int multiplayerJoinBackHandler( float, float ) {
    //handles connection failed
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
            setMessageBoxTextID("Interface.Menu.Messages.IPEmpty");
            showMessageBoxButtons();
        } else {
            CEditWindow* playerNameInput = dynamic_cast<CEditWindow*>(_shellIconManager.GetWnd(SQSH_MM_MULTIPLAYER_NAME_INPUT));
            std::string playerName = playerNameInput->GetText();
            CEditWindow* passwordInput = dynamic_cast<CEditWindow*>(_shellIconManager.GetWnd(SQSH_MM_MULTIPLAYER_JOIN_PASSWORD_INPUT));
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
        if (input->isEmptyText()) {
            setupOkMessageBox(nullptr, 0, qdTextDB::instance().getText("Interface.Menu.Messages.NameEmpty"), MBOX_OK);
            showMessageBox();
		} else if (ipInput->isEmptyText()) {
			setupOkMessageBox(nullptr, 0, qdTextDB::instance().getText("Interface.Menu.Messages.IPEmpty"), MBOX_OK);
			showMessageBox();
        } else {
            putStringSettings(regLanName, input->getText());
            putStringSettings("JoinIP", ipInput->getText());
            
            setupOkMessageBox(multiplayerJoinBackHandler, 0, qdTextDB::instance().getText("Interface.Menu.Messages.Connecting"), MBOX_BACK, false);
            showMessageBox();

            _shellIconManager.AddDynamicHandler( joinHostHandler, CBCODE_QUANT );
        }
    }
}