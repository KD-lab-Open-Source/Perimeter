#pragma once
#include "UnkObj.h"

class cUnkLight : public cUnkObj
{
	Vect3f			Direction;
	float			CurrentTime,TimeLife;
	int				NumberKey;
	sParticleKey	*Key;
public:
	cUnkLight();
	virtual ~cUnkLight();
	// общие интерфейсные функции унаследованы от cUnkObj
	virtual void Animate(float dt);
	virtual void PreDraw(cCamera *UCamera);
	virtual void Draw(cCamera *UCamera);
	virtual const MatXf& GetPosition()const; 
	virtual void SetPosition(const MatXf& Matrix);
	virtual void SetDirection(const Vect3f& direction);
	virtual const Vect3f& GetDirection() const;
	virtual void SetAnimation(float Period=2000.f,float StartPhase=0.f,float FinishPhase=-1.f,bool recursive=true);
	virtual void SetAnimKeys(sParticleKey *AnimKeys,int size);
	// интерфейсные функции cUnkLight
	inline Vect3f& GetPos()										{ return GlobalMatrix.trans(); }
	inline float& GetRadius()									{ return Scale.x; }
};
