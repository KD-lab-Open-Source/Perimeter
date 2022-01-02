#include "StdAfx.h"

#include "Umath.h"
#include "IVisGeneric.h"
#include "VisGenericDefine.h"
#include "IRenderDevice.h"

#include "PrmEdit.h"
#include "Runtime.h"

#include "terra.h"

#include "CameraManager.h"

#include "GameShell.h"

#include "Config.h"

#include "PerimeterSound.h"
#include "Controls.h"

#include "MusicManager.h"

#include "PerimeterShellUI.h"

#include "../PluginMAX/ZIPStream.h"

#include "Universe.h"
#include "../resource.h"
#include "files/files.h"
#include "Localization.h"

#include <SDL.h>
#include <SDL_image.h>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#include <windows.h>
#define execv _execv
//Needed for extracting HWND from SDL_Window, in Linux it gives conflict due to XErrorHandler
#include <SDL_syswm.h>
#endif

//#define WINDOW_FULLSCREEN_FLAG SDL_WINDOW_FULLSCREEN
#define WINDOW_FULLSCREEN_FLAG SDL_WINDOW_FULLSCREEN_DESKTOP

#ifndef PERIMETER_EXODUS
#include <commdlg.h>
#endif
#include "../HT/ht.h"
#include "GraphicsOptions.h"
#include "GameContent.h"

const char* currentVersion = 
"Version "
#include "../version.h"
#ifdef _FINAL_VERSION_
" Final"
#else
" Debug"
#endif
#ifdef PERIMETER_EXODUS
" Exodus"
#endif
#ifdef PERIMETER_DEBUG
" (DBG)"
#endif
" (" __DATE__ " " __TIME__ ")"
;

const char* currentShortVersion =
#include "../version.h"
;

static int terMissionEdit = 0;

cLogicGeneric* terLogicGeneric = NULL;
cVisGeneric* terVisGeneric = NULL;
cInterfaceRenderDevice* terRenderDevice = NULL;

cScene* terScene = NULL;
cUnkLight* terLight = NULL;
cTileMap* terMapPoint = NULL;

int terBitPerPixel = 16;
int terScreenRefresh = 0;
int terScreenIndex = 0;
int terFullScreen = 0;
int terResizableWindow = 1;
int terScreenSizeX = 800;
int terScreenSizeY = 600;
float terGraphicsGamma = 1;
int terGrabInput = 1;
int applicationRunBackground = 1;

int terDrawMeshShadow = 2;
int terShadowType	= 0;
float terFarDistanceLOD = 0.003f;
float terNearDistanceLOD = 0.1f;
int terMipMapBlur = 0;
int terMipMapLevel = 3;

float terMapLevelLOD = 0.5f;

GameShell* gameShell = 0;

Vect2i lastWindowSize(0,0);
//Countdown for delaying the window resize propagation
int lastWindowResize = 0;

static bool applicationHasFocus_ = true;
bool applicationRestartFlag = false;
std::vector<std::string> applicationRestartArgs;

SDL_Window* sdlWindow = nullptr;
HWND hWndVisGeneric = nullptr;

extern char _bMenuMode;

SyncroTimer global_time;
SyncroTimer frame_time;
SyncroTimer scale_time;

void PerimeterDataChannelLoad();
void PerimeterDataChannelSave();

bool applicationHasFocus()
{
	return applicationHasFocus_;
}

bool applicationIsGo()
{
	return applicationHasFocus() || (gameShell && gameShell->alwaysRun());
}

void SetShadowType(int shadow_map,int shadow_size,bool update)
{
	static int old_shadow_control=-1;
	// 0 - planar map
	// 1- shadow map
	// 2 - self shadow map;
	if(!terRenderDevice->IsEnableSelfShadow() && shadow_map==2)
		shadow_map=1;
	if(shadow_size==0)
	{
		if(shadow_map!=0)
			update=true;
		shadow_map=0;
	}

	switch(shadow_map)
	{
	case 1:
		terVisGeneric->SetShadowType(SHADOW_MAP,shadow_size);
		break;
	case 2:
		terVisGeneric->SetShadowType(SHADOW_MAP_SELF,shadow_size);
		break;
	default://planar map
		terVisGeneric->SetShadowType(SHADOW_NONE,shadow_size);
		break;
	}

	bool shadow_control=shadow_map!=2;
	vMap.ShadowControl(shadow_control);
	if(old_shadow_control>=0)
	{
		update=shadow_control!=(bool)old_shadow_control;
	}
	old_shadow_control=shadow_control;

	if(update)
	{
		vMap.regRender(0,0,vMap.H_SIZE-1,vMap.V_SIZE-1);
	}
}

//------------------------------

void RestoreGDI()
{
	if(terRenderDevice && terFullScreen)
	{
        SDL_ShowWindow(sdlWindow);
	}

}

