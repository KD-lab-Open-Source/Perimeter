
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
#include "files/files.h"

#include "Silicon.h"
#include "HistoryScene.h"
#include "BGScene.h"
#include "../HT/ht.h"
#include "qd_textdb.h"

#include "Menu/MultiplayerCommon.h"
#include "MessageBox.h"
#include "BelligerentSelect.h"
#include "GameContent.h"
#include "codepages/codepages.h"

extern char _bCursorVisible;
extern char _bMenuMode;

extern MusicPlayer gb_Music;
extern LogStream fout;
/////////////////////////////////////////////////////////////

int terShowTips = 1;

int _id_on;
int _id_off;

bool menuChangingDone;

HistoryScene historyScene;
HistoryScene bwScene;
BGScene bgScene;

std::vector<MissionDescription> savedGames;
std::vector<MissionDescription> replays;
MissionDescription missionToExec;


bool intfCanHandleInput() {
	return !bgScene.isPlaying() && _shellIconManager.isDynQueueEmpty();
}

std::string getOriginalMissionName(const std::string& originalSaveName) {
	std::string res = convert_path_native(originalSaveName);
	res.erase(res.size() - 4, res.size()); 
	size_t pos = res.rfind(PATH_SEP);
	if (pos != std::string::npos) {
		res.erase(0, pos + 1);
	}
	return res;
}

std::string getItemTextFromBase(const char *keyStr) {
    std::string key("Interface.Menu.ComboItems.");
    key += keyStr;
    const char* stringFromBase = qdTextDB::instance().getText(key.c_str());
    return (*stringFromBase) ? stringFromBase : "";
//	return (*stringFromBase) ? stringFromBase : keyStr;
}

const char* getMapName(const char* keyStr) {
    std::string key("MapNames.");
    key += keyStr;
    const char* text = qdTextDB::instance().getText(key.c_str());
    return (text && *text) ? text : keyStr;
}

void processInterfaceMessage(terUniverseInterfaceMessage id, int wndIDToHide = -1) {
    terUniverseInterfaceMessage current = gameShell->currentSingleProfile.getGameResult();
    if (current & UNIVERSE_INTERFACE_MESSAGE_ACTIVE) {
		//Already is displaying a message, don't replace
        return;
    }
    current = static_cast<terUniverseInterfaceMessage>(current & UNIVERSE_INTERFACE_MESSAGE_MASK);
    
    //Set new game result if not defined previously, else use previous result
    if (current == UNIVERSE_INTERFACE_MESSAGE_GAME_RESULT_UNDEFINED) {
        current = id = static_cast<terUniverseInterfaceMessage>(id & UNIVERSE_INTERFACE_MESSAGE_MASK);
    } else {
        id = current;
    }
    gameShell->currentSingleProfile.setGameResult(static_cast<terUniverseInterfaceMessage>(id | UNIVERSE_INTERFACE_MESSAGE_ACTIVE));
    
    if (_shellIconManager.isCutSceneMode()) {
        _shellIconManager.setCutSceneMode(false, false);
    }

	bool multiPlayer = (gameShell->currentSingleProfile.getLastGameType() == UserSingleProfile::MULTIPLAYER);
	bool disableBack = gameShell->currentSingleProfile.getLastGameType() != UserSingleProfile::BATTLE && !multiPlayer;

	_shellIconManager.GetWnd( SQSH_MM_RESUME_BTN )->Enable(!disableBack);
    
    if (universe()) {
        universe()->stopPlayReel();
    }
    switch (id) {
        default:
        case UNIVERSE_INTERFACE_MESSAGE_GAME_RESULT_UNDEFINED:
            break;
        case UNIVERSE_INTERFACE_MESSAGE_GAME_VICTORY: {
            if (wndIDToHide == -1) {
                gameShell->cancelMouseLook();
            }
            if (gameShell->currentSingleProfile.getLastGameType() == UserSingleProfile::SCENARIO && (
				gameShell->currentSingleProfile.getLastMissionNumber() == 0 || historyScene.getMissionNumberToExecute() == gameShell->currentSingleProfile.getLastMissionNumber()
			)) {
                gameShell->currentSingleProfile.setLastMissionNumber(gameShell->currentSingleProfile.getLastMissionNumber() + 1);
            }
                        
            gameShell->currentSingleProfile.setLastWin(true);

            if (multiPlayer) {
                _shellIconManager.hideAllVisibleMenuScr();
            }

            gameShell->prepareForInGameMenu();

            _shellIconManager.playGameOverSound("RESOURCE\\MUSIC\\victory.ogg");

            CTextWindow *Wnd = (CTextWindow*)_shellIconManager.GetWnd( SQSH_MM_RESULT_TXT );
            CScaleResultButton *rWnd = (CScaleResultButton*)_shellIconManager.GetWnd( SQSH_RESULT_WND );
            if (gameShell->currentSingleProfile.getLastGameType() == UserSingleProfile::SURVIVAL) {
                const char* origName = gameShell->CurrentMission.originalSaveName;
                xassert(origName);
                std::string keyName;
                if (!origName) {
                    keyName = gameShell->CurrentMission.missionName();
                } else {
                    keyName = getOriginalMissionName(origName);
                }
                bool record = gameShell->currentSingleProfile.getRecord(keyName) < gameShell->gameTimer();
                if (record) {
                    gameShell->currentSingleProfile.setRecord(keyName, gameShell->gameTimer());
                }
                if (Wnd) {
                    Wnd->setText( formatTimeWithHour(gameShell->gameTimer()) );
                    Wnd->victory = record;
                }
                rWnd->victory = record;
            } else {
                if (Wnd) {
                    Wnd->SetText( qdTextDB::instance().getText("Interface.Menu.Messages.Victory") );
                    Wnd->victory = true;
                }
                rWnd->victory = true;
            }
            _shellIconManager.SwitchMenuScreens(wndIDToHide, SQSH_MM_ENDMISSION_SCR);
            
            break;
        }

        case UNIVERSE_INTERFACE_MESSAGE_GAME_DEFEAT: {
            if (wndIDToHide == -1) {
                gameShell->cancelMouseLook();
            }
			bool win = gameShell->currentSingleProfile.getLastGameType() == UserSingleProfile::SCENARIO
					&& historyScene.getMissionNumberToExecute() < gameShell->currentSingleProfile.getLastMissionNumber();
            gameShell->currentSingleProfile.setLastWin(win);

            if (multiPlayer) {
                _shellIconManager.hideAllVisibleMenuScr();
            }

            gameShell->prepareForInGameMenu();

            _shellIconManager.playGameOverSound("RESOURCE\\MUSIC\\defeat.ogg");

            CTextWindow *Wnd = (CTextWindow*)_shellIconManager.GetWnd( SQSH_MM_RESULT_TXT );
            CScaleResultButton *rWnd = (CScaleResultButton*)_shellIconManager.GetWnd( SQSH_RESULT_WND );
            if (gameShell->currentSingleProfile.getLastGameType() == UserSingleProfile::SURVIVAL) {
                const char* origName = gameShell->CurrentMission.originalSaveName;
                xassert(origName);
                std::string keyName;
                if (!origName) {
                    keyName = gameShell->CurrentMission.missionName();
                } else {
                    keyName = getOriginalMissionName(origName);
                }
                bool record = gameShell->currentSingleProfile.getRecord(keyName) < gameShell->gameTimer();
                if (record) {
                    gameShell->currentSingleProfile.setRecord(keyName, gameShell->gameTimer());
                }
                if(Wnd) {
                    Wnd->setText( formatTimeWithHour(gameShell->gameTimer()) );
                    Wnd->victory = record;
                }
                rWnd->victory = record;
            } else {
                if(Wnd) {
                    Wnd->SetText( qdTextDB::instance().getText("Interface.Menu.Messages.Mission Failed") );
                    Wnd->victory = false;
                }
                rWnd->victory = false;
            }
            _shellIconManager.SwitchMenuScreens(wndIDToHide, SQSH_MM_ENDMISSION_SCR);
            break;
        }
	}
}

int resultWNDID = -1;
terUniverseInterfaceMessage resultID = UNIVERSE_INTERFACE_MESSAGE_GAME_RESULT_UNDEFINED;

int goToResultQuant( float, float ) {
	if (!gameShell->GameActive) {
		return 0;
	} else if (menuChangingDone) {
		processInterfaceMessage(resultID, resultWNDID);
		return 0;
	}
	return 1;
}

void processInterfaceMessageLater(terUniverseInterfaceMessage id, int wndIDToHide = -1) {
	if (gameShell->currentSingleProfile.getLastGameType() == UserSingleProfile::MULTIPLAYER) {
		resultID = id;
        resultWNDID = wndIDToHide;
		_shellIconManager.AddDynamicHandler( goToResultQuant, CBCODE_QUANT );
	} else {
		processInterfaceMessage(id, wndIDToHide);
	}
}

void loadMapVector(std::vector<MissionDescription>& mapVector, const std::string& path, const std::string& mask, bool replay) {
	//fill map list
	std::string path_str = convert_path_native(path.c_str());
    path_str = string_to_lower(path_str.c_str());
	
	//Collect files and order
	std::vector<std::string> paths;
    for (const auto & entry : get_content_entries_directory(path_str)) {
        if (mask.empty() || endsWith(entry->key, mask)) {
            paths.emplace_back(entry->key);
        }
    }
    sort(paths.begin(), paths.end());
    
    //Fill map list from paths
    for (std::string& entry_path : paths) {
        MissionDescription mission;
        mission.setSaveName(entry_path.c_str());
        mission.setReelName(entry_path.c_str());
//			mission.gameType_ = replay ? MissionDescription::GT_playRellGame : MissionDescription::GT_SPGame;
        if((!replay) || isCorrectPlayReelFile(mission.playReelPath().c_str())) {
            mapVector.push_back(mission);
//			MissionDescription mission((string(path) + FindFileData.cFileName).c_str(), replay ? MissionDescription::GT_playRellGame : MissionDescription::GT_SPGame);
//			if(mission.worldID() != -1)
//				mapVector.push_back(mission);
        }
    }
}
void checkMissionDescription(int index, std::vector<MissionDescription>& mVect, GameType gameType) {
	if (mVect[index].worldID() == -1) {
        const char* filepath;
        if (gameType == GT_PLAY_RELL) {
            filepath = mVect[index].playReelPath().c_str();
        } else {
            filepath = mVect[index].savePathKey().c_str();
        }
		mVect[index] = MissionDescription(filepath, gameType);
	}
}
std::string checkMissingContent(const MissionDescription& mission) {
    std::string msg;
    std::vector<GAME_CONTENT> missingContent;
    
    if (mission.isCampaign()) {
		//Check if content for campaign matches the content of this campaign mission
		if (getGameContentCampaign() != mission.gameContent) {
			missingContent = getMissingGameContent(terGameContentAvailable, static_cast<GAME_CONTENT>(mission.gameContent.value()));
			if (missingContent.empty()) {
				//Content is available but not currently active, show message to switch it
				msg = qdTextDB::instance().getText("Interface.Menu.Messages.GameContentSwitch");
				missingContent = getGameContentEnums(mission.gameContent);
			} else {
				//Player doesn't have the required content
				msg = qdTextDB::instance().getText("Interface.Menu.Messages.GameContentMissing");
			}
		}
    } else {
		//Check if selected content is valid for save content
		missingContent = getMissingGameContent(terGameContentSelect, static_cast<GAME_CONTENT>(mission.gameContent.value()));
		if (!missingContent.empty()) {
            if (getMissingGameContent(terGameContentAvailable, static_cast<GAME_CONTENT>(mission.gameContent.value())).empty()) {
                //Content is available but not currently active, show message to switch it
                msg = qdTextDB::instance().getText("Interface.Menu.Messages.GameContentSwitch");
                missingContent = getGameContentEnums(terGameContentBase);
            } else {
                msg = qdTextDB::instance().getText("Interface.Menu.Messages.GameContentMissing");
            }
		}
	} 

    if (!missingContent.empty()) {
		msg += "\n";
        for (auto& content: missingContent) {
            msg += "\n" + getGameContentName(content);
        }
    } else if (mission.worldID() == -1) {
        //Game content is OK but we still don't have this map
        msg = qdTextDB::instance().getText("Interface.Menu.Messages.WorldMissing");
        msg += mission.worldName();
    } else if (mission.saveData.length() == 0 && mission.gameType_ != GT_MULTI_PLAYER_LOAD && (mission.savePathContent().empty() || !get_content_entry(mission.savePathContent()))) {
        //If has saveData ignore since is packed already
        //Game content is OK but we still don't have this save
        msg = qdTextDB::instance().getText("Interface.Menu.Messages.WorldMissing");
        if (mission.savePathContent().empty()) {
            msg += "Key: " + mission.savePathKey();
            msg += "\nWorld: " + mission.worldName();
        } else {
            msg += mission.savePathContent();
        }
    }
    
    return msg;
}
void setupMapDescWnd(int index, std::vector<MissionDescription>& mVect, int mapWndID, int mapDescrWndID, int inputWndID, GameType gameType) {
	checkMissionDescription(index, mVect, gameType);

    MissionDescription& mission = mVect[index];
    std::string missingContent = checkMissingContent(mission);

	if (mapWndID != -1) {
        int id = missingContent.empty() ? mission.worldID() : -2;
		((CShowMapWindow*)_shellIconManager.GetWnd(mapWndID))->setWorldID( id );
	}
	if (mapDescrWndID != -1) {
        std::string text;
        if (!missingContent.empty()) {
            text = missingContent;
        } else {
            text = mission.missionDescription();
        }
        ((CTextWindow*)_shellIconManager.GetWnd(mapDescrWndID))->setText( text );
	}
	if (inputWndID != -1) {
		CEditWindow* input = (CEditWindow*)_shellIconManager.GetWnd(inputWndID);
		input->SetText(mVect[index].missionName().c_str());
	}
}
void setupReplayDescWnd(int index, std::vector<MissionDescription>& mVect, int mapWndID, int mapDescrWndID, int inputWndID = -1) {
	checkMissionDescription(index, mVect, GT_PLAY_RELL);

    MissionDescription& mission = mVect[index];
    std::string missingContent = checkMissingContent(mission);

    int id = missingContent.empty() ? mission.worldID() : -2;
	((CShowMapWindow*)_shellIconManager.GetWnd(mapWndID))->setWorldID( id );

    std::string text;
    if (!missingContent.empty()) {
        text = missingContent;
    } else {
        text = mission.missionDescription();

        int diff = compare_versions(currentVersionNumbers, mission.version.value().c_str());
        if (0 != diff) {
            if (!text.empty()) text += "\n\n";
            text += qdTextDB::instance().getText("Interface.Menu.Messages.ReplayGameVersionDifferent");
            text += " " + mission.version.value();
        }
    }
    
	((CTextWindow*)_shellIconManager.GetWnd(mapDescrWndID))->setText( text );
	if (inputWndID != -1) {
		CEditWindow* input = (CEditWindow*)_shellIconManager.GetWnd(inputWndID);
		input->SetText(mission.missionNamePlayReelGame.c_str());
	}
}
void clearMapDescWnd(int mapWndID, int mapDescrWndID, int inputWndID) {
	if (mapWndID != -1) {
		((CShowMapWindow*)_shellIconManager.GetWnd(mapWndID))->setWorldID( -1 );
	}
	if (mapDescrWndID != -1) {
		((CTextWindow*)_shellIconManager.GetWnd(mapDescrWndID))->setText( "" );
	}
	if (inputWndID != -1) {
		CEditWindow* input = (CEditWindow*)_shellIconManager.GetWnd(inputWndID);
		input->SetText("");
	}
}
void fillList(int listID, std::vector<MissionDescription>& mVect, int mapWndID, int mapDescrWndID, int inputWndID) {
	CListBoxWindow* list = (CListBoxWindow*)_shellIconManager.GetWnd(listID);
	list->NewItem(1);
	list->Clear();
	int i;
	int s;
	for (i = 0, s = mVect.size(); i < s; i++) {
		list->AddString( mVect[i].missionName().c_str(), 0 );
	}
	if (s) {
		list->SetCurSel(0);
		setupMapDescWnd(0, mVect, mapWndID, mapDescrWndID, inputWndID);
	} else {
		list->SetCurSel(-1);
		clearMapDescWnd(mapWndID, mapDescrWndID, inputWndID);
	}
}
void fillReplayList(int listID, std::vector<MissionDescription>& mVect, int mapWndID, int mapDescrWndID, int inputWndID = -1) {
	CListBoxWindow* list = (CListBoxWindow*)_shellIconManager.GetWnd(listID);
	list->NewItem(1);
	list->Clear();
	int i;
	int s;
	for (i = 0, s = mVect.size(); i < s; i++) {
		list->AddString( mVect[i].missionNamePlayReelGame.c_str(), 0 );
	}
	if (s) {
		list->SetCurSel(0);
		setupReplayDescWnd(0, mVect, mapWndID, mapDescrWndID, inputWndID);
	} else {
		list->SetCurSel(-1);
		clearMapDescWnd(mapWndID, mapDescrWndID, inputWndID);
	}
}
void hideTables() {
	_shellIconManager.GetWnd(SQSH_MM_STATS_TOTAL_HEAD_LIST)->Show(0);
	_shellIconManager.GetWnd(SQSH_MM_STATS_TOTAL_LIST)->Show(0);

	_shellIconManager.GetWnd(SQSH_MM_STATS_UNITS_HEAD_LIST)->Show(0);
	_shellIconManager.GetWnd(SQSH_MM_STATS_UNITS_LIST)->Show(0);

	_shellIconManager.GetWnd(SQSH_MM_STATS_BUILDINGS_HEAD_LIST)->Show(0);
	_shellIconManager.GetWnd(SQSH_MM_STATS_BUILDINGS_LIST)->Show(0);

	_shellIconManager.GetWnd(SQSH_MM_STATS_GENERAL_HEAD_LIST)->Show(0);
	_shellIconManager.GetWnd(SQSH_MM_STATS_GENERAL_LIST)->Show(0);

	_shellIconManager.GetWnd(SQSH_MM_STATS_TOTAL_RAMKA)->Show(1);
	_shellIconManager.GetWnd(SQSH_MM_STATS_GENERAL_RAMKA)->Show(1);
	_shellIconManager.GetWnd(SQSH_MM_STATS_UNITS_RAMKA)->Show(1);
	_shellIconManager.GetWnd(SQSH_MM_STATS_BUILDINGS_RAMKA)->Show(1);
}
void prepareStatsListWindow(int listID, int columnCount, float* weights) {
	CStatListBoxWindow* list = (CStatListBoxWindow*)_shellIconManager.GetWnd(listID);
	list->NewItem(columnCount);
	list->Clear();
	for (int i = 0; i < columnCount; i++) {
		list->GetItem(i).x = weights[i];
	}
}
void prepareHeadList(int listID, const std::vector<std::string>& mVect, float* weights) {
	CListBoxWindow* list = (CListBoxWindow*)_shellIconManager.GetWnd(listID);
	list->NewItem(mVect.size());
	list->Clear();
	for (int i = 0, s = mVect.size(); i < s; i++) {
		list->GetItem(i).x = weights[i];
		list->AddString( mVect[i].c_str(), i );
	}
	list->SetCurSel(0);
}

