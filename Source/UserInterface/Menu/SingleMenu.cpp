#include "StdAfx.h"
#include "MainMenu.h"
#include "GameShell.h"
#include "PerimeterShellUI.h"
#include "MessageBox.h"
#include "HistoryScene.h"
#include "BGScene.h"
#include "GameContent.h"

extern MusicPlayer gb_Music;

void onMMSingleButton(CShellWindow* pWnd, InterfaceEventCode code, int param) {
    if( code == EVENT_UNPRESSED && intfCanHandleInput() ) {
        showSingleMenu(pWnd);
    }
}

//single player menu
void showSingleMenu(CShellWindow* pWnd) {
//	CShellPushButton* txtWnd = dynamic_cast<CShellPushButton*>(_shellIconManager.GetWnd(SQSH_MM_PROFILE_BTN));
//	txtWnd->setText(gameShell->currentSingleProfile.getName());
    if (gameShell->currentSingleProfile.getCurrentProfileIndex() == -1) {
        _shellIconManager.SwitchMenuScreens(pWnd->m_pParent->ID, SQSH_MM_PROFILE_SCR);
    } else {
        _shellIconManager.SwitchMenuScreens(pWnd->m_pParent->ID, SQSH_MM_SINGLE_SCR);
    }
}

void onMMScenarioButton(CShellWindow* pWnd, InterfaceEventCode code, int param) {
    if( code == EVENT_UNPRESSED && intfCanHandleInput() ) {
        _shellIconManager.SwitchMenuScreens(pWnd->m_pParent->ID, SQSH_MM_SCENARIO_SCR);
//		} else {
//			historyScene.goToMission(-1);
//			_shellIconManager.SwitchMenuScreens(pWnd->m_pParent->ID, SQSH_MM_BRIEFING_SCR);
//		}
    }
}
void onMMProfileButton(CShellWindow* pWnd, InterfaceEventCode code, int param) {
    if( code == EVENT_UNPRESSED && intfCanHandleInput() ) {
        _shellIconManager.SwitchMenuScreens(pWnd->m_pParent->ID, SQSH_MM_PROFILE_SCR);
    }
}
void onMMBattleButton(CShellWindow* pWnd, InterfaceEventCode code, int param) {
    if( code == EVENT_UNPRESSED && intfCanHandleInput() ) {
        _shellIconManager.SwitchMenuScreens(pWnd->m_pParent->ID, SQSH_MM_BATTLE_SCR);
    }
}
void onMMLoadButton(CShellWindow* pWnd, InterfaceEventCode code, int param) {
    if( code == EVENT_UNPRESSED && intfCanHandleInput() ) {
        _shellIconManager.SwitchMenuScreens(pWnd->m_pParent->ID, SQSH_MM_LOAD_SCR);
    }
}
void onMMLoadReplayButton(CShellWindow* pWnd, InterfaceEventCode code, int param) {
    if( code == EVENT_UNPRESSED && intfCanHandleInput() ) {
        _shellIconManager.SwitchMenuScreens(pWnd->m_pParent->ID, SQSH_MM_LOAD_REPLAY_SCR);
    }
}

//profile menu
void fillProfileList() {
    CListBoxWindow* list = (CListBoxWindow*)_shellIconManager.GetWnd(SQSH_MM_PROFILE_LIST);
    list->NewItem(1);
    list->Clear();
    const std::vector<Profile>& profiles = gameShell->currentSingleProfile.getProfilesVector();
    for (int i = 0, s = profiles.size(); i < s; i++) {
        list->AddString( profiles[i].name.c_str(), 0 );
    }
    if (gameShell->currentSingleProfile.getCurrentProfileIndex() != -1) {
        list->SetCurSel(gameShell->currentSingleProfile.getCurrentProfileIndex());
    } else {
        list->SetCurSel(list->GetItemCount() - 1);
    }
}

