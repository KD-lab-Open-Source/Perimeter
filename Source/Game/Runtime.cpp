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
#include "codepages/codepages.h"

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_vulkan.h>

#ifdef _WIN32
#include <commdlg.h>
#endif

//#define WINDOW_FULLSCREEN_FLAG SDL_WINDOW_FULLSCREEN
#define WINDOW_FULLSCREEN_FLAG SDL_WINDOW_FULLSCREEN_DESKTOP

#include "../HT/ht.h"
#include "GraphicsOptions.h"
#include "GameContent.h"

const char* currentShortVersion = PERIMETER_VERSION;

const char* currentVersion = 
"Version " PERIMETER_VERSION
#ifndef _FINAL_VERSION_
" NotFinal"
#endif
#ifdef PERIMETER_DEBUG
" Debug"
#endif
;

uint16_t currentVersionNumbers[] = {0, 0, 0};

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
int terGrabInput = 0;
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
#ifdef _WIN32
HWND hWndVisGeneric = nullptr;
#endif

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
    RestoreGDI(); //TODO is this necessary anymore?
	if(universe()) universe()->allSavePlayReel();
#endif
}

void refresh_window_size(bool update_resolution) {
    Vect2i size;
    if (terFullScreen || !sdlWindow) {
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
        std::string crash = CRASH_DIR;
        terminate_with_char(crash, PATH_SEP);
        crash += std::to_string(time(nullptr)) + "_";

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

void PerimeterCreateWindow(uint32_t window_flags) {
    window_flags |= SDL_WINDOW_HIDDEN;
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
    if (window_flags & SDL_WINDOW_VULKAN) {
        int res = SDL_Vulkan_LoadLibrary(nullptr);
        if (res) {
            fprintf(stderr, "SDL_Vulkan_LoadLibrary: Error '%d', '%s'", res, SDL_GetError());
        }
    }
    if (window_flags & SDL_WINDOW_OPENGL) {
        int res = SDL_GL_LoadLibrary(nullptr);
        if (res) {
            fprintf(stderr, "SDL_GL_LoadLibrary: Error '%d', '%s'", res, SDL_GetError());
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
            SDL_free((void*) icon_path_ptr);
        }
    }
    if (!icon_path.empty()) {
        if (std::filesystem::exists(std::filesystem::u8path(icon_path))) {
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
}

cInterfaceRenderDevice* SetGraph()
{
    const char* graph_str = check_command_line("graph");
    std::string graph(graph_str ? graph_str : "");

    eRenderDeviceSelection deviceSelection;
    if (graph == "dx" || graph == "d3d" || graph == "dx9" || graph == "d3d9") {
        deviceSelection = DEVICE_D3D9;
    } else if (graph == "sokol" || graph == "gl" || graph == "gles" || graph == "metal") {
        deviceSelection = DEVICE_SOKOL;
    } else if (graph == "headless" || graph == "none" || graph == "null" || graph == "nowindow") {
        deviceSelection = DEVICE_HEADLESS;
    } else {
#ifdef PERIMETER_D3D9
        deviceSelection = DEVICE_D3D9;
#else
#ifdef PERIMETER_SOKOL
        deviceSelection = DEVICE_SOKOL;
#else
        deviceSelection = DEVICE_HEADLESS;
#endif
#endif
    }
    
	cInterfaceRenderDevice *IRenderDevice = CreateIRenderDevice(deviceSelection);

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
        ModeRender |= RENDERDEVICE_MODE_MULTITHREAD;

    if (deviceSelection != DEVICE_HEADLESS) {
        PerimeterCreateWindow(IRenderDevice->GetWindowCreationFlags());
    }

    int error = IRenderDevice->Init(terScreenSizeX,terScreenSizeY,ModeRender,sdlWindow,terScreenRefresh);
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

	pDefaultFont= terVisGeneric->CreateGameFont("Arial", 12);
	xassert(pDefaultFont);
	terRenderDevice->SetDefaultFont(pDefaultFont);
	
	//terLight->SetDirection( &Vect3f( -1, 0, -1 ) );
	Vect3f dir(-234,0,-180);
	dir.normalize();
	terLight->SetDirection(dir);

	terCamera = new terCameraType(terScene->CreateCamera());

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
	if(terVisGeneric) {
        terVisGeneric->ClearData();
    }
	if(terVisGeneric) {
        terVisGeneric->Release();
    }
	if (terRenderDevice) {
        terRenderDevice->Done();
        terRenderDevice->Release();
    }
	if(terLogicGeneric) {
        terLogicGeneric->Release();
    }

	terRenderDevice = NULL;
	terVisGeneric = NULL;
	terLogicGeneric = NULL;
    
    if (sdlWindow) {
        SDL_ShowCursor(SDL_TRUE);
        SDL_DestroyWindow(sdlWindow);
        sdlWindow = nullptr;
#ifdef _WIN32
        hWndVisGeneric = nullptr;
#endif
    }
}

//--------------------------------

void InitSound(bool sound, bool music, bool firstTime)
{
	terSoundEnable = sound;
	terMusicEnable = music;
    
    int mixChannels = 30; //Default SDL_mixer is 8, DirectSound has 31
    int chunkSizeFactor = 12; //1056 bytes under 2 channel 22khz 16 bits 

    IniManager ini("Perimeter.ini");
	if (firstTime) {
		terSoundVolume = ini.getFloat("Sound","SoundVolume");
		terMusicVolume = ini.getFloat("Sound","MusicVolume");
        IniManager ini_no("Perimeter.ini", false);
        ini_no.getInt("Sound","MixChannels", mixChannels);
        ini_no.getInt("Sound","ChunkSize", chunkSizeFactor);
	} else {
        ini.putInt("Sound","SoundEnable", terSoundEnable);
        ini.putInt("Sound","MusicEnable", terMusicEnable);
    }
    if (terRenderDevice->GetRenderSelection() == DEVICE_HEADLESS
    || check_command_line("disable_sound") != nullptr) {
        terSoundEnable = terMusicEnable = false;
    }

	if(terSoundEnable  || terMusicEnable){
		static int inited = 0;

		SNDSetLocDataDirectory(getLocDataPath().c_str());

		if(!inited){
			inited = 1;

			SNDSetSoundDirectory("RESOURCE\\SOUNDS\\EFF\\");

			if(terEnableSoundLog)
				SNDEnableErrorLog("sound.txt");

			if(SNDInitSound(mixChannels, chunkSizeFactor)){
				SNDScriptPrmEnableAll();
			} else {
                terMusicEnable = false;
                terSoundEnable = false;
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
	ini.putFloat("Sound","SoundVolume", terSoundVolume);
	ini.putFloat("Sound","MusicVolume", terMusicVolume);

	if(!terSoundEnable && !terMusicEnable)
		return;

	SNDReleaseSound();
}

//------------------------------

void decode_stacktrace() {
    const char* stackframes_str = check_command_line("stack_frames");
    if (stackframes_str) {
        uint64_t frame_offset = 0;
        const char* sdlmain_frame_str = check_command_line("stack_reference");
        if (sdlmain_frame_str) {
            frame_offset = strtoull(sdlmain_frame_str, nullptr,16);
        }
        
        //Parse each frame address
        XBuffer buf((char*) stackframes_str, strlen(stackframes_str));
        while (!buf.end()) {
            char* p = buf.address() + buf.offset;
            if (*p == ',') p++;
            if (*p == '\0') break;
            uint64_t addr = strtoull(p, &p, 16);
            buf.offset += p - (buf.address() + buf.offset);
            
            //Apply offset if any and correct to our one
            if (frame_offset) {
                addr -= frame_offset;
                addr += reinterpret_cast<uint64_t>(&decodeStackAddress);
            }

            std::string line = decodeStackAddress(reinterpret_cast<void*>(addr));
            if (line.empty()) continue;
            printf("%s\n", line.c_str());
        }
        
        ErrH.Exit();
    }
}

//------------------------------

void show_help() {
    printf(
            "Perimeter %s\n%s\n\n"
            "Modding and debugging:\n"
            "    mods=0 - Disables mods folder loading\n"
            "    edit=1 - Enables mission editor mode, use map loading args or file open dialog will appear\n"
            "    mainmenu=0/1 - Enables/disables main menu, needs to obtain what map to load via args if not enabled\n"
            "    icon=path/of/icon - Provide alternate window/app icon to use\n"
            "    no_console_redirect=1 - Prints logs in stdio instead of log file\n"
            "    content_debug=1 - Shows debug info about content and mods loading\n"
            "    content_dump_debug=1 - Writes internal content filesystem mapping into a file\n"
            "    stack_frames/stack_reference - Parameters provided by crash dumps to allow reconstructing stacktrace using same binary\n"
            "    xprm_compiler - Enables runtime XPrm compilation and compiler for .prm files\n"
            "    not_triggerchains_binary=1 - Disallows loading triggerchain stored in .bin instead of .spg\n"
            "    debug_key_handler=1 - Enables debug key handler\n"
            "    explore=1 - Opens Debug.prm editor and closes game\n"
            "    compress_worlds=0/1 - Attempts to decompress or compress all worlds\n"
            "    start_splash=0/1 - Enables or disables intro movies\n"
            "    show_fps=0/1 - Displays FPS counter\n"
            "    convert=1 - Saves opened map and closes game\n"
            "\n"
            "    More info and source code: https://github.com/KD-lab-Open-Source/Perimeter\n"
            "\n"
            "Multiplayer:\n"
            "    server=IP:PORT - Opens game in server mode and binds to address\n"
            "    connect=IP:PORT - Connects to provided server address\n"
            "    password=Password - Password to use as server or connecting to server\n"
            "    name=Name - Player name to use as server or connecting to server\n"
            "    save=savegame - Multiplayer save name to use to resume as server\n"
            "    room=Room - Game/room name for server, computed from server player name if empty\n"
          //"    public=1 - Sets server to broadcast into public list\n" //TODO unimplemented yet
            "\n"
            "Map loading args:\n"
            "    open=path/inside/Resource - Relative path inside resource, like Saves/Profile0/mysave\n"
            "    save=path/inside/Resource/Saves - Relative path inside saves, like Profile0/mysave\n"
            "    mission=path/inside/Resource/Missions - Relative path inside missions, like 00EASTER\n"
            "\n"
            "General arguments:\n"
            "    resx=1280 resy=720 - Allows setting resolution to use\n"
            "    uianchor=0/1/2/3 - Controls UI anchoring when aspect ratio is wider than 4:3\n"
            "    GrabInput=0/1 - Controls window input grabbing\n"
            "    RunBackground=0/1 - Enables or disables running game while not focused\n"
            "    disable_sound=1 - Disables sound in this launch\n"
            "    initial_menu= - Tells game to load this menu screen as initial menu, examples can be SINGLE, MULTIPLAYER_LIST, BATTLE...\n"
            "    content=path/of/game - Use this path for game data/content (must contain Resource, Scripts...)\n"
            "    clearlocale=1 - Clears current default and displays language dialog\n"
            "    locale=Russian - Use different language than current default\n"
            "    --version -v - Shows version\n"
            "    --help -h /? - This help\n"
            ,
            currentShortVersion,
            currentVersion
    );
    ErrH.Exit();
}

//------------------------------
#ifndef _WIN32
int main(int argc, char *argv[]) {
    //Call SDL main init
    SDL_SetMainReady();

    return SDL_main(argc, argv);
}
#endif

char* alloc_exec_arg_string(std::string arg, bool wrap_spaces) {
#ifdef _WIN32
    //Workaround for win32 execv() getting confused when .exe is inside a path containing spaces
    if (wrap_spaces && arg.find(' ') != std::string::npos) {
        arg = "\"" + arg + "\"";
    }
#endif
    size_t len = arg.length() + 1;
    char* str = static_cast<char*>(malloc(len));
    strcpy(str, arg.c_str());
    return str;
}

int SDL_main(int argc, char *argv[])
{
    //Show help if requested
    for(int i = 0; i < argc; i ++) {
        std::string arg = string_to_lower(argv[i]);
        if (arg == "help" || arg == "--help" || arg == "-h" || arg == "/?") {
            show_help();
        } else if (arg == "--version" || arg == "-v") {
            printf("Perimeter %s\n%s\n", currentShortVersion, currentVersion);
            ErrH.Exit();
        }
    }
    
    //We need to copy argc/argv so they can be accessed later via check_command_line etc
    setup_argcv(argc, argv);

    //Init clock
    initclock();

    //Decode stacktrace if requested
    decode_stacktrace();
    
    //Redirect stdio and print version
    ErrH.RedirectStdio();
    printf("Perimeter %s - %s\n", currentShortVersion, currentVersion);

    //Parse version string
    decode_version(currentShortVersion, currentVersionNumbers);

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
    std::vector<std::string> paths = {
            CRASH_DIR,
            "cache/font",
            "cache/bump",
            REPLAY_PATH,
    };
    if(IniManager("Perimeter.ini", false).getInt("Game","AutoSavePlayReel")!=0){
        extern const char* autoSavePlayReelDir;
        paths.emplace_back(autoSavePlayReelDir);
    }
    {
        std::string path = convert_path_content("resource/saves/");
        if (path.empty()) path = "resource/saves/";
        paths.emplace_back(path + "Multiplayer");
    }
    for (std::string path : paths) {
        path = convert_path_native(path);
        bool ok = create_directories(path);
        xassert(ok || std::filesystem::is_directory(std::filesystem::u8path(path)));
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
                    if (gameShell->getNetClient()->setPause(!applicationHasFocus())) {
                        runapp = applicationIsGo();
                    }
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
#ifdef _WIN32
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
        const char* exec_path = nullptr;
        std::vector<char*> exec_argv;
        for (int i = 0; i < app_argc; ++i) {
            if (startsWith(app_argv[i], "tmp_")) {
                //These are passed internally and are not supposed to pass into next instance
                continue;
            }

            if (i == 0) {
                //Doesn't like "s
                exec_path = alloc_exec_arg_string(app_argv[i], false);
            }
            exec_argv.emplace_back(alloc_exec_arg_string(app_argv[i], true));
        }
        
        //Add extra args
        for (auto const& arg : applicationRestartArgs) {
            exec_argv.emplace_back(alloc_exec_arg_string(arg, true));
        }
        
        //Shouldn't happen
        if (!exec_path && !exec_argv.empty()) {
            exec_path = exec_argv[0];
        }

        //execv last arg must be null for termination
        exec_argv.emplace_back(nullptr);
        
        //launch ourselves again, execution of this process stops here
        printf("Restarting: '%s' args:", exec_path);
        for (auto const& str : exec_argv) {
            if (str) {
                printf(" '%s'", str);
            } else {
                printf(" NULL");
            }
        }
        printf("\n");
        int ret;
#ifdef _WIN32
        ret = _execv(exec_path, exec_argv.data());
#else
        ret = execv(exec_path, exec_argv.data());
#endif
        //We shouldn't reach this point        
        ErrH.Abort("Error restarting the application", XERR_USER, ret, strerror(errno));
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
    
    uint32_t windowID = sdlWindow ? SDL_GetWindowID(sdlWindow) : 0;

    //Iterate each SDL event that we may have queued since last poll
    SDL_Event event;
    bool closing = false;
    while (SDL_PollEvent(&event) == 1) {
        if (sdlWindow && event.window.windowID && event.window.windowID != windowID) {
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
                if (terGrabInput && !terFullScreen && applicationHasFocus_ && sdlWindow && SDL_GetWindowGrab(sdlWindow) == SDL_FALSE) {
                    SDL_SetWindowGrab(sdlWindow, SDL_TRUE);
                }
                break;
            }
            case SDL_WINDOWEVENT: {
                switch (event.window.event) {
                    case SDL_WINDOWEVENT_RESIZED:
                    case SDL_WINDOWEVENT_SIZE_CHANGED: {
                        if (0 <= lastWindowResize) {
#if 1
                            lastWindowResize = 10;
#else
                            if (_bMenuMode) {
                                lastWindowResize = 10;
                            } else {
                                //Game can be unstable during frequent resizing
                                lastWindowResize = 20;
                            }
#endif
                        }
                        break;
                    }
                    case SDL_WINDOWEVENT_FOCUS_LOST: {
                        if (terGrabInput && sdlWindow) {
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
                    case SDL_WINDOWEVENT_TAKE_FOCUS:
                    case SDL_WINDOWEVENT_ENTER:
                    case SDL_WINDOWEVENT_LEAVE:
                    case SDL_WINDOWEVENT_EXPOSED:
                    case SDL_WINDOWEVENT_SHOWN:
                    case SDL_WINDOWEVENT_HIDDEN: {
                        break;
                    }
                    default:
                        break;
                }
                break;
            }
            case SDL_KEYUP:
            case SDL_KEYDOWN:
            case SDL_MOUSEBUTTONUP:
            case SDL_MOUSEMOTION: {
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
            if (gameShell) {
                if (gameShell->GameActive && !isShiftPressed() && terRenderDevice->GetRenderSelection() != DEVICE_HEADLESS) {
#ifdef PERIMETER_DEBUG
                    //Nobody got time for this
                    gameShell->terminate();
#else
                    //When game is running we want to gracefully shutdown the game by showing main menu
                    sKey k(VK_ESCAPE, true);
                    gameShell->KeyPressed(k);
                    gameShell->KeyUnpressed(k);
#endif
                } else {
                    //Terminate it
                    gameShell->terminate();
                }
            } else {
                //No gameshell available, manually close stuff
                SDL_ShowCursor(SDL_TRUE);
                SDL_DestroyWindow(sdlWindow);
                sdlWindow = nullptr;
#ifdef _WIN32
                hWndVisGeneric = nullptr;
#endif
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
#ifdef _WIN32
//TODO remove this ifdef block once dialog stuff is ported

bool _setupFileDialog(std::string& filename, const char* initialDir, const char* extention, const char* title, bool save) {
    std::u16string u16string_filename = utf8_to_utf16(filename.c_str());
    u16string_filename.resize(2048);

    UTF8_TO_WCHAR(initialDir, initialDir);
    UTF8_TO_WCHAR(extention, extention);
    UTF8_TO_WCHAR(title, title);
    
	std::u16string filter = utf8_to_utf16(title);
	filter += static_cast<char16_t>('\0');
    filter += static_cast<char16_t>('*');
    filter += static_cast<char16_t>('.');
    filter += utf8_to_utf16(extention);
    filter += static_cast<char16_t>('\0');
    filter += static_cast<char16_t>('\0');

    OPENFILENAMEW ofn;
    memset(&ofn,0,sizeof(OPENFILENAMEW));
    ofn.lpstrFile = checked_reinterpret_cast_ptr<char16_t, wchar_t>(u16string_filename.data());
    ofn.nMaxFile = u16string_filename.capacity()-1;
    ofn.lStructSize = sizeof(OPENFILENAMEW);
    ofn.hwndOwner = hWndVisGeneric;
    std::string fullTitle = save ? "Save: " : "Open: ";
    fullTitle += title;
    UTF8_TO_WCHAR(fullTitle, fullTitle.c_str());
    ofn.lpstrTitle = wchar_fullTitle;
    ofn.lpstrFilter = checked_reinterpret_cast_ptr<char16_t, wchar_t>(filter.data());;
    ofn.lpstrInitialDir = wchar_initialDir;
    ofn.Flags = OFN_PATHMUSTEXIST|OFN_HIDEREADONLY|OFN_EXPLORER|OFN_NOCHANGEDIR;
    ofn.lpstrDefExt = wchar_extention;
    
    if (save) {
        if (!GetSaveFileNameW(&ofn))
            return false;
    } else {
        if (!GetOpenFileNameW(&ofn))
            return false;
    }
    
    filename = utf16_to_utf8(u16string_filename);
    return true;
}
#endif

bool openFileDialog(std::string& filename, const char* initialDir, const char* extention, const char* title)
{
#ifdef _WIN32
    return _setupFileDialog(filename, initialDir, extention, title, false);
#else
	//TODO
    fprintf(stderr, "openFileDialog not implemented for this platform\n");
	return false;
#endif
}

bool saveFileDialog(std::string& filename, const char* initialDir, const char* extention, const char* title)
{
#ifdef _WIN32
    return _setupFileDialog(filename, initialDir, extention, title, true);
#else
    //TODO
    fprintf(stderr, "saveFileDialog not implemented for this platform\n");
    return false;
#endif
}

const char* popupMenu(std::vector<const char*> items) // returns zero if cancel
{
	if(items.empty())
		return 0;

#ifdef _WIN32
	HMENU hMenu = CreatePopupMenu();
	
	std::vector<const char*>::iterator i;
	FOR_EACH(items, i) {
        std::string text = convertToCodepage(*i, getLocale());
		AppendMenuA(hMenu, MF_STRING, 1 + i - items.begin(), text.c_str());
    }
	
	Vect2i ps = gameShell->convertToScreenAbsolute(gameShell->mousePosition());
	int index = TrackPopupMenu(hMenu, TPM_RETURNCMD | TPM_LEFTALIGN | TPM_RIGHTBUTTON, ps.x, ps.y, 0, hWndVisGeneric, 0);
	
	DestroyMenu(hMenu);
	
	if(index > 0 && index <= items.size())
		return items[index - 1];
	else
		return 0;
#else
    //TODO
    fprintf(stderr, "popupMenu not implemented for this platform\n");
    return 0;
#endif
}

int popupMenuIndex(std::vector<const char*> items) // returns -1 if cancel
{
	if(items.empty())
		return -1;

#ifdef _WIN32
	HMENU hMenu = CreatePopupMenu();
	
	std::vector<const char*>::iterator i;
	FOR_EACH(items, i) {
        std::string text = convertToCodepage(*i, getLocale());
		AppendMenuA(hMenu, MF_STRING, 1 + i - items.begin(), text.c_str());
    }
	
	Vect2i ps = gameShell->convertToScreenAbsolute(gameShell->mousePosition());
	int index = TrackPopupMenu(hMenu, TPM_RETURNCMD | TPM_LEFTALIGN | TPM_RIGHTBUTTON, ps.x, ps.y, 0, hWndVisGeneric, 0);
	
	DestroyMenu(hMenu);
	
	if(index > 0 && index <= items.size())
		return index - 1;
	else
		return -1;
#else
    //TODO
    fprintf(stderr, "popupMenuIndex not implemented for this platform\n");
    return -1;
#endif
}



//-----------------------------------------
static std::string editTextString;
#ifdef _WIN32
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
			GetWindowTextA(h,tmpstr,256);
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
			SetWindowTextA(h, editTextString.c_str());
		}
		return TRUE;
	}
	return FALSE;
}
#endif

const char* editText(const char* defaultValue)
{
	editTextString = defaultValue;
#ifdef _WIN32
	DialogBox(GetModuleHandle(0),MAKEINTRESOURCE(IDD_DIALOG_INPUT_TEXT),hWndVisGeneric,DialogProc);
#else
    //TODO
    fprintf(stderr, "editText not implemented for this platform\n");
#endif
	return editTextString.c_str();
}

const char* editTextMultiLine(const char* defaultValue, void* hwnd)
{
	editTextString = defaultValue;
#ifdef _WIN32
	DialogBox(GetModuleHandle(0),MAKEINTRESOURCE(IDD_DIALOG_INPUT_TEXT_MULTILINE), static_cast<HWND>(hwnd), DialogProc);
#else
    //TODO
    fprintf(stderr, "editTextMultiLine not implemented for this platform\n");
#endif
	return editTextString.c_str();
}
