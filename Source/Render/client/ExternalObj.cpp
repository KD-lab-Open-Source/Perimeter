#include "StdAfxRD.h"
#include "ExternalObj.h"

cExternalObj::cExternalObj(ExternalObjFunction func_) 
: cAnimUnkObj(KIND_EXTERNALOBJ)
{
	func=func_;
	sort_pass=false;
}
cExternalObj::~cExternalObj()
{
}
void cExternalObj::PreDraw(cCamera *DrawNode)
{
	if(GetAttribute(ATTRUNKOBJ_IGNORE)==0)
	{
		if(GetTexture() && GetTexture()->GetAttribute(TEXTURE_ALPHA_BLEND|TEXTURE_ALPHA_TEST))
			DrawNode->Attach(SCENENODE_OBJECTSPECIAL,this);
		else
			DrawNode->Attach(SCENENODE_OBJECT,this);
	}
	
}
void cExternalObj::Draw(cCamera *DrawNode)
{
	cURenderDevice *Render=DrawNode->GetRenderDevice();
	extern void terExternalShowCall(int ObjType,void *pointer,int TestSizePerByte);

	Render->SetRenderState( RS_CULLMODE, D3DCULL_NONE );

	if(GetTexture()->GetAttribute(TEXTURE_ALPHA_BLEND|TEXTURE_ALPHA_TEST))
	{
		Render->SetNoMaterial(ALPHA_BLEND,GetFrame()->GetPhase(),GetTexture());
	}else
	{
		Render->SetNoMaterial(ALPHA_NONE,GetFrame()->GetPhase(),GetTexture());
	}

	uint32_t zwrite=gb_RenderDevice3D->GetRenderState(D3DRS_ZWRITEENABLE);
	if(sort_pass)
		gb_RenderDevice3D->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );

	func();
	
	gb_RenderDevice3D->SetRenderState( D3DRS_ZWRITEENABLE, zwrite );
	Render->SetRenderState(RS_CULLMODE,-1);
}