void onMMProfileList(CShellWindow* pWnd, InterfaceEventCode code, int param) {
    if ( code == EVENT_DOUBLECLICK && intfCanHandleInput() ) {
        CListBoxWindow* list = (CListBoxWindow*)pWnd;
        int pos = list->GetCurSel();
        if (pos != -1) {
            gameShell->currentSingleProfile.setCurrentProfileIndex( pos );
            putStringSettings("ProfileName", gameShell->currentSingleProfile.getCurrentProfile().name);
            showSingleMenu(pWnd);
        }
    }
}
void onMMNewProfileButton(CShellWindow* pWnd, InterfaceEventCode code, int param) {
    if( code == EVENT_UNPRESSED && intfCanHandleInput() && pWnd->isEnabled()) {
        CEditWindow* input = (CEditWindow*)_shellIconManager.GetWnd(SQSH_MM_PROFILE_NAME_INPUT);
        gameShell->currentSingleProfile.addProfile(input->GetText());
        fillProfileList();
    } else if (code == EVENT_DRAWWND) {
        CEditWindow* input = (CEditWindow*)_shellIconManager.GetWnd(SQSH_MM_PROFILE_NAME_INPUT);
        bool enable = false;
        if ( *(input->GetText()) ) {
            const std::vector<Profile>& profiles = gameShell->currentSingleProfile.getProfilesVector();
            enable = true;
            for (int i = 0, s = profiles.size(); i < s; i++) {
                if (strcmp(input->GetText(), profiles[i].name.c_str()) == 0) {
                    enable = false;
                    break;
                }
            }
        }
        pWnd->Enable( enable );
    }
}
int delProfileAction(float, float) {
    CListBoxWindow* list = (CListBoxWindow*)_shellIconManager.GetWnd(SQSH_MM_PROFILE_LIST);
    gameShell->currentSingleProfile.removeProfile(list->GetCurSel());
    fillProfileList();
    return 0;
}
void onMMDelProfileButton(CShellWindow* pWnd, InterfaceEventCode code, int param) {
    if( code == EVENT_UNPRESSED && intfCanHandleInput() && param == VK_LBUTTON) {
        //delete selected profile
        CListBoxWindow* list = (CListBoxWindow*)_shellIconManager.GetWnd(SQSH_MM_PROFILE_LIST);

        std::string text = qdTextDB::instance().getText("Interface.Menu.Messages.Confirmations.DeleteProfile");
        std::string profileName = gameShell->currentSingleProfile.getProfilesVector()[list->GetCurSel()].name;
        char* mess = new char[text.length() + profileName.length()];
        sprintf(mess, text.c_str(), profileName.c_str());
        setupYesNoMessageBox(delProfileAction, 0, mess);
        delete [] mess;

        showMessageBox();
    } else if (code == EVENT_DRAWWND) {
        CListBoxWindow* list = (CListBoxWindow*)_shellIconManager.GetWnd(SQSH_MM_PROFILE_LIST);
        pWnd->Enable( list->GetCurSel() >= 0 );
    }
}
void onMMSelectProfileButton(CShellWindow* pWnd, InterfaceEventCode code, int param) {
    if ( code == EVENT_UNPRESSED && intfCanHandleInput() && pWnd->isEnabled() ) {
        CListBoxWindow* list = (CListBoxWindow*)_shellIconManager.GetWnd(SQSH_MM_PROFILE_LIST);
        gameShell->currentSingleProfile.setCurrentProfileIndex( list->GetCurSel() );
        putStringSettings("ProfileName", gameShell->currentSingleProfile.getCurrentProfile().name);
        showSingleMenu(pWnd);
    } else if (code == EVENT_DRAWWND) {
        CListBoxWindow* list = (CListBoxWindow*)_shellIconManager.GetWnd(SQSH_MM_PROFILE_LIST);
        pWnd->Enable( list->GetCurSel() >= 0 );
    }
}
void onMMBackFromProfileButton(CShellWindow* pWnd, InterfaceEventCode code, int param) {
    if ( code == EVENT_UNPRESSED && intfCanHandleInput() ) {
        if (gameShell->currentSingleProfile.getCurrentProfileIndex() != -1) {
            showSingleMenu(pWnd);
        } else {
            _shellIconManager.SwitchMenuScreens(pWnd->m_pParent->ID, SQSH_MM_START_SCR);
        }
    }
}