void request_application_restart(std::vector<std::string>* args) {
    applicationRestartFlag = true;
    if (args) {
        applicationRestartArgs = *args;
    } else {
        applicationRestartArgs.clear();
    }
}

void InternalErrorHandler()
{
    if (sdlWindow && terGrabInput) {
        SDL_SetWindowGrab(sdlWindow, SDL_FALSE);
    }
#ifndef _FINAL_VERSION_
    RestoreGDI();
	if(universe()) universe()->allSavePlayReel();
#endif
}

void refresh_window_size(bool update_resolution) {
    Vect2i size;
    if (terFullScreen) {
        size.x = terScreenSizeX;
        size.y = terScreenSizeY;
    } else {
        SDL_GetWindowSize(sdlWindow, &size.x, &size.y);
    }
    if (!update_resolution || size != lastWindowSize) {
        lastWindowSize = size;
        if(gameShell) {
            gameShell->setWindowClientSize(size);
            if (update_resolution) {
                terScreenSizeX = size.x;
                terScreenSizeY = size.y;
                gameShell->updateResolution(false, true, false);
            }
        }
        _shellCursorManager.OnWMSetCursor();
    }
}

void CrashHandler()
{
    if (gameShell && universe()) {
        std::string crash = get_content_root_path() + CRASH_DIR + PATH_SEP + std::to_string(time(nullptr)) + "_";

        //Attempt to save reel
        terHyperSpace::SAVE_REPLAY_RESULT statereel = universe()->savePlayReel((crash + "reel").c_str());
        fprintf(stderr, "Crash reel result: %d\n", statereel);

        //Attempt to save state
        bool statesave = gameShell->universalSave((crash + "save").c_str(), true);
        fprintf(stderr, "Crash save result: %d\n", statesave);
    }
}

void initInterfaceAttributes();

void HTManager::init()
{
	interpolation_timer_ = 0;
	interpolation_factor_ = 0;

    ErrH.SetPrefix(currentVersion);
    ErrH.SetRestore(InternalErrorHandler);
    ErrH.SetCrash(CrashHandler);
	SetAssertRestoreGraphicsFunction(RestoreGDI);
	
	//xt_get_cpuid();
	//xt_getMMXstatus();
	initclock();

	allocation_tracking("before");

	if (IniManager("Perimeter.ini").getInt("Game","ZIP")) {
        ZIPOpen("resource.pak");
    }

	PerimeterDataChannelLoad();

	terMissionEdit = IniManager("Perimeter.ini").getInt("Game","MissionEdit");
	check_command_line_parameter("edit", terMissionEdit);

	GameShell::preLoad();

	initGraphics();

    initInterfaceAttributes();
    initAttributes();

	allocation_tracking("PerimeterGraphicsInit");
	
	InitSound(IniManager("Perimeter.ini").getInt("Sound","SoundEnable"), IniManager("Perimeter.ini").getInt("Sound","MusicEnable"));

	gameShell = new GameShell(terMissionEdit);
    refresh_window_size(false);

	allocation_tracking("PerimeterLogicInit");

	start_timer=false;
};

void HTManager::done()
{
	if(!terMissionEdit)
		PerimeterDataChannelSave();
	
	// Logic
	delete gameShell;
	gameShell = 0;

	FinitSound();
	finitGraphics();

	ZIPClose();
}

bool HTManager::LogicQuant()
{
	{
		MTAuto lock(&lock_logic);
		if(universe())
		{
			terVisGeneric->SetLogicQuant(universe()->quantCounter()+2);
		}

		{
			start_timer_auto(profiler_quant, STATISTICS_GROUP_OVERALL);
			profiler_quant();
		}

		SoundQuant();
	}

	ClearDeleteUnit(false);

	bool b;
	{
		MTAuto lock(&lock_logic);
		b=gameShell->LogicQuant();
	}

	if(b)
	{
		MTAuto lock(&lock_fps);
		logic_fps.quant();
	}

	return b;
}

void HTManager::GraphQuant()
{
	if(universe())
	{
		int quant_counter=universe()->quantCounter();
		if(terVisGeneric->GetGraphLogicQuant()!=quant_counter)
		{
			interpolation_timer_ = 0; 
			interpolation_factor_ = 0; 
		}

		terVisGeneric->SetGraphLogicQuant(quant_counter);
		interpolation_timer_ += scale_time.delta();
		interpolation_factor_ = clamp(interpolation_timer_*terLogicTimePeriodInv, 0, 1);
	}

	gb_VisGeneric->SetInterpolationFactor(interpolation_factor_);
	stream_interpolator.ProcessData();
	gameShell->GraphQuant();
}

//--------------------------------
void ErrorInitialize3D();

