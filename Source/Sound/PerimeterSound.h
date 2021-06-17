#pragma once

#ifdef PERIMETER_EXODUS

//Dummy implementation

//Инициализация/деинициализация библиотеки
bool SNDInitSound(HWND g_hWnd,bool bEnable3d,bool soft3d) { return true; }
void SNDReleaseSound() {}
void* SNDGetDirectSound() { return nullptr; } //Возвращает указатель на LPDIRECTSOUND8

void SNDEnableSound(bool enable) {}
void SNDEnableVoices(bool enable) {}
bool SNDIsVoicesEnabled() { return true; }

static char sound_directory[260]="";
void SNDSetSoundDirectory(LPCSTR dir)
{
    strncpy(sound_directory,dir,sizeof(sound_directory));
    sound_directory[sizeof(sound_directory)-1]=0;
}
LPCSTR SNDGetSoundDirectory()
{
    return sound_directory;
}

void SNDSetLocDataDirectory(LPCSTR dir) {}
void SNDSetBelligerentIndex(int idx) {}

//Работа с ошибками
bool SNDEnableErrorLog(LPCSTR file) { return true; }

void SNDSetVolume(float volume) {} //volume=0..1

//Грузит соответстивующую группу звуков в память
//и позволяет к ним обращаться с помощью
bool SNDScriptPrmEnableAll() { return true; }

//Функции сделанные для вызова менюшек
//Вполне естественно, что внутри нежелательно вызывать
//SNDScriptDisable, то есть можно, но только для тех скриптов, что не
//исполдьзуются, иначе звуки не запустятся опыть

void SNDPausePush() {} //Остановить все звуки, c возможностью продолжения проигрывания
void SNDPausePop() {} //Продолжить играть все остановленные звуки
int SNDGetPushLevel() { return true; } //Возвращает уровень вложенности

void SNDStopAll() {} //Остановить все звуки

////////////////////////////3D/////////////////////////////////


class SND3DSound
{
public:
    SND3DSound() = default;
    ~SND3DSound() = default;
    bool Init(LPCSTR name) { return true; }

    bool Play(bool cycled=true) { return true; }
    bool Stop() { return true; }
    bool IsPlayed() { return true; }

    void SetPos(const Vect3f& pos) {} //Обязательно вызвать до Play
    void SetVelocity(const Vect3f& velocity) {}
    void SetVolume(float vol) {} //0..1 учитывает volmin и volume


    //ScriptFrequency - установить относительную
    bool SetFrequency(float frequency) { return true; }//0..2 - 0 - минимальная, 1 - по умолчанию

    //SetFrequency - frequency=1..44100 Гц, оригинальная - 0
    void SetRealVolume(float vol) {} //0..1
};

bool SND3DPlaySound(LPCSTR name,
                    const Vect3f* pos,
                    const Vect3f* velocity=NULL//По умолчанию объект считается неподвижным
) {
    return true;
}

class SND3DListener
{
protected:
    friend struct SNDOneBuffer;
    friend class SoftSound3D;
    friend class SND3DSound;

    Vect3f position;

public:
    SND3DListener() = default;
    ~SND3DListener() = default;

    //Параметры изменяемые редко (скорее всего их менять и устанавливать не придётся никогда)
    //К тому-же они не работают (уж не знаю по какой причине)
    bool SetDistanceFactor(float) { return true; }//1 - в метрах, 1000 - в километрах
    bool SetDopplerFactor(float) { return true; }//0..10, по умолчанию 1
    bool SetRolloffFactor(float) { return true; }

    //SetPos надо изменять каждый кадр
    bool SetPos(const MatXf& mat) { return true; }

    Vect3f GetPos() {return position;};

    //SetVelocity - желательно изменять каждый кадр
    //иначе не будет смысла в SetDopplerFactor,SetRolloffFactor
    bool SetVelocity(const Vect3f& velocity) { return true; }

    //Функция специально для Рубера
    //Что-бы расстояние по Z было меньше.
    //в реальном времени криво работает\
	//zmul=0..1
    void SetZMultiple(float zmul) {}

    //Update - Вызывать после установки параметров (SetPos,...)
    //(один раз на кадр!)
    bool Update() { return true; }
};

extern SND3DListener snd_listener;

////////////////////////////2D/////////////////////////////////

//volume : 0 - миниум, 1 - максиум
//pan : 0 - крайне левое положение, 0.5 - центр, +1 - крайне правое
//Для звуков, которые должны проиграться один раз
//и играться в одном месте
bool SND2DPlaySound(LPCSTR name,float x=0.5f,DWORD frequency=0) { return true; }

//Устанавливает параметры влияющие на SND2DPanByX
//width - ширина экрана, power - влияет на то,
//насколько крайне правая точка будет звучать в левом наушнике
//power=1 - максимальное разнесение, power=0 - минимальное
void SND2DPanByX(float width,float power) {}

class SND2DSound
{
public:
    SND2DSound();
    ~SND2DSound();
    bool Init(LPCSTR) { return true; }

    bool Play(bool cycled=true) { return true; }
    bool Stop() { return true; }
    bool IsPlayed() const { return true; }

    bool SetPos(float x) { return true; } //Обязательно вызвать до Play
    bool SetFrequency(float frequency) { return true; } //0..2 - 0 - минимальная, 1 - по умолчанию
    void SetVolume(float vol) {} //0..1

    ////
    void SetRealVolume(float vol) {} //0..1
};

#else //PERIMETER_EXODUS

//Инициализация/деинициализация библиотеки
bool SNDInitSound(HWND g_hWnd,bool bEnable3d,bool soft3d);
void SNDReleaseSound();
void* SNDGetDirectSound();//Возвращает указатель на LPDIRECTSOUND8

void SNDEnableSound(bool enable);
void SNDEnableVoices(bool enable);
bool SNDIsVoicesEnabled();

void SNDSetSoundDirectory(LPCSTR dir);
LPCSTR SNDGetSoundDirectory();

void SNDSetLocDataDirectory(LPCSTR dir);
void SNDSetBelligerentIndex(int idx);

//Работа с ошибками
bool SNDEnableErrorLog(LPCSTR file);

void SNDSetVolume(float volume);//volume=0..1

//Грузит соответстивующую группу звуков в память 
//и позволяет к ним обращаться с помощью 
bool SNDScriptPrmEnableAll();

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
protected:
	inline void AssertValid();
	inline void Destroy();
};

#endif //PERIMETER_EXODUS
