#pragma once

#include "VertexFormat.h"
#include "ShadowVolume.h"

class cMeshTri
{
public:
	DrawBuffer* db;
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

	inline Vect3f& GetPos(void *pVertex,int n)				{ return ((Vect3f*)((char*)pVertex+n*db->vb.VertexSize))[0]; }
	inline Vect3f& GetNormal(void *pVertex,int n)			{ return ((Vect3f*)((char*)pVertex+n*db->vb.VertexSize))[1]; }
	inline sVertexXYZNT1& GetVertex(void *pVertex,int n)	{ return ((sVertexXYZNT1*)((char*)pVertex+n*db->vb.VertexSize))[0]; }
	void SetPosition(const MatXf &matrix);	// матрица действует на вершины объекта (полигональную решетку)
	void GetBoundingBox(Vect3f &min,Vect3f &max);	// возвращает бокс-баунд
	void CalcBumpST();
	void InvertTri();

	bool Intersect(const Vect3f& p0,const Vect3f& p1);
};
