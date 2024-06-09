
#include "StdAfx.h"
#include "Config.h"
#include "Runtime.h"
#include "terra.h"
#include "GenericControls.h"
#include "Universe.h"
#include "UniverseInterface.h"
#include "GameShell.h"
#include "AudioPlayer.h"
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
#include "MainMenu.h"

extern MusicPlayer gb_Music;
extern MissionDescription missionToExec;
extern BGScene bgScene;

std::vector<MissionDescription> battleMaps;
int defaultBattleMapCount = 0;
MonoSelect battleColors;
bool flagNeedRefresh = false;

//We can only show 4 simultaneous player in UI, so we use paging to allow seeing more
int battlePlayersPage = 0;

//battle menu
void loadBattleList() {
	if (battleMaps.empty()) {
        const std::vector<const char*> paths = { "RESOURCE/BATTLE", "RESOURCE/BATTLE/SCENARIO" };
        loadMapVector(battleMaps, paths, ".spg", false);
	}
}

void setMapText() {
    bool survival = true;
    for (int i = 1, s = missionToExec.playersAmountScenarioMax(); i < s; i++) {
        if (battleColors.getPosition(i) != -1) {
            survival = false;
        }
    }

    CTextWindow* textWnd = (CTextWindow*)_shellIconManager.GetWnd(SQSH_MM_BATTLE_MAP_DESCR_TXT);
	std::string text;
    if (survival) {
        text = qdTextDB::instance().getText("Interface.Menu.Messages.Survival");
        text += "\n\n";
        text += getMapName(missionToExec.missionName().c_str());
        text += qdTextDB::instance().getText("Interface.Menu.Messages.SurvivalRecord");
        text += formatTimeWithHour(gameShell->currentSingleProfile.getRecord(missionToExec.missionName()));
    } else {
        text = qdTextDB::instance().getText("Interface.Menu.Messages.Battle");
    }
    textWnd->setText(text);
}