void fillRowStatsListWindow(int listID, int rowNumber, const std::vector<std::string>& mVect, int race, const sColor4c& clr) {
	CStatListBoxWindow* list = (CStatListBoxWindow*)_shellIconManager.GetWnd(listID);
	list->AddRace( race, clr );
	for (int i = 0, s = mVect.size(); i < s; i++) {
		list->AddString( mVect[i].c_str(), i + 1 );
	}
}

void fillStatsLists() {
	char buffer[30 + 1];

	std::vector<std::string> temp;

	temp.push_back("");
	temp.push_back(qdTextDB::instance().getText("Interface.Menu.Stats.Name"));
	temp.push_back(qdTextDB::instance().getText("Interface.Menu.Stats.General"));
	temp.push_back(qdTextDB::instance().getText("Interface.Menu.Stats.Units"));
	temp.push_back(qdTextDB::instance().getText("Interface.Menu.Stats.Buildings"));
	temp.push_back(qdTextDB::instance().getText("Interface.Menu.Stats.TotalScore"));
	prepareHeadList(SQSH_MM_STATS_TOTAL_HEAD_LIST, temp, totalColumnStatsWeights);

	temp.clear();
	temp.push_back("");
	temp.push_back(qdTextDB::instance().getText("Interface.Menu.Stats.Name"));
	temp.push_back(qdTextDB::instance().getText("Interface.Menu.Stats.LeveledArea"));
	temp.push_back(qdTextDB::instance().getText("Interface.Menu.Stats.CoverEff"));
	temp.push_back(qdTextDB::instance().getText("Interface.Menu.Stats.Scourge"));
	temp.push_back(qdTextDB::instance().getText("Interface.Menu.Stats.Energy"));
	temp.push_back(qdTextDB::instance().getText("Interface.Menu.Stats.Total"));
	prepareHeadList(SQSH_MM_STATS_GENERAL_HEAD_LIST, temp, generalColumnStatsWeights);

	temp.clear();
	temp.push_back("");
	temp.push_back(qdTextDB::instance().getText("Interface.Menu.Stats.Name"));
	temp.push_back(qdTextDB::instance().getText("Interface.Menu.Stats.UnitsProduced"));
	temp.push_back(qdTextDB::instance().getText("Interface.Menu.Stats.UnitsKilled"));
	temp.push_back(qdTextDB::instance().getText("Interface.Menu.Stats.UnitsLost"));
	temp.push_back(qdTextDB::instance().getText("Interface.Menu.Stats.Total"));
	prepareHeadList(SQSH_MM_STATS_UNITS_HEAD_LIST, temp, unitsColumnStatsWeights);

	temp.clear();
	temp.push_back("");
	temp.push_back(qdTextDB::instance().getText("Interface.Menu.Stats.Name"));
	temp.push_back(qdTextDB::instance().getText("Interface.Menu.Stats.BuildingsBuilt"));
	temp.push_back(qdTextDB::instance().getText("Interface.Menu.Stats.BuildingsRazed"));
	temp.push_back(qdTextDB::instance().getText("Interface.Menu.Stats.BuildingsLost"));
	temp.push_back(qdTextDB::instance().getText("Interface.Menu.Stats.Total"));
	prepareHeadList(SQSH_MM_STATS_BUILDINGS_HEAD_LIST, temp, buildingsColumnStatsWeights);

	prepareStatsListWindow(SQSH_MM_STATS_TOTAL_LIST, 6, totalColumnStatsWeights);
	prepareStatsListWindow(SQSH_MM_STATS_GENERAL_LIST, 7, generalColumnStatsWeights);
	prepareStatsListWindow(SQSH_MM_STATS_UNITS_LIST, 6, unitsColumnStatsWeights);
	prepareStatsListWindow(SQSH_MM_STATS_BUILDINGS_LIST, 6, buildingsColumnStatsWeights);

	std::vector<terPlayer*>& players = universe()->Players;
	terPlayer* player;
	for (int i = 0; i < gameShell->CurrentMission.playerAmountScenarioMax; i++) {
        if (i >= 9) break; //TODO add paging to stats screen to handle more than 9 players
		int playerID = gameShell->CurrentMission.playersData[i].playerID;
		if (( gameShell->CurrentMission.playersData[i].realPlayerType == REAL_PLAYER_TYPE_PLAYER
              || gameShell->CurrentMission.playersData[i].realPlayerType == REAL_PLAYER_TYPE_AI
              || gameShell->CurrentMission.playersData[i].realPlayerType == REAL_PLAYER_TYPE_PLAYER_AI 
            )
			&&	playerID >= 0
			&&	playerID < universe()->Players.size()
			&& !(player = universe()->findPlayer(playerID))->isWorld()) {

//			terPlayer* player = universe()->findPlayer(playerID);
			const PlayerStats& stats = player->getStats();
			sColor4c clr = sColor4c(player->unitColor());
            BELLIGERENT_FACTION faction = getBelligerentFaction(player->belligerent());
			int race;
			switch (faction) {
                case EXODUS:
					race = 1;
					break;
				case HARKBACK:
					race = 2;
					break;
				case EMPIRE:
				default:
					race = 0;
			}

            time_type time = std::max(10000U, gameShell->gameTimer());

			temp.clear();
			temp.push_back(gameShell->CurrentMission.playersData[i].name());

			sprintf(buffer, "%d", stats.getGeneralTotalScore(time));
			temp.push_back(buffer);
			sprintf(buffer, "%d", stats.getUnitsTotalScore());
			temp.push_back(buffer);
			sprintf(buffer, "%d", stats.getBuildingsTotalScore());
			temp.push_back(buffer);
			sprintf(buffer, "%d", stats.getTotalScore(time));
			temp.push_back(buffer);

			fillRowStatsListWindow(SQSH_MM_STATS_TOTAL_LIST, i, temp, race, clr);

			temp.clear();
			temp.push_back(gameShell->CurrentMission.playersData[i].name());

			sprintf(buffer, "%d", stats.getMaxLeveledArea());
			temp.push_back(buffer);
			sprintf(buffer, "%d%%", xm::round(stats.getMedEfficiency() * 100));
			temp.push_back(buffer);
			sprintf(buffer, "%d", stats.getScourgeKilled());
			temp.push_back(buffer);
			sprintf(buffer, "%d", xm::round(stats.getEnergy()));
			temp.push_back(buffer);
			sprintf(buffer, "%d", stats.getGeneralTotalScore(time));
			temp.push_back(buffer);

			fillRowStatsListWindow(SQSH_MM_STATS_GENERAL_LIST, i, temp, race, clr);

			temp.clear();
			temp.push_back(gameShell->CurrentMission.playersData[i].name());

			sprintf(buffer, "%d", stats.getUnitCount());
			temp.push_back(buffer);
			sprintf(buffer, "%d", stats.getUnitKilled());
			temp.push_back(buffer);
			sprintf(buffer, "%d", stats.getUnitLost());
			temp.push_back(buffer);
			sprintf(buffer, "%d", stats.getUnitsTotalScore());
			temp.push_back(buffer);

			fillRowStatsListWindow(SQSH_MM_STATS_UNITS_LIST, i, temp, race, clr);

			temp.clear();
			temp.push_back(gameShell->CurrentMission.playersData[i].name());

			sprintf(buffer, "%d", stats.getBuildingCount());
			temp.push_back(buffer);
			sprintf(buffer, "%d", stats.getBuildingRazed());
			temp.push_back(buffer);
			sprintf(buffer, "%d", stats.getBuildingLost());
			temp.push_back(buffer);
			sprintf(buffer, "%d", stats.getBuildingsTotalScore());
			temp.push_back(buffer);

			fillRowStatsListWindow(SQSH_MM_STATS_BUILDINGS_LIST, i, temp, race, clr);
		}
	}
}

void StartSpace() {
	if (!bgScene.inited()) {
		bgScene.init(terVisGeneric);
	}
	bgScene.setSkinColor();
	if (!bwScene.ready()) {
		bwScene.init(terVisGeneric, true, false);
		bwScene.getController()->goToYear(300);
	}
}

void hideAuxBriefingButtons() {
	_shellIconManager.GetWnd(SQSH_MM_CONTINUE_BRIEFING_BTN)->Show(0);
	_shellIconManager.GetWnd(SQSH_MM_CONTINUE_BRIEFING_BORDER)->Show(0);
	_shellIconManager.GetWnd(SQSH_MM_SKIP_MISSION_BTN)->Show(0);
	_shellIconManager.GetWnd(SQSH_MM_SKIP_MISSION_BORDER)->Show(0);
	_shellIconManager.GetWnd(SQSH_MM_START_MISSION_BTN)->Show(0);
	_shellIconManager.GetWnd(SQSH_MM_START_BRIEFING_BORDER)->Show(0);
}

/* TODO unused?
void showStartMissionButton() {
	CShellWindow* wnd = _shellIconManager.GetWnd(SQSH_MM_START_MISSION_BTN);
	wnd->Show(1);
	_shellIconManager.Effect(effectButtonsFadeIn, wnd); //запустить разлет
//	bgScene.markToPlay(wnd->m_attr->bgObject.name, wnd->m_attr->bgObject.chainName, true);

	_shellIconManager.GetWnd(SQSH_MM_START_BRIEFING_BORDER)->Show(1);
	_shellIconManager.Effect(effectButtonsFadeIn, _shellIconManager.GetWnd(SQSH_MM_START_BRIEFING_BORDER)); //запустить разлет
}
*/

int setupMenuCursorQuant( float, float ) {
    if (menuChangingDone) {
        //int id = _shellIconManager.getVisibleMenuScr();
        if (_bMenuMode) {
            _shellCursorManager.SetActiveCursor(CShellCursorManager::arrow, 1);
        }
        return 0;
    }
    return 1;
}

int SwitchMenuScreenQuant2( float, float ) {
	if(!_shellIconManager.IsEffect()){
//		fout < "SwitchMenuScreenQuant2 " < _id_off < " " < _id_on < "\n";
		//fadeIn done
//		if(_id_on<0) return 0;
		if (_id_on >= 0) {
			_shellIconManager.SetFocus( _id_on );
			_shellIconManager.UnEffectControls( _shellIconManager.GetWnd(_id_on), effectButtonsFadeIn ); //запустить слет
//			_shellIconManager.GetWnd( _id_on )->ClearEffect();
			switch (_id_on) {
				case SQSH_MM_BRIEFING_SCR:
					if (_id_off < 0) {
						historyScene.getController()->eventOccured(Controller::MISSION_VICTORY_EVENT);
					}
					historyScene.start();
					break;
				case SQSH_MM_LOADING_MISSION_SCR:
					{
	//					if (_id_off == SQSH_MM_MULTIPLAYER_LOBBY_SCR) {
	//						gameShell->getNetClient()->StartLoadTheGame();
	//					} else {
	//						_shellIconManager.ClearQueue();
							switch (gameShell->currentSingleProfile.getLastGameType()) {
								case UserSingleProfile::SCENARIO:
                                    missionToExec.missionNumber = historyScene.getMissionNumberToExecute();
									break;
								case UserSingleProfile::SURVIVAL:
									missionToExec.missionNumber = -2;
									break;
                                case UserSingleProfile::REPLAY:
                                    //Already have mission number builtin
                                    break;
								default:
									missionToExec.missionNumber = -1;
                                    break;
							}
                            HTManager::instance()->GameStart(missionToExec);
                            gameShell->currentSingleProfile.setCurrentMissionNumber(missionToExec.missionNumber);
							gameShell->currentSingleProfile.setGameResult(UNIVERSE_INTERFACE_MESSAGE_GAME_RESULT_UNDEFINED);
	//					}
						if (_id_off == SQSH_MM_BRIEFING_SCR) {
							historyScene.done();
							bgScene.done();
						} else {
							bwScene.done();
							bgScene.done();
						}
					}
					break;
			}
		} else {
			switch (_id_on) {
				case SKIP_MISSION: {
                    historyScene.getController()->eventOccured(Controller::MISSION_VICTORY_EVENT);
                    break;
                }
			}
		}
		menuChangingDone = true;
		return 0;
	}
	return 1;
}

