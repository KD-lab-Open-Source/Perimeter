
#include "StdAfx.h"
#include "Runtime.h"
#include "InterfaceLogic.h"

#include "Config.h"
#include "Runtime.h"
#include "Terra.h"
#include "GenericControls.h"
#include "Universe.h"
#include "UniverseInterface.h"
#include "..\sound\PerimeterSound.h"
#include "Squad.h"

#include "GameShell.h"
#include "CameraManager.h"

#include "PerimeterShellUI.h"
#include "MissionEdit.h"
#include "HotKey.h"

#include "LogicUpdater.h"
#include "GenericFilth.h"
#include "IronPort.h"
#include "qd_textdb.h"

extern UnitInterfacePrm interface_squad1_prm;
extern UnitInterfacePrm interface_squad3_prm;
extern UnitInterfacePrm interface_frame_uninstalled;

extern int mt_interface_quant;

const float fCursorWorkareaDefaultHeight = 700;

extern MpegSound gb_Music;

extern bool  bNoUnitAction;
extern bool  bNoTracking;
extern int _nActiveClusterNum;

extern cFont* hFontMainmenu1;
extern cFont* hFontMainmenu2;
extern cFont* hFontMainmenu3;
extern cFont* hFontMainmenu4;

list<int> MissionDisabledButtonsList;

char _bCursorVisible = 1;
char _bMenuMode = 0;
const float nButtonHoldDelay = 1000;

void updateButtonLabel(int id, int val, float progress, bool pause, bool bCanBuild);
int Structure2ButtonID(int i);
int Structure2ButtonID_(int i);
STARFORCE_API_NEW  terUnitAttributeID Button2StructureID(int nBtnID);
int LegionID2Button(int nAttrID);
terUnitSquad* GetSquadByNumber(int n);

extern string getItemTextFromBase(const char *keyStr);

struct {
	int id;
	EVENTPROC handler;
}
_handlertbl[] = {
//	{, onMMButton},

	//start menu
	{SQSH_MM_SINGLE_BTN, onMMSingleButton},
	{SQSH_MM_OPTIONS_BTN, onMMOptionsButton},
	{SQSH_MM_LAN_BTN, onMMLanButton},
	{SQSH_MM_ONLINE_BTN, onMMOnlineButton},
	{SQSH_MM_CREDITS_BTN, onMMCreditsButton},
	{SQSH_MM_QUIT_BTN, onMMQuitButton},

	//single player menu
	{SQSH_MM_PROFILE_BTN, onMMProfileButton},
	{SQSH_MM_SCENARIO_BTN, onMMScenarioButton},
	{SQSH_MM_BACK_FROM_SINGLE_BTN, onMMBackButton},
	{SQSH_MM_BATTLE_BTN, onMMBattleButton},
	{SQSH_MM_LOAD_BTN, onMMLoadButton},
	{SQSH_MM_REPLAY_BTN, onMMLoadReplayButton},

	//profile editor
	{SQSH_MM_NEW_PROFILE_BTN, onMMNewProfileButton},
	{SQSH_MM_DEL_PROFILE_BTN, onMMDelProfileButton},
	{SQSH_MM_SELECT_PROFILE_BTN, onMMSelectProfileButton},
	{SQSH_MM_BACK_FROM_PROFILE_BTN, onMMBackFromProfileButton},
	{SQSH_MM_PROFILE_LIST, onMMProfileList},

	//scenario
	{SQSH_MM_MISSION_LIST, onMMMissionList},
	{SQSH_MM_DIFFICULTY_COMBO, onMMDifficultyCombo},
	{SQSH_MM_GO_BTN, onMMGoButton},
	{SQSH_MM_BACK_FROM_SCENARIO_BTN, onMMBackButton},

	//briefing
	{SQSH_MM_NOMAD_TXT, onMMNomadNameBriefing},
	{SQSH_MM_NOMAD_ICON, onMMNomadIconBriefing},
	{SQSH_MM_QUIT_FROM_BRIEFING_BTN, onMMQuitFromBriefingButton},
	{SQSH_MM_CONTINUE_BRIEFING_BTN, onMMContinueBriefingButton},
	{SQSH_MM_START_MISSION_BTN, onMMStartMissionButton},
	{SQSH_MM_SKIP_MISSION_BTN, onMMSkipMissionButton},
	{SQSH_MM_SKIP_BRIEFING_BTN, onMMSkipBriefingButton},
	{SQSH_MM_BRIEFING_YEAR_TXT, onMMYearBriefing},
	
	//end mission
	{SQSH_MM_RESUME_BTN, onMMResumeButton},
	{SQSH_MM_CONTINUE_BTN, onMMContinueButton},

	//stats
	{SQSH_MM_RESTART_BTN, onMMRestartButton},
	{SQSH_MM_SAVE_REPLAY_BTN, onMMSaveReplayButton},
	{SQSH_MM_QUIT_FROM_STATS_BTN, onMMQuitFromStatsButton},
	{SQSH_MM_CONTINUE_FROM_STATS_BTN, onMMContinueFromStatsButton},
	{SQSH_MM_STATS_TOTAL_BTN, onMMStatsTotalButton},
	{SQSH_MM_STATS_GENERAL_BTN, onMMStatsGeneralButton},
	{SQSH_MM_STATS_UNITS_BTN, onMMStatsUnitsButton},
	{SQSH_MM_STATS_BUILDINGS_BTN, onMMStatsBuildingsButton},

	//inmission
	{SQSH_MM_INMISSION_SAVE_BTN, onMMInMissSaveButton},
	{SQSH_MM_INMISSION_LOAD_BTN, onMMInMissLoadButton},
	{SQSH_MM_INMISSION_OPTIONS_BTN, onMMInMissOptionsButton},
	{SQSH_MM_INMISSION_RESTART_BTN, onMMInMissRestartButton},
	{SQSH_MM_INMISSION_RESUME_BTN, onMMInMissResumeButton},
	{SQSH_MM_INMISSION_QUIT_BTN, onMMInMissQuitButton},

	//battle
	{SQSH_MM_MAP_LIST, onMMMapList},

	//credits
	{SQSH_MM_BACK_CREDITS_BTN, onMMBackButton},


	{SQSH_MM_BATTLE_PLAYER1_FRM_BTN, onMMBattleFrmButton},
	{SQSH_MM_BATTLE_PLAYER2_FRM_BTN, onMMBattleFrmButton},
	{SQSH_MM_BATTLE_PLAYER3_FRM_BTN, onMMBattleFrmButton},
	{SQSH_MM_BATTLE_PLAYER4_FRM_BTN, onMMBattleFrmButton},
	{SQSH_MM_BATTLE_PLAYER1_CLR_BTN, onMMBattleClrButton},
	{SQSH_MM_BATTLE_PLAYER2_CLR_BTN, onMMBattleClrButton},
	{SQSH_MM_BATTLE_PLAYER3_CLR_BTN, onMMBattleClrButton},
	{SQSH_MM_BATTLE_PLAYER4_CLR_BTN, onMMBattleClrButton},
	{SQSH_MM_BATTLE_PLAYER1_SLOT_BTN, onMMBattleFirstSlotButton},
	{SQSH_MM_BATTLE_PLAYER2_SLOT_BTN, onMMBattleSlotButton},
	{SQSH_MM_BATTLE_PLAYER3_SLOT_BTN, onMMBattleSlotButton},
	{SQSH_MM_BATTLE_PLAYER4_SLOT_BTN, onMMBattleSlotButton},
	{SQSH_MM_BATTLE_PLAYER1_CLAN_BTN, onMMBattleClanButton},
	{SQSH_MM_BATTLE_PLAYER2_CLAN_BTN, onMMBattleClanButton},
	{SQSH_MM_BATTLE_PLAYER3_CLAN_BTN, onMMBattleClanButton},
	{SQSH_MM_BATTLE_PLAYER4_CLAN_BTN, onMMBattleClanButton},
	{SQSH_MM_BATTLE_PLAYER1_HC_BTN, onMMBattleHCButton},
	{SQSH_MM_BATTLE_PLAYER2_HC_BTN, onMMBattleHCButton},
	{SQSH_MM_BATTLE_PLAYER3_HC_BTN, onMMBattleHCButton},
	{SQSH_MM_BATTLE_PLAYER4_HC_BTN, onMMBattleHCButton},
	
	{SQSH_MM_BATTLE_GO_BTN, onMMBattleGoButton},
	{SQSH_MM_BACK_FROM_BATTLE_BTN, onMMBackButton},


	//load game
	{SQSH_MM_LOAD_MAP_LIST, onMMLoadMapList},
	{SQSH_MM_LOAD_GO_BTN, onMMLoadGoButton},
	{SQSH_MM_BACK_FROM_LOAD_BTN, onMMBackButton},
	{SQSH_MM_BACK_FROM_LOAD_BTN, onMMBackButton},
	{SQSH_MM_DEL_SAVE_BTN, onMMDelSaveButton},

	//load replay
	{SQSH_MM_LOAD_REPLAY_LIST, onMMReplayList},
	{SQSH_MM_LOAD_REPLAY_GO_BTN, onMMReplayGoButton},
	{SQSH_MM_BACK_FROM_LOAD_REPLAY_BTN, onMMBackButton},
	{SQSH_MM_DEL_REPLAY_BTN, onMMDelReplayButton},

	//load in game
	{SQSH_MM_LOAD_IN_GAME_MAP_LIST, onMMLoadInGameMapList},
	{SQSH_MM_LOAD_IN_GAME_GO_BTN, onMMLoadInGameGoButton},
	{SQSH_MM_BACK_FROM_LOAD_IN_GAME_BTN, onMMBackButton},
	{SQSH_MM_LOAD_IN_GAME_DEL_BTN, onMMDelLoadInGameButton},

	//save game
	{SQSH_MM_SAVE_GAME_MAP_LIST, onMMSaveGameMapList},
	{SQSH_MM_SAVE_GAME_GO_BTN, onMMSaveGameGoButton},
	{SQSH_MM_BACK_FROM_SAVE_GAME_BTN, onMMBackButton},
	{SQSH_MM_SAVE_GAME_DEL_BTN, onMMDelSaveGameButton},

	//save replay
	{SQSH_MM_SAVE_REPLAY_LIST, onMMSaveReplayList},
	{SQSH_MM_SAVE_REPLAY_GO_BTN, onMMSaveReplayGoButton},
	{SQSH_MM_BACK_FROM_SAVE_REPLAY_BTN, onMMBackButton},
	{SQSH_MM_SAVE_REPLAY_DEL_BTN, onMMDelSaveReplayButton},

	//task
	{SQSH_MM_BACK_FROM_TASK_BTN, onMMBackButton},

	{SQSH_TASK_BUTTON_ID, onMMTaskButton},

	//name input
	{SQSH_MM_APPLY_NAME_BTN, onMMApplyNameBtn},
	{SQSH_MM_BACK_FROM_NAME_INPUT_BTN, onMMBackButton},
	{SQSH_MM_CONNECTION_TYPE_COMBO, onMMConnectionTypeCombo},

	//lan
	{SQSH_MM_GAME_LIST, onMMGameList},
	{SQSH_MM_LAN_CREATE_GAME_BTN, onMMLanCreateGameButton},
	{SQSH_MM_JOIN_BTN, onMMJoinButton},
	{SQSH_MM_BACK_FROM_LAN_BTN, onMMLanBackButton},

	//create game
	{SQSH_MM_LAN_MAP_LIST, onMMLanMapList},
	{SQSH_MM_CREATE_BTN, onMMCreateButton},
//	{SQSH_MM_LAN_GAME_SPEED_SLIDER, onMMLanGameSpeedSlider},
	{SQSH_MM_BACK_FROM_CREATE_GAME_BTN, onMMCreateLanBackButton},

	//lobby
	{SQSH_MM_LOBBY_GAME_NAME_BTN, onMMLobby},
	{SQSH_MM_LOBBY_PLAYER1_NAME_BTN, onMMLobbyNameButton},
	{SQSH_MM_LOBBY_PLAYER2_NAME_BTN, onMMLobbyNameButton},
	{SQSH_MM_LOBBY_PLAYER3_NAME_BTN, onMMLobbyNameButton},
	{SQSH_MM_LOBBY_PLAYER4_NAME_BTN, onMMLobbyNameButton},
	{SQSH_MM_LOBBY_PLAYER1_FRM_BTN, onMMLobbyFrmButton},
	{SQSH_MM_LOBBY_PLAYER2_FRM_BTN, onMMLobbyFrmButton},
	{SQSH_MM_LOBBY_PLAYER3_FRM_BTN, onMMLobbyFrmButton},
	{SQSH_MM_LOBBY_PLAYER4_FRM_BTN, onMMLobbyFrmButton},
	{SQSH_MM_LOBBY_PLAYER1_CLR_BTN, onMMLobbyClrButton},
	{SQSH_MM_LOBBY_PLAYER2_CLR_BTN, onMMLobbyClrButton},
	{SQSH_MM_LOBBY_PLAYER3_CLR_BTN, onMMLobbyClrButton},
	{SQSH_MM_LOBBY_PLAYER4_CLR_BTN, onMMLobbyClrButton},
	{SQSH_MM_LOBBY_PLAYER1_SLOT_BTN, onMMLobbySlotButton},
	{SQSH_MM_LOBBY_PLAYER2_SLOT_BTN, onMMLobbySlotButton},
	{SQSH_MM_LOBBY_PLAYER3_SLOT_BTN, onMMLobbySlotButton},
	{SQSH_MM_LOBBY_PLAYER4_SLOT_BTN, onMMLobbySlotButton},
	{SQSH_MM_LOBBY_PLAYER1_CLAN_BTN, onMMLobbyClanButton},
	{SQSH_MM_LOBBY_PLAYER2_CLAN_BTN, onMMLobbyClanButton},
	{SQSH_MM_LOBBY_PLAYER3_CLAN_BTN, onMMLobbyClanButton},
	{SQSH_MM_LOBBY_PLAYER4_CLAN_BTN, onMMLobbyClanButton},
	{SQSH_MM_LOBBY_PLAYER1_HC_BTN, onMMLobbyHCButton},
	{SQSH_MM_LOBBY_PLAYER2_HC_BTN, onMMLobbyHCButton},
	{SQSH_MM_LOBBY_PLAYER3_HC_BTN, onMMLobbyHCButton},
	{SQSH_MM_LOBBY_PLAYER4_HC_BTN, onMMLobbyHCButton},
	{SQSH_MM_LOBBY_START_BTN, onMMLobbyStartButton},
	{SQSH_MM_BACK_FROM_LOBBY_BTN, onMMLobbyBackButton},
	{SQSH_MM_LOBBY_CHAT_INPUT, onMMLobbyChatInputButton},
	{SQSH_MM_LOBBY_MAP_LIST, onMMLobbyMapList},

	//online
	{SQSH_MM_ONLINE_GAME_LIST, onMMOnlineGameList},
	{SQSH_MM_ONLINE_CREATE_GAME_BTN, onMMOnlineCreateGameButton},
	{SQSH_MM_ONLINE_JOIN_BTN, onMMOnlineJoinButton},
	{SQSH_MM_BACK_FROM_ONLINE_BTN, onMMOnlineBackButton},

	//create online game
	{SQSH_MM_ONLINE_MAP_LIST, onMMOnlineMapList},
	{SQSH_MM_ONLINE_CREATE_BTN, onMMOnlineCreateButton},
	{SQSH_MM_ONLINE_GAME_SPEED_SLIDER, onMMOnlineGameSpeedSlider},
	{SQSH_MM_BACK_FROM_CREATE_ONLINE_GAME_BTN, onMMBackButton},

	//online lobby
	{SQSH_MM_ONLINE_LOBBY_GAME_NAME_BTN, onMMOnlineLobby},
	{SQSH_MM_ONLINE_LOBBY_PLAYER1_NAME_BTN, onMMOnlineLobbyNameButton},
	{SQSH_MM_ONLINE_LOBBY_PLAYER2_NAME_BTN, onMMOnlineLobbyNameButton},
	{SQSH_MM_ONLINE_LOBBY_PLAYER3_NAME_BTN, onMMOnlineLobbyNameButton},
	{SQSH_MM_ONLINE_LOBBY_PLAYER4_NAME_BTN, onMMOnlineLobbyNameButton},
	{SQSH_MM_ONLINE_LOBBY_PLAYER1_FRM_BTN, onMMOnlineLobbyFrmButton},
	{SQSH_MM_ONLINE_LOBBY_PLAYER2_FRM_BTN, onMMOnlineLobbyFrmButton},
	{SQSH_MM_ONLINE_LOBBY_PLAYER3_FRM_BTN, onMMOnlineLobbyFrmButton},
	{SQSH_MM_ONLINE_LOBBY_PLAYER4_FRM_BTN, onMMOnlineLobbyFrmButton},
	{SQSH_MM_ONLINE_LOBBY_PLAYER1_CLR_BTN, onMMOnlineLobbyClrButton},
	{SQSH_MM_ONLINE_LOBBY_PLAYER2_CLR_BTN, onMMOnlineLobbyClrButton},
	{SQSH_MM_ONLINE_LOBBY_PLAYER3_CLR_BTN, onMMOnlineLobbyClrButton},
	{SQSH_MM_ONLINE_LOBBY_PLAYER4_CLR_BTN, onMMOnlineLobbyClrButton},
	{SQSH_MM_ONLINE_LOBBY_PLAYER1_SLOT_BTN, onMMOnlineLobbySlotButton},
	{SQSH_MM_ONLINE_LOBBY_PLAYER2_SLOT_BTN, onMMOnlineLobbySlotButton},
	{SQSH_MM_ONLINE_LOBBY_PLAYER3_SLOT_BTN, onMMOnlineLobbySlotButton},
	{SQSH_MM_ONLINE_LOBBY_PLAYER4_SLOT_BTN, onMMOnlineLobbySlotButton},
	{SQSH_MM_ONLINE_LOBBY_START_BTN, onMMOnlineLobbyStartButton},
	{SQSH_MM_BACK_FROM_ONLINE_LOBBY_BTN, onMMOnlineLobbyBackButton},

	//options
	{SQSH_MM_OPTIONS_GAME_BTN, onMMGameButton},
	{SQSH_MM_OPTIONS_GRAPHICS_BTN, onMMGraphicsButton},
	{SQSH_MM_OPTIONS_SOUND_BTN, onMMSoundButton},
	{SQSH_MM_BACK_FROM_OPTIONS_BTN, onMMBackButton},

	//game
	{SQSH_MM_BACK_FROM_GAME_BTN, onMMBackButton},

	//graphics
	{SQSH_MM_SETTINGS_COMBO, OnComboGraphicsSettings},
	{SQSH_MM_CUSTOM_GRAPHICS_BTN, OnGraphicsCustomBtn},
	{SQSH_MM_APPLY_BTN, OnGraphicsApplyBtn},
	{SQSH_MM_BACK_FROM_GRAPHICS_BTN, onMMBackButton},

	//custom
	{SQSH_MM_GRAPHICS_LOD_COMBO, onLODCombo},
	{SQSH_MM_PARTICLE_RATE_SLIDER, onParticleSlider},
	{SQSH_MM_GRAPHICS_OCCLUSION_COMBO, onOcclusionCombo},
	{SQSH_MM_GRAPHICS_POINT_LIGHT_COMBO, onPointLightCombo},
	{SQSH_MM_GRAPHICS_SHADOWS_SAMPLES_COMBO, onSamplesCombo},
	{SQSH_MM_GRAPHICS_BUMP_COMBO, onBumpCombo},
	{SQSH_MM_GRAPHICS_BUMP_CHAOS_COMBO, onBumpChaosCombo},
	{SQSH_MM_GRAPHICS_COMPRESS_COMBO, onCompressCombo},
	{SQSH_MM_BACK_FROM_CUSTOM_BTN, onMMBackButton},

	//sound
	{SQSH_MM_BACK_FROM_SOUND_BTN, onMMBackButton},

	//submit dialog
	{SQSH_MM_SUBMIT_YES_BTN, onMMSubmitYesButton},
	{SQSH_MM_SUBMIT_NO_BTN, onMMSubmitNoButton},
	{SQSH_MM_SUBMIT_OK_BTN, onMMSubmitYesButton},

	//ingame graphics
	{SQSH_MM_CUSTOM_INGAME_GRAPHICS_BTN, OnIngameGraphicsCustomBtn},
	{SQSH_MM_INGAME_APPLY_BTN, OnGraphicsApplyBtn},
	{SQSH_MM_BACK_FROM_INGAME_GRAPHICS_BTN, onMMBackButton},

	{SQSH_MM_OPTIONS, OnButtonMMOptions},
	{SQSH_MM_BACK, onMMBackButton},
	{SQSH_MM_OPTIONS_GAME,OnButtonOptionGame},
	{SQSH_MM_OPTIONS_GRAPHICS,OnButtonOptionGraphics},
	{SQSH_MM_OPTIONS_SOUND,OnButtonOptionSound},
	{SQSH_MM_GRAPHICS_GAMMA_SLIDER, OnSliderGraphicsGamma},
	{SQSH_MM_SOUND_MUSIC_COMBO,OnComboSoundMusic},
	{SQSH_MM_SOUND_SOUNDEFFECTS_COMBO,OnComboSoundEffects},
	{SQSH_MM_SOUND_SOUNDEFFECTS_COMBO,OnComboSoundEffects},
	{SQSH_MM_SOUND_SOUNDVOLUME_SLIDER,OnSliderSoundVolume},
	{SQSH_MM_SOUND_MUSICVOLUME_SLIDER,OnSliderMusicVolume},

	{SQSH_MM_GRAPHICS_FURROWS_COMBO,OnComboGraphicsFurrows},
	{SQSH_MM_GAME_TOOLTIPS_COMBO,OnComboGameTooltips},
	{SQSH_MM_GRAPHICS_COLORDEPTH_COMBO,OnComboGraphicsColorDepth},
	{SQSH_MM_GRAPHICS_MODE_COMBO,OnComboGraphicsMode},
	{SQSH_MM_GRAPHICS_SHADOWS_COMBO,OnComboGraphicsShadows},
	{SQSH_MM_GRAPHICS_REFLECTION_COMBO,OnComboGraphicsReflection},
	{SQSH_MM_GRAPHICS_RESOLUTION_COMBO,OnComboGraphicsResolution},
	{SQSH_MM_GAME_SCROLLRATE_SLIDER,OnSliderScrollRate},
	{SQSH_MM_GAME_ANGLESENS_SLIDER,OnSliderAngleSens},

	{SQSH_MM_BACK_FROM_INGAME_OPTIONS, onMMBackButton},
	{SQSH_MM_BACK_FROM_INGAME_GAME_OPTIONS, onMMBackButton},
	{SQSH_MM_BACK_FROM_INGAME_CUSTOM, onMMBackButton},
	{SQSH_MM_BACK_FROM_INGAME_SOUND, onMMBackButton},

	{SQSH_MM_SPLASH1,OnSplashScreenKey1},
	{SQSH_MM_SPLASH2,OnSplashScreenKey2},
	{SQSH_MM_SPLASH3,OnSplashScreenKey3},
	{SQSH_MM_SPLASH4,OnSplashScreenKey4},
	{SQSH_MM_SPLASH_LAST,OnSplashScreenKeyLast},

	{SQSH_INGAME_CHAT_EDIT_ID, onMMInGameChatInputButton},

			
	//внутренний интерфейс
	{SQSH_WORKAREA2_ID, OnButtonWorkArea},
	{SQSH_WORKAREA3_ID, OnButtonWorkArea},
	{SQSH_WORKAREA4_ID, OnButtonWorkArea},
	{SQSH_SOLDIER_ID, OnButtonLegion},
	{SQSH_OFFICER_ID, OnButtonLegion},
	{SQSH_TECHNIC_ID, OnButtonLegion},
	{SQSH_TOGETHER_ID, OnButtonTogether},
	{SQSH_FIELD_ON_ID, OnButtonFieldOn},
	{SQSH_FIELD_OFF_ID, OnButtonFieldOff},
	{SQSH_YADRO_ID, OnButtonStructure},
	{SQSH_YADRO_EX_ID, OnButtonStructure},
	{SQSH_SOLDIER_PLANT_ID, OnButtonStructure},
	{SQSH_OFFICER_PLANT_ID, OnButtonStructure},
	{SQSH_TECHNIC_PLANT_ID, OnButtonStructure},
	{SQSH_COMMANDER_ID, OnButtonStructure},
	{SQSH_RELAY_ID, OnButtonStructure},
	{SQSH_STATION1_ID, OnButtonStructure},
	{SQSH_STATION2_ID, OnButtonStructure},
	{SQSH_STATION3_ID, OnButtonStructure},
	{SQSH_STATION4_ID, OnButtonStructure},
	{SQSH_STATION5_ID, OnButtonStructure},
	{SQSH_STATION_ELECTRO_LAB_ID, OnButtonStructure},
	{SQSH_STATION_EXODUS_LAB_ID, OnButtonStructure},
	{SQSH_STATION_EMPIRE_LAB_ID, OnButtonStructure},
	{SQSH_STATION_HARKBACK_LAB_ID, OnButtonStructure},
	{SQSH_GUN_LASER_ID, OnButtonStructure},
	{SQSH_GUN_ELECTRO_ID, OnButtonStructure},
	{SQSH_GUN_ROCKET_ID, OnButtonStructure},
	{SQSH_GUN_HOWITZER_ID, OnButtonStructure},
	{SQSH_GUN_GIMLET_ID, OnButtonStructure},
	{SQSH_GUN_FILTH_ID, OnButtonStructure},
	{SQSH_GUN_BALLISTIC_ID, OnButtonStructure},
	{SQSH_GUN_SUBCHASER_ID, OnButtonStructure},
	{SQSH_CORRIDOR_ALPHA_ID, OnButtonStructure},
	{SQSH_CORRIDOR_OMEGA_ID, OnButtonStructure},
	{SQSH_STATIC_BOMB_ID, OnButtonStructure},

	{SQSH_SQUAD_DISINTEGRATE_ID, OnButtonDisintegrate},
	{SQSH_SQUAD_UNIT1, OnButtonSquadMutate},
	{SQSH_SQUAD_UNIT2, OnButtonSquadMutate},
	{SQSH_SQUAD_UNIT3, OnButtonSquadMutate},
	{SQSH_SQUAD_UNIT4, OnButtonSquadMutate},
	{SQSH_SQUAD_UNIT5, OnButtonSquadMutate},
	{SQSH_SQUAD_UNIT6, OnButtonSquadMutate},
	{SQSH_SQUAD_UNIT7, OnButtonSquadMutate},
	{SQSH_SQUAD_UNIT8, OnButtonSquadMutate},
	{SQSH_SQUAD_UNIT9, OnButtonSquadMutate},
	{SQSH_SQUAD_UNIT10, OnButtonSquadMutate},
	{SQSH_SQUAD_UNIT11, OnButtonSquadMutate},
	{SQSH_SQUAD_UNIT12, OnButtonSquadMutate},
	{SQSH_SQUAD_UNIT13, OnButtonSquadMutate},
	{SQSH_SQUAD_UNIT14, OnButtonSquadMutate},
	{SQSH_SQUAD_UNIT15, OnButtonSquadMutate},
	{SQSH_SQUAD_UNIT16, OnButtonSquadMutate},
	{SQSH_SQUAD_UNIT17, OnButtonSquadMutate},
	{SQSH_SQUAD_UNIT18, OnButtonSquadMutate},
	{SQSH_SQUAD_UNIT19, OnButtonSquadMutate},
	{SQSH_SQUAD_UNIT20, OnButtonSquadMutate},
	{SQSH_SQUAD_UNIT21, OnButtonSquadMutate},
	{SQSH_SQUAD_UNIT22, OnButtonSquadMutate},
	{SQSH_SQUAD_UNIT23, OnButtonSquadMutate},
	{SQSH_SQUAD_UNIT24, OnButtonSquadMutate},
	{SQSH_SQUAD_UNIT25, OnButtonSquadMutate},
	{SQSH_MAP_WINDOW_ID, OnMapWindowClicked},

	{SQSH_FRAME_TERRAIN_BUILD1_ID, OnButtonTerrainBuild},
	{SQSH_FRAME_TERRAIN_BUILD2_ID, OnButtonTerrainBuild},
	{SQSH_FRAME_TERRAIN_BUILD3_ID, OnButtonTerrainBuild},
	{SQSH_FRAME_TERRAIN_BUILD4_ID, OnButtonTerrainBuild},
	{SQSH_FRAME_TERRAIN_BUILD5_ID, OnButtonTerrainBuild},

	{SQSH_SPEED_PAUSE, OnButtonSpeed},
	{SQSH_SPEED_50, OnButtonSpeed},
	{SQSH_SPEED_100, OnButtonSpeed},
	{SQSH_SPEED_150, OnButtonSpeed},

	{SQSH_REPLAY_PLAYER_BUTTON_ID, OnButtonReplayPlayer},

	{SQSH_TAB_SQUAD_ID, OnSquadTabEvent},
	{SQSH_TAB_BUILD_ID, OnFrameTabEvent},
	{SQSH_SELPANEL_SQ_BACK_ID, OnButtonGotoBase},
	{SQSH_SELPANEL_MOVE_ID, OnButtonTarget},
	{SQSH_SELPANEL_SQ_ATTACK_ID, OnButtonAttack},
	{SQSH_SELPANEL_SQ_OFDEF_ID, OnButtonOffensiveDefensive},
	{SQSH_MENU_BUTTON_ID, OnButtonMenu},
	{SQSH_SELPANEL_SELL_ID, OnButtonSell},
	{SQSH_SELPANEL_FRAME_INSTALL_ID, OnButtonFrameInstall},
	{SQSH_SELPANEL_FRAME_TELEPORTATE_ID, OnButtonFrameTeleportate},
	{SQSH_SELPANEL_UPGRADE_ID, OnButtonUpgrade},
	{SQSH_SELPANEL_BRIG_BACK_ID, OnButtonBackToFrame},
	{SQSH_SELPANEL_FRAME_ALARM_ID, OnButtonAlarm},
	{SQSH_SELPANEL_FIELDON_ID, OnButtonFieldSingleOn},
	{SQSH_SELPANEL_FIELDOFF_ID, OnButtonFieldSingleOff},
	{SQSH_SELPANEL_POWERON_ID, OnButtonStructuteOn},
	{SQSH_SELPANEL_POWEROFF_ID, OnButtonStructuteOff},
	{SQSH_SELPANEL_SQ_PATROL_ID, OnButtonPatrol},
	{SQSH_SELPANEL_STOP_ID, OnButtonStop},
	{SQSH_SELPANEL_STOP2_ID, OnButtonStop},
	{SQSH_SELPANEL_BRIG_CHANGE_ID, OnButtonBrigadierChange},
	{SQSH_SELPANEL_BRIG_BUILD_ID, OnButtonBrigadierBuild},

	{SQSH_SELPANEL_START_CHARGE_ID, OnButtonStartCharge},
	{SQSH_SELPANEL_STOP_CHARGE_ID, OnButtonStopCharge},

	{SQSH_SELPANEL_UPGRADE_LASER1_ID, OnButtonUpgrade},
	{SQSH_SELPANEL_UPGRADE_LASER2_ID, OnButtonUpgrade},
	{SQSH_SELPANEL_UPGRADE_ELECTRO1_ID, OnButtonUpgrade},
	{SQSH_SELPANEL_UPGRADE_ELECTRO2_ID, OnButtonUpgrade},
	{SQSH_SELPANEL_UPGRADE_BOMB1_ID, OnButtonUpgrade},
	{SQSH_SELPANEL_UPGRADE_BOMB2_ID, OnButtonUpgrade},
	{SQSH_SELPANEL_UPGRADE_ROCKET1_ID, OnButtonUpgrade},
	{SQSH_SELPANEL_UPGRADE_ROCKET2_ID, OnButtonUpgrade},
	{SQSH_SELPANEL_UPGRADE_EXODUS1_ID, OnButtonUpgrade},
	{SQSH_SELPANEL_UPGRADE_EXODUS2_ID, OnButtonUpgrade},
	{SQSH_SELPANEL_UPGRADE_EMPIRE1_ID, OnButtonUpgrade},
	{SQSH_SELPANEL_UPGRADE_EMPIRE2_ID, OnButtonUpgrade},
	{SQSH_SELPANEL_UPGRADE_HARKBACK1_ID, OnButtonUpgrade},
	{SQSH_SELPANEL_UPGRADE_HARKBACK2_ID, OnButtonUpgrade},
	{SQSH_SELPANEL_UPGRADE_FLY_ID, OnButtonUpgrade},
	{SQSH_SELPANEL_UPGRADE_OMEGA_ID, OnButtonUpgrade},
	{SQSH_SELPANEL_UPGRADE_SUBTERRA_ID, OnButtonUpgrade},
	
};

