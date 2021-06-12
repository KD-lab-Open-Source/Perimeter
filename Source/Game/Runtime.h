#ifndef __RUNTIME_H__
#define __RUNTIME_H__

extern SyncroTimer global_time;	// детерминированный таймер
extern SyncroTimer frame_time; // недетерминированный таймер
extern SyncroTimer scale_time; // недетерминированный ускоряемый таймер

//--------------------------------------

void setLogicFp();

void PlayMusic(const char *str);
void SetVolumeMusic(float f);
void MusicEnable(int enable);
STARFORCE_API void InitSound(bool sound, bool music, bool firstTime = true);
void SoundQuant();
void FinitSound();


//--------------------------------------
extern class cVisGeneric* terVisGeneric;
extern class cInterfaceRenderDevice* terRenderDevice;
extern class cLogicGeneric* terLogicGeneric;

extern class cScene* terScene;
extern class cUnkLight* terLight;
extern class cTileMap* terMapPoint;

extern int terFullScreen;
extern int terScreenSizeX;
extern int terScreenSizeY;
extern int terBitPerPixel;

extern int terMapReflection;
extern int terObjectReflection;

extern int terWinVideoValid;
extern int terWinVideoEnable;

extern int terSoundEnable;		// 0,1
extern int terMusicEnable;		// 0,1
extern float terSoundVolume;	// 0..1
extern float terMusicVolume;	// 0..1

extern float terGraphicsGamma;	// 0.5..2.5

extern int terShadowType;
extern int terDrawMeshShadow;
extern bool terEnableBumpChaos;
STARFORCE_API void SetShadowType(int shadow_map,int shadow_size,bool update);

extern const char* currentVersion;
extern const char* currentShortVersion;

//-------------------------------------------------
bool openFileDialog(string& filename, const char* initialDir, const char* extention, const char* title);
bool saveFileDialog(string& filename, const char* initialDir, const char* extention, const char* title);
const char* popupMenu(vector<const char*> items); // returns zero if cancel
int popupMenuIndex(vector<const char*> items); // returns -1 if cancel
const char* editText(const char* defaultValue);
const char* editTextMultiLine(const char* defaultValue, HWND hwnd);

#endif //__RUNTIME_H__