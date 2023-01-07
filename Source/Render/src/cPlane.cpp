#include "StdAfxRD.h"
#include "cPlane.h"
#include "DrawBuffer.h"
#include "VertexFormat.h"

cPlane::cPlane()
:cUnkObj(KIND_NULL)
{
	umin=vmin=0;
	umax=vmax=1;
}

Vect3f cPlane::GetCenterObject()
{
	return GetGlobalMatrix().trans()+GetScale()*0.5f;
}

void cPlane::PreDraw(cCamera *DrawNode)
{
	if(GetAttribute(ATTRUNKOBJ_IGNORE)==0)
	{
		if(GetTexture() && GetTexture()->IsAlpha())
			DrawNode->Attach(SCENENODE_OBJECTSORT,this);
		else
			DrawNode->Attach(SCENENODE_OBJECT,this);
	}
}

void cPlane::Draw(cCamera *DrawNode)
{
    gb_RenderDevice->SetRenderState(RS_BILINEAR,0);
    gb_RenderDevice->SetNoMaterial(ALPHA_NONE,0,GetTexture());
    gb_RenderDevice->SetWorldMatXf(GetGlobalMatrix());
    
    DrawBuffer* db = gb_RenderDevice->GetDrawBuffer(sVertexXYZDT1::fmt, PT_TRIANGLES); 
	sVertexXYZDT1* vertex=db->LockQuad<sVertexXYZDT1>(1);
	vertex[0].pos.set(0,0,0);
    vertex[0].diffuse=0xFFFFFFFF;
	vertex[0].uv[0]=umin;
	vertex[0].uv[1]=vmin;

	vertex[1].pos.set(0,GetScale().y,0);
    vertex[1].diffuse=0xFFFFFFFF;
	vertex[1].uv[0]=umin;
	vertex[1].uv[1]=vmax;

	vertex[2].pos.set(GetScale().x,0,0);
    vertex[2].diffuse=0xFFFFFFFF;
	vertex[2].uv[0]=umax;
	vertex[2].uv[1]=vmin;

	vertex[3].pos.set(GetScale().x,GetScale().y,0);
    vertex[3].diffuse=0xFFFFFFFF;
	vertex[3].uv[0]=umax;
	vertex[3].uv[1]=vmax;
    db->Unlock();
    
	gb_RenderDevice->SetRenderState(RS_BILINEAR,1);
}

void cPlane::SetUV(float _umin,float _vmin,float _umax,float _vmax)
{
	umin=_umin;
	vmin=_vmin;
	umax=_umax;
	vmax=_vmax;
}
