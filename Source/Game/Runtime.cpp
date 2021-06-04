#include "StdAfx.h"

#include "umath.h"
#include "IVisGeneric.h"
#include "VisGenericDefine.h"
#include "IRenderDevice.h"

#include "PrmEdit.h"
#include "Runtime.h"

#include "Terra.h"

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

#include "..\PluginMax\ZIPStream.h"

#include "Universe.h"
#include "..\resource.h"

#include <commdlg.h>
#include "..\ht\ht.h"
#include "GraphicsOptions.h"

const char* currentVersion = 
"Ver "
#include "..\version.h" 
" (" __DATE__ " " __TIME__ ")";

#ifdef _SINGLE_DEMO_
const char* currentShortVersion = 
"Комтек'04 демо, v"
#include "..\version.h" 
;
#endif

#ifdef _MULTIPLAYER_DEMO_
const char* currentShortVersion = 
"Multiplayer Demo (for GameSpy internal testing only), v"
#include "..\version.h" 
;
#endif

#ifndef _DEMO_
const char* currentShortVersion = 
"v"
#include "..\version.h" 
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

HWND hWndVisGeneric=0;
HINSTANCE gb_hInstance=NULL;

int terSetDebugWindow = IniManager("Perimeter.ini").getInt("Graphics","SetDebugWindow");

SyncroTimer global_time;
SyncroTimer frame_time;
SyncroTimer scale_time;

void PerimeterDataChannelLoad();
void PerimeterDataChannelSave();

void PerimeterAviInit();
int PerimeterAviQuant();
void PerimeterAviFinit();

HWND Win32_CreateWindow(char *WinName,int xPos,int yPos,int xScr,int yScr,WNDPROC lpfnWndProc=DefWindowProc,int dwStyle=WS_OVERLAPPEDWINDOW);
LRESULT CALLBACK VisPerimetrClient_WndProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam);

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
		ShowWindow(hWndVisGeneric,SW_MINIMIZE);
	}

}	

void InternalErrorHandler()
{
	RestoreGDI();
	if(universe()) universe()->allSavePlayReel();
}


void HTManager::init()
{
	interpolation_timer_ = 0;
	interpolation_factor_ = 0;

	static XBuffer errorHeading;
	errorHeading.SetRadix(16);
	errorHeading < currentVersion <
#ifdef _FINAL_VERSION_
		" Final"
#else
		" Release"
#endif
		< " OS: " <= GetVersion();

	ErrH.SetPrefix(errorHeading);
	ErrH.SetRestore(InternalErrorHandler);
	SetAssertRestoreGraphicsFunction(RestoreGDI);
	
	xt_get_cpuid();
	xt_getMMXstatus();
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
	gameShell->setWindowClientSize(windowClientSize);

	allocation_tracking("PerimeterLogicInit");

	start_timer=false;
};

void HTManager::done()
{
	if(!terMissionEdit)
		PerimeterDataChannelSave();

	PerimeterAviFinit();
	
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

cInterfaceRenderDevice* SetGraph(int Mode,int xScr,int yScr,int FullScr,int ColorBit,int RefreshRateInHz)
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

	hWndVisGeneric = Win32_CreateWindow("Perimeter",0,0,xScr,yScr,VisPerimetrClient_WndProc,
		(FullScr ? 0 : WS_OVERLAPPEDWINDOW)|WS_POPUP|WS_VISIBLE);

	if(IRenderDevice->Init(xScr,yScr,ModeRender,hWndVisGeneric,RefreshRateInHz))
	{
		ErrorInitialize3D();
	}

	return IRenderDevice;
}

//--------------------------------

