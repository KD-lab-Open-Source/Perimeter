
#include "StdAfx.h"
#include "MainMenu.h"
#include "Config.h"
#include "Runtime.h"
#include "terra.h"
#include "GenericControls.h"
#include "Universe.h"
#include "UniverseInterface.h"
#include "GameShell.h"
#include "PerimeterShellUI.h"
#include "Controls.h"
#include "../Sound/PerimeterSound.h"
#include "GraphicsOptions.h"

#include "Silicon.h"
#include "HistoryScene.h"
#include "BGScene.h"
#include "MonoSelect.h"
#include "../HT/ht.h"
#include "qd_textdb.h"
#include "BelligerentSelect.h"
#include "files/files.h"

extern MusicPlayer gb_Music;
extern MissionDescription missionToExec;
extern BGScene bgScene;

std::vector<MissionDescription> battleMaps;
int defaultBattleMapCount = 0;
MonoSelect battleColors(UI_PLAYERS_MAX, playerAllowedColorSize);

//battle menu
void loadBattleList() {
	if (battleMaps.empty()) {
		loadMapVector(battleMaps, "RESOURCE/BATTLE", ".spg");
		defaultBattleMapCount = battleMaps.size();
        loadMapVector(battleMaps, "RESOURCE/BATTLE/SCENARIO", ".spg");
	}
}
std::string getSurvivalText(int pos) {
	std::string name = getMapName(battleMaps[pos].missionName().c_str());
	return name + qdTextDB::instance().getText("Interface.Menu.Messages.SurvivalRecord") + formatTimeWithHour(gameShell->currentSingleProfile.getRecord(battleMaps[pos].missionName()));
}

bool isSurvivalMode(int pos) {
	for (int i = 1, s = battleMaps[pos].playersAmountScenarioMax(); i < s; i++) {
		if (battleColors.getPosition(i) != -1) {
			return false;
		}
	}
	return true;
}
void setSlotClosed(int i, bool closed, bool initSlot = false) {
	CComboWindow* pCombo = (CComboWindow*) _shellIconManager.GetWnd(SQSH_MM_BATTLE_PLAYER1_SLOT_BTN + i);
	if (closed) {
		pCombo->pos = 0;
	} else if (initSlot) {
		pCombo->pos = 1;
	}
	CColorComboWindow* pColorCombo = (CColorComboWindow*) _shellIconManager.GetWnd(SQSH_MM_BATTLE_PLAYER1_CLR_BTN + i);
	if (closed) {
		battleColors.disable(i);
	} else if (battleColors.getPosition(i) == -1) {
		pColorCombo->pos = battleColors.enable(i);
	}
}

