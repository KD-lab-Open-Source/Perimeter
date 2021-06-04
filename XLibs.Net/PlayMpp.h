#ifndef _PLAYMPP_H_
#define _PLAYMPP_H_
#pragma once
//Balmer

#include <dsound.h>

#define MPP_STAT
#ifdef MPP_STAT
double MpegCPUUsing();//Возвращает используемое на проигрывание Mpeg время (1 - всё время загрузки)
#endif MPP_STAT

//
bool MpegInitLibrary(void* LPDIRECTSOUND_pDS);
void MpegDeinitLibrary();

enum MpegState
{
	MPEG_STOP=0,
	MPEG_PLAY=1,
	MPEG_PAUSE=2,
};

typedef const char* (*MpegFuncGetNextFile)();

//Возвращает длинну в секундах (но не очень точно, 
//может ошибаться на 26 мс)
//Хоть эта фонкция и существует, пользоваться ей не 
//рекомендуется. В правильно написанном коде игры 
//такая функция не нужна
double MpegGetLen(const char* fname);

#pragma pack(push,4)
struct MpegPos
{
	DWORD PrevBitsRead;
	DWORD N;
	DWORD NewBeginSample;//Сэмпл, на который необходимо спозиционироваться
};
#pragma pack(pop)

class MpegSound
{
	class MppLoad* pMppLoad;
	MpegSound *prev,*next;

	DWORD sizeDSBuffer;
	bool b_cycled;
	long volume;
	LPDIRECTSOUNDBUFFER pDSBuffer;
	DWORD dwWriteOffset;
	DWORD Wraps;//Сколько раз был записан звуковой буффер
	DWORD BeginBufferOffset;
	DWORD OldWraps,OldBeginBufferOffset;
	DWORD OffsetBeginPlayFile;//С какого места начал играться файл

	bool bOldFrame;//Установлен, если играются старые данные, а пишутся новые

	char fname[260];

	bool clear_end_buffer;
	MpegState mpeg_state;

	MpegFuncGetNextFile pMpegFuncGetNextFile;

	bool b_deferred_seek;
	DWORD deferred_prev_sample;
	DWORD SeekSkipByte;//Сколько байт необходимо пропустить с начала фрэйма
	MpegPos deferred_sample;
	char deferred_fname[260];

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
	//Не играть музыку, если громкость музыки равна 0
	//(по умолчанию музыка играется)
	void SetPauseIfNullVolume(bool set=true);

	MpegState IsPlay();

	bool DebugRealPlay();//Действительно ли проигрывается музыка

	void SetVolume(int volume);//0..255
	int GetVolume();

	inline int GetSamplePerSecond(){return 44100;}

	//Всё связанное с временем оперирует в сэмплах

	//Возвращает длинну в сэмплах (но не очень точно, 
	//может ошибаться на 1152 сэмпла,26 мс в большую сторону)
	//-1 - ошибка
	int GetLen();

	//GetCurPos - возвращает текущее положение
	//если *prevfile==true, то доигрывается предыдущий файл,
	//но уже начал распаковываться текущий
	int GetCurPos(bool* prevfile=NULL);

	//GetCurPos - возвращает текущее положение записи в буффер
	//если *prevfile==true, то доигрывается предыдущий файл,
	//но уже начал распаковываться текущий
	int GetCurWritePos(bool* prevfile=NULL);

	//Возвращает величину буфера в сэмплах
	inline int GetBufferLen(){return sizeDSBuffer/4;}

	//Так как распаковка буфера происходит не мгновенно
	//то SetGetNextFile,DeferredSeek
	//должны вызываться заранее  -
	//приблизительно за GetBufferLen() секунд.
	//!!! Все callback функции вызываются из другого потока

	//Когда до конца файла остаётся некоторое время
	//и была установленна функция MpegFuncGetNextFile,
	//то она запускается для того, чтобы узнать,
	//какой файл будет играться следующим
	void SetGetNextFile(MpegFuncGetNextFile p);

	//sample - количество сэмплов от начала файла, куда переместится.
	//prev_sample - сэмпл, после которого необходимо переместиться.
	//name - файл, который необходимо начать играть.
	//этот сэмпл должен быть ещё не проигранным
	//prev_sample==-1 - переместиться после следующего фрейма
	//не рекомендуется делать Seek на последнюю секунду файла.
	//Даже если эта функция возвратила true, это не значит,
	//что операция будет выполнена.
	//Если prev_sample<GetCurWritePos; //GetCurPos()+GetBufferLen(),
	//то функция вернёт false.
	//Эта функция не проверяет наличие и корректность файла fname.
	bool DeferredSeek(MpegPos seek_sample,int prev_sample,LPCSTR fname=NULL);

	//Вызывает DeferredSeek, выставляя prev_sample минимальным
	bool Seek(MpegPos seek_sample,LPCSTR fname=NULL);

	//Была принята к исполнению, но ещё не 
	//выполнена DeferredSeek,
	//так-же очищаются при вызове OpenToPlay
	bool IsDeferredSeek();

	static MpegPos null_sample;//Сэмпл начала файла

	//Постепенно изменить громкость с текущей до new_volume за время time
	//очищается при смене файла или вызове SetVolume
	//Криво работает с DeferredSeek
	bool FadeVolume(float time,int new_volume=0);
protected:
	void InternalMpegSetVolume(int _volume);
	bool InitSoundBuffer();
	void InternalMpegStop();
	bool InternalMpegOpenToPlay(const char* _fname,bool cycled);
	bool InternalDeferredSeek(MpegPos seek_sample,int prev_sample,LPCSTR fname);

	int InternalGetCurPos(DWORD dwPlayCursor,bool* prevfile);//Возвращает значение в байтах
	int InternalGetCurPos(bool* prevfile);//Возвращает значение в байтах

	void TimeCallbackTrue();

	//В какое место буфера надо будет писать, 
	//если в него записали offset байт
	void AddWriteOffset(DWORD offset);

	void PostSeek();//Вызывается по окончании процесса Seek

	friend DWORD WINAPI MpegThreadProc(LPVOID lpParameter);
	friend void MpegDeinitLibrary();

	bool DefferredSeek(DWORD cur_pos,DWORD cur_write_byte);

	void ClearFade();
	void FadeQuant();
};

//Этот класс является отдельным от MpegSound, так как 
//знание позиции часто нужно до открытия файла,
//да и само открытие файла mpeg при потоковых операциях
//может наступить не совсем тогда, когда нужно.
class MpegSamplePoint
{
	int mpegsize;
	MpegPos* mpegpos;
	void Destroy();
public:
	MpegSamplePoint();
	~MpegSamplePoint();
	bool Load(LPCSTR fname);
	inline int size(){return mpegsize;}
	const MpegPos& operator[](int pos){return mpegpos[pos];};

};

#endif // _PLAYMPP_H_
