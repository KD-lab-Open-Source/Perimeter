#include "StdAfxSound.h"
#include "../Render/inc/RenderMT.h"
#include "C3D.h"
#include "SoundInternal.h"
#include "SoundScript.h"
#include "Sample.h"
#include <cfloat>
#include <algorithm>
#include "files/files.h"

int SNDScript::belligerentIndex = 0;
std::string SNDScript::locDataPath;

using namespace SND;

SNDScript script3d(false);
SNDScript script2d(true);


SNDScript::SNDScript(bool _b2d)
:b2d(_b2d)
{
}

SNDScript::~SNDScript()
{
    RemoveAll();
}


ScriptParam* SNDScript::Find(const char* name)
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
	os->name=strdup(prm->name);

	ParseSoundScriptPrm(prm,os->sounds);

	map_map.push_back(os);
	RebuildMapScript();

	return true;
}

bool SNDScript::RemoveScript(const char* name)
{
	if(!RemoveScriptInternal(name))
		return false;
	RebuildMapScript();
	return true;
}

bool SNDScript::RemoveScriptInternal(const char* name)
{
	std::vector<OneScript*>::iterator it;
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

	std::vector<OneScript*>::iterator it;
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

	std::vector<OneScript*>::iterator it_map;
	FOR_EACH(map_map,it_map)
	{
		std::vector<ScriptParam>& vc=(*it_map)->sounds;
		std::vector<ScriptParam>::iterator it;

		FOR_EACH(vc,it)
		{
			ScriptParam& sp=*it;
			map_script[sp.sound_name]=&sp;
		}
	}
}

void SNDScript::RemoveScriptVector(std::vector<ScriptParam>& vc)
{
	std::vector<ScriptParam>::iterator it;
	FOR_EACH(vc,it)
	{
		ScriptParam& sp=*it;
		sp.Release();
	}
}


void SNDScript::ParseSoundScriptPrm(const SoundScriptPrm* prm, std::vector<ScriptParam>& vc)
{
	for(int i = 0; i < prm->data.size(); i++){
		const SoundSetupPrm& snd = prm->data[i];
		ScriptParam sp;
		{
			MTAuto lock(sp.GetLock());
			sp.sound_name = strdup(snd.name);
			sp.radius = snd.radius;
			sp.max_radius = (snd.max_radius >= 0) ? snd.max_radius : std::numeric_limits<float>::infinity();

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
		
		std::vector<SNDOneBuffer>::iterator itb;
		FOR_EACH(sp->GetBuffer(),itb)
		{
			SNDOneBuffer& sb=*itb;
            if (sb.buffer->isPlaying()) {
                sb.buffer->stop();
                sb.pause_level=0;
			}
		}
	}
}

void SNDScript::PauseAllPlayed(int pause_level)
{    
	SNDScript::MapScript::iterator it;
	FOR_EACH(map_script,it) {
		ScriptParam* sp=(*it).second;
		MTAuto lock(sp->GetLock());
		std::vector<SNDOneBuffer>::iterator itb;
		FOR_EACH(sp->GetBuffer(),itb) {
			SNDOneBuffer& sb=*itb;
            if (sb.buffer) {
                if (sb.buffer->isPlaying()) {
                    sb.buffer->pause();
                    sb.pause_level = pause_level;
                }

                if (sb.p3DBuffer) {
                    sb.p3DBuffer->Pause(true);
                    sb.pause_level = pause_level;
                }
            }
		}
	}
}

void SNDScript::PlayByLevel(int pause_level)
{    
	SNDScript::MapScript::iterator it;
	FOR_EACH(map_script,it) {
		ScriptParam* sp=(*it).second;
		MTAuto lock(sp->GetLock());
		std::vector<SNDOneBuffer>::iterator itb;
		FOR_EACH(sp->GetBuffer(),itb) {
			SNDOneBuffer& sb=*itb;
			if(sb.pause_level == pause_level && sb.buffer) {
				if(sb.p3DBuffer) {
                    sb.p3DBuffer->Pause(false);
                } else {
                    sb.buffer->resume();
                }
				sb.pause_level=0;
			}
		}
	}
}

std::string SNDScript::filePath(const ScriptParam* prm,const char* file_name,int belligerent_index)
{
	std::string path;

	if(!prm->language_dependency) {
        path = SNDGetSoundDirectory() + file_name + ".wav";
    } else {
        path = locDataPath + file_name + ".wav";
    }

	if(prm->belligerent_dependency){
		std::string path_parent, path_filename;
        split_path_parent(path, path_parent, &path_filename);

		xassert(belligerent_index >= 0 && belligerent_index < soundScriptTable().belligerentPrefix.size());

        path = path_parent + PATH_SEP + soundScriptTable().belligerentPrefix[belligerent_index].value() + path_filename;
	}

	return path;
}

///////////SNDOneBuffer///////////////////////////////////
SNDOneBuffer::SNDOneBuffer()
{
	script=NULL;
	buffer=NULL;
	nSamplesPerSec=0;
	p3DBuffer=NULL;
	pSound=NULL;
	used=false;
	played_cycled=false;
	pause_level=0;
	begin_play_time=0;
	pos.set(0,0,0);
	velocity.set(0,0,0);
	volume=0;
    frequency=0;
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

		if(p3DBuffer)
			return p3DBuffer->SetFrequency(mul);
		else
			this->frequency = mul;
	}

	return false;
}


void ScriptParam::LoadSound(const std::string& name)
{
    SND_Sample* sample=SNDLoadSound(name);

	if (sample == nullptr) {
        logs("Sound not loaded: %s\n", name.c_str());
    } else {
        sample->channel_group = SND_GROUP_EFFECTS;
        GetSounds().push_back(sample);
    }
}

void ScriptParam::Release()
{
	if(sound_name)free(sound_name);
	sound_name=NULL;

	std::vector<SNDOneBuffer>::iterator its;
	FOR_EACH(soundbuffer,its)
	{
		SNDOneBuffer& sb=*its;
		SAFE_DELETE(sb.buffer);
		SAFE_DELETE(sb.p3DBuffer);
	}
	soundbuffer.clear();

    for (auto sound : sounds) {
        SAFE_DELETE(sound)
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
	std::vector<SNDOneBufferDistance> real_played;
	std::vector<SNDOneBuffer>::iterator its;
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

	std::sort(real_played.begin(),real_played.end(),s);

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

