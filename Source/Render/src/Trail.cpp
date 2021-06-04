#include "StdAfxRD.h"
#include "Particle.h"
#include "Trail.h"

cTrail::cTrail(float fTimeLife) : cAnimUnkObj(KIND_PARTICLE)
{
	Bound.min=Bound.max=Vect3f(0,0,0);
	CurrentTime=0;
	TimeLife=fTimeLife;
	Key=NULL;
	NumberKey=0;
	LastParticle=0;
}

cTrail::~cTrail()
{

}

void cTrail::PreDraw(cCamera *DrawNode)
{
	if(Bound.min.x>Bound.max.x)return;
	
	if(DrawNode->TestVisible(GetGlobalMatrix(),Bound.min,Bound.max))
	{
		DrawNode->Attach(SCENENODE_OBJECTSORT,this);
	}
}

void cTrail::Animate(float dt)
{
	CurrentTime+=dt;
	GetFrame()->AddPhase(dt);
	if(GetRef()>1) return; // запрещено удаление
	if(Particle.empty()||(CurrentTime-Particle[LastParticle].time*TimeLife)>TimeLife) 
		Release();
}

void cTrail::Draw(cCamera *DrawNode)
{
	if(Particle.size()<2)
		return;

	DrawStrip strip;
	gb_RenderDevice->SetNoMaterial(GetAttribute(ATTRUNKOBJ_ADDBLEND)?ALPHA_ADDBLEND:ALPHA_BLEND,0,GetTexture());

	strip.Begin();

	sVertexXYZDT1 v1,v2;
	Vect3f &PosCamera=DrawNode->GetPos(),Tangent(0,0,0);
	sColor4c Diffuse(GetDiffuse());
	float dt=CurrentTime/TimeLife;
	for(int i=Particle.size()-1;i>=0;i--)
	{
		// set diffuse color
		float phase=dt-Particle[i].time,scale;
		if(phase>1)
			phase=1.001f;
		if(Key)
		{
			sParticleKey &key=Key[round(phase*(NumberKey-1))];
			v2.diffuse=v1.diffuse=key.diffuse;
			scale=key.rotate.x;
		}
		else
		{
			v1.diffuse=Diffuse;
			v1.diffuse.a=round(Diffuse.a*(1-phase));
			v2.diffuse=v1.diffuse;
			scale=GetScale().x;
		}
		
		// set texture coord
		v1.u1()=v2.u1()=Particle[i].v.x+GetFrame()->GetPhase();
		v1.v1()=0,v2.v1()=1;
		// set position
		if(i>0)
		{
			Tangent=Particle[i-1].pos-Particle[i].pos;
			FastNormalize(Tangent);
		}
		Vect3f vCameraToObject=PosCamera-Particle[i].pos;
		FastNormalize(vCameraToObject);
		Vect3f Orientation; 
		Orientation.cross(vCameraToObject,Tangent);
		Orientation*=scale;
		v1.pos=Particle[i].pos-Orientation;
		v2.pos=Particle[i].pos+Orientation;
		if(phase>=1)
			break;
		strip.Set(v1,v2);
	}

	strip.End();
}
void cTrail::SetData(const Vect3f* pPos,const Vect3f* pVelocity)
{
	GlobalMatrix.trans()=LocalMatrix.trans()=*pPos;
	Bound.min=Bound.max=Vect3f(0,0,0);

	Particle.push_back(sParticle());

	sParticle *v=&Particle.back();

	v->time=CurrentTime/TimeLife;
	v->pos=*pPos;
	if(Particle.size()>1) 
		v->v.x=v[-1].v.x+v->pos.distance(v[-1].pos)/GetScale().z; 
	else v->v.x=0;

	LastParticle=Particle.size()-1;
}

void cTrail::SetAnimKeys(sParticleKey *AnimKeys,int size)
{
	NumberKey=size;
	Key=AnimKeys;
}