///Returns true if its true fullscreen (no borderless window)
bool isTrueFullscreen() {
    return terFullScreen && WINDOW_FULLSCREEN_FLAG == SDL_WINDOW_FULLSCREEN;
}

void PerimeterSetupDisplayMode() {
#if PERIMETER_DEBUG
    printf("PerimeterSetupDisplayMode\n");
#endif
    SDL_SetWindowGrab(sdlWindow, SDL_FALSE);
    bool windowFullscreen = SDL_GetWindowFlags(sdlWindow)&WINDOW_FULLSCREEN_FLAG;
    
    //Create display mode with current settings
    SDL_DisplayMode mode;
    mode.w = terScreenSizeX;
    mode.h = terScreenSizeY;
    mode.format = 0;
    mode.refresh_rate = terScreenRefresh;
    mode.driverdata = nullptr;

    //Get display mode if no display was set previously
    int windowScreenIndex = SDL_GetWindowDisplayIndex(sdlWindow);
    if (windowScreenIndex < 0) {
        SDL_PRINT_ERROR("SDL_GetWindowDisplayIndex");
    }
    if (terScreenIndex < 0) {
        terScreenIndex = windowScreenIndex;
        if (SDL_GetDesktopDisplayMode(terScreenIndex, &mode) != 0) {
            SDL_PRINT_ERROR("SDL_GetDesktopDisplayMode");
            terScreenIndex = -1;
        } else if (!terFullScreen) {
            //Preserve window size if we are retrieving screen mode
            mode.w = terScreenSizeX;
            mode.h = terScreenSizeY;
        }
    }

    //Place window in selected display
    if (0 <= terScreenIndex && terScreenIndex != windowScreenIndex) {
#if PERIMETER_DEBUG
        printf("SDL_SetWindowPosition %d -> %d\n", windowScreenIndex, terScreenIndex);
#endif
        unsigned int windowPos = SDL_WINDOWPOS_CENTERED_DISPLAY(terScreenIndex);
        SDL_SetWindowPosition(sdlWindow, windowPos, windowPos);
    }

    //Set current fullscreen state
    if (windowFullscreen != terFullScreen) {
#if PERIMETER_DEBUG
        printf("SDL_SetWindowFullscreen %d\n", terFullScreen);
#endif
        SDL_SetWindowFullscreen(sdlWindow, terFullScreen ? WINDOW_FULLSCREEN_FLAG : 0);
    }
    
    //Set the display mode only on true fullscreen
    if (0 <= terScreenIndex) {
        if (terFullScreen) {
            SDL_DisplayMode closest;
            if (SDL_GetClosestDisplayMode(terScreenIndex, &mode, &closest) == nullptr) {
                SDL_PRINT_ERROR("SDL_GetClosestDisplayMode");
            } else {
                if (SDL_SetWindowDisplayMode(sdlWindow, &closest)) {
                    SDL_PRINT_ERROR("SDL_SetWindowDisplayMode");
                } else {
#if PERIMETER_DEBUG
                    printf("SDL_SetWindowDisplayMode\n");
#endif
                    //Get refresh rate
                    if (SDL_GetCurrentDisplayMode(terScreenIndex, &mode) != 0) {
                        SDL_PRINT_ERROR("SDL_GetCurrentDisplayMode");
                        terScreenRefresh = 0;
                    } else {
                        terScreenRefresh = mode.refresh_rate;
                    }
                }
            }
        }

        //bool trueFullScreen = WINDOW_FULLSCREEN_FLAG == SDL_WINDOW_FULLSCREEN;
        if (!terFullScreen) {
            terScreenRefresh = mode.refresh_rate;
            //Check if we need to restore before resizing, otherwise the set size does nothing
            if (SDL_GetWindowFlags(sdlWindow)&(SDL_WINDOW_MAXIMIZED|SDL_WINDOW_MINIMIZED)) {
                SDL_RestoreWindow(sdlWindow);
            }
            SDL_SetWindowSize(sdlWindow, mode.w, mode.h);
#if PERIMETER_DEBUG
            printf("SDL_SetWindowSize\n");
#endif
            
            //Grab window
            if (terGrabInput) {
                SDL_SetWindowGrab(sdlWindow, SDL_TRUE);
            }
        }
    }

    //Update globals with our window size
    SDL_GetWindowSize(sdlWindow, &terScreenSizeX, &terScreenSizeY);
}

