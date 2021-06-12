#pragma once

#include "ShadowVolume.h"

class cMeshTri
{
public:
	sPtrIndexBuffer			*ib;
	sPtrVertexBuffer		*vb;
	int			OffsetPolygon;
	int			OffsetVertex;
	int			NumPolygon;
	int			NumVertex;

	cMeshTri();
	~cMeshTri();
	inline int GetNumberVertex()							{ return NumVertex; }
	inline int GetOffsetVertex()							{ return OffsetVertex; }
	inline int GetNumberPolygon()							{ return NumPolygon; }
	inline int GetOffsetPolygon()							{ return OffsetPolygon; }

	inline Vect3f& GetPos(void *pVertex,int n)				{ return ((Vect3f*)((char*)pVertex+n*vb->size))[0]; }
	inline Vect3f& GetNormal(void *pVertex,int n)			{ return ((Vect3f*)((char*)pVertex+n*vb->size))[1]; }
	inline Vect2f& GetTexel(void *pVertex,int n)			{ return ((Vect2f*)((char*)pVertex+n*vb->size))[3]; }
	inline sVertexXYZNT1& GetVertex(void *pVertex,int n)	{ return ((sVertexXYZNT1*)((char*)pVertex+n*vb->size))[0]; }
	void SetPosition(const MatXf &matrix);	// матрица действует на вершины объекта (полигональную решетку)
	void GetBoundingBox(Vect3f &min,Vect3f &max);	// возвращает бокс-баунд
	void CalcBumpST();
	void InvertTri();

	bool Intersect(const Vect3f& p0,const Vect3f& p1);
};
