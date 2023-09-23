#include "StdAfxRD.h"
#include "Line3d.h"
#include "DrawBuffer.h"
#include "VertexFormat.h"

cLine3d::cLine3d() : cAnimUnkObj(KIND_LINE3D)
{
	MTINIT(lock);
}

cLine3d::~cLine3d()
{
	MTDONE(lock);
}
void cLine3d::Animate(float dt)
{
	GetFrame()->AddPhase(dt);
}
void cLine3d::PreDraw(cCamera *DrawNode)
{
	MTEnter mtlock(lock);
	if(DrawNode->TestVisible(GetGlobalMatrix(),Bound.min,Bound.max))
	{
		DrawNode->Attach(SCENENODE_OBJECTSORT,this);
		if(GetTexture(1))
			DrawNode->Attach(SCENENODE_OBJECTSORT_NOZ,this);
	}
		
}
void cLine3d::Draw(cCamera *DrawNode)
{
	MTEnter mtlock(lock);
	if(Vertex.size()<2) return;
	
	if(DrawNode->GetCameraPass()==SCENENODE_OBJECTSORT_NOZ)
		gb_RenderDevice->SetNoMaterial(GetAttribute(ATTRUNKOBJ_ADDBLEND)?ALPHA_ADDBLEND:ALPHA_BLEND,0,GetTexture(1));
	else
		gb_RenderDevice->SetNoMaterial(GetAttribute(ATTRUNKOBJ_ADDBLEND)?ALPHA_ADDBLEND:ALPHA_BLEND,0,GetTexture());

//	float width=(DrawNode->GetCameraPass()==SCENENODE_OBJECTSORT_NOZ)?0.5f:1.0f;

    gb_RenderDevice->SetWorldMat4f(nullptr);
    DrawBuffer* db = gb_RenderDevice->GetDrawBuffer(sVertexXYZDT1::fmt, PT_TRIANGLESTRIP);
	Vect3f Orientation;

	sVertexXYZDT1* vb = db->LockTriangleStripSteps<sVertexXYZDT1>(Vertex.size());
    size_t i = 0;
	for( int nVertex=Vertex.size()-1; nVertex>=0; nVertex-- )
	{
		if(nVertex>0)
		{
            Orientation=Vertex[nVertex].pos-Vertex[nVertex-1].pos;
            FastNormalize(Orientation);
/*
			Orientation.precross(DrawNode->GetWorldK()*Vertex[nVertex].width);
/*/

			Vect3f o1=(Vertex[nVertex].pos+Vertex[nVertex-1].pos)*0.5f-DrawNode->GetPos();
			FastNormalize(o1);

			Orientation.precross(o1*(Vertex[nVertex].width));
/**/
		}

        sVertexXYZDT1& v0 = vb[i++];
        sVertexXYZDT1& v1 = vb[i++];
		v0.pos=Vertex[nVertex].pos+Orientation; 
		v1.pos=Vertex[nVertex].pos-Orientation;
		v0.u1()=   v1.u1()=Vertex[nVertex].v-GetFrame()->GetPhase();
		v0.v1()=0; v1.v1()=1;
		v0.diffuse=v1.diffuse=gb_RenderDevice->ConvertColor(Vertex[nVertex].color);
	}

    db->Unlock();
    db->EndTriangleStrip();
}

void cLine3d::UpdateVertex(int num_vertex, sVertexLine* varray)
{
	MTEnter mtlock(lock);

	Vertex.resize(num_vertex);
	if(num_vertex)
		memcpy(&Vertex[0],varray,sizeof(sVertexLine)*num_vertex);
	Bound.min.set(1e30f,1e30f,1e30f);
	Bound.max.set(-1e30f,-1e30f,-1e30f);
	for( int i=0; i<Vertex.size(); i++ )
		Bound.AddBound(Vertex[i].pos);
}

void cLine3d::UpdateVertexPos(int num_vertex, Vect3f* varray)
{
	MTEnter mtlock(lock);

	VISASSERT(Vertex.size()==num_vertex);

	Bound.min.set(1e30f,1e30f,1e30f);
	Bound.max.set(-1e30f,-1e30f,-1e30f);
	for( int i=0; i<Vertex.size(); i++ )
	{
		Vertex[i].pos=varray[i];
		Bound.AddBound(Vertex[i].pos);
	}
}
