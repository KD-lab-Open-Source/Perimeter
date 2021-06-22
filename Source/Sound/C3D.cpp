#include "StdAfx.h"
#include "../Render/inc/RenderMT.h"
#include "C3D.h"
#include "SoundInternal.h"
#include "SoundScript.h"
#include <algorithm>

int SNDScript::belligerentIndex = 0;
string SNDScript::locDataPath;

using namespace SND;

SNDScript script3d(false);
SNDScript script2d(true);


SNDScript::SNDScript(bool _b2d)
:b2d(_b2d)
{
}

SNDScript::~SNDScript()
{
}


ScriptParam* SNDScript::Find(LPCSTR name)
{
	if(!name)
		return NULL;
	MapScript::iterator it=map_script.find(name);
	if(it==map_script.end())
	{
		logs("Sound not found:%s\n",name);
		return NULL;
	}
	return ((*it).second);
}



bool SNDScript::AddScript(const struct SoundScriptPrm* prm)
{
	RemoveScriptInternal(prm->name);

	OneScript* os=new OneScript;
	os->name=_strdup(prm->name);

	ParseSoundScriptPrm(prm,os->sounds);

	map_map.push_back(os);
	RebuildMapScript();

	return true;
}

bool SNDScript::RemoveScript(LPCSTR name)
{
	if(!RemoveScriptInternal(name))
		return false;
	RebuildMapScript();
	return true;
}

bool SNDScript::RemoveScriptInternal(LPCSTR name)
{
	vector<OneScript*>::iterator it;
	FOR_EACH(map_map,it)
	{
		if(strcmp((*it)->name,name)==0)
		{
			break;
		}
	}
	
	if(it==map_map.end())
		return false;

	RemoveScriptVector((*it)->sounds);
	delete *it;
	map_map.erase(it);

	return true;
}

void SNDScript::RemoveAll()
{
	map_script.clear();

	vector<OneScript*>::iterator it;
	FOR_EACH(map_map,it)
	{
		RemoveScriptVector((*it)->sounds);
		delete *it;
	}

	map_map.clear();
}


void SNDScript::RebuildMapScript()
{
	map_script.clear();

	vector<OneScript*>::iterator it_map;
	FOR_EACH(map_map,it_map)
	{
		vector<ScriptParam>& vc=(*it_map)->sounds;
		vector<ScriptParam>::iterator it;

		FOR_EACH(vc,it)
		{
			ScriptParam& sp=*it;
			map_script[sp.sound_name]=&sp;
		}
	}
}

void SNDScript::RemoveScriptVector(vector<ScriptParam>& vc)
{
	vector<ScriptParam>::iterator it;
	FOR_EACH(vc,it)
	{
		ScriptParam& sp=*it;
		sp.Release();
	}
}


void SNDScript::ParseSoundScriptPrm(const SoundScriptPrm* prm,vector<ScriptParam>& vc)
{
	for(int i = 0; i < prm->data.size(); i++){
		const SoundSetupPrm& snd = prm->data[i];
		ScriptParam sp;
		{
			MTAuto lock(sp.GetLock());
			sp.sound_name = _strdup(snd.name);
			sp.radius = snd.radius;
			sp.max_radius = (snd.max_radius >= 0) ? snd.max_radius : DS3D_DEFAULTMAXDISTANCE;

			sp.max_num_sound = snd.max_num_sound;

			sp.def_volume = snd.volume;
			sp.min_volume = snd.volmin;

			sp.language_dependency = snd.language_dependency;
			sp.belligerent_dependency = snd.belligerent_dependency;

			if(snd.delta.enable){
				sp.delta_enable = true;
				sp.delta_random = snd.delta.random;
				sp.delta_up = snd.delta.up;
				sp.delta_down = snd.delta.down;
			}
			else {
				sp.delta_up = 1.5f;
				sp.delta_down = 0.5f;
			}

			if(sp.belligerent_dependency){
				sp.setBelligerentCount(soundScriptTable().belligerentPrefix.size());
				for(int j = 0; j < soundScriptTable().belligerentPrefix.size(); j++){
					sp.setBelligerentIndex(j,sp.GetSounds().size());
					sp.LoadSound(filePath(&sp,snd.soundName,j));
					for(int k = 0; k < snd.additionalNames.size(); k++)
						sp.LoadSound(filePath(&sp,snd.additionalNames[k],j));
				}
			}
			else {
				sp.LoadSound(filePath(&sp,snd.soundName));
				for(int k = 0; k < snd.additionalNames.size(); k++)
					sp.LoadSound(filePath(&sp,snd.additionalNames[k]));
			}

			if(sp.GetSounds().empty()){
				free(sp.sound_name);
				continue;
			}

			vc.push_back(sp);
		}
	}
}


