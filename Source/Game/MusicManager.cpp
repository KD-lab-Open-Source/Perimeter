#include "StdAfx.h"

#include "Umath.h"
#include "IVisGeneric.h"
#include "VisGenericDefine.h"
#include "IRenderDevice.h"

#include "CameraManager.h"

#include "terra.h"
#include "Runtime.h"

#include "UnitAttribute.h"
#include "GenericControls.h"
#include "Universe.h"
#include "GameShell.h"

#include "PlayOgg.h"
#include "Config.hi"


extern MpegSound gb_Music;
extern string strMusic;
extern SyncroTimer global_time;

class MusicClass
{
	enum PLAY
	{
		PLAY_CONSTRUCTION,
		PLAY_BATTLE,
		PLAY_REGULAR
	} play;

	int on_damage;
	int on_cluster;

	bool active;

	MpegSound* mpeg;
public:
	MusicClass()
	{
		on_damage=on_cluster=0;
		active=false;
		mpeg=&gb_Music;
	}

	~MusicClass()
	{
		Release();
	}

	void Release()
	{
		on_damage=on_cluster=0;
		active=false;
	}

	bool Play(PLAY play, LPCSTR default_fname)
	{
		if(!terMusicEnable)
			return false;
		LPCSTR fname = gameShell->manualData().soundTracks[play].fileName();
		if(fname==NULL)
			fname=default_fname;

		if(fname)
			strMusic=fname;

		SetVolumeMusic( terMusicVolume );
		bool b=mpeg->OpenToPlay(fname,true);
		xassert_s(b && "Cannot open music: ",fname);
		return b;
	}

	void OpenWorld()
	{
		if(!terMusicEnable || active)return;
		active=true;
		on_damage=on_cluster=INT_MIN;
		Play(play = PLAY_CONSTRUCTION,"RESOURCE\\Music\\construction.ogg");
	}

	void CloseWorld()
	{
		if(!active)return;
		Release();
		mpeg->Stop();
	}

	void OnDamage()
	{
		on_damage=global_time();
	}

	void OnClusterIsOn()
	{
		on_cluster=global_time();
	}

	inline int change_time(){return round(music_change_time*1000);}

	void Quant()
	{
		if(!active)
			return;

		terPlayer* active_player = universe()->activePlayer();
		if(active_player && active_player->isFieldOn())
			OnClusterIsOn();

		if(global_time()-on_damage<change_time()){
			if(play!=PLAY_BATTLE){
				Play(play=PLAY_BATTLE,"RESOURCE\\Music\\battle.ogg");
			}
			on_cluster=INT_MIN;
		}
		else if(global_time()-on_cluster<change_time()){
			if(play!=PLAY_REGULAR){
				Play(play=PLAY_REGULAR,"RESOURCE\\Music\\regular.ogg");
			}
			on_damage=INT_MIN;
		}
		else{
			if(play!=PLAY_CONSTRUCTION){
				Play(play=PLAY_CONSTRUCTION, "RESOURCE\\Music\\construction.ogg");
			}
		}
	}
};

static MusicClass music;

void MusicOpenWorld()
{
	music.OpenWorld();
}

void MusicCloseWorld()
{
	music.CloseWorld();
}

void MusicOnDamage()
{
	music.OnDamage();
}

void MusicQuant()
{
	music.Quant();
}

SoundTrack::SoundTrack() 
{
	randomChoice = 0;
	index = 0;
}

const char* SoundTrack::fileName() const 
{ 
	int i = (randomChoice ? terEffectRND() : index++) % fileNames.size();
	return fileNames[i].c_str(); 
}
