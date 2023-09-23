#include "StdAfx.h"

#include "PrmEdit.h"
#include "Region.h"
#include "CameraManager.h"
#include "Runtime.h"
#include "GameShell.h"
#include "GenericControls.h"
#include "Universe.h"
#include "Config.h"

#include "MissionEdit.h"
#include "UniverseInterface.h"

//#include "InterfaceScript.h"
#include "PerimeterShellUI.h"
#include "Controls.h"
#include "GameContent.h"

#include "Silicon.h"
#include "HistoryScene.h"
#include "BGScene.h"
#include "../Game/MusicManager.h"

#include "RigidBody.h"
#include "chaos.h"
#include "P2P_interface.h"

#include "GenericFilth.h"
#include "GeoControl.h"
#include "AITileMap.h"
#include "PerimeterSound.h"

#include "ExternalShow.h"
#include "Triggers.h"
#include "../TriggerEditor/TriggerEditor.h"

#include "qd_textdb.h"
#include "../HT/ht.h"
#include "VideoPlayer.h"

#include "EditArchive.h"
#include "XPrmArchive.h"
#include "SoundScript.h"
#include "BelligerentSelect.h"
#include "files/files.h"
#include "Localization.h"
#include "codepages/codepages.h"
#include <SDL.h>

int terShowFPS = 0;

CShellIconManager   _shellIconManager;
CShellCursorManager _shellCursorManager;

extern char _bMenuMode;
extern char  _bCursorVisible;
extern bool bWasShiftUnpressed;

void SetCameraPosition(cCamera *UCamera,const MatXf &Matrix);
void ToolzerSizeChangeQuant();

void EnterInMissionMenu();
void CancelEditWorkarea();

extern HistoryScene historyScene;
extern HistoryScene bwScene;
extern BGScene bgScene;
extern void PlayMusic(const char *str = 0);

bool terEnableGDIPixel=false;

//extern XStream quantTimeLog;

int showReels( float, float ) {
	for (int i = 0; i < REEL_COUNT; i++) {
		gameShell->reelAbortEnabled = reels[i].abortEnabled;
		if (reels[i].video) {
			gameShell->showReelModal(reels[i].name, 0, reels[i].localized);
		} else {
			gameShell->showPictureModal(reels[i].name, reels[i].localized, reels[i].time);
		}
        if (!gameShell->GameContinue) {
            break;
        }
	}
	gameShell->reelAbortEnabled = true;
//	_shellIconManager.GetWnd(SQSH_MM_SPLASH4)->Show(1);
//	_shellIconManager.SetModalWnd(SQSH_MM_SPLASH4);

	gameShell->switchToInitialMenu();
	return 0;
}

void abortWithMessage(const std::string& messageID) {
	SNDReleaseSound();
	ErrH.Abort( qdTextDB::instance().getText(messageID.c_str()) );
}

void ErrorInitialize3D() {
    fprintf(stderr, "%dx%d %dhz Display: %d\n", terScreenSizeX, terScreenSizeY, terScreenRefresh, terScreenIndex);
	abortWithMessage("Interface.Menu.Messages.Init3DError");
}

void checkCmdLineArg(const char* argument, const char* argName) {
	if (!argument) {
        std::string msg("Missing cmdline argument ");
        ErrH.Abort(msg + argName);
	}
}

//------------------------
GameShell::GameShell(bool mission_edit) :
chaos(0),
NetClient(0), 
missionEditor_(0),
BuildingInstaller(nullptr),
windowClientSize_(1024, 768)
{
	gameShell = this;

	scriptReelEnabled = false;

	startedWithMainmenu = false;

	setLogicFp();

	setLocalizedFontSizes();

	soundPushedByPause = false;
	soundPushedPushLevel=INT_MIN;

	alwaysRun_ = check_command_line("active");
	GameActive = false;
	GameContinue = true;
	showKeysHelp_ = false;
	interfaceShowFlag_ = true;

	autoSwitchAITimer = 0;

	reelManager.sizeType = ReelManager::FULL_SCREEN;

	reelAbortEnabled = true;
	gamePausedByMenu = false;

    IniManager("Perimeter.ini", false).getInt("Game","DoubleClickTime", doubleClickTime);
    IniManager("Perimeter.ini", false).getInt("Game","DoubleClickDistance", doubleClickDistance);

	debug_allow_replay = true; //IniManager("Perimeter.ini", false).getInt("Game","EnableReplay");

    terShowFPS = IniManager("Perimeter.ini").getInt("Game","ShowFPS");
    check_command_line_parameter("show_fps", terShowFPS);

    MainMenuEnable = IniManager("Perimeter.ini").getInt("Game","MainMenu");
	check_command_line_parameter("mainmenu", MainMenuEnable);
	if(mission_edit)
		MainMenuEnable = false;

	currentSingleProfile.scanProfiles();
	currentSingleProfile.setCurrentProfile(getStringSettings("ProfileName"));
	if (!MainMenuEnable && currentSingleProfile.getCurrentProfileIndex() == -1) {
		if (!currentSingleProfile.getProfilesVector().size()) {
			currentSingleProfile.addProfile("Test");
		}
		currentSingleProfile.setCurrentProfileIndex(0);
	}

	float menuAnimSpeedCoeff = IniManager("Perimeter.ini").getFloat("Graphics","MenuAnimationSpeedFactor");
	_fEffectButtonTime1 *= menuAnimSpeedCoeff;
	_fEffectButtonTime2 *= menuAnimSpeedCoeff;
	_fEffectButtonTime3 *= menuAnimSpeedCoeff;
	_fEffectButtonTotalTime *= menuAnimSpeedCoeff;
	_fEffectIntfRoll1 *= menuAnimSpeedCoeff;
	_fEffectIntfRoll2 *= menuAnimSpeedCoeff;
	_fEffectIntfRoll3 *= menuAnimSpeedCoeff;
	bgEffectTime *= menuAnimSpeedCoeff;


//	autoSwitchAIEnabled = check_command_line("autoSwitchAI") || IniManager("Perimeter.ini").getInt("Game","AutoSwitchAI");
	autoSwitchAIEnabled = check_command_line("autoSwitchAI");

	briefingEnabled = !(disableBriefing || check_command_line("disableBriefing"));

	terCamera->setRestriction(IniManager("Perimeter.ini").getInt("Game","CameraRestriction") && !mission_edit);
	EnableDebugKeyHandlers = EnableDebugKeyHandlersInitial;

	shotNumber_ = -1;
	recordMovie_ = false;
	movieShotNumber_ = 0;

	game_speed_to_resume = game_speed = 1;

	countDownTimeLeft = "";
	countDownTimeMillisLeft = -1;
	countDownTimeMillisLeftVisible = -1;
	
	showWireFrame_ = false;

	initResourceDispatcher();

	cameraMouseTrack = false;
	cameraMouseShift = false;
	cameraMouseZoom  = false;
	toolzerSizeTrack = false;
	
	MouseMoveFlag = 0;
	MousePositionLock = 0;
	
	mouseLeftPressed_ = false;
	mouseRightPressed_ = false;
	mousePosition_ = Vect2f::ZERO;
	mousePositionDelta_ = Vect2f::ZERO;
    mousePositionRelative_ = Vect2f::ZERO;
	
	CursorOverInterface = false;

	activePlayerID_ = 0;
	
	mousePressControl_ = Vect2f::ZERO;
	mapMoveStartPoint_ = Vect3f::ZERO;

	debugFont_ = terVisGeneric->CreateGameFont(sqshFontPopup, 15);

	hotKeyManager = new HotKeyManager();
	_shellCursorManager.Load();

#ifdef PERIMETER_DEBUG
    debugPrm_.load();
    if(check_command_line("explore")){
        debugPrm_.edit();
        ErrH.Exit();
    }
#endif

	if (check_command_line("pause")) {
		ErrH.Abort("Pause!!!");
	}

    const char* server = check_command_line("server"); 
    const char* connect = check_command_line("connect");
    if (server || connect) {
        CommandLineData data;
        data.server = server != nullptr;
        
        const char* password = check_command_line("password");
        if (password) data.password = password;
        
        const char* playerName = check_command_line("name");
        checkCmdLineArg(playerName, "name");
        data.playerName = playerName;
		
        if (server) {
            checkCmdLineArg(server, "server");
            const char* savePath = check_command_line("save");
            if (savePath) data.save = savePath;
            const char* roomName = check_command_line("room");
            if (roomName) data.roomName = roomName;
            data.address = server;
		} else {
            checkCmdLineArg(connect, "connect");
            data.address = connect;
        }
        data.publicHost = check_command_line("public") != nullptr;

        startCmdline(data);
	} else if (MainMenuEnable) {
		startedWithMainmenu = true;
		_shellIconManager.LoadControlsGroup(SHELL_LOAD_GROUP_MENU);
		//_shellIconManager.SwitchMenuScreens(-1, SQSH_MM_SCREEN1);
        
        int splash = IniManager("Perimeter.ini").getInt("Game","StartSplash");
        check_command_line_parameter("start_splash", splash);
        if (splash) {
			_bCursorVisible = 0;
//			_shellIconManager.GetWnd(SQSH_MM_SPLASH1)->Show(1);
//			_shellIconManager.SetModalWnd(SQSH_MM_SPLASH1);
			_shellIconManager.AddDynamicHandler(showReels, CBCODE_QUANT); //ждать пока не слетится
		} else {
            switchToInitialMenu();
		}
	}

	if(mission_edit)
		missionEditor_ = new MissionEditor;

	if(!MainMenuEnable){
	    std::string resource_path = convert_path_content("RESOURCE");
		std::string name;
		std::string path = resource_path;

		if(check_command_line("save")){
			path = convert_path_content(UserSingleProfile::getAllSavesDirectory());
			name = check_command_line("save");
		} else if(check_command_line("mission")){
			path = convert_path_content(MISSIONS_PATH);
			name = check_command_line("mission");
		} else if (check_command_line("open")) {
            name = check_command_line("open");
        } else if (mission_edit) {
			path = convert_path_content(MISSIONS_PATH);
			name = "";
		}

        if (path.empty()) {
            path = resource_path;
        }
		if (name.empty()) {
            name = "XXX";
        }
        
        terminate_with_char(path, PATH_SEP);
		std::string spgPath = convert_path_content(setExtension(path + name, "spg"));
        if (spgPath.empty()) {
            spgPath = setExtension(path + name, "spg");
        }

		if (!XStream(0).open(spgPath)) {
            if (name != "XXX") {
                fprintf(stderr, "File not found: %s\n", spgPath.c_str());
            }
			if (openFileDialog(spgPath, path.c_str(), "spg", "Mission Name")){
				size_t pos = spgPath.rfind(path);
				if (pos != std::string::npos) {
                    spgPath.erase(0, pos);
                }
			} else {
                ErrH.Exit();
            }
        }
		
		if(check_command_line(KEY_REPLAY_REEL)){
			const char* fname=check_command_line(KEY_REPLAY_REEL);
			HTManager::instance()->GameStart(MissionDescription(fname, GT_PLAY_RELL));
		} else {
			int aiMode = 1;
			check_command_line_parameter("AI", aiMode);
            
            if (!XStream(0).open(spgPath)) {
                fprintf(stderr, "File not found: %s\n", spgPath.c_str());
                ErrH.Exit();
            }
            
            MissionDescription md(spgPath.c_str(), aiMode == 1 ? GT_SINGLE_PLAYER : (aiMode == 2 ? GT_SINGLE_PLAYER_ALL_AI : GT_SINGLE_PLAYER_NO_AI));
            if (!md.isCampaign() && string_to_lower(spgPath.c_str()).find("resource" PATH_SEP_STR "missions" PATH_SEP_STR) != std::string::npos) {
                //Workaround to load campaign attrs when using cmdline
                md.missionNumber = 0;
            }
			HTManager::instance()->GameStart(md);
		}

		if(check_command_line("convert")){
			universalSave(spgPath.c_str(), false);
			SNDReleaseSound();
			ErrH.Exit();
		}
	}
}

GameShell::~GameShell()
{
	GameContinue = false;
	setScriptReelEnabled(false);

	if (soundPushedByPause) {
		SNDPausePop();
	}

	HTManager::instance()->GameClose();

	if(missionEditor_)
		delete missionEditor_;

	_shellCursorManager.Done();

	if(NetClient)
		delete NetClient;

	debugFont_->Release();
	if (hotKeyManager) {
		delete hotKeyManager;
	}
}