void GameShell::SetFontDirectory()
{
	terVisGeneric->SetFontRootDirectory("resource\\LocData");
	string dir=getLocDataPath();
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
	terLight->SetColor(&sColor4f(0,0,0,1),&sColor4f(1,1,1,1));

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

	vMap.prepare("RESOURCE\\WORLDS\\worlds.prm");//,NULL,NULL,0,terRenderDevice->GetSizeX(),terRenderDevice->GetSizeY());
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

sWinVideo terWinVideo;
int terWinVideoValid = 0;
int terWinVideoEnable = 0;//getPerimeterIniInt("Game","PlayAVI");

void PerimeterAviInit()
{
	int sx,sy;
//ForAvi
	if(terWinVideoEnable){
		sx = 0;
		sy = 0;
		terWinVideo.Open("RESOURCE\\VIDEO\\intro.avi");
		terWinVideo.GetSize(&sx,&sy);
		int x = (terScreenSizeX - sx)/2, y = (terScreenSizeY - sy)/2;
		terWinVideo.SetWin(hWndVisGeneric,x,y,sx,sy);
		terWinVideo.FullScreen(terFullScreen);
		terWinVideo.HideCursor(0);
		terWinVideo.Play();
		terWinVideoValid = 1;
	};
};

int PerimeterAviQuant()
{
	if(terWinVideoValid && terWinVideo.IsComplete()){
		PerimeterAviFinit();
		return 1;
	};
	return 0;
};

void PerimeterAviFinit()
{
	if(terWinVideoValid){
		terWinVideo.Stop();
		terWinVideo.Close();
		terWinVideoValid = 0;
	}
}

void checkSingleRunning()
{
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
}

//------------------------------
int PASCAL WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR szCmdLine, int sw)
{
#ifdef _FINAL_VERSION_
	checkSingleRunning();
#endif

	gb_hInstance=hInst;
	int ht=IniManager("Perimeter.ini").getInt("Game","HT");
	check_command_line_parameter("HT", ht);
	HTManager* runtime_object = new HTManager(ht);
	runtime_object->setUseHT(ht?true:false);

	xassert(!(gameShell && gameShell->alwaysRun() && terFullScreen));

	bool run = true;
	MSG msg;
	while(run){
		if(PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE)){
			if(!GetMessage(&msg, NULL, 0, 0))
				break;
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}else {
			//NetworkPause handler
			static bool runapp=true;
			if(applicationIsGo()!=runapp){
				if(gameShell && (!gameShell->alwaysRun()) ){
					if(gameShell->getNetClient()){
						if(gameShell->getNetClient()->setPause(!applicationHasFocus_))
							runapp=applicationIsGo();
					}
				}
			}
//			if(gameShell && (!gameShell->alwaysRun()) ){
//				if(gameShell->getNetClient())
//					gameShell->getNetClient()->pauseQuant(applicationIsGo()));
//			}

			if(applicationIsGo())
				run = runtime_object->Quant();
			else
				WaitMessage();
		}
	}

	delete runtime_object;

	return 0;
}

//--------------------------------
STARFORCE_API void CalcRealWindowPos(int xPos,int yPos,int xScr,int yScr,bool fullscreen,Vect2i& pos,Vect2i& size)
{
	if(xScr<0) xScr=GetSystemMetrics(SM_CXSCREEN);
	if(yScr<0) yScr=GetSystemMetrics(SM_CYSCREEN);
	if(!fullscreen)
	{
		xScr += GetSystemMetrics(SM_CXSIZEFRAME)*2;
		yScr += GetSystemMetrics(SM_CYSIZEFRAME)*2 + GetSystemMetrics(SM_CYCAPTION);
	}
	if(terMissionEdit) 
		yScr += GetSystemMetrics(SM_CYMENU);
	if(terSetDebugWindow)
	{
		xPos-=GetSystemMetrics(SM_CXSIZEFRAME);
		yPos-=GetSystemMetrics(SM_CYSIZEFRAME)+GetSystemMetrics(SM_CYCAPTION);
	}

	pos.x=xPos;
	pos.y=yPos;
	size.x=xScr;
	size.y=yScr;
}