void setSlotVisible(int i, bool visible) {
	bool slotStateVisible = _shellIconManager.GetWnd(SQSH_MM_BATTLE_PLAYER1_SLOT_BTN + i)->isVisible();
//	_shellIconManager.GetWnd(SQSH_MM_BATTLE_PLAYER1_SLOT_BTN + i)->Show(stateVisible);
	_shellIconManager.GetWnd(SQSH_MM_BATTLE_PLAYER1_CLAN_BTN + i)->Show(visible && slotStateVisible);
	_shellIconManager.GetWnd(SQSH_MM_BATTLE_PLAYER1_HC_BTN + i)->Show(visible && slotStateVisible);
	_shellIconManager.GetWnd(SQSH_MM_BATTLE_PLAYER1_FRM_BTN + i)->Show(visible && slotStateVisible);
	_shellIconManager.GetWnd(SQSH_MM_BATTLE_PLAYER1_CLR_BTN + i)->Show(visible && slotStateVisible);
	_shellIconManager.GetWnd(SQSH_MM_BATTLE_PLAYER1_CLR_BG + i)->Show(visible && slotStateVisible);
}
void setupBattleDescWnd(int index, std::vector<MissionDescription>& mVect, int mapWndID, int mapDescrWndID) {
	checkMissionDescription(index, mVect, GT_SINGLE_PLAYER);
	((CShowMapWindow*)_shellIconManager.GetWnd(mapWndID))->setWorldID( mVect[index].worldID() );
//	((CTextWindow*)_shellIconManager.GetWnd(mapDescrWndID))->setText( mVect[index].missionDescription() );
	((CTextWindow*)_shellIconManager.GetWnd(SQSH_MM_BATTLE_SURVIVAL_TXT))->setText( getSurvivalText(index) );
}
void onBattleMenuOpening() {
	StartSpace();
	CListBoxWindow* list = (CListBoxWindow*)_shellIconManager.GetWnd(SQSH_MM_MAP_LIST);
	list->NewItem(1);
	list->Clear();
    int s = defaultBattleMapCount + gameShell->currentSingleProfile.getLastMissionNumber();
//	int s = defaultBattleMapCount;
    if(s > battleMaps.size() || s < 0)
        s = battleMaps.size();

	for (int i = 0; i < s; i++) {
		std::string name = getMapName(battleMaps[i].missionName().c_str());
		list->AddString(name.c_str(), 0 );
	}

	if (s) {
		list->SetCurSel(0);
		setupBattleDescWnd(0, battleMaps, SQSH_MM_BATTLE_MAP, SQSH_MM_BATTLE_MAP_DESCR_TXT);
	} else {
		list->SetCurSel(-1);
		clearMapDescWnd(SQSH_MM_BATTLE_MAP, SQSH_MM_BATTLE_MAP_DESCR_TXT, -1);
		((CTextWindow*)_shellIconManager.GetWnd(SQSH_MM_BATTLE_SURVIVAL_TXT))->setText( "" );
	}
	battleColors.reset();
	battleColors.enable(0);
	setSlotClosed(1, false, true);
	setSlotClosed(2, true);
	setSlotClosed(3, true);
}
std::string getSurvivalFileName(const std::string& fileName) {
    //Will return empty string if file wasn't indexed
    std::string path = convert_path_content("Resource/BATTLE/SURVIVAL/" + fileName + ".spg");
    bool exists = !path.empty() && std::filesystem::exists(std::filesystem::u8path(path));
	return exists ? path : std::string();
}

void startBattle(int pos, CShellWindow* pWnd) {
	checkMissionDescription(pos, battleMaps, GT_SINGLE_PLAYER);
	missionToExec = battleMaps[pos];
	int i;
	for (i = 1; i < missionToExec.playersAmountScenarioMax(); i++) {
		if (battleColors.getPosition(i) != -1) {
			break;
		}
	}
	bool survival = false;
	if (i == missionToExec.playersAmountScenarioMax()) {
		survival = true;
		std::string surv = getSurvivalFileName(battleMaps[pos].missionName());
		if (surv != "") {
			missionToExec = MissionDescription(surv.c_str());
		}
	}

	missionToExec.getActivePlayerData().setName(gameShell->currentSingleProfile.getCurrentProfile().name.c_str());

	for (i = 0; i < missionToExec.playersAmountScenarioMax(); i++) {
		if (battleColors.getPosition(i) != -1) {
			missionToExec.playersData[i].colorIndex = battleColors.getPosition(i);
			missionToExec.playersData[i].realPlayerType = i ? REAL_PLAYER_TYPE_AI : REAL_PLAYER_TYPE_PLAYER;
			CComboWindow* combo = (CComboWindow*) _shellIconManager.GetWnd(SQSH_MM_BATTLE_PLAYER1_FRM_BTN + i);
            missionToExec.playersData[i].belligerent = SelectableBelligerents[combo->pos];
			combo = (CComboWindow*) _shellIconManager.GetWnd(SQSH_MM_BATTLE_PLAYER1_SLOT_BTN + i);
			int diff = combo->pos - 1;
			if (diff < DIFFICULTY_EASY || diff >= DIFFICULTY_MAX) {
				diff = DIFFICULTY_HARD;
			}
			missionToExec.playersData[i].difficulty = (Difficulty) diff;

			combo = (CComboWindow*) _shellIconManager.GetWnd(SQSH_MM_BATTLE_PLAYER1_CLAN_BTN + i);
			missionToExec.playersData[i].clan = combo->pos;
			combo = (CComboWindow*) _shellIconManager.GetWnd(SQSH_MM_BATTLE_PLAYER1_HC_BTN + i);
			switch (combo->pos) {
				case 0:
					missionToExec.playersData[i].handicap = 100;
					break;
				case 1:
					missionToExec.playersData[i].handicap = 70;
					break;
				case 2:
					missionToExec.playersData[i].handicap = 50;
					break;
				case 3:
					missionToExec.playersData[i].handicap = 30;
					break;
			}
		} else {
			missionToExec.playersData[i].realPlayerType = REAL_PLAYER_TYPE_CLOSE;
		}
	}

	missionToExec.shufflePlayers();

	gameShell->currentSingleProfile.setLastGameType(survival ? UserSingleProfile::SURVIVAL : UserSingleProfile::BATTLE);

	gb_Music.FadeVolume(_fEffectButtonTotalTime*0.001f);
	_shellIconManager.SwitchMenuScreens(pWnd->m_pParent->ID, SQSH_MM_LOADING_MISSION_SCR);
}

