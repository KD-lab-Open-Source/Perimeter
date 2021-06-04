#include "StdAfx.h"
#include "NetPlayer.h"
#include "Runtime.h"
#include "GameShell.h"

extern float CAMERA_SCROLL_SPEED_DELTA,CAMERA_BORDER_SCROLL_SPEED_DELTA;

extern int terCurrentServerIP;
extern int terCurrentServerPort;
extern char* terCurrentServerName;

extern float terMapLevelLOD;
extern int terDrawMeshShadow;
extern int terShadowType;
extern int terMipMapLevel;
extern int terShowTips;

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
	terScreenSizeX = ini.getInt("Graphics","ScreenSizeX");
	terScreenSizeY = ini.getInt("Graphics","ScreenSizeY");
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

	//Network
	const char* s = ini_no_check.get("Network","ServerName");
	if(s){
		terCurrentServerName = new char[strlen(s) + 1];
		strcpy(terCurrentServerName,s);
	}
	else 
		terCurrentServerName = NULL;

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
	ini.putInt("Game", "ScrollRate", round(CAMERA_SCROLL_SPEED_DELTA));
	ini.putFloat("Game", "MouseLookRate", round(CAMERA_MOUSE_ANGLE_SPEED_DELTA));
//	ini.putFloat("Game", "GameSpeed", gameShell->getSpeed());
	
	//GraphicsSection
	ini.putInt("Graphics","FullScreen", terFullScreen);
//	ini.putInt("Graphics","ScreenSizeX", terScreenSizeX);
//	ini.putInt("Graphics","ScreenSizeY", terScreenSizeY);
//	ini.putInt("Graphics","BPP", terBitPerPixel);


//	ini.putInt("Graphics","MapLevelLOD", round(terMapLevelLOD));
//	ini.putInt("Graphics","DrawMeshShadow", terDrawMeshShadow);
//	ini.putInt("Graphics","ShadowType", terShadowType);

	ini.putInt("Graphics","NearDistanceLOD", round(terNearDistanceLOD));
	ini.putInt("Graphics","FarDistanceLOD", round(terFarDistanceLOD));


	ini.putInt("Graphics","MipMapBlur", terMipMapBlur);
	ini.putInt("Graphics","MipMapLevel", terMipMapLevel);
//	ini.putInt("Graphics","MapReflection", terMapReflection);
//	ini.putInt("Graphics","ObjectReflection", terObjectReflection);
//	ini.putFloat("Graphics","Gamma", terGraphicsGamma);

	//Network
	ini.put("Network","ServerName", terCurrentServerName);
}