void SNDScript::StopAllVoices()
{
	StopAll(true);
}

void SNDScript::StopAllPlayed()
{
	StopAll(false);
}

void SNDScript::StopAll(bool only_voices)
{
	SNDScript::MapScript::iterator it;
	FOR_EACH(map_script,it)
	{
		ScriptParam* sp=(*it).second;
		MTAuto lock(sp->GetLock());
		if(only_voices && !sp->language_dependency)
			continue;
		
		vector<SNDOneBuffer>::iterator itb;
		FOR_EACH(sp->GetBuffer(),itb)
		{
			SNDOneBuffer& sb=*itb;
			LPDIRECTSOUNDBUFFER buffer=sb.buffer;
			DWORD status;
			if(buffer->GetStatus(&status)==DS_OK)
			{
				if(status&DSBSTATUS_PLAYING)
				{
					buffer->Stop();
					sb.pause_level=0;
				}
			}
		}
	}
}

void SNDScript::PauseAllPlayed(int pause_level)
{
	SNDScript::MapScript::iterator it;
	FOR_EACH(map_script,it)
	{
		ScriptParam* sp=(*it).second;
		MTAuto lock(sp->GetLock());
		vector<SNDOneBuffer>::iterator itb;
		FOR_EACH(sp->GetBuffer(),itb)
		{
			SNDOneBuffer& sb=*itb;
			LPDIRECTSOUNDBUFFER buffer=sb.buffer;
			DWORD status;
			if(buffer->GetStatus(&status)==DS_OK)
			{
				if(status&DSBSTATUS_PLAYING)
				{
					buffer->Stop();
					sb.pause_level=pause_level;
				}

				if(sb.p3DBuffer)
				{
					sb.p3DBuffer->Pause(true);
					sb.pause_level=pause_level;
				}
			}
		}
	}
}

void SNDScript::PlayByLevel(int pause_level)
{
	SNDScript::MapScript::iterator it;
	FOR_EACH(map_script,it)
	{
		ScriptParam* sp=(*it).second;
		MTAuto lock(sp->GetLock());
		vector<SNDOneBuffer>::iterator itb;
		FOR_EACH(sp->GetBuffer(),itb)
		{
			SNDOneBuffer& sb=*itb;
			LPDIRECTSOUNDBUFFER buffer=sb.buffer;
			if(sb.pause_level==pause_level)
			{
				if(sb.p3DBuffer)
					sb.p3DBuffer->Pause(false);
				else
					buffer->Play(0,0,sb.played_cycled?DSBPLAY_LOOPING:0);
				sb.pause_level=0;
			}
		}
	}
}

const char* SNDScript::filePath(const ScriptParam* prm,const char* file_name,int belligerent_index)
{
	static XBuffer fname(MAX_PATH);

	fname.init();

	if(!prm->language_dependency)
		fname < SNDGetSoundDirectory() < file_name < ".wav";
	else
		fname < locDataPath.c_str() < file_name < ".wav";

	if(prm->belligerent_dependency){
		char drive[_MAX_DRIVE];
   		char dir[_MAX_DIR];
		char name[_MAX_FNAME];
   		char ext[_MAX_EXT];

		_splitpath(fname.address(),drive,dir,name,ext);

		xassert(belligerent_index >= 0 && belligerent_index < soundScriptTable().belligerentPrefix.size());

		fname.init();
		fname < drive < dir < soundScriptTable().belligerentPrefix[belligerent_index] < name < ext;
	}

	return fname.address();
}