EVENTPROC _GetEventHandler(int ID)
{
	int count = sizeof(_handlertbl)/sizeof(_handlertbl[0]);
	for(int i=0; i<count; i++)
		if(_handlertbl[i].id == ID)
			return _handlertbl[i].handler;
	return 0;
}


///////////////////////////////////////////////////////////////////////////

CShellCursorManager::CShellCursorManager()
{
	m_pActiveCursor = 0;
	m_bCursorPermanent = 1;
	m_nCursorShift = 0;
	m_ftime = 0;

	m_bShowSideArrows = 1;
}
CShellCursorManager::~CShellCursorManager()
{
//	Done();
}
void CShellCursorManager::Done()
{
	deque<CURSOR>::iterator i;
	FOR_EACH(m_cursors, i)
		if(!i->bCursorSystem)
			_RELEASE(i->hCursorProgram);

	_RELEASE(m_hFontCursorWorkarea);
	_RELEASE(hFontMainmenu1);
	_RELEASE(hFontMainmenu2);
	_RELEASE(hFontMainmenu3);
	_RELEASE(hFontMainmenu4);
}

void CShellCursorManager::Load()
{
	CURSOR _c;
	for(int i=0; i<_sqsh_cursor_count; i++)
	{
		sqshCursor& cc = _sqsh_cursors[i];

		_c.bCursorSystem = cc.system;
		_c.sx = cc.sx; 
		_c.sy = cc.sy;
		_c.bHotspotCentred = cc.hotspot_center;
		if(cc.system)
			_c.hCursorSystem = (HCURSOR)LoadImage(0, cc.image, IMAGE_CURSOR, 0, 0, LR_LOADFROMFILE);
		else
			_c.hCursorProgram = terVisGeneric->CreateTexture(cc.image);

		m_cursors.push_back(_c);
	}

	m_hCursorDefault = (HCURSOR)LoadImage(0, "resource\\cursors\\arrow.cur", IMAGE_CURSOR, 0, 0, LR_LOADFROMFILE);

	SetActiveCursor(arrow);	

	m_hFontCursorWorkarea = terVisGeneric->CreateFont(sqshShellMainFont, sqshCursorWorkAreaSize);
}
void CShellCursorManager::SetActiveCursor(int cursor, char bPermanent)
{
	if(m_pActiveCursor != &m_cursors[cursor])
	{
		bool bWasCentred = m_pActiveCursor ? m_pActiveCursor->bHotspotCentred : false;

		m_pActiveCursor = &m_cursors[cursor];

		if(m_pActiveCursor->bHotspotCentred && !bWasCentred)
		{
			cur_x -= m_pActiveCursor->sx/terScreenSizeX*fWorkAreaCenterX;
			cur_y -= m_pActiveCursor->sy/terScreenSizeY*fWorkAreaCenterY;
		}
	}

	m_bCursorPermanent = bPermanent;
	OnWMSetCursor();
}

void CShellCursorManager::OnWMSetCursor()
{
	if(!m_pActiveCursor)
		return;

	int bSysCursor = m_pActiveCursor->bCursorSystem;

	if(_bCursorVisible)
	{
		if(m_nCursorShift)
		{
			//сдвиг карты
			if(m_cursors[m_nCursorShift].bCursorSystem)
				SetCursor(m_cursors[m_nCursorShift].hCursorSystem);
			else
				::SetCursor(0);

			return;
		}

		if(_shellIconManager.m_pCtrlHover)
		{
			//курсор на элементе интерфейса
			SetCursor(m_hCursorDefault);
			return;
		}

		if(bSysCursor)
			::SetCursor(m_pActiveCursor->hCursorSystem);
		else
			::SetCursor(0);
	}
	else
		::SetCursor(0);
}
void CShellCursorManager::OnMouseMove(float x, float y)
{
	if(_pShellDispatcher->m_bToolzerSizeChangeMode)
		return;

	cur_x = x; cur_y = y;

	if(m_pActiveCursor && !m_pActiveCursor->bCursorSystem)
	{
		//float _x = x, _y = y;
		if(m_pActiveCursor->bHotspotCentred)
		{
			float fScale = fCursorWorkareaDefaultHeight/terCamera->coordinate().distance();
			cur_x -= fScale*m_pActiveCursor->sx/terScreenSizeX*fWorkAreaCenterX;
			cur_y -= fScale*m_pActiveCursor->sy/terScreenSizeY*fWorkAreaCenterY;
		}
	}

	//проверка на курсор сдвига карты
	m_nCursorShift = 0;

	if(m_bShowSideArrows/* && !gameShell->CursorOverInterface*/)
	{
		if(x < CAMERA_BORDER_SCROLL_AREA_HORZ)
			m_nCursorShift = arrow_left;
		else if(1 - x < CAMERA_BORDER_SCROLL_AREA_HORZ)
			m_nCursorShift = arrow_right;

		if(y < CAMERA_BORDER_SCROLL_AREA_UP)
		{
			if(m_nCursorShift == arrow_left)
				m_nCursorShift = arrow_up_left;
			else if(m_nCursorShift == arrow_right)
				m_nCursorShift = arrow_up_right;
			else
				m_nCursorShift = arrow_up;
		}
		else if(1 - y < CAMERA_BORDER_SCROLL_AREA_DN)
		{
			if(m_nCursorShift == arrow_left)
				m_nCursorShift = arrow_down_left;
			else if(m_nCursorShift == arrow_right)
				m_nCursorShift = arrow_down_right;
			else
				m_nCursorShift = arrow_down;
		}
	}
}

extern int _tmpLastCameraTilt;

void CShellCursorManager::draw()
{
	m_ftime += frame_time.delta();

	if(_shellIconManager.IsInterface())
	{
		if(gameShell->cameraMouseShift)
		{
			//курсор сдвига карты
			Vect3f v1,e1;
			terCamera->GetCamera()->ConvertorWorldToViewPort(&gameShell->mapMoveStartPoint(), &v1, &e1);

			CURSOR* pCursor = &m_cursors[map_move];

			e1.x -= 16;
			e1.y -= 16;

			terRenderDevice->DrawSprite(e1.x, e1.y, pCursor->sx, pCursor->sy,
				0, 0, 1, 1, pCursor->hCursorProgram, sColor4c(255, 255, 255, 255));

			return;
		}
		if(gameShell->cameraMouseTrack)
		{
			CURSOR* pCursor;
			if(terCamera->tilting())
				 pCursor = &m_cursors[tilt];
			else
				 pCursor = &m_cursors[rotate];

			int x = (gameShell->mousePressControl().x + 0.5f)*terScreenSizeX - 16;
			int y = (gameShell->mousePressControl().y + 0.5f)*terScreenSizeY - 16;

			terRenderDevice->DrawSprite(x, y, pCursor->sx, pCursor->sy,
				0, 0, 1, 1, pCursor->hCursorProgram, sColor4c(255, 255, 255, 255));

			return;
		}
	}

	if(m_nCursorShift && !m_cursors[m_nCursorShift].bCursorSystem)
	{
		int dx = 0, dy = 0;
		switch(m_nCursorShift)
		{
		case arrow_up:
			break;
		case arrow_down:
			dy = -m_cursors[m_nCursorShift].sy;
			break;
		case arrow_left:
			break;
		case arrow_right:
			dx = -m_cursors[m_nCursorShift].sx;
			break;
		case arrow_up_left:
			break;
		case arrow_up_right:
			dx = -m_cursors[m_nCursorShift].sx;
			break;
		case arrow_down_left:
			dy = -m_cursors[m_nCursorShift].sy;
			break;
		case arrow_down_right:
			dy = -m_cursors[m_nCursorShift].sy;
			dx = -m_cursors[m_nCursorShift].sx;
			break;
		}

		//сдвиг карты
		terRenderDevice->DrawSprite(round(terScreenSizeX*cur_x) + dx,round(terScreenSizeY*cur_y) + dy,
			m_cursors[m_nCursorShift].sx, m_cursors[m_nCursorShift].sy,
			0, 0, 1, 1, m_cursors[m_nCursorShift].hCursorProgram, sColor4c(255, 255, 255, 255), fmod(m_ftime, 1000)/1000.f);
		return;
	}

	if(_shellIconManager.m_pCtrlHover || m_nCursorShift)
		return;

	if(_bCursorVisible && !m_nCursorShift && m_pActiveCursor && 
		!m_pActiveCursor->bCursorSystem && !_shellIconManager.m_pCtrlHover)
	{
		float fScale = 1.0f;
		if((m_pActiveCursor == &m_cursors[workarea_in]) || (m_pActiveCursor == &m_cursors[workarea_out]))
			fScale = fCursorWorkareaDefaultHeight/terCamera->coordinate().distance();

		terRenderDevice->DrawSprite(round(terScreenSizeX*cur_x),round(terScreenSizeY*cur_y),
			fScale*m_pActiveCursor->sx, fScale*m_pActiveCursor->sy,
			0, 0, 1, 1, m_pActiveCursor->hCursorProgram, sColor4c(255, 255, 255, 255), fmod(m_ftime, 1000)/1000.f);
	}

	//высота до зерослоя
	if(!_pShellDispatcher->m_bToolzerSizeChangeMode)
		if((m_pActiveCursor == &m_cursors[workarea_in]) || (m_pActiveCursor == &m_cursors[workarea_out]))
		{
			Vect3f v;
			if(terCamera->cursorTrace(Vect2f(_pShellDispatcher->m_fMouseCurrentX-0.5f, _pShellDispatcher->m_fMouseCurrentY-0.5f), v))
			{
				char cb[20];
				_itoa(int(v.z - vMap.hZeroPlast), cb, 10);

				terRenderDevice->SetFont(m_hFontCursorWorkarea);
				terRenderDevice->OutText(terScreenSizeX*(_pShellDispatcher->m_fMouseCurrentX + sqshCursorWorkAreaDx), 
					terScreenSizeY*(_pShellDispatcher->m_fMouseCurrentY + sqshCursorWorkAreaDy), cb, sColor4f(1, 1, 1, 1));
				terRenderDevice->SetFont(0);
			}
		}
}

float CShellCursorManager::GetSize()
{
	if(m_pActiveCursor)
		return m_pActiveCursor->sx;

	return 0;
}
void  CShellCursorManager::SetSize(float sx)
{
	if(m_pActiveCursor && !m_pActiveCursor->bCursorSystem)
	{
		m_pActiveCursor->sx = sx;
		m_pActiveCursor->sy = sx;//*(float)(terScreenSizeX)/(float)(terScreenSizeY);

		if(m_pActiveCursor == &m_cursors[workarea_in])
		{
			m_cursors[workarea_out].sx = m_pActiveCursor->sx;
			m_cursors[workarea_out].sy = m_pActiveCursor->sy;
		}

		if(m_pActiveCursor == &m_cursors[workarea_out])
		{
			m_cursors[workarea_in].sx = m_pActiveCursor->sx;
			m_cursors[workarea_in].sy = m_pActiveCursor->sy;
		}

		cur_x = gameShell->mousePressControl().x - m_pActiveCursor->sx*fWorkAreaCenterX/terScreenSizeX;
		cur_y = gameShell->mousePressControl().y - m_pActiveCursor->sy*fWorkAreaCenterY/terScreenSizeY;
	}
}
void CShellCursorManager::HideCursor()
{
	_bCursorVisible = 0;

	if(m_pActiveCursor && m_pActiveCursor->bCursorSystem)
		SetCursor(0);
}
void CShellCursorManager::ShowCursor()
{
	_bCursorVisible = 1;

	OnWMSetCursor();
}

///////////////////////////////////////////////////////////////////////////



CShellIconManager::CShellIconManager()
{
	m_pDesktop = 0;
	m_pFocus = 0;
	m_pCtrlHover = 0;
	m_fTimePressed = 0;
	m_pLastClicked = 0;
	m_fEffectTime = 0;
	m_hFontPopup = 0;
	m_hFontCountDownTime = 0;
	m_hPopupTexture = 0;
	m_pModalWnd = 0;
	m_bEnergyCanBuild = true;
	cutSceneAnimTimer = 0;

	m_editMode = false;

	speechSound = 0;
	resultMusicStarted = false;

	m_fPhase = 0;

	m_fProgressEnergy = 0;
	m_nPromptMessageDelayTime = 0;

	m_hTextureProgressBars = 0;

	m_nTotalUnitsBaseCount = 0;

	m_nControlGroupLoaded = -1;

	externalBuildTabStates.resize(3);
	externalSquadTabStates.resize(5);
	int i;
	int s;
	for (i = 0, s = externalBuildTabStates.size(); i < s; i++) {
		externalBuildTabStates[i].tabNumber = i;
	}
	
	for (i = 0, s = externalSquadTabStates.size(); i < s; i++) {
		externalSquadTabStates[i].tabNumber = i;
	}

	cutSceneModeOn = false;
	cutSceneCommand.active = false;

	game_speed_to_resume = 0;
}

