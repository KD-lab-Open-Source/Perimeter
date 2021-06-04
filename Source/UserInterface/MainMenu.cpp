
#include "StdAfx.h"
#include "MainMenu.h"
#include "Config.h"
#include "Runtime.h"
#include "Terra.h"
#include "GenericControls.h"
#include "Universe.h"
#include "UniverseInterface.h"
#include "GameShell.h"
#include "PerimeterShellUI.h"
#include "Controls.h"
#include "..\sound\PerimeterSound.h"
#include "GraphicsOptions.h"

#include "silicon.h"
#include "HistoryScene.h"
#include "BGScene.h"
#include "MonoSelect.h"
#include "..\ht\ht.h"
#include "qd_textdb.h"

#include "MessageBox.h"

extern char _bCursorVisible;
extern char _bMenuMode;

extern MpegSound gb_Music;
extern LogStream fout;
/////////////////////////////////////////////////////////////

int terShowTips = IniManager("Perimeter.ini").getInt("Game","ShowTips");

int _id_on;
int _id_off;

bool menuChangingDone;

HistoryScene historyScene("RESOURCE\\scenario.hst");
HistoryScene bwScene("RESOURCE\\menu.hst");
BGScene bgScene;

int currYear = -1;

vector<MissionDescription> multiplayerMaps;
vector<MissionDescription> savedGames;
vector<MissionDescription> replays;
MissionDescription missionToExec;

string defaultSaveName;


bool intfCanHandleInput() {
	return !bgScene.isPlaying() && _shellIconManager.isDynQueueEmpty();
}

string getOriginalMissionName(const string& originalSaveName) {
	string res = originalSaveName;
	res.erase(res.size() - 4, res.size()); 
	size_t pos = res.rfind("\\");
	if (pos != string::npos) {
		res.erase(0, pos + 1);
	}
	return res;
}

string getItemTextFromBase(const char *keyStr) {
	string key("Interface.Menu.ComboItems.");
	key += keyStr;
	const char* stringFromBase = qdTextDB::instance().getText(key.c_str());
	return (*stringFromBase) ? stringFromBase : "";
//	return (*stringFromBase) ? stringFromBase : keyStr;
}

terUniverseInterfaceMessage resultID = UNIVERSE_INTERFACE_MESSAGE_GAME_RESULT_UNDEFINED;

