#include "StdAfxRD.h"
#include "UnkLight.h"

cUnkLight::cUnkLight() : cUnkObj(KIND_LIGHT)
{
	Direction.set(0,0,-0.995f);
	CurrentTime=0;
	TimeLife=1000;
	NumberKey=0;
}

cUnkLight::~cUnkLight()
{
}

void cUnkLight::Animate(float dt)
{
	if(CurrentTime==0) dt=0.1f;
	CurrentTime+=dt;
}
void cUnkLight::SetAnimation(float Period,float StartPhase,float FinishPhase,bool recursive)
{
	TimeLife = Period;
}
void cUnkLight::PreDraw(cCamera *DrawNode)
{ 
	if( NumberKey ) 
	{
		int nKey = round( floor( NumberKey*fmodf( CurrentTime/TimeLife, 0.999f ) ) );
		VISASSERT( 0<=nKey && nKey<NumberKey );
		GetRadius() = max(Key[nKey].rotate.x,1.0f);

		sColor4f d(Key[nKey].diffuse.r/255.f, Key[nKey].diffuse.g/255.f, Key[nKey].diffuse.b/255.f, Key[nKey].diffuse.a/255.f);
		SetDiffuse(d);
	}

	if( GetAttribute(ATTRLIGHT_SPHERICAL_SPRITE) )
		if( DrawNode->TestVisible(GetGlobalMatrix().trans(),GetRadius()) )
			DrawNode->Attach(SCENENODE_OBJECTSORT,this); // спрайты всегда выводятся последними
}
void cUnkLight::Draw(cCamera *DrawNode)
{
	if(DrawNode->GetAttribute(ATTRCAMERA_REFLECTION)&&(GetGlobalMatrix().trans().z<DrawNode->GetHReflection()))
		return;
	DrawStrip strip;
	gb_RenderDevice->SetNoMaterial(ALPHA_ADDBLEND,0,GetTexture());

	Vect3f pv0,pe0,pv,pe;
	const int NumberPlane=10;
	sColor4c Diffuse(GetDiffuse().a*GetDiffuse().r,
					GetDiffuse().a*GetDiffuse().g,
					GetDiffuse().a*GetDiffuse().b,1);
	float dr=2*GetRadius()/(NumberPlane+1);

	strip.Begin();
	float tex=0;
	Vect3f WorldK=DrawNode->GetPos()-GetPos();
	FastNormalize(WorldK);

	sVertexXYZDT1 v0,v1;
	for(int i=0;i<NumberPlane;i++,tex+=1)
	{ // NumberPlane>=2
		float r=GetRadius()*(1-ABS(2.f*(i+1)/(NumberPlane+2)-1));
		Vect3f sx=r*DrawNode->GetWorldI(),sy=r*DrawNode->GetWorldJ(),
//			sz=(GetRadius()-(i+1)*dr)*DrawNode->GetWorldK();
			sz=(GetRadius()-(i+1)*dr)*WorldK;
		if(i&1)
			v0.pos=GetPos()-sx+sy-sz,
			v1.pos=GetPos()-sx-sy-sz;
		else
			v0.pos=GetPos()+sx+sy-sz,
			v1.pos=GetPos()+sx-sy-sz;
		v0.GetTexel().set(tex,0); v1.GetTexel().set(tex,1);
		v0.diffuse=v1.diffuse=Diffuse;
		strip.Set(v0,v1);
	}

	strip.End();
}
const MatXf& cUnkLight::GetPosition() const
{
	return GetLocalMatrix();
}
void cUnkLight::SetPosition(const MatXf& Matrix)
{
	GetPos()=Matrix.trans();
}
void cUnkLight::SetDirection(const Vect3f& direction)
{
	Direction=direction;
	Direction.Normalize(0.995f);
}
const Vect3f& cUnkLight::GetDirection() const
{
	return Direction;
}
void cUnkLight::SetAnimKeys(sParticleKey *AnimKeys,int size)
{
	NumberKey=size;
	Key=AnimKeys;
}
