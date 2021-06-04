#ifndef __GAME_SHELL_H__
#define __GAME_SHELL_H__

#include "InterfaceLogic.h"
#include "Installer.h"
#include "ResourceDispatcher.h"
#include "Save.h"
#include "HotKey.h"
#include "UserSingleProfile.h"
#include "..\\Network\\P2P_interface.h"
#include "LogicUpdater.h"
#include "ReelManager.h"


class MissionEditor;

struct CommandLineData {

	CommandLineData(
		bool host = false,
		string name = "",
		bool p2p = false,
		string ip = "",
		GUID gameID = GUID(),
		string roomName = "",
		string password = "" );

	bool host;
	string name;
	bool p2p;
	string ip;
	GUID gameID;
	string roomName;
	string password;
};

//------------------------------------------

class GameShell 
{
public:
	GameShell(bool mission_edit);
	~GameShell();

	//Не вызывать напрямую, рользоваться HTManager::GameStart и HTManager::GameClose
	void GameStart(const MissionDescription& mission);
	void GameClose();
	void done();
	void terminate() { GameContinue = false; }

	void startOnline(CommandLineData data);
	void startWithScreen(int id);

	void LANGameStart(const MissionDescription& mission);

	bool universalSave(const char* name, bool userSave);
	SavePrm& savePrm() { return savePrm_; }
	const SaveManualData& manualData() { return savePrm_.manualData; }
	
	void GraphQuant();
	void Show();

	void showWays();

	bool LogicQuant();

	MissionEditor* missionEditor() const { return missionEditor_; }

	bool showEnergy() const;

	void setCountDownTime(int timeLeft);
	const string& getCountDownTime();
	string getTotalTime() const;

//------------------------
	void EventHandler(UINT uMsg,WPARAM wParam,LPARAM lParam);
	void KeyPressed(sKey &key);
	void KeyUnpressed(sKey &key);
	bool DebugKeyPressed(sKey& Key);
	void editParameters();
	void ControlPressed(int key);
	void ControlUnpressed(int key);
	void MouseLeftPressed(const Vect2f& pos);
	void MouseRightPressed(const Vect2f& pos);
	void MouseRightUnpressed(const Vect2f& pos);
	void MouseLeftUnpressed(const Vect2f& pos);

	void MouseMidPressed(const Vect2f& pos);
	void MouseMidUnpressed(const Vect2f& pos);

	void MouseMove(const Vect2f& pos);
	void MouseLeftDoubleClick(const Vect2f& pos);
	void MouseRightDoubleClick(const Vect2f& pos);
	void MouseWheel(int delta);
	void MouseLeave();
	void OnWindowActivate();

	void cancelMouseLook();

	void CameraQuant();

	void updatePosition();

	Vect2f convert(int x, int y) const; // screen -> ([-0.5..0.5], [-0.5..0.5])
	Vect2i convertToScreenAbsolute(const Vect2f& pos); // Absolute screen for cursor positioning

	void updateMap();
	void updateResolution(int sx, int sy,bool change_depth,bool change_size);

	void recreateChaos();
	void createChaos();
//---------------------------------------
	void startResourceDispatcher();
	void initResourceDispatcher();

	void setSpeed(float speed);
	void decrSpeed() { setSpeed(game_speed*0.95f); }
	void incrSpeed() { setSpeed((game_speed + 0.01f)*1.05f); }
	void decrSpeedStep() {
		if (game_speed > 0) {
			if (game_speed <= 1) {
				setSpeed(0.5f);
			} else if (game_speed <= 2) {
				setSpeed(1);
			}
		}
	}
	void incrSpeedStep() {
		if (game_speed >= 1) {
			setSpeed(2);
		} else if (game_speed >= 0.5f) {
			setSpeed(1);
		}
	}

	float getSpeed() const { return game_speed; }

	time_type gameTimer() const { return global_time(); }

	void pauseGame(bool pausedByMenu = false) {
		if (game_speed) {
			gamePausedByMenu = pausedByMenu;
			game_speed_to_resume = game_speed;
			setSpeed(0);
		}
	}
	void resumeGame(bool resumedByMenu = false) {
		if (!resumedByMenu || gamePausedByMenu) {
			setSpeed(game_speed_to_resume);
		}
	}
	bool isPaused() const { return game_speed < 1e-5f; }
	bool recordMovie() const { return recordMovie_; }

