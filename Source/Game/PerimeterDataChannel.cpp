#include "StdAfx.h"
#include "NetPlayer.h"
#include "Runtime.h"
#include "GameShell.h"
#include "CameraManager.h"

extern float CAMERA_SCROLL_SPEED_DELTA,CAMERA_BORDER_SCROLL_SPEED_DELTA;

extern int terCurrentServerIP;
extern int terCurrentServerPort;

extern float terMapLevelLOD;
extern int terDrawMeshShadow;
extern int terShadowType;
extern int terMipMapLevel;
extern int terShowTips;
extern int terResizableWindow;
extern int applicationRunBackground;

extern float terNearDistanceLOD;
extern float terFarDistanceLOD;

extern int terMipMapBlur;

int terMapReflection = 0;
int terObjectReflection = 0;

int terSoundEnable = 1;
int terMusicEnable = 1;
float terSoundVolume = 1;
float terMusicVolume = 1;


bool terEnableBumpChaos = true;

void PerimeterDataChannelLoad()
{
	IniManager ini("Perimeter.ini");
	IniManager ini_no_check("Perimeter.ini", false);

	//GraphicsSection
	terFullScreen = ini.getInt("Graphics","FullScreen");
    terScreenIndex = -1;
    ini_no_check.getInt("Graphics", "ScreenIndex", terScreenIndex);
    terScreenRefresh = ini_no_check.getInt("Graphics", "ScreenRefresh");
    int ScreenSizeX = ini.getInt("Graphics","ScreenSizeX");
    int ScreenSizeY = ini.getInt("Graphics","ScreenSizeY");
    if (!terFullScreen) {
        check_command_line_parameter("resx", ScreenSizeX);
        check_command_line_parameter("resy", ScreenSizeY);
    }
	if (0 < ScreenSizeX) terScreenSizeX = ScreenSizeX;
	if (0 < ScreenSizeY) terScreenSizeY = ScreenSizeY;
    ini_no_check.getInt("Graphics","ResizableWindow", terResizableWindow);
    check_command_line_parameter("resizablewindow", terResizableWindow);
	terBitPerPixel = ini.getInt("Graphics","BPP");
	terMapLevelLOD = ini.getInt("Graphics","MapLevelLOD");

	terDrawMeshShadow = ini.getInt("Graphics","DrawMeshShadow");
	terShadowType = ini.getInt("Graphics","ShadowType");
	terEnableBumpChaos = ini.getInt("Graphics","EnableBumpChaos");
	gb_VisGeneric->SetFavoriteLoadDDS(ini.getInt("Graphics","FavoriteLoadDDS"));

	terNearDistanceLOD = ini.getInt("Graphics","NearDistanceLOD");
	terFarDistanceLOD = ini.getInt("Graphics","FarDistanceLOD");

	terMipMapBlur = ini.getInt("Graphics","MipMapBlur");

	terMipMapLevel = ini.getInt("Graphics","MipMapLevel");
//	terMapReflection = ini.getInt("Graphics","MapReflection");
//	terObjectReflection = ini.getInt("Graphics","ObjectReflection");
	terGraphicsGamma = ini.getFloat("Graphics","Gamma");
	terShowTips = ini.getInt("Game","ShowTips");
    ini_no_check.getInt("Graphics","GrabInput", terGrabInput);
    check_command_line_parameter("GrabInput", terGrabInput);
    ini_no_check.getInt("Game","RunBackground", applicationRunBackground);
    check_command_line_parameter("RunBackground", applicationRunBackground);

	CAMERA_SCROLL_SPEED_DELTA = CAMERA_BORDER_SCROLL_SPEED_DELTA = ini.getInt("Game","ScrollRate");
	CAMERA_MOUSE_ANGLE_SPEED_DELTA = ini_no_check.getFloat("Game","MouseLookRate");
	if (CAMERA_MOUSE_ANGLE_SPEED_DELTA == 0) {
		CAMERA_MOUSE_ANGLE_SPEED_DELTA = 3;
	}
	CAMERA_KBD_ANGLE_SPEED_DELTA = CAMERA_MOUSE_ANGLE_SPEED_DELTA / 30.0f;
}

void PerimeterDataChannelSave()
{
	IniManager ini("Perimeter.ini");

	//GameSection
    ini.putInt("Game", "ShowTips", terShowTips);
    ini.putInt("Game", "RunBackground", applicationRunBackground);
	ini.putInt("Game", "ScrollRate", xm::round(CAMERA_SCROLL_SPEED_DELTA));
	ini.putFloat("Game", "MouseLookRate", xm::round(CAMERA_MOUSE_ANGLE_SPEED_DELTA));
//	ini.putFloat("Game", "GameSpeed", gameShell->getSpeed());
    if (terCamera) {
        ini.putInt("Game", "CameraRestriction", terCamera->restricted() ? 1 : 0);
    }
	
	//GraphicsSection
//	ini.putInt("Graphics","FullScreen", terFullScreen);
    if (!terFullScreen) {
        //We want window size to be stored in case of windowed mode
        if (sdlWindow) {
            int windowScreenIndex = SDL_GetWindowDisplayIndex(sdlWindow);
            if (0 <= windowScreenIndex) {
                terScreenIndex = windowScreenIndex;
            }
        }
        ini.putInt("Graphics","ScreenIndex", terScreenIndex);
        ini.putInt("Graphics","ScreenSizeX", terScreenSizeX);
        ini.putInt("Graphics","ScreenSizeY", terScreenSizeY);
    }
//	ini.putInt("Graphics","ScreenRefresh", terScreenRefresh);
//	ini.putInt("Graphics","BPP", terBitPerPixel);


//	ini.putInt("Graphics","MapLevelLOD", xm::round(terMapLevelLOD));
//	ini.putInt("Graphics","DrawMeshShadow", terDrawMeshShadow);
//	ini.putInt("Graphics","ShadowType", terShadowType);

	ini.putInt("Graphics", "NearDistanceLOD", xm::round(terNearDistanceLOD));
	ini.putInt("Graphics", "FarDistanceLOD", xm::round(terFarDistanceLOD));


	ini.putInt("Graphics","MipMapBlur", terMipMapBlur);
	ini.putInt("Graphics","MipMapLevel", terMipMapLevel);
//	ini.putInt("Graphics","MapReflection", terMapReflection);
//	ini.putInt("Graphics","ObjectReflection", terObjectReflection);
//	ini.putFloat("Graphics","Gamma", terGraphicsGamma);
}

