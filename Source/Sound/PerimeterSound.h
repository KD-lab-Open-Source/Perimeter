#pragma once

//Инициализация/деинициализация библиотеки
bool SNDInitSound(HWND g_hWnd,bool bEnable3d,bool soft3d);
void SNDReleaseSound();
void* SNDGetDirectSound();//Возвращает указатель на LPDIRECTSOUND8

void SNDEnableSound(bool enable);
void SNDEnableVoices(bool enable);
bool SNDIsVoicesEnabled();
bool SNDIsSoundEnabled();

void SNDSetSoundDirectory(LPCSTR dir);
LPCSTR SNDGetSoundDirectory();

void SNDSetLocDataDirectory(LPCSTR dir);
void SNDSetBelligerentIndex(int idx);

//Работа с ошибками
bool SNDEnableErrorLog(LPCSTR file);

void SNDSetVolume(float volume);//volume=0..1
float SNDGetVolume();

//Грузит соответстивующую группу звуков в память 
//и позволяет к ним обращаться с помощью 
bool SNDScriptPrmEnableAll();
//Очищает память, отведённую под соответствующую группу звуков
bool SND3DScriptDisable(LPCSTR name);
void SND3DScriptDisableAll();
bool SND2DScriptDisable(LPCSTR name);
void SND2DScriptDisableAll();

//Функции сделанные для вызова менюшек
//Вполне естественно, что внутри нежелательно вызывать 
//SNDScriptDisable, то есть можно, но только для тех скриптов, что не 
//исполдьзуются, иначе звуки не запустятся опыть

void SNDPausePush();//Остановить все звуки, c возможностью продолжения проигрывания 
void SNDPausePop();//Продолжить играть все остановленные звуки
int SNDGetPushLevel();//Возвращает уровень вложенности

void SNDStopAll();//Остановить все звуки

////////////////////////////3D/////////////////////////////////


class SND3DSound
{
	struct ScriptParam* script;
	int cur_buffer;
public:
	SND3DSound();
	~SND3DSound();
	bool Init(LPCSTR name);

	bool Play(bool cycled=true);
	bool Stop();
	bool IsPlayed();
	
	void SetPos(const Vect3f& pos);//Обязательно вызвать до Play
	void SetVelocity(const Vect3f& velocity);
	void SetVolume(float vol);//0..1 учитывает volmin и volume


	//ScriptFrequency - установить относительную 
	bool SetFrequency(float frequency);//0..2 - 0 - минимальная, 1 - по умолчанию

	//SetFrequency - frequency=1..44100 Гц, оригинальная - 0
	bool SetRealFrequency(DWORD frequency);
	void SetRealVolume(float vol);//0..1
protected:
	inline void AssertValid();
	void Destroy();
};

bool SND3DPlaySound(LPCSTR name,
					const Vect3f* pos,
					const Vect3f* velocity=NULL//По умолчанию объект считается неподвижным
					);

class SND3DListener
{
protected:
	friend struct SNDOneBuffer;
	friend class SoftSound3D;
	friend class SND3DSound;

	Mat3f rotate,invrotate;
	Vect3f position;
	//MatXf mat;
	Vect3f velocity;

	//Дупликаты для software
	float s_distance_factor;
	float s_doppler_factor;
	float s_rolloff_factor;

	Vect3f front,top,right;

	float zmultiple;
public:
	SND3DListener();
	~SND3DListener();

	//Параметры изменяемые редко (скорее всего их менять и устанавливать не придётся никогда)
	//К тому-же они не работают (уж не знаю по какой причине)
	bool SetDistanceFactor(float);//1 - в метрах, 1000 - в километрах
	bool SetDopplerFactor(float);//0..10, по умолчанию 1
	bool SetRolloffFactor(float);

	//SetPos надо изменять каждый кадр
	bool SetPos(const MatXf& mat);

	Vect3f GetPos(){return position;};

	//SetVelocity - желательно изменять каждый кадр
	//иначе не будет смысла в SetDopplerFactor,SetRolloffFactor
	bool SetVelocity(const Vect3f& velocity);

	//Функция специально для Рубера
	//Что-бы расстояние по Z было меньше.
	//в реальном времени криво работает\
	//zmul=0..1
	void SetZMultiple(float zmul){zmultiple=zmul;};

	//Update - Вызывать после установки параметров (SetPos,...)
	//(один раз на кадр!)
	bool Update();
};

extern SND3DListener snd_listener;

////////////////////////////2D/////////////////////////////////

//volume : 0 - миниум, 1 - максиум
//pan : 0 - крайне левое положение, 0.5 - центр, +1 - крайне правое
//Для звуков, которые должны проиграться один раз
//и играться в одном месте
bool SND2DPlaySound(LPCSTR name,float x=0.5f,DWORD frequency=0);

//Устанавливает параметры влияющие на SND2DPanByX
//width - ширина экрана, power - влияет на то, 
//насколько крайне правая точка будет звучать в левом наушнике
//power=1 - максимальное разнесение, power=0 - минимальное
void SND2DPanByX(float width,float power);

class SND2DSound
{
	struct ScriptParam* script;
	int cur_buffer;
public:
	SND2DSound();
	~SND2DSound();
	bool Init(LPCSTR name);

	bool Play(bool cycled=true);
	bool Stop();
	bool IsPlayed() const;
	
	bool SetPos(float x);//Обязательно вызвать до Play
	bool SetFrequency(float frequency);//0..2 - 0 - минимальная, 1 - по умолчанию
	void SetVolume(float vol);//0..1

	////
	void SetRealVolume(float vol);//0..1
	bool SetRealFrequency(DWORD frequency);
protected:
	inline void AssertValid();
	inline void Destroy();
};

