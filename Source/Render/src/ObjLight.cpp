#include "StdAfxRD.h"
#include "ObjNode.h"
#include "ObjLight.h"
#include "AnimChannel.h"
#include "Scene.h"
#include "MeshBank.h"

cObjLight::cObjLight():cObjectNode(KIND_LIGHT)
{ 
	FarAttenuation=0;
	Intensity=1;

	AnimChannelMat=NULL;
	Texture=NULL;

	fade=0;
	prev_time=0;
}
cObjLight::~cObjLight()
{
	RELEASE(AnimChannelMat);
	RELEASE(Texture);
}

//////////////////////////////////////////////////////////////////////////////////////////
// реализация интерфейса cUnkObj
//////////////////////////////////////////////////////////////////////////////////////////

void cObjLight::OcclusionTest()
{
	if(occlusion.IsInit())
	{
		bool occlude=occlusion.IsVisible();
		double cur_time=clockf();
		double dt=cur_time-prev_time;
		prev_time=cur_time;
		if(dt>0.1e3)
			dt=0.1e3;
		if(occlude)
			fade+=dt*Option_InvOcclusionFadeTime;
		else
			fade-=dt*Option_InvOcclusionFadeTime;
		if(fade<0)
			fade=0;
		if(fade>1)
			fade=1;
	}
}

void cObjLight::PreDraw(cCamera *DrawNode)
{ 
	cObjectNode::PreDraw(DrawNode);

	if(NodeAttribute.GetAttribute(ATTRNODE_IGNORE)) return;
//	if(AnimChannelMat->IsAnimMaterial(GetCurrentChannel())) // анимация материала
		AnimChannelMat->GetChannel(GetCurrentChannel())->GetDiffuse(GetPhase(),Diffuse);
	if((GetDiffuse().a<0.004f) || 
		(GetDiffuse().r<0.004f && GetDiffuse().g<0.004f && GetDiffuse().b<0.004f)) return;

	// тест на видимость источника света
	Vect3f PosCamera=DrawNode->GetPos();//DrawNode->GetMatrix().trans();

	Vect3f &PosLight=GlobalMatrix.trans();
	Intensity=1-PosCamera.distance2(PosLight)/(GetFarAttenuation()*GetFarAttenuation());
	if(Intensity>0)
		DrawNode->Attach(SCENENODE_OBJECTSORT,this); // спрайты всегда выводятся последними

}
void cObjLight::SetAttr(int attribute)
{
	SetAttribute(attribute);
	ClearAttribute(ATTRUNKOBJ_SHADOW);
}
void cObjLight::Draw(cCamera *DrawNode)
{
	if(DrawNode->GetAttribute(ATTRCAMERA_REFLECTION)&&
		(GetGlobalMatrix().trans().z<DrawNode->GetHReflection()))
		return;

	float alpha=255.0f*GetIntensity();

	DWORD zfunc=gb_RenderDevice3D->GetRenderState(D3DRS_ZFUNC);

	if(occlusion.IsInit() && !DrawNode->GetParent())
	{
		occlusion.Test(GetGlobalMatrix().trans());
		alpha*=fade;
		gb_RenderDevice3D->SetRenderState(D3DRS_ZFUNC,D3DCMP_ALWAYS);
	}

	sColor4c Diffuse(round(GetDiffuse().r*alpha),round(GetDiffuse().g*alpha),round(GetDiffuse().b*alpha),round(GetDiffuse().a*alpha));
	float radius=GetRadius()*GetGlobalMatrix().rot().xcol().norm();

	cVertexBuffer<sVertexXYZDT1>* buf=gb_RenderDevice->GetBufferXYZDT1();
	sVertexXYZDT1 *v=buf->Lock(4);
	Vect3f sx=radius*DrawNode->GetWorldI(),sy=radius*DrawNode->GetWorldJ();
	v[0].pos=GetGlobalMatrix().trans()+sx+sy; v[0].u1()=0, v[0].v1()=0; 
	v[1].pos=GetGlobalMatrix().trans()+sx-sy; v[1].u1()=0, v[1].v1()=1;
	v[2].pos=GetGlobalMatrix().trans()-sx+sy; v[2].u1()=1, v[2].v1()=0;
	v[3].pos=GetGlobalMatrix().trans()-sx-sy; v[3].u1()=1, v[3].v1()=1;
	v[0].diffuse=v[1].diffuse=v[2].diffuse=v[3].diffuse=Diffuse;
	buf->Unlock(4);

	gb_RenderDevice->SetNoMaterial(ALPHA_ADDBLEND,GetPhase(),GetTexture());
	buf->DrawPrimitive(PT_TRIANGLESTRIP,2,MatXf::ID);

	if(occlusion.IsInit())
		gb_RenderDevice3D->SetRenderState(D3DRS_ZFUNC,zfunc);

}

void cObjLight::SetColor(const sColor4f *Ambient,const sColor4f *Diffuse,const sColor4f *Specular)
{
	VISASSERT(0);
//	Material.SetColor(Diffuse,Specular);
}

cIUnkObj* cObjLight::BuildCopy()
{
	cObjLight *Light=new cObjLight;
	cObjectNode::SetCopy(Light);
	Light->FarAttenuation=FarAttenuation;
	Light->Intensity=Intensity;
	if(AnimChannelMat)AnimChannelMat->IncRef();
	Light->AnimChannelMat=AnimChannelMat;

	if(Texture)Texture->IncRef();
	Light->Texture=Texture;
	return Light;
}
//////////////////////////////////////////////////////////////////////////////////////////
// реализация интерфейса cUnkTile
//////////////////////////////////////////////////////////////////////////////////////////
void cObjLight::GetBoundingBox(Vect3f &Min,Vect3f &Max)
{ 
	Min=GetGlobalMatrix().xformPoint(-GetRadius()*Vect3f(1,1,1));
	Max=GetGlobalMatrix().xformPoint(+GetRadius()*Vect3f(1,1,1));
}
void cObjLight::CalcBorder(Vect3f& Min,Vect3f& Max)
{
	Vect3f min,max;
	min=-GetRadius()*Vect3f(1,1,1);
	max=+GetRadius()*Vect3f(1,1,1);
	Min=::GetMin(min,Min);
	Min=::GetMin(max,Min);
	Max=::GetMax(min,Max);
	Max=::GetMax(max,Max);

	cObjectNode::CalcBorder(Min,Max);
}

void cObjLight::SetTextureLight(cTexture *pTexture,int attr)
{
	RELEASE(Texture);

	Texture=pTexture;
}

void cObjLight::ChangeBank(cAllMeshBank* new_root)
{
	cObjectNode::ChangeBank(new_root);
	cTexture *pNew=TextureWithAnnoterPath(Texture,new_root->GetTexturePath(),new_root->GetDefTexturePath());
	RELEASE(Texture);
	Texture=pNew;
}