void GameShell::done() {
	historyScene.done();
	bwScene.done();
	bgScene.done();
	_shellIconManager.Done();
	_shellCursorManager.Done();
}

void GameShell::terminate() {
    GameContinue = false;
    if (reelManager.isVisible()) {
        reelManager.hide();
    }
}

PNetCenter* GameShell::getNetClient() {
//	if (!NetClient) {
//		NetClient = new PNetCenter(true);
//	}
	return NetClient;
}

void GameShell::switchToInitialMenu() {
	_bCursorVisible = 1;
	_WaitCursor();
	_shellIconManager.SwitchMenuScreens(-1, _shellIconManager.initialMenu);
	_shellIconManager.SetModalWnd(0);
}

void GameShell::createNetClient() {
    destroyNetClient();
    NetClient = new PNetCenter();
}

void GameShell::destroyNetClient() {
	if (NetClient) {
		delete NetClient;
		NetClient = nullptr;
	}
}

void GameShell::GameStart(const MissionDescription& mission)
{
	_WaitCursor();

	setScriptReelEnabled(false);

	check_determinacy_quant(true);

//	HTManager::instance()->GameClose();
	cVisGeneric::SetAssertEnabled(false);
	CurrentMission = mission;

	for (int i = 0; i < CurrentMission.playersData.size(); i++) {
		PlayerData* data = &CurrentMission.playersData[i];
        std::string playerName;
		if (data->realPlayerType == REAL_PLAYER_TYPE_PLAYER && *(data->name()) == 0) {
            if (currentSingleProfile.getCurrentProfileIndex() == -1 && currentSingleProfile.getLastGameType() != UserSingleProfile::MULTIPLAYER) {
                playerName = currentSingleProfile.getCurrentProfile().name;
            }
		} else if (data->realPlayerType == REAL_PLAYER_TYPE_AI) {
            playerName = getBelligerentName(data->belligerent);
		}
        if (!playerName.empty()) {
            data->setName(playerName);
        }
	}

	LoadProgressStart();

	LoadProgressBlock(0.1f);
	LoadProgressUpdate(1);

	triggersDisabled_ = false;
	cutSceneSkipped_ = false;
	lastSkipTime_.stop();
    
    if (!BuildingInstaller) {
        BuildingInstaller = new terBuildingInstaller();
    }
		
	//vMap.selectUsedWorld(CurrentMission.worldID());

//	SetShadowType(terShadowType,terDrawMeshShadow,false);

	//vMap.fullLoad(IniManager("Perimeter.ini").getInt("TD","FastLoad"));

//	IniManager world_ini(GetTargetName(vMap.worldIniFile));
//	FogStart = world_ini.getFloat("Visualization Parameters","FogStart");
//	FogEnd = world_ini.getFloat("Visualization Parameters","FogEnd");
//	world_ini.getFloatArray("Visualization Parameters","FogColor", 3, &FogColor.r);
//	FogColor /= 255;
	
	setSpeed(IniManager("Perimeter.ini").getFloat("Game", "GameSpeed"));

	terCamera->reset();

	LoadProgressBlock(0.6f);
	CurrentMission.packPlayerIDs();
	new terUniverse(NetClient, CurrentMission, LoadProgressUpdate);
    universe()->universalLoad(CurrentMission, this->savePrm(), LoadProgressUpdate);
    
#ifdef PERIMETER_DEBUG
    log_var(XRndGet());
    log_var(logicRND.get());
    log_var(xm_random_generator.get());
    /*
    uint32_t attrcrc = startCRC32;
    attrcrc = getSerializationCRC<BinaryOArchive>(rigidBodyPrmLibrary(), attrcrc);
    attrcrc = getSerializationCRC<BinaryOArchive>(attributeLibrary(), attrcrc);
    attrcrc = getSerializationCRC<BinaryOArchive>(globalAttr(), attrcrc);
    log_var(attrcrc);
    */
#endif

    int fogEnable = 1;
    IniManager("Perimeter.ini", false).getInt("Graphics", "FogEnable", fogEnable);
	IniManager world_ini(GetTargetName(vMap.worldIniFile).c_str());
    if (fogEnable) {
        FogStart = world_ini.getFloat("Visualization Parameters", "FogStart");
        FogEnd = world_ini.getFloat("Visualization Parameters", "FogEnd");
    } else {
        FogStart = 0;
        FogEnd = 0;
    }
	world_ini.getFloatArray("Visualization Parameters","FogColor", 3, &FogColor.r);
	FogColor /= 255;

	HTManager::instance()->setSyncroTimer(synchroByClock_, framePeriod_, terMaxTimeInterval);
	HTManager::instance()->setLogicTimePeriod(terLogicTimePeriod);
	HTManager::instance()->setSpeedSyncroTimer(getSpeed());

	LoadProgressBlock(0.7f);
	LoadProgressUpdate(1);
	m_ShellDispatcher.init();
	//universe()->universalLoad(CurrentMission, savePrm());
	createChaos();
	GameActive = true;
	MusicOpenWorld();

//	CameraUpdateFocusPoint();
//	CameraQuant();
//	terCamera->SetPosition(CameraPosition);
//	terCamera->SetTarget(CameraAngle);

	LoadProgressBlock(1);
	LoadProgressUpdate(1);

	bool occlusion=IniManager("Perimeter.ini").getInt("Graphics","EnableOcclusion");
	terVisGeneric->EnableOcclusion(occlusion);

//	float particle_rate=IniManager("Perimeter.ini").getFloat("Graphics","ParticleRate");
//	xassert(particle_rate>=0 && particle_rate<=1);
//	terVisGeneric->SetGlobalParticleRate(particle_rate);

	autoSwitchAITimer = 0;

	_shellIconManager.Done();
	_shellIconManager.LoadControlsGroup(SHELL_LOAD_GROUP_GAME);
	_shellIconManager.load(savePrm().activeTasks);

	terScene->Compact();

	universe()->relaxLoading();
	universe()->setShouldIgnoreIntfCommands(CurrentMission.gameType_ == GT_PLAY_RELL);

	game_speed_to_resume = game_speed;

    if (missionEditor()) {
        setSpeed(0);
    } else if (0 < CurrentMission.gameSpeed) {
		setSpeed(CurrentMission.gameSpeed);
		if(CurrentMission.gamePaused) {
            pauseGame();
        }
	}

	startResourceDispatcher();

	if (NetClient) {
        NetClient->GameIsReady();
    }

	_shellIconManager.gameTypeChanged(currentSingleProfile.getLastGameType());

	_shellIconManager.setCutSceneMode(!(manualData().interfaceEnabled || missionEditor()), false);

	getLogicUpdater().update();
	getLogicUpdater().exchange();

	setCountDownTime(-1);

//	gameTimer_.start();
}

void GameShell::GameClose()
{
	getLogicUpdater().reset();
	stream_interpolator.ClearData();
	m_ShellDispatcher.close();
	//_shellIconManager.Done();

	_pUnitHover = 0;

	if(chaos){
		delete chaos;
		chaos = 0;
	}

	if (universe()) {
        universe()->allSavePlayReel();
        delete universe();
    }

    delete BuildingInstaller;
    BuildingInstaller = nullptr;

	terScene->Compact();

	if (soundPushedByPause) {
		soundPushedByPause = false;
		SNDPausePop();
		xassert(soundPushedPushLevel==SNDGetPushLevel());
		soundPushedPushLevel=INT_MIN;
	}

//	for (int i = SNDGetPushLevel(); i > 0; i--) {
//		SNDPausePop();
//	}
	SNDStopAll();
//	soundPushedByPause = false;
//	soundPushedPushLevel = INT_MIN;

	GameActive = false;
	MusicCloseWorld();
	cVisGeneric::SetAssertEnabled(true);
	debugPrm_.save();

#if 1 && defined(PERIMETER_DEBUG_ASSERT)
    std::vector<cIUnkClass*> allowed;
    allowed.push_back(terLight);
    terScene->CheckPendingObjects(allowed);
#endif
}

bool GameShell::universalSave(const char* name, bool userSave, MissionDescription* missionOutput)
{
	MTAutoSingleThread skip_assert;

    MissionDescription* mission;
    if (missionOutput) {
        *missionOutput = MissionDescription(CurrentMission);
        mission = missionOutput;
    } else {
        mission = new MissionDescription(CurrentMission);
    }
    mission->missionNumber = currentSingleProfile.getCurrentMissionNumber();
    mission->gameContent = mission->isCampaign() ? getGameContentCampaign() : terGameContentSelect;
    if (userSave) {
        mission->globalTime = global_time();
        mission->gameSpeed = game_speed ? game_speed : game_speed_to_resume;
        mission->gamePaused = !gamePausedByMenu && !game_speed;
    } else {
        mission->globalTime = 0;
        mission->gameSpeed = 1;
        mission->gamePaused = false;
    }
    mission->setSaveName(name ? name : "");
    bool result = universe()->universalSave(*mission, userSave);
    if (!mission->savePathKey().empty()) {
        scan_resource_paths(currentSingleProfile.getSavesDirectory());
    }
    if (!missionOutput) {
        //Delete mission since it was temporary
        delete mission;
    }
	return result;
}

void GameShell::NetQuant()
{
	if(NetClient) {
		NetClient->P2PIQuant();
	}
}

bool GameShell::LogicQuant()
{
	start_timer_auto(LogicQuant,STATISTICS_GROUP_TOTAL);
	bool resultTUQuant=true;
	if(GameActive){
		resultTUQuant=universe()->PrimaryQuant();
		getLogicUpdater().update();
		getLogicUpdater().exchange();
	}
	return resultTUQuant;
}


void GameShell::GraphQuant()
{
	if(GameActive){
		universe()->PrepareQuant();

		if(_pShellDispatcher->m_nEditRegion == editRegion1 && isControlPressed() && !isShiftPressed()) {
			toolzerSizeTrack = true;
			ToolzerSizeChangeQuant();
		} else {
			toolzerSizeTrack = false;
			CameraQuant();
		}
	}
//	else {
//		if(NetClient)
//			NetClient->refreshGameHostList();
//	}

	m_ShellDispatcher.quant(GameActive);
	_shellIconManager.quant( frame_time.delta() );
	Show();
	
	mousePositionDelta_ = Vect2f::ZERO;
    mousePositionRelative_ = Vect2f::ZERO;
}

bool GameShell::showEnergy() const 
{
	MTG();
	if(universe()->activePlayer() && universe()->activePlayer()->frame())
	{
		CSELECT_AUTOLOCK();
		const UnitList& select=universe()->select.GetSelectList();
		if (BuildingInstallerInited()) {
            return true;
        } else {
			UnitList::const_iterator ui;
			FOR_EACH(select,ui)
				if((*ui)->attr()->MakeEnergy > 0)
					return true;
		}
	}
	return false;
}

void TestText()
{
	cFont* pFont= terVisGeneric->CreateGameFont(sqshFontPopup, 70);
	cTexture* pTexture=terVisGeneric->CreateTexture("RESOURCE\\Models\\MENU\\Textures\\tv.avi");
	terRenderDevice->SetFont(pFont);

	float time=2.0f;
	float phase= xm::fmod(clockf() * 1e-3, static_cast<double>(time)) / time;

	cTexture *pTexture0,*pTexture1;
	pTexture0 = terVisGeneric->CreateTexture( "RESOURCE\\ICONS\\MAINMENU\\lightmap.tga" );
	pTexture1 = terVisGeneric->CreateTexture( "RESOURCE\\ICONS\\MAINMENU\\tv.avi" ); 
	terRenderDevice->DrawSprite2(600, 400, 128, 128,
		0, 0, 1, 1, 
		0, 0, 1, 1, 
		pTexture0, pTexture1,  1.0f,1.0f, phase );

	pTexture0->Release();
	pTexture1->Release();
/*
	terRenderDevice->OutText(512,320,"HELLO",sColor4f(0,1,0,1),0,ALPHA_BLEND,
		                 pTexture,COLOR_MOD,
						 Vect2f(0,0),
						 Vect2f(1/128.0f,1/128.0f),
						phase,0.5f);

*/
	terRenderDevice->SetFont(NULL);
	pFont->Release();
	pTexture->Release();
}