int SwitchMenuBGQuant2( float, float ) {
	//!!!
	if (!bgScene.isPlaying() && !_shellIconManager.IsEffect()) {
		if (_id_on >= 0) {
			_shellIconManager.GetWnd(_id_on)->Show( 1 );
			if (_id_on == SQSH_MM_SUBMIT_DIALOG_SCR) {
				prepareMessageBoxToShow();
			}
			_shellIconManager.UnEffectControls(_shellIconManager.GetWnd(_id_on), effectCtrlRollOut); //запустить слет
			_shellIconManager.Effect(effectButtonsFadeIn, _shellIconManager.GetWnd(_id_on)); //запустить слет
			_shellIconManager.AddDynamicHandler(SwitchMenuScreenQuant2, CBCODE_QUANT); //ждать пока не слетится
			switch (_id_on) {
                case SQSH_MM_START_SCR:
                    //Only enable if user didn't choose a specific content
                    _shellIconManager.GetWnd(SQSH_MM_MULTIPLAYER_BTN)->Enable(terGameContentAvailable == terGameContentSelect);
                    break;
				case SQSH_MM_SINGLE_SCR:
                    //Only enable if user didn't choose a specific content
                    _shellIconManager.GetWnd(SQSH_MM_BATTLE_BTN)->Enable(terGameContentAvailable == terGameContentSelect);
					if (!debug_allow_replay) {
						_shellIconManager.GetWnd(SQSH_MM_REPLAY_LINE)->Show(0);
						_shellIconManager.GetWnd(SQSH_MM_REPLAY_BORDER)->Show(0);
						_shellIconManager.GetWnd(SQSH_MM_REPLAY_BTN)->Show(0);
					}
					break;
				case SQSH_MM_BRIEFING_SCR:
					{
						hideAuxBriefingButtons();
						_shellIconManager.GetWnd(SQSH_MM_SKIP_BRIEFING_BTN)->Show(1);
						_shellIconManager.GetWnd(SQSH_MM_SKIP_BRIEFING_BORDER)->Show(1);
					}
					break;
				case SQSH_MM_ENDMISSION_SCR:
					{
						bool disableBack = (gameShell->currentSingleProfile.getLastGameType() != UserSingleProfile::BATTLE || (gameShell->currentSingleProfile.getGameResult() & UNIVERSE_INTERFACE_MESSAGE_GAME_DEFEAT))
                                        && gameShell->currentSingleProfile.getLastGameType() != UserSingleProfile::MULTIPLAYER;
						_shellIconManager.GetWnd( SQSH_MM_RESUME_BTN )->Show(!disableBack);
						_shellIconManager.GetWnd( SQSH_MM_RESUME_BORDER )->Show(!disableBack);
					}
					break;
				case SQSH_MM_STATS_SCR:
					{
						if (!debug_allow_replay) {
							_shellIconManager.GetWnd(SQSH_MM_SAVE_REPLAY_BORDER)->Show(0);
							_shellIconManager.GetWnd(SQSH_MM_SAVE_REPLAY_BTN)->Show(0);
						}
                        bool showCont = gameShell->currentSingleProfile.getLastGameType() == UserSingleProfile::SCENARIO
                                     && gameShell->currentSingleProfile.isLastWin();

						_shellIconManager.GetWnd(SQSH_MM_CONTINUE_FROM_STATS_BTN)->Show(showCont);
						_shellIconManager.GetWnd(SQSH_MM_CONTINUE_FROM_STATS_BORDER)->Show(showCont);

						_shellIconManager.GetWnd(SQSH_MM_RESTART_BTN)->Enable(
								gameShell->currentSingleProfile.getLastGameType() != UserSingleProfile::MULTIPLAYER );

						CShellWindow* replayBtn = _shellIconManager.GetWnd(SQSH_MM_SAVE_REPLAY_BTN);
						if (replayBtn) {
							replayBtn->Enable( gameShell->currentSingleProfile.getLastGameType() != UserSingleProfile::REPLAY );
						}

						hideTables();
						_shellIconManager.GetWnd(SQSH_MM_STATS_TOTAL_RAMKA)->Show(0);
						_shellIconManager.GetWnd(SQSH_MM_STATS_TOTAL_HEAD_LIST)->Show(1);
						_shellIconManager.GetWnd(SQSH_MM_STATS_TOTAL_LIST)->Show(1);
						fillStatsLists();
					}
					break;
			}
		} else {
			switch (_id_on) {
				case SKIP_MISSION:
					{
						//continue briefing after skip mission
						CShellWindow* wnd = _shellIconManager.GetWnd(SQSH_MM_SKIP_BRIEFING_BTN);
						wnd->Show(1);
						_shellIconManager.Effect(effectButtonsFadeIn, wnd); //запустить разлет
						wnd = _shellIconManager.GetWnd(SQSH_MM_SKIP_BRIEFING_BORDER);
						wnd->Show(1);
						_shellIconManager.Effect(effectButtonsFadeIn, wnd); //запустить разлет
						_shellIconManager.AddDynamicHandler(SwitchMenuScreenQuant2, CBCODE_QUANT); //ждать пока не слетится
					}
					break;
				case SHOW_START_SKIP_MISSION:
					{
						CShellWindow* wnd = _shellIconManager.GetWnd(SQSH_MM_SKIP_MISSION_BTN);
						wnd->Show(1);
						_shellIconManager.Effect(effectButtonsFadeIn, wnd); //запустить разлет
						wnd = _shellIconManager.GetWnd(SQSH_MM_SKIP_MISSION_BORDER);
						wnd->Show(1);
						_shellIconManager.Effect(effectButtonsFadeIn, wnd); //запустить разлет
					}
				case SHOW_START_HIDE_SKIP_MISSION:
				case SHOW_START_MISSION:
					{
						//show start mission button
						CShellWindow* wnd = _shellIconManager.GetWnd(SQSH_MM_START_MISSION_BTN);
						wnd->Show(1);
						_shellIconManager.Effect(effectButtonsFadeIn, wnd); //запустить разлет
						wnd = _shellIconManager.GetWnd(SQSH_MM_START_BRIEFING_BORDER);
						wnd->Show(1);
						_shellIconManager.Effect(effectButtonsFadeIn, wnd); //запустить разлет
						menuChangingDone = true;
					}
					break;
				case SHOW_SKIP_CONTINUE:
					{
						CShellWindow* wnd = _shellIconManager.GetWnd(SQSH_MM_SKIP_BRIEFING_BTN);
						wnd->Show(1);
						_shellIconManager.Effect(effectButtonsFadeIn, wnd); //запустить разлет
						wnd = _shellIconManager.GetWnd(SQSH_MM_SKIP_BRIEFING_BORDER);
						wnd->Show(1);
						_shellIconManager.Effect(effectButtonsFadeIn, wnd); //запустить разлет
					}
				case SHOW_CONTINUE:
					{
						//show continue button
						CShellWindow* wnd = _shellIconManager.GetWnd(SQSH_MM_CONTINUE_BRIEFING_BTN);
						wnd->Show(1);
						_shellIconManager.Effect(effectButtonsFadeIn, wnd); //запустить разлет
						wnd = _shellIconManager.GetWnd(SQSH_MM_CONTINUE_BRIEFING_BORDER);
						wnd->Show(1);
						_shellIconManager.Effect(effectButtonsFadeIn, wnd); //запустить разлет
						menuChangingDone = true;
					}
					break;
			}
		}
		return 0;
	}
	return 1;
}
int SwitchMenuScreenQuant1( float, float ) {
	static int bPlayFly=0;

	//!!!
	if (!bgScene.isPlaying() && !_shellIconManager.IsEffect()) {
		if (_id_off >= 0) {
			_shellIconManager.GetWnd(_id_off)->Show( 0 );
			if (_id_off == SQSH_MM_SUBMIT_DIALOG_SCR) {
				_shellIconManager.SetModalWnd(0);
				menuChangingDone = true;
				if (_id_on == FAKE) {
					//resume game
					if (_shellIconManager.isInGameGroup() && !_shellIconManager.menuVisible()) {
						gameShell->resumeGame(true);
						_shellCursorManager.m_bShowSideArrows=1;
						_shellCursorManager.ShowCursor();
						_bMenuMode = 0;
					}
				}
				return 0;
			} else {
				if (_id_off == SQSH_MM_MULTIPLAYER_LOBBY_SCR && _id_on != SQSH_MM_LOADING_MISSION_SCR) {
					gameShell->getNetClient()->FinishGame();
/*
					if (gameShell->getNetClient()->isHost()) {
						gameShell->getNetClient()->StopServerAndStartFindHost();
					} else {
						gameShell->getNetClient()->DisconnectAndStartFindHost();
					}
*/
				}
			}
		}
		_shellIconManager.AddDynamicHandler(SwitchMenuBGQuant2, CBCODE_QUANT); //ждать пока не слетится BG
		if (_id_on >= 0) {
            if (_id_on == _shellIconManager.initialMenu) {
                PlayMusic( mainMenuMusic );

                CShellWindow* pWnd = _shellIconManager.GetWnd(SQSH_MM_SPLASH4);
                if(pWnd && (pWnd->state & SQSH_VISIBLE)) {
                    pWnd->Show(0);
                }

                loadBattleList();
                loadMultiplayerList();

                //Needed to initialize bgScene
                StartSpace();

//				_shellCursorManager.SetActiveCursor(CShellCursorManager::arrow, 1);	
            }
			if (!gb_Music.IsPlay() && !_shellIconManager.isInGameGroup()) {
				PlayMusic( mainMenuMusic );
			}
			switch (_id_on) {
				case SQSH_MM_START_SCR:
                    if (gameShell->getNetClient()) {
                        gameShell->destroyNetClient();
                    }
                    //Remove last game type Multiplayer if set
                    if (gameShell->currentSingleProfile.getLastGameType() == UserSingleProfile::MULTIPLAYER) {
                        gameShell->currentSingleProfile.setLastGameType(UserSingleProfile::UNDEFINED);
                    }
                    historyScene.stop();
                    StartSpace();
                    historyScene.done();
					break;
				case SQSH_MM_SINGLE_SCR:
                    historyScene.stop();
                    StartSpace();
                    historyScene.done();
					break;
                case SQSH_MM_CONTENT_CHOOSER_SCR:
                    fillContentChooserList();
                    break;
                case SQSH_MM_ADDONS_SCR:
                    loadAddonsList();
                    break;
				case SQSH_MM_PROFILE_SCR:
					{
						fillProfileList();
						CEditWindow* input = (CEditWindow*)_shellIconManager.GetWnd(SQSH_MM_PROFILE_NAME_INPUT);
						if (input->isEmptyText()) {
							input->SetText(qdTextDB::instance().getText("Interface.Menu.EmptyName.NewPlayer"));
						}

						historyScene.stop();
						StartSpace();
						historyScene.done();
					}
					break;
				case SQSH_MM_BRIEFING_SCR:
					{
//						PlayMusic( briefingMusic );
						_shellCursorManager.m_bShowSideArrows = 1;

						if (!bgScene.inited()) {
							bgScene.init(terVisGeneric);
						}
						Frame* frame = historyScene.getNomadFrame();
						bgScene.setSkinColor(frame ? sColor4f(playerColors[frame->getColorIndex()].unitColor) : sColor4f(1, 1, 1, 1));
						historyScene.init(terVisGeneric, false, HISTORY_ADD_BLEND_ALPHA_MODE);
						historyScene.playMusic();
						bwScene.done();

//						_shellCursorManager.SetActiveCursor(CShellCursorManager::arrow, 1);	
					}
					break;
				case SQSH_MM_SCENARIO_SCR:
					{
						//fill mission list
						PlayMusic( mainMenuMusic );
						CComboWindow* diffCombo = (CComboWindow*)_shellIconManager.GetWnd(SQSH_MM_DIFFICULTY_COMBO);
						diffCombo->pos = gameShell->currentSingleProfile.getDifficulty();
						CListBoxWindow* list = (CListBoxWindow*)_shellIconManager.GetWnd(SQSH_MM_MISSION_LIST);
						list->NewItem(1);
						list->Clear();

                        int lastWinnedMissionNumber = gameShell->currentSingleProfile.getLastMissionNumber();
                        if (lastWinnedMissionNumber >= historyScene.missionCount()) {
                            lastWinnedMissionNumber = historyScene.missionCount() - 1;
                        }

						for (int i = 0; i <= lastWinnedMissionNumber; i++) {
							const char* stringFromBase = qdTextDB::instance().getText(historyScene.getMission(i).name.c_str());
							list->AddString( (*stringFromBase) ? stringFromBase : historyScene.getMission(i).name.c_str(), 0 );
						}
						if (list->GetItemCount() > 2) {
							list->SetCurSel(list->GetItemCount() - 1);
						} else {
							list->SetCurSel(0);
						}
						historyScene.getController()->setNormalSpeedMode(true);
						historyScene.stop();
						StartSpace();
						historyScene.done();
					}
					break;
				case SQSH_MM_BATTLE_SCR:
					{
						onBattleMenuOpening();
					}
					break;
				case SQSH_MM_LOAD_SCR:
					{
						const std::string& savesDir = gameShell->currentSingleProfile.getSavesDirectory();
                        savedGames.clear();
						loadMapVector(savedGames, savesDir, ".spg");
						StartSpace();
						fillList(SQSH_MM_LOAD_MAP_LIST, savedGames, SQSH_MM_LOAD_MAP, SQSH_MM_LOAD_MAP_DESCR_TXT);
					}
					break;
				case SQSH_MM_LOAD_REPLAY_SCR:
					{
                        replays.clear();
						loadMapVector(replays, REPLAY_PATH, "", true);
						StartSpace();
						fillReplayList(SQSH_MM_LOAD_REPLAY_LIST, replays, SQSH_MM_LOAD_REPLAY_MAP, SQSH_MM_LOAD_REPLAY_DESCR_TXT);
					}
					break;
				case SQSH_MM_SAVE_REPLAY_SCR:
					{
                        replays.clear();
						loadMapVector(replays, REPLAY_PATH, "", true);
//						StartSpace();
						CEditWindow* input = (CEditWindow*)_shellIconManager.GetWnd(SQSH_MM_REPLAY_NAME_INPUT);
//						if (input->isEmptyText()) {
							input->SetText(gameShell->CurrentMission.worldName().c_str());
//						}
						fillReplayList(SQSH_MM_SAVE_REPLAY_LIST, replays, SQSH_MM_SAVE_REPLAY_MAP, SQSH_MM_SAVE_REPLAY_DESCR_TXT);
					}
					break;
				case SQSH_MM_LOAD_IN_GAME_SCR:
					{
                        savedGames.clear();
						const std::string& savesDir = gameShell->currentSingleProfile.getSavesDirectory();
						loadMapVector(savedGames, savesDir, ".spg");
//						StartSpace();
						fillList(SQSH_MM_LOAD_IN_GAME_MAP_LIST, savedGames, SQSH_MM_LOAD_IN_GAME_MAP, SQSH_MM_LOAD_IN_GAME_MAP_DESCR_TXT);
					}
					break;
				case SQSH_MM_SAVE_GAME_SCR:
					{
                        savedGames.clear();
                        const std::string& savesDir = gameShell->currentSingleProfile.getSavesDirectory();
                        loadMapVector(savedGames, savesDir, ".spg");
						CEditWindow* input = (CEditWindow*)_shellIconManager.GetWnd(SQSH_MM_SAVE_NAME_INPUT);
                        input->SetText(gameShell->CurrentMission.worldName().c_str());

//						StartSpace();
						fillList(SQSH_MM_SAVE_GAME_MAP_LIST, savedGames, SQSH_MM_SAVE_GAME_MAP, SQSH_MM_SAVE_GAME_MAP_DESCR_TXT);
					}
					break;
				case SQSH_MM_LOADING_MISSION_SCR:
					{
                        if (!bgScene.inited()) {
                            //bgScene.init(terVisGeneric);
                            StartSpace();
                        }

						CTextWindow* txtWnd = (CTextWindow*)_shellIconManager.GetWnd(SQSH_MM_MISSION_DESCR_TXT);

                        switch (missionToExec.gameType_) {
                            case GT_MULTI_PLAYER_RESTORE_PARTIAL:
                            case GT_MULTI_PLAYER_RESTORE_FULL:
                                txtWnd->setText( qdTextDB::instance().getText("Interface.Menu.Messages.Multiplayer.Nonsinchronization") );
                                break;
                            default:
                                switch(gameShell->currentSingleProfile.getLastGameType()) {
                                    case UserSingleProfile::BATTLE:
                                    case UserSingleProfile::MULTIPLAYER:
                                        txtWnd->setText( qdTextDB::instance().getText("Interface.Menu.Messages.Battle") );
                                        break;
                                    case UserSingleProfile::SURVIVAL:
                                        txtWnd->setText( qdTextDB::instance().getText("Interface.Menu.Messages.Survival") );
                                        break;
                                    default:
                                        txtWnd->SetText(missionToExec.missionDescription().c_str());
                                }
                                break;
                        }

						
						CShowMapWindow* mapWnd = (CShowMapWindow*)_shellIconManager.GetWnd(SQSH_MM_MAPWINDOW);
						mapWnd->setWorldID(missionToExec.worldID());

						CLogoWindow* logoWnd = (CLogoWindow*)_shellIconManager.GetWnd(SQSH_MM_LOADING_NOMAD_ICON);
						txtWnd = (CTextWindow*)_shellIconManager.GetWnd(SQSH_MM_LOADING_NOMAD_TXT);

						Frame* frame = historyScene.getNomadFrame();
                        auto player = missionToExec.getActivePlayerData();
						if (gameShell->currentSingleProfile.getLastGameType() == UserSingleProfile::SCENARIO && frame) {
							txtWnd->setText(HistoryScene::getFrameNameFromBase(frame->getName()));
							txtWnd->colorIndex = frame->getColorIndex();
							logoWnd->setRace(frame->getRace());
						} else {
						    BELLIGERENT_FACTION faction = getBelligerentFaction(player.belligerent);
							switch (faction) {
								case EXODUS:
									logoWnd->setRace(1);
									break;
								case HARKBACK:
									logoWnd->setRace(2);
									break;
								case EMPIRE:
								default:
									logoWnd->setRace(0);
							}
							txtWnd->setText(player.name());
							txtWnd->colorIndex = player.colorIndex;
						}
                        bgScene.setSkinColor(sColor4f(playerColors[txtWnd->colorIndex].unitColor));
					}
					break;
                case SQSH_MM_MULTIPLAYER_LIST_SCR:
                    {                        
                        std::string name = getStringSettings(regLanName);
                        if (name.empty() && gameShell->currentSingleProfile.getCurrentProfileIndex() != -1) {
                            name = gameShell->currentSingleProfile.getCurrentProfile().name;
                        }
                        CEditWindow* input = (CEditWindow*)_shellIconManager.GetWnd(SQSH_MM_MULTIPLAYER_NAME_INPUT);
                        if (!name.empty()) {
                            input->SetText(name.c_str());
                        } else {
                            input->SetText(qdTextDB::instance().getText("Interface.Menu.EmptyName.NewPlayer"));
                        }
                        
                        gameShell->createNetClient();
                        gameShell->getNetClient()->StartFindHost();
                        
                        historyScene.stop();
                        StartSpace();
                        historyScene.done();
                    }
                    break;
                case SQSH_MM_MULTIPLAYER_LOBBY_SCR:
                    {
                        setupMultiplayerLobby();
                    }
                    break;
                case SQSH_MM_MULTIPLAYER_HOST_SCR:
                    {
                        CEditWindow* playerNameInput = (CEditWindow*)_shellIconManager.GetWnd(SQSH_MM_MULTIPLAYER_NAME_INPUT);
                        std::string name = getStringSettings("HostName");
                        if (name.empty()) {
                            //Get player name and add Server
                            name = playerNameInput->getText();
                            if (name.empty()) name = getStringSettings(regLanName);
                            const char* server = qdTextDB::instance().getText("Interface.Menu.Multiplayer.Server");
                            if (name.empty()) {
                                name = "Game";
                            }
                            name = name + " " + server;
                        }

                        CEditWindow* hostNameInput = (CEditWindow*)_shellIconManager.GetWnd(SQSH_MM_MULTIPLAYER_HOST_NAME_INPUT);
                        hostNameInput->SetText(name.c_str());

                        std::string text = getStringSettings("HostPort");
                        if (text.empty()) {
                            text = std::to_string(PERIMETER_IP_PORT_DEFAULT);
                        }
                        CEditWindow* hostPortInput = (CEditWindow*)_shellIconManager.GetWnd(SQSH_MM_MULTIPLAYER_HOST_PORT_INPUT);
                        hostPortInput->SetText(text.c_str());

                        CComboWindow* hostTypeCombo = (CComboWindow*)_shellIconManager.GetWnd(SQSH_MM_MULTIPLAYER_HOST_TYPE_COMBO);
                        hostTypeCombo->pos = 0;
                        
                        //Sets for proper saves deletion
                        gameShell->currentSingleProfile.setLastGameType(UserSingleProfile::MULTIPLAYER);

                        //Ensure saves are updated before listing
                        multiplayerSaves.clear();
                        fillMultiplayerHostList();
                    }
                    break;
				case SQSH_MM_MULTIPLAYER_JOIN_SCR:
					{
						CEditWindow* input = (CEditWindow*)_shellIconManager.GetWnd(SQSH_MM_MULTIPLAYER_JOIN_IP_INPUT);
						if (input->isEmptyText()) {
                            std::string text = getStringSettings("JoinIP");
							input->SetText(text.c_str());
						}
					}
					break;
			}
			//show and start effect
			if (_id_on != SQSH_MM_SUBMIT_DIALOG_SCR) {
				CShellWindow* pWnd = _shellIconManager.GetWnd(_id_on);
				if (pWnd && pWnd->m_attr_cont) {
					for (int i = 0; i < pWnd->m_attr_cont->bgObjects.size(); i++) {
						bgScene.markToPlay(pWnd->m_attr_cont->bgObjects[i].name, pWnd->m_attr_cont->bgObjects[i].chainName, true);
					}
				}
			}
			//!!!
			_shellIconManager.GetWnd(_id_on)->Show(true, effectCtrlRollIn);
			_shellIconManager.Effect(effectCtrlRollIn, _shellIconManager.GetWnd(_id_on)); //запустить разлет
		} else {
			switch (_id_on) {
				case SHOW_LAST_SPLASH:
					_shellCursorManager.HideCursor();
					bgScene.done();
					bwScene.done();
					historyScene.done();
                    //show last splash
                    gameShell->reelAbortEnabled = lastReel.abortEnabled;
                    if (lastReel.video) {
                        gb_Music.SetVolume(0.0f);
                        gb_Music.Stop();
                        gameShell->showReelModal(lastReel.name, 0, lastReel.localized);
                    } else {
                        gameShell->showPictureModal(lastReel.name, lastReel.localized, lastReel.time);
                    }
					gameShell->GameContinue = 0;
//					_shellIconManager.GetWnd(SQSH_MM_SPLASH_LAST)->Show(1);
//					_shellIconManager.SetModalWnd(SQSH_MM_SPLASH_LAST);
					break;
                case RESTART_GAME:
                    //Close UI
                    _shellCursorManager.HideCursor();
                    gameShell->done();
                    gameShell->terminate();
                    break;
				case RESUME_GAME:
					//resume game
                    gameShell->currentSingleProfile.setGameResult(static_cast<terUniverseInterfaceMessage>(
                            gameShell->currentSingleProfile.getGameResult() & ~UNIVERSE_INTERFACE_MESSAGE_ACTIVE
                    ));
					_shellIconManager.LoadControlsGroup(SHELL_LOAD_GROUP_GAME);
					gameShell->resumeGame(true);
					_shellCursorManager.m_bShowSideArrows=1;
					_shellCursorManager.ShowCursor();
                    menuChangingDone = true;
					_bMenuMode = 0;
					break;
				case CONTINUE_BRIEFING:
					//continue briefing after pause
					historyScene.getController()->eventOccured(Controller::CONTROL_SUBMIT_EVENT);
					historyScene.stopAudio();
					break;
				case READY:
					gameShell->getNetClient()->StartLoadTheGame(true);
					menuChangingDone = true;
					break;
				case SKIP_MISSION:
					{
						//continue briefing after skip mission
						historyScene.stopAudio();
						CShellWindow* wnd = _shellIconManager.GetWnd(SQSH_MM_SKIP_BRIEFING_BTN);
//						bgScene.markToPlay(wnd->m_attr->bgObject.name, wnd->m_attr->bgObject.chainName, true);
					}
					break;
				case SKIP_BRIEFING_AFTER_PAUSE:
				case SKIP_BRIEFING:
					historyScene.stopAudio();
					//set fast speed
					historyScene.start();
					historyScene.setNormalSpeedMode(false);
					historyScene.getController()->eventOccured(Controller::CONTROL_SUBMIT_EVENT);
					break;
				case SHOW_START_SKIP_MISSION:
					{
						CShellWindow* wnd = _shellIconManager.GetWnd(SQSH_MM_SKIP_MISSION_BTN);
//						bgScene.markToPlay(wnd->m_attr->bgObject.name, wnd->m_attr->bgObject.chainName, true);
					}
				case SHOW_START_HIDE_SKIP_MISSION:
				case SHOW_START_MISSION:
					{
						//show start mission button
						CShellWindow* wnd = _shellIconManager.GetWnd(SQSH_MM_START_MISSION_BTN);
//						bgScene.markToPlay(wnd->m_attr->bgObject.name, wnd->m_attr->bgObject.chainName, true);
						historyScene.setNormalSpeedMode(true);
					}
					break;
				case SHOW_SKIP_CONTINUE:
					{
						CShellWindow* wnd = _shellIconManager.GetWnd(SQSH_MM_SKIP_BRIEFING_BTN);
//						bgScene.markToPlay(wnd->m_attr->bgObject.name, wnd->m_attr->bgObject.chainName, true);
					}
				case SHOW_CONTINUE:
					{
						//show continue button
						CShellWindow* wnd = _shellIconManager.GetWnd(SQSH_MM_CONTINUE_BRIEFING_BTN);
//						bgScene.markToPlay(wnd->m_attr->bgObject.name, wnd->m_attr->bgObject.chainName, true);
					}
					break;
			}
		}
		if (_id_on != SQSH_MM_SUBMIT_DIALOG_SCR) {
			bgScene.play(); //запустить слет BG
		}
		return 0;
	}
	return 1;
}
int SwitchMenuBGQuant1( float, float ) {
	if (!_shellIconManager.IsEffect()) {
		//fadeOut done
		CMoveButton::snd=CScaleButton::snd1=CScaleButton::snd2=0;
		if (_id_off >= 0) {
			//!!!
//			_shellIconManager.GetWnd(_id_off)->Show( 0 );

//			if (_shellIconManager.GetWnd(SQSH_MM_SUBMIT_DIALOG_SCR)->isVisible()) {
//				_shellIconManager.GetWnd(SQSH_MM_SUBMIT_DIALOG_SCR)->Show( 0 );
//				_shellIconManager.SetModalWnd(0);
//			}
			if (_id_off == SQSH_MM_SUBMIT_DIALOG_SCR) {
//				_shellIconManager.SetModalWnd(0);
//				menuChangingDone = true;
				_shellIconManager.GetWnd(_id_off)->Show(false, effectButtonsFadeOut);
				_shellIconManager.UnEffectControls(_shellIconManager.GetWnd(_id_off), effectButtonsFadeOut); //запустить слет
				_shellIconManager.Effect(effectCtrlRollOut, _shellIconManager.GetWnd(_id_off)); //запустить разлет
				_shellIconManager.AddDynamicHandler(SwitchMenuScreenQuant1, CBCODE_QUANT); //ждать пока не разлетится BG
				return 0;
			}
			bgScene.markAllToPlay(false);
			if (_id_on > 0 && _id_on != SQSH_MM_SUBMIT_DIALOG_SCR) {
				CShellWindow* pWnd = _shellIconManager.GetWnd(_id_on);
				if (pWnd && pWnd->m_attr_cont) {
					for (int i = 0; i < pWnd->m_attr_cont->bgObjects.size(); i++) {
						bgScene.unmarkToPlay(pWnd->m_attr_cont->bgObjects[i].name, pWnd->m_attr_cont->bgObjects[i].chainName, true);
					}
				}
			}
			//!!!
			_shellIconManager.GetWnd(_id_off)->Show(false, effectButtonsFadeOut);
			_shellIconManager.UnEffectControls(_shellIconManager.GetWnd(_id_off), effectButtonsFadeOut); //запустить слет
			_shellIconManager.Effect(effectCtrlRollOut, _shellIconManager.GetWnd(_id_off)); //запустить разлет
		} else {
			switch (_id_off) {
				case CONTINUE_BRIEFING:
					{
						//continue briefing after pause
						_shellIconManager.GetWnd(SQSH_MM_CONTINUE_BRIEFING_BORDER)->Show(0);
						CShellWindow* wnd = _shellIconManager.GetWnd(SQSH_MM_CONTINUE_BRIEFING_BTN);
						wnd->Show(0);
//						bgScene.markToPlay(wnd->m_attr->bgObject.name, wnd->m_attr->bgObject.chainName, false);
					}
					break;
				case READY:
					{
						_shellIconManager.GetWnd(SQSH_MM_LOBBY_START_BORDER)->Show(0);
						CShellWindow* wnd = _shellIconManager.GetWnd(SQSH_MM_LOBBY_START_BTN);
						wnd->Show(0);
						bgScene.markToPlay(wnd->m_attr->bgObject.name, wnd->m_attr->bgObject.chainName, false);
					}
					break;
				case SKIP_MISSION:
					{
						_shellIconManager.GetWnd(SQSH_MM_SKIP_MISSION_BORDER)->Show(0);
						_shellIconManager.GetWnd(SQSH_MM_START_BRIEFING_BORDER)->Show(0);

						CShellWindow* wnd = _shellIconManager.GetWnd(SQSH_MM_START_MISSION_BTN);
						wnd->Show(0);
//						bgScene.markToPlay(wnd->m_attr->bgObject.name, wnd->m_attr->bgObject.chainName, false);
						wnd = _shellIconManager.GetWnd(SQSH_MM_SKIP_MISSION_BTN);
						wnd->Show(0);
//						bgScene.markToPlay(wnd->m_attr->bgObject.name, wnd->m_attr->bgObject.chainName, false);
					}
					break;
				case SKIP_BRIEFING_AFTER_PAUSE:
					{
						_shellIconManager.GetWnd(SQSH_MM_CONTINUE_BRIEFING_BORDER)->Show(0);
						CShellWindow* wnd = _shellIconManager.GetWnd(SQSH_MM_CONTINUE_BRIEFING_BTN);
						wnd->Show(0);
//						bgScene.markToPlay(wnd->m_attr->bgObject.name, wnd->m_attr->bgObject.chainName, false);
					}
				case SKIP_BRIEFING:
				case SHOW_START_HIDE_SKIP_MISSION:
					{
						_shellIconManager.GetWnd(SQSH_MM_SKIP_BRIEFING_BORDER)->Show(0);
						CShellWindow* wnd = _shellIconManager.GetWnd(SQSH_MM_SKIP_BRIEFING_BTN);
						wnd->Show(0);
//						bgScene.markToPlay(wnd->m_attr->bgObject.name, wnd->m_attr->bgObject.chainName, false);
					}
					break;
			}
		}
		if (_id_on != SQSH_MM_SUBMIT_DIALOG_SCR) {
			bgScene.play(); //запустить разлет BG
		}
		_shellIconManager.AddDynamicHandler(SwitchMenuScreenQuant1, CBCODE_QUANT); //ждать пока не разлетится BG
		return 0;
	}
	return 1;
}
void CShellIconManager::SwitchMenuScreens(int id_off, int id_on) {
	if (id_off == LAST_VISIBLE) {
		int id = getVisibleMenuScr();
		id_off = (id < 0) ? FAKE : id;
	}
	if (gameShell->BuildingInstallerInited()) {
        gameShell->BuildingInstaller->CancelObject();
    }
	gameShell->cancelMouseLook();

    //Content is selected and user wants to go some menu that needs default content to be restored
    if (terGameContentAvailable != terGameContentSelect) {
        std::string initial_menu;
        switch (id_on) {
            case SQSH_MM_START_SCR:
            //These are just in case
            case SQSH_MM_BATTLE_SCR:
            case SQSH_MM_MULTIPLAYER_LIST_SCR:
            case SQSH_MM_ADDONS_SCR:
                initial_menu = "START";
                break;
            //Required if user clicks on Community from Credits menu
            case SQSH_MM_COMMUNITY_SCR:
                initial_menu = "COMMUNITY";
                break;
        }
        if (!initial_menu.empty()) {
            std::vector<std::string> args;
            args.emplace_back("tmp_start_splash=0");
            if (initial_menu != "START") {
                args.emplace_back("tmp_initial_menu=" + initial_menu);
            }
            request_application_restart(&args);
            //We play splash since "we are leaving the content" and user may not see content specific splash otherwise
            id_on = SQSH_MM_SPLASH_LAST;
        }
    }

	menuChangingDone = false;
	_id_on = id_on;
	_id_off = id_off;
#if defined(PERIMETER_DEBUG) && 0
    printf("Switch menu %s -> %s\n",
           (0 < _id_off ? getEnumName((ShellControlID)_id_off) : std::to_string(_id_off).c_str()),
           (0 < _id_on ? getEnumName((ShellControlID)_id_on) : std::to_string(_id_on).c_str())
    );
#endif
	if (_id_off >= 0) {
		if (_shellIconManager.GetWnd(SQSH_MM_SUBMIT_DIALOG_SCR)->isVisible()) {
			Effect(effectButtonsFadeOut, _shellIconManager.GetWnd(SQSH_MM_SUBMIT_DIALOG_SCR)); //запустить разлет
		}
		Effect(effectButtonsFadeOut, _shellIconManager.GetWnd(_id_off)); //запустить разлет
	} else {
		switch (_id_off) {
			case CONTINUE_BRIEFING:
				//continue briefing after pause
				_shellIconManager.Effect(effectButtonsFadeOut, _shellIconManager.GetWnd(SQSH_MM_CONTINUE_BRIEFING_BTN)); //запустить разлет
				_shellIconManager.Effect(effectButtonsFadeOut, _shellIconManager.GetWnd(SQSH_MM_CONTINUE_BRIEFING_BORDER)); //запустить разлет
				break;
			case READY:
				_shellIconManager.Effect(effectButtonsFadeOut, _shellIconManager.GetWnd(SQSH_MM_LOBBY_START_BTN)); //запустить разлет
				_shellIconManager.Effect(effectButtonsFadeOut, _shellIconManager.GetWnd(SQSH_MM_LOBBY_START_BORDER)); //запустить разлет
				break;
			case SKIP_MISSION:
				//continue briefing after skip mission
				_shellIconManager.Effect(effectButtonsFadeOut, _shellIconManager.GetWnd(SQSH_MM_START_MISSION_BTN));
				_shellIconManager.Effect(effectButtonsFadeOut, _shellIconManager.GetWnd(SQSH_MM_SKIP_MISSION_BTN));
				_shellIconManager.Effect(effectButtonsFadeOut, _shellIconManager.GetWnd(SQSH_MM_SKIP_MISSION_BORDER));
				_shellIconManager.Effect(effectButtonsFadeOut, _shellIconManager.GetWnd(SQSH_MM_START_BRIEFING_BORDER));
				break;
			case SHOW_START_HIDE_SKIP_MISSION:
				//show start mission button
				_shellIconManager.Effect(effectButtonsFadeOut, _shellIconManager.GetWnd(SQSH_MM_SKIP_BRIEFING_BORDER));
				_shellIconManager.Effect(effectButtonsFadeOut, _shellIconManager.GetWnd(SQSH_MM_SKIP_BRIEFING_BTN));
				break;
			case SKIP_BRIEFING_AFTER_PAUSE:
				_shellIconManager.Effect(effectButtonsFadeOut, _shellIconManager.GetWnd(SQSH_MM_CONTINUE_BRIEFING_BTN)); //запустить разлет
				_shellIconManager.Effect(effectButtonsFadeOut, _shellIconManager.GetWnd(SQSH_MM_CONTINUE_BRIEFING_BORDER)); //запустить разлет
			case SKIP_BRIEFING:
				_shellIconManager.Effect(effectButtonsFadeOut, _shellIconManager.GetWnd(SQSH_MM_SKIP_BRIEFING_BORDER)); //запустить разлет
				_shellIconManager.Effect(effectButtonsFadeOut, _shellIconManager.GetWnd(SQSH_MM_SKIP_BRIEFING_BTN)); //запустить разлет
				break;
		}
	}
	AddDynamicHandler(SwitchMenuBGQuant1, CBCODE_QUANT); //ждать пока не разлетится
    AddDynamicHandler(setupMenuCursorQuant, CBCODE_QUANT);
}