void onMMMapList(CShellWindow* pWnd, InterfaceEventCode code, int param) {
	if ( code == EVENT_PRESSED && intfCanHandleInput() ) {
		CListBoxWindow* list = (CListBoxWindow*)_shellIconManager.GetWnd(SQSH_MM_MAP_LIST);
		int pos = list->GetCurSel();
		if (pos >= 0 && pos < battleMaps.size()) {
			checkMissionDescription(pos, battleMaps, GT_SINGLE_PLAYER);
			((CShowMapWindow*)_shellIconManager.GetWnd(SQSH_MM_BATTLE_MAP))->setWorldID( battleMaps[pos].worldID() );
			((CTextWindow*)_shellIconManager.GetWnd(SQSH_MM_BATTLE_MAP_DESCR_TXT))->setText( battleMaps[pos].missionDescription() );
			((CTextWindow*)_shellIconManager.GetWnd(SQSH_MM_BATTLE_SURVIVAL_TXT))->setText( getSurvivalText(pos) );
		}
	} else if ( code == EVENT_DOUBLECLICK && intfCanHandleInput() ) {
		int pos = ((CListBoxWindow*)_shellIconManager.GetWnd(SQSH_MM_MAP_LIST))->GetCurSel();
		if (pos != -1) {
			startBattle(pos, pWnd);
		}
	} else if (code == EVENT_DRAWWND) {
		int pos = ((CListBoxWindow*)_shellIconManager.GetWnd(SQSH_MM_MAP_LIST))->GetCurSel();
		_shellIconManager.GetWnd(SQSH_MM_BATTLE_GO_BTN)->Enable(pos != -1);
		CTextWindow* textWnd = (CTextWindow*) _shellIconManager.GetWnd(SQSH_MM_BATTLE_MAP_DESCR_TXT);
		if (isSurvivalMode(pos)) {
			textWnd->setText( qdTextDB::instance().getText("Interface.Menu.Messages.Survival") );
			_shellIconManager.GetWnd(SQSH_MM_BATTLE_SURVIVAL_TXT)->Show(1);
		} else {
			textWnd->setText( qdTextDB::instance().getText("Interface.Menu.Messages.Battle") );
			_shellIconManager.GetWnd(SQSH_MM_BATTLE_SURVIVAL_TXT)->Show(0);
		}
	}
}

void onMMBattleGoButton(CShellWindow* pWnd, InterfaceEventCode code, int param) {
	if( code == EVENT_UNPRESSED && intfCanHandleInput() ) {
		int pos = ((CListBoxWindow*)_shellIconManager.GetWnd(SQSH_MM_MAP_LIST))->GetCurSel();
		startBattle(pos, pWnd);
	}		
}

void onMMBattleFrmButton(CShellWindow* pWnd, InterfaceEventCode code, int param) {
    setupFrameButton(pWnd, code, pWnd->ID - SQSH_MM_BATTLE_PLAYER1_FRM_BTN, false);
}

