#pragma once

class VirtualSound3D
{
protected:
	LPDIRECTSOUNDBUFFER pSound;
public:
	VirtualSound3D():pSound(NULL){};
	virtual ~VirtualSound3D(){};
	
	virtual bool Init(LPDIRECTSOUNDBUFFER ptr);

	virtual float GetMaxDistance()=0;
	virtual float GetMinDistance()=0;
	virtual bool SetMaxDistance(float)=0;
	virtual bool SetMinDistance(float)=0;
	virtual Vect3f GetPosition()=0;
	virtual bool SetPosition(const Vect3f& pos)=0;
	virtual Vect3f GetVelocity()=0;
	virtual bool SetVelocity(const Vect3f& vel)=0;

	virtual bool IsPlaying()=0;
	virtual bool Play(bool cycled)=0;
	virtual bool Stop()=0;
	virtual bool SetFrequency(DWORD dwFrequency)=0;

	virtual bool SetVolume(float vol)=0;//vol=0..1

	virtual void RecalculatePos()=0;
	virtual void RecalculateVolume()=0;

	virtual Vect3f VectorToListener()=0;

	virtual void SetClipDistance(float clip_distance)=0;

	virtual void Pause(bool p)=0;
};

class SoftSound3D:public VirtualSound3D
{
	Vect3f position;
	Vect3f velocity;
	float min_distance,max_distance;
	float clip_distance;//после неё звук перестаёт звучать

	bool is_playing,is_cycled;
	bool pause;
	float volume;
	float set_volume;

	double begin_play_sound,last_start_stop;

	DWORD BytePerSample;
	DWORD nSamplesPerSec,//frequency==nSamplesPerSec
		nAvgBytesPerSec,dwBufferBytes;

	DWORD RealFrequency;

	DWORD GetCurPos(double curtime);
public:
	SoftSound3D();
	~SoftSound3D();

	bool Init(LPDIRECTSOUNDBUFFER ptr);

	float GetMaxDistance();
	float GetMinDistance();
	bool SetMaxDistance(float);
	bool SetMinDistance(float);
	Vect3f GetPosition();
	bool SetPosition(const Vect3f& pos);
	Vect3f GetVelocity();
	bool SetVelocity(const Vect3f& vel);

	bool IsPlaying();
	bool Play(bool cycled);
	bool Stop();
	bool SetFrequency(DWORD dwFrequency);
	bool SetVolume(float vol);

	void RecalculatePos();
	void RecalculateVolume();

	Vect3f VectorToListener();
	void SetClipDistance(float _clip_distance){clip_distance=_clip_distance;};
	void Pause(bool p)
	{
		pause=p;
	}
};