void GameShell::showWays() {
	int i;
	for (i = 0; i < FRAME_SLOTS_MAX; i++) {
		TerrainButtonData* slotData = &(getLogicUpdater().getLogicData()->slots[i]);
		if (slotData->unit) {
			slotData->unit->showPath(slotData->wayPoints);
		}
	}
	for (i = 0; i < 5; i++) {
		SquadPageData* page = &(getLogicUpdater().getLogicData()->squads[i]);
		if (page->squad) {
			page->squad->showPath(page->wayPoints, page->patrolPoints, page->hasAttackPoint ? (&page->attackPoint) : 0);
		}
	}
	if (getLogicUpdater().getLogicData()->frame) {
		getLogicUpdater().getLogicData()->frame->showPath(getLogicUpdater().getLogicData()->frameWayPoints);
	}
}

void renderEndScene() {
    //We need to flush, otherwise the primitive vertex will be after the cursor in the buffer
    terRenderDevice->FlushPrimitive2D();
    terRenderDevice->FlushPrimitive3D();
    //Draw cursor
    _shellCursorManager.draw();
    //End scene
    terRenderDevice->EndScene();
}

void GameShell::Show()
{
	start_timer_auto(GS_Show,STATISTICS_GROUP_TOTAL);
	frame_time.next_frame();

	if(GameActive){

		if(!isPaused())
			scale_time.next_frame();
		else
			scale_time.skip();

		{
			MetaRegionLock lock(_pShellDispatcher->regionMetaDispatcher());
			_pShellDispatcher->regionMetaDispatcher()->postOperateAnalyze();
		}

		terScene->dSetTime(scale_time.delta());

		terExternalQuant();
		
        if (BuildingInstaller) {
            BuildingInstaller->UpdateInfo(terCamera->GetCamera());
        }
		terScene->PreDraw(terCamera->GetCamera());

		m_ShellDispatcher.PreDraw(frame_time.delta());

		terRenderDevice->Fill(0,0,0);
		terRenderDevice->BeginScene();

		if(FogStart>=0 && FogEnd>0)//terFogStart=300,terFogEnd=1000
				terRenderDevice->SetGlobalFog(FogColor,Vect2f(FogStart,FogEnd));
		
		if(chaos)
			chaos->Draw();

		if(showWireFrame_)
			terRenderDevice->SetRenderState(RS_WIREFRAME,1);

		terScene->Draw(terCamera->GetCamera());

		if(showWireFrame_)
			terRenderDevice->SetRenderState(RS_WIREFRAME,0);

		if(interfaceShowFlag_)
			universe()->ShowInfo();

		showWays();		

		terRenderDevice->SetDrawTransform(terCamera->GetCamera());
		if(debug_show_mode){
            if (debug_show_mode != 2) {
                show_dispatcher.clear();
            }
			universe()->showDebugInfo();
			show_dispatcher.draw();
		}

		if(missionEditor_)
			missionEditor_->quant();

		terRenderDevice->FlushPrimitive3D();
		terRenderDevice->SetClipRect(0,0,terRenderDevice->GetSizeX(),terRenderDevice->GetSizeY());

		if (bgScene.ready()) {
			bgScene.quant(frame_time.delta());
			bgScene.preDraw();
			bgScene.draw();
			bgScene.postDraw();
		}

		_shellIconManager.draw();
		m_ShellDispatcher.draw();

		if (autoSwitchAIEnabled) {
			checkAutoswitchAI();
		}

		gb_VisGeneric->DrawInfo();
		ai_tile_map->drawWalkMap();
//		terCamera->GetCamera()->DrawTestGrid();
		HTManager::instance()->Show();

        renderEndScene();

		if(recordMovie_)
			makeMovieShot();

		terRenderDevice->Flush();

		m_ShellDispatcher.PostDraw();

		terScene->PostDraw(terCamera->GetCamera());

		if(debug_write_mode & DEBUG_WRITE_BODY_STATE)
			universe()->WriteDebugInfo();
		if(debug_write_mode & DEBUG_SHOW_WATCH)
			show_watch();
	}
	else{
		//quant
		if(bwScene.ready()){
			bwScene.quant(mousePosition(), frame_time.delta());
		} else if (!historyScene.ready()) {
		} else {
			historyScene.quant(mousePosition(), frame_time.delta());
		}

		if (bgScene.ready()) {
			bgScene.quant(frame_time.delta());
		}
		
		//draw
		terRenderDevice->Fill(0,0,0);
		terRenderDevice->BeginScene();

		if (bwScene.ready()) {
//			bwScene.quant(mousePosition(), frame_time.delta());
			if (bwScene.ready()) {
				bwScene.preDraw();
				bwScene.draw();
				bwScene.postDraw();
			}
		} else if (!historyScene.ready()) {
			terScene->dSetTime(frame_time.delta());
			terScene->PreDraw(terCamera->GetCamera());
			terScene->Draw(terCamera->GetCamera());
		} else {
//			historyScene.quant(mousePosition(), frame_time.delta());
			if (historyScene.ready()) {
				historyScene.preDraw();
				historyScene.draw();
				historyScene.postDraw();
			}
		}

		if (bgScene.ready()) {
//			bgScene.quant(frame_time.delta());
			bgScene.preDraw();
			bgScene.draw();
			bgScene.postDraw();
		}
		_shellIconManager.draw();

		//m_ShellDispatcher.draw();

        renderEndScene();

		terRenderDevice->Flush();
		terScene->PostDraw(terCamera->GetCamera());
	}


/*	if(terEnableGDIPixel)
	{
		terRenderDevice->OutText(0,0,"A",255,255,255,"Arial");
//		HDC hDC=GetDC(terRenderDevice->GetWindowHandle());
//		SetPixel(hDC,0,0,RGB(255,255,255));
	}
*/
}

//--------------------------------------------------------
inline int IsMapArea(const Vect2f& pos)
{
	if(!_shellIconManager.IsInterface())
		return 1;

	//return y < 580 && y > 10;
	return pos.y < 0.255 && pos.y > -0.487;
}

Vect2f GameShell::convert(int x, int y) const 
{
	return Vect2f(float(x)/float(windowClientSize().x) - 0.5f, float(y)/float(windowClientSize().y) - 0.5f);
}

Vect2i GameShell::convertToScreenAbsolute(const Vect2f& pos)
{
    return Vect2i((pos.x + 0.5f)*(float)windowClientSize().x, (pos.y + 0.5f)*(float)windowClientSize().y);
}

void GameShell::EventHandler(SDL_Event& event) {
    if (reelManager.isVisible()) {
        if (reelAbortEnabled) {
            switch (event.type) {
                case SDL_KEYUP: {
                    int key = sKey(event.key.keysym, true).fullkey;
                    if (key == VK_SPACE || key == VK_ESCAPE || key == VK_END) {
                        reelManager.hide();
                    }
                    break;
                }
                case SDL_MOUSEBUTTONUP:
                    if (event.button.button & (SDL_BUTTON_LMASK | SDL_BUTTON_MMASK | SDL_BUTTON_RMASK)) {
                        reelManager.hide();
                    }
                    break;
                default:
                    break;
            }
        }
        if (!GameContinue) {
            reelManager.hide();
        }
        return;
    }

/*

    switch(uMsg){
        case WM_MOUSELEAVE:
            MouseLeave();
            break;
    }
*/
    //Sets the SDL2 text input mode according to current text edit mode in UI
    if (_shellIconManager.isInEditMode() != SDL_IsTextInputActive()) {
        if (_shellIconManager.isInEditMode()) {
            SDL_StartTextInput();
        } else {
            SDL_StopTextInput();
        }
    }

    sKey s;
    switch (event.type) {
        case SDL_MOUSEBUTTONDOWN:
        case SDL_MOUSEBUTTONUP: {
            bool pressed = event.button.state == SDL_PRESSED;
            bool doubleClick = false;
            if (!pressed) {
                float dist = xm::abs(event.button.x - lastClickPosition.x) + xm::abs(event.button.y - lastClickPosition.y);
                doubleClick = (clockf() - lastClickTime) < doubleClickTime && dist < doubleClickDistance && lastClickButton == event.button.button;
                lastClickPosition.set(event.button.x, event.button.y);
                lastClickTime = clockf();
                if (doubleClick) lastClickTime -= doubleClickTime;
                lastClickButton = event.button.button;
            }
            Vect2f where = convert(event.button.x, event.button.y);
            //printf("M %fx%f B %dn", where.x, where.y, event.button.button, pressed);
            switch (event.button.button) {
                case SDL_BUTTON_LEFT:
                    if (doubleClick) {
                        MouseLeftPressed(where);
                        MouseLeftUnpressed(where);
                        MouseLeftDoubleClick(where);
                    } else {
                        if (pressed) {
                            MouseLeftPressed(where);
                        } else {
                            MouseLeftUnpressed(where);
                        }
                    }
                    break;
                case SDL_BUTTON_MIDDLE:
                    if (pressed) {
                        MouseMidPressed(where);
                    } else {
                        MouseMidUnpressed(where);
                    }
                    break;
                case SDL_BUTTON_RIGHT:
                    if (doubleClick) {
                        MouseRightPressed(where);
                        MouseRightUnpressed(where);
                        MouseRightDoubleClick(where);
                    } else {
                        if (pressed) {
                            MouseRightPressed(where);
                        } else {
                            MouseRightUnpressed(where);
                        }
                    }
                    break;
                default:
                    break;
            }
            break;
        }
        case SDL_MOUSEWHEEL: {
            bool normal = event.wheel.direction == SDL_MOUSEWHEEL_NORMAL;
            int delta = event.wheel.y * (normal ? 1 : -1);
            MouseWheel(delta);
            break;
        }
        case SDL_MOUSEMOTION: {
            Vect2f where = convert(event.motion.x, event.motion.y);
            //printf("M %fx%f\n", where.x, where.y * 100);
            MouseMove(where, Vect2f(
                static_cast<float>(event.motion.xrel),
                static_cast<float>(event.motion.yrel)
            ));
            break;
        }
        case SDL_KEYDOWN: {
        case SDL_KEYUP:
            bool editMode = _shellIconManager.isInEditMode();
            SDL_KeyboardEvent key = event.key;
            s = sKey(key.keysym, true);
            if (editMode && s.fullkey == ('V' | KBD_CTRL)) {
                //Pasting keycombo, discard normal keydown/up
                if (key.state == SDL_PRESSED && SDL_HasClipboardText()) {
                    char* text = SDL_GetClipboardText();
                    if (text && *text != '\0') {
                        std::string codepaged = convertToCodepage(text, getLocale());
                        for (auto& c : codepaged) {
                            if (0 <= c && c < ' ') break;
                            _shellIconManager.OnChar(c);
                        }
                    }
                    SDL_free(text);
                }
            } else {
                if (key.state == SDL_PRESSED) {
                    KeyPressed(s);

                    //We need to send certain keys when editing as SDL_TEXTINPUT don't receive them
                    if (editMode) {
                        switch (s.key) {
                            case VK_BACK:
                            case VK_RETURN:
                                _shellIconManager.OnChar(static_cast<char>(s.key));
                                break;
                            default:
                                break;
                        }
                    }
                } else {
                    KeyUnpressed(s);
                }
            }
            break;
        }
        case SDL_TEXTINPUT: {
            //NOTE: _shellIconManager.isInEditMode() is implicit here as SDL2 edit mode is changed accordingly
            //printf("TI %s\n", event.text.text);
            std::string key = convertToCodepage(event.text.text, getLocale());
            if (!key.empty()) {
                _shellIconManager.OnChar(key.front());
            }
            break;
        }
        case SDL_TEXTEDITING: {
            //printf("TE %s S %d L %d\n", event.edit.text,  event.edit.start, event.edit.length);
            break;
        }
        case SDL_WINDOWEVENT: {
            switch (event.window.event) {
                case SDL_WINDOWEVENT_FOCUS_GAINED: {
                    OnWindowActivate();
                    break;
                }
                default:
                    break;
            }
            break;
        }
        default: {
            break;
        }
    }
}

void GameShell::DebugCteateFilth(terFilthSpotID id)
{
	Vect3f v;
	if(universe()->worldPlayer() && terCamera->cursorTrace(mousePosition(),v))
	{
		terFilthSpot* p = safe_cast<terFilthSpot*>(universe()->worldPlayer()->buildUnit(UNIT_ATTRIBUTE_FILTH_SPOT));
		p->setPose(Se3f(QuatF::ID, v), true);
		p->SetFilthParamID(id);
		p->Start();
	}
}