	bool mouseLeftPressed() const { return mouseLeftPressed_; }
	bool mouseRightPressed() const { return mouseRightPressed_; }
	const Vect2f mousePosition() const { return mousePosition_; }
	const Vect2f mousePositionDelta() const { return mousePositionDelta_; }
	
	const Vect2f mousePressControl() const { return mousePressControl_; }
	void setMousePressControl(const Vect2f& pos) { mousePressControl_ = pos; }

	const Vect3f& mapMoveStartPoint() const { return mapMoveStartPoint_; }

	void setWindowClientSize(const Vect2i& size) { windowClientSize_ = size; }
	const Vect2i& windowClientSize() const { return windowClientSize_; }

	void setCursorPosition(const Vect2f& pos);
	void setCursorPosition(const Vect3f& posW);

	void rememberPlayerCamera(terPlayer* player, const char* triggerName);

	bool interfaceShowFlag() const { return interfaceShowFlag_; }
	bool showKeysHelp() const { return showKeysHelp_; }

	cFont* debugFont() const { return debugFont_; }
	void setSideArrowsVisible(bool visible);

	void createNetClient(PNetCenter::e_PNCWorkMode _workMode, const char* playerName = 0, const char* InternetAddress=0, const char* password = 0);
	PNetCenter* getNetClient();
	void stopNetClient();

	bool triggersDisabled() const { return triggersDisabled_; }
	void setTriggersDisabled() { triggersDisabled_ = true; }

	void setSkipCutScene(bool skip);
	bool cutSceneSkipped() const { return cutSceneSkipped_;	}
	bool cutSceneWasSkipped(int time) const { return lastSkipTime_() && lastSkipTime_() < time; }

	bool alwaysRun() const { return alwaysRun_; }
	
	bool GameActive;
	bool MainMenuEnable;
	bool GameContinue;

	int MouseMoveFlag;
	int MousePositionLock;
	
	bool   cameraMouseShift;
	bool   cameraMouseTrack;
	bool   cameraMouseZoom;
	bool   toolzerSizeTrack;
	bool   cameraCursorInWindow;
	bool  CursorOverInterface;
		
	terBuildingInstaller BuildingInstaller;
	
	CShellLogicDispatcher  m_ShellDispatcher;

	MissionDescription CurrentMission;

	HotKeyManager* hotKeyManager;

	UserSingleProfile currentSingleProfile;
	bool briefingEnabled;

	LogicUpdater& getLogicUpdater() {
		return logicUpdater;
	}

	void showReelModal(const char* binkFileName, const char* soundFileName, bool localized = false, bool stopBGMusic = true, int alpha = 255);
	void showPictureModal(const char* pictureFileName, bool localized, int stableTime);

	void changeControlState(const vector<SaveControlData>& newControlStates);
	void fillControlState(vector<SaveControlData>& controlStatesToSave);

	void setScriptReelEnabled(bool isScriptReelEnabled);
	bool isScriptReelEnabled() {
		return scriptReelEnabled;
	}

	void toggleScriptReelEnabled() {
		setScriptReelEnabled(!scriptReelEnabled);
	}

	static const string& getLocDataPath() {
		if (locale.empty()) {
			locale = getStringFromReg(mainCurrUserRegFolder, "Locale");
			if (locale.empty()) {
				locale = IniManager("Perimeter.ini", false).get("Game","DefaultLanguage");
				if (locale.empty()) {
					locale = "English";
				}
			}
			locale = "RESOURCE\\LocData\\" + locale + "\\"; 
		}
		return locale;
	}

	//-----Network function-----
	void NetQuant();

