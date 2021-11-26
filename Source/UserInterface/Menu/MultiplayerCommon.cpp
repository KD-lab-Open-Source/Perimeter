#include "StdAfx.h"
#include "MainMenu.h"
#include "GameShell.h"
#include "PerimeterShellUI.h"
#include "MessageBox.h"
#include "../../HT/ht.h"
#include "Localization.h"
#include "MultiplayerCommon.h"

//This file handles mostly ingame multiplayer stuff but some parts are shared with main menu

extern LogStream fout;
extern MissionDescription missionToExec;
extern MusicPlayer gb_Music;

std::vector<MissionDescription> multiplayerMaps;
std::vector<MissionDescription> multiplayerSaves;

void loadMultiplayerList() {
    if (multiplayerMaps.empty()) {
        loadMapVector(multiplayerMaps, "RESOURCE/MULTIPLAYER", ".spg");
    }
    if (multiplayerSaves.empty()) {
        loadMapVector(multiplayerSaves, "RESOURCE/SAVES/MULTIPLAYER", ".spg");
    }
}

int getMultiplayerMapNumber(const std::string& missionName) {
    for (uint32_t i = 0, s = multiplayerMaps.size(); i < s; i++) {
        if (missionName == multiplayerMaps[i].missionName()) {
            return static_cast<int>(i);
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

int SwitchMultiplayerToRestoreQuant(float, float ) {
    if (menuChangingDone) {
        if (missionToExec.gameType_ == GT_MULTI_PLAYER_RESTORE_PARTIAL) {
            MTAuto lock(HTManager::instance()->GetLockLogic());
            MTAutoSkipAssert skip_assert;
            universe()->clear();
            universe()->universalLoad(missionToExec, gameShell->savePrm(), nullptr);
            missionToExec.gameType_ = GT_MULTI_PLAYER_LOAD;
            //Report host that we finished
            gameShell->getNetClient()->GameIsReady();
        } else {
            gb_Music.FadeVolume(_fEffectButtonTotalTime * 0.001f);
            HTManager::instance()->GameClose();
            StartSpace();
            _shellIconManager.SetModalWnd(0);
            _shellIconManager.LoadControlsGroup(SHELL_LOAD_GROUP_MENU);
            _shellIconManager.SwitchMenuScreens(-1, SQSH_MM_LOADING_MISSION_SCR);
        }
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
    if (!isWorldIDValid(mission.worldID())
        || (mission.gameType_ == GT_MULTI_PLAYER_CREATE && getMultiplayerMapNumber(mission.missionName()) == -1)) {
        _shellIconManager.AddDynamicHandler(multiplayerMapNotFoundQuant, CBCODE_QUANT);
    } else {
        missionToExec = mission;
        _shellIconManager.AddDynamicHandler(SwitchMultiplayerToLoadQuant, CBCODE_QUANT);
    }
}

void GameShell::MultiplayerGameRestore(const MissionDescription& mission) {
    missionToExec = mission;
    _shellIconManager.AddDynamicHandler(SwitchMultiplayerToRestoreQuant, CBCODE_QUANT);
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

int showGeneralErrorMessageQuant(float, float ) {
    if (menuChangingDone) {
        std::string textID;
        bool terminate = true;
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
            case GameShell::DESYNC:
                textID = "Interface.Menu.Messages.Multiplayer.Nonsinchronization";
                terminate = false;
                break;
            default:
                textID = "Interface.Menu.Messages.UnknownError";
                break;
        }
        
        if (terminate) {
            gameShell->getNetClient()->FinishGame();
            setupOkMessageBox(exitToMultiplayerScreenAction, 0, 
                              qdTextDB::instance().getText(textID.c_str()), MBOX_EXIT);
        } else {
            setupOkMessageBox(nullptr, 0,
                              qdTextDB::instance().getText(textID.c_str()), MBOX_OK);
        }
        gameShell->prepareForInGameMenu();
//		fout < "showGeneralErrorMessageQuant showMessageBox()\n";
        showMessageBox();
        return 0;
    }
    return 1;
}

void GameShell::generalErrorOccured(GeneralErrorType error) {
    gtError = error;
    _shellIconManager.AddDynamicHandler(showGeneralErrorMessageQuant, CBCODE_QUANT);
}

////////// Lobby/Ingame chat /////////////

void chatWindowInput(CChatInGameEditWindow* chatInput, bool lobby) {
    std::string strToSay = (lobby ? "" : chatInput->getModePostfix()) + ": " + chatInput->getText();
    gameShell->getNetClient()->chatMessage(lobby ? false : chatInput->alliesOnlyMode, strToSay, getLocale());
    chatInput->SetText("");
}

void onMMLobbyChatInputButton(CShellWindow* pWnd, InterfaceEventCode code, int param) {
    if( code == EVENT_DOUBLECLICK && intfCanHandleInput() ) {
        CChatInGameEditWindow* chatInput = (CChatInGameEditWindow*) pWnd;
        if (!chatInput->getText().empty()) {
            chatWindowInput(chatInput, true);
        }
    }
}

void onMMInGameChatInputButton(CShellWindow* pWnd, InterfaceEventCode code, int param) {
    if( code == EVENT_DOUBLECLICK && intfCanHandleInput() ) {
        terPlayer* activePlayer = universe()->activePlayer();
        CChatInGameEditWindow* chatInput = (CChatInGameEditWindow*) pWnd;
        if (activePlayer && !chatInput->getText().empty()) {
            chatWindowInput(chatInput, false);
        }
    }
}


LocalizedText toChatText;

int addStringToChatWindowQuant( float, float ) {
    ChatWindow* chatWnd = (ChatWindow*)_shellIconManager.GetWnd(SQSH_MM_LOBBY_CHAT_TEXT);
    chatWnd->AddString(&toChatText);
    return 0;
}

int addStringToChatHintWindowQuant( float, float ) {
    _shellIconManager.addChatString(&toChatText);
    return 0;
}

void GameShell::addStringToChatWindow(const std::string& newString, const std::string& locale) {
    toChatText.set(newString, locale);
    if (_shellIconManager.GetWnd(SQSH_MM_LOBBY_CHAT_TEXT)) {
        _shellIconManager.AddDynamicHandler( addStringToChatWindowQuant, CBCODE_QUANT );
    } else {
        _shellIconManager.AddDynamicHandler( addStringToChatHintWindowQuant, CBCODE_QUANT );
    }
}