bool GameShell::DebugKeyPressed(sKey& Key)
{
	MTAutoSingleThread mtAutoSkipAssert;

	switch(Key.fullkey){
	case VK_F1:
		showKeysHelp_ = !showKeysHelp_;
		return false;
	case VK_F9:
		terCamera->addCurrentToPath();
		break;
	case VK_F9 | KBD_SHIFT:
		terCamera->removeLastPointFromPath();
		break;
	case VK_F9 | KBD_CTRL | KBD_SHIFT:
		terCamera->erasePath();
		break;
    case 'C' | KBD_CTRL | KBD_SHIFT: {
        MissionDescription md;
        gameShell->universalSave(nullptr, true, &md);
        universe()->universalLoad(md, this->savePrm(), nullptr);
        break;
    }
    case 'V' | KBD_CTRL | KBD_SHIFT: {
        MissionDescription md;
        gameShell->universalSave(nullptr, true, &md);
        HTManager::instance()->setMissionToStart(md);
        break;
    }
	case VK_F9 | KBD_CTRL: {
		if(!terCamera->isPlayingBack()){
			const char* name = manualData().popupCameraSplineName();
			if(name){
				const SaveCameraSplineData* spline = manualData().findCameraSpline(name);
				if(spline)
					terCamera->loadPath(*spline, false);
			}
			terCamera->startReplayPath(CAMERA_REPLAY_DURATION, 10);
		} else {
			terCamera->stopReplayPath();
        }
		break;
		}
	case VK_F10 | KBD_CTRL: {
		savePrm().manualData.cameras.push_back(SaveCameraSplineData());
		terCamera->savePath(savePrm().manualData.cameras.back());
		XBuffer name;
		name < "Camera" <= static_cast<uint32_t>(manualData().cameras.size());
		savePrm().manualData.cameras.back().name = editText(name);
		SavePrm data;
		CurrentMission.loadMission(data);
		data.manualData = manualData();
		CurrentMission.saveMission(data, false);
		} break; 
		
	case KBD_CTRL | VK_F11:
		startStopRecordMovie();
		break;
	case VK_F12 | KBD_CTRL: 
		m_ShellDispatcher.OnInterfaceMessage(UNIVERSE_INTERFACE_MESSAGE_GAME_VICTORY);
		break; 
	case VK_F12 | KBD_SHIFT: 
		m_ShellDispatcher.OnInterfaceMessage(UNIVERSE_INTERFACE_MESSAGE_GAME_DEFEAT);
		break; 
	case VK_F5:
		{
			void UpdateRegionMap(int x1,int y1,int x2,int y2);
			vMap.toShowDbgInfo(!(vMap.IsShowDbgInfo()));
			UpdateRegionMap(0, 0, vMap.H_SIZE-1, vMap.V_SIZE-1);
		}
		break;
	case VK_F2:
			if(isShiftPressed())
				debug_write_mode ^= DEBUG_WRITE_BODY_STATE;
			else
				debug_write_mode ^= DEBUG_SHOW_WATCH;
		if(!debug_write_mode){
//			hide_debug_window();
            SDL_ShowCursor(SDL_FALSE);
			}
		break;
	case VK_F3:
		debug_show_mode ^= 1;
		break;

	case VK_F4 | KBD_SHIFT:
	case VK_F4:
		if(isAltPressed())
			break;
		editParameters();
		break;

#ifdef PERIMETER_DEBUG
	case VK_RETURN | KBD_CTRL: 
	case VK_RETURN | KBD_CTRL | KBD_SHIFT: {
		terRenderDevice->Flush(true);
        SDL_ShowCursor(SDL_TRUE);
		//setUseAlternativeNames(true);
#ifndef _FINAL_VERSION_
        //TODO Port TriggerChain to ingame dev UI instead of using win32 stuff
		static TriggerEditor triggerEditor(triggerInterface());
		TriggerChain* triggerChain = universe()->activePlayer()->getStrategyToEdit();
		if(triggerChain && triggerEditor.run(*triggerChain, hWndVisGeneric)){
			triggerChain->initializeTriggersAndLinks();
			triggerChain->save();
			triggerChain->buildLinks();

			SavePrm data;
			CurrentMission.loadMission(data);
			data.manualData = manualData();
			CurrentMission.saveMission(data, false);
		}
#endif

		terCamera->setFocus(HardwareCameraFocus);
        SDL_ShowCursor(SDL_FALSE);
		RestoreFocus();									
		break;
	}
#endif

	case VK_F4 | KBD_CTRL:
		if(!missionEditor_){
			setCutSceneMode(false, false);
			missionEditor_ = new MissionEditor;
		} else {
			delete missionEditor_;
			missionEditor_ = nullptr;
		}
		break;

	case VK_F6:
		terRenderDevice->Flush(true);
        SDL_ShowCursor(SDL_TRUE);
		profiler_start_stop();
        SDL_ShowCursor(SDL_FALSE);
		RestoreFocus();
		break;

	case 'N': 
		debug_variation = 1 - debug_variation;
		break;

	case 'M' | KBD_CTRL:
		rememberPlayerCamera(universe()->activePlayer(), "Camera");
		break;

	case 'M':
		terCamera->setRestriction(!terCamera->restricted());
		break;
	case 219:
		extern float terMapLevelLOD, terNearDistanceLOD;
		if(terMapLevelLOD>0) terMapLevelLOD-=5;
		if(terNearDistanceLOD>5) terNearDistanceLOD-=5;
		terVisGeneric->SetMapLevel(terMapLevelLOD);
		terVisGeneric->SetNearDistanceLOD(terNearDistanceLOD);
		break;
	case 221:
		extern float terMapLevelLOD, terNearDistanceLOD;
		if(terMapLevelLOD<100-5) 
			terMapLevelLOD += 5;
		if(terNearDistanceLOD<100-5) 
			terNearDistanceLOD += 5;
		terVisGeneric->SetMapLevel(terMapLevelLOD);
		terVisGeneric->SetNearDistanceLOD(terNearDistanceLOD );
		break;

	case 'S':
		if(!missionEditor()){
			std::string name = CurrentMission.savePathContent();
			size_t pos = name.rfind(PATH_SEP);
			if(pos != std::string::npos)
				name.erase(0, pos + 1);
			name = UserSingleProfile::getAllSavesDirectory() + name;
			universalSave(name.c_str(), true);
		}
		break;

	case 'S' | KBD_CTRL: {
		std::string saveName = CurrentMission.savePathContent();
		std::string savesDir = UserSingleProfile::getAllSavesDirectory();
		if(saveFileDialog(saveName, missionEditor() ? MISSIONS_PATH : savesDir.c_str(), "spg", "Mission Name")){
			size_t pos = saveName.rfind(convert_path_content("RESOURCE") + PATH_SEP);
			if(pos != std::string::npos)
				saveName.erase(0, pos);
			universalSave(saveName.c_str(), false);
		}
		break;
		}
    case 'S' | KBD_CTRL | KBD_SHIFT: {
        std::string saveName = CurrentMission.savePathContent();
        universalSave(saveName.c_str(), false);
    } break;

	case 'O' | KBD_CTRL: {
		std::string saveName = CurrentMission.savePathKey();
        std::string savesDir = UserSingleProfile::getAllSavesDirectory();
		if(openFileDialog(saveName, missionEditor() ? MISSIONS_PATH : savesDir.c_str(), "spg", "Mission Name")){
			size_t pos = saveName.rfind(convert_path_content("RESOURCE") + PATH_SEP);
			if(pos != std::string::npos)
				saveName.erase(0, pos);
			//Несколько кривой участок кода, не будет работать с HT
			HTManager::instance()->GameClose();
			HTManager::instance()->GameStart(MissionDescription(saveName.c_str()));
		}
		break;
		}

	case 'O' | KBD_CTRL | KBD_SHIFT: 
		HTManager::instance()->GameClose();
		HTManager::instance()->GameStart(CurrentMission);
		break;

		
	case VK_PAUSE|KBD_SHIFT:
		alwaysRun_ = !alwaysRun_;
		break;

	case 'X':
		universe()->SetActivePlayer(++activePlayerID_ %= universe()->Players.size() - 1);
		break;
	case 'X'|KBD_SHIFT:
		universe()->activePlayer()->setAI(!universe()->activePlayer()->isAI());
        break;
	case 'F':
		terShowFPS ^= 1;
//		gb_VisGeneric->SetShadowMapSelf4x4(terShowFPS);
/*
		if(terShowFPS)
			terVisGeneric->SetFontDirectory("russian");
		else
			terVisGeneric->SetFontDirectory("courier");
		terVisGeneric->ReloadAllFont();
*/
		break;
	case 'G':
		terEnableGDIPixel=!terEnableGDIPixel;
		gb_VisGeneric->SetShadowMapSelf4x4(terEnableGDIPixel);
//		toggleScriptReelEnabled();
		break;

	case 'K'|KBD_SHIFT:
		universe()->makeCommand(COMMAND_ID_PRODUCTION_PAUSE_OFF,0);
		break;
	case 'K':
		universe()->makeCommand(COMMAND_ID_PRODUCTION_PAUSE_ON,0);
		break;
	case 'L':
		universe()->switchFieldTransparency();
		break;
	case VK_SPACE:
		showWireFrame_ = !showWireFrame_;
		break;
	case VK_F8:
		_shellIconManager.Toggle(GameActive);
		interfaceShowFlag_ ^= 1;
		break;
	case VK_F7:
		interfaceShowFlag_ ^= 1;
		_shellIconManager.setCutSceneMode(!interfaceShowFlag_);
		break;
	case 'D':
		universe()->select.explodeUnit();
		break;
	case 'D' | KBD_CTRL | KBD_SHIFT:
		universe()->DeleteSelectedObjects();
		break;

	// Debug objects
	case 'Q':
		if(!terRenderDevice->IsFullScreen())
		{
			terFilthSpotID SelectFilth();
			terFilthSpotID id=SelectFilth();

			if(id!=FILTH_SPOT_ID_NONE)
			{
				DebugCteateFilth(id);
			}
		}
		break;
	case 'W':
		{
			Vect3f v;
			if(universe()->worldPlayer() && terCamera->cursorTrace(mousePosition(),v))
			{
				terUnitAttributeID SelectGeo();
				terUnitAttributeID id=SelectGeo();
				if(id!=UNIT_ATTRIBUTE_NONE)
				{
					terGeoControl* p = safe_cast<terGeoControl*>(universe()->worldPlayer()->buildUnit(id));
					p->setPose(Se3f(QuatF::ID, v), true);
					p->Start();
				}
			}
		}
		break;

/*
	//for ATI
	case '0'|KBD_CTRL:
		gb_VisGeneric->XorShowType(SHOW_INFO);
		break;
	case '1'|KBD_CTRL:
		gb_VisGeneric->XorShowType(SHOW_NORMAL);
		break;
	case '2'|KBD_CTRL:
		gb_VisGeneric->XorShowType(SHOW_SHADOW);
		break;
	case '3'|KBD_CTRL:
		gb_VisGeneric->XorShowType(SHOW_REDLECTION);
		break;
	case '4'|KBD_CTRL:
		gb_VisGeneric->XorShowType(SHOW_TILEMAP);
		break;
	case '5'|KBD_CTRL:
		gb_VisGeneric->XorShowType(SHOW_OBJECT);
		break;
*/		
	//end for ATI
	case 'Z':
		decrSpeed();
		break;
	case 'A':
		incrSpeed();
		break;
	case KBD_SHIFT | 'A':
		setSpeed(1);
		break;
	case 'P'|KBD_CTRL:
		debug_show_intf_borders ^= 1;
//		abnormalNetCenterTermination();
		break;
	default:
		return false;
	}
	return true;
}

