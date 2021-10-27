#include "StdAfx.h"
#include "MainMenu.h"
#include "GameShell.h"
#include "PerimeterShellUI.h"
#include "MessageBox.h"
#include "../../HT/ht.h"
#include "MultiplayerCommon.h"

//This file handles mostly ingame multiplayer stuff but some parts are shared with main menu

extern LogStream fout;
extern MissionDescription missionToExec;
extern MusicPlayer gb_Music;

std::vector<MissionDescription> multiplayerMaps;

void loadMultiplayerList() {
    if (multiplayerMaps.empty()) {
        loadMapVector(multiplayerMaps, "RESOURCE/MULTIPLAYER", ".spg");
    }
}

int getMultiplayerMapNumber(const std::string& saveName) {
    for (int i = 0, s = multiplayerMaps.size(); i < s; i++) {
        if (saveName == multiplayerMaps[i].saveName()) {
            return i;
        }
    }
    return -1;
}

int SwitchMultiplayerToLoadQuant(float, float ) {
    if (menuChangingDone) {
        gameShell->currentSingleProfile.setLastGameType(UserSingleProfile::MULTIPLAYER);

        gb_Music.FadeVolume(_fEffectButtonTotalTime*0.001f);
        _shellIconManager.SwitchMenuScreens(SQSH_MM_MULTIPLAYER_LOBBY_SCR, SQSH_MM_LOADING_MISSION_SCR);
        return 0;
    }
    return 1;
}

int exitMultiplayerGameQuant(float, float ) {
    if (menuChangingDone) {
        //meassge to NetCenter
//		gameShell->getNetClient()->FinishGame();
//		gameShell->getNetClient()->DisconnectAndStartFindHost();
        exitToInterfaceMessage(0);
        return 0;
    }
    return 1;
}

int returnToMultiplayerScreenQuant(float, float ) {
    if (menuChangingDone) {
        _shellIconManager.SwitchMenuScreens( LAST_VISIBLE, SQSH_MM_MULTIPLAYER_LIST_SCR );
        return 0;
    }
    return 1;
}

int exitToMultiplayerScreenAction(float, float ) {
    if (!gameShell->isStartedWithMainmenu()) {
        gameShell->GameContinue = 0;
    } else if (universe()) {
        hideMessageBox();
        _shellIconManager.AddDynamicHandler(exitMultiplayerGameQuant, CBCODE_QUANT);
    } else {
        hideMessageBox();
        _shellIconManager.AddDynamicHandler(returnToMultiplayerScreenQuant, CBCODE_QUANT);
    }
    return 1;
}

int multiplayerMapNotFoundQuant(float, float ) {
    if (menuChangingDone) {
        setupOkMessageBox(exitToMultiplayerScreenAction, 0,
                          qdTextDB::instance().getText("Interface.Menu.Messages.CantFindMap"), MBOX_EXIT);
        gameShell->getNetClient()->FinishGame();
        gameShell->getNetClient()->StartFindHost();
        showMessageBox();
        return 0;
    }
    return 1;
}

void GameShell::MultiplayerGameStart(const MissionDescription& mission) {
//	fout < "LANGameStart()\n";
    if ( !isWorldIDValid(mission.worldID()) || getMultiplayerMapNumber(mission.saveName()) == -1 ) {
        _shellIconManager.AddDynamicHandler(multiplayerMapNotFoundQuant, CBCODE_QUANT);
    } else {
        missionToExec = mission;
        _shellIconManager.AddDynamicHandler(SwitchMultiplayerToLoadQuant, CBCODE_QUANT);
    }
}

//////////show/hide message ingame////////////////////

std::string messageBoxText;
bool connRestored;

int exitAction( float, float ) {
    hideMessageBox();
    _shellIconManager.AddDynamicHandler(exitMultiplayerGameQuant, CBCODE_QUANT);
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

void GameShell::showConnectFailedInGame(const std::string& playerList) {
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
            _shellIconManager.AddDynamicHandler(exitMultiplayerGameQuant, CBCODE_QUANT);
        }
        return 0;
    }
    return 1;
}