/////////////////////////////////////////////////////////////
static int nCurrentSplash = -1;
bool bSplashAbort = false;

int HideSplashScreen(float, float)
{
	CShellWindow* pWnd = _shellIconManager.GetWnd(nCurrentSplash);
	if(pWnd)
	{
		if(pWnd->state & SQSH_VISIBLE)
			pWnd->Show(0);
	}

	return 0;
}
void OnSplashScreenKey1(CShellWindow* pWnd, InterfaceEventCode code, int param)
{
	if(code == EVENT_SHOWWND)
	{
		nCurrentSplash = SQSH_MM_SPLASH1;
		_shellIconManager.AddDynamicHandler(HideSplashScreen, CBCODE_QUANT, splash_delay_time);
	}
	else if(code == EVENT_UNPRESSED)
	{
		pWnd->Show(0);
		//bSplashAbort = true;
	}
	else if(code == EVENT_HIDEWND)
	{
		if(bSplashAbort)
		{
			gameShell->switchToInitialMenu();
		}
		else
		{
			_shellIconManager.GetWnd(SQSH_MM_SPLASH2)->Show(1);
			_shellIconManager.SetModalWnd(SQSH_MM_SPLASH2);
		}
	}
}
void OnSplashScreenKey2(CShellWindow* pWnd, InterfaceEventCode code, int param)
{
	if(code == EVENT_SHOWWND)
	{
		nCurrentSplash = SQSH_MM_SPLASH2;
		_shellIconManager.AddDynamicHandler(HideSplashScreen, CBCODE_QUANT, splash_delay_time);
	}
	else if(code == EVENT_UNPRESSED)
	{
		pWnd->Show(0);
		//bSplashAbort = true;
	}
	else if(code == EVENT_HIDEWND)
	{
		if(bSplashAbort)
		{
            gameShell->switchToInitialMenu();
		}
		else
		{
			_shellIconManager.GetWnd(SQSH_MM_SPLASH3)->Show(1);
			_shellIconManager.SetModalWnd(SQSH_MM_SPLASH3);
		}
	}
}
void OnSplashScreenKey3(CShellWindow* pWnd, InterfaceEventCode code, int param)
{
	if(code == EVENT_SHOWWND)
	{
		nCurrentSplash = SQSH_MM_SPLASH3;
		_shellIconManager.AddDynamicHandler(HideSplashScreen, CBCODE_QUANT, splash_delay_time);
	}
	else if(code == EVENT_UNPRESSED)
	{
		pWnd->Show(0);
		//bSplashAbort = true;
	}
	else if(code == EVENT_HIDEWND)
	{
		if(bSplashAbort)
		{
            gameShell->switchToInitialMenu();
		}
		else
		{
			_shellIconManager.GetWnd(SQSH_MM_SPLASH4)->Show(1);
			_shellIconManager.SetModalWnd(SQSH_MM_SPLASH4);
		}
	}
}
void OnSplashScreenKey4(CShellWindow* pWnd, InterfaceEventCode code, int param)
{
	static bool bAlreadyClicked = false;
	if(code == EVENT_SHOWWND)
	{
		nCurrentSplash = SQSH_MM_SPLASH4;
		_shellIconManager.AddDynamicHandler(HideSplashScreen, CBCODE_QUANT, splash_delay_time);
		bAlreadyClicked = false;
	}
	else if(code == EVENT_UNPRESSED || code == EVENT_HIDEWND)
	{
		if(!bAlreadyClicked)
		{
			CSplashWindow* pSplash = (CSplashWindow*)pWnd;

			if(pSplash->m_alpha == 255 || code == EVENT_HIDEWND)
			{
                gameShell->switchToInitialMenu();

				bAlreadyClicked = true;
			}
		}
	}
}
void OnSplashScreenKeyLast(CShellWindow* pWnd, InterfaceEventCode code, int param)
{
	if(code == EVENT_SHOWWND)
	{
		nCurrentSplash = SQSH_MM_SPLASH_LAST;
		_shellIconManager.AddDynamicHandler(HideSplashScreen, CBCODE_QUANT, splash_delay_time);
	}
	else if(code == EVENT_UNPRESSED)
	{
		pWnd->Show(0);
		//bSplashAbort = true;
	}
	else if(code == EVENT_HIDEWND)
	{
		gameShell->GameContinue = 0;
	}
}