void PerimeterCreateWindow() {
    Uint32 window_flags = SDL_WINDOW_HIDDEN;
#ifndef _WIN32
    //On non Windows we use dxvk-native which uses Vulkan
    window_flags |= SDL_WINDOW_VULKAN;
#endif
    if (terFullScreen) {
        window_flags |= WINDOW_FULLSCREEN_FLAG;
    }
    if (terResizableWindow) {
        window_flags |= SDL_WINDOW_RESIZABLE;
    }
    unsigned int windowPos = SDL_WINDOWPOS_UNDEFINED;
    if (0 <= terScreenIndex) {
        if (terFullScreen) {
            windowPos = SDL_WINDOWPOS_CENTERED_DISPLAY(terScreenIndex);
        } else {
            windowPos = SDL_WINDOWPOS_UNDEFINED_DISPLAY(terScreenIndex);            
        }
    }
    sdlWindow = SDL_CreateWindow(
            "Perimeter",
            windowPos, windowPos,
            terScreenSizeX, terScreenSizeY,
            window_flags
    );
    if (!sdlWindow) {
        ErrH.Abort("Error creating SDL window", XERR_CRITICAL, window_flags, SDL_GetError());
    }

    //Set minimum resolution
    if (terResizableWindow) {
        const UIResolution& minRes = getMinimumUIResolution();
        SDL_SetWindowMinimumSize(sdlWindow, minRes.x, minRes.y);
    }

    //Setup window icon
    std::string icon_path;
    const char* icon_path_str = check_command_line("icon");
    if (icon_path_str) icon_path = icon_path_str;
    if (icon_path.empty()) {
        icon_path = convert_path_content("resource/icons/icon.png");
    }
    if (icon_path.empty()) {
        const char* icon_path_ptr = GET_PREF_PATH();
        if (icon_path_ptr) {
            icon_path += "icon.png";
        }
    }
    if (!icon_path.empty()) {
        if (std::filesystem::exists(icon_path)) {
            SDL_Surface* icon = IMG_Load(icon_path.c_str());
            if (icon) {
                SDL_SetWindowIcon(sdlWindow, icon);
                SDL_FreeSurface(icon);
            } else {
                fprintf(stderr, "Window icon IMG_Load error: %s\n", IMG_GetError());
            }
        } else {
            printf("Window icon not found in resource/icons/icon.png or in %s\n", icon_path.c_str());
        }
    }
    
    //Show the window
    SDL_ShowWindow(sdlWindow);

    //Setup the window display mode
    PerimeterSetupDisplayMode();
    
    //Grab input
    if (terGrabInput && !terFullScreen) {
        SDL_SetWindowGrab(sdlWindow, SDL_TRUE);
    }

#ifdef _WIN32
    //Get HWND from SDL window
    SDL_SysWMinfo wm_info;
	SDL_VERSION(&wm_info.version);
	SDL_GetWindowWMInfo(sdlWindow, &wm_info);
	
    hWndVisGeneric = wm_info.info.win.window;
#else
    //dxvk-native uses HWND as SDL2 window handle, so this is allowed
    hWndVisGeneric = static_cast<HWND>(sdlWindow);
#endif
}

cInterfaceRenderDevice* SetGraph()
{
	cInterfaceRenderDevice *IRenderDevice=CreateIRenderDevice();

	int ModeRender=0;
	if (!isTrueFullscreen()) {
        ModeRender |= RENDERDEVICE_MODE_WINDOW;
    }
	if (terBitPerPixel==32) {
        ModeRender |= RENDERDEVICE_MODE_RGB32;
    } else {
        ModeRender |= RENDERDEVICE_MODE_RGB16;
    }

//	if(HTManager::instance()->IsUseHT())
		ModeRender|=RENDERDEVICE_MODE_MULTITHREAD;

    PerimeterCreateWindow();

    int error = IRenderDevice->Init(terScreenSizeX,terScreenSizeY,ModeRender,hWndVisGeneric,terScreenRefresh);
	if(error)
	{
        fprintf(stderr, "SetGraph init error: %d\n", error);
		ErrorInitialize3D();
	}

	return IRenderDevice;
}

//--------------------------------

