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

#include "WinVideo.h"
#include "Config.h"

#include "PerimeterSound.h"
#include "PlayOgg.h"
#include "Controls.h"

#include "MusicManager.h"

#include "UnitAttribute.h"
#include "PerimeterShellUI.h"

#include "../PluginMAX/ZIPStream.h"

#include "Universe.h"
#include "../resource.h"

#include <SDL.h>
#ifdef _WIN32
//Needed for extracting HWND from SDL_Window, in Linux it gives conflict due to XErrorHandler
#include <SDL_syswm.h>
#endif

#ifndef PERIMETER_EXODUS
#include <commdlg.h>
#endif
#include "../HT/ht.h"
#include "GraphicsOptions.h"

const char* currentVersion = 
"Ver "
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

#ifdef _SINGLE_DEMO_
const char* currentShortVersion = 
"Комтек'04 демо, v"
#include "../version.h"
;
#endif

#ifndef _DEMO_
const char* currentShortVersion = 
"v"
#include "../version.h"
;
#endif

static int terMissionEdit = 0;

cLogicGeneric* terLogicGeneric = NULL;
cVisGeneric* terVisGeneric = NULL;
cInterfaceRenderDevice* terRenderDevice = NULL;

cScene* terScene = NULL;
cUnkLight* terLight = NULL;
cTileMap* terMapPoint = NULL;

int terBitPerPixel = 16;
int tepRefreshRateInHz = 0;					  
int terFullScreen = 0;
int terScreenSizeX = 800;
int terScreenSizeY = 600;
float terGraphicsGamma = 1;

int terDrawMeshShadow = 2;
int terShadowType	= 0;
float terFarDistanceLOD = 0.003f;
float terNearDistanceLOD = 0.1f;
int terMipMapBlur = 0;
int terMipMapLevel = 3;

float terMapLevelLOD = 0.5f;

GameShell* gameShell = 0;

static Vect2i windowClientSize = Vect2i(1024, 768);

static bool applicationHasFocus_ = true;

SDL_Window* sdlWindow = nullptr;
HWND hWndVisGeneric = nullptr;

int terSetDebugWindow = 0;

SyncroTimer global_time;
SyncroTimer frame_time;
SyncroTimer scale_time;

void PerimeterDataChannelLoad();
void PerimeterDataChannelSave();

void SDL_event_poll();

bool applicationHasFocus()
{
	return applicationHasFocus_;
}

bool applicationIsGo()
{
	return applicationHasFocus() || (gameShell && gameShell->alwaysRun());
}

STARFORCE_API void SetShadowType(int shadow_map,int shadow_size,bool update)
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

void InternalErrorHandler()
{
	RestoreGDI();
	if(universe()) universe()->allSavePlayReel();
}

void refresh_window_size(bool update_resolution) {
    if(terFullScreen){
        windowClientSize.x = terScreenSizeX;
        windowClientSize.y = terScreenSizeY;
    } else {
        SDL_GetWindowSize(sdlWindow, &windowClientSize.x, &windowClientSize.y);
    }
    if(gameShell) {
        gameShell->setWindowClientSize(windowClientSize);
        if (update_resolution) {
            gameShell->updateResolution(windowClientSize.x, windowClientSize.y, false, true);
        }
        _shellCursorManager.OnWMSetCursor();
    }
}

void CrashHandler()
{
    if (gameShell && universe()) {
        std::string crash = std::string(CRASH_DIR) + PATH_SEP + std::to_string(time(nullptr)) + "_";
        
        //First attempt to save reel
        terHyperSpace::SAVE_REPLAY_RESULT statereel = universe()->savePlayReel((crash + "reel").c_str());
        fprintf(stderr, "Crash reel result: %d\n", statereel);

        //Attempt to save state
        MissionDescription mission(gameShell->CurrentMission);
        mission.setSaveName((crash + "save").c_str());
        mission.missionNumber = gameShell->currentSingleProfile.getCurrentMissionNumber();
        bool statesave = universe()->universalSave(mission, true);
        fprintf(stderr, "Crash save result: %d\n", statesave);
    }
}

