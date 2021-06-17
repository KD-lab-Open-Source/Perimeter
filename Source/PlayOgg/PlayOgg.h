#ifndef _PLAYMPP_H_
#define _PLAYMPP_H_
#pragma once
//Balmer

enum MpegState
{
    MPEG_STOP=0,
    MPEG_PLAY=1,
    MPEG_PAUSE=2,
};

#ifdef PERIMETER_EXODUS

//Dummy implementation

bool MpegInitLibrary(void*) {}
void MpegDeinitLibrary() {}

class MpegSound {
public:
    MpegSound() = default;
    ~MpegSound() = default;

    bool OpenToPlay(const char*, bool cycled=true) { return true; }
    void Stop() {}
    void Pause() {}
    void Resume() {}

    const char* GetFileName() { return nullptr; }

    MpegState IsPlay() { return MPEG_STOP; }

    void SetVolume(int) {}
    int GetVolume() { return 0; }

    //Время считается в секундах

    //Постепенно изменить громкость с текущей до new_volume за время time
    //очищается при смене файла или вызове SetVolume
    //Криво работает с DeferredSeek
    bool FadeVolume(float, int new_volume=0) { return false; }

    float GetLen() { return 0.0f; }
};

#else //PERIMETER_EXODUS

#include <dsound.h>

#define MPP_STAT

bool MpegInitLibrary(void* LPDIRECTSOUND_pDS);
void MpegDeinitLibrary();

class MpegSound
{
	class MppLoad* pMppLoad;
	MpegSound *prev,*next;

	DWORD sizeDSBuffer;
	bool b_cycled;
	long volume;

	WAVEFORMATEX  wave_format;
	LPDIRECTSOUNDBUFFER pDSBuffer;
	DWORD dwWriteOffset;
	DWORD Wraps;//Сколько раз был записан звуковой буффер
	DWORD OldWraps;

	bool bOldFrame;//Установлен, если играются старые данные, а пишутся новые

	char fname[260];

	bool clear_end_buffer;
	MpegState mpeg_state;

	DWORD SeekSkipByte;//Сколько байт необходимо пропустить с начала фрэйма

	enum {block_size=36*32};//block_size==BLK_SIZE
	short last_signal[block_size*2];
	bool last_signal_is_full;
	int  last_signal_offset;

	void HammingCorrect(short* pData,int len);

	bool enable_fade;
	double fade_begin_time,fade_time;
	long fade_begin_volume,fade_end_volume;
public:
	MpegSound();
	~MpegSound();

	bool OpenToPlay(const char* fname,bool cycled=true);
	void Stop();
	void Pause();
	void Resume();

	const char* GetFileName();

	MpegState IsPlay();

	void SetVolume(int volume);//0..255
	int GetVolume();

	//Время считается в секундах

	//Постепенно изменить громкость с текущей до new_volume за время time
	//очищается при смене файла или вызове SetVolume
	//Криво работает с DeferredSeek
	bool FadeVolume(float time,int new_volume=0);

	float GetLen();//в секундах
protected:
	void InternalMpegSetVolume(int _volume);
	bool InitSoundBuffer();
	void InternalMpegStop();
	bool InternalMpegOpenToPlay(const char* _fname,bool cycled);

	void TimeCallbackTrue();

	//В какое место буфера надо будет писать, 
	//если в него записали offset байт
	void AddWriteOffset(DWORD offset);

	friend DWORD WINAPI MpegThreadProc(LPVOID lpParameter);
	friend void MpegDeinitLibrary();

	void ClearFade();
	void FadeQuant();
};

#endif //PERIMETER_EXODUS

#endif // _PLAYMPP_H_