void GameShell::KeyPressed(sKey& Key)
{
    if (Key.fullkey == (VK_ESCAPE|KBD_SHIFT|KBD_CTRL)) {
        //Restart game now
        request_application_restart();
        terminate();
        return;
    }
    
	if(missionEditor_ && missionEditor_->keyPressed(Key))
		return;

	if (Key.fullkey == (VK_F1|KBD_SHIFT|KBD_CTRL)) {
#ifndef PERIMETER_DEBUG
        if (check_command_line("debug_key_handler"))
#endif
        {
		    EnableDebugKeyHandlersInitial = EnableDebugKeyHandlers ^= 1;
        }
		return;
	}

	if(_bMenuMode){
		if(EnableDebugKeyHandlers){
			if(Key.fullkey == VK_F4 && reload_parameters()){
				_shellIconManager.LoadControlsGroup(SHELL_LOAD_GROUP_MENU, true);
				_shellIconManager.SwitchMenuScreens(-1000, reloadID);
			} else if (Key.fullkey == ('P'|KBD_CTRL)) {
				debug_show_intf_borders ^= 1;
//				abnormalNetCenterTermination();
			}
		}
//		if (reelManager.isVisible()) {
//			reelManager.hide();
//		}
 		_shellIconManager.OnKeyDown(Key.fullkey);
		return;
	}

	if (isScriptReelEnabled()) {
 		_shellIconManager.OnKeyDown(Key.fullkey);
		return;
	}

	if (EnableDebugKeyHandlers) {
		if (DebugKeyPressed(Key)) {
			return;
		}
	}

	if (_shellIconManager.isCutSceneMode() && (Key.fullkey == VK_SPACE || Key.fullkey == VK_RETURN || Key.fullkey == VK_ESCAPE)) {
		setSkipCutScene(true);
		return;
	}

	if (gameShell->currentSingleProfile.getLastGameType() == UserSingleProfile::MULTIPLAYER) {
		CChatInGameEditWindow* chatEdit = (CChatInGameEditWindow*) _shellIconManager.GetWnd(SQSH_INGAME_CHAT_EDIT_ID);
		CChatInfoWindow* chatInfo = (CChatInfoWindow*) _shellIconManager.GetWnd(SQSH_CHAT_INFO_ID);
		if (Key.fullkey == VK_INSERT) {
			if (chatEdit->isVisible()) {
				if (!chatEdit->alliesOnlyMode) {
					chatEdit->alliesOnlyMode = true;
				} else {
					chatEdit->Show(0);
					_shellIconManager.SetFocus(0);
					chatInfo->setTime(CHATINFO_VISIBLE_TIME_AFTER_HIDE_EDIT);
				}
			} else {
				_shellIconManager.SetFocus(SQSH_INGAME_CHAT_EDIT_ID);
				chatEdit->Show(1);
				chatInfo->setTime(-1);
				chatInfo->Show(1);
				chatEdit->alliesOnlyMode = true;
			}
			return;
		} else if (Key.fullkey == (VK_INSERT | KBD_CTRL) || Key.fullkey == (VK_SPACE | KBD_CTRL)) {
			if (chatEdit->isVisible()) {
				if (chatEdit->alliesOnlyMode) {
					chatEdit->alliesOnlyMode = false;
				} else {
					chatEdit->Show(0);
					_shellIconManager.SetFocus(0);
					chatInfo->setTime(CHATINFO_VISIBLE_TIME_AFTER_HIDE_EDIT);
				}
			} else {
				_shellIconManager.SetFocus(SQSH_INGAME_CHAT_EDIT_ID);
				chatEdit->Show(1);
				chatInfo->setTime(-1);
				chatInfo->Show(1);
				chatEdit->alliesOnlyMode = false;
			}
			return;
		} else if (chatEdit->isVisible()) {
			if (Key.fullkey == VK_ESCAPE || (Key.fullkey == VK_RETURN && chatEdit->isEmptyText())) {
				chatEdit->Show(0);
				_shellIconManager.SetFocus(0);
				chatInfo->setTime(CHATINFO_VISIBLE_TIME_AFTER_HIDE_EDIT);
			}
			return;
		}
	}

	switch(Key.fullkey)
	{
		case VK_PAUSE:
			if(!_shellIconManager.isCutSceneMode() && currentSingleProfile.getLastGameType() != UserSingleProfile::MULTIPLAYER) {
				if(!isPaused())
					pauseGame();
				else
					resumeGame();
			}
			return;

		case VK_F11:
			MakeShot();
			break;
	}

	ControlPressed(Key.fullkey);
	hotKeyManager->keyPressed(Key.fullkey);
}

void GameShell::ControlPressed(int key)
{
	if (isScriptReelEnabled()) {
		return;
	}
	if (_shellIconManager.IsInterface()) {

		if (key >= '0' && key <= '9') {
			if (universe()) {
				universe()->select.selectGroup(key - '0');
			}
			return;
		} else if (key >= ('0' + KBD_CTRL) && key <= ('9' + KBD_CTRL)) {
			if (universe()) {
				universe()->select.putCurrentSelectionToGroup(key - ('0' + KBD_CTRL));
			}
			return;
		} else if (key >= ('0' + KBD_SHIFT) && key <= ('9' + KBD_SHIFT)) {
			if (universe()) {
				universe()->select.addCurrentSelectionToGroup(key - ('0' + KBD_SHIFT));
			}
			return;
		}
	}

	int ctrl = g_controls_converter.control(key);
	if (ctrl == CTRL_ESCAPE) {
		//temp
		if(_shellIconManager.IsInterface() && interfaceShowFlag_)
		{
			//if(MainMenuEnable)
				EnterInMissionMenu();
			//else if(!missionEditor() && !NetClient)
			//	GameContinue = false;
		}
		return;
	}
	if (_shellIconManager.isCutSceneMode()) {
		return;
	}
	switch(ctrl)
	{
		case CTRL_TIME_NORMAL:
			if (currentSingleProfile.getLastGameType() != UserSingleProfile::MULTIPLAYER) {
				_shellIconManager.setNormalSpeed();
			}
			break;
		case CTRL_TIME_DEC:
			if (currentSingleProfile.getLastGameType() != UserSingleProfile::MULTIPLAYER) {
				_shellIconManager.decrSpeedStep();
			}
			break;
		case CTRL_TIME_INC:
			if (currentSingleProfile.getLastGameType() != UserSingleProfile::MULTIPLAYER) {
				_shellIconManager.incrSpeedStep();
			}
			break;

		case CTRL_CAMERA_SAVE1:
		case CTRL_CAMERA_SAVE2:
		case CTRL_CAMERA_SAVE3:
		case CTRL_CAMERA_SAVE4:
		case CTRL_CAMERA_SAVE5:
			terCamera->SaveCamera(ctrl - CTRL_CAMERA_SAVE1);
			break;
		case CTRL_CAMERA_RESTORE1:
		case CTRL_CAMERA_RESTORE2:
		case CTRL_CAMERA_RESTORE3:
		case CTRL_CAMERA_RESTORE4:
		case CTRL_CAMERA_RESTORE5:
			terCamera->RestoreCamera(ctrl - CTRL_CAMERA_RESTORE1);
			break;

		case CTRL_CAMERA_MOUSE_LOOK:
			if(IsMapArea(mousePosition()))
				if(!cameraMouseTrack)
				{
					cameraMouseTrack = true;
					mousePressControl_ = mousePosition();
					_shellCursorManager.HideCursor();
                    SDL_SetRelativeMouseMode(SDL_TRUE);
				}
			break;

		case CTRL_CAMERA_MAP_SHIFT:
			if(!cameraMouseShift){
				cameraMouseShift = true;
				_shellCursorManager.HideCursor();
				
				terCamera->cursorTrace(mousePosition(), mapMoveStartPoint_);
			}
			break;

		case CTRL_CAMERA_TO_EVENT:
			if (_shellIconManager.getMiniMapEventIcons().size()) {
				terCamera->setCoordinate(CameraCoordinate(_shellIconManager.getMiniMapEventIcons().back().pos, terCamera->coordinate().psi(), terCamera->coordinate().theta(), terCamera->coordinate().distance()));
			}
			break;
		case CTRL_TOGGLE_MUSIC:
			InitSound(terSoundEnable, !terMusicEnable, false);
			MusicEnable( terMusicEnable );
			break;
		case CTRL_TOGGLE_SOUND:
			InitSound(!terSoundEnable, terMusicEnable, false);
			historyScene.setupAudio();
			_shellIconManager.setupAudio();
			break;
		case CTRL_LOAD:
			prepareForInGameMenu();
			_shellIconManager.SwitchMenuScreens(-1, SQSH_MM_LOAD_IN_GAME_SCR);
			break;
		case CTRL_SAVE:
			prepareForInGameMenu();
			_shellIconManager.SwitchMenuScreens(-1, SQSH_MM_SAVE_GAME_SCR);
			break;
		case CTRL_HOLD_PRODUCTION:
			universe()->toggleHold();
			break;
		case CTRL_TOGGLE_LIFEBARS:
			m_ShellDispatcher.toggleAlwaysShowLifebars();
			break;
	}
}

void GameShell::KeyUnpressed(sKey& Key)
{
	if(missionEditor_ && missionEditor_->keyUnpressed(Key))
		return;

	if(_bMenuMode){
		_shellIconManager.OnKeyUp(Key.fullkey);
		return;
	}

	if (isScriptReelEnabled()) {
 		_shellIconManager.OnKeyDown(Key.fullkey);
		return;
	}

	ControlUnpressed(Key.fullkey);
	
	if(Key.fullkey == VK_SHIFT)
		bWasShiftUnpressed = true;
}

void GameShell::ControlUnpressed(int key)
{
	if (isScriptReelEnabled()) {
		return;
	}
	if (_shellIconManager.isCutSceneMode()) {
		return;
	}
	int ctrl = g_controls_converter.control(key);
	switch(ctrl)
	{
		case CTRL_CAMERA_MOUSE_LOOK:
			cancelMouseLook();
			break;

		case CTRL_CAMERA_MAP_SHIFT:
			if(cameraMouseShift){
				cameraMouseShift = false;
				
				if(_shellIconManager.IsInterface())
					_shellCursorManager.ShowCursor();
				
				//восстановить положение курсора
				//Vect2f v;
				//ConvertWorldToScreen(_MapMoveStartPoint, v);
				//SetCursorPos(v.x, v.y);
				
			}
			break;
	}
}

void GameShell::cancelMouseLook() {
	if(cameraMouseTrack)
	{
		cameraMouseTrack = false;
        SDL_SetRelativeMouseMode(SDL_FALSE);
		setCursorPosition(mousePressControl_);

		if(_shellIconManager.IsInterface())
			_shellCursorManager.ShowCursor();
	}
}

void GameShell::updatePosition() 
{
	_shellCursorManager.OnMouseMove(mousePosition().x+0.5f, mousePosition().y+0.5f);
    if (BuildingInstallerInited()) {
        BuildingInstaller->SetBuildPosition(Vect2f(mousePosition().x, mousePosition().y), universe()->activePlayer());
    }
}

void GameShell::MouseMove(const Vect2f& pos, const Vect2f& rel)
{
	if(missionEditor_ && missionEditor_->mouseMove(pos))
		return;

	cameraCursorInWindow = true;

	if(MousePositionLock){
		mousePosition_ = pos;
		mousePositionDelta_ = Vect2f::ZERO;
        mousePositionRelative_ = Vect2f::ZERO;
	}
	else{
        mousePositionDelta_ = pos - mousePosition(); //Do this before setting the current position so we can get Delta
		mousePosition_ = pos;
        mousePositionRelative_ = rel;
		MouseMoveFlag = 1;
	}

	if (BuildingInstallerInited()){
		if(isShiftPressed()) {
            BuildingInstaller->ChangeBuildAngle(mousePositionDelta().y * 50, universe()->activePlayer());
        } else {
            BuildingInstaller->SetBuildPosition(pos, universe()->activePlayer());
        }
	}

	_shellCursorManager.OnMouseMove(mousePosition().x+0.5f, mousePosition().y+0.5f);

	if(!cameraMouseZoom && !cameraMouseShift && !cameraMouseTrack && !toolzerSizeTrack){
		if(_pShellDispatcher->m_nState != STATE_TRACKING)
			CursorOverInterface = _shellIconManager.OnMouseMove(mousePosition().x+0.5f, mousePosition().y+0.5f);

		m_ShellDispatcher.OnMouseMove(mousePosition().x+0.5f, mousePosition().y+0.5f);
	}
}

void GameShell::MouseMidPressed(const Vect2f& pos) {
	if(!_bMenuMode){
		ControlPressed(sKey(VK_MBUTTON, true).fullkey);
	}
}
void GameShell::MouseMidUnpressed(const Vect2f& pos) {
	if(!_bMenuMode){
		ControlUnpressed(sKey(VK_MBUTTON, true).fullkey);
	}
}

