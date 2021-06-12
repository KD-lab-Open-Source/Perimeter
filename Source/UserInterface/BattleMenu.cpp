
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

extern MpegSound gb_Music;
extern MissionDescription missionToExec;
extern BGScene bgScene;

vector<MissionDescription> battleMaps;
int defaultBattleMapCount = 0;
MonoSelect battleColors(4, playerAllowedColorSize);

//battle menu
STARFORCE_API void loadBattleList() {
	if (battleMaps.empty()) {
		loadMapVector(battleMaps, "RESOURCE\\BATTLE\\", "RESOURCE\\BATTLE\\*.spg");
		defaultBattleMapCount = battleMaps.size();
		FILE* file = fopen("RESOURCE\\BATTLE\\SCENARIO\\maplist.txt", "rt");
		if (file) {
			char* buff = new char[201];
			while ( fgets(buff, 200, file) != NULL ) {
				if ( ferror(file) ) {
					break;
				}
				MissionDescription mission;
				string name = "RESOURCE\\BATTLE\\SCENARIO\\" + string(buff);
				mission.setSaveName(name.c_str());
				battleMaps.push_back(mission);
			}
			delete [] buff;
			fclose(file);
		}
	}
}
string getSurvivalText(int pos) {
	string name = "MapNames.";
	name += battleMaps[pos].missionName();
	name = qdTextDB::instance().getText(name.c_str());
	if (name.empty()) {
		name = battleMaps[pos].missionName();
	}
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
void setupBattleDescWnd(int index, vector<MissionDescription>& mVect, int mapWndID, int mapDescrWndID) {
	checkMissionDescription(index, mVect);
	((CShowMapWindow*)_shellIconManager.GetWnd(mapWndID))->setWorldID( mVect[index].worldID() );
//	((CTextWindow*)_shellIconManager.GetWnd(mapDescrWndID))->setText( mVect[index].missionDescription() );
	((CTextWindow*)_shellIconManager.GetWnd(SQSH_MM_BATTLE_SURVIVAL_TXT))->setText( getSurvivalText(index) );
}
STARFORCE_API void onBattleMenuOpening() {
	StartSpace();
	CListBoxWindow* list = (CListBoxWindow*)_shellIconManager.GetWnd(SQSH_MM_MAP_LIST);
	list->NewItem(1);
	list->Clear();
	#ifdef _DEMO_
		int s = 2;
	#else
		int s = defaultBattleMapCount + gameShell->currentSingleProfile.getLastMissionNumber();
//		int s = defaultBattleMapCount;
		if(s > battleMaps.size() || s < 0)
			s = battleMaps.size();
	#endif

	for (int i = 0; i < s; i++) {
		string name = "MapNames.";
		name += battleMaps[i].missionName();
		name = qdTextDB::instance().getText(name.c_str());
		if (name.empty()) {
			name = battleMaps[i].missionName();
		}
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
string getSurvivalFileName(const string& fileName) {
	string res = "RESOURCE\\BATTLE\\SURVIVAL\\" + fileName + ".spg";

	WIN32_FIND_DATA FindFileData;
	HANDLE hf = FindFirstFile( res.c_str(), &FindFileData );
	return (hf == INVALID_HANDLE_VALUE) ? string("") : res;
}

STARFORCE_API void startBattle(int pos, CShellWindow* pWnd) {
	checkMissionDescription(pos, battleMaps);
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
		string surv = getSurvivalFileName(battleMaps[pos].missionName());
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
			switch (combo->pos) {
				case 0:
					missionToExec.playersData[i].belligerent = BELLIGERENT_EXODUS0;
					break;
				case 1:
					missionToExec.playersData[i].belligerent = BELLIGERENT_EMPIRE0;
					break;
				#ifndef _DEMO_
					case 2:
						missionToExec.playersData[i].belligerent = BELLIGERENT_HARKBACKHOOD0;
						break;
				#endif
				default:
					missionToExec.playersData[i].belligerent = BELLIGERENT_EXODUS0;
			}
			combo = (CComboWindow*) _shellIconManager.GetWnd(SQSH_MM_BATTLE_PLAYER1_SLOT_BTN + i);
			int diff = combo->pos - 1;
			if (diff < DIFFICULTY_EASY || diff >= DIFFICULTY_MAX) {
				diff = DIFFICULTY_HARD;
			}
			missionToExec.playersData[i].difficulty = (Difficulty) diff;

			#ifdef _DEMO_
				if (missionToExec.playersData[i].difficulty == DIFFICULTY_HARD) {
					missionToExec.playersData[i].difficulty = DIFFICULTY_NORMAL;
				}
			#endif

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
			checkMissionDescription(pos, battleMaps);
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
	if( code == EVENT_CREATEWND ) {
		CComboWindow *pCombo = (CComboWindow*) pWnd;
		pCombo->Array.push_back( getItemTextFromBase("Exodus").c_str() );
		pCombo->Array.push_back( getItemTextFromBase("Empire").c_str() );
#if !defined(_DEMO_) && !defined(_PERIMETER_ADDON_)
        pCombo->Array.push_back( getItemTextFromBase("Harkback").c_str() );
#endif
		pCombo->size = pCombo->Array.size();
		pCombo->pos = 0;
	}
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
		pCombo->Array.push_back( getItemTextFromBase("Player").c_str() );
		pCombo->size = 1;
		pCombo->pos = 0;
	} else if ( code == EVENT_DRAWWND ) {
		int maxCount = battleMaps[((CListBoxWindow*)_shellIconManager.GetWnd(SQSH_MM_MAP_LIST))->GetCurSel()].playerAmountScenarioMax;
		for (int i = 1; i < 4; i++) {
			_shellIconManager.GetWnd(SQSH_MM_BATTLE_PLAYER1_SLOT_BTN + i)->Show(maxCount > i);
		}
		setSlotVisible(1, battleColors.getPosition(1) != -1);
		setSlotVisible(2, battleColors.getPosition(2) != -1);
		setSlotVisible(3, battleColors.getPosition(3) != -1);
	}
}

void onMMBattleSlotButton(CShellWindow* pWnd, InterfaceEventCode code, int param) {
	CComboWindow *pCombo = (CComboWindow*) pWnd;
	if( code == EVENT_CREATEWND ) {
		pCombo->Array.push_back( getItemTextFromBase("Closed").c_str() );
		pCombo->Array.push_back( getItemTextFromBase("AI (Easy)").c_str() );
		pCombo->Array.push_back( getItemTextFromBase("AI (Normal)").c_str() );
		#ifndef _DEMO_
			pCombo->Array.push_back( getItemTextFromBase("AI (Hard)").c_str() );
		#endif
		pCombo->size = pCombo->Array.size();
		pCombo->pos = 0;
	} else if ( (code == EVENT_UNPRESSED || code == EVENT_RUNPRESSED) && intfCanHandleInput() ) {
		setSlotClosed(3 - (SQSH_MM_BATTLE_PLAYER4_SLOT_BTN - pWnd->ID), pCombo->pos == 0);
	}
}
void onMMBattleClanButton(CShellWindow* pWnd, InterfaceEventCode code, int param) {
	if( code == EVENT_CREATEWND ) {
		CComboWindow *pCombo = (CComboWindow*) pWnd;
		string clan = getItemTextFromBase("Clan");
		char buff[30 + 1];
		for (int i = 0; i < NETWORK_PLAYERS_MAX; i++) {
			sprintf(buff, "%d", (i + 1));
			pCombo->Array.push_back( (clan + buff).c_str() );
		}
		pCombo->size = NETWORK_PLAYERS_MAX;
		pCombo->pos = 3 - (SQSH_MM_BATTLE_PLAYER4_CLAN_BTN - pWnd->ID);
	}
}
void onMMBattleHCButton(CShellWindow* pWnd, InterfaceEventCode code, int param) {
	if( code == EVENT_CREATEWND ) {
		CComboWindow *pCombo = (CComboWindow*) pWnd;
		pCombo->Array.push_back( "100%" );
		pCombo->Array.push_back( "70%" );
		pCombo->Array.push_back( "50%" );
		pCombo->Array.push_back( "30%" );
		pCombo->size = 4;
		pCombo->pos = 0;
	}
}


//////////////////////