void onMMBattleClrButton(CShellWindow* pWnd, InterfaceEventCode code, int param) {
	CColorComboWindow *pCombo = (CColorComboWindow*) pWnd;
	if( code == EVENT_CREATEWND ) {
		pCombo->pos = 3 - (SQSH_MM_BATTLE_PLAYER4_CLR_BTN - pWnd->ID);
	} else if ( code == EVENT_UNPRESSED && intfCanHandleInput() ) {
		pCombo->pos = battleColors.putNext(3 - (SQSH_MM_BATTLE_PLAYER4_CLR_BTN - pWnd->ID));
	} else if ( code == EVENT_RUNPRESSED && intfCanHandleInput() ) {
		pCombo->pos = battleColors.putPrevious(3 - (SQSH_MM_BATTLE_PLAYER4_CLR_BTN - pWnd->ID));
	}
}

void onMMBattleFirstSlotButton(CShellWindow* pWnd, InterfaceEventCode code, int param) {
	if( code == EVENT_CREATEWND ) {
		CComboWindow *pCombo = (CComboWindow*) pWnd;
		pCombo->Array.emplace_back(getItemTextFromBase("Player"));
		pCombo->size = 1;
		pCombo->pos = 0;
	} else if ( code == EVENT_DRAWWND ) {
		int maxCount = battleMaps[((CListBoxWindow*)_shellIconManager.GetWnd(SQSH_MM_MAP_LIST))->GetCurSel()].playerAmountScenarioMax;
		for (int i = 1; i < UI_PLAYERS_MAX; i++) {
			_shellIconManager.GetWnd(SQSH_MM_BATTLE_PLAYER1_SLOT_BTN + i)->Show(i < maxCount);
            setSlotVisible(i, battleColors.getPosition(i) != -1);
            
            //Setup clan combo if amount of map doesn't match
            CComboWindow *pCombo = dynamic_cast<CComboWindow*>(_shellIconManager.GetWnd(SQSH_MM_BATTLE_PLAYER1_CLAN_BTN + i));
            if (pCombo->size != maxCount) {
                pCombo->Array.clear();
                std::string clan = getItemTextFromBase("Clan");
                for (int i = 0; i < maxCount; i++) {
                    pCombo->Array.emplace_back(clan + std::to_string(i + 1));
                }
                pCombo->size = pCombo->Array.size();
                pCombo->pos = i;
            }
        }
	}
}

void onMMBattleSlotButton(CShellWindow* pWnd, InterfaceEventCode code, int param) {
	CComboWindow *pCombo = (CComboWindow*) pWnd;
	if( code == EVENT_CREATEWND ) {
		pCombo->Array.emplace_back( getItemTextFromBase("Closed") );
		pCombo->Array.emplace_back( getItemTextFromBase("AI (Easy)") );
		pCombo->Array.emplace_back( getItemTextFromBase("AI (Normal)") );
        pCombo->Array.emplace_back( getItemTextFromBase("AI (Hard)") );
		pCombo->size = pCombo->Array.size();
		pCombo->pos = 0;
	} else if ( (code == EVENT_UNPRESSED || code == EVENT_RUNPRESSED) && intfCanHandleInput() ) {
		setSlotClosed(3 - (SQSH_MM_BATTLE_PLAYER4_SLOT_BTN - pWnd->ID), pCombo->pos == 0);
	}
}
void onMMBattleClanButton(CShellWindow* pWnd, InterfaceEventCode code, int param) {
	if( code == EVENT_CREATEWND ) {
		CComboWindow *pCombo = (CComboWindow*) pWnd;
        std::string clan = getItemTextFromBase("Clan");
        pCombo->Array.clear();
        pCombo->Array.push_back(clan + "1");
		pCombo->size = pCombo->Array.size();
		pCombo->pos = 0;
	}
}
void onMMBattleHCButton(CShellWindow* pWnd, InterfaceEventCode code, int param) {
	if( code == EVENT_CREATEWND ) {
		CComboWindow *pCombo = (CComboWindow*) pWnd;
		pCombo->Array.emplace_back("100%" );
		pCombo->Array.emplace_back("70%" );
		pCombo->Array.emplace_back("50%" );
		pCombo->Array.emplace_back("30%" );
		pCombo->size = 4;
		pCombo->pos = 0;
	}
}


//////////////////////

