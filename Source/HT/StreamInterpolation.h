#pragma once
/*
Потоковая интерполяция данных.
Каждая команда - указатель на функцию.
int command(void* data);
Функция возвращает величину данных data,
которые ей нужны.
*/

using InterpolateFunction = void (*)(cUnknownClass*, XBuffer*);

const int INTERPOLATION_DATA_FLAG_CLEARED = 1 << 0;

#pragma pack(push,1)

struct InterpolateHeader {
    uint16_t flags = 0;
    uint16_t data_len = 0;
    InterpolateFunction func = nullptr;
    cUnknownClass* obj = nullptr;
};

#pragma pack(pop)

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
	XBuffer stream;
    size_t last_header = 0;
	MTDECLARE(lock);
	bool in_avatar;
public:
	StreamInterpolator();
	~StreamInterpolator();
	void ProcessData();
	void ClearData();

	void Lock(){MTENTER(lock);};
	void Unlock(){MTLEAVE(lock);};

	void SetInAvatar(bool in){in_avatar=in;}

	StreamInterpolator& set(InterpolateFunction func,cUnknownClass* obj);

#ifdef PERIMETER_DEBUG
    void CheckData() {
        xassert(in_avatar);
        xassert(last_header + sizeof(InterpolateHeader) <= stream.tell());
        InterpolateHeader& data = *reinterpret_cast<InterpolateHeader*>(&stream[last_header]);
        xassert(last_header + sizeof(InterpolateHeader) + data.data_len == stream.tell());
    }
#else
#define CheckData()
#endif

    template<typename T>
    StreamInterpolator& write(const T& v) {
        CheckData();
        //Increment data len with data about to write
        InterpolateHeader& data = *reinterpret_cast<InterpolateHeader*>(&stream[last_header]);
        xassert(data.data_len + sizeof(T) <= std::numeric_limits<uint16_t>().max());
        data.data_len += sizeof(T);
        //Write data
        stream.write(v);
        return *this;
    }
    
    template<typename T>
    StreamInterpolator& operator<<(const T& f) { return write(f); }
};

extern StreamInterpolator stream_interpolator;

void fSpriteInterpolation(cUnknownClass* cur, XBuffer* data);
void fLineInterpolation(cUnknownClass* cur, XBuffer* data);
void fSe3fInterpolation(cUnknownClass* cur, XBuffer* data);
void fPhaseInterpolation(cUnknownClass* cur, XBuffer* data);
void fAngleInterpolation(cUnknownClass* cur, XBuffer* data);
void fFloatInterpolation(cUnknownClass* cur, XBuffer* data);
void fColorInterpolation(cUnknownClass* cur, XBuffer* data);
void fColorDiffuseInterpolation(cUnknownClass* cur, XBuffer* data);
void fParticleRateInterpolation(cUnknownClass* cur, XBuffer* data);
