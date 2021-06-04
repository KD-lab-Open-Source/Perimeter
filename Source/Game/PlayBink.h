#pragma once

class PlayBink
{
protected:
	PlayBink(){};
public:
	static PlayBink* Create();
	virtual ~PlayBink(){};
	virtual Vect2i GetSize()=0;

	virtual void SetVolume(float vol)=0;
	virtual bool Init(const char* bink_name)=0;
	virtual void Draw(int x,int y,int dx,int dy, int alpha = 255)=0;
	virtual bool CalcNextFrame()=0;
	virtual bool IsEnd()=0;
};

class PlayBinkSample
{
public:
	PlayBinkSample();
	~PlayBinkSample();

	void SetPos(Vect2i pos_){pos=pos_;}
	void SetSize(Vect2i size_){size=size_;}

	void DoModal(const char* bink_name,const char* sound_name);
public:
	PlayBink* bink;
	cTexture* pTextureBackground;
	Vect2i pos;
	Vect2i size;
	bool MustInterrupt();
};
