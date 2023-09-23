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
	cInterfaceRenderDevice *Render=DrawNode->GetRenderDevice();

    uint32_t cull = Render->GetRenderState(RS_CULLMODE);
	Render->SetRenderState( RS_CULLMODE, CULL_NONE );

	if(GetTexture()->GetAttribute(TEXTURE_ALPHA_BLEND|TEXTURE_ALPHA_TEST)) {
		Render->SetNoMaterial(ALPHA_BLEND,GetFrame()->GetPhase(),GetTexture());
	} else {
		Render->SetNoMaterial(ALPHA_NONE,GetFrame()->GetPhase(),GetTexture());
	}

	uint32_t zwrite = Render->GetRenderState(RS_ZWRITEENABLE);
    if (sort_pass) {
        Render->SetRenderState(RS_ZWRITEENABLE, false);
    }

	func();

    Render->SetRenderState(RS_ZWRITEENABLE, zwrite);
    Render->SetRenderState(RS_CULLMODE, cull);
}