void HTManager::initGraphics()
{
    initSourceUIResolution();
	terLogicGeneric = CreateILogicGeneric();
	terVisGeneric = CreateIVisGeneric();
	terVisGeneric->SetMapLevel(terMapLevelLOD);
    terVisGeneric->SetFontRootDirectory(getLocRootPath());

	terVisGeneric->SetEffectLibraryPath("RESOURCE\\FX","RESOURCE\\FX\\TEXTURES");

	bool occlusion=IniManager("Perimeter.ini").getInt("Graphics","EnableOcclusion");
	terVisGeneric->EnableOcclusion(occlusion);
	bool point_light=IniManager("Perimeter.ini").getInt("Graphics","EnablePointLight");
	terVisGeneric->EnablePointLight(point_light);

	terVisGeneric->SetFarDistanceLOD(terFarDistanceLOD);
	terVisGeneric->SetNearDistanceLOD(terNearDistanceLOD);
	terVisGeneric->SetMipMapBlur(terMipMapBlur);
	terVisGeneric->SetMipMapLevel(terMipMapLevel);

	terRenderDevice = SetGraph();

//	terRenderDevice = (cInterfaceRenderDevice*)(terVisGeneric -> CreateGraph(terGraphicsMode));
//	terVisGeneric -> SetGraph(terRenderDevice,terScreenSizeX,terScreenSizeY,terFullScreen,16);
//	terGraph3d = terVisGeneric -> GetIGraph3d(terRenderDevice);
	terRenderDevice->SetGamma( terGraphicsGamma );
	terRenderDevice->SetClipRect(5,5,635,475);
	terScreenSizeX = terRenderDevice->GetSizeX();
	terScreenSizeY = terRenderDevice->GetSizeY();
	terVisGeneric->SetData(terRenderDevice);

//---------------------
	SetShadowType(terShadowType,terDrawMeshShadow,false);
	terScene = terVisGeneric->CreateScene();

	terLight = terScene->CreateLight(ATTRLIGHT_DIRECTION);
	terLight->SetPosition(MatXf(Mat3f::ID,Vect3f(0,0,0)));
    sColor4f a(0,0,0,1);
    sColor4f b(1,1,1,1);
	terLight->SetColor(&a, &b);

	pDefaultFont=terVisGeneric->CreateFont("Arial",12);
	xassert(pDefaultFont);
	terRenderDevice->SetDefaultFont(pDefaultFont);
	
	//terLight->SetDirection( &Vect3f( -1, 0, -1 ) );
	Vect3f dir(-234,0,-180);
	dir.normalize();
	terLight->SetDirection(dir);

	terCamera = new terCameraType(terScene->CreateCamera());

//For AVI
/*	if(terWinVideoEnable){
		terRenderDevice->Fill(0,0,0);
		terRenderDevice->Flush();
	};*/

    setSourceUIResolution(Vect2i(terScreenSizeX, terScreenSizeY));

	vMap.prepare("RESOURCE\\Worlds");
	GraphOptionsManager::getInstance().load();
	GraphOptionsManager::getInstance().apply();
}

void HTManager::finitGraphics()
{
	gameShell->done();

	RELEASE(terLight);
	if(terCamera){
		delete terCamera;
		terCamera = NULL;
	}

	RELEASE(terScene);

	terRenderDevice->SetDefaultFont(NULL);
	terRenderDevice->SetFont(NULL);
	RELEASE(pDefaultFont);
	if(terVisGeneric)
		terVisGeneric->ClearData();
	if(terVisGeneric)
		terVisGeneric->Release();
	if(terRenderDevice)
		terRenderDevice->Release();
	if(terLogicGeneric)
		terLogicGeneric->Release();

	terRenderDevice = NULL;
	terVisGeneric = NULL;
	terLogicGeneric = NULL;
    
    if (sdlWindow) {
        SDL_ShowCursor(SDL_TRUE);
        SDL_DestroyWindow(sdlWindow);
        sdlWindow = nullptr;
        hWndVisGeneric = nullptr;
    }
}

//--------------------------------

void InitSound(bool sound, bool music, bool firstTime)
{
	terSoundEnable = sound;
	terMusicEnable = music;
    int mixChannels = 30; //Default SDL_mixer is 8, DirectSound has 31
    int chunkSizeFactor = 12; //1056 bytes under 2 channel 22khz 16 bits 

	if (firstTime) {
		IniManager ini("Perimeter.ini");
		terSoundVolume = ini.getFloat("Sound","SoundVolume");
		terMusicVolume = ini.getFloat("Sound","MusicVolume");
        IniManager ini_no("Perimeter.ini", false);
        ini_no.getInt("Sound","MixChannels", mixChannels);
        ini_no.getInt("Sound","ChunkSize", chunkSizeFactor);
	}

	if(terSoundEnable  || terMusicEnable){
		static int inited = 0;

		SNDSetLocDataDirectory(getLocDataPath().c_str());

		if(!inited && check_command_line("disable_sound") == nullptr){
			inited = 1;

			SNDSetSoundDirectory("RESOURCE\\SOUNDS\\EFF\\");

			if(terEnableSoundLog)
				SNDEnableErrorLog("sound.txt");

			if(SNDInitSound(mixChannels, chunkSizeFactor)){
				SNDScriptPrmEnableAll();
			}
		}
	
		SetVolumeMusic( terMusicVolume );
		SNDSetVolume( terSoundVolume );

		SND2DPanByX(1, fSoundWidthPower);
		snd_listener.SetZMultiple(fSoundZMultiple);
	}

	SNDEnableSound(terSoundEnable);
}

void SoundQuant()
{
	if(!terSoundEnable)
		return;

	snd_listener.SetPos(terCamera->matrix());
	snd_listener.SetVelocity(Vect3f(0,0,0));

	snd_listener.Update();

	MusicQuant();
}