void CShellIconManager::Done()
{
	if(m_pDesktop)
	{
		Destroy(m_pDesktop);
		m_pDesktop = 0;
	}

	if (speechSound) {
		SNDEnableVoices(true);
		speechSound->Stop();
		delete speechSound;
		speechSound = 0;
	}

	cutSceneModeOn = false;
	cutSceneCommand.active = false;
	m_pFocus = 0;
	m_pCtrlHover = 0;
	m_fTimePressed = 0;
	m_pLastClicked = 0;
	_bMenuMode = 0;

	_RELEASE(m_hFontPopup);
	_RELEASE(m_hFontCountDownTime);
	_RELEASE(m_hPopupTexture);

	_RELEASE(hFontMainmenu1);
	_RELEASE(hFontMainmenu2);
	_RELEASE(hFontMainmenu3);
	_RELEASE(hFontMainmenu4);

	_RELEASE(m_hTextureProgressBars);

	ClearSquadIconTable();

	m_nControlGroupLoaded = -1;
	miniMapEventIcons.clear();
}
CShellIconManager::~CShellIconManager()
{
//	Done();
	if (speechSound) {
		delete speechSound;
		speechSound = 0;
	}
}

void CShellIconManager::addChatString(const string& newChatString) {
	CChatInfoWindow* wnd;
	if (cutSceneModeOn) {
		wnd = (CChatInfoWindow*)controls[SQSH_CHAT_INFO_ID];
	} else {
		wnd = (CChatInfoWindow*)GetWnd(SQSH_CHAT_INFO_ID);
	}
	xassert(wnd);
	if (wnd) {
		wnd->addString(newChatString);
		wnd->Show(true);
		wnd->updateTime(3000);
	}
}

void CShellIconManager::showHintDisconnect(const string& players, int showTime, bool disconnected) {
	CChatInfoWindow* wnd;
	if (cutSceneModeOn) {
		wnd = (CChatInfoWindow*)controls[SQSH_CHAT_INFO_ID];
	} else {
		wnd = (CChatInfoWindow*)GetWnd(SQSH_CHAT_INFO_ID);
	}
	if (wnd) {
		string res = qdTextDB::instance().getText(disconnected ? "Interface.Menu.Messages.PlayersDisconnected" : "Interface.Menu.Messages.PlayersExited");

		const int bufferSize = 200;
		static char tempBuffer[bufferSize];
		sprintf(tempBuffer, res.c_str(), players.c_str());


		wnd->addString(tempBuffer);
		wnd->Show(true);
		wnd->updateTime(showTime);
	}
}
void CShellIconManager::showHint(const char* text, int showTime, ActionTask::Type actionType) {
	if (gameShell->currentSingleProfile.getLastGameType() == UserSingleProfile::LAN) {
		return;
	}

	CHintWindow* wnd;
	if (cutSceneModeOn) {
		wnd = (CHintWindow*)controls[SQSH_HINT_ID];
	} else {
		wnd = (CHintWindow*)GetWnd(SQSH_HINT_ID);
	}
	xassert(wnd);
	if (wnd) {
		string taskTxt(text);
		list<Task>::iterator i;
		FOR_EACH(tasks, i) {
			if (taskTxt == (*i).text) {
				break;
			}
		}

		if (i == tasks.end() && actionType != ActionTask::ASSIGNED) {
			return;
		}

		string res = qdTextDB::instance().getText(text);
		switch (actionType) {
			case ActionTask::COMPLETED:
				res = qdTextDB::instance().getText("Interface.Tips.Completed") + res;
				break;
			case ActionTask::FAILED:
				res = qdTextDB::instance().getText("Interface.Tips.Failed") + res;
				break;
			case ActionTask::TO_DELETE:
				return;
		}
		wnd->SetText(res.c_str());
		wnd->Show(true);
		wnd->SetTime(showTime);
	}
}

void CShellIconManager::setTask(const char* id, ActionTask::Type actionType) {
	string taskTxt(id);
	list<Task>::iterator i;
	FOR_EACH(tasks, i) {
		if (taskTxt == (*i).text) {
			break;
		}
	}

	if (i == tasks.end() && actionType == ActionTask::ASSIGNED) {
		tasks.push_back(Task(taskTxt, actionType));
	} else if (i != tasks.end()) {
		if (actionType != ActionTask::TO_DELETE) {
			(*i).type = actionType;
		} else  {
			tasks.erase(i);
		}
	}

	fillTaskWnd();
}
#include "HistoryScene.h"
extern HistoryScene historyScene;
void CShellIconManager::fillTaskWnd() {
	string taskTxt;
	if (gameShell->currentSingleProfile.getLastGameType() == UserSingleProfile::SCENARIO) {
		taskTxt = qdTextDB::instance().getText(historyScene.getMission(historyScene.getMissionNumberToExecute()).name.c_str());
		taskTxt += "\n\n";
	}
	
	list<Task>::iterator i;
	FOR_EACH(tasks, i) {
		switch ((*i).type) {
			case ActionTask::COMPLETED:
				taskTxt += "&00FF00";
				break;
			case ActionTask::FAILED:
				taskTxt += "&FF0000";
				break;
			default:
				taskTxt += "&FFFFFF";
		}
		taskTxt += string(qdTextDB::instance().getText((*i).text.c_str())) + "\n";
	}

	CTextWindow* wnd;
	if (cutSceneModeOn) {
		wnd = (CTextWindow*)controls[SQSH_MM_MISSION_TASK_TXT];
	} else {
		wnd = (CTextWindow*)GetWnd(SQSH_MM_MISSION_TASK_TXT);
	}
	xassert(wnd);
	if (wnd) {
		wnd->SetText(taskTxt.c_str());
	}
}
void CShellIconManager::load(const SaveTaskList& tasksIn) {
	tasks.clear();
	SaveTaskList::const_iterator ti;
	FOR_EACH(tasksIn, ti) 
		tasks.push_back(Task((const char*)ti->id, ti->type));

	fillTaskWnd();
}

void CShellIconManager::save(SaveTaskList& tasksOut) const {
	tasksOut.clear();
	TaskList::const_iterator ti;
	FOR_EACH(tasks, ti) 
		tasksOut.push_back(SaveTask(ti->text.c_str(), ti->type));
}

void CShellIconManager::onSizeChanged() {
	_pShellDispatcher->updateSmallCamera();

	if (m_pDesktop) {
		reload(m_pDesktop);
	}

	//scale textures
	_RELEASE(m_hPopupTexture);
	_RELEASE(m_hTextureProgressBars);
	m_hPopupTexture = terVisGeneric->CreateTexture(sPopupTexture);
	m_hTextureProgressBars = terVisGeneric->CreateTexture(progress_texture);
}

void CShellIconManager::reload(CShellWindow* pTop) {
	if (pTop) {
		pTop->reload();
		list<CShellWindow*>::iterator i;
		FOR_EACH(pTop->m_children, i)
			if (*i) reload(*i);
	}
}

void CShellIconManager::LoadControlsGroup(int nGroup, bool force)
{
	xassert(nGroup > 0);

	hold = qdTextDB::instance().getText("Interface.Labels.Hold");
	units = qdTextDB::instance().getText("Interface.Labels.Units");
	totalTime = qdTextDB::instance().getText("Interface.Labels.TotalTime");

	if(!force && m_nControlGroupLoaded == nGroup)
		return;

	tasks.clear();

	_WaitCursor();

	Done();

	m_pLastClicked = 0;
	m_pModalWnd = 0;

	if (nGroup == SHELL_LOAD_GROUP_GAME) {
		speechSound = new MpegSound();
		resultMusicStarted = false;
	}

	for (int id = 0; id <= SQSH_MAX; id++) {
		controls[id] = 0;
	}

	m_pDesktop = new CShellWindow(-1, 0);

	if(nGroup == SHELL_LOAD_GROUP_GAME) {
		LoadTabSheets();
//		OpenPromtMessagesFile();
	}

	for(int i=0; i< interfaceAttr().controls.size(); i++)
	{
		sqshControlContainer& cont = interfaceAttr().controls[i];

		if((cont.load_group & nGroup) == 0)
			continue;

		CShellWindow* pWndCntr = CreateWnd(cont.id, cont.type, 0, _GetEventHandler(cont.id));
		xassert(cont.id <= SQSH_MAX);
//		controls[cont.id] = pWndCntr;
		xassert(pWndCntr);

		pWndCntr->LoadMenuWnd(&cont);
		pWndCntr->Move(cont.x, cont.y, cont.sx, cont.sy);
//		pWndCntr->scale(Vect2f(terScreenSizeX / 1024.0f, terScreenSizeY / 768.0f));
		pWndCntr->state = cont.state;

		for(int c=0; c<cont.controls.size(); c++)
		{
			const sqshControl& tc = cont.controls[c];

			CShellWindow* pCtrl = CreateWnd(tc.id, tc.type, pWndCntr, _GetEventHandler(tc.id));
			xassert(tc.id <= SQSH_MAX);
//			controls[tc.id] = pCtrl;
			if(pCtrl)
			{
				pCtrl->state = tc.state;
				pCtrl->Load(&tc);
				pCtrl->Move(tc.x, tc.y, tc.sx, tc.sy);
//				pCtrl->scale(Vect2f(terScreenSizeX / 1024.0f, terScreenSizeY / 768.0f));
			}
		}
		if((cont.state & SQSH_VISIBLE) == 0)
			pWndCntr->Show(0);
	}

	//post load
	hFontMainmenu1 = terVisGeneric->CreateFont(shell_main_menu_font, shell_main_menu_font_size1);
	hFontMainmenu2 = terVisGeneric->CreateFont(shell_main_menu_font, shell_main_menu_font_size2);
	hFontMainmenu3 = terVisGeneric->CreateFont(shell_main_menu_font, shell_main_menu_font_size3);
	hFontMainmenu4 = terVisGeneric->CreateFont(shell_main_menu_font, shell_main_menu_font_size4);

	switch(nGroup)
	{
	case SHELL_LOAD_GROUP_MENU:
		{
			_shellCursorManager.m_bShowSideArrows=0;
			_bMenuMode = 1;
//			if (gameShell->GameActive) {
//				gameShell->pauseGame(true);
//			}
			((CTextWindow*) GetWnd(SQSH_MM_VERSION_TXT))->setText(currentShortVersion);
		}
		break;

	case SHELL_LOAD_GROUP_GAME:
		{
			_shellCursorManager.m_bShowSideArrows = 1;
			_bMenuMode = 0;

			m_hFontPopup = terVisGeneric->CreateFont(sqshFontPopup, infoWndFontSize);
			m_hFontCountDownTime = terVisGeneric->CreateFont(sqshFontCountDownTime, sqshFontCountDownTimeSize);
			m_hPopupTexture = terVisGeneric->CreateTexture(sPopupTexture);

			PostLoadTabSheets();

			BuildSquadIconTable();
//			OpenPromtMessagesFile();
			GetWnd(SQSH_REPLAY_PLAYER_BUTTON_ID)->Show(gameShell->currentSingleProfile.getLastGameType() == UserSingleProfile::REPLAY);
		}
		break;
	}
		   
	m_hTextureProgressBars = terVisGeneric->CreateTexture(progress_texture);

	m_nControlGroupLoaded = nGroup;
}

void CShellIconManager::LoadTabSheets()
{
	for(int i=0; i < interfaceAttr().tabSheets.size(); i++)
	{
		sqshTabSheet& sheet = interfaceAttr().tabSheets[i];

		CUITabSheet* pSheet = new CUITabSheet(sheet.id, m_pDesktop, _GetEventHandler(sheet.id));
		xassert(sheet.id <= SQSH_MAX);
		controls[sheet.id] = pSheet;
		pSheet->Load(&sheet);
//		pSheet->scale(Vect2f(terScreenSizeX / 1024.0f, terScreenSizeY / 768.0f));
		pSheet->Show(1);
	}
}
void CShellIconManager::PostLoadTabSheets()
{
	//подставить SQSH_BACKGRND_ID как parent для tabsheets 
	CShellWindow* pBkg = GetWnd(SQSH_BACKGRND_ID);
	if(pBkg)
	{
		CShellWindow* pTab;
		list<CShellWindow*>::iterator i_tab;
				
		//buildings tab
		FOR_EACH(m_pDesktop->m_children, i_tab)
		{
			if((*i_tab)->ID == SQSH_TAB_BUILD_ID)
			{
				pTab = *i_tab;
				m_pDesktop->m_children.erase(i_tab);
				break;
			}
		}
		pBkg->m_children.push_back(pTab);

		//squad tab
		FOR_EACH(m_pDesktop->m_children, i_tab)
		{
			if((*i_tab)->ID == SQSH_TAB_SQUAD_ID)
			{
				pTab = *i_tab;
				m_pDesktop->m_children.erase(i_tab);
				break;
			}
		}
		pBkg->m_children.push_back(pTab);
	}

	((CUITabSheet*) GetWnd(SQSH_TAB_BUILD_ID))->postLoad();
}

void CShellIconManager::OpenPromtMessagesFile()
{
}
char* CShellIconManager::FormatMessageText(const char* cbTag, char* cb, ...)
{
	const int bufferSize = 2000;
	static char cbTempBuffer[bufferSize];

	string text;
	//ищем тэг
	if (*cbTag == '<') {
		string tag = cbTag;
		tag = tag.substr(1, tag.length() - 2);
		tag = "Interface.Tips." + tag;
		text = qdTextDB::instance().getText(tag.c_str());
	}
	if (text.length()) {
		strncpy(cbTempBuffer, text.c_str(), text.length());
		cbTempBuffer[text.length()] = '\0';

		va_list va;
		va_start(va, cb);
		vsprintf(cb, cbTempBuffer, va);
		va_end(va);
	}
	else
		*cb = '\0';

	xassert(strlen(cb) < bufferSize);

	return cb;
}

void CShellIconManager::setCutSceneMode(bool on, bool animated) {
	if (!on) {
		if (animated) {
			cutSceneAnimDirection = -1;
			cutSceneAnimStep = 0;
			cutSceneAnimTimer = CUT_SCENE_FADEOUT_TIME;
		} else {
			onCutSceneStop();
			terCamera->SetFrustumGame();
			_shellCursorManager.ShowCursor();
			cutSceneAnimDirection = 0;
			cutSceneAnimTimer = 0;
			cutSceneAnimStep = 0;
		}
	} else {
		if (animated) {
			onCutSceneStart();
			cutSceneAnimDirection = 1;
			_shellCursorManager.HideCursor();
			cutSceneAnimTimer = CUT_SCENE_FADEOUT_TIME;
			cutSceneAnimStep = 0;
		} else {
			onCutSceneStart();
			terCamera->SetFrustumCutScene();
			_shellCursorManager.HideCursor();
			cutSceneAnimDirection = 0;
			cutSceneAnimTimer = 0;
			cutSceneAnimStep = 0;
		}
	}
}

void CShellIconManager::setCutSceneModeSafe(bool on, bool animated) {
	cutSceneCommand.on = on;
	cutSceneCommand.animated = animated;
	cutSceneCommand.active = true;
}

void CShellIconManager::onCutSceneStart() {
	cutSceneModeOn = true;
	SNDEnableVoices(false);
	game_speed_to_resume = gameShell->getSpeed();
	gameShell->setSpeed(1);
}
void CShellIconManager::onCutSceneStop() {
	if (cutSceneModeOn) {
		gameShell->setSpeed(game_speed_to_resume);
		cutSceneModeOn = false;
	}
}

void CShellIconManager::Toggle(bool game_active)
{
	if(!m_pDesktop)
	{
		if(game_active)
		{
			LoadControlsGroup(SHELL_LOAD_GROUP_GAME);

			terCamera->SetFrustumGame();
		}
		else
		{
			LoadControlsGroup(SHELL_LOAD_GROUP_MENU);
			SwitchMenuScreens(-1, SQSH_MM_START_SCR);
		}

		_shellCursorManager.ShowCursor();
	}
	else
	{
		Done();
		_shellCursorManager.HideCursor();

		terCamera->SetFrustumMenu();
	}
}
void CShellIconManager::speedChanged(float speed) {
	if (speechSound) {
		if (speed) {
			if (speechSound->IsPlay() == MPEG_PAUSE) {
				speechSound->Resume();
			}
		} else {
			if (speechSound->IsPlay() == MPEG_PLAY) {
				speechSound->Pause();
			}

		}
	}
}

float CShellIconManager::playSpeech(const char* id) {
	string sound = qdTextDB::instance().getSound(id);
	if (terSoundEnable && speechSound && !sound.empty()) {
		int pos = sound.find("Voice");
		if(pos != string::npos)
			sound.erase(0, pos);
		string soundName = gameShell->getLocDataPath() + sound;
		SNDEnableVoices(false);
		speechSound->SetVolume(round(255*terSoundVolume));
		int ret = speechSound->OpenToPlay(soundName.c_str(), false);
		xassert(ret);
		return speechSound->GetLen();
	}
	return 0;
}
void CShellIconManager::playMusic(const char* musicNamePath) {
	if (terSoundEnable && speechSound) {
		gb_Music.FadeVolume(0.5f, 0 );
		speechSound->FadeVolume(0.5f, round(255*terSoundVolume) );
		resultMusicStarted = true;
		int ret = speechSound->OpenToPlay(musicNamePath, false);
		xassert(ret);
	}
}
bool CShellIconManager::isSpeechPlay() {
	return speechSound ? speechSound->IsPlay() : false;
}
void CShellIconManager::setupAudio() {
	if (speechSound) {
		if (!terSoundEnable) {
			speechSound->Stop();
		}
		speechSound->SetVolume( round(255 * terSoundVolume) );
	}
}

void CShellIconManager::Destroy(CShellWindow* pTop)
{
//	if (gameShell && universe()) {
//		gameShell->resumeGame();
//	}

	list<CShellWindow*>::iterator i;
	FOR_EACH(pTop->m_children, i)
		Destroy(*i);

	delete pTop;
}

CShellWindow* CShellIconManager::GetWnd(int id)
{
//	if(!getDesktop())
//		return 0;


	if(getDesktop() && m_pDesktop->ID == id)
		return m_pDesktop;
	else
		return controls[id];

//	return FindWnd(m_pDesktop, id);
}

CShellWindow* CShellIconManager::CreateWnd(int id, ShellControlType type, CShellWindow* pParent, EVENTPROC proc)
{
	CShellWindow* pWnd = 0;

	if(pParent == 0)
		pParent = m_pDesktop;

	switch(type)
	{
	case SQSH_GENERAL_TYPE:
		pWnd = new CShellWindow(id, pParent, proc);
		break;

	case SQSH_PUSH_BUTTON_TYPE:
		pWnd = new CShellPushButton(id, pParent, proc);
		break;

	case SQSH_LIST_BOX_TYPE:
		pWnd = new CListBoxWindow(id, pParent, proc);
		break;

	case SQSH_CHAT_BOX_TYPE:
		pWnd = new ChatWindow(id, pParent, proc);
		break;

	case SQSH_STATS_LIST_TYPE:
		pWnd = new CStatListBoxWindow(id, pParent, proc);
		break;

	case SQSH_SLIDER_TYPE:
		pWnd = new CSliderWindow(id, pParent, proc);
		break;

	case SQSH_COMBO_TYPE:
		pWnd = new CComboWindow(id, pParent, proc);
		break;

	case SQSH_COLOR_COMBO_TYPE:
		pWnd = new CColorComboWindow(id, pParent, proc);
		break;

	case SQSH_EDIT_BOX_TYPE:
		pWnd = new CEditWindow(id, pParent, proc);
		break;

	case SQSH_INGAME_CHAT_EDIT_BOX_TYPE:
		pWnd = new CChatInGameEditWindow(id, pParent, proc);
		break;
		
	case SQSH_MAP_WINDOW_TYPE:
		pWnd = new CMapWindow(id, pParent, proc);
		break;

	case SQSH_PORTRAIT_TYPE:
		pWnd = new CPortraitWindow(id, pParent, proc);
		break;

	case SQSH_NOMAD_LOGO_TYPE:
		pWnd = new CLogoWindow(id, pParent, proc);
		break;
		
	case SQSH_LEGION_BUTTON_TYPE:
		pWnd = new CShellLegionButton(id, pParent, proc);
		break;

	case SQSH_CREDITS_TEXT_TYPE:
		pWnd = new CCreditsWindow(id, pParent, proc);
		break;

	case SQSH_TEXT_WINDOW_TYPE:
		pWnd = new CTextWindow(id, pParent, proc);
		break;

	case SQSH_TEXT_STRING_WINDOW_TYPE:
		pWnd = new CTextStringWindow(id, pParent, proc);
		break;

	case SQSH_COMPLEX_PUSH_BUTTON_TYPE:
		pWnd = new CShellComplexPushButton(id, pParent, proc);
		break;

	case SQSH_ATOM_BUTTON_TYPE:
		pWnd = new CShellAtomButton(id, pParent, proc);
		break;

	case SQSH_MULTITEX_WINDOW:
		pWnd = new CMultiTexWindow(id, pParent, proc);
		break;

	case SQSH_MOVE_BUTTON_TYPE:
		pWnd = new CMoveButton(id, pParent, proc);
		break;

	case SQSH_SCALE_BUTTON_TYPE:
		pWnd = new CScaleButton(id, pParent, proc);
		break;

	case SQSH_SCALE_RESULT_BUTTON_TYPE:
		pWnd = new CScaleResultButton(id, pParent, proc);
		break;

	case SQSH_PUSHSCALE_BUTTON_TYPE:
		pWnd = new CPushScaleButton(id, pParent, proc);
		break;

	case SQSH_WORLD_WINDOW_TYPE:
		pWnd = new CWorldEffect(id, pParent, proc);
		break;

	case SQSH_SPLASHSCREEN:
		pWnd = new CSplashWindow(id, pParent, proc);
		break;

	case SQSH_MAPWINDOW:
		pWnd = new CShowMapWindow(id, pParent, proc);
		break;

	case SQSH_BUTTON_TYPE:
		pWnd = new CPushButton(id, pParent, proc);
		break;

	case SQSH_TERRAINBUILDBUTTON_TYPE:
		pWnd = new CTerrainBuildButton(id, pParent, proc);
		break;

	case SQSH_PROGRESS_COLLECTED_TYPE:
		pWnd = new CProgressCollected(id, pParent, proc);
		break;

	case SQSH_PROGRESS_ENERGY_TYPE:
		pWnd = new CProgressEnergy(id, pParent, proc);
		break;

	case SQSH_PROGRESS_SHIELD_TYPE:
		pWnd = new CProgressShield(id, pParent, proc);
		break;

	case SQSH_PROGRESS_TERRAIN_TYPE:
		pWnd = new CProgressTerrain(id, pParent, proc);
		break;

	case SQSH_PROGRESS_MUTATION_TYPE:
		pWnd = new CProgressMutation(id, pParent, proc);
		break;

	case SQSH_INFOWND_TYPE:
		pWnd = new CInfoWindow(id, pParent, proc);
		break;

	case SQSH_CHATINFO_WND_TYPE:
		pWnd = new CChatInfoWindow(id, pParent, proc);
		break;

	case SQSH_HINTWND_TYPE:
		pWnd = new CHintWindow(id, pParent, proc);
		break;

	case SQSH_DLG_TYPE:
		pWnd = new CDialogWindow(id, pParent, proc);
		break;

	case SQSH_PROGRESS_UNIT_CHARGE_TYPE:
		pWnd = new CProgressUnitCharge(id, pParent, proc);
		break;

	case SQSH_BACKGROUND_TYPE:
		pWnd = new CBackgroundWindow(id, pParent, proc);
		break;

	case SQSH_GENERAL_WND_TYPE:
		pWnd = new CGeneralWindow(id, pParent, proc);
		break;
		
	case SQSH_PLAYER_COLOR_WND_TYPE:
		pWnd = new CPlayerColorWnd(id, pParent, proc);
		break;

	case SQSH_REPLAY_PLAYER_BUTTON_TYPE:
		pWnd = new CReplayPlayerPushButton(id, pParent, proc);
		break;

	default:
		pWnd = new CShellWindow(id, pParent, proc);
	}
	
	controls[id] = pWnd;
	return pWnd;
}