//-----------------------------------
//general
void onMMBackButton(CShellWindow* pWnd, InterfaceEventCode code, int param) {
	if( code == EVENT_UNPRESSED && intfCanHandleInput() ) {
		int nShow;
		switch ( pWnd->m_pParent->ID ) {
			case SQSH_MM_SINGLE_SCR:
            case SQSH_MM_MULTIPLAYER_LIST_SCR:
            case SQSH_MM_COMMUNITY_SCR:
            case SQSH_MM_OPTIONS_SCR:
            case SQSH_MM_CREDITS_SCR:
                nShow = SQSH_MM_START_SCR;
                break;
            case SQSH_MM_ADDONS_SCR:
                nShow = SQSH_MM_COMMUNITY_SCR;
                break;
			case SQSH_MM_GAME_SCR:
			case SQSH_MM_SOUND_SCR:
				nShow = SQSH_MM_OPTIONS_SCR;
				break;
			case SQSH_MM_GRAPHICS_SCR:
				GraphOptionsManager::getInstance().reset();
				nShow = SQSH_MM_OPTIONS_SCR;
				break;
            case SQSH_MM_MULTIPLAYER_JOIN_SCR:
            case SQSH_MM_MULTIPLAYER_LOBBY_SCR:
				nShow = SQSH_MM_MULTIPLAYER_LIST_SCR;
				break;
            case SQSH_MM_MULTIPLAYER_HOST_SCR:
                nShow = SQSH_MM_MULTIPLAYER_LOBBY_SCR;
                break;
			case SQSH_MM_SCENARIO_SCR:
			case SQSH_MM_BATTLE_SCR:
			case SQSH_MM_LOAD_SCR:
			case SQSH_MM_LOAD_REPLAY_SCR:
            case SQSH_MM_CONTENT_CHOOSER_SCR:
				nShow = SQSH_MM_SINGLE_SCR;
				break;
			case SQSH_MM_LOAD_IN_GAME_SCR:
			case SQSH_MM_SAVE_GAME_SCR:
				nShow = SQSH_MM_INMISSION_SCR;
				break;
			case SQSH_MM_SAVE_REPLAY_SCR:
				nShow = SQSH_MM_STATS_SCR;
				break;
			case SQSH_MM_PROFILE_SCR:
				showSingleMenu(pWnd);
				return;
			case SQSH_MM_MISSION_TASK_SCR:
				nShow = RESUME_GAME;
				break;
			case SQSH_MM_CUSTOM_SCR:
				OnComboGraphicsSettings(_shellIconManager.GetWnd(SQSH_MM_SETTINGS_COMBO), EVENT_CREATEWND, -1);
				nShow = SQSH_MM_GRAPHICS_SCR;
				break;
			case SQSH_MM_SCREEN_OPTIONS:
//				GraphOptionsManager::getInstance().reset();
//				extern void PerimeterDataChannelSave();
//				PerimeterDataChannelSave();
				nShow = SQSH_MM_INMISSION_SCR;
				break;
			case SQSH_MM_SCREEN_GAME:
			case SQSH_MM_SCREEN_SOUND:
				nShow = SQSH_MM_SCREEN_OPTIONS;
				break;
			case SQSH_MM_INGAME_CUSTOM_SCR:
				OnComboGraphicsSettings(_shellIconManager.GetWnd(SQSH_MM_SETTINGS_COMBO), EVENT_CREATEWND, -1);
				nShow = SQSH_MM_SCREEN_GRAPHICS;
				break;
			case SQSH_MM_SCREEN_GRAPHICS:
				GraphOptionsManager::getInstance().reset();
				nShow = SQSH_MM_SCREEN_OPTIONS;
				break;
			default:
				return;
		}
		_shellIconManager.SwitchMenuScreens( pWnd->m_pParent->ID, nShow );
	}
}