void FinitSound()
{
	IniManager ini("Perimeter.ini");
	ini.putInt("Sound","SoundEnable", terSoundEnable);
	ini.putInt("Sound","MusicEnable", terMusicEnable);
	ini.putFloat("Sound","SoundVolume", terSoundVolume);
	ini.putFloat("Sound","MusicVolume", terMusicVolume);

	if(!terSoundEnable && !terMusicEnable)
		return;

	SNDReleaseSound();
}

//------------------------------
#ifndef _WIN32
int main(int argc, char *argv[]) {
    //Call SDL main init
    SDL_SetMainReady();

    return SDL_main(argc, argv);
}
#endif

int SDL_main(int argc, char *argv[])
{
    //We need to copy argc/argv so they can be accessed later via check_command_line etc
    setup_argcv(argc, argv);
    
    //Redirect stdio and print version
    ErrH.RedirectStdio();
    printf("Perimeter %s - %s\n", currentShortVersion, currentVersion);

    //Start SDL stuff
    int sdlresult = SDL_Init(SDL_INIT_TIMER | SDL_INIT_AUDIO | SDL_INIT_VIDEO | SDL_INIT_EVENTS);
    if (sdlresult < 0) {
        ErrH.Abort("Error initializing SDL", XERR_CRITICAL, sdlresult, SDL_GetError());
    }
    sdlresult = SDLNet_Init();
    if (sdlresult < 0) {
        ErrH.Abort("Error initializing SDLNet", XERR_CRITICAL, sdlresult, SDLNet_GetError());
    }

    //Do game content detection
    detectGameContent();

    //Create some folders
    for (std::string path : {
        CRASH_DIR,
        "cache",
        "cache/font",
        "cache/bump",
    }) {
        path = get_content_root_path() + convert_path_native(path);
        bool ok = create_directories(path);
        xassert(ok || std::filesystem::is_directory(path));
    }
    std::string path = convert_path_content("resource/saves/") + "Multiplayer";
    if (!std::filesystem::exists(std::filesystem::path(path))) {
        bool ok = create_directories(path);
        xassert(ok || std::filesystem::is_directory(path));
    }

    //Load perimeter parameters
    int xprmcompiler = IniManager("Perimeter.ini", false).getInt("Game", "XPrmCompiler");
    check_command_line_parameter("xprm_compiler", xprmcompiler);
    if (xprmcompiler) {
        reload_parameters();
    }

    g_controls_converter.LoadKeyNameTable();

    int ht = IniManager("Perimeter.ini").getInt("Game", "HT");
    check_command_line_parameter("HT", ht);
    HTManager* runtime_object = new HTManager(ht);
    runtime_object->setUseHT(ht ? true : false);

    xassert(!(gameShell && gameShell->alwaysRun() && terFullScreen));

    bool run = true;
    while (run) {
        app_event_poll();

        //NetworkPause handler
        static bool runapp = true;
        if (applicationIsGo() != runapp) {
            if (gameShell && (!gameShell->alwaysRun())) {
                if (gameShell->getNetClient()) {
                    if (gameShell->getNetClient()->setPause(!applicationHasFocus()))
                        runapp = applicationIsGo();
                }
            }
        }
//		if(gameShell && (!gameShell->alwaysRun()) ){
//			if(gameShell->getNetClient())
//				gameShell->getNetClient()->pauseQuant(applicationIsGo()));
//		}

        if (applicationIsGo()) {
            run = runtime_object->Quant();
        } else {
#ifndef PERIMETER_EXODUS
            //TODO is this necessary under SDL2 in Win32?
            WaitMessage();
#endif
        }
    }

    delete runtime_object;
	
    SDLNet_Quit();
	SDL_Quit();
    
    if (applicationRestartFlag) {
        //NOTE: execv is used since is the only reasonable way to reset all state as launching again
        //doesn't seem to work due to some parts of code not being properly cleanup during shutdown
        applicationRestartFlag = false;
        
        //Copy the args that launched this game
        std::vector<char*> exec_argv;
        for (int i = 0; i < __argc; ++i) {
            if (startsWith(__argv[i], "tmp_")) {
                //These are passed internally and are not supposed to pass into next instance
                continue;
            }
            
            size_t len = strlen(__argv[i]) + 1;
            char* str = static_cast<char*>(malloc(len));
            strcpy(str, __argv[i]);
            exec_argv.emplace_back(str);
        }
        
        //Add extra args
        for (auto const& arg : applicationRestartArgs) {
            size_t len = arg.length() + 1;
            char* str = static_cast<char*>(malloc(len));
            strcpy(str, arg.c_str());
            exec_argv.emplace_back(str);
        }

        //execv last arg must be null for termination
        exec_argv.emplace_back(nullptr);
        
        //launch ourselves again, execution of this process stops here
        printf("Restarting:");
        for (auto const& str : exec_argv) {
            if (str) printf(" %s", str);
        }
        printf("\n");
        int ret = execv(exec_argv[0], exec_argv.data());
        
        //We shouldn't reach this
        ErrH.Abort("Error restarting the application", XERR_USER, ret);
    }

	return 0;
}

