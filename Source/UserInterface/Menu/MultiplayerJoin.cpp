#include "StdAfx.h"
#include "GameShell.h"
#include "PerimeterShellUI.h"
#include "MessageBox.h"
#include "MultiplayerCommon.h"
#include "MainMenu.h"

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

///Join handling

void GameShell::callBack_JoinGameReturnCode(e_JoinGameReturnCode retCode, std::string extraInfo) {
    std::string textID;
    bool returnMultiplayerList = false;
    switch (retCode) {
        case JG_RC_OK:
            hideMessageBox();
            _shellIconManager.AddDynamicHandler( toLobbyQuant, CBCODE_QUANT );
            return;
        case JG_RC_CONNECTION_ERR: {
            textID = "Interface.Menu.Messages.Multiplayer.ConnectionFailed";
            int id = _shellIconManager.getVisibleMenuScr();
            if (id == SQSH_MM_MULTIPLAYER_PASSWORD_SCR) {
                //This means the room might be closed or unavailable, return back to list
                returnMultiplayerList = true;
            }
            break;
        }
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
        case JG_RC_UNKNOWN_ERR:
        default:
            textID = "Interface.Menu.Messages.UnknownError";
            break;
    }
    
    std::string text = qdTextDB::instance().getText(textID.c_str());
    if (!extraInfo.empty()) {
        text += "\n\n" + extraInfo;
        printf("Connection response info: \n%s\n", text.c_str());
    }
    
    setupOkMessageBox(
            returnMultiplayerList ? exitToMultiplayerScreenAction : nullptr,
            0,
            text,
            returnMultiplayerList ? MBOX_BACK : MBOX_OK
    );
    showMessageBoxButtons();
}

int joinHostHandler( float, float ) {
    if (menuChangingDone) {
        CEditWindow* playerNameInput = dynamic_cast<CEditWindow*>(_shellIconManager.GetWnd(SQSH_MM_MULTIPLAYER_NAME_INPUT));
        std::string playerName = playerNameInput->GetText();
        CEditWindow* passwordInput = dynamic_cast<CEditWindow*>(_shellIconManager.GetWnd(SQSH_MM_MULTIPLAYER_JOIN_PASSWORD_INPUT));
        std::string password = passwordInput->getText();
        CEditWindow* addressInput = dynamic_cast<CEditWindow*>(_shellIconManager.GetWnd(SQSH_MM_MULTIPLAYER_JOIN_IP_INPUT));
        NetAddress conn;
        if (!NetAddress::resolve(conn, addressInput->getText())) {
            setMessageBoxTextID("Interface.Menu.Messages.IPEmpty");
            showMessageBoxButtons();
        } else {
            gameShell->getNetClient()->JoinDirectGame(conn, playerName, password);
        }
        return 0;
    }
    return 1;
}

void onMMMultiplayerJoinNextBtn(CShellWindow* pWnd, InterfaceEventCode code, int param) {
    if( code == EVENT_UNPRESSED && intfCanHandleInput() ) {
        CEditWindow* name_input = (CEditWindow*)_shellIconManager.GetWnd(SQSH_MM_MULTIPLAYER_NAME_INPUT);
        CEditWindow* ip_input = (CEditWindow*)_shellIconManager.GetWnd(SQSH_MM_MULTIPLAYER_JOIN_IP_INPUT);
        if (name_input->isEmptyText()) {
            setupOkMessageBox(nullptr, 0, qdTextDB::instance().getText("Interface.Menu.Messages.NameEmpty"), MBOX_OK);
            showMessageBox();
		} else if (ip_input->isEmptyText()) {
			setupOkMessageBox(nullptr, 0, qdTextDB::instance().getText("Interface.Menu.Messages.IPEmpty"), MBOX_OK);
			showMessageBox();
        } else {
            putStringSettings(regLanName, name_input->getText());
            putStringSettings("JoinIP", ip_input->getText());
            
            setupOkMessageBox(nullptr, 0, qdTextDB::instance().getText("Interface.Menu.Messages.Connecting"), MBOX_OK, false);
            showMessageBox();
            
            _shellIconManager.AddDynamicHandler( joinHostHandler, CBCODE_QUANT );
        }
    }
}