void CShellIconManager::AddDynamicHandler(DYNCALLBACK _p, int code, int delay)
{
	MTAuto dynQueue_autolock(&dynQueue_lock);

//	fout < "AddDynamicHandler\n";

	list<DYN_QUEUE_ITEM>::iterator i;
	FOR_EACH(m_dyn_queue, i)
		if((i->cbproc == _p) && (i->code == code))
		{
			i->bDelete = 0;
			i->time_delay = delay;
			return;
		}

	m_dyn_queue.push_back(DYN_QUEUE_ITEM(_p, code, delay));
}
void CShellIconManager::DelDynamicHandler(DYNCALLBACK _p, int code)
{
	MTAuto dynQueue_autolock(&dynQueue_lock);

//	fout < "DelDynamicHandler\n";

	list<DYN_QUEUE_ITEM>::iterator i;
	FOR_EACH(m_dyn_queue, i)
		if(( (i->cbproc == _p) || (_p == 0) ) && ( (i->code == code) || (code == 0) ))
		{
			i->bDelete = 1;
			break;
		}
}

bool CShellIconManager::HasDynamicHandler(DYNCALLBACK _p, int code) {
	MTAuto dynQueue_autolock(&dynQueue_lock);

	list<DYN_QUEUE_ITEM>::iterator i;
	FOR_EACH (m_dyn_queue, i) {
		if (( (i->cbproc == _p) || (_p == 0) ) && ( (i->code == code) || (code == 0) )) {
			return true;
		}
	}
	return false;
}

CShellWindow* CShellIconManager::HitTest(float x, float y)
{
	if(!getDesktop())
		return 0;

	CShellWindow* pWnd = HitTest(m_pDesktop, x, y);
	if(pWnd && (pWnd->state & SQSH_TRANSPARENT) == 0)
		return pWnd; 

	return m_pDesktop;
}

CShellWindow* CShellIconManager::HitTest(CShellWindow* pTop, float x, float y)
{
	CShellWindow* p = 0;

	list<CShellWindow*>::reverse_iterator i;
	for(i = pTop->m_children.rbegin(); i != pTop->m_children.rend(); i++)
		if((*i)->HitTest(x, y))
		{
			if(p = HitTest(*i, x, y))
				return p;
		}
	return pTop;
}

CShellWindow* CShellIconManager::FindWnd(CShellWindow* pTop, int id)
{
	CShellWindow* p = 0;

	if(pTop->ID == id)
		p = pTop;
	else
	{
		list<CShellWindow*>::iterator i;
		FOR_EACH(pTop->m_children, i)
			if(p = FindWnd(*i, id))
				break;
	}
	return p;
}

void CShellIconManager::SetFocus(int id)
{
	m_pLastClicked = GetWnd(id);
}

int CShellIconManager::IsFocusControl()
{
	return _bMenuMode && (m_pLastClicked != 0);
}

bool CShellIconManager::isInEditMode() const
{
	return (m_pLastClicked != 0 && m_pLastClicked->isEditWindow());
}

void CShellIconManager::SetModalWnd(int id)
{
	m_pModalWnd = id ? GetWnd(id) : 0;
}

int CShellIconManager::OnMouseMove(float x, float y)
{
	if(!getDesktop())
		return ProcessDynQueue(CBCODE_MOUSEMOVE, x, y);


	int r = 0;
	CShellWindow* pWnd = HitTest(x, y);
	if (m_pModalWnd && pWnd != m_pModalWnd && pWnd->m_pParent != m_pModalWnd) {
		return 1;
	}
	if(pWnd != m_pDesktop)
	{
		if(m_pCtrlHover != pWnd)
		{
			if(m_pCtrlHover)
				m_pCtrlHover->OnWindow(0);

			(m_pCtrlHover = pWnd)->OnWindow(1);
		}

		pWnd->OnMouseMove(x, y);
		r = 1;
	}
	else
	{
		if(m_pFocus)
		{
			m_pFocus->OnLButtonUp(x, y);
			m_pFocus = 0;
		}

		if(m_pCtrlHover)
			m_pCtrlHover->OnWindow(0);

		m_pCtrlHover = 0;

		ProcessDynQueue(CBCODE_MOUSEMOVE, x, y);
	}
	return (_bMenuMode || r);
}
int CShellIconManager::OnLButtonDown(float x, float y)
{
	m_nMouseButtonsState |= MK_LBUTTON;

	if(!getDesktop())
		return ProcessDynQueue(CBCODE_LBDOWN, x, y);


	int r = 0;
	CShellWindow* pWnd = HitTest(x, y);
	if (m_pModalWnd && pWnd != m_pModalWnd && pWnd->m_pParent != m_pModalWnd) {
		return 1;
	}
	if(pWnd != m_pDesktop)
	{
		if (_pShellDispatcher->m_nEditRegion != editRegionNone) {
			ProcessDynQueue(CBCODE_RBDOWN, x, y);
		}
		(m_pLastClicked = m_pFocus = pWnd)->OnLButtonDown(x, y);
		if (universe()) {
			universe()->checkEvent(EventClickOnButton((ShellControlID)pWnd->ID));
		}
		m_fTimePressed = frame_time();
		r = 1;
	}
	else
	{
		m_pLastClicked = 0;
		r = ProcessDynQueue(CBCODE_LBDOWN, x, y);
	}

	return (_bMenuMode || r);
}
int CShellIconManager::OnLButtonUp(float x, float y)
{
	if(!getDesktop())
		return ProcessDynQueue(CBCODE_LBUP, x, y);
	int r = 0;
	CShellWindow* pWnd = HitTest(x, y);
	if (m_pModalWnd && pWnd != m_pModalWnd && pWnd->m_pParent != m_pModalWnd) {
		return 1;
	}
	if(pWnd != m_pDesktop)
	{
		pWnd->OnLButtonUp(x, y);
		m_pFocus = 0;
		r = 1;
	}
	else
		r = ProcessDynQueue(CBCODE_LBUP, x, y);

	m_nMouseButtonsState &= ~MK_LBUTTON;
	return (_bMenuMode || r);
}
void CShellIconManager::lButtonReset() {
	m_nMouseButtonsState &= ~MK_LBUTTON;
}
int CShellIconManager::OnLButtonDblClk(float x, float y)
{
	CShellWindow* pWnd = HitTest(x, y);
	if(pWnd != getDesktop()) {
		pWnd->OnLButtonDblClk(x, y);
		return 1;
	}

	return 0;
}
int CShellIconManager::OnRButtonDown(float x, float y)
{
	m_nMouseButtonsState |= MK_RBUTTON;

	int r = 0;
	CShellWindow* pWnd = HitTest(x, y);
	if (m_pModalWnd && pWnd != m_pModalWnd && pWnd->m_pParent != m_pModalWnd) {
		return 1;
	}
	if(pWnd != getDesktop())
	{
		pWnd->OnRButtonDown(x, y);
		m_pFocus = 0;
		r = 1;
	}
	else
		r = ProcessDynQueue(CBCODE_RBDOWN, x, y);

	return (_bMenuMode || r);
}
int CShellIconManager::OnRButtonUp(float x, float y)
{
	int r = 0;
	CShellWindow* pWnd = HitTest(x, y);
	if (m_pModalWnd && pWnd != m_pModalWnd && pWnd->m_pParent != m_pModalWnd) {
		return 1;
	}
	if(pWnd != getDesktop())
	{
		pWnd->OnRButtonUp(x, y);
		m_pFocus = 0;
		r = 1;
	}
	else
		r = ProcessDynQueue(CBCODE_RBUP, x, y);

	m_nMouseButtonsState &= ~MK_RBUTTON;
	return (_bMenuMode || r);
}
int CShellIconManager::OnMouseWheel(int delta)
{
	if(m_pCtrlHover)
	{
		m_pCtrlHover->OnMouseWheel((delta > 0) ? -3 : 3);
		return 1;
	}

	return 0;
}
int CShellIconManager::OnKeyDown(int key)
{
//	if(m_pModalWnd)
//	{
//		m_pModalWnd->OnKeyDown(key);
//		return 1;
//	}
	if (m_pModalWnd) {
		m_pLastClicked = m_pModalWnd;
	} else if (!m_pLastClicked || !m_pLastClicked->isVisible()) {
		int visID = getVisibleMenuScr();
		if (visID != -1) {
			m_pLastClicked = GetWnd(visID);
		}
	}

	if(m_pLastClicked && _bMenuMode)
	{
		m_pLastClicked->OnKeyDown(key);
		return 1;
	}

	return 0;
}
int CShellIconManager::OnChar(char key)
{
//	if(m_pModalWnd)
//	{
//		m_pModalWnd->OnChar(key);
//		return 1;
//	}

	if (m_pModalWnd) {
		m_pLastClicked = m_pModalWnd;
	}

	if(m_pLastClicked)
	{
		m_pLastClicked->OnChar(key);
		return 1;
	}
	return 0;
}
int CShellIconManager::OnKeyUp(int key)
{
//	if(m_pModalWnd)
//	{
//		m_pModalWnd->OnKeyUp(key);
//		return 1;
//	}
	if (m_pModalWnd) {
		m_pLastClicked = m_pModalWnd;
	}

	if(m_pLastClicked && _bMenuMode)
	{
		m_pLastClicked->OnKeyUp(key);
		return 1;
	}

	return 0;
}

void CShellIconManager::DrawControls(CShellWindow* pTop)
{
	if(pTop->ID != SQSH_BACKGRND_ID)
		pTop->draw(m_pLastClicked == pTop);

	list<CShellWindow*>::iterator i;
	FOR_EACH(pTop->m_children, i)
		DrawControls(*i);

	if (pTop->ID == SQSH_GAME_SCREEN_ID) {
		terRenderDevice->FlushPrimitive2D();		
	}
}
int CShellIconManager::ProcessDynQueue(int code, float x, float y)
{
	MTAuto dynQueue_autolock(&dynQueue_lock);

//	fout < "ProcessDynQueue " < code < "\n";

	int r = 0;

	list<DYN_QUEUE_ITEM>::iterator i = m_dyn_queue.begin();
	while (i != m_dyn_queue.end()) {
//		fout < "	 DYN_QUEUE_ITEM=" < ((DWORD)i->cbproc) < "\n";
		if (i->bDelete) {
//			fout < "	 delete\n";
			i = m_dyn_queue.erase(i);
		} else {
			if ((i->code == code) && (i->time_delay <= 0)) {
//				fout < "	 process\n";
				if ( !(*i->cbproc)(x,y) ) {
					i->bDelete = 1;
					return 1;
				}
				r = 1;
			}
			i++;
		}
	}

	return r;
}
void CShellIconManager::QuantDynQueue(int dt)
{
	MTAuto dynQueue_autolock(&dynQueue_lock);

	list<DYN_QUEUE_ITEM>::iterator i;
	FOR_EACH(m_dyn_queue, i)
		i->time_delay -= dt;
}

bool CShellIconManager::isDynQueueEmpty() {
	MTAuto dynQueue_autolock(&dynQueue_lock);
	return (m_dyn_queue.size() == 0);
}


static char cbPromptBuffer[500];
static const char* pCurrentPrompt = 0;
//static bool bPeriodicMessage = false;
//static bool bInPeriod = false;

void CShellIconManager::quant(float dTime)
{
	if (cutSceneCommand.active) {
		setCutSceneMode(cutSceneCommand.on, cutSceneCommand.animated);
		cutSceneCommand.active = false;
	}

	m_fEffectTime -= dTime;
	if(m_pFocus)
	{
		//проверка на удерживание кнопки
		if(frame_time() - m_fTimePressed > nButtonHoldDelay)
			m_pFocus->OnLButtonHold();
	}

	if (!SNDIsVoicesEnabled() && speechSound && !speechSound->IsPlay() && !cutSceneModeOn) {
		SNDEnableVoices(true);
	}

	if (
			speechSound
		&&	!speechSound->IsPlay()
		&&	resultMusicStarted ) {

		resultMusicStarted = false;
		gb_Music.FadeVolume(2, round(255*terMusicVolume) );
	}

	gameShell->getLogicUpdater().lock();
	if (cutSceneModeOn) {
		cutSceneModeOn = false;
		UpdateIcons();
		cutSceneModeOn = true;
	} else {
		UpdateIcons();
	}
	gameShell->getLogicUpdater().unlock();

	if(terShowTips && !_bMenuMode)
	{
		//UpdatePromptText();

		m_nPromptMessageDelayTime -= dTime;
		m_fPhase += dTime;
	}

	QuantDynQueue(dTime);
	ProcessDynQueue(CBCODE_QUANT);
}

inline int _is_track(RECT& rc)
{
	return rc.left || rc.top || rc.right || rc.bottom;
}
inline int rect_width(RECT& rc)
{
	return rc.right - rc.left;
}
inline int rect_height(RECT& rc)
{
	return rc.bottom - rc.top;
}

void PopupFormatAttack(const AttributeBase* attr, char* cbBuffer, bool gun) {
	string attacks;
	string unitClass;
	int count = 0;
	for (int i = 1; i < unitClassKeyNumber; i++) {
		if (attr->UnitClass & unitClassDescriptionKeys[i].unitClass) {
			unitClass = qdTextDB::instance().getText(unitClassDescriptionKeys[i].textID);
		}
		if (attr->AttackClass == UNIT_CLASS_ALL && attacks.empty()) {
			count++;
			attacks = qdTextDB::instance().getText("Interface.UnitClass.All");
		} else if (attr->AttackClass != UNIT_CLASS_ALL && attr->AttackClass & unitClassDescriptionKeys[i].unitClass) {
			if (count) {
				if (count % attackClassTipAutoFormatCount) {
					attacks += ", ";
				} else {
					attacks += ", \n";
				}
			}
			count++;
			attacks += qdTextDB::instance().getText(unitClassDescriptionKeys[i].textID);
		}
	}
	//bad but developers have no time til release
	if (attr->ID == UNIT_ATTRIBUTE_TECHNIC || attr->ID == UNIT_ATTRIBUTE_DIGGER || !count) {
		attacks = qdTextDB::instance().getText(unitClassDescriptionKeys[0].textID);
	}

	string balance;
	static char cbTemp[200];
	if (attr->intfBalanceData.power) {
		_shellIconManager.FormatMessageText("<damage>", cbTemp, attr->intfBalanceData.power, attr->intfBalanceData.width );
		balance += "\n";
		balance += cbTemp;
	}
	if (!gun) {
		_shellIconManager.FormatMessageText("<armor>", cbTemp, round(attr->intfBalanceData.armor) );
		if (balance.empty()) {
			balance += "\n";
		}
		balance += cbTemp;
	}
	balance += "\n";

	_shellIconManager.FormatMessageText(
		"<Attack>",
		cbBuffer,
		balance.c_str(),
		unitClass.c_str(),
		attacks.c_str()
	);
}

void PopupFormatBuilding(const AttributeBase* attr, char* cbBuffer, bool onControl, terUnitBase* unit, bool ampl, bool gun)
{
	const UnitInterfacePrm& prm = attr->interfacePrm;

	string sRequired;

//	terPlayer* player = universe()->activePlayer();
	terPlayer* player = unit ? universe()->findPlayer(unit->playerID()) : universe()->activePlayer();
	bool enemy = unit ? (unit->playerID() != universe()->activePlayer()->playerID()) : false;

	static char cbTemp[200];
	_shellIconManager.FormatMessageText("<requires>", cbTemp);
	if (enemy) {
		if(attr->isBuilding())
		{
			EnableData& evolution = player->GetEvolutionBuildingData(attr->ID);
			const AttributeBuilding& attrB = *safe_cast<const AttributeBuilding*>(attr);
			int minVisibleCount = attrB.hasDowngrade() ? 2 : 1;
			if (attrB.EnableStructure.size() >= minVisibleCount) {
				sRequired = "\n";
				sRequired += cbTemp;
			}
			for(int i=0; i < attrB.EnableStructure.size(); i++)
			{
				if (attrB.downgrade() != attrB.EnableStructure[i]) {
					const AttributeBase* attr = player->unitAttribute(attrB.EnableStructure[i]);
					if (i) {
						sRequired += "\n  ";
					}
					sRequired += "&00FF00";
					sRequired += attr->interfaceName();
				}
			}
		}

//		if(sRequired.empty())
//			_shellIconManager.FormatMessageText(prm.popup, cbBuffer, "", attr->buildEnergy());
//		else
//		{
			sRequired += "&FFFFFF";
			if (gun) {
				PopupFormatAttack(attr, cbTemp, true);
				_shellIconManager.FormatMessageText(prm.popup, cbBuffer, sRequired.c_str(), "", attr->buildEnergy(), cbTemp);
			} else {
				float buildEnergy = attr->buildEnergy();
				_shellIconManager.FormatMessageText(prm.popup, cbBuffer, sRequired.c_str(), "", buildEnergy, ampl ? attr->energyCapacity : attr->MakeEnergy);
			}
//		}
	} else {
		if(attr->isBuilding())
		{
			EnableData& evolution = player->GetEvolutionBuildingData(attr->ID);
			const AttributeBuilding& attrB = *safe_cast<const AttributeBuilding*>(attr);
			int minVisibleCount = attrB.hasDowngrade() ? 2 : 1;
			if (attrB.EnableStructure.size() >= minVisibleCount) {
				sRequired = "\n";
				sRequired += cbTemp;
			}
			for(int i=0; i < attrB.EnableStructure.size(); i++)
			{
				if (attrB.downgrade() != attrB.EnableStructure[i]) {
					const AttributeBase* attr = player->unitAttribute(attrB.EnableStructure[i]);
					EnableData& reqEvolution = player->GetEvolutionBuildingData(attr->ID);

					if (i) {
						sRequired += "\n  ";
					}

					if(reqEvolution.Worked)
						sRequired += "&00FF00";
					else
						sRequired += "&FF0000";
					sRequired += attr->interfaceName();
				}
			}
		}
		bool noEnergy = false;
		//if ((unit && !unit->isBuildingPowerOn()) || onControl) 
		//	noEnergy = player->energyData().accumulated() < attr->buildEnergy();

//		if(sRequired.empty())
//			_shellIconManager.FormatMessageText(prm.popup, cbBuffer, (noEnergy ? "&FF0000" : ""), attr->buildEnergy());
//		else
//		{
			sRequired += "&FFFFFF";
			if (gun) {
				PopupFormatAttack(attr, cbTemp, true);
				_shellIconManager.FormatMessageText( prm.popup,	cbBuffer, sRequired.c_str(), (noEnergy ? "&FF0000" : ""), attr->buildEnergy(), cbTemp);
			} else {
				_shellIconManager.FormatMessageText(prm.popup, cbBuffer, sRequired.c_str(), (noEnergy ? "&FF0000" : ""), attr->buildEnergy(), ampl ? attr->energyCapacity : attr->MakeEnergy);
			}
//		}
	}

}
void PopupFormatCore(const AttributeBase* attr, char* cbBuffer, terUnitBase* unit)
{
	_shellIconManager.FormatMessageText(
		attr->interfacePrm.popup,
		cbBuffer,
		"",
		attr->buildEnergy(),
		attr->MakeEnergy,
		(unit && safe_cast<terProtector*>(unit)->canStartField()) ? "&00FF00" : "&FF0000");
}
void PopupFormatFrame(const AttributeBase* attr, char* cbBuffer, terUnitBase* unit)
{
	int spiralLevel = unit ? round(100 * safe_cast<terFrame*>(unit)->spiralLevel() - 0.5f) : 0;
	_shellIconManager.FormatMessageText(
		attr->interfacePrm.popup,
		cbBuffer,
		attr->MakeEnergy,
		attr->energyCapacity,
		spiralLevel);
}
void PopupFormatSquad(const AttributeBase* attr, char* cbBuffer, terUnitBase* unit)
{
	static char cbTemp[256];
	PopupFormatAttack(attr, cbTemp, false);
	_shellIconManager.FormatMessageText(
		attr->interfacePrm.popup,
		cbBuffer,
		attr->interfaceName(),
		cbTemp);
}
void PopupFormatMMP(const AttributeBase* attr, char* cbBuffer, terUnitBase* unit) {
	_shellIconManager.FormatMessageText(
		attr->interfacePrm.popup,
		cbBuffer);
}
void CShellIconManager::FormatUnitPopup(const AttributeBase* attr, char* cbBuffer, bool onControl, terUnitBase* unit)
{
	if(strlen(attr->interfacePrm.popup) == 0)
		return;

	switch(attr->interfacePrm.format_group)
	{
	case POPUP_FORMAT_GUN:
		PopupFormatBuilding(attr, cbBuffer, onControl, unit, false, true);
		break;
	case POPUP_FORMAT_BUILDING:
		PopupFormatBuilding(attr, cbBuffer, onControl, unit, false, false);
		break;
	case POPUP_FORMAT_CORE:
		PopupFormatCore(attr, cbBuffer, unit);
		break;
	case POPUP_FORMAT_FRAME:
		PopupFormatFrame(attr, cbBuffer, unit);
		break;
	case POPUP_FORMAT_AMPLIFIER:
		PopupFormatBuilding(attr, cbBuffer, onControl, unit, true, false);
		break;
	case POPUP_FORMAT_SQUAD:
		PopupFormatSquad(attr, cbBuffer, unit);
		break;
	case POPUP_FORMAT_MMP:
		PopupFormatMMP(attr, cbBuffer, unit);
		break;
	}

	if (strlen(cbBuffer) && unit && !unit->Player->isWorld()) {
//		string finalPopup(qdTextDB::instance().getText("Player"));
		string finalPopup("[");
		finalPopup += unit->Player->name();

		if(gameShell->CurrentMission.isMultiPlayer() || gameShell->currentSingleProfile.getLastGameType() == UserSingleProfile::BATTLE){
			string clan_str = getItemTextFromBase("Clan");

			char buff[30 + 1];
			sprintf(buff, "%d", (unit->Player->clan() + 1));

			finalPopup += " / ";
			finalPopup += clan_str;
			finalPopup += buff;
		}

		finalPopup += "&FFFFFF]\n\n";
		finalPopup += cbBuffer;
		strcpy(cbBuffer, finalPopup.c_str());
	}

}

