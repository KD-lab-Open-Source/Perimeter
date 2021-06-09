#pragma once

struct KeyGeneral
{
	float time;
	static float time_delta;
};

template<class Key>
class CKeyBase:public vector<Key>
{
public:
	typedef typename vector<Key>::iterator iterator;
	typedef typename Key::value value;

	value Get(float t);
	Key& InsertKey(float t);
	Key* GetOrCreateKey(float t,float life_time,float create_time,bool* create);
};

template<class Key>
typename CKeyBase<Key>::value CKeyBase<Key>::Get(float t)
{
	if(this->empty())return Key::none;
	if(this->size()==1)return (*this)[0].Val();

	if(t<(*this)[0].time)
		return (*this)[0].Val();

	for(int i=1;i<this->size();i++)
	if(t<(*this)[i].time)
	{
		Key& f0=(*this)[i-1];
		Key& f1=(*this)[i];
		float dx=f1.time-f0.time;
		xassert(dx>=0);
		xassert(t>=f0.time);
		float tx=(t-f0.time)/dx;

		value out;
		f0.interpolate(out,f0.Val(),f1.Val(),tx);
		return out;
	}

	return this->back().Val();
}

template<class Key>
Key& CKeyBase<Key>::InsertKey(float t)
{
	Key p;
	p.Val()=Get(t);
	p.time=t;

	if(t<this->front().time)
	{
        this->insert(this->begin(),p);
		return this->front();
	}
	
	for(int i=1;i<this->size();i++)
	if(t<(*this)[i].time)
	{
		return *this->insert(this->begin()+i,p);
	}

	this->push_back(p);
	return this->back();
}

template<class Key>
Key* CKeyBase<Key>::GetOrCreateKey(float t,float life_time,float create_time,bool* create)
{
	if(create)
		*create=false;
	if(life_time<KeyGeneral::time_delta)
		return &(*this)[0];

	iterator it;
	FOR_EACH(*this,it)
	{
		Key& p=*it;
		float tp=p.time*life_time+create_time;
		if(fabsf(tp-t)<=KeyGeneral::time_delta)
			return &p;
	}

	if(create)
		*create=true;
	return &InsertKey((t-create_time)/life_time);
}


struct KeyFloat:KeyGeneral
{
	KeyFloat(){}
	KeyFloat(float time_,float f_){time=time_;f=f_;}
	float f;

	typedef float value;
	static value none;
	value& Val(){return f;};
	inline void interpolate(value& out,const value& in1,const value& in2,float tx)
	{
		out=in1*(1-tx)+in2*tx;
	}
};

class CVectVect3f : public vector<Vect3f>
{
public:
	void Save(CSaver& s,int id);
	void Load(CLoadIterator rd);
};

class CKey:public CKeyBase<KeyFloat>
{
public:
	void Save(CSaver& s,int id);
	void Load(CLoadIterator rd);
};
struct KeyPos:public KeyGeneral
{
	Vect3f pos;

	typedef Vect3f value;
	static value none;
	value& Val(){return pos;};
	inline void interpolate(value& out,const value& in1,const value& in2,float tx)
	{
		out=in1*(1-tx)+in2*tx;
	}
};

class CKeyPos:public CKeyBase<KeyPos>
{
public:
	void Save(CSaver& s,int id);
	void Load(CLoadIterator rd);
protected:
	void SaveInternal(CSaver& s);
};

class CKeyPosHermit:public CKeyPos
{
public:
	enum CLOSING
	{
		T_CLOSE,
		T_FREE,
		T_CYCLE,
	};

	CLOSING cbegin,cend;

	CKeyPosHermit();
	value Get(float t);

	Vect3f Clamp(int i);
	void Save(CSaver& s,int id);
	void Load(CLoadIterator rd);
};

struct KeyRotate:public KeyGeneral
{
	QuatF pos;

	typedef QuatF value;
	static value none;
	value& Val(){return pos;};
	inline void interpolate(value& out,const value& in1,const value& in2,float tx)
	{
		out.slerp(in1,in2,tx);
	}
};

class CKeyRotate:public CKeyBase<KeyRotate>
{
public:
	void Save(CSaver& s,int id);
	void Load(CLoadIterator rd);
};

struct KeyColor:public KeyGeneral,public sColor4f
{

	typedef sColor4f value;
	static value none;
	value& Val(){return (value&)*this;};
	inline void interpolate(value& out,const value& in1,const value& in2,float tx)
	{
		out=in1*(1-tx)+in2*tx;
	}
};

class CKeyColor:public CKeyBase<KeyColor>
{
public:
	void Save(CSaver& s,int id);
	void Load(CLoadIterator rd);
};

/////////////////////////////
template <class type>
class BackVector:public vector<type>
{
	vector<int>	stopped;
public:
	bool is_empty(){return this->size()<=stopped.size();}
	type& GetFree();
	int GetIndexFree();
	void SetFree(int n);

	void Compress();
};


template <class type> int BackVector<type>::GetIndexFree()
{
	int FreeParticle=-1;
	if(stopped.empty())
	{
		FreeParticle=this->size();
		this->push_back(type());
	}else
	{
		FreeParticle=stopped.back();
		stopped.pop_back();
		VISASSERT((*this)[FreeParticle].key==-1);
	}
	return FreeParticle;
}

template <class type> type& BackVector<type>::GetFree()
{
	int FreeParticle=-1;
	if(stopped.empty())
	{
		FreeParticle=this->size();
		this->push_back(type());
	}else
	{
		FreeParticle=stopped.back();
		stopped.pop_back();
		VISASSERT((*this)[FreeParticle].key==-1);
	}
	return (*this)[FreeParticle];
}

template <class type>
void BackVector<type>::SetFree(int n)
{
	type &p=(*this)[n];
	p.key=-1;
	stopped.push_back(n);
}

template <class type>
void BackVector<type>::Compress()
{
	if(this->size()<6)
		return;
	if(stopped.size()*2<=this->size())
		return;
	stopped.clear();
	int curi=0;
	for(int i=0;i<this->size();i++)
	{
		if((*this)[i].key!=-1)
		{
			if(i!=curi)
				(*this)[curi]=(*this)[i];
			curi++;
		}
	}

	this->resize(curi);
}