void setSlotClosed(int i, bool closed, bool initSlot = false) {
    int u = i % UI_PLAYERS_MAX;
	CComboWindow* pCombo = (CComboWindow*) _shellIconManager.GetWnd(SQSH_MM_BATTLE_PLAYER1_SLOT_BTN + u);
	if (closed) {
		pCombo->pos = 0;
	} else if (initSlot) {
		pCombo->pos = 1;
	}
	CColorComboWindow* pColorCombo = (CColorComboWindow*) _shellIconManager.GetWnd(SQSH_MM_BATTLE_PLAYER1_CLR_BTN + u);
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

void setupFirstSlot(CComboWindow* combo, bool player) {
    if ((player && combo->Array.size() == 1) || (!player && combo->Array.size() != 1)) {
        //Already setup
        return;
    }
    combo->Enable(!player);
    combo->Array.clear();
    if (player) {
        combo->Array.emplace_back(getItemTextFromBase("Player"));
        combo->pos = 0;
    } else {
        combo->Array.emplace_back( getItemTextFromBase("Closed") );
        combo->Array.emplace_back( getItemTextFromBase("AI (Easy)") );
        combo->Array.emplace_back( getItemTextFromBase("AI (Normal)") );
        combo->Array.emplace_back( getItemTextFromBase("AI (Hard)") );
    }
    combo->size = combo->Array.size();

    CComboWindow* clan = dynamic_cast<CComboWindow*>(_shellIconManager.GetWnd(SQSH_MM_BATTLE_PLAYER1_CLAN_BTN));
    clan->Enable(!player);
    if (player) {
        clan->pos = 0;
    }
}

///Copies from UI[u] to MD[i]
void copyToMD() {
    for (int u = 0; u < UI_PLAYERS_MAX; ++u) {
        int i = UI_PLAYERS_MAX * battlePlayersPage + u;
        if (battleColors.getPosition(i) != -1) {
            CComboWindow* combo = (CComboWindow*) _shellIconManager.GetWnd(SQSH_MM_BATTLE_PLAYER1_FRM_BTN + u);
            missionToExec.playersData[i].belligerent = SelectableBelligerents[combo->pos];

            combo = (CComboWindow*) _shellIconManager.GetWnd(SQSH_MM_BATTLE_PLAYER1_SLOT_BTN + u);
            int diff = combo->pos - 1;
            if (diff < DIFFICULTY_EASY || diff >= DIFFICULTY_MAX) {
                diff = DIFFICULTY_HARD;
            }
            missionToExec.playersData[i].difficulty = (Difficulty) diff;

            combo = (CComboWindow*) _shellIconManager.GetWnd(SQSH_MM_BATTLE_PLAYER1_CLAN_BTN + u);
            missionToExec.playersData[i].clan = combo->pos;

            combo = (CComboWindow*) _shellIconManager.GetWnd(SQSH_MM_BATTLE_PLAYER1_HC_BTN + u);
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
        }
    }
}

///Copies from MD[i] to UI[u]
void copyToUI() {
    for (int u = 0; u < UI_PLAYERS_MAX; ++u) {
        CComboWindow* combo = (CComboWindow*) _shellIconManager.GetWnd(SQSH_MM_BATTLE_PLAYER1_SLOT_BTN + u);
        int i = UI_PLAYERS_MAX * battlePlayersPage + u;
        if (i == 0) {
            //Special case for first slot for human player
            setupFirstSlot(combo, true);
            setSlotVisible(0, true);
        } else {
            if (u == 0 && battlePlayersPage != 0) {
                //Setup first slot with AI choices
                setupFirstSlot(combo, false);
            }
            
            bool playerValid = i < missionToExec.playersAmountScenarioMax();
            combo->Show(playerValid);
            if (playerValid && battleColors.getPosition(i) != -1) {
                combo->pos = missionToExec.playersData[i].difficulty + 1;
                if (0 >= combo->pos || (combo->pos - 1) >= DIFFICULTY_MAX) combo->pos = 1;
                setSlotVisible(u, true);
            } else {
                combo->pos = 0;
                setSlotVisible(u, false);
            }
        }

        CColorComboWindow* clr_combo = (CColorComboWindow*) _shellIconManager.GetWnd(SQSH_MM_BATTLE_PLAYER1_CLR_BTN + u);
        clr_combo->pos = battleColors.getPosition(i);

        combo = (CComboWindow*) _shellIconManager.GetWnd(SQSH_MM_BATTLE_PLAYER1_FRM_BTN + u);
        combo->pos = getSelectableBelligerentIndex(missionToExec.playersData[i].belligerent);

        combo = (CComboWindow*) _shellIconManager.GetWnd(SQSH_MM_BATTLE_PLAYER1_CLAN_BTN + u);
        combo->pos = missionToExec.playersData[i].clan;
        if (0 > combo->pos || combo->pos >= missionToExec.playersAmountScenarioMax()) combo->pos = i;

        combo = (CComboWindow*) _shellIconManager.GetWnd(SQSH_MM_BATTLE_PLAYER1_HC_BTN + u);
        switch (missionToExec.playersData[i].handicap) {
            default:
            case 100:
                combo->pos = 0;
                break;
            case 70:
                combo->pos = 1;
                break;
            case 50:
                combo->pos = 2;
                break;
            case 30:
                combo->pos = 3;
                break;
        }
    }
}

///Updates buttons and page text controls
void updateBattlePageControls() {
    int maxPage = (missionToExec.playersAmountScenarioMax() - 1) / UI_PLAYERS_MAX;
    
    CShellPushButton* prev_btn = ((CShellPushButton*) _shellIconManager.GetWnd(SQSH_MM_BATTLE_PLAYER_PAGE_PREV_BTN));
    prev_btn->Show(0 < battlePlayersPage);
    CShellPushButton* next_btn = ((CShellPushButton*) _shellIconManager.GetWnd(SQSH_MM_BATTLE_PLAYER_PAGE_NEXT_BTN));
    next_btn->Show(battlePlayersPage < maxPage);
    CTextWindow* textWindow = ((CTextWindow*) _shellIconManager.GetWnd(SQSH_MM_BATTLE_PLAYER_PAGE_TXT));
    textWindow->Show(0 < maxPage);
    if (maxPage) {
        std::string pagetext = std::to_string(battlePlayersPage + 1) + " / " + std::to_string(maxPage + 1);
        ((CTextWindow*) _shellIconManager.GetWnd(SQSH_MM_BATTLE_PLAYER_PAGE_TXT))->SetText(pagetext.c_str());
    }
}

///Sets selected map as missionToExec
void setCurrentMission(int pos) {
    //Set page to first and update UI, so we don't go into weird state when changing MD, no need to copy UI to MD
    if (battlePlayersPage != 0) {
        battlePlayersPage = 0;
        copyToUI();
    }
    
    int maxPlayers = 1;
    if (0 <= pos) {
        checkMissionDescription(pos, battleMaps, GT_SINGLE_PLAYER);
        missionToExec = battleMaps[pos];
        maxPlayers = missionToExec.playersAmountScenarioMax();
    }
    battleColors.resize(maxPlayers);
    for (int i = 0; i < UI_PLAYERS_MAX; i++) {
        CComboWindow* slot = dynamic_cast<CComboWindow*>(_shellIconManager.GetWnd(SQSH_MM_BATTLE_PLAYER1_SLOT_BTN + i));
        slot->Show(i < maxPlayers);
        bool closed = battleColors.getPosition(i) == -1; //Page is always 0 anyway
        setSlotClosed(i, closed);
        setSlotVisible(i, !closed);

        //Setup clan combo if amount of map doesn't match
        CComboWindow *pCombo = dynamic_cast<CComboWindow*>(_shellIconManager.GetWnd(SQSH_MM_BATTLE_PLAYER1_CLAN_BTN + i));
        if (pCombo->size != maxPlayers) {
            pCombo->Array.clear();
            std::string clan = getItemTextFromBase("Clan");
            for (int pi = 0; pi < maxPlayers; pi++) {
                pCombo->Array.emplace_back(clan + std::to_string(pi + 1));
            }
            pCombo->size = pCombo->Array.size();
            pCombo->pos = i;
        }
    }

    updateBattlePageControls();
    _shellIconManager.GetWnd(SQSH_MM_BATTLE_GO_BTN)->Enable(pos != -1);
    setMapText();
}

void setupBattleDescWnd(int index, std::vector<MissionDescription>& mVect, int mapWndID, int mapDescrWndID) {
	checkMissionDescription(index, mVect, GT_SINGLE_PLAYER);
	((CShowMapWindow*)_shellIconManager.GetWnd(mapWndID))->setWorldID( mVect[index].worldID() );
    setMapText();
}

void onBattleMenuOpening() {
	StartSpace();
	CListBoxWindow* list = (CListBoxWindow*)_shellIconManager.GetWnd(SQSH_MM_MAP_LIST);
	list->NewItem(1);
	list->Clear();
    for (int i = 0; i < battleMaps.size(); i++) {
		std::string name = getMapName(battleMaps[i].missionName().c_str());
		list->AddString(name, 0);
	}
    
    battleColors.reset(UI_PLAYERS_MAX, playerAllowedColorSize);
    battleColors.enable(0);
    CComboWindow* slot1 = (CComboWindow*) _shellIconManager.GetWnd(SQSH_MM_BATTLE_PLAYER1_SLOT_BTN);
    setupFirstSlot(slot1, true);
    setSlotClosed(1, false, true);
    setSlotClosed(2, true);
    setSlotClosed(3, true);

    if (battleMaps.empty()) {
        list->SetCurSel(-1);
        clearMapDescWnd(SQSH_MM_BATTLE_MAP, SQSH_MM_BATTLE_MAP_DESCR_TXT, -1);
    } else {
        list->SetCurSel(0);
        setupBattleDescWnd(0, battleMaps, SQSH_MM_BATTLE_MAP, SQSH_MM_BATTLE_MAP_DESCR_TXT);
    }

    flagNeedRefresh = true;
}

std::string getSurvivalFileName(const std::string& fileName) {
    //Will return empty string if file wasn't indexed
    std::string path = convert_path_content("Resource/BATTLE/SURVIVAL/" + fileName + ".spg");
    bool exists = !path.empty() && std::filesystem::exists(std::filesystem::u8path(path));
    if (exists) {
        return path;
    } else {
        fprintf(stderr, "Survival file not found for current map at: %s\n", path.c_str());
        return "";
    }
}

void startBattle(int pos, CShellWindow* pWnd) {
	int i;
	for (i = 1; i < missionToExec.playersAmountScenarioMax(); i++) {
		if (battleColors.getPosition(i) != -1) {
			break;
		}
	}
	bool survival = false;
	if (i == missionToExec.playersAmountScenarioMax()) {
        //All slots are closed
		std::string surv = getSurvivalFileName(battleMaps[pos].missionName());
		if (!surv.empty()) {
            survival = true;
			missionToExec = MissionDescription(surv.c_str());
		}
	}

    if (const Profile* profile = gameShell->currentSingleProfile.getCurrentProfile()) {
        auto player = missionToExec.getActivePlayerData();
        if (player) player->setName(profile->name);
    }
    
    //Write current page UI settings into MD
    copyToMD();
    
    //Setup all player slots
	for (i = 0; i < missionToExec.playersAmountScenarioMax(); i++) {
        if (battleColors.getPosition(i) != -1) {
            missionToExec.playersData[i].colorIndex = battleColors.getPosition(i);
            missionToExec.playersData[i].realPlayerType = i ? REAL_PLAYER_TYPE_AI : REAL_PLAYER_TYPE_PLAYER;
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
		if (0 <= pos && pos < battleMaps.size()) {
            setCurrentMission(pos);
			((CShowMapWindow*)_shellIconManager.GetWnd(SQSH_MM_BATTLE_MAP))->setWorldID( missionToExec.worldID() );
            setMapText();
		}
	} else if ( code == EVENT_DOUBLECLICK && intfCanHandleInput() ) {
		int pos = ((CListBoxWindow*)_shellIconManager.GetWnd(SQSH_MM_MAP_LIST))->GetCurSel();
        if (0 <= pos && pos < battleMaps.size()) {
            setCurrentMission(pos);
			startBattle(pos, pWnd);
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
	if ( code == EVENT_DRAWWND ) {
        //Workaround as doing this before menu switch doesn't work
        if (flagNeedRefresh) {
            flagNeedRefresh = false;
            int pos = ((CListBoxWindow*) _shellIconManager.GetWnd(SQSH_MM_MAP_LIST))->GetCurSel();
            setCurrentMission(pos);
        }
    } else if ( (code == EVENT_UNPRESSED || code == EVENT_RUNPRESSED) && intfCanHandleInput() ) {
        if (battlePlayersPage != 0) {
            int i = UI_PLAYERS_MAX * battlePlayersPage;
            CComboWindow *pCombo = (CComboWindow*) pWnd;
            setSlotClosed(i, pCombo->pos == 0);
            setSlotVisible(0, battleColors.getPosition(i) != -1);
            setMapText();
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
        int u = UI_PLAYERS_MAX - 1 - (SQSH_MM_BATTLE_PLAYER4_SLOT_BTN - pWnd->ID);
        int i = UI_PLAYERS_MAX * battlePlayersPage + u;
        setSlotClosed(i, pCombo->pos == 0);
        setSlotVisible(u, battleColors.getPosition(i) != -1);
        setMapText();
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

void setBattlePage(bool direction) {
    if (direction) {
        int maxPage = (missionToExec.playersAmountScenarioMax() - 1) / UI_PLAYERS_MAX;
        if (0 < maxPage && battlePlayersPage < maxPage) {
            copyToMD();
            battlePlayersPage = std::min(battlePlayersPage + 1, maxPage);
            copyToUI();
            updateBattlePageControls();
        }
    } else {
        if (0 < battlePlayersPage) {
            copyToMD();
            battlePlayersPage--;
            copyToUI();
            updateBattlePageControls();
        }
    }
}

void onMMBattlePageNextButton(CShellWindow* pWnd, InterfaceEventCode code, int param) {
    if ((code == EVENT_UNPRESSED || code == EVENT_RUNPRESSED) && intfCanHandleInput()) {
        setBattlePage(true);
    }
}

void onMMBattlePagePrevButton(CShellWindow* pWnd, InterfaceEventCode code, int param) {
    if ((code == EVENT_UNPRESSED || code == EVENT_RUNPRESSED) && intfCanHandleInput()) {
        setBattlePage(false);
    }
}

void onMMBattlePageText(CShellWindow* pWnd, InterfaceEventCode code, int param) {
    if (code == EVENT_UNPRESSED && intfCanHandleInput()) {
        setBattlePage(true);
    } else if (code == EVENT_RUNPRESSED && intfCanHandleInput()) {
        setBattlePage(false);
    }
}

//////////////////////

