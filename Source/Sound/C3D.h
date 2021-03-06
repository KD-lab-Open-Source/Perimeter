#pragma once

struct ScriptParam;
#include "SoftwareBuffer.h"
#include "SoundInternal.h"

struct SNDOneBuffer
{
	ScriptParam* script;

	Vect3f pos,velocity;
    float volume;
    float frequency;

    SND_Sample* buffer;
	uint32_t nSamplesPerSec;//Частота сэмпла см. WAVEFORMATEX

	VirtualSound3D* p3DBuffer;

	void* pSound;//SND2DSound или SND3DSound
	bool used,played_cycled;

	int pause_level;//Если не 0, то звук остановлен функцией SNDPausePush();

	double begin_play_time;

	SNDOneBuffer();
	~SNDOneBuffer();

	inline bool RecalculatePos();
	inline void RecalculateVolume();

	//Автоматическое задание нестандартной частоты
	void PlayPreprocessing();

	bool SetFrequency(float frequency);
};

struct ScriptParam
{
	char* sound_name;

	float radius;//
	float max_radius;
	int max_num_sound;
	float def_volume;
	float min_volume;

	bool delta_enable;
	bool delta_random;
	float delta_up,delta_down;

	bool language_dependency; // зависит от языка
	bool belligerent_dependency; // зависит от воюющей стороны


	ScriptParam():
		sound_name(NULL),
		def_volume(1.0f),
		delta_enable(false),
		language_dependency(false),
		belligerent_dependency(false)
	{};

	void LoadSound(const std::string& name);

	void Release();

	//Рассчитывает, какой должна быть ClipDistance, чтобы звучало не более max_num_sound
	void RecalculateClipDistance();

	MTSection* GetLock(){return &mtlock;};
	std::vector<SND_Sample*>& GetSounds(){ASSERT(mtlock.is_lock());return sounds;}
	std::vector<SNDOneBuffer>& GetBuffer(){ASSERT(mtlock.is_lock());return soundbuffer;}

	bool incBelligerentIndex(int idx){ xassert(idx >= 0 && idx < belligerentIndex_.size()); belligerentIndex_[idx]++; return true; }
	bool setBelligerentCount(int count){ belligerentIndex_.resize(count,0); return true; }
	bool setBelligerentIndex(int idx,int count){ xassert(idx >= 0 && idx < belligerentIndex_.size()); belligerentIndex_[idx] = count; return true; }
	int belligerentIndex(int idx) const { xassert(idx >= 0 && idx < belligerentIndex_.size()); return belligerentIndex_[idx]; }

protected:
	MTSection mtlock;
	//Звуки, которые выбираются при загрузке по RND
	std::vector<SND_Sample*> sounds;
	std::vector<SNDOneBuffer> soundbuffer;
	// количество звуков для разных воюющих сторон
	std::vector<int> belligerentIndex_;
};

struct SoundScriptPrm;

class SNDScript
{
	struct ltstr
	{
	  bool operator()(const char* s1, const char* s2) const
	  {
		return strcmp(s1, s2) < 0;
	  }
	};

	static int belligerentIndex;
	static std::string locDataPath;

public:
	typedef std::map<const char*, ScriptParam*, ltstr> MapScript;

	struct OneScript
	{
        char* name;
		//Один раз инициализировали, много раз использовали
		//не добавлять и не удалять а процессе работы
		std::vector<ScriptParam> sounds;

		OneScript():name(NULL){}
		~OneScript(){if(name)free(name);}
	};

	MapScript map_script;
	std::vector<OneScript*> map_map;

	bool b2d;
public:
	SNDScript(bool b2d);
	~SNDScript();

	bool AddScript(const struct SoundScriptPrm* prm);
	bool RemoveScript(const char* name);
	void RemoveAll();

	ScriptParam* Find(const char* name);
	bool FindFree(const char* name, ScriptParam*& script, int& nfree);

	void PauseAllPlayed(int pause_level);
	void PlayByLevel(int pause_level);

	void StopAllPlayed();
	void StopAllVoices();

	static void setBelligerentIndex(int idx){ belligerentIndex = idx; }
	static void setLocDataPath(const char* path){ locDataPath = path; }

protected:
	void StopAll(bool only_voices);

	void RebuildMapScript();
	void RemoveScriptVector(std::vector<ScriptParam>& vc);

	void ParseSoundScriptPrm(const struct SoundScriptPrm* prm,
		std::vector<ScriptParam>& vc);

	bool RemoveScriptInternal(const char* name);
	
	static std::string filePath(const ScriptParam* prm,const char* file_name,int belligerent_index = 0);
};

extern SNDScript script3d;
extern SNDScript script2d;

/////////////////////////inline//////////////////////////