void CShellIconManager::draw()
{
	terRenderDevice->FlushPrimitive2D();

	if (ht_intf_test) {
		if (getDesktop()) {
			//background
			CShellWindow* pBackgrnd = GetWnd(SQSH_BACKGRND_ID);
			if(pBackgrnd)
				pBackgrnd->draw(false);

			DrawControls(m_pDesktop);
		}
		_shellCursorManager.draw();
		return;
	}

	if(!mt_interface_quant)
		return;


	if(getDesktop())
	{
		if(IsInterface())
		{
			if(_is_track(_pShellDispatcher->m_rcTrack))
			{
				terRenderDevice->DrawRectangle(_pShellDispatcher->m_rcTrack.left, _pShellDispatcher->m_rcTrack.top, 
					rect_width(_pShellDispatcher->m_rcTrack), rect_height(_pShellDispatcher->m_rcTrack), sColor4c(0, 255, 0, 255), 1);

				terRenderDevice->FlushPrimitive2D();
			}
		}

		//background
		CShellWindow* pBackgrnd = GetWnd(SQSH_BACKGRND_ID);
		if(pBackgrnd)
			pBackgrnd->draw(false);

		DrawControls(m_pDesktop);

		if(terShowTips)
		{
			if(!_bMenuMode)
			{
				static char cbPopupBuffer[2000];

				if((GetWnd(SQSH_INFOWND_ID)->state & SQSH_VISIBLE) == 0)
				{
					*cbPopupBuffer = 0;

					if(m_pCtrlHover)
					{
						if(m_pCtrlHover->m_attr && strlen(m_pCtrlHover->m_attr->popup)) {
							m_pCtrlHover->OnFormatPopupMessage(cbPopupBuffer);
							m_pCtrlHover->OnFormatHotKeyPopupMessage(cbPopupBuffer);
						}
					}
//					else if(_pUnitHover && (_pUnitHover->playerID() == universe()->activePlayer()->playerID()))
					else if (_pUnitHover())
						FormatUnitPopup(&_pUnitHover->attr(), cbPopupBuffer, false, _pUnitHover());
//					else if(_pShellDispatcher->m_pUnitInfo)
//						FormatUnitPopup(_pShellDispatcher->m_pUnitInfo, cbPopupBuffer, false, 0);

					if(*cbPopupBuffer)
					{
						terRenderDevice->SetFont(m_hFontPopup);

						Vect2f v1, v2;
						OutTextRect(0, 0 , cbPopupBuffer, -1, v1, v2);
						v2.x += 2;

						int pos_x = terScreenSizeX - (v2.x-v1.x) - 1;
						int delta_y = absoluteY(nPopupTextPosY + nPopupSY) - (v2.y-v1.y);

						terRenderDevice->DrawSprite(pos_x - 2, delta_y, v2.x-v1.x + 2, v2.y-v1.y,
							0, 0, 1, 1, m_hPopupTexture, sColor4c(255,255,255,255));

						terRenderDevice->OutText(pos_x, delta_y, cbPopupBuffer, sColor4f(1, 1, 1, 1), -1);
						terRenderDevice->SetFont(0);


						Vect2f va(pos_x - 2, delta_y);
						Vect2f vb(va.x + v2.x-v1.x + 1, va.y + v2.y-v1.y);

						draw_rect_empty(va, vb, sColor4f(1, 1, 1, 1));
					}
				}

				if(gameShell->showKeysHelp()){
					FormatMessageText(gameShell->missionEditor() ? "<mission_editor_help>" : "<keys_help>", cbPopupBuffer);
					if(gameShell->missionEditor())
						strcat(cbPopupBuffer, gameShell->missionEditor()->info());
					terRenderDevice->SetFont(m_hFontPopup);
					OutText(10, 30, cbPopupBuffer, &sColor4f(1, 1, 1, 1), -1);
					terRenderDevice->SetFont(0);
				}
				else if(gameShell->missionEditor()){
					terRenderDevice->SetFont(m_hFontPopup);
					OutText(10, 30, gameShell->missionEditor()->info(), &sColor4f(1, 1, 1, 1), -1);
					terRenderDevice->SetFont(0);
				}
			}
		}
		if (!_bMenuMode) {
			if (gameShell->GameActive && !_bMenuMode && gameShell->getCountDownTime() != "") {
				terRenderDevice->SetFont(m_hFontCountDownTime);
				OutText(absoluteX(countDownTimerX), absoluteY(countDownTimerY), gameShell->getCountDownTime().c_str(), &sColor4f(sqshFontCountDownTimeColor), -1);
				terRenderDevice->SetFont(0);
			}

			if (gameShell->GameActive && !_bMenuMode) {
				terRenderDevice->SetFont(m_hFontPopup);
				OutText(absoluteX(totalTimerX), absoluteY(totalTimerY), (totalTime + gameShell->getTotalTime()).c_str(), &sColor4f(1, 1, 1, 1), -1);
				terRenderDevice->SetFont(0);
			}

			//общее кол-во базовых юнитов
			static char _cb[25];
			sprintf(_cb, units.c_str(), m_nTotalUnitsBaseCount, globalAttr().baseUnitsMax);
			terRenderDevice->SetFont(m_hFontPopup);

			Vect2f v1, v2;
			OutTextRect(0, 0 , _cb, -1, v1, v2);
			float baseUnitsX = absoluteX(nBaseUnitsDisplayX);
			float baseUnitsY = absoluteY(nBaseUnitsDisplayY);
			OutText(baseUnitsX, baseUnitsY, _cb, &sColor4f(1, 1, 1, 1), -1);
			terRenderDevice->SetFont(0);

			Vect2f va(baseUnitsX - 2, baseUnitsY - 2);
			Vect2f vb(va.x + v2.x-v1.x + 2, va.y + v2.y-v1.y + 2);

			draw_rect_empty(va, vb, sColor4f(1, 1, 1, 1));
		}

	} else if (m_pDesktop) {
		if (cutSceneAnimDirection) {
			switch (cutSceneAnimStep) {
				case 0:
					{
						if (cutSceneAnimDirection > 0) {
							forceDraw();
							controls[SQSH_HINT_ID]->draw(false);
						}
						int alpha = 255 * (1 - cutSceneAnimTimer / float(CUT_SCENE_FADEIN_TIME));
						terRenderDevice->DrawRectangle(0, 0, terRenderDevice->GetSizeX(), terRenderDevice->GetSizeY(), sColor4c(0, 0, 0, alpha));
//						terRenderDevice->SetGamma( terGraphicsGamma * cutSceneAnimTimer / float(CUT_SCENE_FADEIN_TIME) );
					}
					break;
				case 1:
					terRenderDevice->DrawRectangle(0, 0, terRenderDevice->GetSizeX(), terRenderDevice->GetSizeY(), sColor4c(0, 0, 0, 255));
					break;
				case 2:
					{
						if (cutSceneAnimDirection < 0) {
							forceDraw();
							controls[SQSH_HINT_ID]->draw(false);
						}
						int alpha = 255 * cutSceneAnimTimer / float(CUT_SCENE_FADEOUT_TIME);
						terRenderDevice->DrawRectangle(0, 0, terRenderDevice->GetSizeX(), terRenderDevice->GetSizeY(), sColor4c(0, 0, 0, alpha));
//						terRenderDevice->SetGamma( terGraphicsGamma * (1 - cutSceneAnimTimer / float(CUT_SCENE_FADEOUT_TIME)) );
					}
					break;
			}
			cutSceneAnimTimer -= frame_time.delta();

			if (cutSceneAnimTimer < 0) {
				switch (cutSceneAnimStep) {
					case 0:
//						terRenderDevice->SetGamma( 0 );
						if (cutSceneAnimDirection > 0) {
							terCamera->SetFrustumCutScene();
						} else {
							terCamera->SetFrustumGame();
						}
						cutSceneAnimTimer = CUT_SCENE_PAUSE_TIME;
						break;
					case 1:
						cutSceneAnimTimer = CUT_SCENE_FADEIN_TIME;
						break;
					case 2:
//						terRenderDevice->SetGamma( terGraphicsGamma );
						if (cutSceneAnimDirection < 0) {
							onCutSceneStop();
							_shellCursorManager.ShowCursor();
						}
						cutSceneAnimDirection = 0;
						cutSceneAnimTimer = 0;
						break;
				}
				cutSceneAnimStep++;
			}

		} else {
			((CHintWindow*)controls[SQSH_HINT_ID])->drawHint(true);
		}

	}

	_shellCursorManager.draw();
}

void CShellIconManager::forceDraw() {
	CShellWindow* pBackgrnd = controls[SQSH_BACKGRND_ID];
	if (pBackgrnd) {
		pBackgrnd->draw(false);
	}
	DrawControls(m_pDesktop);
}

void EnableSquadMutationButton(terUnitAttributeID id, int count, bool append, int enabled, bool bS, bool bO, bool bT, bool bNoMutationEnergy, bool checked, bool visible)
{
	int nButtonID = LegionID2Button(id);

	if(nButtonID > 0)
	{
		CShellLegionButton* pWnd = (CShellLegionButton*)_shellIconManager.GetWnd(nButtonID);
		if(pWnd)
		{
			pWnd->Show(visible);
			pWnd->SetTotalUnitCount(count, append);
			pWnd->Enable(enabled);
			pWnd->SetAtomEnable(bS, bO, bT);
			pWnd->SetStatusNoEnergy(bNoMutationEnergy);
			pWnd->SetCheck(checked);
		}
	}
}

float GetUnitBuildProgress(terUnitBase* p) {
	if(p->isConstructed())
		return 1.0f;

	float res = p->damageMolecula().phase();

	return res > 0 ? res : FLT_EPS;
}

float GetUnitUpgradeProgress(terBuilding* p) {
	if(p->isConstructed())
		return 1.0f;

	float res = p->upgradePhase();

	return res > 0 ? res : FLT_EPS;
}


void CShellIconManager::changeControlState(const vector<SaveControlData>& newControlStates) {
	for (int i = 0, s = newControlStates.size(); i < s; i++) {
		if (newControlStates[i].controlID == SQSH_TAB_BUILD_ID) {
			if (newControlStates[i].tabNumber == -1) {
				externalControlStates[newControlStates[i].controlID] = newControlStates[i];
			} else {
				externalBuildTabStates[newControlStates[i].tabNumber] = newControlStates[i];
			}
		} else if (newControlStates[i].controlID == SQSH_TAB_SQUAD_ID) {
			if (newControlStates[i].tabNumber == -1) {
				externalControlStates[newControlStates[i].controlID] = newControlStates[i];
			} else {
				externalSquadTabStates[newControlStates[i].tabNumber] = newControlStates[i];
			}
		} else {
			externalControlStates[newControlStates[i].controlID] = newControlStates[i];
		}
	}
}

void CShellIconManager::fillControlState(vector<SaveControlData>& controlStatesToSave) {
	controlStatesToSave.clear();
	for (int i = 0; i < SQSH_GAME_MAX; i++) {
		externalControlStates[i].controlID = (ShellControlID)i;
		controlStatesToSave.push_back(externalControlStates[i]);
		if (i == SQSH_TAB_BUILD_ID) {
			controlStatesToSave.back().tabNumber = -1;
			for (int j = 0, s = externalBuildTabStates.size(); j < s; j++) {
				externalBuildTabStates[j].controlID = SQSH_TAB_BUILD_ID;
				externalBuildTabStates[j].tabNumber = j;
				controlStatesToSave.push_back(externalBuildTabStates[j]);
			}
		} else if (i == SQSH_TAB_SQUAD_ID) {
			controlStatesToSave.back().tabNumber = -1;
			for (int j = 0, s = externalSquadTabStates.size(); j < s; j++) {
				externalSquadTabStates[j].controlID = SQSH_TAB_SQUAD_ID;
				externalSquadTabStates[j].tabNumber = j;
				controlStatesToSave.push_back(externalSquadTabStates[j]);
			}
		}
	}

}

STARFORCE_API_NEW void CancelEditWorkarea();

void CShellIconManager::updateControlsFromExternalStates() {
	for (int i = 0; i < SQSH_GAME_MAX; i++) {
		CShellWindow* wnd = GetWnd(externalControlStates[i].controlID);
		if (wnd) {
			if (externalControlStates[i].controlID == SQSH_TAB_BUILD_ID) {
			} else if (externalControlStates[i].controlID == SQSH_TAB_SQUAD_ID) {
			} else {
				if (externalControlStates[i].controlID >= SQSH_FRAME_TERRAIN_BUILD1_ID && externalControlStates[i].controlID <= SQSH_FRAME_TERRAIN_BUILD5_ID) {
					((CTerrainBuildButton*)wnd)->partDisable = externalControlStates[i].tabNumber;
				}
				if (!externalControlStates[i].visible) {
					wnd->Show(0);
					wnd->Enable(false);
				}
				if (!externalControlStates[i].enabled) {
					if (
							((_pShellDispatcher->m_nEditRegion == editRegion1) && (externalControlStates[i].controlID == SQSH_WORKAREA3_ID))
						||	((_pShellDispatcher->m_nEditRegion == editRegion2) && (externalControlStates[i].controlID == SQSH_WORKAREA2_ID))	) {

						CancelEditWorkarea();
					}
					wnd->Enable(false);
				}
				if ( externalControlStates[i].flashing ) {
					wnd->setFlashingInterval(0);
				} else if ( wnd->getFlashingInterval() == 0 ) {
					wnd->setFlashingInterval(-1);
				}
			}
		}
	}
	CUITabSheet* pSheet = (CUITabSheet*)GetWnd(SQSH_TAB_BUILD_ID);
	for (int j = 0, s = externalBuildTabStates.size(); j < s; j++) {
		if ( externalBuildTabStates[j].flashing ) {
			pSheet->setFlashingPageInterval(externalBuildTabStates[j].tabNumber, 0);
		} else if ( pSheet->getFlashingPageInterval(externalBuildTabStates[j].tabNumber) == 0 ) {
			pSheet->setFlashingPageInterval(externalBuildTabStates[j].tabNumber, -1);
		}
		pSheet->ShowPage(j, externalBuildTabStates[j].visible);
		pSheet->EnablePage(j, externalBuildTabStates[j].enabled);
//		pSheet->setFlashingPageInterval(externalBuildTabStates[j].tabNumber, externalBuildTabStates[j].flashing ? 0 : -1);
	}
	pSheet = (CUITabSheet*)GetWnd(SQSH_TAB_SQUAD_ID);
	int j;
	int s;
	for (j = 0, s = externalSquadTabStates.size(); j < s; j++) {
		if ( externalSquadTabStates[j].flashing ) {
			pSheet->setFlashingPageInterval(externalSquadTabStates[j].tabNumber, 0);
		} else if ( pSheet->getFlashingPageInterval(externalSquadTabStates[j].tabNumber) == 0 ) {
			pSheet->setFlashingPageInterval(externalSquadTabStates[j].tabNumber, -1);
		}
//		pSheet->setFlashingPageInterval(externalSquadTabStates[j].tabNumber, externalSquadTabStates[j].flashing ? 0 : -1);
	}
}

void CShellIconManager::UpdateIcons()
{
	if (ht_intf_test) {
		if(_bDebugDisplayAllIcons)
			return;

		if(!getDesktop() || _bMenuMode)
			return;

		CUITabSheet* pSheetBuild = (CUITabSheet*)GetWnd(SQSH_TAB_BUILD_ID);
		pSheetBuild->setActivePageChildrenVisible(true);

		UpdateMiniMapEvents();

		UpdateBuildingsIcons();
		UpdateGunsIcons();

	//	HT-SELECT!!!
	//	UpdateSelectionIcons();

		UpdateBrigFrameIcons();
		UpdateSquadIcons();

		UpdateMiscIcons();

		updateControlsFromExternalStates();
//		hideInaccessible();

		return;
	}

	if(!mt_interface_quant)
		return;

	if(_bDebugDisplayAllIcons)
		return;

	if(!getDesktop() || _bMenuMode)
		return;

//	if(!universe()->activePlayer())
//		return;

//	terPlayer* player = universe()->activePlayer();

//	terFrame* frame = player->frame();
//	if(!frame)
//		return;



	CUITabSheet* pSheetBuild = (CUITabSheet*)GetWnd(SQSH_TAB_BUILD_ID);
	pSheetBuild->setActivePageChildrenVisible(true);

	UpdateMiniMapEvents();

	UpdateBuildingsIcons();
	UpdateGunsIcons();

	UpdateSelectionIcons();

	UpdateBrigFrameIcons();

	UpdateSquadIcons();

	UpdateMiscIcons();

	updateControlsFromExternalStates();
//	hideInaccessible();
}

void CShellIconManager::UpdateMiniMapEvents() {
	LogicData* logicData = gameShell->getLogicUpdater().getLogicData();
	list<MiniMapEventIcon>::iterator ii = miniMapEventIcons.begin();
	float dt = frame_time.delta();
	bool hasAttack = false;
	while (ii != miniMapEventIcons.end()) {
		(*ii).timeRemain -= dt;
		if ((*ii).timeRemain <= 0) {
			ii = miniMapEventIcons.erase(ii);
		} else {
			if ((*ii).code == EVENT_UNIT_UNDER_ATTACK) {
				hasAttack = true;
			}
			ii++;
		}
	}
	list<MiniMapEvent>::iterator ei;
	FOR_EACH(logicData->miniMapEvents, ei) {
		bool attack = (*ei).code == EVENT_UNIT_UNDER_ATTACK;
		if (!attack || !hasAttack) {
			miniMapEventIcons.push_back(MiniMapEventIcon(*ei));
		}
		if (attack) {
			hasAttack = true;
			switch ((*ei).btnID) {
				case -1:
					break;
				case SQSH_TAB_BUILD_ID:
				case SQSH_TAB_SQUAD_ID:
					{
						CUITabSheet* pSheet = (CUITabSheet*)GetWnd((*ei).btnID);
						if ( pSheet->getFlashingPageInterval((*ei).tabNumber) < 0 ) {
							pSheet->setFlashingPageInterval((*ei).tabNumber, ATTACK_FLASHING_TIME);
						}
					}
					break;
				default:
					{
						CShellWindow* wnd = GetWnd((*ei).btnID);
						if ( wnd->getFlashingInterval() < 0 ) {
							wnd->setFlashingInterval(ATTACK_FLASHING_TIME);
						}
					}
			}
		}
	}
	logicData->miniMapEvents.clear();
}

LogicData::sProgress& CShellIconManager::GetProgress(terUnitAttributeID n_struct)
{
	LogicData* logicData = gameShell->getLogicUpdater().getLogicData();
	xassert(n_struct >= 0 && n_struct < UNIT_ATTRIBUTE_STRUCTURE_MAX);
	return logicData->progress[n_struct];
}

void CShellIconManager::UpdateMiscIcons() {
	LogicData* logicData = gameShell->getLogicUpdater().getLogicData();

	GetWnd(SQSH_SOLDIER_ID)->Enable(logicData->soldPlantEnable);
	GetWnd(SQSH_OFFICER_ID)->Enable(logicData->offPlantEnable);
	GetWnd(SQSH_TECHNIC_ID)->Enable(logicData->techPlantEnable);

	((CReplayPlayerPushButton*)GetWnd(SQSH_REPLAY_PLAYER_BUTTON_ID))->setPlayer(logicData->activePlayerName, logicData->activePlayerColor);

	//кнопка поля
	CShellComplexPushButton* fieldOnBtn = ((CShellComplexPushButton*)GetWnd(SQSH_FIELD_ON_ID));
	fieldOnBtn->Show(1);
	fieldOnBtn->SetCheck(logicData->fieldOnBtn.checked);
	fieldOnBtn->Enable(logicData->fieldOnBtn.enabled);
	CShellComplexPushButton* fieldOffBtn = ((CShellComplexPushButton*)GetWnd(SQSH_FIELD_OFF_ID));
	fieldOffBtn->Enable(logicData->fieldOffBtn.enabled);
	fieldOffBtn->Show(1);

	CShellComplexPushButton* pBtn;

	//тулзеры
//	GetWnd(SQSH_WORKAREA4_ID)->Enable(isControlAccessible(SQSH_WORKAREA4_ID));
	GetWnd(SQSH_WORKAREA4_ID)->Enable(true);

	pBtn = (CShellComplexPushButton*)GetWnd(SQSH_WORKAREA3_ID);
//	pBtn->MarkWindow(logicData->zeroToolzBtn.marked);
	pBtn->Enable(true);

	pBtn = (CShellComplexPushButton*)GetWnd(SQSH_WORKAREA2_ID);
//	pBtn->MarkWindow(logicData->abyssToolzBtn.marked);
	pBtn->Enable(true);


	((CShellComplexPushButton*)_shellIconManager.GetWnd(SQSH_SPEED_PAUSE))->SetCheck(0);
	((CShellComplexPushButton*)_shellIconManager.GetWnd(SQSH_SPEED_50))->SetCheck(0);
	((CShellComplexPushButton*)_shellIconManager.GetWnd(SQSH_SPEED_100))->SetCheck(0);
	((CShellComplexPushButton*)_shellIconManager.GetWnd(SQSH_SPEED_150))->SetCheck(0);
	float speed = gameShell->getSpeed();
	if (speed == 1.0f) {
		((CShellComplexPushButton*)_shellIconManager.GetWnd(SQSH_SPEED_100))->SetCheck(true);
	} else if (speed == 0.0f) {
		((CShellComplexPushButton*)_shellIconManager.GetWnd(SQSH_SPEED_PAUSE))->SetCheck(true);
	} else if (speed == 2.0f) {
		((CShellComplexPushButton*)_shellIconManager.GetWnd(SQSH_SPEED_150))->SetCheck(true);
	} else if (speed == 0.5f) {
		((CShellComplexPushButton*)_shellIconManager.GetWnd(SQSH_SPEED_50))->SetCheck(true);
	}

	//кнопки фрейма
	pBtn = (CShellComplexPushButton*)GetWnd(SQSH_SELPANEL_FRAME_INSTALL_ID);
	if (pBtn) {
		pBtn->SetCheck(logicData->frameInstallBtn.checked);
		pBtn->Enable(logicData->frameInstallBtn.enabled);
	}
	pBtn = (CShellComplexPushButton*)GetWnd(SQSH_SELPANEL_FRAME_TELEPORTATE_ID);
	if (pBtn) {
		if (logicData->spiralLevel < 1.0f) {
			pBtn->Show(false);
		}
		pBtn->SetCheck(logicData->frameTeleportBtn.checked);
		pBtn->Enable(logicData->frameTeleportBtn.enabled);
	}

	((CProgressEnergy*) GetWnd(SQSH_PROGRESS_ENERGY))->setEnergyData(logicData->energyProduced, logicData->energyRequested);
	((CProgressCollected*) GetWnd(SQSH_PROGRESS_COLLECTED))->setEnergyData(logicData->energyProduced, logicData->energyRequested, logicData->energyCapacity, logicData->energyAccumulated);

	for (int i = 0; i < UNIT_ATTRIBUTE_STRUCTURE_MAX; i++) {
		int iButton = Structure2ButtonID(i);
		if(iButton > 0) {
			updateButtonLabel(iButton, 0, logicData->progress[i].progress, logicData->progress[i].hold, true);
		}
	}

//	HT-SELECT!!!
	if (mt_interface_quant) {
		CSELECT_AUTOLOCK();
		const UnitList& sList=universe()->select.GetSelectList();
		UnitList::const_iterator ui;

		terPlayer* player = universe()->activePlayer();
		terFrame* frame = player->frame();

		bool isSpiral = frame && frame->selected();
		float level = 0;
		if (isSpiral) {
			level = frame->spiralLevel();
		} else if (sList.size()) {
			float chargeLevel = 0;
			FOR_EACH (sList, ui) {
				chargeLevel += (*ui)->chargeLevel();
			}
			level = chargeLevel / float(sList.size());
		}
		((CProgressUnitCharge*) GetWnd(SQSH_SELPANEL_UNIT_CHARGE_ID))->setParams(level, isSpiral);
	}
}