void GameShell::MouseLeftPressed(const Vect2f& pos)
{
	if (!_bMenuMode && isPressed(VK_RBUTTON)) {
		ControlPressed(sKey(VK_MBUTTON, true).fullkey);
	}

	if(missionEditor_ && missionEditor_->mouseLeftPressed(pos))
		return;

	if (universe()) {
        Event ev(Event::MOUSE_CLICK);
		universe()->checkEvent(&ev);
	}

	if (autoSwitchAIEnabled) {
		setActivePlayerAIOff();
	}

	ControlPressed(sKey(VK_LBUTTON, true).fullkey);

	if(!mouseLeftPressed())
	{
		mouseLeftPressed_ = true;
		mousePositionDelta_ = pos - mousePosition();
		mousePosition_= pos;

		if(!cameraMouseZoom && !cameraMouseShift && !cameraMouseTrack && !toolzerSizeTrack)
		{
			if(_shellIconManager.IsInterface())
			{
				if(_shellIconManager.OnLButtonDown(mousePosition().x+0.5f, mousePosition().y+0.5f))
					return;

				m_ShellDispatcher.OnLButtonDown(mousePosition().x+0.5f, mousePosition().y+0.5f);
			}
		}
	}
	if (BuildingInstallerInited()){
		SND2DPlaySound(BuildingInstaller->valid() ? "building_set" : "unable_build");
		BuildingInstaller->ConstructObject(universe()->activePlayer());
	}
}

void GameShell::MouseRightPressed(const Vect2f& pos)
{

	if (!_bMenuMode && isPressed(VK_LBUTTON)) {
		ControlPressed(sKey(VK_MBUTTON, true).fullkey);
	}

	if(missionEditor_ && missionEditor_->mouseRightPressed(pos))
		return;

	if (autoSwitchAIEnabled) {
		setActivePlayerAIOff();
	}

	ControlPressed(sKey(VK_RBUTTON, true).fullkey);

	if(!mouseRightPressed())
	{
		mouseRightPressed_ = true;
		mousePositionDelta_ = pos - mousePosition();
		mousePosition_ = pos;

		if (BuildingInstallerInited()) {
            BuildingInstaller->CancelObject();
        }

		//if(!cameraMouseZoom && !cameraMouseShift && !cameraMouseTrack && !toolzerSizeTrack)
		if(!cameraMouseZoom && !cameraMouseShift)
		{
			if(_shellIconManager.IsInterface())
			{
				if(_shellIconManager.OnRButtonDown(mousePosition().x+0.5f, mousePosition().y+0.5f))
					return;
				m_ShellDispatcher.OnRButtonDown(mousePosition().x+0.5f, mousePosition().y+0.5f);
			}
		}
	}
}

void GameShell::MouseLeftUnpressed(const Vect2f& pos)
{
	if (!_bMenuMode && cameraMouseTrack) {
		ControlUnpressed(sKey(VK_MBUTTON, true).fullkey);
	}

	ControlUnpressed(sKey(VK_LBUTTON, true).fullkey);

	if(mouseLeftPressed()){
		mouseLeftPressed_ = false;
		mousePositionDelta_ = pos - mousePosition();
		mousePosition_ = pos;

//		if(BuildingInstaller.inited()){
//			SND2DPlaySound(BuildingInstaller.valid() ? "building_set" : "unable_build");
//			BuildingInstaller.ConstructObject(universe()->activePlayer());
//		}

		if(!cameraMouseZoom && !cameraMouseShift && !cameraMouseTrack && !toolzerSizeTrack && _shellIconManager.IsInterface()){
			if(_pShellDispatcher->m_nState != STATE_TRACKING 
				&& _shellIconManager.OnLButtonUp(mousePosition().x+0.5f, mousePosition().y+0.5f))
					return;
			_shellIconManager.lButtonReset();
			m_ShellDispatcher.OnLButtonUp(mousePosition().x+0.5f, mousePosition().y+0.5f);
		}
	}
}

void GameShell::MouseRightUnpressed(const Vect2f& pos)
{
	if (!_bMenuMode && cameraMouseTrack) {
		ControlUnpressed(sKey(VK_MBUTTON, true).fullkey);
	}

	ControlUnpressed(sKey(VK_RBUTTON, true).fullkey);

	if(mouseRightPressed())
	{
		mouseRightPressed_ = false;
		mousePositionDelta_ = pos - mousePosition();
		mousePosition_ = pos;

		//if(!cameraMouseZoom && !cameraMouseShift && !cameraMouseTrack && !toolzerSizeTrack)
		{
			if(_shellIconManager.IsInterface())
			{
				if(_shellIconManager.OnRButtonUp(mousePosition().x+0.5f, mousePosition().y+0.5f))
					return;
				m_ShellDispatcher.OnRButtonUp(mousePosition().x+0.5f, mousePosition().y+0.5f);
			}
		}
	}
}

void GameShell::MouseWheel(int delta)
{
	if(!_bMenuMode && GameActive && !isScriptReelEnabled()) {
        CChatInfoWindow* chatInfo = (CChatInfoWindow*) _shellIconManager.GetWnd(SQSH_CHAT_INFO_ID);
        if (!chatInfo || !chatInfo->isVisible() || !chatInfo->HitTest(mousePosition().x+0.5f, mousePosition().y+0.5f)) {
            terCamera->mouseWheel(delta);
        }
    }
	if (historyScene.ready()) {
		historyScene.getCamera()->mouseWheel(delta);
	}

	_shellIconManager.OnMouseWheel( delta );

	m_ShellDispatcher.OnMouseMove(mousePosition().x+0.5f, mousePosition().y+0.5f);
	_shellCursorManager.OnMouseMove(mousePosition().x+0.5f, mousePosition().y+0.5f);
}
void GameShell::MouseLeave()
{
	cameraCursorInWindow = false;
}
void GameShell::OnWindowActivate()
{
	cameraMouseShift = false;
	cameraMouseTrack = false;
	toolzerSizeTrack = false;
	 _shellCursorManager.ShowCursor();
}

void GameShell::MouseLeftDoubleClick(const Vect2f& pos)
{
	mousePositionDelta_ = pos - mousePosition();
	mousePosition_ = pos;

	if(_shellIconManager.IsInterface())
	{
		if (_shellIconManager.OnLButtonDblClk(mousePosition().x+0.5f, mousePosition().y+0.5f))
			return;
		m_ShellDispatcher.OnLButtonDblClk(mousePosition().x+0.5f, mousePosition().y+0.5f);
	}
}

void GameShell::MouseRightDoubleClick(const Vect2f& pos)
{
	mousePositionDelta_ = pos - mousePosition();
	mousePosition_ = pos;

	if(_shellIconManager.IsInterface())
		m_ShellDispatcher.OnRButtonDblClk(mousePosition().x+0.5f, mousePosition().y+0.5f);
}


//----------------------------------

void GameShell::ShotsScan()
{
	shotNumber_ = 0;

    std::string path_str = convert_path_native(terScreenShotsPath);
    create_directories(path_str);

    std::vector<std::string> paths;
    for (const auto & entry : std::filesystem::directory_iterator(std::filesystem::u8path(path_str))) {
        std::string entry_path = entry.path().u8string();
        if (endsWith(entry_path, terScreenShotExt)) {
            const char* p = strstr(entry_path.c_str(), terScreenShotName);
            if(p){
                p += strlen(terScreenShotName);
                if(isdigit(*p)){
                    int t = atoi(p) + 1;
                    if(shotNumber_ < t)
                        shotNumber_ = t;
                }
            }
        }
    }
}

void GameShell::MakeShot()
{
	if(shotNumber_ == -1)
		ShotsScan();
	XBuffer fname;
	fname <= shotNumber_/1000 % 10 <= shotNumber_/100 % 10 <= shotNumber_/10 % 10 <= shotNumber_ % 10 < terScreenShotExt;
	shotNumber_++;
    std::string path = convert_path_native(terScreenShotsPath) + PATH_SEP + terScreenShotName + fname.address();
	terRenderDevice->SetScreenShot(path.c_str());
}

void GameShell::startStopRecordMovie()
{
	recordMovie_ = !recordMovie_;

	if(recordMovie_){
		HTManager::instance()->setSyncroTimer(0, framePeriod_, terMaxTimeInterval);
		frame_time.set(0, framePeriod_, terMaxTimeInterval);
		scale_time.set(0, framePeriod_, terMaxTimeInterval);

		movieShotNumber_ = 0;
		movieStartTime_ = frame_time();
		
        std::string path_str = convert_path_native(terMoviePath);
        if (path_str.empty()) return;
        create_directories(path_str);

        int movieNumber = 0;
        std::vector<std::string> paths;
        for (const auto & entry : std::filesystem::directory_iterator(std::filesystem::u8path(path_str))) {
            std::string entry_path = entry.path().u8string();
            if (startsWith(entry_path, terMovieName)) {
				const char* p = strstr(entry_path.c_str(), terMovieName);
				if(p){
					p += strlen(terMovieName);
					if(isdigit(*p)){
						int t = atoi(p) + 1;
						if(movieNumber < t)
							movieNumber = t;
					}
				}
			}
		}
    
		XBuffer buffer;
		buffer < path_str.c_str() < PATH_SEP < terMovieName <= movieNumber/10 % 10 <= movieNumber % 10;
		movieName_ = buffer;
        create_directories(movieName_);
	} 
	else{
		HTManager::instance()->setSyncroTimer(synchroByClock_, framePeriod_, terMaxTimeInterval);
		frame_time.set(synchroByClock_, framePeriod_, terMaxTimeInterval);
		scale_time.set(synchroByClock_, framePeriod_, terMaxTimeInterval);
	}
}

void GameShell::makeMovieShot()
{
	XBuffer fname;
	fname <= movieShotNumber_/1000 % 10 <= movieShotNumber_/100 % 10 <= movieShotNumber_/10 % 10 <= movieShotNumber_ % 10 < terScreenShotExt;
    shotNumber_++;
    std::string path = movieName_ + PATH_SEP + terMovieFrameName + fname.address();
	movieShotNumber_++;
	terRenderDevice->SetScreenShot(path.c_str());

	terRenderDevice->BeginScene();
	terRenderDevice->SetFont(_pShellDispatcher->getFont());
	XBuffer msg;
	msg.SetDigits(3);
	int time = frame_time() - movieStartTime_;
	int ms = time % 1000;
	msg < "REC: " < fname < " \nTime: " <= time/60000 < ":" <= (time % 60000)/1000 < "." <= ms/100 % 10 <= ms/10 % 10 <= ms % 10;
    sColor4f c(1, 1, 1, 1);
	OutText(20, 20, msg, &c);
	terRenderDevice->SetFont(0);
	terRenderDevice->EndScene();
}


//----------------------------

void GameShell::CameraQuant()
{
	if(!cameraMouseTrack && cameraCursorInWindow && !_bMenuMode && 
		!cameraMouseShift && !cameraMouseZoom && !isScriptReelEnabled()){
		//сдвиг когда курсор у края окна
		//if(!CursorOverInterface)
		terCamera->mouseQuant(mousePosition());
	}
	
	MousePositionLock = 0;
	
	//поворот вслед за мышью
	if(cameraMouseTrack && MouseMoveFlag){
		terCamera->tilt(mousePositionRelative_);
		
		MousePositionLock = 1;
		setCursorPosition(Vect2f::ZERO);
	}
	
	//смещение вслед за мышью
	if(cameraMouseShift && MouseMoveFlag){
		terCamera->shift(mousePositionDelta());
		setCursorPosition(mapMoveStartPoint());
		MousePositionLock = 1;
	}

    float delta = frame_time.delta() / 1000.0f;// * PerimeterCameraControlFPS / 1000.0f;
	terCamera->quant(mousePositionDelta().x, mousePositionDelta().y, delta, cameraMouseTrack && MouseMoveFlag);
	
//	mousePositionDelta_ = Vect2f::ZERO;
	MouseMoveFlag = 0;

	if (!_bMenuMode && !isScriptReelEnabled()) {
		terCamera->controlQuant();
	}
}

