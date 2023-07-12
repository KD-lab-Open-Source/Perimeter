#pragma once

#include "VertexFormat.h"
#include "ShadowVolume.h"

class cMeshTri
{
public:
    //Reference to DrawBuffer containing all meshes data loaded in GPU
	DrawBuffer* db;
    
    //Range of db which contains this mesh data
    DrawRange dbr;
    
    //Reference to cMeshStatic data
    sVertexXYZNT1* VertexBuffer;
    sPolygon* PolygonBuffer;
    
    //Info about what part of cMeshStatic this belongs to
	int OffsetPolygon;
	int OffsetVertex;
	int NumPolygon;
	int NumVertex;

	cMeshTri();
	~cMeshTri();

	inline const Vect3f& GetPos(size_t n) const { return VertexBuffer[n].pos; }
	inline const Vect3f& GetNormal(size_t n) const { return VertexBuffer[n].n; }
	inline const sVertexXYZNT1& GetVertex(size_t n) const { return VertexBuffer[n]; }
    
	void GetBoundingBox(Vect3f &min,Vect3f &max);	// возвращает бокс-баунд
	void CalcBumpST();
	void InvertTri();

	bool Intersect(const Vect3f& p0,const Vect3f& p1);
};
