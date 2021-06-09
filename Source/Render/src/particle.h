#ifndef __PARTICLE_H__
#define __PARTICLE_H__

struct sParticle
{
	float	time;		// time/timelife
	Vect3f	pos;		// position
	Vect3f	v;			// velocity
};

class cParticle : public cAnimUnkObj
{
protected:
	vector<sParticle>			Particle;
	vector<int>					StoppedParticle;

	float						CurrentTime,TimeLife,dTime;
	Vect3f						Dumping;
	Vect3f						Force;
	sParticleKey				*Key;
	short						NumberKey;
	short						LastParticle;
	sBox6f						Bound;
	static int					RandomVal;
	Vect2f						TexSize;
	char						bNotUpdate;
public:
	cParticle(float fTimeLife=1000.f,const Vect2f &vTexSize=Vect2f(1,1));
	virtual ~cParticle();
	virtual void Animate(float dt);
	virtual void PreDraw(cCamera *UCamera);
	virtual void Draw(cCamera *UClass);
	virtual void SetData(const Vect3f* pPos,const Vect3f* pVelocity);
	virtual void SetAnimKeys(sParticleKey *AnimKeys,int size);
	virtual void SetProperties(const Vect3f *Dumping,const Vect3f *Force,const Vect3f *ReflectionImpulse=0);
	virtual void Update();
	virtual void Free();

	__forceinline unsigned int rand()
	{
		return(((RandomVal = RandomVal * 214013L + 2531011L) >> 16) & RAND_MAX);
	}
	
	int GetNumParticle(){return Particle.size();}
public:
	sParticle& GetFreeParticle();
	void SetFreeParticle(int n);
};

#endif //__PARTICLE_H__