//--------------------------------

void app_event_poll() {
    //Check if window resizing has been calm down and propagate event
    if (0 < lastWindowResize) {
        lastWindowResize--;
        if (lastWindowResize <= 0) {
            refresh_window_size(true);
        }
    }
    
    uint32_t windowID = SDL_GetWindowID(sdlWindow);

    //Iterate each SDL event that we may have queued since last poll
    SDL_Event event;
    bool closing = false;
    while (SDL_PollEvent(&event) == 1) {
        if (event.window.windowID && (!sdlWindow || event.window.windowID != windowID)) {
            //Event is for a window that is not current or window is not available
            continue;
        }

        if (gameShell) {
            gameShell->EventHandler(event);
        }

        /* TODO how this should be impl?
            case WM_SETCURSOR:
                if(applicationHasFocus()){
                    int nHittest = LOWORD(lParam);  // hit-test code 
                    if(nHittest==HTCLIENT)
                    {
                        _shellCursorManager.OnWMSetCursor();
                        return TRUE;
                    }
                }
                break;
        */

        //Handle event by type and subtype
        switch (event.type) {
            case SDL_MOUSEBUTTONDOWN: {
                //Grab window at click if window is resizable and is not already grabbed
                if (terGrabInput && !terFullScreen && applicationHasFocus_ && SDL_GetWindowGrab(sdlWindow) == SDL_FALSE) {
                    SDL_SetWindowGrab(sdlWindow, SDL_TRUE);
                }
                break;
            }
            case SDL_WINDOWEVENT: {
                switch (event.window.event) {
                    case SDL_WINDOWEVENT_SHOWN: {
                        break;
                    }
                    case SDL_WINDOWEVENT_RESIZED:
                    case SDL_WINDOWEVENT_SIZE_CHANGED: {
                        if (0 <= lastWindowResize) {
                            if (_bMenuMode) {
                                lastWindowResize = 15;
                            } else {
                                //Game can be unstable during frequent resizing
                                lastWindowResize = 30;
                            }
                        }
                        break;
                    }
                    case SDL_WINDOWEVENT_FOCUS_LOST: {
                        if (terGrabInput) {
                            SDL_SetWindowGrab(sdlWindow, SDL_FALSE);
                        }
                        if (!applicationRunBackground) {
                            applicationHasFocus_ = false;
                        }
                        break;
                    }
                    case SDL_WINDOWEVENT_FOCUS_GAINED: {
                        applicationHasFocus_ = true;
                        break;
                    }
                    case SDL_WINDOWEVENT_CLOSE: {
                        //NOTE: Seems that MacOS uses this event instead of SDL_QUIT when window is requested to close
                        closing = true;
                        break;
                    }
                    default:
                        break;
                }
                break;
            }
            case SDL_QUIT: {
                closing = true;
                break;
            }
            default: {
                break;
            }
        }
        
        if (closing) {
            if(gameShell) {
                gameShell->terminate();
            } else {
                SDL_ShowCursor(SDL_TRUE);
                SDL_DestroyWindow(sdlWindow);
                sdlWindow = nullptr;
                hWndVisGeneric = nullptr;
            }
            break;
        }
    }
}

void setLogicFp()
{
#ifdef _WIN32
	//_controlfp(_PC_24, _MCW_PC);

#ifndef _FINAL_VERSION_
	static int enable = IniManager("Perimeter.ini").getInt("Game","ControlFpEnable");
	if(enable){
		_controlfp( _controlfp(0,0) & ~(EM_OVERFLOW | EM_ZERODIVIDE | EM_DENORMAL |  EM_INVALID),  _MCW_EM ); 
		_clearfp();
	}
#endif
#endif
}

//-------------------------------------------------
bool openFileDialog(std::string& filename, const char* initialDir, const char* extention, const char* title)
{
	XBuffer filter;
	filter < title < '\0' < "*." < extention < '\0' < '\0';

#ifndef PERIMETER_EXODUS
	OPENFILENAME ofn;
	memset(&ofn,0,sizeof(ofn));
	char fname[2048];
	strcpy(fname,filename.c_str());
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hWndVisGeneric;
	std::string fullTitle = std::string("Open: ") + title;
	ofn.lpstrTitle = fullTitle.c_str();
	ofn.lpstrFilter = filter;
	ofn.lpstrFile = fname;
	ofn.nMaxFile = sizeof(fname)-1;
	ofn.lpstrInitialDir = initialDir;
	ofn.Flags = OFN_PATHMUSTEXIST|OFN_HIDEREADONLY|OFN_EXPLORER|OFN_NOCHANGEDIR;
	ofn.lpstrDefExt = extention;
	if(!GetOpenFileName(&ofn))
		return false;
	filename = fname;
	return true;
#else
	//TODO
	return false;
#endif
}

