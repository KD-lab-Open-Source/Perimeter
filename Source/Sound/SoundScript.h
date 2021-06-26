#ifndef __SOUND_SCRIPT_H__
#define __SOUND_SCRIPT_H__

#include "Handle.h"
#include "Serialization.h"

struct SoundDeltaPrm 
{
	bool enable;
	bool random;	// выбирать частоту звука случайно при его запуске
			
	float up; // Во сколько раз частота звука может увеличиваться (0..2)
	float down; //Во сколько раз частота звука может увеличиться (0..20)

	SoundDeltaPrm() {
		enable = false;
		random = false;	
		up = 1.5; 
		down = 0.5; 
	}

#include "SoundScript-1251-1.inl"
};

struct SoundSetupPrm
{
	PrmString name;
	float volume; // Громкость звука 0..1 (По умолчанию: 1)
	float volmin; // Минимальная громкость звука 0..1 (По умолчанию: 0)
	
	bool language_dependency; // 1 если звук разный для разных языков
	bool belligerent_dependency; // 1 если звук разный для разных воюющих сторон
	
	PrmString soundName; //Название звука 
	vector<PrmString> additionalNames; //Название звука (таких полей может быть несколько, тогда поле выбирается по rnd)
	float radius; //Величина объекта (только в 3D) с этого момента громкость звука начинает убывать (По умолчанию: 50)
	float max_radius; //Максимальное расстояние до объекта, после которого звук перестаёт быть слышимым (По умолчанию: oo)

	//Максимальное количество одновременно звучащих звуков (По умолчанию: 5)
	//В случае 2D при попытке загрузить больше, чем max_num_sound звуков - звук не запускается.
	//В случае 3D:
	//    если max_num_sound == 1, запускается только 1 звук, последующие не запускаются.
	//    если max_num_sound>1, запускается все звуки, но слышны только ближайшие.
	int max_num_sound;

	SoundDeltaPrm delta;

	SoundSetupPrm() {
		volume = 1.0; 
		volmin = 0.0; 
	
		language_dependency = false; 
		belligerent_dependency = false; 
	
		radius = 100; 
		max_radius = -1; 

		max_num_sound = 25;
	}

#include "SoundScript-1251-2.inl"
};

struct SoundScriptPrm
{
	bool is3D; // 1 если звуки трёхмерные
	PrmString name;
	vector<SoundSetupPrm> data;

	SoundScriptPrm() {
		is3D = false; 
	}

#include "SoundScript-1251-3.inl"
};

struct SoundScriptTable
{
	// Префикс имени файла для звуков, которые разные для разных воюющих сторон.
	vector<PrmString> belligerentPrefix;
	vector<SoundScriptPrm> table;

#include "SoundScript-1251-4.inl"
};

extern SingletonPrm<SoundScriptTable> soundScriptTable;

DEFINE_SINGLETON_PRM(SoundScriptTable)

#endif //__SOUND_SCRIPT_H__