//content chooser menu
void fillContentChooserList() {
    CListBoxWindow* list = reinterpret_cast<CListBoxWindow*>(_shellIconManager.GetWnd(SQSH_MM_CONTENT_CHOOSER_LIST));
    //list->NewItem(1);
    list->Clear();
    int idx = 0;
    int i = 0;
    for (GAME_CONTENT selectable : getGameContentEnums(terGameContentAvailable)) {
        std::string name = getGameContentName(selectable);
        list->AddString(name.c_str());
        if (selectable == terGameContentBase) {
            selectable = terGameContentAvailable;
        }
        if (selectable == terGameContentSelect) {
            idx = i;
        }
        i++;
    }
    list->SetCurSel(idx);
}

void selectedContentChooser() {
    CListBoxWindow* list = reinterpret_cast<CListBoxWindow*>(_shellIconManager.GetWnd(SQSH_MM_CONTENT_CHOOSER_LIST));
    int pos = list->GetCurSel();
    std::vector<GAME_CONTENT> contents = getGameContentEnums(terGameContentAvailable);
    if (0 <= pos && pos < contents.size()) {
        switchGameContent(contents[pos], "SINGLE");
    }
}

void onMMContentChooserButton(CShellWindow* pWnd, InterfaceEventCode code, int param) {
    if( code == EVENT_UNPRESSED && intfCanHandleInput() ) {
        _shellIconManager.SwitchMenuScreens(pWnd->m_pParent->ID, SQSH_MM_CONTENT_CHOOSER_SCR);
    }
}
        
void onMMContentChooserList(CShellWindow* pWnd, InterfaceEventCode code, int param) {
    if ( code == EVENT_DOUBLECLICK && intfCanHandleInput() ) {
        selectedContentChooser();
    }
}

void onMMContentChooserSelectButton(CShellWindow* pWnd, InterfaceEventCode code, int param) {
    if ( code == EVENT_UNPRESSED && intfCanHandleInput() && pWnd->isEnabled() ) {
        selectedContentChooser();
    //} else if (code == EVENT_DRAWWND) {
    //    pWnd->Enable( list->GetCurSel() >= 0 );
    }
}

//briefing menu
extern MissionDescription missionToExec;
extern HistoryScene historyScene;
extern BGScene bgScene;
extern bool menuChangingDone;
double lastClockSound = 0;
int currYear = -1;