HWND Win32_CreateWindow(char *WinName,int xPos,int yPos,int xScr,int yScr,WNDPROC lpfnWndProc,int dwStyle)
{
	HICON hIconSm=(HICON)LoadImage(gb_hInstance,"PERIMETER",IMAGE_ICON,GetSystemMetrics(SM_CXSMICON),GetSystemMetrics(SM_CYSMICON),LR_DEFAULTCOLOR);
	HICON hIcon=(HICON)LoadImage(gb_hInstance,"PERIMETER",IMAGE_ICON,GetSystemMetrics(SM_CXICON),GetSystemMetrics(SM_CYICON),LR_DEFAULTCOLOR);
	WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_DBLCLKS|CS_HREDRAW|CS_VREDRAW,lpfnWndProc,0,0,gb_hInstance,hIcon,0,(HBRUSH)GetStockObject(BLACK_BRUSH),0,WinName, hIconSm};
    if(RegisterClassEx(&wc)==0) return 0;

	Vect2i real_pos,real_size;
	CalcRealWindowPos(xPos,yPos,xScr,yScr,!(dwStyle&WS_OVERLAPPEDWINDOW),real_pos,real_size);
	xPos=real_pos.x;
	yPos=real_pos.y;
	xScr=real_size.x;
	yScr=real_size.y;

	HWND hWnd=CreateWindow(WinName,WinName,dwStyle,xPos,yPos,xScr,yScr,0,0,gb_hInstance,0);
	if(hWnd == 0)	{ UnregisterClass(WinName,gb_hInstance); return 0; }
	if(xScr >= GetSystemMetrics(SM_CXSCREEN) && yScr >= GetSystemMetrics(SM_CYSCREEN)
		&& !terSetDebugWindow) 
		ShowWindow(hWnd,SHOW_FULLSCREEN);
	else
		ShowWindow(hWnd,SW_SHOWNORMAL);
//	ShowCursor(0);
	return hWnd;
}
void Win32_ReleaseWindow(HWND hWnd)
{
	ShowCursor(1);
	if(hWnd) DestroyWindow(hWnd); 
}

LRESULT CALLBACK VisPerimetrClient_WndProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	if(uMsg == WM_GETMINMAXINFO && terSetDebugWindow) {
		if(terScreenSizeX == GetSystemMetrics(SM_CXSCREEN) &&
		  terScreenSizeY == GetSystemMetrics(SM_CYSCREEN)) {
			MINMAXINFO *pMinMax;
			pMinMax = (MINMAXINFO *)lParam;
			POINT& p=pMinMax->ptMaxTrackSize;
			p.x=terScreenSizeX;
			p.y=terScreenSizeY;
			p.x += GetSystemMetrics(SM_CXSIZEFRAME)*2;
			p.y += GetSystemMetrics(SM_CYSIZEFRAME)*2 + GetSystemMetrics(SM_CYCAPTION);
			if(terMissionEdit) 
				p.y += GetSystemMetrics(SM_CYMENU);
			return 0;
		}
	}

	if(gameShell)
		gameShell->EventHandler(uMsg, wParam, lParam);

    switch(uMsg) 
	{
	case WM_CREATE:
		break;
    case WM_PAINT:
        break;
    case WM_MOVE:
        break;
    case WM_SIZE: {
			RECT rc;
			if(terFullScreen){
				//GetWindowRect(hWnd, &rc);
				windowClientSize.x = terScreenSizeX;
				windowClientSize.y = terScreenSizeY;
			}
			else
			{
				GetClientRect(hWnd, &rc);
				windowClientSize.x = rc.right - rc.left;
				windowClientSize.y = rc.bottom - rc.top;
			}
			if(gameShell)
				gameShell->setWindowClientSize(windowClientSize);
		}
/*
		if((SIZE_MAXHIDE==wParam)||(SIZE_MINIMIZED==wParam)) 
			applicationHasFocus_ = false;
		else 
			applicationHasFocus_= true;
*/
		break;
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
    case WM_CLOSE:
		#ifndef _DEMO_
			if(gameShell)
				gameShell->terminate();
			else
				Win32_ReleaseWindow(hWnd);
		#endif
        return 0;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0L;
	case WM_MOUSELEAVE:
		break;
	case WM_MOUSEMOVE: {
		TRACKMOUSEEVENT tme;
		tme.cbSize  = sizeof(TRACKMOUSEEVENT);
		tme.dwFlags = TME_LEAVE;
		tme.hwndTrack = hWnd;
		_TrackMouseEvent(&tme);
		break;
		}
    case WM_ACTIVATEAPP:
		applicationHasFocus_ = (bool)wParam;
        return 0;
	case WM_KEYUP:
	case WM_SYSKEYUP:
		if (wParam == VK_MENU) {
			return 0;
		} else {
			break;
		}
    }
    return DefWindowProc(hWnd,uMsg,wParam,lParam);
}

