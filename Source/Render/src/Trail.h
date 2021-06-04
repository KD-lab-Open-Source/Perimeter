#pragma once

#include "particle.h"

class cTrail : public cAnimUnkObj
{
	vector<sParticle>			Particle;
	sParticleKey				*Key;
	short						NumberKey;
	short						LastParticle;
	float						CurrentTime,TimeLife;
	sBox6f						Bound;
public:
	cTrail(float TimeLife=1000.f);
	~cTrail();
	void PreDraw(cCamera *UCamera);
	void Animate(float dt);
	void Draw(cCamera *UClass);
	void SetData(const Vect3f* pPos,const Vect3f* pVelocity=NULL);
	void SetAnimKeys(sParticleKey *AnimKeys,int size);
};

