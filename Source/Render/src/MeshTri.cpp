#include "StdAfxRD.h"
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
	ib=NULL;
	vb=NULL;
	OffsetPolygon=-1;
	OffsetVertex=-1;
	NumPolygon=-1;
	NumVertex=-1;

//	vbSoft=NULL;
//	ibSoft=NULL;
}
cMeshTri::~cMeshTri()
{
}

void cMeshTri::GetBoundingBox(Vect3f &Min,Vect3f &Max)
{
	void *pVertex=gb_RenderDevice->LockVertexBuffer(*vb);
	Min.set(1e30f,1e30f,1e30f),Max.set(-1e30f,-1e30f,-1e30f);
	for(int i=0;i<NumVertex;i++)
	{
		Vect3f &v=GetPos(pVertex,i+OffsetVertex);
		Min=::GetMin(Min,v);
		Max=::GetMax(Max,v);
	}
	gb_RenderDevice->UnlockVertexBuffer(*vb);
}
void cMeshTri::InvertTri()
{
	sPolygon *Polygon=gb_RenderDevice->LockIndexBuffer(*ib);
	void *pVertex=gb_RenderDevice->LockVertexBuffer(*vb);
	int i;
	for(i=0;i<NumVertex;i++)
	{
		Vect3f &v=-GetNormal(pVertex,i+OffsetVertex);
		v=-v;
	}
	for(i=0;i<NumPolygon;i++)
	{
		sPolygon &p=Polygon[i+OffsetPolygon];
		int tmp=p.p1; p.p1=p.p2; p.p2=tmp;
	}
	gb_RenderDevice->UnlockVertexBuffer(*vb);
	gb_RenderDevice->UnlockIndexBuffer(*ib);
}
void cMeshTri::SetPosition(const MatXf &matrix)							
{ 
	void *pVertex=gb_RenderDevice->LockVertexBuffer(*vb);
	for(int i=0;i<NumVertex;i++)
		GetPos(pVertex,i+OffsetVertex)=matrix.xformPoint(GetPos(pVertex,i+OffsetVertex)); 
	gb_RenderDevice->UnlockVertexBuffer(*vb);
}

void cMeshTri::CalcBumpST()
{
	if(vb->size!=sizeof(sVertexDot3))
	{
		VISASSERT(0);
		return;
	}

	sVertexDot3 *v=(sVertexDot3*)gb_RenderDevice->LockVertexBuffer(*vb);
	sPolygon *Polygon=gb_RenderDevice->LockIndexBuffer(*ib);
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
		sPolygon &p=Polygon[i+OffsetPolygon];
		sVertexDot3 &v0=v[p.p1],&v1=v[p.p2],&v2=v[p.p3];

		// x, s, t
		edge01.set( v1.pos.x-v0.pos.x, v1.u1()-v0.u1(), v1.v1()-v0.v1() );
		edge02.set( v2.pos.x-v0.pos.x, v2.u1()-v0.u1(), v2.v1()-v0.v1() );

		cp.cross(edge01,edge02);
		if ( fabs(cp.x) > FLT_EPS )
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
		if ( fabs(cp.x) > FLT_EPS )
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
		if ( fabs(cp.x) > FLT_EPS )
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

	gb_RenderDevice->UnlockVertexBuffer(*vb);
	gb_RenderDevice->UnlockIndexBuffer(*ib);
}

bool cMeshTri::Intersect(const Vect3f& p0,const Vect3f& p1)
{
//	return true;
//*
	bool intersect=false;
	void *pVertex=gb_RenderDevice->LockVertexBuffer(*vb);
	sPolygon* pIndex=gb_RenderDevice->LockIndexBuffer(*ib);
	Vect3f pn=p1-p0;

	for(int i=0;i<NumPolygon;i++)
	{
		sPolygon &p=pIndex[i+OffsetPolygon];
		float t;
		Vect3f a=GetPos(pVertex,p.p1);
		Vect3f b=GetPos(pVertex,p.p2);
		Vect3f c=GetPos(pVertex,p.p3);

		if(Intersection(a,b,c,p0,p1,pn,t))
		{
			intersect=true;
			break;
		}
	}

	gb_RenderDevice->UnlockIndexBuffer(*ib);
	gb_RenderDevice->UnlockVertexBuffer(*vb);
	return intersect;
/*/
	bool intersect=false;
	Vect3f pn=p1-p0;

	for(int i=0;i<NumPolygon;i++)
	{
		sPolygon &p=ibSoft[i+OffsetPolygon];
		float t;
		Vect3f a=vbSoft[p.p1];
		Vect3f b=vbSoft[p.p2];
		Vect3f c=vbSoft[p.p3];

		if(Intersection(a,b,c,p0,p1,pn,t))
		{
			intersect=true;
			break;
		}
	}

	return intersect;
/**/
}