#include "StdAfxRD.h"
#include "ObjNode.h"
#include "ObjLight.h"
#include "DrawBuffer.h"
#include "AnimChannel.h"
#include "Scene.h"
#include "MeshBank.h"

cObjLight::cObjLight():cObjectNode(KIND_LIGHT)
{ 
	FarAttenuation=0;
	Intensity=1;

	AnimChannelMat=NULL;
	Texture=NULL;
}
cObjLight::~cObjLight()
{
	RELEASE(AnimChannelMat);
	RELEASE(Texture);
}

//////////////////////////////////////////////////////////////////////////////////////////
// реализация интерфейса cUnkObj
//////////////////////////////////////////////////////////////////////////////////////////

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

    bool move_forward = false;
    Vect3f pos = GetCenterObject();
    
    if (gb_VisGeneric->IsEnableOcclusion()) {
        Vect3f posLight = pos;
        Vect3f posParent = GetRootNode() ? GetRootNode()->GetCenterObject() : pos;
        Vect3f posCamera = DrawNode->GetPos();
        //First make sure is not too close to center, to avoid going off and on when rotating camera around object
        //Then if light is closer to camera pos than object center, move it a bit forward so the mesh won't intersect with light quad
        move_forward = 5.0f <= Vect2f(posLight).distance2(posParent)
                   && posCamera.distance2(posLight) <= posCamera.distance2(posParent);
    }

    gb_RenderDevice->SetWorldMat4f(nullptr);
    gb_RenderDevice->SetNoMaterial(ALPHA_ADDBLEND, GetPhase(), GetTexture());

    sColor4c Diffuse(xm::round(GetDiffuse().r * alpha), xm::round(GetDiffuse().g * alpha), xm::round(GetDiffuse().b * alpha),
                     xm::round(GetDiffuse().a * alpha));
                     
    float radius=GetRadius()*GetGlobalMatrix().rot().xcol().norm();

    Vect3f sx = radius * DrawNode->GetWorldI();
    Vect3f sy = radius * DrawNode->GetWorldJ();

    if (move_forward) {
        pos -= (radius / 4.0f) * DrawNode->GetWorldK();
    }

    DrawBuffer* db = gb_RenderDevice->GetDrawBuffer(sVertexXYZDT1::fmt, PT_TRIANGLES);
    sVertexXYZDT1* v=db->LockQuad<sVertexXYZDT1>(1);
    v[0].pos = pos + sx + sy; v[0].u1()=0, v[0].v1()=0;
    v[1].pos = pos + sx - sy; v[1].u1()=0, v[1].v1()=1;
    v[2].pos = pos - sx + sy; v[2].u1()=1, v[2].v1()=0;
    v[3].pos = pos - sx - sy; v[3].u1()=1, v[3].v1()=1;
    v[0].diffuse=v[1].diffuse=v[2].diffuse=v[3].diffuse=gb_RenderDevice->ConvertColor(Diffuse);
    db->Unlock();
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
    Vect3f min=-GetRadius()*Vect3f(1,1,1);
    Vect3f max=+GetRadius()*Vect3f(1,1,1);
	Min=GetGlobalMatrix().xformPoint(min);
	Max=GetGlobalMatrix().xformPoint(max);
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