void onMMYearBriefing(CShellWindow* pWnd, InterfaceEventCode code, int param) {
    if ( code == EVENT_DRAWWND ) {
        CTextWindow* txtWnd = (CTextWindow*) pWnd;
        char buffer[30 + 1];
        int year = historyScene.getController()->getCurrentYear();
        if (currYear == -1 || currYear != year) {
            currYear = historyScene.getController()->getCurrentYear();
            //Avoid too much clock noises at once
            //We try to emulate the same random effect that DirectSound API gave in this situation
            if (clockf() > lastClockSound) {
                lastClockSound = clockf() + 50.0 + 50.0 * terEffectRND.frand();
                SND2DPlaySound("mainmenu_clock");
                //printf("clock! %d\n", currYear);
            }
        }
        sprintf(buffer, "%d", currYear >= 0 ? currYear : 0);
        std::string res(getItemTextFromBase("Year"));
        res += buffer;
        txtWnd->setText(res);
    }
}
void onMMNomadNameBriefing(CShellWindow* pWnd, InterfaceEventCode code, int param) {
    if ( code == EVENT_DRAWWND ) {
        CTextWindow* txtWnd = (CTextWindow*) pWnd;
        Frame* frame = historyScene.getNomadFrame();
        if (frame) {
            txtWnd->setText(HistoryScene::getFrameNameFromBase(frame->getName()));
            txtWnd->colorIndex = frame->getColorIndex();
        } else if (txtWnd->getText() != "") {
            txtWnd->setText("");
        }
        int index = frame ? frame->getColorIndex() : DEFAULT_NOMAD_COLOR_INDEX;
        bgScene.setSkinColor(sColor4f(playerColors[index].unitColor));
    }
}
void onMMNomadIconBriefing(CShellWindow* pWnd, InterfaceEventCode code, int param) {
    if( code == EVENT_DRAWWND ) {
        Frame* frame = historyScene.getNomadFrame();
        if (frame) {
            ((CLogoWindow*)pWnd)->setRace(frame->getRace());
        } else {
            ((CLogoWindow*)pWnd)->setRace(-1);
        }
    }
}
void onMMSkipBriefingButton(CShellWindow* pWnd, InterfaceEventCode code, int param) {
    if( code == EVENT_UNPRESSED && intfCanHandleInput() ) {
        if (!_shellIconManager.IsEffect()) {
            historyScene.stop();
            if (_shellIconManager.GetWnd(SQSH_MM_CONTINUE_BRIEFING_BTN)->isVisible()) {
                _shellIconManager.SwitchMenuScreens( SKIP_BRIEFING_AFTER_PAUSE, SKIP_BRIEFING_AFTER_PAUSE );
            } else {
                _shellIconManager.SwitchMenuScreens( SKIP_BRIEFING, SKIP_BRIEFING );
            }
        }
    }
}
void onMMQuitFromBriefingButton(CShellWindow* pWnd, InterfaceEventCode code, int param) {
    if( code == EVENT_UNPRESSED && intfCanHandleInput() ) {
        historyScene.stopAudio();
        historyScene.stop();
        historyScene.hideText();
        _shellIconManager.SwitchMenuScreens(pWnd->m_pParent->ID, SQSH_MM_SCENARIO_SCR);
    }
}
void onMMContinueBriefingButton(CShellWindow* pWnd, InterfaceEventCode code, int param) {
    if( code == EVENT_UNPRESSED && intfCanHandleInput() ) {
        _shellIconManager.SwitchMenuScreens( CONTINUE_BRIEFING, CONTINUE_BRIEFING );
    }
}
void HistoryScene::audioStopped() {
    if( menuChangingDone && intfCanHandleInput() && _shellIconManager.GetWnd(SQSH_MM_CONTINUE_BRIEFING_BTN)->isVisible()) {
        _shellIconManager.SwitchMenuScreens( CONTINUE_BRIEFING, CONTINUE_BRIEFING );
    }
}
void onMMStartMissionButton(CShellWindow* pWnd, InterfaceEventCode code, int param) {
    if( code == EVENT_UNPRESSED && intfCanHandleInput() ) {
        historyScene.stopAudio();
        historyScene.stop();
//		missionToExec = MissionDescription(
//				gameShell->currentSingleProfile.getFileNameWithDifficulty( historyScene.getMissionToExecute().fileName ).c_str()
//			);
        missionToExec = MissionDescription( ("RESOURCE\\MISSIONS\\" + historyScene.getMissionToExecute().fileName).c_str() );

        //NOTE: should be removed when difficulty will be implemented for each separate player
//		missionToExec.getActivePlayerData().difficulty = gameShell->currentSingleProfile.getDifficulty();
        missionToExec.setSinglePlayerDifficulty( gameShell->currentSingleProfile.getDifficulty() );
        missionToExec.getActivePlayerData().setName(gameShell->currentSingleProfile.getCurrentProfile().name.c_str());
        //NOTE: Setup all names

        gameShell->currentSingleProfile.setLastGameType(UserSingleProfile::SCENARIO);

        gb_Music.FadeVolume(_fEffectButtonTotalTime*0.001f);
        _shellIconManager.SwitchMenuScreens(pWnd->m_pParent->ID, SQSH_MM_LOADING_MISSION_SCR);
    }
}
void onMMSkipMissionButton(CShellWindow* pWnd, InterfaceEventCode code, int param) {
    if( code == EVENT_UNPRESSED && intfCanHandleInput() ) {
        _shellIconManager.SwitchMenuScreens( SKIP_MISSION, SKIP_MISSION );
    }
}

