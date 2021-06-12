#pragma once
/*
Потоковая интерполяция данных.
Каждая команда - указатель на функцию.
int command(void* data);
Функция возвращает величину данных data,
которые ей нужны.
*/

typedef int (*InterpolateFunction)(cIUnkObj* cur,void* data);

struct sColorInterpolate
{
	sColor4f color,add_color;
};

struct sAngleInterpolate
{
	float angle;
	eAxis axis;
};

class StreamInterpolator
{
	vector<char> stream;
	MTDECLARE(lock);
	bool in_avatar;
public:
	StreamInterpolator();
	~StreamInterpolator();
	void Init(){stream.clear();}
	void ProcessData();
	void ClearData();

	void Lock(){MTENTER(lock);};
	void Unlock(){MTLEAVE(lock);};

	void SetInAvatar(bool in){in_avatar=in;}

	StreamInterpolator& set(InterpolateFunction func,cIUnkClass* obj);
	StreamInterpolator& operator<<(Vect3f f[2]);
	StreamInterpolator& operator<<(Se3f f[2]);
	StreamInterpolator& operator<<(float f[2]);
	StreamInterpolator& operator<<(sColor4c f[2]);
	StreamInterpolator& operator<<(sColor4f f[2]);
	StreamInterpolator& operator<<(int f);
	StreamInterpolator& operator<<(sColorInterpolate f[2]);
};

extern StreamInterpolator stream_interpolator;

int fSpriteInterpolation(cIUnkObj* cur,void* data);
int fLineInterpolation(cIUnkObj* cur,void* data);
int fSe3fInterpolation(cIUnkObj* cur,void* data);
int fPhaseInterpolation(cIUnkObj* cur,void* data);
int fAngleInterpolation(cIUnkObj* cur,void* data);
int fFloatInterpolation(cIUnkObj* cur,void* data);
int fColorInterpolation(cIUnkObj* cur,void* data);
int fColorDiffuseInterpolation(cIUnkObj* cur,void* data);
int fParticleRateInterpolation(cIUnkObj* cur,void* data);