void terGameShellShowRegionMain()
{
	if(_pShellDispatcher->ShowTerraform())
	{
		MetaRegionLock lock(_pShellDispatcher->regionMetaDispatcher());
		MetaLockRegionDispatcher region=(*_pShellDispatcher->regionMetaDispatcher())[0];
		terExternalRegionShowLineZeroplast(region.data(),gb_RenderDevice->ConvertColor(sColor4c(RegionMain.line_color)));
	}
}

void terGameShellShowRegionMainAlpha()
{
	if(_pShellDispatcher->ShowTerraform())
	{
		MetaRegionLock lock(_pShellDispatcher->regionMetaDispatcher());
		MetaLockRegionDispatcher region=(*_pShellDispatcher->regionMetaDispatcher())[0];
		terExternalRegionShowLineZeroplastVertical(region.data(),gb_RenderDevice->ConvertColor(sColor4c(RegionMain.vertical_color)));
		Column& c=region->getRasterizeColumn();
		terExternalRegionShowColumn(&c,gb_RenderDevice->ConvertColor(sColor4c(RegionMain.area_color)));
	}
}

void terGameShellShowRegionAbyss()
{
	if(_pShellDispatcher->ShowTerraform())
	{
		MetaRegionLock lock(_pShellDispatcher->regionMetaDispatcher());
		MetaLockRegionDispatcher region=(*_pShellDispatcher->regionMetaDispatcher())[1];
		terExternalRegionShowUniform(region.data(),gb_RenderDevice->ConvertColor(sColor4c(255,255,255)));
	}
}

void terGameShellShowEnergy()
{
	MTG();
	if(gameShell->showEnergy())
	{
		terPlayer* player=universe()->activePlayer();
		terUniverse* tu=universe();
		MTAuto lock(universe()->EnergyRegionLocker());
		terExternalRegionShowLine(&player->energyRegion(),gb_RenderDevice->ConvertColor(sColor4c(255,255,255)));
	}
}

//-----------------------------------------------
void CShellLogicDispatcher::init()
{
	alwaysShowLifeBars = IniManager("Perimeter.ini", false).getInt("Game","ShowLifeBars");
	m_pShowExternal[GAME_SHELL_SHOW_REGION_MAIN] = terScene->CreateExternalObj(terGameShellShowRegionMain,RegionMain.texture);
	m_pShowExternal[GAME_SHELL_SHOW_REGION_MAIN_ALPHA] = terScene->CreateExternalObj(terGameShellShowRegionMainAlpha,sRegionTextureEnergy);
	m_pShowExternal[GAME_SHELL_SHOW_REGION_MAIN_ALPHA]->SetSortPass(true);
	m_pShowExternal[GAME_SHELL_SHOW_REGION_ABYSS] = terScene->CreateExternalObj(terGameShellShowRegionAbyss,sRegionTextureAbyss);
	m_pShowExternal[GAME_SHELL_SHOW_ENERGY] = terScene->CreateExternalObj(terGameShellShowEnergy,sRegionTextureEnergy);

	delete pColumnMain;
	pColumnMain=new terRegionColumnMain;

	delete gbCircleShow;
	gbCircleShow=new cCircleShow;
	
	initFonts();

	//камера и сцена для моделей в окошке
	m_hScene = terVisGeneric->CreateScene();

	m_hLight = m_hScene->CreateLight(ATTRLIGHT_DIRECTION);
	m_hLight->SetPosition(MatXf(Mat3f::ID,Vect3f(0,0,0)));
    sColor4f a(0,0,0,1);
    sColor4f b(1,1,1,1);
	m_hLight->SetColor(&a,&b);
	m_hLight->SetDirection(Vect3f(0,0,-1));

	m_hCamera = m_hScene->CreateCamera();
	m_hCamera->SetAttr(ATTRCAMERA_PERSPECTIVE); // перспектива
    m_hCamera->SetAttr(ATTRCAMERA_CLEARZBUFFER);

	MatXf CameraMatrix;
	Identity(CameraMatrix);
	Vect3f CameraPos(0, 0, -1024);
	SetPosition(CameraMatrix,CameraPos,Vect3f(0,0,0));
	SetCameraPosition(m_hCamera, CameraMatrix);
    
    updateSmallCamera();
}

void CShellLogicDispatcher::updateSmallCamera() {
    if (m_hCamera) {
        //float _small_camera_x = small_camera_x*float(terScreenSizeX);
        //float _small_camera_y = small_camera_y*float(terScreenSizeY);
        //float _small_camera_rect_dx  = small_camera_rect_dx*float(terScreenSizeX)/2.f;
        //float _small_camera_rect_dy  = small_camera_rect_dy*float(terScreenSizeY)/2.f;

        //m_hCamera handles the small 3D view of selected stuff
        //clip takes screen relative position but hardcoded positions are relative to 4:3
        //so we just convert X stuff to absolute and then make it relative to screen position
        //this way they are properly positioned/sized in different resolutions

        float renderX = static_cast<float>(terRenderDevice->GetSizeX());
        float _small_camera_x = absoluteUIPosX(small_camera_x, SHELL_ANCHOR_DEFAULT) / renderX;
        float _small_camera_y = small_camera_y;
        float _small_camera_rect_dx  = (absoluteUISizeX(small_camera_rect_dx, SHELL_ANCHOR_DEFAULT) / renderX)/2.f;
        float _small_camera_rect_dy  = small_camera_rect_dy/2.f;

        Vect2f center(_small_camera_x + _small_camera_rect_dx,
                      _small_camera_y + _small_camera_rect_dy);
        sRectangle4f clip(-_small_camera_rect_dx, -_small_camera_rect_dy,
                          _small_camera_rect_dx, _small_camera_rect_dy);
        Vect2f focus(1.0f, 1.0f);
        Vect2f zplane(30.0f, 10000.0f);
        m_hCamera->SetFrustum(                          // устанавливается пирамида видимости
                &center,								// центр камеры
                &clip,									// видимая область камеры
                &focus,									// фокус камеры
                &zplane									// ближайший и дальний z-плоскости отсечения
        );
	}
}

void CShellLogicDispatcher::initFonts() {
	_RELEASE(m_hFontUnitsLabel);
	m_hFontUnitsLabel = terVisGeneric->CreateGameFont(sqshShellMainFont1);
}

void CShellLogicDispatcher::close()
{
	for(int i = 0;i < GAME_SHELL_SHOW_MAX;i++){
		if(m_pShowExternal[i])
			m_pShowExternal[i]->Release();
		m_pShowExternal[i] = NULL;
	}

	delete gbCircleShow;
	gbCircleShow=NULL;
	delete pColumnMain;
	pColumnMain=NULL;

	_RELEASE(m_hFontUnitsLabel);

	_RELEASE(m_hModel);
	_RELEASE(m_hCamera);
	_RELEASE(m_hLight);
	_RELEASE(m_hScene);
}

void CShellLogicDispatcher::RegionEndEdit()
{
	XBuffer buffer(8192, true);
    auto regionDispatcher = regionMetaDispatcher();
    regionDispatcher->saveEditing(buffer);
    universe()->activePlayer()->ChangeRegion(buffer);
	buffer.set(0);
    regionDispatcher->loadEditing(buffer);
}

bool CShellLogicDispatcher::ShowTerraform() const
{
	if(universe()->activePlayer()){
		if(m_nEditRegion != editRegionNone)
			return true;
		CSELECT_AUTOLOCK();
		const UnitList& select_list=universe()->select.GetSelectList();
		UnitList::const_iterator ui;
		FOR_EACH(select_list, ui)
		{
			terUnitBase* unit=*ui;
			if(unit->attr()->ID == UNIT_ATTRIBUTE_TERRAIN_MASTER)
				return true;
		}
	}
	return false;
}


//------------------------------------------
void GameShell::initResourceDispatcher()
{
	synchroByClock_ = IniManager("Perimeter.ini").getInt("Timer","SynchroByClock");
    int sfr = IniManager("Perimeter.ini").getInt("Timer","StandartFrameRate");
	framePeriod_ = 1000/sfr;

	check_command_line_parameter("synchro_by_clock", synchroByClock_);

	if(const char* s = check_command_line("fps"))
		framePeriod_ = 1000/atoi(s);

	global_time.set(0,terLogicTimePeriod,1000);
	if(check_command_line("synchro_by_clock"))
		synchroByClock_ = 1;
	frame_time.set(synchroByClock_, framePeriod_, terMaxTimeInterval);
	scale_time.set(synchroByClock_, framePeriod_, terMaxTimeInterval);


	///setSpeed(IniManager("Perimeter.ini").getFloat("Game", "GameSpeed"));
}

void GameShell::startResourceDispatcher()
{
	frame_time.adjust();
	scale_time.adjust();

	HTManager::instance()->startSyncroTimer();
}

void GameShell::setSpeed(float d)
{
	MTAuto mtlock(HTManager::instance()->GetLockLogic());
	game_speed = clamp(d, 0, 10);
	HTManager::instance()->setSpeedSyncroTimer(d);
	scale_time.setSpeed(d);
	if (game_speed!=0 && soundPushedByPause) {
		soundPushedByPause = false;
		SNDPausePop();
		xassert(soundPushedPushLevel==SNDGetPushLevel());
		soundPushedPushLevel=INT_MIN;
	} else if (game_speed==0 && !soundPushedByPause) {
		soundPushedByPause = true;
		xassert(soundPushedPushLevel==INT_MIN);
		soundPushedPushLevel=SNDGetPushLevel();
		SNDPausePush();
	}
	_shellIconManager.speedChanged(game_speed);
}

void GameShell::setWindowClientSize(const Vect2i& size) {
    windowClientSize_ = size;
    setSourceUIResolution(size);
}

void GameShell::setCursorPosition(const Vect2f& pos)
{
    Vect2i ps = convertToScreenAbsolute(pos);
    SDL_WarpMouseInWindow(sdlWindow, (int)ps.x, (int)ps.y);
}

void GameShell::setCursorPosition(const Vect3f& posW)
{
	Vect3f v, e;
	terCamera->GetCamera()->ConvertorWorldToViewPort(&posW, &v, &e);

#if 0
	//TODO is necessary to convert position to absolute like Win32 does with ClientToScreen when windowed?
	if(!terFullScreen)
	{
		e.x *= float(windowClientSize().x)/terRenderDevice->GetSizeX();
		e.y *= float(windowClientSize().y)/terRenderDevice->GetSizeY();
		
		POINT pt = {e.x, e.y};
		::ClientToScreen(hWndVisGeneric, &pt);	
		
		e.x = pt.x; e.y = pt.y;
		
	}
	else
#endif
    {
		e.x *= float(windowClientSize().x)/terRenderDevice->GetSizeX();
		e.y *= float(windowClientSize().y)/terRenderDevice->GetSizeY();
	}

    SDL_WarpMouseInWindow(sdlWindow, (int)e.x, (int)e.y);
}

void GameShell::setSideArrowsVisible(bool visible) {
	_shellCursorManager.m_bShowSideArrows = visible ? 1 : 0;
}

void GameShell::rememberPlayerCamera(terPlayer* player, const char* triggerName)
{
	if(universe()->activePlayer()->isWorld())
		return;

	SavePrm data;
	if(!CurrentMission.loadMission(data))
		return;
	data.manualData.saveCamera(player->playerStrategyIndex(), triggerName);
	CurrentMission.saveMission(data, false);
}

void GameShell::setCountDownTime(int timeLeft) {
	countDownTimeMillisLeft = timeLeft;
}

const std::string& GameShell::getCountDownTime() {
	if (countDownTimeMillisLeft != countDownTimeMillisLeftVisible) {
		countDownTimeMillisLeftVisible = countDownTimeMillisLeft;
		countDownTimeLeft = formatTimeWithoutHour(countDownTimeMillisLeftVisible);
	}
	return countDownTimeLeft;
}

std::string GameShell::getTotalTime() const {
	return formatTimeWithHour(gameTimer());
}

//Autoswitch AI
void GameShell::checkAutoswitchAI() {
	autoSwitchAITimer += frame_time.delta();
	if (
			autoSwitchAITimer > 1000 * 60
		&&	universe()
		&&	universe()->activePlayer()
		&&	!universe()->activePlayer()->isAI() ) {
		
		autoSwitchAITimer = 0;
		universe()->activePlayer()->setAI(true);
	}
}