void CShellIconManager::UpdateBuildingsIcons() {
	LogicData* logicData = gameShell->getLogicUpdater().getLogicData();

	CUITabSheet* pSheetBuild = (CUITabSheet*)GetWnd(SQSH_TAB_BUILD_ID);

    for (int i = UNIT_ATTRIBUTE_CORE; i <= UNIT_ATTRIBUTE_HARKBACK_STATION3; i++) {
		int iButton = Structure2ButtonID(i);
		if (iButton > 0) {
			ComplexButtonData* button = &(logicData->buildings[i - UNIT_ATTRIBUTE_CORE]);
			CShellComplexPushButton* pBtn = (CShellComplexPushButton*)GetWnd(iButton);
//			if (!isControlAccessible(iButton)) {
//				pBtn->Show(false);
//			} else if (i >= UNIT_ATTRIBUTE_LASER_STATION1 && i <= UNIT_ATTRIBUTE_HARKBACK_STATION3 && pSheetBuild && (pSheetBuild->GetActivePage() == 1)) {
//				pBtn->Show(button->visible);
//			}
			if (i >= UNIT_ATTRIBUTE_LASER_STATION1 && i <= UNIT_ATTRIBUTE_HARKBACK_STATION3 && pSheetBuild && (pSheetBuild->GetActivePage() == 1)) {
				pBtn->Show(button->visible);
			}
			pBtn->Enable(button->enabled);
		}
	}
//	pSheetBuild->EnablePage(1, logicData->hasBuilding);
}

void CShellIconManager::UpdateGunsIcons() {
	LogicData* logicData = gameShell->getLogicUpdater().getLogicData();

	CUITabSheet* pSheetBuild = (CUITabSheet*)GetWnd(SQSH_TAB_BUILD_ID);

	//стационарные орудия
//	if (pSheetBuild->getActivePageNumber() == 2) {
//		pSheetBuild->MarkPage(2, false);
//	}
//	((CShellComplexPushButton*)GetWnd(SQSH_GUN_FILTH_ID))->Show(0);
	for (int i = UNIT_ATTRIBUTE_CORRIDOR_ALPHA; i < UNIT_ATTRIBUTE_STRUCTURE_MAX; i++) {
		int iButton = Structure2ButtonID(i);
		if (iButton > 0) {
			ComplexButtonData* button = &(logicData->guns[i - UNIT_ATTRIBUTE_CORRIDOR_ALPHA]);
			CShellComplexPushButton* pBtn = (CShellComplexPushButton*)GetWnd(iButton);
			if (((i >= UNIT_ATTRIBUTE_GUN_SCUM_DISRUPTOR && i <= UNIT_ATTRIBUTE_GUN_FILTH_NAVIGATOR) || i == UNIT_ATTRIBUTE_STATIC_BOMB || i == UNIT_ATTRIBUTE_CORRIDOR_ALPHA || i == UNIT_ATTRIBUTE_CORRIDOR_OMEGA) && pSheetBuild && (pSheetBuild->GetActivePage() == 2)) {
				pBtn->Show(button->visible);
			}
			pBtn->Enable(button->enabled);
		}
	}

	//страница "стац. орудия недоступна" при неустановленном фрейме и 
	//  если недоступно ни одно орудие
//	pSheetBuild->EnablePage(2, logicData->hasGun);
}

inline void CheckBuildingPowerButton(terUnitBase* pUnit)
{
	CShellComplexPushButton* pBtnOn = (CShellComplexPushButton*)_shellIconManager.GetWnd(SQSH_SELPANEL_POWERON_ID);
	CShellComplexPushButton* pBtnOff = (CShellComplexPushButton*)_shellIconManager.GetWnd(SQSH_SELPANEL_POWEROFF_ID);
	if (pBtnOn && pBtnOff) {
		pBtnOn->Show(TRUE);
		pBtnOff->Show(TRUE);
		pBtnOn->Enable(pUnit == 0 ? TRUE : (!pUnit->isBuildingPowerOn()));
		pBtnOff->Enable(pUnit == 0 ? TRUE : (pUnit->isBuildingPowerOn()));
	}
}

void CShellIconManager::ShowActionIcons(const UnitInterfacePrm& intf_prm, const UnitInterfaceActions& actions, terUnitBase* pUnit)
{
	CShellComplexPushButton* pBtn;

#define SHOW_ICON_IF(cond, enbl, id)  if(cond){pBtn = (CShellComplexPushButton*)GetWnd(id); if(pBtn){ pBtn->Show(TRUE); pBtn->Enable(enbl);}}
#define SHOW_ICON_IF2(cond, enbl, id)  if(cond){pBtn = (CShellComplexPushButton*)GetWnd(id); if(pBtn){ pBtn->Show(TRUE); pBtn->Enable(enbl || pBtn->GetCheck());}}

//	bool bShowAttack = actions.op_attack && pUnit->isBuildingPowerOn() && pUnit->CanBuildingEnable();

	SHOW_ICON_IF(actions.op_move,   TRUE, SQSH_SELPANEL_MOVE_ID);
	SHOW_ICON_IF(actions.op_stop,   TRUE, SQSH_SELPANEL_STOP_ID);
	SHOW_ICON_IF(actions.op_stop2,  TRUE, SQSH_SELPANEL_STOP2_ID);
	SHOW_ICON_IF(actions.op_attack,	TRUE, SQSH_SELPANEL_SQ_ATTACK_ID);
	SHOW_ICON_IF(actions.op_back,   TRUE, SQSH_SELPANEL_SQ_BACK_ID);
	SHOW_ICON_IF(actions.op_offdeff,TRUE, SQSH_SELPANEL_SQ_OFDEF_ID);
	SHOW_ICON_IF(actions.op_patrol, TRUE, SQSH_SELPANEL_SQ_PATROL_ID);
	SHOW_ICON_IF(actions.op_sell,   TRUE, SQSH_SELPANEL_SELL_ID);
	SHOW_ICON_IF(actions.op_install,TRUE, SQSH_SELPANEL_FRAME_INSTALL_ID);
	SHOW_ICON_IF(actions.op_teleportate,TRUE, SQSH_SELPANEL_FRAME_TELEPORTATE_ID);
	SHOW_ICON_IF(actions.op_alarm,  TRUE, SQSH_SELPANEL_FRAME_ALARM_ID);
	SHOW_ICON_IF(actions.op_brig_back, TRUE, SQSH_SELPANEL_BRIG_BACK_ID);
	SHOW_ICON_IF(actions.op_brig_change,TRUE, SQSH_SELPANEL_BRIG_CHANGE_ID);
	SHOW_ICON_IF(actions.op_brig_build, TRUE, SQSH_SELPANEL_BRIG_BUILD_ID);
	SHOW_ICON_IF(actions.op_upgrade && pUnit->isBuildingPowerOn() && pUnit->CanBuildingEnable(), 
		safe_cast<terBuilding*>(pUnit)->canUpgrade(), intf_prm.upgrade_button_id);
	SHOW_ICON_IF(actions.op_charging, TRUE, SQSH_SELPANEL_UNIT_CHARGE_ID);

	updateButtonLabel(intf_prm.upgrading_button_id, 0, 0, false, true);
	updateButtonLabel(intf_prm.upgrade_button_id, 0, 0, false, true);

#undef SHOW_ICON_IF
#undef SHOW_ICON_IF2

	if (actions.op_power_onoff && pUnit->CanBuildingEnable()) {
		CShellComplexPushButton* pBtnOn = (CShellComplexPushButton*)_shellIconManager.GetWnd(SQSH_SELPANEL_POWERON_ID);
		if (pBtnOn) {
			pBtnOn->Show(TRUE);
			bool powerOn = false;
			CSELECT_AUTOLOCK();
			const UnitList& selList=universe()->select.GetSelectList();
			UnitList::const_iterator selIt;
			for (selIt = selList.begin(); selIt != selList.end(); selIt++) {
				if (!(*selIt)->isBuildingPowerOn()) {
					powerOn = true;
					break;
				}
			}
			pBtnOn->SetCheck(!powerOn);
		}
	}
	UpdateActionStatus(pUnit);

	if(actions.op_field)
	{
		CShellComplexPushButton* pBtnOn = (CShellComplexPushButton*)GetWnd(SQSH_SELPANEL_FIELDON_ID);
		CShellComplexPushButton* pBtnCharge = (CShellComplexPushButton*)GetWnd(SQSH_SELPANEL_START_CHARGE_ID);
		pBtnOn->Show(TRUE);
		pBtnCharge->Show(TRUE);
		bool fieldOn = false;
		bool fieldOff = false;
		bool chargeDisabled = false;
		CSELECT_AUTOLOCK();
		const UnitList& selList=universe()->select.GetSelectList();
		UnitList::const_iterator selIt;
		for (selIt = selList.begin(); selIt != selList.end(); selIt++) {
			if (safe_cast<terProtector*>(*selIt)->isFieldOn()) {
				fieldOff = true;
			} else if (safe_cast<terProtector*>(*selIt)->canStartField()) {
				fieldOn = true;
			}
			if ( !safe_cast<terProtector*>(*selIt)->enableCharge() ) {
				chargeDisabled = true;
			}
			if (fieldOn && fieldOff && chargeDisabled) {
				break;
			}
		}
		
		pBtnOn->Enable(!universe()->activePlayer()->totalDefenceMode() && (fieldOn || fieldOff));
		if (fieldOff) {
			pBtnOn->SetCheck(!fieldOn);
		} else if (fieldOn) {
			pBtnOn->SetCheck(false);
		}
//		pBtnCharge->Enable(true);
		pBtnCharge->SetCheck(!chargeDisabled);
	}

	if (actions.op_offdeff) {
		CShellComplexPushButton* pBtnOffDef = (CShellComplexPushButton*)GetWnd(SQSH_SELPANEL_SQ_OFDEF_ID);
		pBtnOffDef->Show(TRUE);
		pBtnOffDef->SetCheck(true);
		CSELECT_AUTOLOCK();
		//!!! В SelectList не обязательно сквад !!!
		const UnitList& selList = universe()->select.GetSelectList();
		UnitList::const_iterator selIt;
		for (selIt = selList.begin(); selIt != selList.end(); selIt++) {
			if (safe_cast<terUnitSquad*>(*selIt)->GetInterfaceOffensiveMode() == 0) {
				pBtnOffDef->SetCheck(false);
				break;
			}
		}
	}
}
void CShellIconManager::UpdateActionStatus(terUnitBase* pUnit)
{
	CShellComplexPushButton* pBtn;

	for(int id = SQSH_SELPANEL_MOVE_ID; id <= SQSH_SELPANEL_BRIG_BUILD_ID; id++)
	{
		pBtn = (CShellComplexPushButton*)GetWnd(id);
		if(pBtn)
			pBtn->SetCheck(false);
	}

	bool bNoStop = true;
	switch(_pShellDispatcher->m_nPickAction)
	{
	case SHELL_PICK_UNIT_TARGET:
		pBtn = (CShellComplexPushButton*)GetWnd(SQSH_SELPANEL_MOVE_ID);
		if(pBtn)
			pBtn->SetCheck(true);
		break;
	case SHELL_PICK_UNIT_ATTACK:
		pBtn = (CShellComplexPushButton*)GetWnd(SQSH_SELPANEL_SQ_ATTACK_ID);
		if(pBtn)
			pBtn->SetCheck(true);
		break;
	case SHELL_PICK_UNIT_PATROL:
		pBtn = (CShellComplexPushButton*)GetWnd(SQSH_SELPANEL_SQ_PATROL_ID);
		if(pBtn)
			pBtn->SetCheck(true);
		break;
	case SHELL_PICK_UNIT_BUILD:
		pBtn = (CShellComplexPushButton*)GetWnd(SQSH_SELPANEL_BRIG_BUILD_ID);
		if(pBtn)
			pBtn->SetCheck(true);
		bNoStop = false;
		break;

	default:
		bNoStop = false;
	}

	int nMode = pUnit->GetInterfaceLegionMode();
	
	if(nMode & INTERFACE_LEGION_MODE_STOP)
	{
		pBtn = (CShellComplexPushButton*)GetWnd(SQSH_SELPANEL_STOP_ID);
		if(pBtn && !bNoStop)
			pBtn->SetCheck(true);
	}
	if(nMode & INTERFACE_LEGION_MODE_ATTACK)
	{
		pBtn = (CShellComplexPushButton*)GetWnd(SQSH_SELPANEL_SQ_ATTACK_ID);
		if(pBtn)
			pBtn->SetCheck(true);
	}
	if(nMode & INTERFACE_LEGION_MODE_MOVE)
	{
		pBtn = (CShellComplexPushButton*)GetWnd(SQSH_SELPANEL_MOVE_ID);
		if(pBtn)
			pBtn->SetCheck(true);
	}
	if(nMode & INTERFACE_LEGION_MODE_PATROL)
	{
		pBtn = (CShellComplexPushButton*)GetWnd(SQSH_SELPANEL_SQ_PATROL_ID);
		if(pBtn)
			pBtn->SetCheck(true);
	}
	if(nMode & INTERFACE_LEGION_MODE_BACK_TO_FRAME)
	{
		pBtn = (CShellComplexPushButton*)GetWnd(SQSH_SELPANEL_BRIG_BACK_ID);
		if(pBtn)
			pBtn->SetCheck(true);

		pBtn = (CShellComplexPushButton*)GetWnd(SQSH_SELPANEL_SQ_BACK_ID);
		if(pBtn)
			pBtn->SetCheck(true);

	}
	pBtn = (CShellComplexPushButton*)GetWnd(SQSH_SELPANEL_BRIG_CHANGE_ID);
	if(pBtn && pBtn->isVisible()) {
		bool checked = nMode & INTERFACE_LEGION_MODE_EXCHANGE;
		LogicData* data = gameShell->getLogicUpdater().getLogicData();
		for(int i=0; i<5; i++)
			if (data->slots[i].isInChanging) {
				break;
			}
//		pBtn->Enable(checked || i == 5);
		pBtn->SetCheck(checked);
	}
	if(nMode & INTERFACE_LEGION_MODE_TARGET_BUILD)
	{
		pBtn = (CShellComplexPushButton*)GetWnd(SQSH_SELPANEL_BRIG_BUILD_ID);
		if(pBtn)
			pBtn->SetCheck(true);
	}
	

	//состояние offensive/defensive
//	if(pUnit->Attribute.ID == UNIT_ATTRIBUTE_SQUAD){
//		pBtn = (CShellComplexPushButton*)GetWnd(SQSH_SELPANEL_SQ_OFDEF_ID);
//		pBtn->SetCheck(safe_cast<terUnitSquad*>(pUnit)->GetInterfaceOffensiveMode() == 0);
//	}
}
void CShellIconManager::ShowUpgradeBuildProcessIcons(terUnitBase* pUnit)
{
	if(pUnit->attr().ID == UNIT_ATTRIBUTE_TERRAIN_MASTER ||
		pUnit->attr().ID == UNIT_ATTRIBUTE_BUILD_MASTER)
		return;

	if (pUnit->attr().isUpgrade || pUnit->attr().ID == UNIT_ATTRIBUTE_CORRIDOR_OMEGA)	{
		GetWnd(pUnit->attr().interfacePrm.upgrading_button_id)->Enable(FALSE);
		_shellIconManager.GetWnd(SQSH_SELPANEL_SELL_ID)->Show(pUnit->attr().ID != UNIT_ATTRIBUTE_CORRIDOR_OMEGA);
		_shellIconManager.GetWnd(pUnit->attr().interfacePrm.upgrading_button_id)->Show(TRUE);
		terBuilding* b = safe_cast<terBuilding*>(pUnit);
		updateButtonLabel(pUnit->attr().interfacePrm.upgrading_button_id, 0, GetUnitUpgradeProgress(b), b->buildingStatus() & BUILDING_STATUS_HOLD_CONSTRUCTION, true);
	} else {
		ShellControlID ids[] = { SQSH_SELPANEL_SELL_ID };
		ShowControls(TRUE, ids, sizeof(ids)/sizeof(ShellControlID));
	}
}

void CShellIconManager::ShowControls(int bShow, const ShellControlID ids[], int numIds)
{
	for(int i = 0; i < numIds; i++)
		if(CShellWindow* pWnd = _shellIconManager.GetWnd(ids[i]))
			pWnd->Show(bShow);
}

void CShellIconManager::UpdateSelectionIcons()
{
	ShellControlID ids[] = { SQSH_SELPANEL_MOVE_ID,SQSH_SELPANEL_STOP_ID,SQSH_SELPANEL_STOP2_ID,SQSH_SELPANEL_SQ_ATTACK_ID,
		SQSH_SELPANEL_SQ_BACK_ID,SQSH_SELPANEL_SQ_OFDEF_ID,SQSH_SELPANEL_SQ_PATROL_ID, 
		SQSH_SELPANEL_POWERON_ID,SQSH_SELPANEL_POWEROFF_ID,SQSH_SELPANEL_SELL_ID,SQSH_SELPANEL_UPGRADE_ID,SQSH_SELPANEL_FIELDON_ID,SQSH_SELPANEL_FIELDOFF_ID,
		SQSH_SELPANEL_FRAME_INSTALL_ID,SQSH_SELPANEL_FRAME_TELEPORTATE_ID,SQSH_SELPANEL_FRAME_ALARM_ID,SQSH_SELPANEL_BRIG_CHANGE_ID,
		SQSH_SELPANEL_BRIG_BACK_ID,SQSH_SELPANEL_BRIG_BUILD_ID,
		SQSH_SELPANEL_START_CHARGE_ID, SQSH_SELPANEL_STOP_CHARGE_ID,
		SQSH_SELPANEL_UPGRADE_LASER1_ID,SQSH_SELPANEL_UPGRADE_LASER2_ID, SQSH_SELPANEL_UPGRADE_ELECTRO1_ID,SQSH_SELPANEL_UPGRADE_ELECTRO2_ID,
		SQSH_SELPANEL_UPGRADE_BOMB1_ID,	SQSH_SELPANEL_UPGRADE_BOMB2_ID,SQSH_SELPANEL_UPGRADE_ROCKET1_ID,SQSH_SELPANEL_UPGRADE_ROCKET2_ID,
		SQSH_SELPANEL_UPGRADE_FLY_ID,SQSH_SELPANEL_UPGRADE_OMEGA_ID,SQSH_SELPANEL_UPGRADE_SUBTERRA_ID,SQSH_SELPANEL_UNIT_CHARGE_ID,
		SQSH_SELPANEL_UPGRADE_EXODUS1_ID,SQSH_SELPANEL_UPGRADE_EXODUS2_ID,
		SQSH_SELPANEL_UPGRADE_EMPIRE1_ID,SQSH_SELPANEL_UPGRADE_EMPIRE2_ID,
		SQSH_SELPANEL_UPGRADE_HARKBACK1_ID,SQSH_SELPANEL_UPGRADE_HARKBACK2_ID };

	ShowControls(FALSE, ids, sizeof(ids)/sizeof(ShellControlID));

	const UnitInterfacePrm* unit_prm = 0;
	UnitInterfaceActions    actions;

	terUnitBase*          pUnit = 0;
	const AttributeBase* attr = 0;

	actions.set();

	{
		UnitList::const_iterator i_unit;
		CSELECT_AUTOLOCK();
		const UnitList& list_units = universe()->select.GetSelectList();

		if(list_units.empty())
			return;

		FOR_EACH(list_units, i_unit)
		{
			pUnit = *i_unit;
			attr = &(pUnit->attr());

	/*		if(framePowered_)
			{
	*/
				if(pUnit->isConstructed())
					unit_prm = &attr->interfacePrm;
				else
					ShowUpgradeBuildProcessIcons(pUnit);

	/*		}
			else
			{
				if(attr->interfacePrm.actions.op_sell){
					ShellControlID ids[] = { SQSH_SELPANEL_SELL_ID };
					ShowControls(TRUE, ids, sizeof(ids)/sizeof(ShellControlID));
				}
			}
	*/

			switch(pUnit->attr().ID)
			{
			case UNIT_ATTRIBUTE_SQUAD:
				{
					terUnitSquad* sq = safe_cast<terUnitSquad*>(pUnit);
					if(sq->Empty())
					{
						attr = 0;
//						actions.clear();
						unit_prm = &interface_squad3_prm;
					}
					else
					{
						terUnitAttributeID id = sq->currentMutation();
						if(id != UNIT_ATTRIBUTE_NONE)
						{
							attr = universe()->activePlayer()->unitAttribute(id);

							if (sq->currentAttribute()->AttackClass == UNIT_CLASS_IGNORE) {
								unit_prm = &interface_squad1_prm;
							}
						}
						else {
							attr = universe()->activePlayer()->unitAttribute(UNIT_ATTRIBUTE_SOLDIER);
							if (sq->attackEnemyClass() == UNIT_CLASS_IGNORE) {
								unit_prm = &interface_squad1_prm;
							}
						}
					}
				}
				break;

			case UNIT_ATTRIBUTE_FRAME:
				if(safe_cast<terFrame*>(pUnit)->frameStatus() == terFrame::MOBILE)
					unit_prm = &interface_frame_uninstalled;

				if (pUnit != universe()->activePlayer()->frame()) {
					unit_prm = 0;
				}
				break;

			case UNIT_ATTRIBUTE_TERRAIN_MASTER:
			case UNIT_ATTRIBUTE_BUILD_MASTER:
				unit_prm = &attr->interfacePrm; //даже если фрейм не установлен
				break;

			case UNIT_ATTRIBUTE_FILTH_SPOT:
			case UNIT_ATTRIBUTE_FILTH_ANTS:
			case UNIT_ATTRIBUTE_FILTH_WASP:
				attr = 0;
				actions.clear();
				break;
			}

			if (unit_prm) {
				bool sharedAttack = actions.op_attack;
				actions &= unit_prm->actions;
				if (!sharedAttack && unit_prm->actions.op_attack && (!pUnit->isBuildingPowerOn() || !pUnit->CanBuildingEnable())) {
					actions.op_attack = 0;
				}
			}

		}
		if (list_units.size() > 1 && actions.op_upgrade == 1) {
			actions.op_upgrade = 0;
		}
		xassert(pUnit);
	}

	if(unit_prm)
		ShowActionIcons(*unit_prm, actions, pUnit);

	_pShellDispatcher->SetUnitView(attr, pUnit->isBuildingPowerOn() && pUnit->CanBuildingEnable());
}
void CShellIconManager::UpdateBrigFrameIcons()
{
//	for (int i = 0; i < 5; i++) {
//		CTerrainBuildButton* slotBtn = (CTerrainBuildButton*) GetWnd(i + SQSH_FRAME_TERRAIN_BUILD1_ID);
//		TerrainButtonData* slotData = &(gameShell->getLogicUpdater().getLogicData()->slots[i]);
//		slotBtn->setParams(slotData->unit, slotData->productionPhase, slotData->isBrig);
//		slotBtn->Enable(slotData->enabled);
//	}

//	HT-SELECT!!!
	if (mt_interface_quant) {

		//бригадиры	 
		for (int tid = SQSH_FRAME_TERRAIN_BUILD1_ID; tid <= SQSH_FRAME_TERRAIN_BUILD5_ID; tid++)	{
			CTerrainBuildButton* pBtn = (CTerrainBuildButton*) GetWnd(tid);
			TerrainButtonData* slotData = &(gameShell->getLogicUpdater().getLogicData()->slots[tid - SQSH_FRAME_TERRAIN_BUILD1_ID]);
			pBtn->setParams(slotData->unit, slotData->productionPhase, slotData->isBrig);
			pBtn->Enable(slotData->enabled);

			if (slotData->unit && pBtn) {
				bool bSel = slotData->unit->selected();

				if (bSel && !pBtn->GetCheck()) {
					slotData->unit->soundEvent(SOUND_EVENT_SELECT);
					slotData->unit->soundEvent(SOUND_VOICE_UNIT_SELECTED);
				}

				pBtn->SetCheck(bSel);
			}
			else
				pBtn->SetCheck(false);

		}
	}
}
inline bool HasStationForUnit(terUnitAttributeID id)
{
	terPlayer* pPlayer= universe()->activePlayer();
	if(pPlayer)
		return pPlayer->GetMutationElement(id).Enabled;

	return false;
}