bool saveFileDialog(std::string& filename, const char* initialDir, const char* extention, const char* title)
{
	XBuffer filter;
	filter < title < '\0' < "*." < extention < '\0' < '\0';

#ifndef PERIMETER_EXODUS
	OPENFILENAME ofn;
	memset(&ofn,0,sizeof(ofn));
	char fname[2048];
	strcpy(fname,filename.c_str());
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hWndVisGeneric;
	std::string fullTitle = std::string("Save: ") + title;
	ofn.lpstrTitle = fullTitle.c_str();
	ofn.lpstrFilter = filter;
	ofn.lpstrFile = fname;
	ofn.nMaxFile = sizeof(fname)-1;
	ofn.lpstrInitialDir = initialDir;
	ofn.Flags = OFN_PATHMUSTEXIST|OFN_HIDEREADONLY|OFN_EXPLORER|OFN_NOCHANGEDIR;
	ofn.lpstrDefExt = extention;
	if(!GetSaveFileName(&ofn))
		return false;
	filename = fname;
	return true;
#else
    //TODO
    return false;
#endif
}

const char* popupMenu(std::vector<const char*> items) // returns zero if cancel
{
	if(items.empty())
		return 0;

#ifndef PERIMETER_EXODUS
	HMENU hMenu = CreatePopupMenu();
	
	std::vector<const char*>::iterator i;
	FOR_EACH(items, i)
		AppendMenu(hMenu, MF_STRING, 1 + i - items.begin(), *i);
	
	Vect2i ps = gameShell->convertToScreenAbsolute(gameShell->mousePosition());
	int index = TrackPopupMenu(hMenu, TPM_RETURNCMD | TPM_LEFTALIGN | TPM_RIGHTBUTTON, ps.x, ps.y, 0, hWndVisGeneric, 0);
	
	DestroyMenu(hMenu);
	
	if(index > 0 && index <= items.size())
		return items[index - 1];
	else
		return 0;
#else
    //TODO
    return 0;
#endif
}

int popupMenuIndex(std::vector<const char*> items) // returns -1 if cancel
{
	if(items.empty())
		return -1;

#ifndef PERIMETER_EXODUS
	HMENU hMenu = CreatePopupMenu();
	
	std::vector<const char*>::iterator i;
	FOR_EACH(items, i)
		AppendMenu(hMenu, MF_STRING, 1 + i - items.begin(), *i);
	
	Vect2i ps = gameShell->convertToScreenAbsolute(gameShell->mousePosition());
	int index = TrackPopupMenu(hMenu, TPM_RETURNCMD | TPM_LEFTALIGN | TPM_RIGHTBUTTON, ps.x, ps.y, 0, hWndVisGeneric, 0);
	
	DestroyMenu(hMenu);
	
	if(index > 0 && index <= items.size())
		return index - 1;
	else
		return -1;
#else
    //TODO
    return -1;
#endif
}



//-----------------------------------------
static std::string editTextString;
#ifndef PERIMETER_EXODUS
static INT_PTR CALLBACK DialogProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	switch(msg)
	{
	case WM_SYSCOMMAND:
		if(wParam==SC_CLOSE)
		{
			EndDialog(hwnd,IDCANCEL);
		}
		break;
	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK)
		{
			char tmpstr[256];
			HWND h;

			h=GetDlgItem(hwnd,IDC_INPUT_TEXT);
			GetWindowText(h,tmpstr,256);
			editTextString = tmpstr;

			EndDialog(hwnd,IDOK);
			break;
		}
		if (LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hwnd,IDCANCEL);
			break;
		}
		break;
	case WM_INITDIALOG:
		{
			HWND h;
			h=GetDlgItem(hwnd,IDC_INPUT_TEXT);
			SetWindowText(h, editTextString.c_str());
		}
		return TRUE;
	}
	return FALSE;
}
#endif

const char* editText(const char* defaultValue)
{
	editTextString = defaultValue;
#ifndef PERIMETER_EXODUS
	DialogBox(GetModuleHandle(0),MAKEINTRESOURCE(IDD_DIALOG_INPUT_TEXT),hWndVisGeneric,DialogProc);
#endif
	return editTextString.c_str();
}

const char* editTextMultiLine(const char* defaultValue, void* hwnd)
{
	editTextString = defaultValue;
#ifndef PERIMETER_EXODUS
	DialogBox(GetModuleHandle(0),MAKEINTRESOURCE(IDD_DIALOG_INPUT_TEXT_MULTILINE), static_cast<HWND>(hwnd), DialogProc);
#endif
	return editTextString.c_str();
}