//start menu
void onMMQuitButton(CShellWindow* pWnd, InterfaceEventCode code, int param) {
	if( code == EVENT_UNPRESSED && intfCanHandleInput() ) {
		_shellIconManager.SwitchMenuScreens(pWnd->m_pParent->ID, SHOW_LAST_SPLASH);
	}
}
void onMMOptionsButton(CShellWindow* pWnd, InterfaceEventCode code, int param) {
	if( code == EVENT_UNPRESSED && intfCanHandleInput() ) {
		_shellIconManager.SwitchMenuScreens(pWnd->m_pParent->ID, SQSH_MM_OPTIONS_SCR);
	}
}

void onMMLangButton(CShellWindow* pWnd, InterfaceEventCode code, int param) {
	if( code == EVENT_SHOWWND ) {
        CShellPushButton* btn = dynamic_cast<CShellPushButton*>(pWnd);
        if (btn->labelText.empty()) {
            std::string locale = getLocale();
            locale = string_to_capitalize(locale.c_str());
            string_replace_all(locale, "Russian", "Русский");
            btn->setText(convertToCodepage(locale.c_str(), getLocale()));
        }
    } else if( code == EVENT_UNPRESSED && intfCanHandleInput() ) {
        //Reset locale and restart game so it shows lang selector
        std::vector<std::string> args;
        args.emplace_back("tmp_chooselocale=1");
		args.emplace_back("tmp_start_splash=0");
        request_application_restart(&args);
        _shellIconManager.SwitchMenuScreens(pWnd->m_pParent->ID, RESTART_GAME);
    }
}

//end mission menu
void onMMResumeButton(CShellWindow* pWnd, InterfaceEventCode code, int param) {
	if( code == EVENT_UNPRESSED && intfCanHandleInput() ) {
		_shellIconManager.SwitchMenuScreens( pWnd->m_pParent->ID, RESUME_GAME );
	}
}
void onMMContinueButton(CShellWindow* pWnd, InterfaceEventCode code, int param) {
	if( code == EVENT_UNPRESSED && intfCanHandleInput() ) {
		_shellIconManager.SwitchMenuScreens( pWnd->m_pParent->ID, SQSH_MM_STATS_SCR );
	}
}

//stats menu
int restartAction(float, float) {
	gb_Music.FadeVolume(_fEffectButtonTotalTime*0.001f);
	HTManager::instance()->GameClose();
	StartSpace();
	missionToExec.restart();
	_shellIconManager.SetModalWnd(0);
	_shellIconManager.LoadControlsGroup(SHELL_LOAD_GROUP_MENU);
	_shellIconManager.SwitchMenuScreens(RESUME_GAME, SQSH_MM_LOADING_MISSION_SCR);
	return 1;
}
void onMMRestartButton(CShellWindow* pWnd, InterfaceEventCode code, int param) {
	if( code == EVENT_UNPRESSED && intfCanHandleInput() ) {

		std::string text = qdTextDB::instance().getText("Interface.Menu.Messages.Confirmations.Restart");
		setupYesNoMessageBox(restartAction, 0, text);

		showMessageBox();
	}
}
void onMMSaveReplayButton(CShellWindow* pWnd, InterfaceEventCode code, int param) {
	if( code == EVENT_UNPRESSED && intfCanHandleInput() ) {
		_shellIconManager.SwitchMenuScreens( pWnd->m_pParent->ID, SQSH_MM_SAVE_REPLAY_SCR );
	}
}
void onMMQuitFromStatsButton(CShellWindow* pWnd, InterfaceEventCode code, int param) {
	if( code == EVENT_UNPRESSED && intfCanHandleInput() ) {
		if (!gameShell->isStartedWithMainmenu()) {
			gameShell->GameContinue = 0;
			return;
		}
		HTManager::instance()->GameClose();
//		bgScene.setEnabled(true);
		_shellIconManager.LoadControlsGroup(SHELL_LOAD_GROUP_MENU);
		switch(gameShell->currentSingleProfile.getLastGameType()) {
			case UserSingleProfile::SCENARIO:
				_shellIconManager.SwitchMenuScreens(-1, SQSH_MM_SCENARIO_SCR);
				break;
			case UserSingleProfile::BATTLE:
			case UserSingleProfile::SURVIVAL:
				_shellIconManager.SwitchMenuScreens(-1, SQSH_MM_BATTLE_SCR);
				break;
			case UserSingleProfile::MULTIPLAYER:
				gameShell->getNetClient()->FinishGame();
				_shellIconManager.SwitchMenuScreens(-1, SQSH_MM_MULTIPLAYER_LIST_SCR);
				break;
			case UserSingleProfile::REPLAY:
				_shellIconManager.SwitchMenuScreens(-1, SQSH_MM_LOAD_REPLAY_SCR);
				break;
			default:
				_shellIconManager.SwitchMenuScreens(-1, SQSH_MM_START_SCR);
		}
	}
}
void onMMContinueFromStatsButton(CShellWindow* pWnd, InterfaceEventCode code, int param) {
	if( code == EVENT_UNPRESSED && intfCanHandleInput() ) {
		HTManager::instance()->GameClose();
//		bgScene.setEnabled(true);
		_shellIconManager.LoadControlsGroup(SHELL_LOAD_GROUP_MENU);
		if (gameShell->briefingEnabled) {
			hideAuxBriefingButtons();
			_shellIconManager.SwitchMenuScreens(-1, SQSH_MM_BRIEFING_SCR);
		} else {
//			Instant mission
			_shellIconManager.SwitchMenuScreens(-1, SQSH_MM_SCENARIO_SCR);
		}
	}
}
void onMMStatsTotalButton(CShellWindow* pWnd, InterfaceEventCode code, int param) {
	if( code == EVENT_UNPRESSED ) {
		hideTables();
		_shellIconManager.GetWnd(SQSH_MM_STATS_TOTAL_HEAD_LIST)->Show(1);
		_shellIconManager.GetWnd(SQSH_MM_STATS_TOTAL_LIST)->Show(1);
		_shellIconManager.GetWnd(SQSH_MM_STATS_TOTAL_RAMKA)->Show(0);
	}
}
void onMMStatsGeneralButton(CShellWindow* pWnd, InterfaceEventCode code, int param) {
	if( code == EVENT_UNPRESSED ) {
		hideTables();
		_shellIconManager.GetWnd(SQSH_MM_STATS_GENERAL_HEAD_LIST)->Show(1);
		_shellIconManager.GetWnd(SQSH_MM_STATS_GENERAL_LIST)->Show(1);
		_shellIconManager.GetWnd(SQSH_MM_STATS_GENERAL_RAMKA)->Show(0);
	}
}
void onMMStatsUnitsButton(CShellWindow* pWnd, InterfaceEventCode code, int param) {
	if( code == EVENT_UNPRESSED ) {
		hideTables();
		_shellIconManager.GetWnd(SQSH_MM_STATS_UNITS_HEAD_LIST)->Show(1);
		_shellIconManager.GetWnd(SQSH_MM_STATS_UNITS_LIST)->Show(1);
		_shellIconManager.GetWnd(SQSH_MM_STATS_UNITS_RAMKA)->Show(0);
	}
}
void onMMStatsBuildingsButton(CShellWindow* pWnd, InterfaceEventCode code, int param) {
	if( code == EVENT_UNPRESSED ) {
		hideTables();
		_shellIconManager.GetWnd(SQSH_MM_STATS_BUILDINGS_HEAD_LIST)->Show(1);
		_shellIconManager.GetWnd(SQSH_MM_STATS_BUILDINGS_LIST)->Show(1);
		_shellIconManager.GetWnd(SQSH_MM_STATS_BUILDINGS_RAMKA)->Show(0);
	}
}

//inmission menu
void onMMInMissSaveButton(CShellWindow* pWnd, InterfaceEventCode code, int param) {
	if( code == EVENT_UNPRESSED && intfCanHandleInput() ) {
		_shellIconManager.SwitchMenuScreens( pWnd->m_pParent->ID, SQSH_MM_SAVE_GAME_SCR );
	}
}
void onMMInMissLoadButton(CShellWindow* pWnd, InterfaceEventCode code, int param) {
	if( code == EVENT_UNPRESSED && intfCanHandleInput() ) {
		_shellIconManager.SwitchMenuScreens(pWnd->m_pParent->ID, SQSH_MM_LOAD_IN_GAME_SCR);
	}
}
void onMMInMissOptionsButton(CShellWindow* pWnd, InterfaceEventCode code, int param) {
	if( code == EVENT_UNPRESSED && intfCanHandleInput() ) {
        //TODO why was disabled in multi?
		//_shellIconManager.GetWnd(SQSH_MM_OPTIONS_GRAPHICS)->Enable(gameShell->currentSingleProfile.getLastGameType() != UserSingleProfile::MULTIPLAYER);
		_shellIconManager.SwitchMenuScreens(pWnd->m_pParent->ID, SQSH_MM_SCREEN_OPTIONS);
	}		
}
void onMMInMissRestartButton(CShellWindow* pWnd, InterfaceEventCode code, int param) {
	if( code == EVENT_UNPRESSED && intfCanHandleInput() ) {
		std::string text = qdTextDB::instance().getText("Interface.Menu.Messages.Confirmations.Restart");
		setupYesNoMessageBox(restartAction, 0, text);

		showMessageBox();
	}		
}
void onMMInMissResumeButton(CShellWindow* pWnd, InterfaceEventCode code, int param) {
	if( code == EVENT_UNPRESSED && intfCanHandleInput() ) {
		_shellIconManager.SwitchMenuScreens( pWnd->m_pParent->ID, RESUME_GAME );
	}		
}
void exitToInterfaceMessage(CShellWindow* pWnd) {
    int id_off = pWnd ? pWnd->m_pParent->ID : -1;
	switch(gameShell->currentSingleProfile.getLastGameType()) {
		case UserSingleProfile::REPLAY:
			{
				if (_shellIconManager.isCutSceneMode()) {
					_shellIconManager.setCutSceneMode(false, false);
				}
				_shellIconManager.GetWnd( SQSH_MM_RESUME_BTN )->Enable(false);
				gameShell->cancelMouseLook();
				gameShell->prepareForInGameMenu();
				CTextWindow *Wnd = (CTextWindow*)_shellIconManager.GetWnd( SQSH_MM_RESULT_TXT );
				if (Wnd) {
					Wnd->SetText( "" );
				}
				_shellIconManager.SwitchMenuScreens(id_off, SQSH_MM_ENDMISSION_SCR );
			}
			break;
		default:
            terUniverseInterfaceMessage msg = gameShell->currentSingleProfile.getGameResult();
            if (msg == UNIVERSE_INTERFACE_MESSAGE_GAME_RESULT_UNDEFINED) {
                msg = UNIVERSE_INTERFACE_MESSAGE_GAME_DEFEAT;
            }
			processInterfaceMessageLater(msg, id_off);
	}
}
int toQuitQuant( float, float ) {
	if (menuChangingDone) {
		exitToInterfaceMessage(_shellIconManager.GetWnd( SQSH_MM_INMISSION_QUIT_BTN ));
		return 0;
	}
	return 1;
}
int quitAction(float, float) {
	hideMessageBox();
	_shellIconManager.AddDynamicHandler( toQuitQuant, CBCODE_QUANT );
	return 1;
}
void onMMInMissQuitButton(CShellWindow* pWnd, InterfaceEventCode code, int param) {
	if( code == EVENT_UNPRESSED && intfCanHandleInput() ) {
		std::string text = qdTextDB::instance().getText("Interface.Menu.Messages.Confirmations.Quit");
		setupYesNoMessageBox(quitAction, 0, text);

		showMessageBox();
//		exitToInterfaceMessage(pWnd);
	}		
}

//game content

//TODO call this from load/ingame-load/replay menus to enable auto switching (provide initial_menu to current menu ID)
void switchGameContent(GAME_CONTENT selected, const std::string& initial_menu) {
	if (selected == terGameContentBase) {
		selected = terGameContentAvailable;
	}
	if (selected == terGameContentSelect) {
		//Selected the already selected content, no need to restart
		_shellIconManager.SwitchMenuScreens( SQSH_MM_CONTENT_CHOOSER_SCR, SQSH_MM_SINGLE_SCR );
	} else {
		//Set args and restart
		std::vector<std::string> args;
		args.emplace_back("tmp_initial_menu=" + initial_menu);
		args.emplace_back("tmp_content_select=" + getGameContentEnumName(selected));
		if (selected == terGameContentAvailable) {
			args.emplace_back("tmp_start_splash=0");
		}
		request_application_restart(&args);
		_shellIconManager.SwitchMenuScreens( SQSH_MM_CONTENT_CHOOSER_SCR, RESTART_GAME );
	}
}

//load game
int delLoadSaveAction(float, float) {
	CListBoxWindow* list = (CListBoxWindow*)_shellIconManager.GetWnd(SQSH_MM_LOAD_MAP_LIST);
//	DeleteFile( savedGames[list->GetCurSel()].saveName() );
//	loadMapVector(savedGames, "RESOURCE\\SAVES\\", "RESOURCE\\SAVES\\*.spg");
	gameShell->currentSingleProfile.deleteSave(savedGames[list->GetCurSel()].savePathContent());
    savedGames.clear();
	const std::string& savesDir = gameShell->currentSingleProfile.getSavesDirectory();
	loadMapVector(savedGames, savesDir, ".spg");
	fillList(SQSH_MM_LOAD_MAP_LIST, savedGames, SQSH_MM_LOAD_MAP, SQSH_MM_LOAD_MAP_DESCR_TXT);
	return 0;
}
bool setupMissionToExec(int pos) {
    if (pos < 0 || pos > savedGames.size()) {
        return false;
    }
    checkMissionDescription(pos, savedGames, GT_SINGLE_PLAYER);
	
	MissionDescription& desc = savedGames[pos];

    //Check if content is compatible
    std::string missingContent = checkMissingContent(desc);
    if (!missingContent.empty()) {
        setupOkMessageBox(nullptr, 0, missingContent, MBOX_BACK);
        showMessageBox();
        return false;
    }
	
	missionToExec = desc;

    //Setup according to mission type
    if (missionToExec.missionNumber == -1) {
        gameShell->currentSingleProfile.setLastGameType(UserSingleProfile::BATTLE);
//		strncpy(missionToExec.getActivePlayerData().Name, gameShell->currentSingleProfile.getCurrentProfile().name.c_str(), PERIMETER_CONTROL_NAME_SIZE);
    } else if (missionToExec.missionNumber == -2) {
        gameShell->currentSingleProfile.setLastGameType(UserSingleProfile::SURVIVAL);
    } else {
        gameShell->currentSingleProfile.setLastGameType(UserSingleProfile::SCENARIO);
        historyScene.goToJustAfterMissionPosition(missionToExec.missionNumber);
        historyScene.setMissionNumberToExecute(missionToExec.missionNumber);
//		strncpy(missionToExec.getActivePlayerData().Name, gameShell->currentSingleProfile.getCurrentProfile().name.c_str(), PERIMETER_CONTROL_NAME_SIZE);
    }

    return true;
}
void loadGame(CListBoxWindow* listBox) {
	int pos = listBox->GetCurSel();
    if (setupMissionToExec(pos)) {
        gb_Music.FadeVolume(_fEffectButtonTotalTime * 0.001f);
        _shellIconManager.SwitchMenuScreens(listBox->m_pParent->ID, SQSH_MM_LOADING_MISSION_SCR);
    }
}