void GameShell::hideConnectFailedInGame(bool connectionRestored) {
    connRestored = connectionRestored;
    _shellIconManager.AddDynamicHandler( hideMessageInGameQuant, CBCODE_QUANT );
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

int showTerminationToMultiplayerQuant(float, float ) {
    if (menuChangingDone) {
        std::string textID;
        switch (gtError) {
            case GameShell::GENERAL_CONNECTION_FAILED:
                textID = "Interface.Menu.Messages.Multiplayer.ConnectionFailed";
                break;
            case GameShell::CLIENT_DROPPED:
                textID = "Interface.Menu.Messages.Multiplayer.ClientDropped";
                break;
            case GameShell::HOST_TERMINATED:
                textID = "Interface.Menu.Messages.Multiplayer.HostTerminated";
                break;
            default:
                textID = "Interface.Menu.Messages.UnknownError";
        }


        setupOkMessageBox(exitToMultiplayerScreenAction, 0, qdTextDB::instance().getText(textID.c_str()), MBOX_EXIT);
        gameShell->prepareForInGameMenu();
//		fout < "showTerminationToMultiplayerQuant showMessageBox()\n";
        showMessageBox();
        return 0;
    }
    return 1;
}

void GameShell::generalErrorOccured(GeneralErrorType error) {
    gtError = error;
    gameShell->getNetClient()->FinishGame();
//	fout < "generalErrorOccured AddDynamicHandler(showTerminationToMultiplayerQuant)\n";
    _shellIconManager.AddDynamicHandler(showTerminationToMultiplayerQuant, CBCODE_QUANT);
}

////////// Lobby/Ingame chat /////////////

std::string colorComponentToString(unsigned char component) {
    char buff[3];
    snprintf(buff, 3, "%x", component);
    buff[2] = 0;
    if (strlen(buff) > 1) {
        return buff;
    } else {
        return std::string("0") + buff;
    }
}

void chatWindowInput(CChatInGameEditWindow* chatInput, int clan, const sColor4c& color, const std::string& name) {
    std::string strToSay =
            "&"
            +	colorComponentToString(color.r)
            +	colorComponentToString(color.g)
            +	colorComponentToString(color.b)
            +	name
            +	"&FFFFFF"
            +	(clan == -2 ? "" : chatInput->getModePostfix())
            +	": "
            +	chatInput->getText();

    //gameShell->addStringToChatWindow(strToSay);
    gameShell->getNetClient()->chatMessage(clan < 0 ? -1 : clan, strToSay.c_str());
    chatInput->SetText("");
}

void onMMLobbyChatInputButton(CShellWindow* pWnd, InterfaceEventCode code, int param) {
    if( code == EVENT_DOUBLECLICK && intfCanHandleInput() ) {
        CChatInGameEditWindow* chatInput = (CChatInGameEditWindow*) pWnd;
        if (!chatInput->getText().empty()) {
            MissionDescription& currMission = gameShell->getNetClient()->getCurrentMissionDescription();
            int colorIndex = currMission.getActivePlayerData().colorIndex;
            sColor4c activePlayerColor = sColor4c(sColor4f(playerColors[colorIndex].unitColor));

            chatWindowInput(
                chatInput,
                -2,
                activePlayerColor,
                currMission.getActivePlayerData().name()
            );
        }
    }
}

void onMMInGameChatInputButton(CShellWindow* pWnd, InterfaceEventCode code, int param) {
    if( code == EVENT_DOUBLECLICK && intfCanHandleInput() ) {
        terPlayer* activePlayer = universe()->activePlayer();
        CChatInGameEditWindow* chatInput = (CChatInGameEditWindow*) pWnd;
        if (activePlayer && !chatInput->getText().empty()) {
            int colorIndex = activePlayer->colorIndex();
            sColor4c activePlayerColor = sColor4c( sColor4f(playerColors[colorIndex].unitColor) );
            chatWindowInput(
                chatInput,
                chatInput->alliesOnlyMode ? activePlayer->clan() : -1,
                activePlayerColor,
                activePlayer->name()
            );
        }
    }
}


std::string toChatStr;

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