//scenario menu
void onMMDifficultyCombo(CShellWindow* pWnd, InterfaceEventCode code, int param) {
    if( code == EVENT_CREATEWND ) {
        CComboWindow *pCombo = (CComboWindow*) pWnd;
        pCombo->Array.push_back( getItemTextFromBase("Easy").c_str() );
        pCombo->Array.push_back( getItemTextFromBase("Normal").c_str() );
        pCombo->Array.push_back( getItemTextFromBase("Hard").c_str() );
        pCombo->size = pCombo->Array.size();
        pCombo->pos = 0;
    } else if ( (code == EVENT_UNPRESSED || code == EVENT_RUNPRESSED) && intfCanHandleInput() )	{
        CComboWindow *pCombo = (CComboWindow*) pWnd;
        gameShell->currentSingleProfile.setDifficulty((Difficulty)pCombo->pos);
    }
}

void goToMissionWorkaround(int missionNumber) {
    //Weird workaround by trial and error for Perimeter (not ET) first mission but works for ET too
    //this basically jumpstarts the history to the first mission while showing the intro vid
    historyScene.goToMission(0);
    int year = historyScene.getController()->getMissionYear(missionNumber);
    year = std::max(0, year-2);
    historyScene.getController()->goToYear(year);
}

void launchCurrentMission(CShellWindow* pWnd) {
    CListBoxWindow* list = (CListBoxWindow*) _shellIconManager.GetWnd(SQSH_MM_MISSION_LIST);
    int missionNumber = list->GetCurSel();
    if ( gameShell->briefingEnabled && missionNumber >= firstMissionNumber) {
        historyScene.setMissionNumberToExecute(missionNumber);
        if (missionNumber == 0 || missionNumber == firstMissionNumber) {
            goToMissionWorkaround(missionNumber);
        } else {
            //Start after finishing the prev mission, so we can see all the plot and also not have screwed up
            //dialog or camera
            historyScene.goToJustAfterMissionPosition(missionNumber - 1);
        }
        historyScene.hideText();
        _shellIconManager.SwitchMenuScreens(pWnd->m_pParent->ID, SQSH_MM_BRIEFING_SCR);
    } else {
        if (missionNumber < firstMissionNumber) {
            //If before first mission (like tutorial) just go to mission after this to avoid year 0 after completing it
            goToMissionWorkaround(missionNumber + 1);
        } else {
            goToMissionWorkaround(missionNumber);
        }
        missionToExec = MissionDescription( ("RESOURCE\\MISSIONS\\" + historyScene.getMission(missionNumber).fileName).c_str() );

        //NOTE: should be removed when difficulty will be implemented for each separate player
        missionToExec.getActivePlayerData().difficulty = gameShell->currentSingleProfile.getDifficulty();
        missionToExec.getActivePlayerData().setName(gameShell->currentSingleProfile.getCurrentProfile().name.c_str());
        //NOTE: Setup all names

        gameShell->currentSingleProfile.setLastGameType(UserSingleProfile::SCENARIO);

        gb_Music.FadeVolume(_fEffectButtonTotalTime*0.001f);
        _shellIconManager.SwitchMenuScreens(pWnd->m_pParent->ID, SQSH_MM_LOADING_MISSION_SCR);
    }
}

void onMMMissionList(CShellWindow* pWnd, InterfaceEventCode code, int param) {
    if( code == EVENT_DOUBLECLICK && intfCanHandleInput() ) {
        launchCurrentMission(pWnd);
    }
}

void onMMGoButton(CShellWindow* pWnd, InterfaceEventCode code, int param) {
    if( code == EVENT_UNPRESSED && intfCanHandleInput() ) {
        launchCurrentMission(pWnd);
    }
}