void HTManager::init()
{
	interpolation_timer_ = 0;
	interpolation_factor_ = 0;

    ErrH.SetPrefix(currentVersion);
#ifndef _FINAL_VERSION_
	ErrH.SetRestore(InternalErrorHandler);
#endif
    ErrH.SetCrash(CrashHandler);
	SetAssertRestoreGraphicsFunction(RestoreGDI);
	
	//xt_get_cpuid();
	//xt_getMMXstatus();
	initclock();

	allocation_tracking("before");

	if(IniManager("Perimeter.ini").getInt("Game","ZIP"))
		ZIPOpen("resource.pak");

	PerimeterDataChannelLoad();

	terMissionEdit = IniManager("Perimeter.ini").getInt("Game","MissionEdit");
	check_command_line_parameter("edit", terMissionEdit);

	GameShell::preLoad();

	initGraphics();

	void debugStartUp();
	debugStartUp();

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

HWND PerimeterCreateWindow(int size_x, int size_y, bool full_screen) {
    Uint32 window_flags = SDL_WINDOW_SHOWN;
#ifndef _WIN32
    //On non Windows we use dxvk-native which uses Vulkan
    window_flags |= SDL_WINDOW_VULKAN;
#endif
    if (full_screen) {
        window_flags |= SDL_WINDOW_FULLSCREEN;
    }
    /* TODO
    if (terResizableWindow) {
        window_flags |= SDL_WINDOW_RESIZABLE;
    }
    */
    sdlWindow = SDL_CreateWindow(
            "Perimeter",
            SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED,
            size_x, size_y,
            window_flags
    );
    if (!sdlWindow) {
        ErrH.Abort("Error creating SDL window", XERR_CRITICAL, window_flags, SDL_GetError());
    }

    //Setup window icon
    //TODO SDL_SetWindowIcon(sdlWindow, icon);

#ifdef _WIN32
    //Get HWND from SDL window
    SDL_SysWMinfo wm_info;
	SDL_VERSION(&wm_info.version);
	SDL_GetWindowWMInfo(sdlWindow, &wm_info);
	
    return wm_info.info.win.window;
#else
    //dxvk-native uses HWND as SDL2 window handle, so this is allowed
    return reinterpret_cast<HWND>(sdlWindow);
#endif
}

cInterfaceRenderDevice* SetGraph(int Mode, int xScr, int yScr, int FullScr, int ColorBit, int RefreshRateInHz)
{
	cInterfaceRenderDevice *IRenderDevice=0;
	IRenderDevice=CreateIRenderDevice();

	int ModeRender=0;
	if(FullScr==0) 
		ModeRender |= RENDERDEVICE_MODE_WINDOW;
	if(ColorBit==32) 
		ModeRender|=RENDERDEVICE_MODE_RGB32; 
	else 
		ModeRender|=RENDERDEVICE_MODE_RGB16;

//	if(HTManager::instance()->IsUseHT())
		ModeRender|=RENDERDEVICE_MODE_MULTITHREAD;

    hWndVisGeneric = PerimeterCreateWindow(xScr, yScr, FullScr != 0);

	if(IRenderDevice->Init(xScr,yScr,ModeRender,hWndVisGeneric,RefreshRateInHz))
	{
		ErrorInitialize3D();
	}

	return IRenderDevice;
}

//--------------------------------

void GameShell::SetFontDirectory()
{
	terVisGeneric->SetFontRootDirectory("RESOURCE\\LocData");
	std::string dir=getLocDataPath();
	dir+="Fonts";
	terVisGeneric->SetFontDirectory(dir.c_str());
}

void HTManager::initGraphics()
{
	terLogicGeneric = CreateILogicGeneric();
	terVisGeneric = CreateIVisGeneric();
	terVisGeneric->SetMapLevel(terMapLevelLOD);
	GameShell::SetFontDirectory();

	terVisGeneric->SetEffectLibraryPath("RESOURCE\\FX","RESOURCE\\FX\\TEXTURES");

	bool occlusion=IniManager("Perimeter.ini").getInt("Graphics","EnableOcclusion");
	terVisGeneric->EnableOcclusion(occlusion);
	bool point_light=IniManager("Perimeter.ini").getInt("Graphics","EnablePointLight");
	terVisGeneric->EnablePointLight(point_light);

	terVisGeneric->SetFarDistanceLOD(terFarDistanceLOD);
	terVisGeneric->SetNearDistanceLOD(terNearDistanceLOD);
	terVisGeneric->SetMipMapBlur(terMipMapBlur);
	terVisGeneric->SetMipMapLevel(terMipMapLevel);

	terRenderDevice = SetGraph(1, terScreenSizeX,terScreenSizeY,terFullScreen,terBitPerPixel,tepRefreshRateInHz);

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

	vMap.prepare("RESOURCE\\Worlds\\WORLDS.PRM");//,NULL,NULL,0,terRenderDevice->GetSizeX(),terRenderDevice->GetSizeY());
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
}

//--------------------------------

STARFORCE_API void InitSound(bool sound, bool music, bool firstTime)
{
	terSoundEnable = sound;
	terMusicEnable = music;

	if (firstTime) {
		IniManager ini("Perimeter.ini");
		terSoundVolume = ini.getFloat("Sound","SoundVolume");
		terMusicVolume = ini.getFloat("Sound","MusicVolume");
	}

	if(terSoundEnable  || terMusicEnable){
		static int inited = 0;

		SNDSetLocDataDirectory(GameShell::getLocDataPath().c_str());

		if(!inited){
			inited = 1;

			SNDSetSoundDirectory("RESOURCE\\SOUNDS\\EFF\\");

			if(terEnableSoundLog)
				SNDEnableErrorLog("sound.txt");

			if(SNDInitSound(hWndVisGeneric,true,true)){
				SNDScriptPrmEnableAll();
				MpegInitLibrary(SNDGetDirectSound());
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

	MpegDeinitLibrary();
	SNDReleaseSound();
}

//--------------------------------

void checkSingleRunning()
{
#if defined(_FINAL_VERSION_) && !defined(PERIMETER_DEBUG) && !defined(PERIMETER_EXODUS)
    //NOTE: with PERIMETER_EXODUS this is disabled due to pevents not supporting named events
    
	static HANDLE hSingularEvent = 0;
	static char psSingularEventName[] = "Perimeter";

    hSingularEvent = OpenEvent(EVENT_ALL_ACCESS, FALSE, psSingularEventName);
    if(!hSingularEvent){
		hSingularEvent = CreateEvent(0, TRUE, TRUE, psSingularEventName);
    }
	else{
		HWND hwnd = FindWindow(0, psSingularEventName);
		if(hwnd)
			SetForegroundWindow(hwnd);
		ErrH.Exit();
	}
#endif
}

//------------------------------
int main(int argc, char *argv[])
{
    //Call SDL main init
    SDL_SetMainReady();
    
    //We need to copy argc/argv so they can be accessed later via check_command_line etc
    setup_argcv(argc, argv);

    //Check if only one instance is running
    checkSingleRunning();
    
    //Create crash folder
    std::filesystem::create_directories(CRASH_DIR);
    
    //Scan resources first
    scan_resource_paths();

    g_controls_converter.LoadKeyNameTable();

    //Start SDL stuff
    int sdlresult = SDL_Init(SDL_INIT_TIMER | SDL_INIT_AUDIO | SDL_INIT_VIDEO | SDL_INIT_EVENTS);
    if (sdlresult < 0) {
        ErrH.Abort("Error initializing SDL", XERR_CRITICAL, sdlresult, SDL_GetError());
    }
    
	int ht=IniManager("Perimeter.ini").getInt("Game","HT");
	check_command_line_parameter("HT", ht);
	HTManager* runtime_object = new HTManager(ht);
	runtime_object->setUseHT(ht?true:false);

	xassert(!(gameShell && gameShell->alwaysRun() && terFullScreen));

	bool run = true;
	while(run){
        SDL_event_poll();

#ifndef PERIMETER_EXODUS
	    //TODO is this necessary under SDL2 in Win32?
        MSG msg;
		if(PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE)){
			if(!GetMessage(&msg, NULL, 0, 0))
				break;
			TranslateMessage( &msg );
			DispatchMessage( &msg );
			continue;
		}
#endif
        //NetworkPause handler
        static bool runapp=true;
        if (applicationIsGo()!=runapp){
            if(gameShell && (!gameShell->alwaysRun()) ){
                if(gameShell->getNetClient()){
                    if(gameShell->getNetClient()->setPause(!applicationHasFocus()))
                        runapp=applicationIsGo();
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
	
	SDL_Quit();

	return 0;
}

//--------------------------------

void SDL_event_poll() {
    uint32_t windowID = SDL_GetWindowID(sdlWindow);
    SDL_Event event;
    bool closing = false;
    while (SDL_PollEvent(&event) == 1) {
        if (event.window.windowID && (!sdlWindow || event.window.windowID != windowID)) {
            //Event is for a window that is not current or window is not available
            continue;
        }

        if(gameShell)
            gameShell->EventHandler(event);

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
            case SDL_WINDOWEVENT: {
                switch (event.window.event) {
                    case SDL_WINDOWEVENT_SHOWN: {
                        break;
                    }
                    case SDL_WINDOWEVENT_RESIZED:
                    case SDL_WINDOWEVENT_SIZE_CHANGED: {
                        refresh_window_size(!terFullScreen);
                        break;
                    }
#ifndef PERIMETER_DEBUG
                    case SDL_WINDOWEVENT_FOCUS_LOST: {
                        applicationHasFocus_ = false;
                        break;
                    }
                    case SDL_WINDOWEVENT_FOCUS_GAINED: {
                        applicationHasFocus_= true;
                        break;
                    }
#endif
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
	_controlfp(_PC_24, _MCW_PC);

#ifndef _FINAL_VERSION_
	static int enable = IniManager("Perimeter.ini").getInt("Game","ControlFpEnable");
	if(enable){
		_controlfp( _controlfp(0,0) & ~(EM_OVERFLOW | EM_ZERODIVIDE | EM_DENORMAL |  EM_INVALID),  _MCW_EM ); 
		_clearfp();
	}
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

const char* editTextMultiLine(const char* defaultValue, HWND hwnd)
{
	editTextString = defaultValue;
#ifndef PERIMETER_EXODUS
	DialogBox(GetModuleHandle(0),MAKEINTRESOURCE(IDD_DIALOG_INPUT_TEXT_MULTILINE),hwnd,DialogProc);
#endif
	return editTextString.c_str();
}