void GameShell::setActivePlayerAIOff() {
	autoSwitchAITimer = 0;
	if (
			universe()
		&&	universe()->activePlayer()
		&&	universe()->activePlayer()->isAI() ) {
		
		universe()->activePlayer()->setAI(false);
	}
}

void GameShell::changeControlState(const std::vector<SaveControlData>& newControlStates, bool reset_controls) {
	_shellIconManager.changeControlState(newControlStates, reset_controls);
}

void GameShell::fillControlState(std::vector<SaveControlData>& controlStatesToSave) {
	_shellIconManager.fillControlState(controlStatesToSave);
}

void GameShell::setScriptReelEnabled(bool isScriptReelEnabled) {
	scriptReelEnabled = isScriptReelEnabled;
/*
	if (scriptReelEnabled) {
		_shellCursorManager.HideCursor();
	} else {
		_shellCursorManager.ShowCursor();
	}
*/
	_shellCursorManager.m_bShowSideArrows = !isScriptReelEnabled;
	CShellWindow* wnd = _shellIconManager.GetWnd(SQSH_EMPTY_WND);
	if (wnd) {
		wnd->Show(scriptReelEnabled);
	}
}

void GameShell::updateMap() {
	if (terMapPoint) {
		terMapPoint->UpdateMap(Vect2i(0,0), Vect2i((int)vMap.H_SIZE-1,(int)vMap.V_SIZE-1) );
	}
}

extern bool isTrueFullscreen();
extern void PerimeterSetupDisplayMode();

void GameShell::updateResolution(bool change_depth, bool change_size, bool change_display_mode) {
    if (change_display_mode) {
        PerimeterSetupDisplayMode();
    }
    
	int mode=RENDERDEVICE_MODE_RETURNERROR;
	if(!isTrueFullscreen())
		mode|=RENDERDEVICE_MODE_WINDOW;
	if(terBitPerPixel==16)
		mode|=RENDERDEVICE_MODE_RGB16;
	else
		mode|=RENDERDEVICE_MODE_RGB32;

	if(!terRenderDevice->ChangeSize(
		terScreenSizeX,
		terScreenSizeY,
		mode
	))
	{
		ErrorInitialize3D();
	}


	if( terRenderDevice->IsFullScreen() && gameShell) {
		gameShell->setWindowClientSize(Vect2i(terScreenSizeX, terScreenSizeY));
	}

    if(change_size) {
        setSourceUIResolution(Vect2i(terScreenSizeX, terScreenSizeY));
		historyScene.onResolutionChanged();
		bwScene.onResolutionChanged();
		bgScene.onResolutionChanged();
		_shellIconManager.onSizeChanged();
		terVisGeneric->ReloadAllFont();
	}

	if(change_depth)
	{
		GetTexLibrary()->ReloadAllTexture();
	}
}

void GameShell::serverMessage(LocalizedText* text) {
    _shellIconManager.showHintChat(text, 5000);
}

void GameShell::showReelModal(const char* videoFileName, const char* soundFileName, bool localized, bool stopBGMusic, int alpha) {
	std::string path;
	if (localized) {
		path = getLocDataPath() + std::string("Video\\") + videoFileName;
	} else {
		path = videoFileName;
	}
    if (getExtension(path, false).empty()) {
        //Attempt to find extension, bik must be last as is the default one
        for (const auto& ext : { ".mkv", ".bik" }) {
            if (get_content_entry(path + ext)) {
                path += ext;
                break;
            }
        }
    }
	reelManager.showModal(path.c_str(), soundFileName, stopBGMusic, alpha);
	if (stopBGMusic) {
		PlayMusic();
	}
}

void GameShell::showPictureModal(const char* pictureFileName, bool localized, int stableTime) {
	std::string path;
	if (localized) {
		path = getLocDataPath() + std::string("Video\\") + pictureFileName;
	} else {
		path = pictureFileName;
	}
	reelManager.showPictureModal(path.c_str(), stableTime);
}

void GameShell::setCutSceneMode(bool on, bool animated) {
	_shellIconManager.setCutSceneModeSafe(on, animated);
}

bool GameShell::isCutSceneMode() {
	return _shellIconManager.isCutSceneMode();
}

void GameShell::prepareForInGameMenu() {
	if (cameraMouseTrack && IsMapArea(mousePosition())) {
		cameraMouseTrack = false;
		setCursorPosition(mousePressControl_);

		if(_shellIconManager.IsInterface())
			_shellCursorManager.ShowCursor();
	}
	if (cameraMouseShift) {
		cameraMouseShift = false;
		
		if(_shellIconManager.IsInterface())
			_shellCursorManager.ShowCursor();
	}
	CancelEditWorkarea();
	_shellCursorManager.m_bShowSideArrows=0;
	_shellCursorManager.ShowCursor();
	_bMenuMode = 1;

	if (currentSingleProfile.getLastGameType() != UserSingleProfile::MULTIPLAYER) {
		pauseGame(true);
	}
}

void GameShell::switchActivePlayer(bool next) {
	MTAuto lock(HTManager::instance()->GetLockLogic());
	activePlayerID_ += next ? 1 : -1;
	int max = (universe()->Players.size() - 1);
	if (activePlayerID_ >= max) {
		activePlayerID_ = 0;
	} else if (activePlayerID_ < 0) {
		activePlayerID_ = max - 1;
	}
	universe()->SetActivePlayer(activePlayerID_);
}

void GameShell::setLocalizedFontSizes() {
	std::string path = getLocDataPath() + "Fonts\\Font.ini";
	IniManager ini = IniManager(path.c_str());

	shell_main_menu_font_size1 = ini.getInt("Sizes","Menu1");
	shell_main_menu_font_size1_5 = ini.getInt("Sizes","Menu2");
	shell_main_menu_font_size2 = ini.getInt("Sizes","Menu3");
	shell_main_menu_font_size3 = ini.getInt("Sizes","Menu4");
	shell_main_menu_font_size4 = ini.getInt("Sizes","Menu5");

	HISTORY_SCENE_LOG_FONT_SIZE = ini.getInt("Sizes","ChainLog");

	defaultFontSize = ini.getInt("Sizes","Default");
	comboBoxFontSize = ini.getInt("Sizes","ComboBox");
	editBoxFontSize = ini.getInt("Sizes","EditBox");

	sqshCursorWorkAreaSize = ini.getInt("Sizes","WorkAreaCursor");

	sqshFontCountDownTimeSize = ini.getInt("Sizes","CountDownTimer");
	infoWndFontSize = ini.getInt("Sizes","InfoWnd");
	HINT_FONT_SIZE = ini.getInt("Sizes","CutSceneHint");

	inGameButtonFontSize = ini.getInt("Sizes","InGameButton");
	collectedEnergyBarFontSize = ini.getInt("Sizes","CollectedEnergyBar");

	statsTableFontSize = ini.getInt("Sizes","StatsTable");
	statsHeadTableFontSize = ini.getInt("Sizes","StatsHeadTable");
}

void GameShell::preLoad() {
    const std::string& locale = getLocale();
    if (get_content_entry("RESOURCE/scenario_" + locale + ".hst")) {
        historyScene.loadProgram("RESOURCE/scenario_" + locale + ".hst");
    } else {
        historyScene.loadProgram("RESOURCE/scenario.hst");
    }
    bwScene.loadProgram("RESOURCE/menu.hst");

    std::string path = getLocDataPath() + "Text";
    qdTextDB& texts = qdTextDB::instance();
    texts.clear();
    texts.load_from_directory(locale, path, true);

    //Iterate each mod Text folder for current locale and parse all files
    for (const auto& pair : getGameMods()) {
        if (!pair.second.enabled) continue;
        texts.load_from_directory(locale, pair.second.path + PATH_SEP + getLocDataPath() + "Text", false);
    }
    
    //Export text if desired
    const char* export_texts = check_command_line("export_texts");
    if (export_texts) {
        texts.exportTexts(export_texts);
        fprintf(stdout, "Texts exported %s\n", export_texts);
        ErrH.Exit();
    }
    
    //Load the builtin texts that might not be provided by mods
    texts.load_supplementary_texts(getLocale());
    texts.load_replacement_texts(getLocale());
    
    //Setup initial menu
    const char* initial_menu_str = check_command_line("initial_menu");
    if (initial_menu_str) {
        std::string initial_menu = std::string("SQSH_MM_") + initial_menu_str + "_SCR";
        int id = getEnumDescriptor(SQSH_MM_START_SCR).keyByName(initial_menu.c_str());
        if (id < SQSH_MM_START_SCR || id >= SQSH_MM_SCREENS_MAX) {
            fprintf(stderr, "Initial menu %s not found\n", initial_menu.c_str());
        } else {
            _shellIconManager.initialMenu = static_cast<ShellControlID>(id);
        }
    }
}

void GameShell::setSkipCutScene(bool skip) 
{ 
	cutSceneSkipped_ = skip; 
	if(skip) 
		lastSkipTime_.start(); 
}

void GameShell::recreateChaos() {
	if (chaos) {
		createChaos();
	}
}
void GameShell::createChaos() {
	if (chaos) {
		delete chaos;
		chaos=0;
	}

	chaos = new CChaos(vMap.H_SIZE,vMap.V_SIZE,terEnableBumpChaos);
}

void GameShell::editParameters()
{
	terRenderDevice->Flush(true);
    SDL_ShowCursor(SDL_TRUE);

	bool reloadParameters = false;
	savePrm().manualData.zeroLayerHeight = vMap.hZeroPlast;
    
    bool russian = startsWith(getLocale(), "russian");
	const char* header = russian ? "Заголовок миссии" : "Mission header";
	const char* mission = russian ? "Миссия" : "Mission";
	const char* missionAll = russian ? "Миссия все данные" : "Mission all data";
	const char* debugPrm = "Debug.prm";
	const char* global = russian ? "Глобальные параметры" : "Global parameters";
	const char* attribute = russian ? "Атрибуты" : "Attributes";
	const char* sounds = russian ? "Звуки" : "Sounds";
	const char* interface_ = russian ? "Интерфейс" : "Interface";
	const char* physics = russian ? "Физические параметры" : "Physics parameters";
    const char* separator = "--------------";

	std::vector<const char*> items;
	items.push_back(header);
	items.push_back(mission);
	items.push_back(separator);
	items.push_back(global);
	items.push_back(attribute);
	items.push_back(sounds);
	items.push_back(interface_);
	items.push_back(physics);
	items.push_back(separator);
	items.push_back(missionAll);
	items.push_back(debugPrm);

	const char* item = popupMenu(items);
	if(!item)
		return;
	else if(item == header){
		if(EditArchive().edit(CurrentMission)){
			SavePrm data;
			CurrentMission.loadMission(data);
			CurrentMission.saveMission(data, false);
		}
	}
	else if(item == mission){
		if(EditArchive().edit(savePrm().manualData, CurrentMission.savePathContent().c_str())){
			SavePrm data;
			CurrentMission.loadMission(data);
			data.manualData = manualData();
			CurrentMission.saveMission(data, false);
		}
	}
	else if(item == missionAll){
		if(EditArchive().edit(savePrm(), CurrentMission.savePathContent().c_str())){
			SavePrm data = savePrm();
			CurrentMission.saveMission(data, false);
		}
	}
	else if(item == debugPrm){
		debugPrm_.edit();
	}
	else if(item == attribute){
        EditArchive ea = EditArchive();
		attributeLibrary.edit(ea);
	}
	else if(item == global){
        EditArchive ea = EditArchive();
		globalAttr.edit(ea);
	}
	else if(item == sounds){
        EditArchive ea = EditArchive();
		soundScriptTable.edit(ea);
	}
	else if(item == interface_){
        EditArchive ea = EditArchive();
		interfaceAttr.edit(ea);
	}
	else if(item == physics){
        EditArchive ea = EditArchive();
		rigidBodyPrmLibrary.edit(ea);
	}

	if(manualData().zeroLayerHeight != vMap.hZeroPlast)
		IniManager(GetTargetName(vMap.worldIniFile).c_str()).putInt("Global Parameters", "hZeroPlast", manualData().zeroLayerHeight);

	if(reloadParameters){
		if(universe())
			universe()->RefreshAttribute();
		_shellIconManager.LoadControlsGroup(SHELL_LOAD_GROUP_GAME, true);
	}

	terCamera->setFocus(HardwareCameraFocus);
    SDL_ShowCursor(SDL_FALSE);
	RestoreFocus();
}
