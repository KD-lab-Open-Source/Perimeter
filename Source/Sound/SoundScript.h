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

	template<class Archive>
	void serialize(Archive& ar) {
		ar & TRANSLATE_OBJECT(enable, "&включить");
		ar & TRANSLATE_OBJECT(random, "выбирать частоту звука случайно при его запуске");
		ar & TRANSLATE_OBJECT(up, "Во сколько раз частота звука может увеличиваться (0..2)");
		ar & TRANSLATE_OBJECT(down, "Во сколько раз частота звука может увеличиться (0..20)");
	}
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

	template<class Archive>
	void serialize(Archive& ar) {
		ar & TRANSLATE_OBJECT(name, "&name");
		ar & TRANSLATE_OBJECT(volume, "Громкость звука 0..1 (По умолчанию: 1)");
		ar & TRANSLATE_OBJECT(volmin, "Минимальная громкость звука 0..1 (По умолчанию: 0)");
	
		ar & TRANSLATE_OBJECT(language_dependency, "звук разный для разных языков");
		ar & TRANSLATE_OBJECT(belligerent_dependency, "звук разный для разных воюющих сторон");

		ar & TRANSLATE_OBJECT(soundName, "Название звука");
		ar & TRANSLATE_OBJECT(additionalNames, "Дополнительные имена для RND");

		ar & TRANSLATE_OBJECT(radius, "Величина объекта (только в 3D) с этого момента громкость звука начинает убывать (По умолчанию: 50)");
		ar & TRANSLATE_OBJECT(max_radius, "Максимальное расстояние до объекта, после которого звук перестаёт быть слышимым (По умолчанию: oo)");

		ar & TRANSLATE_OBJECT(max_num_sound, "Максимальное количество одновременно звучащих звуков (По умолчанию: 5)");

		ar & TRANSLATE_OBJECT(delta, "Дельта");
	}
};

struct SoundScriptPrm
{
	bool is3D; // 1 если звуки трёхмерные
	PrmString name;
	vector<SoundSetupPrm> data;

	SoundScriptPrm() {
		is3D = false; 
	}

	template<class Archive>
	void serialize(Archive& ar) {
		ar & TRANSLATE_OBJECT(name, "&name");
		ar & TRANSLATE_OBJECT(is3D, "звуки трёхмерные");
		ar & TRANSLATE_OBJECT(data, "звуки");
	}
};

struct SoundScriptTable
{
	// Префикс имени файла для звуков, которые разные для разных воюющих сторон.
	vector<PrmString> belligerentPrefix;
	vector<SoundScriptPrm> table;

	template<class Archive>
	void serialize(Archive& ar) {
		ar & TRANSLATE_OBJECT(table, "table");
		ar & TRANSLATE_OBJECT(belligerentPrefix, "Префикс имени файла для звуков, которые разные для разных воюющих сторон");
	}
};

extern SingletonPrm<SoundScriptTable> soundScriptTable;

#endif //__SOUND_SCRIPT_H__