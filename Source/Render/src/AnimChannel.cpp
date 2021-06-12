#include "StdAfxRD.h"
#include "AnimChannel.h"
#include "MeshBank.h"

//////////////////////////////////////////////////////////////////////////////////////////
// реализация cAnimChain
//////////////////////////////////////////////////////////////////////////////////////////
cAnimChainMaterial::cAnimChainMaterial()
{
	parent=NULL;
	is_anim_material=false;
}

cAnimChainMaterial::~cAnimChainMaterial()
{
}

void cAnimChainMaterial::GetDiffuse(float phase,sColor4f &Diffuse)
{
	float CurrentTime=phase*parent->Time;
	if(KeyDiffuse.size()==1)
		Diffuse=KeyDiffuse[0].color;
	else if(KeyDiffuse.size()>1)
	{
		int i = BinaryFind( CurrentTime, KeyDiffuse );
		sKeyColor &a=KeyDiffuse[i],&b=KeyDiffuse[i+1];
		Diffuse.interpolate3(a.color,b.color,(CurrentTime-a.time)/(b.time-a.time));
	}

	if(KeyTransparency.size()==1)
		Diffuse.a=KeyTransparency[0].a;
	else if(KeyTransparency.size()>1)
	{
		int i = BinaryFind( CurrentTime, KeyTransparency );
		sKeyFloat &a=KeyTransparency[i],&b=KeyTransparency[i+1];
		Diffuse.a=a.a+(b.a-a.a)*(CurrentTime-a.time)/(b.time-a.time);
	}else
		Diffuse.a=1.0f;
}

float cAnimChainMaterial::GetTransparency(float phase)
{
//	VISASSERT(KeyTransparency.size());
	
	if(KeyTransparency.size()==1)
		return KeyTransparency[0].a;
	else if(KeyTransparency.size()>1)
	{
		float CurrentTime=phase*parent->Time;
		int i = BinaryFind( CurrentTime, KeyTransparency );
		sKeyFloat &a=KeyTransparency[i],&b=KeyTransparency[i+1];
		return a.a+(b.a-a.a)*(CurrentTime-a.time)/(b.time-a.time);
	}

	return 1.0f;
}

void cAnimChainMaterial::GetMaterial(float phase,cObjMaterial &ObjMaterial)
{
	float CurrentTime=phase*parent->Time;
	if(KeyAmbient.size()==1)
		ObjMaterial.Ambient=KeyAmbient[0].color;
	else
	{
		VISASSERT(KeyAmbient.size()>1);
		int i = BinaryFind( CurrentTime, KeyAmbient );
		sKeyColor &a=KeyAmbient[i],&b=KeyAmbient[i+1];
		ObjMaterial.Ambient.interpolate3(a.color,b.color,(CurrentTime-a.time)/(b.time-a.time));
	}

	if(KeyDiffuse.size()==1)
		ObjMaterial.Diffuse=KeyDiffuse[0].color;
	else 
	{
		VISASSERT(KeyDiffuse.size()>1);
		int i = BinaryFind( CurrentTime, KeyDiffuse );
		sKeyColor &a=KeyDiffuse[i],&b=KeyDiffuse[i+1];
		ObjMaterial.Diffuse.interpolate3(a.color,b.color,(CurrentTime-a.time)/(b.time-a.time));
	}

	if(KeySpecular.size()==1)
		ObjMaterial.Specular=KeySpecular[0].color;
	else
	{
		VISASSERT(KeySpecular.size()>1);
		int i = BinaryFind( CurrentTime, KeySpecular );
		sKeyColor &a=KeySpecular[i],&b=KeySpecular[i+1];
		ObjMaterial.Specular.interpolate3(a.color,b.color,(CurrentTime-a.time)/(b.time-a.time));
	}

	if(KeyEmissive.size()==1)
		ObjMaterial.Emissive=KeyEmissive[0].color;
	else
	{
		VISASSERT(KeyEmissive.size()>1);
		int i = BinaryFind( CurrentTime, KeyEmissive );
		sKeyColor &a=KeyEmissive[i],&b=KeyEmissive[i+1];
		ObjMaterial.Emissive.interpolate3(a.color,b.color,(CurrentTime-a.time)/(b.time-a.time));
	}

	if(KeyTransparency.size()==1)
		ObjMaterial.Diffuse.a=KeyTransparency[0].a;
	else
	{
		VISASSERT(KeyTransparency.size()>1);
		int i = BinaryFind( CurrentTime, KeyTransparency );
		sKeyFloat &a=KeyTransparency[i],&b=KeyTransparency[i+1];
		ObjMaterial.Diffuse.a=a.a+(b.a-a.a)*(CurrentTime-a.time)/(b.time-a.time);
	}

	if(KeyPower.size()==1)
		ObjMaterial.Power=KeyPower[0].a;
	else
	{
		VISASSERT(KeyPower.size()>1);
		int i = BinaryFind( CurrentTime, KeyPower );
		sKeyFloat &a=KeyPower[i],&b=KeyPower[i+1];
		ObjMaterial.Power=a.a+(b.a-a.a)*(CurrentTime-a.time)/(b.time-a.time);
	}
}

void cAnimChainMaterial::GetTexMatrix(float phase,MatXf &Matrix)
{
	float CurrentTime=phase*parent->Time;
	sKeyTexMatrix *a=0,*b=0;
	int i;
	for(i=0;i<KeyTexMatrix.size();i++)
		if(KeyTexMatrix[i].time>=CurrentTime)
		{ b=&KeyTexMatrix[i]; break; }
	for(i=KeyTexMatrix.size()-1;i>=0;i--)
		if(KeyTexMatrix[i].time<=CurrentTime)
		{ a=&KeyTexMatrix[i]; break; }
	VISASSERT(a);
	if( b==0 || a==b )
		Matrix=a->m;
	else
	{
		float f1=(CurrentTime-a->time)/(b->time-a->time),f2=1-f1;
		Matrix.rot()=f2*a->m.rot()+f1*b->m.rot();
		Matrix.trans()=f2*a->m.trans()+f1*b->m.trans();
	}
}

void cAnimChainMaterial::EndBuild()
{
	is_anim_material=KeyAmbient.size()>1||KeyDiffuse.size()>1||KeySpecular.size()>1||KeyTransparency.size()>1||KeyTexMatrix.size()>1;
}

//////////////////////////cAnimChannelMaterial//////////////////
void cAnimChannelMaterial::NewChannel(cAllMeshBank* root)
{
	int n=root->GetNumberChannel();
	cAnimChannelBase<cAnimChainMaterial>::NewChannel(n);
	for(int i=0;i<n;i++)
	{
		GetChannel(i)->parent=&root->GetChannel(i);
	}
}

cAnimChannelMaterial::~cAnimChannelMaterial()
{
}


void cAnimChannelMaterial::EndBuild()
{
	for(int i=0;i<GetNumberChannel();i++)
		GetChannel(i)->EndBuild();
}