void CShellIconManager::UpdateSquadIcons()
{
	if(_bDebugDisplayAllIcons)
		return ;

	ShellControlID ids[] = { SQSH_TAB_SQUAD_ID, SQSH_SQUAD_DISINTEGRATE_ID, SQSH_SQUAD_UNIT1,
		SQSH_SQUAD_UNIT2, SQSH_SQUAD_UNIT3,	SQSH_SQUAD_UNIT4, SQSH_SQUAD_UNIT5,SQSH_SQUAD_UNIT6,
		SQSH_SQUAD_UNIT7, SQSH_SQUAD_UNIT8,	SQSH_SQUAD_UNIT9, SQSH_SQUAD_UNIT10, SQSH_SQUAD_UNIT11,
		SQSH_SQUAD_UNIT12, SQSH_SQUAD_UNIT13, SQSH_SQUAD_UNIT14, SQSH_SQUAD_UNIT15, SQSH_SQUAD_UNIT16,
		SQSH_SQUAD_UNIT17, SQSH_SQUAD_UNIT18, SQSH_SQUAD_UNIT19, SQSH_SQUAD_UNIT20, SQSH_SQUAD_UNIT21,
		SQSH_SQUAD_UNIT22, SQSH_SQUAD_UNIT23, SQSH_SQUAD_UNIT24, SQSH_SQUAD_UNIT25,
		SQSH_SOLDIER_ID,SQSH_OFFICER_ID,SQSH_TECHNIC_ID };

	CUITabSheet* pSquadSheet = (CUITabSheet*)GetWnd(SQSH_TAB_SQUAD_ID);
/*
	if (!isControlAccessible(SQSH_TAB_SQUAD_ID)) {
		ShowControls(false, ids, sizeof(ids) / sizeof(ShellControlID));
		for (int sq = 0; sq < 5; sq++) {
			pSquadSheet->EnablePage(sq, false);
		}
		return;
	}
*/
	ShowControls(true, ids, sizeof(ids) / sizeof(ShellControlID));

	for (int id = SQSH_SQUAD_DISINTEGRATE_ID; id <= SQSH_SQUAD_UNIT25; id++) {
		CShellLegionButton* pWnd = (CShellLegionButton*)_shellIconManager.GetWnd(id);
		if (pWnd) {
//			if (isControlAccessible(id)) {
//				pWnd->SetCheck(false);
//			} else {
//				pWnd->Show(false);
//			}
			pWnd->SetCheck(false);
		}

	}

	int nActivePage = pSquadSheet->GetActivePage();

//	HT-SELECT!!!
	if (mt_interface_quant) {
		SquadPageData* page = &(gameShell->getLogicUpdater().getLogicData()->squads[nActivePage]);

		terUnitSquad* pSquadSelected = page->squad;

		if (pSquadSelected && !page->enabled && pSquadSelected->selected()) {
			universe()->select.unitToSelection(pSquadSelected, COMMAND_SELECTED_MODE_NEGATIVE, true);
		}
	}

	int enabledSQ = -1;
	bool needToSwitch = false;

	for (int sq = 0; sq < 5; sq++) {
		SquadPageData* page = &(gameShell->getLogicUpdater().getLogicData()->squads[sq]);
		
		((CProgressMutation*) GetWnd(SQSH_BAR_SQUAD1_ID + sq))->setMutationEnergy(page->mutationEnergy);
		
		if (page->enabled) {
			enabledSQ = sq;
		} else if (sq == nActivePage) {
			needToSwitch = true;
		}

		pSquadSheet->EnablePage(sq, page->enabled);
		pSquadSheet->ShowPage(sq, page->visible);
//		HT-SELECT!!!
		if (mt_interface_quant) {

			terUnitSquad* pSquadSelected = GetSquadByNumber(nActivePage);
			terUnitSquad* pSquad = page->squad;
			if(pSquad)
			{
				//переключить страничку для селекченного сквада
				if (pSquad->selected() && (!pSquadSelected || !pSquadSelected->selected())) {
					pSquadSheet->SwitchPage(sq, false);
					nActivePage = pSquadSheet->GetActivePage();
				}

	//			pSquadSheet->SelectPage(sq, pSquad->selected() && !pSquad->Empty());
				pSquadSheet->SelectPage(sq, pSquad->selected());
			}
			else
				pSquadSheet->SelectPage(sq, false);
		}

		pSquadSheet->SetPageNumber(sq, 0);

		if (page->unitCount != 0) {
			int number = LegionID2Button(page->tabIconID);
			if (number >= 0) {
				number -= SQSH_SQUAD_UNIT1;
			} else {
				number = m_squad_icon_table.size()-2;
			}
			pSquadSheet->SetSquadIcons(sq, m_squad_icon_table[number]);

			pSquadSheet->SetPageNumber(sq, page->unitCount);
		} else if (!page->empty) {
			pSquadSheet->SetSquadIcons(sq, m_squad_icon_table[m_squad_icon_table.size()-2]);
		} else {
			pSquadSheet->SetSquadIcons(sq, 0);
		}
	}

	if (needToSwitch && enabledSQ > 0) {
		pSquadSheet->SwitchPage(enabledSQ, false);
	}

	nActivePage = pSquadSheet->GetActivePage();
	SquadPageData* page = &(gameShell->getLogicUpdater().getLogicData()->squads[nActivePage]);

	for (int i = 0; i < (UNIT_ATTRIBUTE_LEGIONARY_MAX - UNIT_ATTRIBUTE_SOLDIER - MUTATION_ATOM_MAX); i++) {
		MutationButtonData* button = &(page->mutationButtons[i]);
		EnableSquadMutationButton(
			button->id,
			button->count,
			button->append,
			button->enabled,
			button->bS,
			button->bO,
			button->bT,
			button->bNoMutationEnergy,
			button->checked,
			button->visible
		);
	}

	CShellAtomButton* pBtn = (CShellAtomButton*)GetWnd(SQSH_SQUAD_DISINTEGRATE_ID);
	if (pBtn) {
		pBtn->Enable(page->baseUnitsButton.enabled);
		pBtn->SetStatusNoEnergy(page->baseUnitsButton.bNoMutationEnergy);
		pBtn->SetVal(page->baseUnitsButton.val, page->baseUnitsButton.left);
		pBtn->SetCheck(page->baseUnitsButton.checked);
	}

	CShellAtomButton* sol = (CShellAtomButton*)GetWnd(SQSH_SOLDIER_ID);
	CShellAtomButton* off = (CShellAtomButton*)GetWnd(SQSH_OFFICER_ID);
	CShellAtomButton* tech = (CShellAtomButton*)GetWnd(SQSH_TECHNIC_ID);

	sol->SetVal(page->soldierButton.val, page->soldierButton.left);
	off->SetVal(page->officerButton.val, page->officerButton.left);
	tech->SetVal(page->technicButton.val, page->technicButton.left);

	updateButtonLabel(SQSH_SOLDIER_ID, page->soldierButton.value, page->soldierButton.phase, page->soldierButton.paused, page->soldierButton.bCanBuildNow);
	updateButtonLabel(SQSH_OFFICER_ID, page->officerButton.value, page->officerButton.phase, page->officerButton.paused, page->officerButton.bCanBuildNow);
	updateButtonLabel(SQSH_TECHNIC_ID, page->technicButton.value, page->technicButton.phase, page->technicButton.paused, page->technicButton.bCanBuildNow);

	m_nTotalUnitsBaseCount = gameShell->getLogicUpdater().getLogicData()->totalUnitCount;

//	HT-SELECT!!!
	if (mt_interface_quant) {

		bool showTogetherBtn = false;
		if (	(_pShellDispatcher->GetSelectedUnitsCount() > 1) && 
				(_pShellDispatcher->GetSelectedUnit()->attr().ID == UNIT_ATTRIBUTE_SQUAD)
			) {
			terUnitBase* b = _pShellDispatcher->GetSelectedUnit();

			CUITabSheet* pSquadSheet = (CUITabSheet*)GetWnd(SQSH_TAB_SQUAD_ID);
			int nActivePage = pSquadSheet->GetActivePage();
			terUnitSquad* pSquadSelected = GetSquadByNumber(nActivePage);

			if (pSquadSelected)  {
				CSELECT_AUTOLOCK();
				const UnitList& selList=universe()->select.GetSelectList();
				UnitList::const_iterator selIt;
				for (selIt = selList.begin(); selIt != selList.end(); selIt++) {
					b = *selIt;
					terUnitSquad* sq = (terUnitSquad*)(*selIt);
					if (sq != pSquadSelected && !sq->Empty() && sq->isBase()) {
		//			if ((*selIt) != pSquadSelected && !((terUnitSquad*)(*selIt))->Empty()) {
						showTogetherBtn = true;
						break;
					}
				}
			}
		}
		GetWnd(SQSH_TOGETHER_ID)->Show(showTogetherBtn);
	}
}

void updateButtonLabel(int id, int val, float progress, bool pause, bool bCanBuild)
{
	CShellComplexPushButton* pButton = (CShellComplexPushButton*)_shellIconManager.GetWnd(id);
	if(pButton)
	{
		pButton->SetValue(val, bCanBuild);
		pButton->Pause(pause);

		if(progress >= 0)
			pButton->SetProgress(progress);
	}
}

SQUAD_ICON::SQUAD_ICON(TabIcon* a) {
	attr = a;
	init();
}

void SQUAD_ICON::init() {
	texture = terVisGeneric->CreateTexture( getImageFileName(&(attr->image)).c_str() );
	highlightTexture = terVisGeneric->CreateTexture( getImageFileName(&(attr->image_h)).c_str() );
	checkedTexture = terVisGeneric->CreateTexture( getImageFileName(&(attr->image_check)).c_str() );
	if (strlen(attr->image_disabled.texture)) {
		disabledTexture = terVisGeneric->CreateTexture( getImageFileName(&(attr->image_disabled)).c_str() );
	} else {
		disabledTexture = 0;
	}

	uv = relativeUV(attr->image._ix, attr->image._iy, texture);
	dudv = relativeUV(attr->image.ix, attr->image.iy, texture);

	highUV = relativeUV(attr->image_h._ix, attr->image_h._iy, highlightTexture);
	highDUDV = relativeUV(attr->image_h.ix, attr->image_h.iy, highlightTexture);

	checkedUV = relativeUV(attr->image_check._ix, attr->image_check._iy, checkedTexture);
	checkedDUDV = relativeUV(attr->image_check.ix, attr->image_check.iy, checkedTexture);

	if (disabledTexture) {
		disabledUV = relativeUV(attr->image_disabled._ix, attr->image_disabled._iy, disabledTexture);
		disabledDUDV = relativeUV(attr->image_disabled.ix, attr->image_disabled.iy, disabledTexture);
	} else {
		disabledUV = Vect2f(0, 0);
		disabledDUDV = Vect2f(0, 0);
	}
}
SQUAD_ICON::~SQUAD_ICON()
{
	_RELEASE(texture);
	_RELEASE(checkedTexture);
	_RELEASE(highlightTexture);
	_RELEASE(disabledTexture);
}

void CShellIconManager::BuildSquadIconTable()
{
	for(int i=0; i < interfaceAttr().tabIcons.size(); i++)
	{ 
		SQUAD_ICON* p = new SQUAD_ICON(&interfaceAttr().tabIcons[i]);
		m_squad_icon_table.push_back(p);
	}
}
void CShellIconManager::ClearSquadIconTable()
{
	int k = m_squad_icon_table.size();
	for(int i=0; i<k; i++)
		delete m_squad_icon_table[i];
	m_squad_icon_table.clear();
}

bool CtrlAction::actionPerformed() {
	CShellWindow* wnd = _shellIconManager.GetWnd(controlID);
	if (wnd) {
		return wnd->actionPerformed( action.code, action.param );
	}
	return false;
}

void HotKeyManager::keyPressed(int key) {
	float currTime = frame_time();
	for (int i = 0, s = hotKeys.size(); i < s; i++) {
		if ( checkHotKey(hotKeys[i], key, currTime) ) {
			if (actions[ hotKeys[i].actionNumber ]->actionPerformed()) {
				gameShell->updatePosition();
				break;
			}
		}
	}
	lastKey = key;
	lastKeyTime = currTime;
}

void LogicUpdater::updateIconsData() {
	if (universe()->activePlayer()) {
		updateMiniMap();
		updateMiscData();
		updateBuildingsData();
		updateGunsData();
		updateSlotsData();
		updateSquadsData();
	}
}

enum RACE {
	UNRACIAL = -1,
	EXODUS,
	EMPIRE,
	HARKBACK
};

RACE getRace(int id) {
	switch (id) {
		case UNIT_ATTRIBUTE_EXODUS_STATION1:
		case UNIT_ATTRIBUTE_EXODUS_STATION2:
		case UNIT_ATTRIBUTE_EXODUS_STATION3:
		case UNIT_ATTRIBUTE_GUN_SCUM_DISRUPTOR:
		case UNIT_ATTRIBUTE_SCUM_THROWER:
		case UNIT_ATTRIBUTE_SCUM_TWISTER:
		case UNIT_ATTRIBUTE_SCUM_HEATER:
		case UNIT_ATTRIBUTE_SCUM_SPLITTER:
			return EXODUS;
		case UNIT_ATTRIBUTE_HARKBACK_STATION1:
		case UNIT_ATTRIBUTE_HARKBACK_STATION2:
		case UNIT_ATTRIBUTE_HARKBACK_STATION3:
		case UNIT_ATTRIBUTE_GUN_FILTH_NAVIGATOR:
		case UNIT_ATTRIBUTE_FILTH_SPOT0:
		case UNIT_ATTRIBUTE_FILTH_SPOT1:
		case UNIT_ATTRIBUTE_FILTH_SPOT2:
		case UNIT_ATTRIBUTE_FILTH_SPOT3:
			return HARKBACK;
		case UNIT_ATTRIBUTE_EMPIRE_STATION1:
		case UNIT_ATTRIBUTE_EMPIRE_STATION2:
		case UNIT_ATTRIBUTE_EMPIRE_STATION3:
		case UNIT_ATTRIBUTE_GUN_BALLISTIC:
		case UNIT_ATTRIBUTE_UNSEEN:
		case UNIT_ATTRIBUTE_LEECH:
		case UNIT_ATTRIBUTE_DISINTEGRATOR:
		case UNIT_ATTRIBUTE_PIERCER:
			return EMPIRE;
		default:
			return UNRACIAL;
	}
}

RACE getPlayerRace() {
	switch (universe()->activePlayer()->belligerent()) {
		case BELLIGERENT_EXODUS0:
		case BELLIGERENT_EXODUS1:
		case BELLIGERENT_EXODUS2:
		case BELLIGERENT_EXODUS3:
		case BELLIGERENT_EXODUS4:
			return EXODUS;
		case BELLIGERENT_HARKBACKHOOD0:
		case BELLIGERENT_HARKBACKHOOD1:
			return HARKBACK;
		case BELLIGERENT_EMPIRE0:
		case BELLIGERENT_EMPIRE1:
		case BELLIGERENT_EMPIRE2:
		case BELLIGERENT_EMPIRE3:
		case BELLIGERENT_EMPIRE4:
			return EMPIRE;
		default:
			return UNRACIAL;
	}
}

bool isRaceUnitsVisible(RACE race) {
	if (race != getPlayerRace()) {
		terPlayer* player = universe()->activePlayer();
		switch (race) {
			case EXODUS:
				return player->GetEvolutionBuildingData(UNIT_ATTRIBUTE_EXODUS_STATION1).Worked;
			case HARKBACK:
				return player->GetEvolutionBuildingData(UNIT_ATTRIBUTE_HARKBACK_STATION1).Worked;
			case EMPIRE:
				return player->GetEvolutionBuildingData(UNIT_ATTRIBUTE_EMPIRE_STATION1).Worked;
		}
	}
	return true;
}

void LogicUpdater::updateGunsData() {
	MTL();
	terPlayer* player = universe()->activePlayer();

	bool exodusEnabled = isRaceUnitsVisible(EXODUS);
	bool harkbackEnabled = isRaceUnitsVisible(HARKBACK);
	bool empireEnabled = isRaceUnitsVisible(EMPIRE);

	bool bHasGun = false;
	for (int i = UNIT_ATTRIBUTE_CORRIDOR_ALPHA; i < UNIT_ATTRIBUTE_STRUCTURE_MAX; i++) {
		ComplexButtonData* button = &(logicData->guns[i - UNIT_ATTRIBUTE_CORRIDOR_ALPHA]);
		terUnitAttributeID id = (terUnitAttributeID)i;
		button->id = id;

		EnableData& evolution = player->GetEvolutionBuildingData(id);
		bool bCanShow = evolution.Enabled;
				
//		if ((i >= UNIT_ATTRIBUTE_GUN_SCUM_DISRUPTOR && i <= UNIT_ATTRIBUTE_GUN_FILTH_NAVIGATOR) || i == UNIT_ATTRIBUTE_STATIC_BOMB || i == UNIT_ATTRIBUTE_CORRIDOR_ALPHA || i == UNIT_ATTRIBUTE_CORRIDOR_OMEGA) {
		if (i >= UNIT_ATTRIBUTE_GUN_SCUM_DISRUPTOR && i <= UNIT_ATTRIBUTE_GUN_FILTH_NAVIGATOR) {
			button->visible = true;
		} else if (i == UNIT_ATTRIBUTE_STATIC_BOMB || i == UNIT_ATTRIBUTE_CORRIDOR_ALPHA || i == UNIT_ATTRIBUTE_CORRIDOR_OMEGA) {
			button->visible = !evolution.Constructed;
		}

		bool bBuildProgress = false;
		terBuildingList& lst = universe()->activePlayer()->buildingList(i);
		terBuildingList::iterator it;
		for (it = lst.begin(); it != lst.end(); it++) {
			if (!(*it)->isConstructed()) {
				bBuildProgress = true;
				break;
			}
		}

		if (bCanShow || bBuildProgress) {
			bHasGun = true;
		}
		button->enabled = bCanShow && !bBuildProgress;

		switch (getRace(id)) {
			case EXODUS:
				button->visible &= exodusEnabled;
				break;
			case HARKBACK:
				button->visible &= harkbackEnabled;
				break;
			case EMPIRE:
				button->visible &= empireEnabled;
				break;
		}

		#ifdef _DEMO_
			if (isForbidden(i)) {
				button->enabled = false;
			}
		#endif
	}
	logicData->hasGun = bHasGun;
}

void LogicUpdater::updateBuildingsData() {
	MTL();
	terPlayer* player = universe()->activePlayer();

	bool bHasBuilding = false;

	RACE race = getPlayerRace();

    for (int i = UNIT_ATTRIBUTE_CORE; i <= UNIT_ATTRIBUTE_HARKBACK_STATION3; i++) {

		ComplexButtonData* button = &(logicData->buildings[i - UNIT_ATTRIBUTE_CORE]);
		terUnitAttributeID id = (terUnitAttributeID)i;
		button->id = id;

		EnableData& evolution = player->GetEvolutionBuildingData(id);

		bool visible = true;
		if (id >= UNIT_ATTRIBUTE_LASER_STATION1 && id <= UNIT_ATTRIBUTE_HARKBACK_STATION3) {
			visible = !evolution.Constructed;

			//кнопки снанций невидимы, если станция уже есть
			button->visible = visible;
		}

		RACE buildingRace = getRace(id);
		if (buildingRace != UNRACIAL && buildingRace != race) {
			visible = false;
			button->visible = visible;
		}

		//только 5 ком.центров
		if (id == UNIT_ATTRIBUTE_COMMANDER) {
			visible = visible && player->countUnits(UNIT_ATTRIBUTE_COMMANDER) < 4;
		}

		bool enabled = evolution.Enabled;

		bool bBuildProgress = false;
		terBuildingList& lst = universe()->activePlayer()->buildingList(i);
		terBuildingList::iterator it;
		for (it = lst.begin(); it != lst.end(); it++) {
			if (!(*it)->isConstructed()) {
				bBuildProgress = true;
				break;
			}
		}

		if ((enabled && visible) || bBuildProgress) {
			bHasBuilding = true;
		}

		visible = visible && !bBuildProgress;

		button->enabled = enabled && visible;

		#ifdef _DEMO_
			if (isForbidden(i)) {
				button->enabled = false;
			}
		#endif
	}
	logicData->hasBuilding = bHasBuilding;
}

