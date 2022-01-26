//** 1999 Creator - Balmer **//
#pragma once

#define WR(x) fwrite(&x,sizeof(x),1,f);

class CSaver
{
	std::vector<long> p;
	uint32_t m_Data;
public:
	FILE* f;
	CSaver(const char* name);
	CSaver();
	~CSaver();

	uint32_t GetData(){return m_Data;}
	uint32_t SetData(uint32_t dat){return m_Data = dat;}

	bool Init(const char* name);

	void push(uint32_t id);//Вызывать при начале записи блока
	void pop();//Вызывать при окончании записи блока

protected:
	void push();
public:
	inline CSaver& operator<<(const char *x);
	inline CSaver& operator<<(const std::string& x){*this<<x.c_str(); return *this;}
	inline CSaver& operator<<(bool x){WR(x); return *this;};
    inline CSaver& operator<<(char x){WR(x); return *this;};
    inline CSaver& operator<<(int8_t x){WR(x); return *this;};
    inline CSaver& operator<<(uint8_t x){WR(x); return *this;};
	inline CSaver& operator<<(int16_t x){WR(x); return *this;};
	inline CSaver& operator<<(uint16_t x){WR(x); return *this;};
    inline CSaver& operator<<(int32_t x){WR(x); return *this;};
    inline CSaver& operator<<(uint32_t x){WR(x);return *this;};
    inline CSaver& operator<<(int64_t x){WR(x); return *this;};
    inline CSaver& operator<<(uint64_t x){WR(x);return *this;};
	inline CSaver& operator<<(const float& x){WR(x);return *this;};
	inline CSaver& operator<<(const double& x){WR(x);return *this;};

#ifdef _WIN32
    //These are required for Windows which doesn't allow "long" to be implicitly casted to std types...
    inline CSaver& operator<<(long x){ return this->operator<<(static_cast<int32_t>(x));};
    inline CSaver& operator<<(unsigned long x){ return this->operator<<(static_cast<uint32_t>(x));};
#endif

	inline CSaver& operator<<(Vect3f& x){WR(x);return *this;};
	inline CSaver& operator<<(Vect2f& x){WR(x);return *this;};
	inline CSaver& operator<<(MatXf& x){WR(x);return *this;};
};
#undef WR
#define WR(x) fwrite(&x,sizeof(x),1,s.f);
//#define WRC(x) {uint32_t w=x; fwrite(&w,sizeof(w),1,s.f);}

inline CSaver& CSaver::operator<<(const char *x)
{
	if(x)fwrite(x,strlen(x)+1,1,f);
	else fwrite("",strlen("")+1,1,f);
	return *this;
}

void SaveString(CSaver& s, const char* str,uint32_t ido);

#pragma pack(1)
#pragma warning(disable: 4200)
struct CLoadData
{
	uint32_t id;
	int size;
	uint8_t data[];
};

#pragma warning(default: 4200)
#pragma pack()

class CLoadDirectory
{
protected:
	uint8_t *begin,*cur;
	int size;
public:
	CLoadDirectory(uint8_t* data,uint32_t _size);
	CLoadDirectory(CLoadData* ld);
	CLoadData* next();
};

class CLoadDirectoryFile:public CLoadDirectory
{
public:
	CLoadDirectoryFile();
	~CLoadDirectoryFile();
	bool Load(const char* filename);
};

///Читает лишь в случае, если rd_cur_pos+sizeof(x)<=ld->size
#define RD(x) if(rd_cur_pos+(int)sizeof(x)<=ld->size){memcpy(&x,ld->data+rd_cur_pos,sizeof(x));rd_cur_pos+=sizeof(x);}
class CLoadIterator
{
	CLoadData* ld;
	int rd_cur_pos;
public:
	CLoadIterator(CLoadData* _ld):ld(_ld),rd_cur_pos(0){};
	inline void operator>>(bool& i){RD(i);}
	inline void operator>>(char& i){RD(i);}
    inline void operator>>(int8_t& i){RD(i);}
	inline void operator>>(uint8_t& i){RD(i);}
	inline void operator>>(int16_t& i){RD(i);}
	inline void operator>>(uint16_t& i){RD(i);}
	inline void operator>>(int32_t& i){RD(i);}
	inline void operator>>(uint32_t& i){RD(i);}
	inline void operator>>(int64_t& i){RD(i);}
	inline void operator>>(uint64_t& i){RD(i);}
	inline void operator>>(float& i){RD(i);}
	inline void operator>>(double& i){RD(i);}
	inline void operator>>(void *& i){RD(i);}

#ifdef _WIN32
    //These are required for Windows which doesn't allow "long" to be implicitly casted to std types...
    inline void operator>>(long x){ return this->operator>>(checked_reinterpret_cast_ref<long, int32_t>(x));};
    inline void operator>>(unsigned long x){ return this->operator>>(checked_reinterpret_cast_ref<unsigned long, uint32_t>(x));};
#endif

	const char* LoadString()
	{
		if(rd_cur_pos>=ld->size)return "";

		const char* s=(const char*)(ld->data+rd_cur_pos);
		rd_cur_pos+=strlen(s)+1;
		return s;
	}

	inline void operator>>(std::string& s){s=LoadString();}
	inline void operator>>(Vect3f& i){RD(i);}
	inline void operator>>(Vect2f& i){RD(i);}
	inline void operator>>(MatXf& i){RD(i);}
};

template<class T>
void operator>>(CLoadIterator& it, std::vector<T>& v)
{
	uint32_t size=0;
	it>>size;
	v.resize(size);
	for(int i=0;i<size;i++)
	{
		it>>v[i];
	}
}

template<class T>
CSaver& operator<<(CSaver& s, std::vector<T>& v)
{
	uint32_t size=v.size();
	s<<size;
	typename std::vector<T>::iterator it;
	FOR_EACH(v,it)
	{
		s<<*it;
	}
	return s;
}

#undef RD