void setLogicFp()
{
	_controlfp(_PC_24, _MCW_PC);

#ifndef _FINAL_VERSION_
	static int enable = IniManager("Perimeter.ini").getInt("Game","ControlFpEnable");
	if(enable){
		_controlfp( _controlfp(0,0) & ~(EM_OVERFLOW | EM_ZERODIVIDE | EM_DENORMAL |  EM_INVALID),  MCW_EM ); 
		_clearfp();
	}
#endif
}

//-------------------------------------------------
bool openFileDialog(string& filename, const char* initialDir, const char* extention, const char* title)
{
	XBuffer filter;
	filter < title < '\0' < "*." < extention < '\0' < '\0';

	OPENFILENAME ofn;
	memset(&ofn,0,sizeof(ofn));
	char fname[2048];
	strcpy(fname,filename.c_str());
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hWndVisGeneric;
	string fullTitle = string("Open: ") + title;
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
}

bool saveFileDialog(string& filename, const char* initialDir, const char* extention, const char* title)
{
	XBuffer filter;
	filter < title < '\0' < "*." < extention < '\0' < '\0';

	OPENFILENAME ofn;
	memset(&ofn,0,sizeof(ofn));
	char fname[2048];
	strcpy(fname,filename.c_str());
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hWndVisGeneric;
	string fullTitle = string("Save: ") + title;
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
}

const char* popupMenu(vector<const char*> items) // returns zero if cancel
{
	if(items.empty())
		return 0;

	HMENU hMenu = CreatePopupMenu();
	
	vector<const char*>::iterator i;
	FOR_EACH(items, i)
		AppendMenu(hMenu, MF_STRING, 1 + i - items.begin(), *i);
	
	Vect2i ps = gameShell->convertToScreenAbsolute(gameShell->mousePosition());
	int index = TrackPopupMenu(hMenu, TPM_RETURNCMD | TPM_LEFTALIGN | TPM_RIGHTBUTTON, ps.x, ps.y, 0, hWndVisGeneric, 0);
	
	DestroyMenu(hMenu);
	
	if(index > 0 && index <= items.size())
		return items[index - 1];
	else
		return 0;
}

int popupMenuIndex(vector<const char*> items) // returns -1 if cancel
{
	if(items.empty())
		return -1;

	HMENU hMenu = CreatePopupMenu();
	
	vector<const char*>::iterator i;
	FOR_EACH(items, i)
		AppendMenu(hMenu, MF_STRING, 1 + i - items.begin(), *i);
	
	Vect2i ps = gameShell->convertToScreenAbsolute(gameShell->mousePosition());
	int index = TrackPopupMenu(hMenu, TPM_RETURNCMD | TPM_LEFTALIGN | TPM_RIGHTBUTTON, ps.x, ps.y, 0, hWndVisGeneric, 0);
	
	DestroyMenu(hMenu);
	
	if(index > 0 && index <= items.size())
		return index - 1;
	else
		return -1;
}



//-----------------------------------------
static string editTextString;
static BOOL CALLBACK DialogProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
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

const char* editText(const char* defaultValue)
{
	editTextString = defaultValue;
	int ret = DialogBox(GetModuleHandle(0),MAKEINTRESOURCE(IDD_DIALOG_INPUT_TEXT),hWndVisGeneric,DialogProc);
//	if(ret!=IDOK)
//		return 0;
	return editTextString.c_str();
}

const char* editTextMultiLine(const char* defaultValue, HWND hwnd)
{
	editTextString = defaultValue;
	int ret = DialogBox(GetModuleHandle(0),MAKEINTRESOURCE(IDD_DIALOG_INPUT_TEXT_MULTILINE),hwnd,DialogProc);
//	if(ret!=IDOK)
//		return 0;
	return editTextString.c_str();
}