STARFORCE_API void processInterfaceMessage(terUniverseInterfaceMessage id, int wndIDToHide = -1) {
	if (_shellIconManager.isCutSceneMode()) {
		_shellIconManager.setCutSceneMode(false, false);
	}

	bool multiPlayer = (gameShell->currentSingleProfile.getLastGameType() == UserSingleProfile::LAN);
//	if (multiPlayer) {
//		gameShell->getNetClient()->FinishGame();
//	}

#ifdef _FINAL_VERSION_
	bool disableBack = 
			gameShell->currentSingleProfile.getLastGameType() != UserSingleProfile::BATTLE
		||	gameShell->currentSingleProfile.getGameResult() == UNIVERSE_INTERFACE_MESSAGE_GAME_DEFEAT
		||	(gameShell->currentSingleProfile.getGameResult() == UNIVERSE_INTERFACE_MESSAGE_GAME_RESULT_UNDEFINED && id == UNIVERSE_INTERFACE_MESSAGE_GAME_DEFEAT);

	_shellIconManager.GetWnd( SQSH_MM_RESUME_BTN )->Enable(!disableBack);
#else
	_shellIconManager.GetWnd( SQSH_MM_RESUME_BTN )->Enable(true);
#endif
	if (gameShell->currentSingleProfile.getGameResult() == UNIVERSE_INTERFACE_MESSAGE_GAME_RESULT_UNDEFINED) {
		if (universe()) {
			universe()->stopPlayReel();
		}
		switch (id)	{
			case UNIVERSE_INTERFACE_MESSAGE_GAME_VICTORY:
				{
					if (wndIDToHide == -1) {
						gameShell->cancelMouseLook();
					}
					if (
								gameShell->currentSingleProfile.getLastGameType() == UserSingleProfile::SCENARIO
							&&	(		gameShell->currentSingleProfile.getLastMissionNumber() == 0
									||	historyScene.getMissionNumberToExecute() == gameShell->currentSingleProfile.getLastMissionNumber() )
	//						&&	historyScene.missionCount() > (gameShell->currentSingleProfile.getLastMissionNumber() + 1)
						) {

						gameShell->currentSingleProfile.setLastMissionNumber(gameShell->currentSingleProfile.getLastMissionNumber() + 1);
					}
								
					gameShell->currentSingleProfile.setGameResult(id);
					gameShell->currentSingleProfile.setLastWin(true);

					if (multiPlayer) {
						_shellIconManager.hideAllVisibleMenuScr();
					}

					gameShell->prepareForInGameMenu();

					_shellIconManager.playMusic("RESOURCE\\MUSIC\\victory.ogg");
//					SND2DPlaySound("victory");

					CTextWindow *Wnd = (CTextWindow*)_shellIconManager.GetWnd( SQSH_MM_RESULT_TXT );
					CScaleResultButton *rWnd = (CScaleResultButton*)_shellIconManager.GetWnd( SQSH_RESULT_WND );
					if (gameShell->currentSingleProfile.getLastGameType() == UserSingleProfile::SURVIVAL) {
						const char* origName = gameShell->CurrentMission.originalSaveName;
						xassert(origName);
						string keyName;
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

			case UNIVERSE_INTERFACE_MESSAGE_GAME_DEFEAT:
				{
					if (wndIDToHide == -1) {
						gameShell->cancelMouseLook();
					}
					gameShell->currentSingleProfile.setLastWin(
							gameShell->currentSingleProfile.getLastGameType() == UserSingleProfile::SCENARIO
						&&	historyScene.getMissionNumberToExecute() != gameShell->currentSingleProfile.getLastMissionNumber()
					);
					gameShell->currentSingleProfile.setGameResult(id);

					if (multiPlayer) {
						_shellIconManager.hideAllVisibleMenuScr();
					}

					gameShell->prepareForInGameMenu();

					_shellIconManager.playMusic("RESOURCE\\MUSIC\\defeat.ogg");
//					SND2DPlaySound("defeat");

					CTextWindow *Wnd = (CTextWindow*)_shellIconManager.GetWnd( SQSH_MM_RESULT_TXT );
					CScaleResultButton *rWnd = (CScaleResultButton*)_shellIconManager.GetWnd( SQSH_RESULT_WND );
					if (gameShell->currentSingleProfile.getLastGameType() == UserSingleProfile::SURVIVAL) {
						const char* origName = gameShell->CurrentMission.originalSaveName;
						xassert(origName);
						string keyName;
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
	} else {
		if (wndIDToHide == -1) {
			gameShell->cancelMouseLook();
		}
		gameShell->prepareForInGameMenu();
		_shellIconManager.SwitchMenuScreens(wndIDToHide, SQSH_MM_ENDMISSION_SCR);
	}
}

int goToResultQuant( float, float ) {
	if (!gameShell->GameActive) {
		return 0;
	} else if (menuChangingDone) {
		processInterfaceMessage(resultID);
		return 0;
	}
	return 1;
}

STARFORCE_API void processInterfaceMessageLater(terUniverseInterfaceMessage id, int wndIDToHide = -1) {
	if (gameShell->currentSingleProfile.getLastGameType() == UserSingleProfile::LAN) {
		gameShell->getNetClient()->FinishGame();
		resultID = id;
		_shellIconManager.AddDynamicHandler( goToResultQuant, CBCODE_QUANT );
	} else {
		processInterfaceMessage(id, wndIDToHide);
	}
}

STARFORCE_API void loadMapVector(vector<MissionDescription>& mapVector, const string& path, const string& mask, bool replay) {
	//fill map list
	mapVector.clear();
	WIN32_FIND_DATA FindFileData;
	HANDLE hf = FindFirstFile( mask.c_str(), &FindFileData );
	if(hf != INVALID_HANDLE_VALUE){
		do{
			if(FindFileData.nFileSizeLow){
				MissionDescription mission;
				string name = path + FindFileData.cFileName;
				mission.setSaveName(name.c_str());
				mission.setReelName(name.c_str());
//				mission.gameType_ = replay ? MissionDescription::GT_playRellGame : MissionDescription::GT_SPGame;
				if( (!replay) || isCorrectPlayReelFile(mission.fileNamePlayReelGame.c_str()))
					mapVector.push_back(mission);
//				MissionDescription mission((string(path) + FindFileData.cFileName).c_str(), replay ? MissionDescription::GT_playRellGame : MissionDescription::GT_SPGame);
//				if(mission.worldID() != -1)
//					mapVector.push_back(mission);
			}
		} while(FindNextFile( hf, &FindFileData ));
		FindClose( hf );
	}
}
void checkMissionDescription(int index, vector<MissionDescription>& mVect) {
	if (mVect[index].worldID() == -1) {
		mVect[index] = MissionDescription(mVect[index].saveName());
	}
}
void checkReplayMissionDescription(int index, vector<MissionDescription>& mVect) {
	if (mVect[index].worldID() == -1) {
		mVect[index] = MissionDescription(mVect[index].fileNamePlayReelGame.c_str(), GT_playRellGame);
	}
}
void setupMapDescWnd(int index, vector<MissionDescription>& mVect, int mapWndID, int mapDescrWndID, int inputWndID) {
	checkMissionDescription(index, mVect);
	if (mapWndID != -1) {
		((CShowMapWindow*)_shellIconManager.GetWnd(mapWndID))->setWorldID( mVect[index].worldID() );
	}
	if (mapDescrWndID != -1) {
		((CTextWindow*)_shellIconManager.GetWnd(mapDescrWndID))->setText( mVect[index].missionDescription() );
	}
	if (inputWndID != -1) {
		CEditWindow* input = (CEditWindow*)_shellIconManager.GetWnd(inputWndID);
		input->SetText(mVect[index].missionName());
	}
}
void setupReplayDescWnd(int index, vector<MissionDescription>& mVect, int mapWndID, int mapDescrWndID, int inputWndID = -1) {
	checkReplayMissionDescription(index, mVect);
	((CShowMapWindow*)_shellIconManager.GetWnd(mapWndID))->setWorldID( mVect[index].worldID() );
	((CTextWindow*)_shellIconManager.GetWnd(mapDescrWndID))->setText( mVect[index].missionDescription() );
	if (inputWndID != -1) {
		CEditWindow* input = (CEditWindow*)_shellIconManager.GetWnd(inputWndID);
		input->SetText(mVect[index].missionNamePlayReelGame.c_str());
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
STARFORCE_API void fillList(int listID, vector<MissionDescription>& mVect, int mapWndID, int mapDescrWndID, int inputWndID) {
	CListBoxWindow* list = (CListBoxWindow*)_shellIconManager.GetWnd(listID);
	list->NewItem(1);
	list->Clear();
	int i;
	int s;
	for (i = 0, s = mVect.size(); i < s; i++) {
		list->AddString( mVect[i].missionName(), 0 );
	}
	if (s) {
		list->SetCurSel(0);
		setupMapDescWnd(0, mVect, mapWndID, mapDescrWndID, inputWndID);
	} else {
		list->SetCurSel(-1);
		clearMapDescWnd(mapWndID, mapDescrWndID, inputWndID);
	}
}
STARFORCE_API void fillMultiPlayerList(int listID = SQSH_MM_LAN_MAP_LIST, int mapWndID = SQSH_MM_CREATE_GAME_MAP, int mapDescrWndID = SQSH_MM_CREATE_GAME_MAP_DESCR_TXT) {
	CListBoxWindow* list = (CListBoxWindow*)_shellIconManager.GetWnd(listID);
	list->NewItem(1);
	list->Clear();
	#ifndef _DEMO_
		int i;
		int s;
		for (i = 0, s = multiplayerMaps.size(); i < s; i++) {
			string name = "MapNames.";
			name += multiplayerMaps[i].missionName();
			name = qdTextDB::instance().getText(name.c_str());
			if (name.empty()) {
				name = multiplayerMaps[i].missionName();
			}
			list->AddString(name.c_str(), 0 );
		}
	#else
		int s = 1;
		string name = "MapNames.";
		name += multiplayerMaps[0].missionName();
		name = qdTextDB::instance().getText(name.c_str());
		if (name.empty()) {
			name = multiplayerMaps[0].missionName();
		}
		list->AddString(name.c_str(), 0 );
	#endif
	if (s) {
		list->SetCurSel(0);
		setupMapDescWnd(0, multiplayerMaps, mapWndID, mapDescrWndID, -1);
	} else {
		list->SetCurSel(-1);
		clearMapDescWnd(mapWndID, mapDescrWndID, -1);
	}
}
STARFORCE_API void fillReplayList(int listID, vector<MissionDescription>& mVect, int mapWndID, int mapDescrWndID, int inputWndID = -1) {
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
void prepareHeadList(int listID, const vector<string>& mVect, float* weights) {
	CListBoxWindow* list = (CListBoxWindow*)_shellIconManager.GetWnd(listID);
	list->NewItem(mVect.size());
	list->Clear();
	for (int i = 0, s = mVect.size(); i < s; i++) {
		list->GetItem(i).x = weights[i];
		list->AddString( mVect[i].c_str(), i );
	}
	list->SetCurSel(0);
}
void fillColumnStatsList(int listID, int columnNumber, const vector<string>& mVect) {
	CListBoxWindow* list = (CListBoxWindow*)_shellIconManager.GetWnd(listID);
	for (int i = 0, s = mVect.size(); i < s; i++) {
		list->AddString( mVect[i].c_str(), columnNumber );
	}
}
void fillRowStatsListWindow(int listID, int rowNumber, const vector<string>& mVect, int race, const sColor4c& clr) {
	CStatListBoxWindow* list = (CStatListBoxWindow*)_shellIconManager.GetWnd(listID);
	list->AddRace( race, clr );
	for (int i = 0, s = mVect.size(); i < s; i++) {
		list->AddString( mVect[i].c_str(), i + 1 );
	}
}

STARFORCE_API void fillStatsLists() {
	char buffer[30 + 1];

	vector<string> temp;

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

	vector<terPlayer*>& players = universe()->Players;
	terPlayer* player;
	for (int i = 0; i < NETWORK_PLAYERS_MAX; i++) {
		int playerID = gameShell->CurrentMission.playersData[i].playerID;
		if (
				( gameShell->CurrentMission.playersData[i].realPlayerType == REAL_PLAYER_TYPE_PLAYER || gameShell->CurrentMission.playersData[i].realPlayerType == REAL_PLAYER_TYPE_AI )
			&&	playerID >= 0
			&&	playerID < universe()->Players.size()
			&& !(player = universe()->findPlayer(playerID))->isWorld()) {

//			terPlayer* player = universe()->findPlayer(playerID);
			const PlayerStats& stats = player->getStats();
			sColor4c clr = sColor4c(player->unitColor());
			int race;
			switch (player->belligerent()) {
				case BELLIGERENT_EXODUS0:
				case BELLIGERENT_EXODUS1:
					race = 1;
					break;
				case BELLIGERENT_HARKBACKHOOD0:
				case BELLIGERENT_HARKBACKHOOD1:
					race = 2;
					break;
				case BELLIGERENT_EMPIRE0:
				case BELLIGERENT_EMPIRE1:
				default:
					race = 0;
			}

			int time = gameShell->gameTimer();

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
			sprintf(buffer, "%d%%", round(stats.getMedEfficiency() * 100));
			temp.push_back(buffer);
			sprintf(buffer, "%d", stats.getScourgeKilled());
			temp.push_back(buffer);
			sprintf(buffer, "%d", round(stats.getEnergy()));
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

inline char* GetFileExtension(char *fname)
{
	if( fname==0 ) return 0;
	int l = strlen(fname);
	for(int i=l-1; i>=0; i-- )
		if( fname[i]==0 || fname[i]=='/' ) return 0;
		else if( fname[i]=='.' ) return &fname[i+1];
	return 0;
}

STARFORCE_API void StartSpace() {
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

void showStartMissionButton() {
	CShellWindow* wnd = _shellIconManager.GetWnd(SQSH_MM_START_MISSION_BTN);
	wnd->Show(1);
	_shellIconManager.Effect(effectButtonsFadeIn, wnd); //запустить разлет
//	bgScene.markToPlay(wnd->m_attr->bgObject.name, wnd->m_attr->bgObject.chainName, true);

	_shellIconManager.GetWnd(SQSH_MM_START_BRIEFING_BORDER)->Show(1);
	_shellIconManager.Effect(effectButtonsFadeIn, _shellIconManager.GetWnd(SQSH_MM_START_BRIEFING_BORDER)); //запустить разлет
}

STARFORCE_API void fillProfileList() {
	CListBoxWindow* list = (CListBoxWindow*)_shellIconManager.GetWnd(SQSH_MM_PROFILE_LIST);
	list->NewItem(1);
	list->Clear();
	const vector<Profile>& profiles = gameShell->currentSingleProfile.getProfilesVector();
	for (int i = 0, s = profiles.size(); i < s; i++) {
		list->AddString( profiles[i].name.c_str(), 0 );
	}
	if (gameShell->currentSingleProfile.getCurrentProfileIndex() != -1) {
		list->SetCurSel(gameShell->currentSingleProfile.getCurrentProfileIndex());
	} else {
		list->SetCurSel(list->GetItemCount() - 1);
	}
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
	//					if (_id_off == SQSH_MM_LOBBY_SCR) {
	//						gameShell->getNetClient()->StartLoadTheGame();
	//					} else {
	//						_shellIconManager.ClearQueue();
							HTManager::instance()->GameStart(missionToExec);
							switch (gameShell->currentSingleProfile.getLastGameType()) {
								case UserSingleProfile::SCENARIO:
									gameShell->currentSingleProfile.setCurrentMissionNumber(historyScene.getMissionNumberToExecute());
									break;
								case UserSingleProfile::BATTLE:
									gameShell->currentSingleProfile.setCurrentMissionNumber(-1);
									break;
								case UserSingleProfile::SURVIVAL:
									gameShell->currentSingleProfile.setCurrentMissionNumber(-2);
									break;
								default:
									gameShell->currentSingleProfile.setCurrentMissionNumber(-1);
							}
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
				case SKIP_MISSION:
					{
						historyScene.getController()->eventOccured(Controller::MISSION_VICTORY_EVENT);
					}
					break;
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
				case SQSH_MM_SINGLE_SCR:
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
					#ifdef _FINAL_VERSION_
						bool disableBack = gameShell->currentSingleProfile.getLastGameType() != UserSingleProfile::BATTLE || gameShell->currentSingleProfile.getGameResult() == UNIVERSE_INTERFACE_MESSAGE_GAME_DEFEAT;
						_shellIconManager.GetWnd( SQSH_MM_RESUME_BTN )->Show(!disableBack);
						_shellIconManager.GetWnd( SQSH_MM_RESUME_BORDER )->Show(!disableBack);
					#else
						_shellIconManager.GetWnd( SQSH_MM_RESUME_BTN )->Show(true);
						_shellIconManager.GetWnd( SQSH_MM_RESUME_BORDER )->Show(true);
					#endif
					}
					break;
				case SQSH_MM_STATS_SCR:
					{
						if (!debug_allow_replay) {
							_shellIconManager.GetWnd(SQSH_MM_SAVE_REPLAY_BORDER)->Show(0);
							_shellIconManager.GetWnd(SQSH_MM_SAVE_REPLAY_BTN)->Show(0);
						}
						#ifndef _MULTIPLAYER_DEMO_
							bool showCont = 
										gameShell->currentSingleProfile.getLastGameType() == UserSingleProfile::SCENARIO
									&&	gameShell->currentSingleProfile.isLastWin();
						#else
							bool showCont = false;
						#endif

						_shellIconManager.GetWnd(SQSH_MM_CONTINUE_FROM_STATS_BTN)->Show(showCont);
						_shellIconManager.GetWnd(SQSH_MM_CONTINUE_FROM_STATS_BORDER)->Show(showCont);

						_shellIconManager.GetWnd(SQSH_MM_RESTART_BTN)->Enable(
								gameShell->currentSingleProfile.getLastGameType() != UserSingleProfile::LAN );

						CShellWindow* replayBtn = _shellIconManager.GetWnd(SQSH_MM_SAVE_REPLAY_BTN);
						if (replayBtn) {
							replayBtn->Enable( gameShell->currentSingleProfile.getLastGameType() != UserSingleProfile::REPLAY );
						}

						#ifdef _DEMO_
							if (replayBtn) {
								replayBtn->Enable(false);
							}
						#endif

						hideTables();
						_shellIconManager.GetWnd(SQSH_MM_STATS_TOTAL_RAMKA)->Show(0);
						_shellIconManager.GetWnd(SQSH_MM_STATS_TOTAL_HEAD_LIST)->Show(1);
						_shellIconManager.GetWnd(SQSH_MM_STATS_TOTAL_LIST)->Show(1);
						fillStatsLists();
					}
					break;
				case SQSH_MM_LAN_SCR:
					_shellIconManager.GetWnd(SQSH_MM_LAN_GAMESPY_LOGO)->Show(gameShell->getNetClient()->workMode == PNetCenter::PNCWM_ONLINE_GAMESPY);
					break;
				case SQSH_MM_CREATE_GAME_SCR:
					_shellIconManager.GetWnd(SQSH_MM_LAN_CREATE_GAMESPY_LOGO)->Show(gameShell->getNetClient()->workMode == PNetCenter::PNCWM_ONLINE_GAMESPY);
					break;
				case SQSH_MM_LOBBY_SCR:
					_shellIconManager.GetWnd(SQSH_MM_LAN_LOBBY_GAMESPY_LOGO)->Show(gameShell->getNetClient()->workMode == PNetCenter::PNCWM_ONLINE_GAMESPY);
					break;
				case SQSH_MM_NAME_INPUT_SCR:
					_shellIconManager.GetWnd(SQSH_MM_CONNECTION_TYPE_COMBO)->Show(debug_allow_mainmenu_gamespy);
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
				if (_id_off == SQSH_MM_LOBBY_SCR && _id_on != SQSH_MM_LOADING_MISSION_SCR) {
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
			if (!gb_Music.IsPlay()) {
				PlayMusic( mainMenuMusic );
			}
			switch (_id_on) {
				case SQSH_MM_START_SCR:
					{
						PlayMusic( mainMenuMusic );
						#ifdef _SINGLE_DEMO_
							_shellIconManager.GetWnd(SQSH_MM_LAN_BTN)->Enable(false);
							_shellIconManager.GetWnd(SQSH_MM_ONLINE_BTN)->Enable(false);
						#endif
						#ifdef _MULTIPLAYER_DEMO_
							((CShellPushButton*)_shellIconManager.GetWnd(SQSH_MM_SINGLE_BTN))->labelText = "TUTORIAL";
						#endif
						CShellWindow* pWnd = _shellIconManager.GetWnd(SQSH_MM_SPLASH4);
						if(pWnd && (pWnd->state & SQSH_VISIBLE)) {
							pWnd->Show(0);
						}
//						gameShell->currentSingleProfile.scanProfiles();
						loadBattleList();
						if(multiplayerMaps.empty()){
							loadMapVector(multiplayerMaps, "RESOURCE\\MULTIPLAYER\\", "RESOURCE\\MULTIPLAYER\\*.spg");
						}
						gameShell->stopNetClient();
//						_shellCursorManager.SetActiveCursor(CShellCursorManager::arrow, 1);	
						historyScene.stop();
						StartSpace();
						historyScene.done();
					}
					break;
				case SQSH_MM_SINGLE_SCR:
					#ifdef _DEMO_
						_shellIconManager.GetWnd(SQSH_MM_LOAD_BTN)->Enable(false);
						_shellIconManager.GetWnd(SQSH_MM_REPLAY_BTN)->Enable(false);
					#endif
					historyScene.stop();
					StartSpace();
					historyScene.done();
					break;
				case SQSH_MM_LAN_SCR:
					{
						gameShell->getNetClient()->StartFindHost();
						string name = getStringFromReg(mainCurrUserRegFolder, regLanName);
						CEditWindow* input = (CEditWindow*)_shellIconManager.GetWnd(SQSH_MM_LAN_PLAYER_NAME_INPUT);
						if (!name.empty()) {
							input->SetText(name.c_str());
						} else {
							input->SetText(qdTextDB::instance().getText("Interface.Menu.EmptyName.NewPlayer"));
						}
						historyScene.stop();
						StartSpace();
						historyScene.done();
					}
					break;
				case SQSH_MM_PROFILE_SCR:
					{
						fillProfileList();
						CEditWindow* input = (CEditWindow*)_shellIconManager.GetWnd(SQSH_MM_PROFILE_NAME_INPUT);
						if (input->getText().empty()) {
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

						currYear = historyScene.getController()->getCurrentYear();

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

						#ifdef _DEMO_
							int lastWinnedMissionNumber = 1;
						#else
							int lastWinnedMissionNumber = gameShell->currentSingleProfile.getLastMissionNumber();
							if (lastWinnedMissionNumber == historyScene.missionCount()) {
								lastWinnedMissionNumber--;
							}
						#endif

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
//						loadMapVector(savedGames, "RESOURCE\\SAVES\\", "RESOURCE\\SAVES\\*.spg");
						const string& savesDir = gameShell->currentSingleProfile.getSavesDirectory();
						loadMapVector(savedGames, savesDir, savesDir + "*.spg");
						StartSpace();
						fillList(SQSH_MM_LOAD_MAP_LIST, savedGames, SQSH_MM_LOAD_MAP, SQSH_MM_LOAD_MAP_DESCR_TXT);
					}
					break;
				case SQSH_MM_LOAD_REPLAY_SCR:
					{
						loadMapVector(replays, (string(REPLAY_PATH) + "\\").c_str(), (string(REPLAY_PATH) + "\\*.*").c_str(), true);
						StartSpace();
						fillReplayList(SQSH_MM_LOAD_REPLAY_LIST, replays, SQSH_MM_LOAD_REPLAY_MAP, SQSH_MM_LOAD_REPLAY_DESCR_TXT);
					}
					break;
				case SQSH_MM_SAVE_REPLAY_SCR:
					{
						loadMapVector(replays, (string(REPLAY_PATH) + "\\").c_str(), (string(REPLAY_PATH) + "\\*.*").c_str(), true);
//						StartSpace();
						CEditWindow* input = (CEditWindow*)_shellIconManager.GetWnd(SQSH_MM_REPLAY_NAME_INPUT);
//						if (input->getText().empty()) {
							input->SetText(gameShell->CurrentMission.worldName);
//						}
						fillReplayList(SQSH_MM_SAVE_REPLAY_LIST, replays, SQSH_MM_SAVE_REPLAY_MAP, SQSH_MM_SAVE_REPLAY_DESCR_TXT);
					}
					break;
				case SQSH_MM_LOAD_IN_GAME_SCR:
					{
//						loadMapVector(savedGames, "RESOURCE\\SAVES\\", "RESOURCE\\SAVES\\*.spg");
						const string& savesDir = gameShell->currentSingleProfile.getSavesDirectory();
						loadMapVector(savedGames, savesDir, savesDir + "*.spg");
//						StartSpace();
						fillList(SQSH_MM_LOAD_IN_GAME_MAP_LIST, savedGames, SQSH_MM_LOAD_IN_GAME_MAP, SQSH_MM_LOAD_IN_GAME_MAP_DESCR_TXT);
					}
					break;
				case SQSH_MM_SAVE_GAME_SCR:
					{
//						loadMapVector(savedGames, "RESOURCE\\SAVES\\", "RESOURCE\\SAVES\\*.spg");
						const string& savesDir = gameShell->currentSingleProfile.getSavesDirectory();
						loadMapVector(savedGames, savesDir, savesDir + "*.spg");
						CEditWindow* input = (CEditWindow*)_shellIconManager.GetWnd(SQSH_MM_SAVE_NAME_INPUT);
//						if (input->getText().empty()) {
							input->SetText(gameShell->CurrentMission.worldName);
//						}

//						StartSpace();
						fillList(SQSH_MM_SAVE_GAME_MAP_LIST, savedGames, SQSH_MM_SAVE_GAME_MAP, SQSH_MM_SAVE_GAME_MAP_DESCR_TXT);
					}
					break;
				case SQSH_MM_LOBBY_SCR:
					{
						StartSpace();
						if(multiplayerMaps.empty()){
							loadMapVector(multiplayerMaps, "RESOURCE\\MULTIPLAYER\\", "RESOURCE\\MULTIPLAYER\\*.spg");
						}
						fillMultiPlayerList();
						fillMultiPlayerList(SQSH_MM_LOBBY_MAP_LIST, -1, -1);
//						fillList(SQSH_MM_LAN_MAP_LIST, multiplayerMaps, SQSH_MM_CREATE_GAME_MAP, SQSH_MM_CREATE_GAME_MAP_DESCR_TXT);
						((ChatWindow*)_shellIconManager.GetWnd(SQSH_MM_LOBBY_CHAT_TEXT))->Clear();
					}
					break;
				case SQSH_MM_CREATE_GAME_SCR:
					{
						StartSpace();
						if(multiplayerMaps.empty()){
							loadMapVector(multiplayerMaps, "RESOURCE\\MULTIPLAYER\\", "RESOURCE\\MULTIPLAYER\\*.spg");
						}
						fillMultiPlayerList();
//						fillList(SQSH_MM_LAN_MAP_LIST, multiplayerMaps, SQSH_MM_CREATE_GAME_MAP, SQSH_MM_CREATE_GAME_MAP_DESCR_TXT);
					}
					break;
				case SQSH_MM_LOADING_MISSION_SCR:
					{

						CTextWindow* txtWnd = (CTextWindow*)_shellIconManager.GetWnd(SQSH_MM_MISSION_DESCR_TXT);

						switch(gameShell->currentSingleProfile.getLastGameType()) {
							case UserSingleProfile::BATTLE:
								txtWnd->setText( qdTextDB::instance().getText("Interface.Menu.Messages.Battle") );
								break;
							case UserSingleProfile::SURVIVAL:
								txtWnd->setText( qdTextDB::instance().getText("Interface.Menu.Messages.Survival") );
								break;
							default:
								txtWnd->SetText(missionToExec.missionDescription());
						}
						
						CShowMapWindow* mapWnd = (CShowMapWindow*)_shellIconManager.GetWnd(SQSH_MM_MAPWINDOW);
						mapWnd->setWorldID(missionToExec.worldID());

						CLogoWindow* logoWnd = (CLogoWindow*)_shellIconManager.GetWnd(SQSH_MM_LOADING_NOMAD_ICON);
						txtWnd = (CTextWindow*)_shellIconManager.GetWnd(SQSH_MM_LOADING_NOMAD_TXT);

						bgScene.setSkinColor();
						Frame* frame = historyScene.getNomadFrame();
						if (gameShell->currentSingleProfile.getLastGameType() == UserSingleProfile::SCENARIO && frame) {
							txtWnd->setText(HistoryScene::getFrameNameFromBase(frame->getName()));
							txtWnd->colorIndex = frame->getColorIndex();
							logoWnd->setRace(frame->getRace());
						} else {
							switch (missionToExec.getActivePlayerData().belligerent) {
								case BELLIGERENT_EXODUS0:
								case BELLIGERENT_EXODUS1:
									logoWnd->setRace(1);
									break;
								case BELLIGERENT_HARKBACKHOOD0:
								case BELLIGERENT_HARKBACKHOOD1:
									logoWnd->setRace(2);
									break;
								case BELLIGERENT_EMPIRE0:
								case BELLIGERENT_EMPIRE1:
								default:
									logoWnd->setRace(0);
							}
							txtWnd->setText(missionToExec.getActivePlayerData().name());
							txtWnd->colorIndex = missionToExec.getActivePlayerData().colorIndex;
						}
					
					}
					break;
				case SQSH_MM_NAME_INPUT_SCR:
					{
						string name = getStringFromReg(mainCurrUserRegFolder, regLanName);
						CEditWindow* input = (CEditWindow*)_shellIconManager.GetWnd(SQSH_MM_PLAYER_NAME_INPUT);
						if (!name.empty()) {
							input->SetText(name.c_str());
						} else {
							input->SetText("");
						}
						input = (CEditWindow*)_shellIconManager.GetWnd(SQSH_MM_IP_INPUT);
						if (input->getText().empty()) {
							input->SetText("127.0.0.1");
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
					_bCursorVisible = 0;
					SetCursor(0);
					bgScene.done();
					bwScene.done();
					historyScene.done();
					//only for preview
					#ifdef _DEMO_
						gameShell->reelAbortEnabled = lastDemoReel.abortEnabled;
						if (lastDemoReel.video) {
							gb_Music.SetVolume(0);
							gb_Music.Stop();
							gameShell->showReelModal(lastDemoReel.name, 0, lastDemoReel.localized);
						} else {
							gameShell->showPictureModal(lastDemoReel.name, lastDemoReel.localized, lastDemoReel.time);
						}
					#else
						//show last splash
						gameShell->reelAbortEnabled = lastReel.abortEnabled;
						if (lastReel.video) {
							gb_Music.SetVolume(0);
							gb_Music.Stop();
							gameShell->showReelModal(lastReel.name, 0, lastReel.localized);
						} else {
							gameShell->showPictureModal(lastReel.name, lastReel.localized, lastReel.time);
						}
					#endif
					gameShell->GameContinue = 0;
//					_shellIconManager.GetWnd(SQSH_MM_SPLASH_LAST)->Show(1);
//					_shellIconManager.SetModalWnd(SQSH_MM_SPLASH_LAST);
					break;
				case RESUME_GAME:
					//resume game
					_shellIconManager.LoadControlsGroup(SHELL_LOAD_GROUP_GAME);
					gameShell->resumeGame(true);
					_shellCursorManager.m_bShowSideArrows=1;
					_shellCursorManager.ShowCursor();
					_bMenuMode = 0;
					break;
				case CONTINUE_BRIEFING:
					//continue briefing after pause
					historyScene.getController()->eventOccured(Controller::CONTROL_SUBMIT_EVENT);
					historyScene.stopAudio();
					break;
				case READY:
					gameShell->getNetClient()->StartLoadTheGame();
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
	if (gameShell->BuildingInstaller.inited())
		gameShell->BuildingInstaller.CancelObject();
	gameShell->cancelMouseLook();

	menuChangingDone = false;
	_id_on = id_on;
	_id_off = id_off;
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
			_bCursorVisible = 1;
			_WaitCursor();
			_shellIconManager.SwitchMenuScreens(-1, SQSH_MM_START_SCR);
			_shellIconManager.SetModalWnd(0);
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
			_bCursorVisible = 1;
			_WaitCursor();
			_shellIconManager.SwitchMenuScreens(-1, SQSH_MM_START_SCR);
			_shellIconManager.SetModalWnd(0);
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
			_bCursorVisible = 1;
			_WaitCursor();
			_shellIconManager.SwitchMenuScreens(-1, SQSH_MM_START_SCR);
			_shellIconManager.SetModalWnd(0);
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
				_bCursorVisible = 1;
				_WaitCursor();
				_shellIconManager.SwitchMenuScreens(-1, SQSH_MM_START_SCR);
				_shellIconManager.SetModalWnd(0);

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
				nShow = SQSH_MM_START_SCR;
				break;
			case SQSH_MM_OPTIONS_SCR:
//				GraphOptionsManager::getInstance().reset();
//				extern void PerimeterDataChannelSave();
//				PerimeterDataChannelSave();
				nShow = SQSH_MM_START_SCR;
				break;
			case SQSH_MM_NAME_INPUT_SCR:
				nShow = SQSH_MM_START_SCR;
				break;
			case SQSH_MM_GAME_SCR:
			case SQSH_MM_SOUND_SCR:
				nShow = SQSH_MM_OPTIONS_SCR;
				break;
			case SQSH_MM_GRAPHICS_SCR:
				GraphOptionsManager::getInstance().reset();
				nShow = SQSH_MM_OPTIONS_SCR;
				break;
			case SQSH_MM_CREATE_GAME_SCR:
				nShow = SQSH_MM_LAN_SCR;
				break;
			case SQSH_MM_CREATE_ONLINE_GAME_SCR:
				nShow = SQSH_MM_ONLINE_SCR;
				break;
			case SQSH_MM_SCENARIO_SCR:
			case SQSH_MM_BATTLE_SCR:
			case SQSH_MM_LOAD_SCR:
			case SQSH_MM_LOAD_REPLAY_SCR:
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
			case SQSH_MM_CREDITS_SCR:
				nShow = SQSH_MM_START_SCR;
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
void onMMSingleButton(CShellWindow* pWnd, InterfaceEventCode code, int param) {
	if( code == EVENT_UNPRESSED && intfCanHandleInput() ) {
		#ifdef _MULTIPLAYER_DEMO_
			missionToExec = MissionDescription( ("RESOURCE\\MISSIONS\\" + historyScene.getMission(0).fileName).c_str() );
			missionToExec.getActivePlayerData().difficulty = DIFFICULTY_HARD;
			strncpy(missionToExec.getActivePlayerData().name_, "Demo", PERIMETER_CONTROL_NAME_SIZE);

			gameShell->currentSingleProfile.setLastGameType(UserSingleProfile::SCENARIO);

			gb_Music.FadeVolume(_fEffectButtonTotalTime*0.001f);
			_shellIconManager.SwitchMenuScreens(pWnd->m_pParent->ID, SQSH_MM_LOADING_MISSION_SCR);
		#else
			if (gameShell->currentSingleProfile.getCurrentProfileIndex() != -1) {
				showSingleMenu(pWnd);
			} else {
				_shellIconManager.SwitchMenuScreens(pWnd->m_pParent->ID, SQSH_MM_PROFILE_SCR);
			}
		#endif
	}
}
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
void switchToMultiplayerMenu(CShellWindow* pWnd, bool online) {
	_shellIconManager.GetWnd(SQSH_MM_LAN_PLAYER_NAME_INPUT)->Enable(!online);
	_shellIconManager.SwitchMenuScreens(pWnd->m_pParent->ID, SQSH_MM_LAN_SCR);
}

void onMMOnlineButton(CShellWindow* pWnd, InterfaceEventCode code, int param) {
#ifndef _SINGLE_DEMO_
	if ( code == EVENT_UNPRESSED && intfCanHandleInput() ) {
		_shellIconManager.SwitchMenuScreens(pWnd->m_pParent->ID, SQSH_MM_NAME_INPUT_SCR);
	}
#endif
}

//single player menu
void showSingleMenu(CShellWindow* pWnd) {
//	CShellPushButton* txtWnd = dynamic_cast<CShellPushButton*>(_shellIconManager.GetWnd(SQSH_MM_PROFILE_BTN));
//	txtWnd->setText(gameShell->currentSingleProfile.getName());
	_shellIconManager.SwitchMenuScreens(pWnd->m_pParent->ID, SQSH_MM_SINGLE_SCR);
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
#ifndef _DEMO_
	if( code == EVENT_UNPRESSED && intfCanHandleInput() ) {
		_shellIconManager.SwitchMenuScreens(pWnd->m_pParent->ID, SQSH_MM_LOAD_SCR);
	}
#endif
}
void onMMLoadReplayButton(CShellWindow* pWnd, InterfaceEventCode code, int param) {
#ifndef _DEMO_
	if( code == EVENT_UNPRESSED && intfCanHandleInput() ) {
		_shellIconManager.SwitchMenuScreens(pWnd->m_pParent->ID, SQSH_MM_LOAD_REPLAY_SCR);
	}
#endif
}

//profile menu
void onMMProfileList(CShellWindow* pWnd, InterfaceEventCode code, int param) {
	if ( code == EVENT_DOUBLECLICK && intfCanHandleInput() ) {
		CListBoxWindow* list = (CListBoxWindow*)pWnd;
		int pos = list->GetCurSel();
		if (pos != -1) {
			gameShell->currentSingleProfile.setCurrentProfileIndex( pos );
			putStringToReg(mainCurrUserRegFolder, "ProfileName", gameShell->currentSingleProfile.getCurrentProfile().name);
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
			const vector<Profile>& profiles = gameShell->currentSingleProfile.getProfilesVector();
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

		string text = qdTextDB::instance().getText("Interface.Menu.Messages.Confirmations.DeleteProfile");
		string profileName = gameShell->currentSingleProfile.getProfilesVector()[list->GetCurSel()].name;
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
		putStringToReg(mainCurrUserRegFolder, "ProfileName", gameShell->currentSingleProfile.getCurrentProfile().name);
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

//briefing menu
void onMMYearBriefing(CShellWindow* pWnd, InterfaceEventCode code, int param) {
	if ( code == EVENT_DRAWWND ) {
		CTextWindow* txtWnd = (CTextWindow*) pWnd;
		char buffer[30 + 1];
		if (currYear != historyScene.getController()->getCurrentYear()) {
			currYear = historyScene.getController()->getCurrentYear();
			SND2DPlaySound("mainmenu_clock");
		}
		sprintf(buffer, "%d", currYear >= 0 ? currYear : 0);
		string res(getItemTextFromBase("Year").c_str());
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
//		bgScene.setSkinColor(frame ? sColor4f(playerColors[frame->getColorIndex()].unitColor) : sColor4f(1, 1, 1, 1));
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
		#ifndef _DEMO_
			pCombo->Array.push_back( getItemTextFromBase("Hard").c_str() );
		#endif
		pCombo->size = pCombo->Array.size();
		pCombo->pos = 0;
	} else if ( (code == EVENT_UNPRESSED || code == EVENT_RUNPRESSED) && intfCanHandleInput() )	{
		CComboWindow *pCombo = (CComboWindow*) pWnd;
		gameShell->currentSingleProfile.setDifficulty((Difficulty)pCombo->pos);
	}
}
void onMMMissionList(CShellWindow* pWnd, InterfaceEventCode code, int param) {
	if( code == EVENT_DOUBLECLICK && intfCanHandleInput() ) {
		CListBoxWindow* list = (CListBoxWindow*)_shellIconManager.GetWnd(SQSH_MM_MISSION_LIST);
		if ( gameShell->briefingEnabled && list->GetCurSel() >= firstMissionNumber) {
			historyScene.goToMission(list->GetCurSel() - firstMissionNumber); 
			_shellIconManager.SwitchMenuScreens(pWnd->m_pParent->ID, SQSH_MM_BRIEFING_SCR);
		} else {
			historyScene.goToMission(list->GetCurSel());
			missionToExec = MissionDescription( ("RESOURCE\\MISSIONS\\" + historyScene.getMission(list->GetCurSel()).fileName).c_str() );

			//NOTE: should be removed when difficulty will be implemented for each separate player
			missionToExec.getActivePlayerData().difficulty = gameShell->currentSingleProfile.getDifficulty();
			missionToExec.getActivePlayerData().setName(gameShell->currentSingleProfile.getCurrentProfile().name.c_str());
			//NOTE: Setup all names

			gameShell->currentSingleProfile.setLastGameType(UserSingleProfile::SCENARIO);

			gb_Music.FadeVolume(_fEffectButtonTotalTime*0.001f);
			_shellIconManager.SwitchMenuScreens(pWnd->m_pParent->ID, SQSH_MM_LOADING_MISSION_SCR);
		}
	}
}

void onMMGoButton(CShellWindow* pWnd, InterfaceEventCode code, int param) {
	if( code == EVENT_UNPRESSED && intfCanHandleInput() ) {
		CListBoxWindow* list = dynamic_cast<CListBoxWindow*>(_shellIconManager.GetWnd(SQSH_MM_MISSION_LIST));
		if ( gameShell->briefingEnabled && list->GetCurSel() >= firstMissionNumber) {
			historyScene.goToMission(list->GetCurSel() - firstMissionNumber);
			_shellIconManager.SwitchMenuScreens(pWnd->m_pParent->ID, SQSH_MM_BRIEFING_SCR);
		} else {
			historyScene.goToMission(list->GetCurSel());
			missionToExec = MissionDescription( ("RESOURCE\\MISSIONS\\" + historyScene.getMission(list->GetCurSel()).fileName).c_str() );

			//NOTE: should be removed when difficulty will be implemented for each separate player
			missionToExec.getActivePlayerData().difficulty = gameShell->currentSingleProfile.getDifficulty();
			missionToExec.getActivePlayerData().setName(gameShell->currentSingleProfile.getCurrentProfile().name.c_str());
			//NOTE: Setup all names

			gameShell->currentSingleProfile.setLastGameType(UserSingleProfile::SCENARIO);

			gb_Music.FadeVolume(_fEffectButtonTotalTime*0.001f);
			_shellIconManager.SwitchMenuScreens(pWnd->m_pParent->ID, SQSH_MM_LOADING_MISSION_SCR);
		}
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

		string text = qdTextDB::instance().getText("Interface.Menu.Messages.Confirmations.Restart");
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
#ifndef _MULTIPLAYER_DEMO_
		switch(gameShell->currentSingleProfile.getLastGameType()) {
			case UserSingleProfile::SCENARIO:
				_shellIconManager.SwitchMenuScreens(-1, SQSH_MM_SCENARIO_SCR);
				break;
			case UserSingleProfile::BATTLE:
			case UserSingleProfile::SURVIVAL:
				_shellIconManager.SwitchMenuScreens(-1, SQSH_MM_BATTLE_SCR);
				break;
			case UserSingleProfile::LAN:
				gameShell->getNetClient()->FinishGame();
				_shellIconManager.SwitchMenuScreens(-1, SQSH_MM_LAN_SCR);
				break;
			case UserSingleProfile::REPLAY:
				_shellIconManager.SwitchMenuScreens(-1, SQSH_MM_LOAD_REPLAY_SCR);
				break;
			default:
				_shellIconManager.SwitchMenuScreens(-1, SQSH_MM_START_SCR);
		}
#else
		switch(gameShell->currentSingleProfile.getLastGameType()) {
			case UserSingleProfile::LAN:
				gameShell->getNetClient()->FinishGame();
				_shellIconManager.SwitchMenuScreens(-1, SQSH_MM_LAN_SCR);
				break;
			default:
				_shellIconManager.SwitchMenuScreens(-1, SQSH_MM_START_SCR);
		}
#endif
	}
}
void onMMContinueFromStatsButton(CShellWindow* pWnd, InterfaceEventCode code, int param) {
	if( code == EVENT_UNPRESSED && intfCanHandleInput() ) {
#ifndef _MULTIPLAYER_DEMO_
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
#endif
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
#ifndef _DEMO_
	if( code == EVENT_UNPRESSED && intfCanHandleInput() ) {
		_shellIconManager.SwitchMenuScreens( pWnd->m_pParent->ID, SQSH_MM_SAVE_GAME_SCR );
	}		
#endif
}
void onMMInMissLoadButton(CShellWindow* pWnd, InterfaceEventCode code, int param) {
#ifndef _DEMO_
	if( code == EVENT_UNPRESSED && intfCanHandleInput() ) {
		_shellIconManager.SwitchMenuScreens(pWnd->m_pParent->ID, SQSH_MM_LOAD_IN_GAME_SCR);
	}		
#endif
}
void onMMInMissOptionsButton(CShellWindow* pWnd, InterfaceEventCode code, int param) {
	if( code == EVENT_UNPRESSED && intfCanHandleInput() ) {
		_shellIconManager.GetWnd(SQSH_MM_OPTIONS_GRAPHICS)->Enable(gameShell->currentSingleProfile.getLastGameType() != UserSingleProfile::LAN);
		_shellIconManager.SwitchMenuScreens(pWnd->m_pParent->ID, SQSH_MM_SCREEN_OPTIONS);
	}		
}
void onMMInMissRestartButton(CShellWindow* pWnd, InterfaceEventCode code, int param) {
	if( code == EVENT_UNPRESSED && intfCanHandleInput() ) {
		string text = qdTextDB::instance().getText("Interface.Menu.Messages.Confirmations.Restart");
		setupYesNoMessageBox(restartAction, 0, text);

		showMessageBox();
	}		
}
void onMMInMissResumeButton(CShellWindow* pWnd, InterfaceEventCode code, int param) {
	if( code == EVENT_UNPRESSED && intfCanHandleInput() ) {
		_shellIconManager.SwitchMenuScreens( pWnd->m_pParent->ID, RESUME_GAME );
	}		
}
STARFORCE_API void exitToInterfaceMessage(CShellWindow* pWnd) {
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
				_shellIconManager.SwitchMenuScreens( pWnd->m_pParent->ID, SQSH_MM_ENDMISSION_SCR );
			}
			break;
//		case UserSingleProfile::SCENARIO:
//		case UserSingleProfile::BATTLE:
//		case UserSingleProfile::SURVIVAL:
//		case UserSingleProfile::LAN:
		default:
			processInterfaceMessageLater(UNIVERSE_INTERFACE_MESSAGE_GAME_DEFEAT, pWnd ? pWnd->m_pParent->ID : -1);
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
		string text = qdTextDB::instance().getText("Interface.Menu.Messages.Confirmations.Quit");
		setupYesNoMessageBox(quitAction, 0, text);

		showMessageBox();
//		exitToInterfaceMessage(pWnd);
	}		
}


//load game
int delLoadSaveAction(float, float) {
	CListBoxWindow* list = (CListBoxWindow*)_shellIconManager.GetWnd(SQSH_MM_LOAD_MAP_LIST);
//	DeleteFile( savedGames[list->GetCurSel()].saveName() );
//	loadMapVector(savedGames, "RESOURCE\\SAVES\\", "RESOURCE\\SAVES\\*.spg");
	gameShell->currentSingleProfile.deleteSave(savedGames[list->GetCurSel()].missionName());
	const string& savesDir = gameShell->currentSingleProfile.getSavesDirectory();
	loadMapVector(savedGames, savesDir, savesDir + "*.spg");
	fillList(SQSH_MM_LOAD_MAP_LIST, savedGames, SQSH_MM_LOAD_MAP, SQSH_MM_LOAD_MAP_DESCR_TXT);
	return 0;
}
STARFORCE_API void loadGame(CListBoxWindow* listBox) {
	int pos = listBox->GetCurSel();
	if (pos != -1) {
		checkMissionDescription(pos, savedGames);
		missionToExec = savedGames[pos];
		if (savedGames[pos].missionNumber == -1) {
			gameShell->currentSingleProfile.setLastGameType(UserSingleProfile::BATTLE);
//			strncpy(missionToExec.getActivePlayerData().Name, gameShell->currentSingleProfile.getCurrentProfile().name.c_str(), PERIMETER_CONTROL_NAME_SIZE);
		} else if (savedGames[pos].missionNumber == -2) {
			gameShell->currentSingleProfile.setLastGameType(UserSingleProfile::SURVIVAL);
		} else {
			gameShell->currentSingleProfile.setLastGameType(UserSingleProfile::SCENARIO);
			historyScene.goToJustAfterMissionPosition(savedGames[pos].missionNumber);
			historyScene.setMissionNumberToExecute(savedGames[pos].missionNumber);
//			strncpy(missionToExec.getActivePlayerData().Name, gameShell->currentSingleProfile.getCurrentProfile().name.c_str(), PERIMETER_CONTROL_NAME_SIZE);
		}
		
		gb_Music.FadeVolume(_fEffectButtonTotalTime*0.001f);
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
		string text = qdTextDB::instance().getText("Interface.Menu.Messages.Confirmations.DeleteSave");
		string saveName = savedGames[list->GetCurSel()].missionName();
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
	DeleteFile( replays[list->GetCurSel()].fileNamePlayReelGame.c_str() );
	loadMapVector(replays, (string(REPLAY_PATH) + "\\").c_str(), (string(REPLAY_PATH) + "\\*.*").c_str(), true);
	fillReplayList(SQSH_MM_LOAD_REPLAY_LIST, replays, SQSH_MM_LOAD_REPLAY_MAP, SQSH_MM_LOAD_REPLAY_DESCR_TXT);
	return 0;
}
STARFORCE_API void loadReplay(CListBoxWindow* listBox) {
	int pos = listBox->GetCurSel();
	if (pos != -1) {
		checkReplayMissionDescription(pos, replays);
		missionToExec = replays[pos];
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
		string text = qdTextDB::instance().getText("Interface.Menu.Messages.Confirmations.DeleteReplay");
		string replayName = replays[list->GetCurSel()].missionNamePlayReelGame;
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
//	DeleteFile( savedGames[list->GetCurSel()].saveName() );
	gameShell->currentSingleProfile.deleteSave(savedGames[list->GetCurSel()].missionName());
//	loadMapVector(savedGames, "RESOURCE\\SAVES\\", "RESOURCE\\SAVES\\*.spg");
	const string& savesDir = gameShell->currentSingleProfile.getSavesDirectory();
	loadMapVector(savedGames, savesDir, savesDir + "*.spg");
	fillList(SQSH_MM_LOAD_IN_GAME_MAP_LIST, savedGames, SQSH_MM_LOAD_IN_GAME_MAP, SQSH_MM_LOAD_IN_GAME_MAP_DESCR_TXT);
	return 0;
}
STARFORCE_API void loadFromGame(CListBoxWindow* listBox) {
	int pos = listBox->GetCurSel();
	if (pos != -1) {
		checkMissionDescription(pos, savedGames);
		missionToExec = savedGames[pos];
		if (savedGames[pos].missionNumber == -1) {
			gameShell->currentSingleProfile.setLastGameType(UserSingleProfile::BATTLE);
		} else if (savedGames[pos].missionNumber == -2) {
			gameShell->currentSingleProfile.setLastGameType(UserSingleProfile::SURVIVAL);
		} else {
			gameShell->currentSingleProfile.setLastGameType(UserSingleProfile::SCENARIO);
			historyScene.goToJustAfterMissionPosition(savedGames[pos].missionNumber);
			historyScene.setMissionNumberToExecute(savedGames[pos].missionNumber);
		}
		
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
		string text = qdTextDB::instance().getText("Interface.Menu.Messages.Confirmations.DeleteSave");
		string saveName = savedGames[list->GetCurSel()].missionName();
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
//	DeleteFile( savedGames[list->GetCurSel()].saveName() );
	gameShell->currentSingleProfile.deleteSave(savedGames[list->GetCurSel()].missionName());
//	loadMapVector(savedGames, "RESOURCE\\SAVES\\", "RESOURCE\\SAVES\\*.spg");
	const string& savesDir = gameShell->currentSingleProfile.getSavesDirectory();
	loadMapVector(savedGames, savesDir, savesDir + "*.spg");
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

STARFORCE_API int saveGame_(float i, float) {
	gameShell->currentSingleProfile.deleteSave(savedGames[i].missionName());
	string saveName = gameShell->currentSingleProfile.getSavesDirectory() + savedGames[i].missionName();
	if ( gameShell->universalSave(saveName.c_str(), true) ) {
		hideMessageBox();
		_shellIconManager.AddDynamicHandler( toSaveQuant, CBCODE_QUANT );
//		_shellIconManager.SwitchMenuScreens( SQSH_MM_SAVE_GAME_SCR, SQSH_MM_INMISSION_SCR );
	} else {
		setupOkMessageBox(
			0,
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
			string text = qdTextDB::instance().getText("Interface.Menu.Messages.Confirmations.OverwriteSave");
			string saveName = savedGames[pos].missionName();
			char* mess = new char[text.length() + saveName.length()];
			sprintf(mess, text.c_str(), saveName.c_str());
			setupYesNoMessageBox(saveGame, pos, mess);
			delete [] mess;
			showMessageBox();
		}
	} else if (code == EVENT_DRAWWND) {
//		int pos = ((CListBoxWindow*)pWnd)->GetCurSel();
		CEditWindow* input = (CEditWindow*)_shellIconManager.GetWnd(SQSH_MM_SAVE_NAME_INPUT);
		_shellIconManager.GetWnd(SQSH_MM_SAVE_GAME_GO_BTN)->Enable(!input->getText().empty());
	}
}
void onMMSaveGameGoButton(CShellWindow* pWnd, InterfaceEventCode code, int param) {
	if( code == EVENT_UNPRESSED && intfCanHandleInput() ) {
		CListBoxWindow* list = (CListBoxWindow*)_shellIconManager.GetWnd(SQSH_MM_SAVE_GAME_MAP_LIST);
		CEditWindow* input = (CEditWindow*)_shellIconManager.GetWnd(SQSH_MM_SAVE_NAME_INPUT);
		int i;
		int s;
		for (i = 0, s = savedGames.size(); i < s; i++) {
			if (strcmp(input->GetText(), savedGames[i].missionName()) == 0) {
				break;
			}
		}
		if (i != s) {
			string text = qdTextDB::instance().getText("Interface.Menu.Messages.Confirmations.OverwriteSave");
			string saveName = savedGames[i].missionName();
			char* mess = new char[text.length() + saveName.length()];
			sprintf(mess, text.c_str(), saveName.c_str());
			setupYesNoMessageBox(saveGame, i, mess);
			delete [] mess;
			showMessageBox();
		} else {
			string path = gameShell->currentSingleProfile.getSavesDirectory() + input->getText();
			if ( gameShell->universalSave(path.c_str(), true) ) {
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
		string text = qdTextDB::instance().getText("Interface.Menu.Messages.Confirmations.DeleteSave");
		string saveName = savedGames[list->GetCurSel()].missionName();
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
	DeleteFile( replays[list->GetCurSel()].fileNamePlayReelGame.c_str() );
	loadMapVector(replays, (string(REPLAY_PATH) + "\\").c_str(), (string(REPLAY_PATH) + "\\*.*").c_str(), true);
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
	switch ( universe()->savePlayReel(replays[i].fileNamePlayReelGame.c_str()) ) {
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
			string text = qdTextDB::instance().getText("Interface.Menu.Messages.Confirmations.OverwriteReplay");
			string replayName = replays[pos].missionNamePlayReelGame;
			char* mess = new char[text.length() + replayName.length()];
			sprintf(mess, text.c_str(), replayName.c_str());
			setupYesNoMessageBox(saveReplay, pos, mess);
			delete [] mess;
			showMessageBox();
		}
	} else if (code == EVENT_DRAWWND) {
//		int pos = ((CListBoxWindow*)pWnd)->GetCurSel();
		CEditWindow* input = (CEditWindow*)_shellIconManager.GetWnd(SQSH_MM_REPLAY_NAME_INPUT);
		_shellIconManager.GetWnd(SQSH_MM_SAVE_REPLAY_GO_BTN)->Enable(!input->getText().empty());
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
			string text = qdTextDB::instance().getText("Interface.Menu.Messages.Confirmations.OverwriteReplay");
			string replayName = replays[i].missionNamePlayReelGame;
			char* mess = new char[text.length() + replayName.length()];
			sprintf(mess, text.c_str(), replayName.c_str());
			setupYesNoMessageBox(saveReplay, i, mess);
			delete [] mess;
			showMessageBox();
		} else {
			string path = REPLAY_PATH;
			WIN32_FIND_DATA FindFileData;
			HANDLE hf = FindFirstFile( path.c_str(), &FindFileData );
			if (hf == INVALID_HANDLE_VALUE) {
				CreateDirectory(path.c_str(), NULL);
			} else {
				xassert( (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0 );
				FindClose(hf);
			}
			path += "\\";
			path += input->getText();

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
		string text = qdTextDB::instance().getText("Interface.Menu.Messages.Confirmations.DeleteReplay");
		string replayName = replays[list->GetCurSel()].missionNamePlayReelGame;
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

void HistoryScene::executeMission(const string& fileName) {
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
void HistoryScene::showText(const string& text, const string& icon) {
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
	if (gameShell->currentSingleProfile.getLastGameType() == UserSingleProfile::LAN || fromTrigger) {
		processInterfaceMessageLater((terUniverseInterfaceMessage)id);
	}
}

//////////////////////