void onMMLoadMapList(CShellWindow* pWnd, InterfaceEventCode code, int param) {
	if ( code == EVENT_PRESSED && intfCanHandleInput() ) {
		CListBoxWindow* list = (CListBoxWindow*)pWnd;
		int pos = list->GetCurSel();
		if (pos >= 0 && pos < savedGames.size()) {
			setupMapDescWnd(pos, savedGames, SQSH_MM_LOAD_MAP, SQSH_MM_LOAD_MAP_DESCR_TXT);
//			((CShowMapWindow*)_shellIconManager.GetWnd(SQSH_MM_LOAD_MAP))->setWorldID( savedGames[pos].worldID() );
//			((CTextWindow*)_shellIconManager.GetWnd(SQSH_MM_LOAD_MAP_DESCR_TXT))->setText( savedGames[pos].missionDescription() );
		}
	} else if ( code == EVENT_DOUBLECLICK && intfCanHandleInput() ) {
		loadGame((CListBoxWindow*)pWnd);
	} else if (code == EVENT_DRAWWND) {
		int pos = ((CListBoxWindow*)pWnd)->GetCurSel();
		_shellIconManager.GetWnd(SQSH_MM_LOAD_GO_BTN)->Enable(pos != -1);
	}
}
void onMMLoadGoButton(CShellWindow* pWnd, InterfaceEventCode code, int param) {
	if( code == EVENT_UNPRESSED && intfCanHandleInput() ) {
		loadGame((CListBoxWindow*)_shellIconManager.GetWnd(SQSH_MM_LOAD_MAP_LIST));
	}		
}
void onMMDelSaveButton(CShellWindow* pWnd, InterfaceEventCode code, int param) {
	if( code == EVENT_UNPRESSED && intfCanHandleInput() && param == VK_LBUTTON) {

		CListBoxWindow* list = (CListBoxWindow*)_shellIconManager.GetWnd(SQSH_MM_LOAD_MAP_LIST);
		std::string text = qdTextDB::instance().getText("Interface.Menu.Messages.Confirmations.DeleteSave");
		std::string saveName = savedGames[list->GetCurSel()].missionName();
		char* mess = new char[text.length() + saveName.length()];
		sprintf(mess, text.c_str(), saveName.c_str());
		setupYesNoMessageBox(delLoadSaveAction, 0, mess);
		delete [] mess;

		showMessageBox();
	} else if (code == EVENT_DRAWWND) {
		CListBoxWindow* list = (CListBoxWindow*)_shellIconManager.GetWnd(SQSH_MM_LOAD_MAP_LIST);
		pWnd->Enable( list->GetCurSel() >= 0 );
	}
}

//load replay
int delLoadReplayAction(float, float) {
	CListBoxWindow* list = (CListBoxWindow*)_shellIconManager.GetWnd(SQSH_MM_LOAD_REPLAY_LIST);
	std::remove( replays[list->GetCurSel()].playReelPath().c_str() );
    scan_resource_paths(convert_path_content(REPLAY_PATH));
    replays.clear();
	loadMapVector(replays, REPLAY_PATH, "", true);
	fillReplayList(SQSH_MM_LOAD_REPLAY_LIST, replays, SQSH_MM_LOAD_REPLAY_MAP, SQSH_MM_LOAD_REPLAY_DESCR_TXT);
	return 0;
}
void loadReplay(CListBoxWindow* listBox) {
	int pos = listBox->GetCurSel();
	if (pos != -1) {
        checkMissionDescription(pos, replays, GT_PLAY_RELL);
        missionToExec = replays[pos];

        //Check if content is compatible
        std::string missingContent = checkMissingContent(missionToExec);
        if (!missingContent.empty()) {
            setupOkMessageBox(nullptr, 0, missingContent, MBOX_BACK);
            showMessageBox();
            return;
        }
        
		gameShell->currentSingleProfile.setLastGameType(UserSingleProfile::REPLAY);
		gb_Music.FadeVolume(_fEffectButtonTotalTime*0.001f);
		_shellIconManager.SwitchMenuScreens(listBox->m_pParent->ID, SQSH_MM_LOADING_MISSION_SCR);
	}
}
void onMMReplayList(CShellWindow* pWnd, InterfaceEventCode code, int param) {
	if ( code == EVENT_PRESSED && intfCanHandleInput() ) {
		CListBoxWindow* list = (CListBoxWindow*)pWnd;
		int pos = list->GetCurSel();
		if (pos >= 0 && pos < replays.size()) {
			setupReplayDescWnd(pos, replays, SQSH_MM_LOAD_REPLAY_MAP, SQSH_MM_LOAD_REPLAY_DESCR_TXT);
//			((CShowMapWindow*)_shellIconManager.GetWnd(SQSH_MM_LOAD_REPLAY_MAP))->setWorldID( replays[pos].worldID() );
//			((CTextWindow*)_shellIconManager.GetWnd(SQSH_MM_LOAD_REPLAY_DESCR_TXT))->setText( replays[pos].missionNamePlayReelGame );
		}
	} else if ( code == EVENT_DOUBLECLICK && intfCanHandleInput() ) {
		loadReplay((CListBoxWindow*)pWnd);
	} else if (code == EVENT_DRAWWND) {
		int pos = ((CListBoxWindow*)pWnd)->GetCurSel();
		_shellIconManager.GetWnd(SQSH_MM_LOAD_REPLAY_GO_BTN)->Enable(pos != -1);
	}
}

void onMMReplayGoButton(CShellWindow* pWnd, InterfaceEventCode code, int param) {
	if( code == EVENT_UNPRESSED && intfCanHandleInput() ) {
		loadReplay((CListBoxWindow*)_shellIconManager.GetWnd(SQSH_MM_LOAD_REPLAY_LIST));
	}		
}

void onMMDelReplayButton(CShellWindow* pWnd, InterfaceEventCode code, int param) {
	if( code == EVENT_UNPRESSED && intfCanHandleInput() && param == VK_LBUTTON) {
		CListBoxWindow* list = (CListBoxWindow*)_shellIconManager.GetWnd(SQSH_MM_LOAD_REPLAY_LIST);
		std::string text = qdTextDB::instance().getText("Interface.Menu.Messages.Confirmations.DeleteReplay");
		std::string replayName = replays[list->GetCurSel()].missionNamePlayReelGame;
		char* mess = new char[text.length() + replayName.length()];
		sprintf(mess, text.c_str(), replayName.c_str());
		setupYesNoMessageBox(delLoadReplayAction, 0, mess);
		delete [] mess;
		showMessageBox();
	} else if (code == EVENT_DRAWWND) {
		CListBoxWindow* list = (CListBoxWindow*)_shellIconManager.GetWnd(SQSH_MM_LOAD_REPLAY_LIST);
		pWnd->Enable( list->GetCurSel() >= 0 );
	}
}

//load in game
int delLoadInGameSaveAction(float, float) {
	CListBoxWindow* list = (CListBoxWindow*)_shellIconManager.GetWnd(SQSH_MM_LOAD_IN_GAME_MAP_LIST);
	gameShell->currentSingleProfile.deleteSave(savedGames[list->GetCurSel()].savePathContent());
    savedGames.clear();
	const std::string& savesDir = gameShell->currentSingleProfile.getSavesDirectory();
	loadMapVector(savedGames, savesDir, ".spg");
	fillList(SQSH_MM_LOAD_IN_GAME_MAP_LIST, savedGames, SQSH_MM_LOAD_IN_GAME_MAP, SQSH_MM_LOAD_IN_GAME_MAP_DESCR_TXT);
	return 0;
}
void loadFromGame(CListBoxWindow* listBox) {
	int pos = listBox->GetCurSel();
	if (setupMissionToExec(pos)) {
		gb_Music.FadeVolume(_fEffectButtonTotalTime*0.001f);
		HTManager::instance()->GameClose();
		StartSpace();
		_shellIconManager.LoadControlsGroup(SHELL_LOAD_GROUP_MENU);
		_shellIconManager.SwitchMenuScreens(RESUME_GAME, SQSH_MM_LOADING_MISSION_SCR);
	}
}
void onMMLoadInGameMapList(CShellWindow* pWnd, InterfaceEventCode code, int param) {
	if ( code == EVENT_PRESSED ) {
		CListBoxWindow* list = (CListBoxWindow*) pWnd;
		int pos = list->GetCurSel();
		if (pos >= 0 && pos < savedGames.size()) {
			setupMapDescWnd(pos, savedGames, SQSH_MM_LOAD_IN_GAME_MAP, SQSH_MM_LOAD_IN_GAME_MAP_DESCR_TXT);
//			((CShowMapWindow*)_shellIconManager.GetWnd(SQSH_MM_LOAD_IN_GAME_MAP))->setWorldID( savedGames[pos].worldID() );
//			((CTextWindow*)_shellIconManager.GetWnd(SQSH_MM_LOAD_IN_GAME_MAP_DESCR_TXT))->setText( savedGames[pos].missionDescription() );
		}
	} else if ( code == EVENT_DOUBLECLICK ) {
		loadFromGame((CListBoxWindow*)pWnd);
	} else if (code == EVENT_DRAWWND) {
		int pos = ((CListBoxWindow*)pWnd)->GetCurSel();
		_shellIconManager.GetWnd(SQSH_MM_LOAD_IN_GAME_GO_BTN)->Enable(pos != -1);
	}
}
void onMMLoadInGameGoButton(CShellWindow* pWnd, InterfaceEventCode code, int param) {
	if( code == EVENT_UNPRESSED && intfCanHandleInput() ) {
		loadFromGame((CListBoxWindow*)_shellIconManager.GetWnd(SQSH_MM_LOAD_IN_GAME_MAP_LIST));
	}		
}
void onMMDelLoadInGameButton(CShellWindow* pWnd, InterfaceEventCode code, int param) {
	if( code == EVENT_UNPRESSED && intfCanHandleInput() && param == VK_LBUTTON) {
		CListBoxWindow* list = (CListBoxWindow*)_shellIconManager.GetWnd(SQSH_MM_LOAD_IN_GAME_MAP_LIST);
		std::string text = qdTextDB::instance().getText("Interface.Menu.Messages.Confirmations.DeleteSave");
		std::string saveName = savedGames[list->GetCurSel()].missionName();
		char* mess = new char[text.length() + saveName.length()];
		sprintf(mess, text.c_str(), saveName.c_str());
		setupYesNoMessageBox(delLoadInGameSaveAction, 0, mess);
		delete [] mess;
		showMessageBox();
	} else if (code == EVENT_DRAWWND) {
		CListBoxWindow* list = (CListBoxWindow*)_shellIconManager.GetWnd(SQSH_MM_LOAD_IN_GAME_MAP_LIST);
		pWnd->Enable( list->GetCurSel() >= 0 );
	}
}

//save game
int delSaveGameSaveAction(float, float) {
	CListBoxWindow* list = (CListBoxWindow*)_shellIconManager.GetWnd(SQSH_MM_SAVE_GAME_MAP_LIST);
	gameShell->currentSingleProfile.deleteSave(savedGames[list->GetCurSel()].savePathContent());
    savedGames.clear();
    const std::string& savesDir = gameShell->currentSingleProfile.getSavesDirectory();
	loadMapVector(savedGames, savesDir, ".spg");
	fillList(SQSH_MM_SAVE_GAME_MAP_LIST, savedGames, SQSH_MM_SAVE_GAME_MAP, SQSH_MM_SAVE_GAME_MAP_DESCR_TXT);
	return 0;
}

int toSaveQuant( float, float ) {
	if (menuChangingDone) {
		_shellIconManager.SwitchMenuScreens( SQSH_MM_SAVE_GAME_SCR, SQSH_MM_INMISSION_SCR );
		return 0;
	}
	return 1;
}

int saveGame_(float i, float) {
    size_t ii = static_cast<size_t>(i);
	gameShell->currentSingleProfile.deleteSave(savedGames[ii].savePathContent());
	std::string saveName = gameShell->currentSingleProfile.getSavesDirectory() + savedGames[ii].missionName();
    bool user_save = !gameShell->missionEditor();
	if ( gameShell->universalSave(saveName.c_str(), user_save) ) {
		hideMessageBox();
		_shellIconManager.AddDynamicHandler( toSaveQuant, CBCODE_QUANT );
//		_shellIconManager.SwitchMenuScreens( SQSH_MM_SAVE_GAME_SCR, SQSH_MM_INMISSION_SCR );
	} else {
		setupOkMessageBox(
			nullptr,
			0,
			qdTextDB::instance().getText("Interface.Menu.Messages.DiskFull"),
			MBOX_BACK );

		showMessageBoxButtons();
	}
	
	return 1;
}

int saveGame(float i, float y) {
	return saveGame_(i, y);
}