void LogicUpdater::updateMiscData() {
	MTL();
	terPlayer* player = universe()->activePlayer();
	terFrame* frame = player->frame();

	if (player) {
		logicData->activePlayerName = player->name();
		logicData->activePlayerColor = player->unitColor();
	} else {
		logicData->activePlayerName = "";
	}

	logicData->frame = frame;
	logicData->frameWayPoints.clear();
	if (logicData->frame) {
		logicData->frameWayPoints.resize(logicData->frame->getWayPoints().size());
		for (int i = 0, s = logicData->frame->getWayPoints().size(); i < s; i++) {
			logicData->frameWayPoints[i] = logicData->frame->getWayPoints()[i];
		}
	}

	if (frame) {
		//plants
		logicData->soldPlantEnable = player->countBuildingsPowered(UNIT_ATTRIBUTE_SOLDIER_PLANT);
		logicData->offPlantEnable = player->countBuildingsPowered(UNIT_ATTRIBUTE_OFFICER_PLANT);
		logicData->techPlantEnable = player->countBuildingsPowered(UNIT_ATTRIBUTE_TECHNIC_PLANT);

		//кнопка поля
		logicData->fieldOnBtn.checked = player->totalDefenceMode();
		logicData->fieldOnBtn.enabled = player->isFieldOn() || player->canStartField();
		logicData->fieldOffBtn.enabled = player->totalDefenceMode() || player->isFieldOn();
	} else {
		//plants
		logicData->soldPlantEnable = false;
		logicData->offPlantEnable = false;
		logicData->techPlantEnable = false;

		//кнопка поля
		logicData->fieldOnBtn.checked = false;
		logicData->fieldOnBtn.enabled = false;
		logicData->fieldOffBtn.enabled = false;
	}

	//toolz
	const terFrameStatisticsType& t = player->GetFrameStats();
	logicData->zeroToolzBtn.marked = t.ZeroSquareRequest == t.ZeroSquareComplete;
	logicData->abyssToolzBtn.marked = t.AbyssSquareRequest == t.AbyssSquareComplete;

	logicData->speed = gameShell->getSpeed();

	if (frame) {
		logicData->spiralLevel = frame->spiralLevel();
		logicData->frameTeleportBtn.enabled = frame->canTeleportate();
		logicData->frameTeleportBtn.checked = frame->underTeleportation();
		if (frame->attached()) {
			logicData->frameInstallBtn.checked = true;
			logicData->frameInstallBtn.enabled = true;
		} else {
			logicData->frameInstallBtn.checked = false;
			logicData->frameInstallBtn.enabled = frame->basementReady();
		}
		terEnergyDataType& energyData = player->energyData();
		logicData->energyProduced = energyData.produced();
		logicData->energyRequested = energyData.requested();
		logicData->energyCapacity = energyData.capacity();
		logicData->energyAccumulated = energyData.accumulated() >= 0 ? energyData.accumulated() : 0;
	} else {
		logicData->spiralLevel = 0;
		logicData->frameInstallBtn.checked = false;
		logicData->frameInstallBtn.enabled = false;
		logicData->frameTeleportBtn.checked = false;
		logicData->frameTeleportBtn.enabled = false;
		logicData->energyProduced = 0;
		logicData->energyRequested = 0;
		logicData->energyCapacity = 0;
		logicData->energyAccumulated = 0;
	}


	logicData->num_core=player->countUnits(UNIT_ATTRIBUTE_CORE);

	{//StructureBuildProgress
		LogicData::sProgress* progress=logicData->progress;
		for (int i = 0; i < UNIT_ATTRIBUTE_STRUCTURE_MAX; i++) {
			LogicData::sProgress& p=progress[i];
			p.progress=0;
			p.hold=false;
			p.unit=NULL;
			terBuildingList& lst = player->buildingList(i);
			terBuildingList::iterator it;
			FOR_EACH(lst, it)
				if(!(*it)->isConstructed()) {
					p.progress = GetUnitBuildProgress(*it);
					p.hold = (*it)->buildingStatus() & BUILDING_STATUS_HOLD_CONSTRUCTION;
					p.unit=*it;
				}
		}
	}

}

void LogicUpdater::updateSlotsData() {

	int phase[FRAME_SLOTS_MAX];
	int id[FRAME_SLOTS_MAX];
	terUnitBase* units[FRAME_SLOTS_MAX];
	terFrame* frame = universe()->activePlayer()->frame();
	if (frame) {
		frame->GetInterfaceFrameProduction(phase, id, units);
		for (int i = 0; i < FRAME_SLOTS_MAX; i++) {
			TerrainButtonData* slotData = &(logicData->slots[i]);

			slotData->unit = (terUnitReal*) units[i];
			slotData->productionPhase = phase[i] / 100.0f;
			slotData->isBrig = !units[i] || (units[i]->attr().ID == UNIT_ATTRIBUTE_BUILD_MASTER);
			slotData->enabled = (id[i] < 0) || units[i];
			slotData->isInChanging = units[i] && (units[i]->GetInterfaceLegionMode() & INTERFACE_LEGION_MODE_EXCHANGE);
			slotData->requestedAttr = id[i];

			slotData->wayPoints.clear();
			if (slotData->unit) {
				slotData->wayPoints.resize(slotData->unit->getWayPoints().size());
//				copy(slotData->unit->getWayPoints().begin(), slotData->unit->getWayPoints().end(), slotData->wayPoints.begin());
				for (int i = 0, s = slotData->unit->getWayPoints().size(); i < s; i++) {
					slotData->wayPoints[i] = slotData->unit->getWayPoints()[i];
				}
			}
		}
	} else {
		for (int i = 0; i < FRAME_SLOTS_MAX; i++) {
			TerrainButtonData* slotData = &(logicData->slots[i]);

			slotData->productionPhase = 0;
			slotData->isBrig = true;
			slotData->enabled = false;
			slotData->isInChanging = false;
			slotData->unit = 0;
			slotData->requestedAttr = -1;
			slotData->wayPoints.clear();
		}
	}
}

void LogicUpdater::updateSquadsData() {
	terPlayer* player = universe()->activePlayer();

	//squads
	logicData->totalUnitCount = 0;
	bool plantExists = (	player->countBuildingsPowered(UNIT_ATTRIBUTE_SOLDIER_PLANT)
						+	player->countBuildingsPowered(UNIT_ATTRIBUTE_OFFICER_PLANT)
						+	player->countBuildingsPowered(UNIT_ATTRIBUTE_TECHNIC_PLANT) ) > 0;

	bool exodusEnabled = isRaceUnitsVisible(EXODUS);
	bool harkbackEnabled = isRaceUnitsVisible(HARKBACK);
	bool empireEnabled = isRaceUnitsVisible(EMPIRE);

	for (int i = 0; i < 5; i++) {
		SquadPageData* page = &(logicData->squads[i]);

		for (int mut = 0; mut < (UNIT_ATTRIBUTE_LEGIONARY_MAX - UNIT_ATTRIBUTE_SOLDIER - MUTATION_ATOM_MAX); mut++) {
			page->mutationButtons[mut].checked = false;
		}

		terUnitSquad* pSquad = universe()->activePlayer()->squad(i);
		page->squad = pSquad;
		page->mutationEnergy = pSquad ? pSquad->mutationEnergy() : 0;

		page->visible =
						pSquad
					&&	pSquad->commander() && pSquad->commander()->isConstructed();


		page->enabled = page->visible && (plantExists || !pSquad->Empty());


		page->wayPoints.clear();
		page->patrolPoints.clear();
		//mutation buttons
		if (pSquad)	{
			//ways
			page->wayPoints.resize(pSquad->getWayPoints().size());
			int k;
			int s;
			for (k = 0, s = pSquad->getWayPoints().size(); k < s; k++) {
				page->wayPoints[k] = pSquad->getWayPoints()[k];
			}
			page->patrolPoints.resize(pSquad->getPatrolPoints().size());
			for (k = 0, s = pSquad->getPatrolPoints().size(); k < s; k++) {
				page->patrolPoints[k] = pSquad->getPatrolPoints()[k];
			}
			terUnitSquad::AttackPoint* ap = pSquad->getAttackPoint();
			page->hasAttackPoint = (ap != 0);
			if (ap) {
				page->attackPoint = (*ap);
			}

			if (pSquad->selected() && !page->enabled) {
				universe()->select.unitToSelection(pSquad, COMMAND_SELECTED_MODE_NEGATIVE, true);
			}

			bool bEmpty = pSquad->Empty();
			//статус кнопок производных юнитов
			bool bNoMutEnergy = pSquad->mutationEnergy() < 1.f - FLT_EPS;

			int atom_data[3] = {0, 0, 0};
			for (int j = 0; j < 3; j++) {
				atom_data[j] = pSquad->squadMutationMolecula().aliveElementCount(1 << j);
			}


			for (int i = UNIT_ATTRIBUTE_SOLDIER + MUTATION_ATOM_MAX; i < UNIT_ATTRIBUTE_LEGIONARY_MAX; i++) {
				terUnitAttributeID id = (terUnitAttributeID)i;
				DamageMolecula damage_molecula(universe()->activePlayer()->unitAttribute(id)->damageMolecula);
				int countPossible = pSquad->countPossibleUnits(id);
				int count = pSquad->countUnits(id);
				if (count > 0) {
					countPossible = countPossible - count;
				}
				MutationButtonData* button = &(page->mutationButtons[i - UNIT_ATTRIBUTE_SOLDIER - MUTATION_ATOM_MAX]);
				button->id = id;
				button->count = countPossible;
				button->append = count > 0;
//				button->visible = !bEmpty && player->GetMutationElement(id).Enabled;
				button->enabled = (!bEmpty || plantExists) && player->GetMutationElement(id).Enabled;
				button->bS = atom_data[0] >= damage_molecula[0];
				button->bO = atom_data[1] >= damage_molecula[1];
				button->bT = atom_data[2] >= damage_molecula[2];
				button->bNoMutationEnergy = bNoMutEnergy;

				button->visible = true;
				switch (getRace(id)) {
					case EXODUS:
						button->visible &= exodusEnabled;
						break;
					case HARKBACK:
						button->visible &= harkbackEnabled;
						break;
					case EMPIRE:
						button->visible &= empireEnabled;
						break;
				}
				#ifdef _DEMO_
					if (isForbidden(i)) {
						button->enabled = false;
					}
				#endif
			
			}

			int nBaseUnitCount = pSquad->squadMutationMolecula().aliveElementCount(DAMAGE_FILTER_BASE);

			page->baseUnitsButton.enabled = !bEmpty;
			page->baseUnitsButton.bNoMutationEnergy = bNoMutEnergy;
			page->baseUnitsButton.val = nBaseUnitCount;
			page->baseUnitsButton.left = true;

			page->soldierButton.val = pSquad->countUnits(terUnitAttributeID(UNIT_ATTRIBUTE_SOLDIER));
			page->soldierButton.left = false;
			page->officerButton.val = pSquad->countUnits(terUnitAttributeID(UNIT_ATTRIBUTE_OFFICER));
			page->officerButton.left = false;
			page->technicButton.val = pSquad->countUnits(terUnitAttributeID(UNIT_ATTRIBUTE_TECHNIC));
			page->technicButton.left = false;

			page->baseUnitsButton.checked = false;
			terUnitAttributeID id = pSquad->currentMutation();
			if ( id != UNIT_ATTRIBUTE_NONE && !bEmpty ) {
				page->mutationButtons[id - UNIT_ATTRIBUTE_SOLDIER - MUTATION_ATOM_MAX].checked = pSquad->countUnits(id) > 0;
			} else {
				page->baseUnitsButton.checked = !bEmpty;
			}

			page->empty = bEmpty;
			page->tabIconID = id;
			page->unitCount = (id != UNIT_ATTRIBUTE_NONE) ? pSquad->countUnits(id) : nBaseUnitCount;

			logicData->totalUnitCount += pSquad->squadMutationMolecula().elementCount(DAMAGE_FILTER_BASE);

			DamageMolecula a_req, a_progr, a_enabled, a_pause;
			pSquad->GetAtomProduction(a_req, a_progr, a_enabled);
			pSquad->GetAtomPaused(a_pause);
			page->soldierButton.value = a_req[0];
			page->officerButton.value = a_req[1];
			page->technicButton.value = a_req[2];
			page->soldierButton.phase = a_progr[0] / 100.0f;
			page->officerButton.phase = a_progr[1] / 100.0f;
			page->technicButton.phase = a_progr[2] / 100.0f;
			page->soldierButton.paused = a_pause[0];
			page->officerButton.paused = a_pause[1];
			page->technicButton.paused = a_pause[2];
			page->soldierButton.bCanBuildNow = a_enabled[0];
			page->officerButton.bCanBuildNow = a_enabled[1];
			page->technicButton.bCanBuildNow = a_enabled[2];
		} else {
			page->unitCount = 0;
			page->soldierButton.value = 0;
			page->officerButton.value = 0;
			page->technicButton.value = 0;
			page->soldierButton.phase = 0;
			page->officerButton.phase = 0;
			page->technicButton.phase = 0;
			page->soldierButton.paused = false;
			page->officerButton.paused = false;
			page->technicButton.paused = false;
			page->soldierButton.bCanBuildNow = false;
			page->officerButton.bCanBuildNow = false;
			page->technicButton.bCanBuildNow = false;

			for (int i = UNIT_ATTRIBUTE_SOLDIER + MUTATION_ATOM_MAX; i < UNIT_ATTRIBUTE_LEGIONARY_MAX; i++) {
				terUnitAttributeID id = (terUnitAttributeID)i;
				DamageMolecula damage_molecula(universe()->activePlayer()->unitAttribute(id)->damageMolecula);
				MutationButtonData* button = &(page->mutationButtons[i - UNIT_ATTRIBUTE_SOLDIER - MUTATION_ATOM_MAX]);
				button->id = id;
				button->count = 0;
				button->append = false;
//				button->visible = !bEmpty && player->GetMutationElement(id).Enabled;
				button->enabled = false;
				button->bS = false;
				button->bO = false;
				button->bT = false;
				button->bNoMutationEnergy = true;
				button->visible = false;
			}

			int nBaseUnitCount = 0;

			page->baseUnitsButton.enabled = false;
			page->baseUnitsButton.bNoMutationEnergy = true;
			page->baseUnitsButton.val = 0;
			page->baseUnitsButton.left = true;

			page->soldierButton.val = 0;
			page->soldierButton.left = false;
			page->officerButton.val = 0;
			page->officerButton.left = false;
			page->technicButton.val = 0;
			page->technicButton.left = false;

			page->baseUnitsButton.checked = false;
//			terUnitAttributeID id = pSquad->currentMutation();
//			if ( id != UNIT_ATTRIBUTE_NONE && !bEmpty ) {
//				page->mutationButtons[id - UNIT_ATTRIBUTE_SOLDIER - MUTATION_ATOM_MAX].checked = pSquad->countUnits(id) > 0;
//			} else {
//				page->baseUnitsButton.checked = !bEmpty;
//			}

			page->empty = true;
			page->tabIconID = UNIT_ATTRIBUTE_NONE;
			page->unitCount = 0;

			page->soldierButton.value = 0;
			page->officerButton.value = 0;
			page->technicButton.value = 0;
			page->soldierButton.phase = 0;
			page->officerButton.phase = 0;
			page->technicButton.phase = 0;
			page->soldierButton.paused = false;
			page->officerButton.paused = false;
			page->technicButton.paused = false;
			page->soldierButton.bCanBuildNow = 0;
			page->officerButton.bCanBuildNow = 0;
			page->technicButton.bCanBuildNow = 0;

		}
	}
}

void LogicUpdater::updateMiniMap() {
	MTL();
	if (logicData->getMiniMap()) {
		logicData->clearMiniMap();

		float w = logicData->getWidth() / vMap.H_SIZE;
		float h = logicData->getHeight() / vMap.V_SIZE;

		logicData->miniMapLabels.clear();
		logicData->miniMapSquadCount = 0;

		PlayerVect::iterator pi;
		FOR_EACH(universe()->Players, pi) {
			terPlayer* player = (*pi);
			if (player->frame()) {
				Column::const_iterator i_line;
				int y0 = 0, y = 0;
				FOR_EACH(player->energyColumn(), i_line) {
					CellLine::const_iterator i_cell;
					if((y - y0) > 8){
						FOR_EACH(*i_line,i_cell) {
							terMapClusterLine(y * h, i_cell->xl * w, i_cell->xr * w, player->unitColor());
						}
						y0 = y;
					}
					y++;
				}
			}

			//miniMapSquads
			if (logicData->miniMapSquads.size() < logicData->miniMapSquadCount + player->squadList().size()) {
				logicData->miniMapSquads.resize(logicData->miniMapSquadCount + player->squadList().size());
			}
			for (int i = 0, s = player->squadList().size(); i < s ; i++) {
				terUnitSquad* sq = player->squadList()[i];
				if (sq && !(sq->emptyForMutation()) ) {
//					&&	(sq->currentMutation() != UNIT_ATTRIBUTE_UNSEEN || universe()->activePlayer() == player) ) {

					logicData->miniMapSquads[logicData->miniMapSquadCount].pos = sq->position2D();
					logicData->miniMapSquads[logicData->miniMapSquadCount].angle = sq->angleZ();
					logicData->miniMapSquads[logicData->miniMapSquadCount].color = player->unitColor();
					logicData->miniMapSquadCount++;
				}
			}
			

			logicData->alphaPath.clear();

			float half = min(logicData->getWidth(), logicData->getHeight()) * buildingMiniMapRatio / 2;
			int smallShift = half < 0.5f ? 0 : (half - 0.5f);
			int bigShift = half;
			const UnitList& unit_list=(*pi)->units();
			UnitList::const_iterator ui;
			FOR_EACH(unit_list,ui) {
				char c = *(*ui)->label();
				if (c && c != '_' && (*ui)->activity()) {
					logicData->miniMapLabels.push_back((*ui)->position2D());
				}

				int unitClass = (*ui)->unitClass();
				sColor4f color(0, 0, 0, 0);
				Vect2f pos;
				if(unitClass & (UNIT_CLASS_STRUCTURE_GUN | UNIT_CLASS_STRUCTURE_CORE |
					 UNIT_CLASS_STRUCTURE |	UNIT_CLASS_STRUCTURE_ENVIRONMENT) 
				  )
				{
					color = player->unitColor() * MiniMapBuildingColorFactor;
					pos = (*ui)->position2D();
				} else if ( unitClass & UNIT_CLASS_FRAME ) {
					color = player->unitColor();
					pos = (*ui)->position2D();
				} else if (	(unitClass & (UNIT_CLASS_AIR_FILTH | UNIT_CLASS_GROUND_FILTH | UNIT_CLASS_UNDERGROUND_FILTH) ) 
							&& (*ui)->attr().ID != UNIT_ATTRIBUTE_FILTH_VOLCANO
							) {
					color = filthMapColor;
					pos = (*ui)->position2D();
				} else if ( unitClass & UNIT_CLASS_CORRIDOR && !(*ui)->isUnseen()) {
					color = player->unitColor();
					pos = (*ui)->position2D();
					if ((*ui)->attr().ID == UNIT_ATTRIBUTE_CORRIDOR_ALPHA) {
						safe_cast<terCorridorAlpha*>(*ui)->putPath(logicData->alphaPath);
					}
				} else {
					switch ((*ui)->attr().ClassID) {
						case UNIT_CLASS_ID_TERRAIN_MASTER:
						case UNIT_CLASS_ID_BUILD_MASTER:
							color = player->unitColor();
							pos = (*ui)->position2D();
							break;
						case UNIT_CLASS_ID_FILTH_SPOT:
							terFilthSpot* spot = safe_cast<terFilthSpot*>(*ui);
							if (spot->GetFilthParamID() == FILTH_SPOT_ID_WORM 
							  || spot->GetFilthParamID() == FILTH_SPOT_ID_VOLCANO 
							  || spot->GetFilthParamID() == FILTH_SPOT_ID_VOLCANO_SCUM_DISRUPTOR
							  ) {
								if(spot->isSwarm())
								{
									pos = spot->getFirstSwarmPos();
									color = filthMapColor;
								}
							}
							break;
					}
				}

				if (color.a != 0) {
					int x = round(pos.x * w);
					int y = round(pos.y * h);
					xassert(x < logicData->getWidth() && x >= 0.0f);
					xassert(y < logicData->getHeight() && y >= 0.0f);
					terMapClusterRect(x - smallShift, y - smallShift, x + bigShift, y + bigShift, color);
				}
			}
		}
	}
}

void LogicUpdater::terMapClusterLine(int yMap, int x_from, int x_to, const sColor4f& color) {

	xassert(x_from < logicData->getWidth() && x_from >= 0.0f);
	xassert(x_to < logicData->getWidth() && x_to >= 0.0f);
	xassert(yMap < logicData->getHeight() && yMap >= 0.0f);

	sColor4c* pixel = logicData->getMiniMap() + int(logicData->getWidth() * yMap) + x_from;
	sColor4c out_color=color * MiniMapZeroLayerColorFactor;
	for (int i = x_from; i < x_to;  i++, pixel++) {
		*pixel = out_color;
	}
}

void LogicUpdater::terMapClusterPoint(int yMap, int xMap, const sColor4c& color) {

	xassert(xMap < logicData->getWidth() && xMap >= 0.0f);
	xassert(yMap < logicData->getHeight() && yMap >= 0.0f);

	sColor4c* pixel = logicData->getMiniMap() + int(logicData->getWidth() * yMap) + xMap;
	*pixel = color;
}

void LogicUpdater::terMapClusterRect(int left, int top, int right, int bottom, const sColor4c& color) {

	if (left < 0) {
		left = 0;
	}
	if (top < 0) {
		top = 0;
	}
	if (right >= logicData->getWidth()) {
		right = logicData->getWidth() - 1;
	}
	if (bottom >= logicData->getHeight()) {
		bottom = logicData->getHeight() - 1;
	}


	sColor4c* pixel = logicData->getMiniMap() + int(logicData->getWidth() * top) + left;
	int w = logicData->getWidth();
	for (int y = top; y <= bottom; y++, pixel += w) {
		sColor4c* line = pixel;
		for (int x = left; x <= right; x++, line++) {
			*line = color;
		}
	}

}

void CShellIconManager::gameTypeChanged(UserSingleProfile::GameType newType) {
	xassert(GetWnd(SQSH_TASK_BUTTON_ID));
	bool lan = (newType == UserSingleProfile::LAN);
	GetWnd(SQSH_TASK_BUTTON_ID)->Enable(!lan);
	GetWnd(SQSH_SPEED_PAUSE)->Enable(!lan);
	GetWnd(SQSH_SPEED_50)->Enable(!lan);
	GetWnd(SQSH_SPEED_100)->Enable(!lan);
	GetWnd(SQSH_SPEED_150)->Enable(!lan);
}

bool CShellIconManager::menuVisible() {
	for (int i = (SQSH_GAME_MAX + 1); i < SQSH_MAX; i++) {
		if ( controls[i] && controls[i]->isVisible() ) {
			return true;
		}
	}
	return false;
}

int CShellIconManager::getVisibleMenuScr() {
	for (int i = (SQSH_GAME_MAX + 1); i <= SQSH_MM_SCREEN_SOUND; i++) {
		if ( controls[i] && controls[i]->isVisible() ) {
			return i;
		}
	}
	return -1;
}

void CShellIconManager::hideAllVisibleMenuScr() {
	for (int i = (SQSH_GAME_MAX + 1); i <= SQSH_MM_SCREEN_SOUND; i++) {
		if ( controls[i] && controls[i]->isVisible() ) {
			controls[i]->Show(0);
		}
	}
	_shellIconManager.SetModalWnd(0);
}
