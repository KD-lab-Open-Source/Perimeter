#ifndef __GAME_SHELL_H__
#define __GAME_SHELL_H__

#include "InterfaceLogic.h"
#include "Installer.h"
#include "Save.h"
#include "HotKey.h"
#include "UserSingleProfile.h"
#include "../Network/P2P_interface.h"
#include "LogicUpdater.h"
#include "ReelManager.h"
#include <SDL_events.h>

struct LocalizedText;
class MissionEditor;

struct CommandLineData {
    bool server;
    std::string save;
    std::string address;
    std::string playerName;
    std::string roomName;
    std::string password;
    bool publicHost;
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
	void terminate();

	void startCmdline(const CommandLineData& data);
	void switchToInitialMenu();

    void MultiplayerGameStarting();
	void MultiplayerGameStart(const MissionDescription& mission);
    void MultiplayerGameRestore(const MissionDescription& mission);

	bool universalSave(const char* name, bool userSave, MissionDescription* missionOutput = nullptr);
	SavePrm& savePrm() { return savePrm_; }
	const SaveManualData& manualData() { return savePrm_.manualData; }
	
	void GraphQuant();
	void Show();

	void showWays();

	bool LogicQuant();

	MissionEditor* missionEditor() const { return missionEditor_; }

	bool showEnergy() const;

	void setCountDownTime(int timeLeft);
	const std::string& getCountDownTime();
	std::string getTotalTime() const;

//------------------------
    void EventHandler(SDL_Event& event);
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

	void MouseMove(const Vect2f& pos, const Vect2f& rel);
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
	void updateResolution(bool change_depth, bool change_size, bool change_display_mode);

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
	const Vect2f& mousePosition() const { return mousePosition_; }
    const Vect2f& mousePositionDelta() const { return mousePositionDelta_; }
    const Vect2f& mousePositionRelative() const { return mousePositionRelative_; }
	
	const Vect2f mousePressControl() const { return mousePressControl_; }
	void setMousePressControl(const Vect2f& pos) { mousePressControl_ = pos; }

	const Vect3f& mapMoveStartPoint() const { return mapMoveStartPoint_; }

	void setWindowClientSize(const Vect2i& size);
	const Vect2i& windowClientSize() const { return windowClientSize_; }

	void setCursorPosition(const Vect2f& pos);
	void setCursorPosition(const Vect3f& posW);

	void rememberPlayerCamera(terPlayer* player, const char* triggerName);

	bool interfaceShowFlag() const { return interfaceShowFlag_; }
	bool showKeysHelp() const { return showKeysHelp_; }

	cFont* debugFont() const { return debugFont_; }
	void setSideArrowsVisible(bool visible);

	void createNetClient();
	PNetCenter* getNetClient();
	void destroyNetClient();

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
		
	terBuildingInstaller* BuildingInstaller = nullptr;
	
	CShellLogicDispatcher  m_ShellDispatcher;

	MissionDescription CurrentMission;

	HotKeyManager* hotKeyManager;

	UserSingleProfile currentSingleProfile;
	bool briefingEnabled;

	LogicUpdater& getLogicUpdater() {
		return logicUpdater;
	}
    
    bool BuildingInstallerInited() const {
        return BuildingInstaller && BuildingInstaller->inited();
    }

	void showReelModal(const char* videoFileName, const char* soundFileName, bool localized = false, bool stopBGMusic = true, int alpha = 255);
	void showPictureModal(const char* pictureFileName, bool localized, int stableTime);

	void changeControlState(const std::vector<SaveControlData>& newControlStates, bool reset_controls);
	void fillControlState(std::vector<SaveControlData>& controlStatesToSave);

	void setScriptReelEnabled(bool isScriptReelEnabled);
	bool isScriptReelEnabled() {
		return scriptReelEnabled;
	}

	void toggleScriptReelEnabled() {
		setScriptReelEnabled(!scriptReelEnabled);
	}

	//-----Network function-----
	void NetQuant();
	enum e_CreateGameReturnCode {
		CG_RC_OK,
		CG_RC_CREATE_HOST_ERR
	};
	void callBack_CreateGameReturnCode(e_CreateGameReturnCode retCode);
	enum e_JoinGameReturnCode {
		JG_RC_OK,
        JG_RC_SIGNATURE_ERR,
		JG_RC_PASSWORD_ERR,
		JG_RC_CONNECTION_ERR,
		JG_RC_GAME_IS_RUN_ERR,
		JG_RC_GAME_IS_FULL_ERR,
		JG_RC_GAME_NOT_EQUAL_VERSION_ERR,
        JG_RC_GAME_NOT_EQUAL_CONTENT_ERR,
        JG_RC_GAME_NOT_EQUAL_ARCH_ERR
	};
	void callBack_JoinGameReturnCode(e_JoinGameReturnCode retCode, std::string extraInfo);
	
	void showConnectFailedInGame(const std::string& playerList);
	void hideConnectFailedInGame(bool connectionRestored = true);

    void serverMessage(LocalizedText* message);

	enum GeneralErrorType {
		GENERAL_CONNECTION_FAILED,
        CLIENT_DROPPED,
		HOST_TERMINATED,
        DESYNC
	};
	void generalErrorOccured(GeneralErrorType error);

	void abnormalNetCenterTermination();
    
    void updateLatencyInfo(const NetLatencyInfo& info);

	void addStringToChatWindow(bool clanOnly, const std::string& newString, const std::string& locale);

	//-----end of network function----

	bool reelAbortEnabled;

	void setCutSceneMode(bool on, bool animated = true);
	bool isCutSceneMode();

	void prepareForInGameMenu();

	void switchActivePlayer(bool next);

	static void preLoad();

	bool isStartedWithMainmenu() const {
		return startedWithMainmenu;
	}

private:
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
    Vect2f mousePositionRelative_;
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

	int synchroByClock_;
	int framePeriod_;

	int shotNumber_;
	bool recordMovie_;
	int movieShotNumber_;
	int	movieStartTime_;
	std::string movieName_;

	SavePrm savePrm_;

	cFont* debugFont_;

	int countDownTimeMillisLeft;
	int countDownTimeMillisLeftVisible;

	std::string countDownTimeLeft;
	std::string totalTimeLeft;

	bool gamePausedByMenu;

	bool triggersDisabled_;
	bool cutSceneSkipped_;
	MeasurementTimer lastSkipTime_;

    //Double clicking vars
    int doubleClickDistance = 10;
    int doubleClickTime = 300;
    int lastClickButton = 0;
    double lastClickTime = 0;
    Vect2f lastClickPosition = Vect2f::ZERO;

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

	LogicUpdater logicUpdater;

	ReelManager reelManager;

	void setLocalizedFontSizes();

	bool soundPushedByPause;
	int soundPushedPushLevel;

	DebugPrm debugPrm_;
};

extern GameShell* gameShell;

void LoadProgressStart();
void LoadProgressBlock(float delta);
void LoadProgressUpdate(float delta);

#endif