///////////SNDOneBuffer///////////////////////////////////
SNDOneBuffer::SNDOneBuffer()
{
	script=NULL;
	buffer=NULL;
	nSamplesPerSec=0;
	nAvgBytesPerSec=0;
	p3DBuffer=NULL;
	pSound=NULL;
	used=false;
	played_cycled=false;
	pause_level=0;
	begin_play_time=0;
	pos.set(0,0,0);
	velocity.set(0,0,0);
	volume=0;
}

SNDOneBuffer::~SNDOneBuffer()
{
}

bool SNDOneBuffer::SetFrequency(float frequency)
{
	if(script->delta_enable)
	{
		float dw=script->delta_up-script->delta_down;
		float mul=frequency*dw+script->delta_down;
		frequency=round(nSamplesPerSec*mul);

		if(p3DBuffer)
			return p3DBuffer->SetFrequency(frequency);
		else
			return buffer->SetFrequency(frequency);
	}

	return false;
}


void ScriptParam::LoadSound(const char* name)
{
	LPDIRECTSOUNDBUFFER buf=SNDLoadSound(name,
		DSBCAPS_STATIC|
		DSBCAPS_GETCURRENTPOSITION2|
		//DSBCAPS_LOCDEFER|
		DSBCAPS_CTRLVOLUME|
		DSBCAPS_CTRLFREQUENCY|DSBCAPS_CTRLPAN
		);

	if(buf == NULL)
		logs("Sound not loaded: %s\n",name);
	else
		GetSounds().push_back(buf);
}

void ScriptParam::Release()
{
	if(sound_name)free(sound_name);
	sound_name=NULL;

	vector<SNDOneBuffer>::iterator its;
	FOR_EACH(soundbuffer,its)
	{
		SNDOneBuffer& sb=*its;
		SAFE_RELEASE(sb.buffer);

		SAFE_DELETE(sb.p3DBuffer);
	}
	soundbuffer.clear();

	vector<LPDIRECTSOUNDBUFFER>::iterator itb;
	FOR_EACH(sounds,itb)
	{
		LPDIRECTSOUNDBUFFER p=*itb;
		if(p)
		{
			int num=p->Release();
			ASSERT(num==0);
		}
	}
	sounds.clear();
}

struct SNDOneBufferDistance
{
	SNDOneBuffer* p;
	float distance;

	inline bool operator()(const SNDOneBufferDistance& s1,const SNDOneBufferDistance& s2)const
	{
		return s1.distance<s2.distance;
	}
};

void ScriptParam::RecalculateClipDistance()
{
	vector<SNDOneBufferDistance> real_played;
	vector<SNDOneBuffer>::iterator its;
	if(max_num_sound<1)
		return;

	if(soundbuffer.size()<=max_num_sound)
		return;

	SNDOneBufferDistance s;
	FOR_EACH(soundbuffer,its)
	{
		SNDOneBuffer& p=*its;
		if(!p.used)continue;
		if(p.p3DBuffer==NULL)continue;

		if(!p.p3DBuffer->IsPlaying())
			continue;
		s.p=&p;
		s.distance=p.p3DBuffer->VectorToListener().norm();
		real_played.push_back(s);
	}

	int sz=real_played.size();
	if(sz<=max_num_sound)
		return;

	sort(real_played.begin(),real_played.end(),s);

	int i;
#ifdef _DEBUG
	for(i=0;i<sz-1;i++)
	{
		float d1=real_played[i].distance;
		float d2=real_played[i+1].distance;
		ASSERT(d1<=d2);
	}
#endif //_DEBUG
/*
	float clip_dist=(real_played[max_num_sound-1].distance+
					 real_played[max_num_sound].distance)*0.5f;

	FOR_EACH(soundbuffer,its)
	if(its->p3DBuffer)
		its->p3DBuffer->SetClipDistance(clip_dist);
/*/
	for(i=0;i<real_played.size();i++)
	{
		if(i<max_num_sound)
			real_played[i].p->p3DBuffer->SetClipDistance(FLT_MAX);
		else
			real_played[i].p->p3DBuffer->SetClipDistance(0);
	}
/**/

}