void onMMSaveGameMapList(CShellWindow* pWnd, InterfaceEventCode code, int param) {
	if ( code == EVENT_PRESSED ) {
		CListBoxWindow* list = (CListBoxWindow*) pWnd;
		int pos = list->GetCurSel();
		if (pos >= 0 && pos < savedGames.size()) {
			setupMapDescWnd(pos, savedGames, SQSH_MM_SAVE_GAME_MAP, SQSH_MM_SAVE_GAME_MAP_DESCR_TXT, SQSH_MM_SAVE_NAME_INPUT);
//			CEditWindow* input = (CEditWindow*)_shellIconManager.GetWnd(SQSH_MM_SAVE_NAME_INPUT);
//			input->SetText(savedGames[pos].missionName());
//			((CShowMapWindow*)_shellIconManager.GetWnd(SQSH_MM_SAVE_GAME_MAP))->setWorldID( savedGames[pos].worldID() );
//			((CTextWindow*)_shellIconManager.GetWnd(SQSH_MM_SAVE_GAME_MAP_DESCR_TXT))->setText( savedGames[pos].missionDescription() );
		}
	} else if ( code == EVENT_DOUBLECLICK ) {
		CListBoxWindow* list = (CListBoxWindow*) pWnd;
		int pos = list->GetCurSel();
		if (pos != -1) {
			std::string text = qdTextDB::instance().getText("Interface.Menu.Messages.Confirmations.OverwriteSave");
			std::string saveName = savedGames[pos].missionName();
			char* mess = new char[text.length() + saveName.length()];
			sprintf(mess, text.c_str(), saveName.c_str());
			setupYesNoMessageBox(saveGame, pos, mess);
			delete [] mess;
			showMessageBox();
		}
	} else if (code == EVENT_DRAWWND) {
//		int pos = ((CListBoxWindow*)pWnd)->GetCurSel();
		CEditWindow* input = (CEditWindow*)_shellIconManager.GetWnd(SQSH_MM_SAVE_NAME_INPUT);
		_shellIconManager.GetWnd(SQSH_MM_SAVE_GAME_GO_BTN)->Enable(input->isValidFilename());
	}
}
void onMMSaveGameGoButton(CShellWindow* pWnd, InterfaceEventCode code, int param) {
	if( code == EVENT_UNPRESSED && intfCanHandleInput() ) {
		CListBoxWindow* list = (CListBoxWindow*)_shellIconManager.GetWnd(SQSH_MM_SAVE_GAME_MAP_LIST);
		CEditWindow* input = (CEditWindow*)_shellIconManager.GetWnd(SQSH_MM_SAVE_NAME_INPUT);
		size_t i, s = 0;
		for (i = 0, s = savedGames.size(); i < s; i++) {
			if (input->GetText() == savedGames[i].missionName()) {
				break;
			}
		}
		if (i != s) {
			std::string text = qdTextDB::instance().getText("Interface.Menu.Messages.Confirmations.OverwriteSave");
			std::string saveName = savedGames[i].missionName();
			char* mess = new char[text.length() + saveName.length()];
			sprintf(mess, text.c_str(), saveName.c_str());
			setupYesNoMessageBox(saveGame, i, mess);
			delete [] mess;
			showMessageBox();
		} else {
			std::string path = gameShell->currentSingleProfile.getSavesDirectory() + input->getText();
			bool user_save = !gameShell->missionEditor();
			if ( gameShell->universalSave(path.c_str(), user_save) ) {
				_shellIconManager.SwitchMenuScreens( pWnd->m_pParent->ID, SQSH_MM_INMISSION_SCR );
			} else {
				setupOkMessageBox(
					0,
					0,
					qdTextDB::instance().getText("Interface.Menu.Messages.DiskFull"),
					MBOX_BACK );

				showMessageBox();
			}
		}
	}		
}

void onMMDelSaveGameButton(CShellWindow* pWnd, InterfaceEventCode code, int param) {
	if( code == EVENT_UNPRESSED && intfCanHandleInput() && param == VK_LBUTTON) {
		CListBoxWindow* list = (CListBoxWindow*)_shellIconManager.GetWnd(SQSH_MM_SAVE_GAME_MAP_LIST);
		std::string text = qdTextDB::instance().getText("Interface.Menu.Messages.Confirmations.DeleteSave");
		std::string saveName = savedGames[list->GetCurSel()].missionName();
		char* mess = new char[text.length() + saveName.length()];
		sprintf(mess, text.c_str(), saveName.c_str());
		setupYesNoMessageBox(delSaveGameSaveAction, 0, mess);
		delete [] mess;
		showMessageBox();
	} else if (code == EVENT_DRAWWND) {
		CListBoxWindow* list = (CListBoxWindow*)_shellIconManager.GetWnd(SQSH_MM_SAVE_GAME_MAP_LIST);
		pWnd->Enable( list->GetCurSel() >= 0 );
	}
}

//save replay
int delSaveReplayAction(float, float) {
	CListBoxWindow* list = (CListBoxWindow*)_shellIconManager.GetWnd(SQSH_MM_SAVE_REPLAY_LIST);
    std::remove( replays[list->GetCurSel()].playReelPath().c_str() );
    replays.clear();
	loadMapVector(replays, REPLAY_PATH, "", true);
	fillReplayList(SQSH_MM_SAVE_REPLAY_LIST, replays, SQSH_MM_SAVE_REPLAY_MAP, SQSH_MM_SAVE_REPLAY_DESCR_TXT);
	return 0;
}
int toSaveReplayQuant( float, float ) {
	if (menuChangingDone) {
		_shellIconManager.SwitchMenuScreens( SQSH_MM_SAVE_REPLAY_SCR, SQSH_MM_STATS_SCR );
		return 0;
	}
	return 1;
}
int saveReplay(float i, float) {
	switch ( universe()->savePlayReel(replays[i].playReelPath().c_str()) ) {
		case terHyperSpace::SAVE_REPLAY_OK:
			hideMessageBox();
			_shellIconManager.AddDynamicHandler( toSaveReplayQuant, CBCODE_QUANT );
			break;
		case terHyperSpace::SAVE_REPLAY_RW_ERROR_OR_DISK_FULL:
			setupOkMessageBox(
				0,
				0,
				qdTextDB::instance().getText("Interface.Menu.Messages.DiskFull"),
				MBOX_BACK );

			showMessageBoxButtons();
			break;
		case terHyperSpace::SAVE_REPLAY_RW_ERROR:
			setupOkMessageBox(
				0,
				0,
				qdTextDB::instance().getText("Interface.Menu.Messages.RWError"),
				MBOX_BACK );

			showMessageBoxButtons();
			break;
	}
	return 1;
}
void onMMSaveReplayList(CShellWindow* pWnd, InterfaceEventCode code, int param) {
	if ( code == EVENT_PRESSED ) {
		CListBoxWindow* list = (CListBoxWindow*) pWnd;
		int pos = list->GetCurSel();
		if (pos >= 0 && pos < replays.size()) {
			setupReplayDescWnd(pos, replays, SQSH_MM_SAVE_REPLAY_MAP, SQSH_MM_SAVE_REPLAY_DESCR_TXT, SQSH_MM_REPLAY_NAME_INPUT);
//			CEditWindow* input = (CEditWindow*)_shellIconManager.GetWnd(SQSH_MM_REPLAY_NAME_INPUT);
//			input->SetText(replays[pos].missionNamePlayReelGame.c_str());
//			((CShowMapWindow*)_shellIconManager.GetWnd(SQSH_MM_SAVE_REPLAY_MAP))->setWorldID( replays[pos].worldID() );
//			((CTextWindow*)_shellIconManager.GetWnd(SQSH_MM_SAVE_REPLAY_DESCR_TXT))->setText( replays[pos].missionDescription() );
		}
	} else if ( code == EVENT_DOUBLECLICK ) {
		CListBoxWindow* list = (CListBoxWindow*) pWnd;
		int pos = list->GetCurSel();
		if (pos != -1) {
			std::string text = qdTextDB::instance().getText("Interface.Menu.Messages.Confirmations.OverwriteReplay");
			std::string replayName = replays[pos].missionNamePlayReelGame;
			char* mess = new char[text.length() + replayName.length()];
			sprintf(mess, text.c_str(), replayName.c_str());
			setupYesNoMessageBox(saveReplay, pos, mess);
			delete [] mess;
			showMessageBox();
		}
	} else if (code == EVENT_DRAWWND) {
//		int pos = ((CListBoxWindow*)pWnd)->GetCurSel();
		CEditWindow* input = (CEditWindow*)_shellIconManager.GetWnd(SQSH_MM_REPLAY_NAME_INPUT);
		_shellIconManager.GetWnd(SQSH_MM_SAVE_REPLAY_GO_BTN)->Enable(input->isValidFilename());
	}
}
void onMMSaveReplayGoButton(CShellWindow* pWnd, InterfaceEventCode code, int param) {
	if( code == EVENT_UNPRESSED && intfCanHandleInput() ) {
		CListBoxWindow* list = (CListBoxWindow*)_shellIconManager.GetWnd(SQSH_MM_SAVE_REPLAY_LIST);
		CEditWindow* input = (CEditWindow*)_shellIconManager.GetWnd(SQSH_MM_REPLAY_NAME_INPUT);
		int i;
		int s;
		for (i = 0, s = replays.size(); i < s; i++) {
			if (strcmp(input->GetText(), replays[i].missionNamePlayReelGame.c_str()) == 0) {
				break;
			}
		}
		if (i != s) {
			std::string text = qdTextDB::instance().getText("Interface.Menu.Messages.Confirmations.OverwriteReplay");
			std::string replayName = replays[i].missionNamePlayReelGame;
			char* mess = new char[text.length() + replayName.length()];
			sprintf(mess, text.c_str(), replayName.c_str());
			setupYesNoMessageBox(saveReplay, i, mess);
			delete [] mess;
			showMessageBox();
		} else {
			std::string path = convert_path_content(REPLAY_PATH, true);
            create_directories(path);
			path += PATH_SEP + input->getText();

			switch ( universe()->savePlayReel(path.c_str()) ) {
				case terHyperSpace::SAVE_REPLAY_OK:
					_shellIconManager.SwitchMenuScreens( pWnd->m_pParent->ID, SQSH_MM_STATS_SCR );
					break;
				case terHyperSpace::SAVE_REPLAY_RW_ERROR_OR_DISK_FULL:
					setupOkMessageBox(
						0,
						0,
						qdTextDB::instance().getText("Interface.Menu.Messages.DiskFull"),
						MBOX_BACK );

					showMessageBox();
					break;
				case terHyperSpace::SAVE_REPLAY_RW_ERROR:
					setupOkMessageBox(
						0,
						0,
						qdTextDB::instance().getText("Interface.Menu.Messages.RWError"),
						MBOX_BACK );

					showMessageBox();
					break;
			}
		}
	}		
}
void onMMDelSaveReplayButton(CShellWindow* pWnd, InterfaceEventCode code, int param) {
	if( code == EVENT_UNPRESSED && intfCanHandleInput() && param == VK_LBUTTON) {
		CListBoxWindow* list = (CListBoxWindow*)_shellIconManager.GetWnd(SQSH_MM_SAVE_REPLAY_LIST);
		std::string text = qdTextDB::instance().getText("Interface.Menu.Messages.Confirmations.DeleteReplay");
		std::string replayName = replays[list->GetCurSel()].missionNamePlayReelGame;
		char* mess = new char[text.length() + replayName.length()];
		sprintf(mess, text.c_str(), replayName.c_str());
		setupYesNoMessageBox(delSaveReplayAction, 0, mess);
		delete [] mess;
		showMessageBox();
	} else if (code == EVENT_DRAWWND) {
		CListBoxWindow* list = (CListBoxWindow*)_shellIconManager.GetWnd(SQSH_MM_SAVE_REPLAY_LIST);
		pWnd->Enable( list->GetCurSel() >= 0 );
	}
}

//credits
void onMMCreditsButton(CShellWindow* pWnd, InterfaceEventCode code, int param) {
	if( code == EVENT_UNPRESSED && intfCanHandleInput() ) {
		_shellIconManager.SwitchMenuScreens( pWnd->m_pParent->ID, SQSH_MM_CREDITS_SCR );
	}
}


//show task menu
void onMMTaskButton(CShellWindow* pWnd, InterfaceEventCode code, int param) {
	if( code == EVENT_UNPRESSED && intfCanHandleInput() ) {
//		CTextWindow* txtWnd = dynamic_cast<CTextWindow*>(_shellIconManager.GetWnd(SQSH_MM_MISSION_TASK_TXT));
//		txtWnd->SetText(gameShell->CurrentMission.missionDescription());
		gameShell->prepareForInGameMenu();
		_shellIconManager.SwitchMenuScreens( -1, SQSH_MM_MISSION_TASK_SCR );
	}
}

void HistoryScene::executeMission(const std::string& fileName) {
	setMissionNumberToExecute(interpreter->findMission(fileName));
	CShellWindow* wnd = _shellIconManager.GetWnd(SQSH_MM_SKIP_BRIEFING_BTN);
	if (!wnd->isVisible()) {
		if (historyScene.getMissionNumberToExecute() < gameShell->currentSingleProfile.getLastMissionNumber()) {
			_shellIconManager.SwitchMenuScreens( SHOW_START_SKIP_MISSION, SHOW_START_SKIP_MISSION );
		} else {
			_shellIconManager.SwitchMenuScreens( SHOW_START_MISSION, SHOW_START_MISSION );
		}
	} else {
		if (historyScene.getMissionNumberToExecute() < gameShell->currentSingleProfile.getLastMissionNumber()) {
			wnd->Show(0);
			_shellIconManager.GetWnd(SQSH_MM_SKIP_BRIEFING_BORDER)->Show(0);

			_shellIconManager.GetWnd(SQSH_MM_SKIP_MISSION_BTN)->Show(1);
			_shellIconManager.GetWnd(SQSH_MM_SKIP_MISSION_BTN)->OnEffectStop(effectButtonsFadeIn);
			_shellIconManager.GetWnd(SQSH_MM_SKIP_MISSION_BORDER)->Show(1);
			_shellIconManager.GetWnd(SQSH_MM_SKIP_MISSION_BORDER)->OnEffectStop(effectButtonsFadeIn);

			_shellIconManager.SwitchMenuScreens( SHOW_START_MISSION, SHOW_START_MISSION );
		} else {
			_shellIconManager.SwitchMenuScreens( SHOW_START_HIDE_SKIP_MISSION, SHOW_START_HIDE_SKIP_MISSION );
		}
	}
}
void HistoryScene::showPaused() {
	if (_shellIconManager.GetWnd(SQSH_MM_SKIP_BRIEFING_BTN)->isVisible()) {
		_shellIconManager.SwitchMenuScreens( SHOW_CONTINUE, SHOW_CONTINUE );
	} else {
		_shellIconManager.SwitchMenuScreens( SHOW_SKIP_CONTINUE, SHOW_SKIP_CONTINUE );
	}
}
void HistoryScene::showText(const std::string& text, const std::string& icon) {
	CTextWindow* wnd = (CTextWindow*)_shellIconManager.GetWnd(SQSH_MM_BRIEFING_TXT);
	if (wnd) {
		wnd->setText( qdTextDB::instance().getText(text.c_str()) );
	}
	CPortraitWindow* btn = (CPortraitWindow*)_shellIconManager.GetWnd(SQSH_MM_BRIEFING_ICON);
	if (btn) {
		btn->setTexture( icon.c_str() );
	}
}
void HistoryScene::hideText() {
	CTextWindow* wnd = dynamic_cast<CTextWindow*>(_shellIconManager.GetWnd(SQSH_MM_BRIEFING_TXT));
	if (wnd) {
		wnd->setText("");
	}
	CPortraitWindow* btn = (CPortraitWindow*)_shellIconManager.GetWnd(SQSH_MM_BRIEFING_ICON);
	if (btn) {
		btn->setTexture( "" );
	}
}

void HistoryScene::finishHistory() {
	historyScene.stop();
//	gb_Music.FadeVolume(_fEffectButtonTotalTime*0.001f);
	_shellIconManager.SwitchMenuScreens(SQSH_MM_BRIEFING_SCR, SQSH_MM_CREDITS_SCR);
}

void CShellLogicDispatcher::OnInterfaceMessage(int id, bool fromTrigger) {
	if (gameShell->currentSingleProfile.getLastGameType() == UserSingleProfile::MULTIPLAYER || fromTrigger) {
		processInterfaceMessageLater((terUniverseInterfaceMessage)id);
	}
}

//////////////////////