	enum e_NetCenterConstructorReturnCode {
		NCC_RC_OK,
		NCC_RC_GAMESPY_CONNECT_ERR,
		NCC_RC_GAMESPY_NICK_ERR,
		NCC_RC_DIRECTPLAY_INIT_ERR
	};
	void callBack_NetCenterConstructorReturnCode(e_NetCenterConstructorReturnCode retCode);
	enum e_CreateGameReturnCode {
		CG_RC_OK,
		CG_RC_CREATE_HOST_ERR
	};
	void callBack_CreateGameReturnCode(e_CreateGameReturnCode retCode);
	enum e_JoinGameReturnCode {
		JG_RC_OK,
		JG_RC_GAMESPY_CONNECTION_ERR,
		JG_RC_GAMESPY_PASSWORD_ERR,
		JG_RC_CONNECTION_ERR,
		JG_RC_GAME_IS_RUN_ERR,
		JG_RC_GAME_IS_FULL_ERR,
		JG_RC_GAME_NOT_EQUAL_VERSION_ERR
	};
	void callBack_JoinGameReturnCode(e_JoinGameReturnCode retCode);
	
	void showConnectFailedInGame(const string& playerList);
	void hideConnectFailedInGame(bool connectionRestored = true);

	void playerDisconnected(string& playerName, bool disconnectOrExit);

	enum GeneralErrorType {
		GENERAL_CONNECTION_FAILED,
		HOST_TERMINATED
	};
	void generalErrorOccured(GeneralErrorType error);

	void abnormalNetCenterTermination();

	void addStringToChatWindow(int clanNum, const char* newString);

	//-----end of network function----

	bool reelAbortEnabled;

	void setCutSceneMode(bool on, bool animated = true);
	bool isCutSceneMode();

	static void SetFontDirectory();

	void prepareForInGameMenu();

	void switchActivePlayer(bool next);

	static void preLoad();

	bool isStartedWithMainmenu() const {
		return startedWithMainmenu;
	}

private:
	static bool canProcessKeyEvent(LPARAM lParam) {
//		return true;
		return !(lParam & 0x40000000);
	}

	class CChaos* chaos;
	
	bool scriptReelEnabled;

	bool startedWithMainmenu;

	float FogStart;
	float FogEnd;
	sColor4f FogColor;

	Vect2i windowClientSize_;

	bool mouseLeftPressed_;
	bool mouseRightPressed_;
	Vect2f mousePosition_;
	Vect2f mousePositionDelta_;
	Vect2f mousePressControl_;

	bool alwaysRun_;
	bool interfaceShowFlag_;
	bool showKeysHelp_;
	bool showWireFrame_;
	int activePlayerID_;

	Vect3f mapMoveStartPoint_;
		
	float game_speed;
	float game_speed_to_resume;
	//MeasurementTimer gameTimer_;

	bool EnableDebugKeyHandlers;
	class PNetCenter* NetClient;
		
	MissionEditor* missionEditor_;

	string defaultSaveName_;

	int synchroByClock_;
	int framePeriod_;

	int shotNumber_;
	bool recordMovie_;
	int movieShotNumber_;
	int	movieStartTime_;
	string movieName_;

	SavePrm savePrm_;

	cFont* debugFont_;

	int countDownTimeMillisLeft;
	int countDownTimeMillisLeftVisible;

	string countDownTimeLeft;
	string totalTimeLeft;

	bool gamePausedByMenu;

	bool triggersDisabled_;
	bool cutSceneSkipped_;
	MeasurementTimer lastSkipTime_;

	//---------------------------------
	void ShotsScan();
	void MakeShot();

	void startStopRecordMovie();
	void makeMovieShot();

	//---------------
	void DebugCteateFilth(terFilthSpotID id);

	//Autoswitch AI
	bool autoSwitchAIEnabled;
	int autoSwitchAITimer;
	void setActivePlayerAIOff();
	void checkAutoswitchAI();

	bool checkReel(UINT uMsg,WPARAM wParam,LPARAM lParam);

	LogicUpdater logicUpdater;

	ReelManager reelManager;

	void setLocalizedFontSizes();

	static string locale;

	bool soundPushedByPause;
	int soundPushedPushLevel;

	DebugPrm debugPrm_;
};

extern GameShell* gameShell;

void LoadProgressStart();
void LoadProgressBlock(float delta);
void LoadProgressUpdate(float delta);

#endif
