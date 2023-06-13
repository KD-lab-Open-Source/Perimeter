#include "StdAfxRD.h"
#include "DrawBuffer.h"
#include "MeshTri.h"

//Функция пересечения треугольника с отрезком
bool Intersection(const Vect3f& a,const Vect3f& b,const Vect3f& c,
		const Vect3f& p0,const Vect3f& p1,const Vect3f& pn,float& t)
{
	//Пересечение отрезка с треугольником
	//a,b,c - треугольник
	//p0,p1,pn - отрезок

	//pn=p1-p0;
	//n.x*(x-a.x)+n.y*(y-a.y)+n.z*(z-a.z)=0;
	//(x,y,z)=pn*t+p0
	//n.x*(pn.x*t+p0.x-a.x)+n.y*(pn.y*t+p0.y-a.y)+n.z*(pn.z*t+p0.z-a.z)=0
	//n*pn*t+n*(p0-a)=0
	//t=n*(p0-a)/(n*pn);

	const float epsilon=1e-6f;
	Vect3f n=(a-b)%(b-c);
	n.Normalize();

	float n_pn=dot(n,pn);
	if(n_pn>-epsilon && n_pn<epsilon)
		return false;
	t=dot(n,(a-p0))/n_pn;
	if(t<0 || t>1)
		return false;

	Vect3f p=pn*t+p0;

	Vect3f nab=(b-a)%n,nbc=(c-b)%n,nca=(a-c)%n;
	float kab=dot(nab,(p-a));
	float kbc=dot(nbc,(p-b));
	float kca=dot(nca,(p-c));
	return (kab<0 && kbc<0 && kca<0)||(kab>0 && kbc>0 && kca>0);
}

cMeshTri::cMeshTri()
{
	db = nullptr;
	OffsetPolygon=-1;
	OffsetVertex=-1;
	NumPolygon=-1;
	NumVertex=-1;
}

cMeshTri::~cMeshTri() = default;

void cMeshTri::GetBoundingBox(Vect3f &Min,Vect3f &Max)
{
	Min.set(1e30f,1e30f,1e30f),Max.set(-1e30f,-1e30f,-1e30f);
	for(int i=0;i<NumVertex;i++)
	{
		const Vect3f&v=GetPos(i);
		Min=::GetMin(Min,v);
		Max=::GetMax(Max,v);
	}
}
void cMeshTri::InvertTri() {
    xassert(!db->IsLocked());
	sPolygon* Polygon = reinterpret_cast<sPolygon*>(gb_RenderDevice->LockIndexBufferRange(db->ib, OffsetPolygon*sPolygon::PN, NumPolygon*sPolygon::PN));
    if (!Polygon) {
        xassert(0);
        return;
    }
    for (int i = 0; i < NumPolygon; i++) {
		sPolygon& p = Polygon[i];
		int tmp=p.p1; p.p1=p.p2; p.p2=tmp;
        PolygonBuffer[i] = p;
	}
	gb_RenderDevice->UnlockIndexBuffer(db->ib);
}

void cMeshTri::CalcBumpST()
{
    //TODO implement this for sokol
    if (gb_RenderDevice->GetRenderSelection() != DEVICE_D3D9) {
        return;
    }
    
#ifdef PERIMETER_D3D9
	if(db->vb.VertexSize!=sizeof(sVertexDot3))
	{
		VISASSERT(0);
		return;
	}

	sVertexDot3 *v = static_cast<sVertexDot3*>(gb_RenderDevice->LockVertexBuffer(db->vb));
	int i;
	for(i=0;i<NumVertex;i++)
	{
		v[i+OffsetVertex].S.set(0,0,0);
		v[i+OffsetVertex].T.set(0,0,0);
	}
	
	for(i=0;i<NumPolygon;i++)
	{
		Vect3f cp;
		Vect3f edge01,edge02;
		const sPolygon &p=PolygonBuffer[i];
		sVertexDot3 &v0=v[p.p1],&v1=v[p.p2],&v2=v[p.p3];

		// x, s, t
		edge01.set( v1.pos.x-v0.pos.x, v1.u1()-v0.u1(), v1.v1()-v0.v1() );
		edge02.set( v2.pos.x-v0.pos.x, v2.u1()-v0.u1(), v2.v1()-v0.v1() );

		cp.cross(edge01,edge02);
		if (xm::abs(cp.x) > FLT_EPS )
		{
			v0.S.x += -cp.y / cp.x;
			v0.T.x += -cp.z / cp.x;

			v1.S.x += -cp.y / cp.x;
			v1.T.x += -cp.z / cp.x;
			
			v2.S.x += -cp.y / cp.x;
			v2.T.x += -cp.z / cp.x;
		}

		// y, s, t
		edge01.set( v1.pos.y-v0.pos.y, v1.u1()-v0.u1(), v1.v1()-v0.v1() );
		edge02.set( v2.pos.y-v0.pos.y, v2.u1()-v0.u1(), v2.v1()-v0.v1() );

		cp.cross(edge01, edge02);
		if (xm::abs(cp.x) > FLT_EPS )
		{
			v0.S.y += -cp.y / cp.x;
			v0.T.y += -cp.z / cp.x;

			v1.S.y += -cp.y / cp.x;
			v1.T.y += -cp.z / cp.x;
			
			v2.S.y += -cp.y / cp.x;
			v2.T.y += -cp.z / cp.x;
		}

		// z, s, t
		edge01.set( v1.pos.z-v0.pos.z, v1.u1()-v0.u1(), v1.v1()-v0.v1() );
		edge02.set( v2.pos.z-v0.pos.z, v2.u1()-v0.u1(), v2.v1()-v0.v1() );

		cp.cross(edge01,edge02);
		if (xm::abs(cp.x) > FLT_EPS )
		{
			v0.S.z += -cp.y / cp.x;
			v0.T.z += -cp.z / cp.x;

			v1.S.z += -cp.y / cp.x;
			v1.T.z += -cp.z / cp.x;
			
			v2.S.z += -cp.y / cp.x;
			v2.T.z += -cp.z / cp.x;
		}
	}

	for(i=0;i<NumVertex;i++)
	{
		sVertexDot3& cv=v[i+OffsetVertex];
  		cv.S.Normalize();
		cv.T.Normalize();
  		cv.SxT.cross(cv.S, cv.T);
  		cv.n.Normalize();
    		
  		// Get the direction of the SxT vector
  		if (cv.SxT.dot(cv.n) < 0.0f)
  		{
  			cv.SxT = -cv.SxT;
  		}
	}

	gb_RenderDevice->UnlockVertexBuffer(db->vb);
#endif
}

bool cMeshTri::Intersect(const Vect3f& p0,const Vect3f& p1)
{
//	return true;
//*
	bool intersect=false;
	Vect3f pn=p1-p0;

	for(int i=0;i<NumPolygon;i++) {
		sPolygon &p=PolygonBuffer[i];
		float t;
		Vect3f a=GetPos(p.p1 - OffsetVertex);
		Vect3f b=GetPos(p.p2 - OffsetVertex);
		Vect3f c=GetPos(p.p3 - OffsetVertex);

		if(Intersection(a,b,c,p0,p1,pn,t))
		{
			intersect=true;
			break;
		}
	}
    
	return intersect;
}