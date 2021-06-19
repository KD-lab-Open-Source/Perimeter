#pragma once

struct AEdge
{
	int vertex[2];
	std::set<int> face;
};

struct AFace
{
	std::set<int> edge;
};

struct AMesh
{
	std::vector<Vect3f> V;
	std::vector<AEdge> E;
	std::vector<AFace> F;

	void CreateABB(Vect3f& vmin,Vect3f& vmax);
};

struct CVertex
{
	Vect3f point;
	float distance;
	int occurs;
	bool visible;

	CVertex(){	distance=0;occurs=0;visible=true;}
};

struct CEdge:public AEdge
{
	bool visible;
	CEdge(){visible=true;}
};

struct CFace:public AFace
{
	bool visible;
	CFace(){visible=true;}
};
/*
struct Plane
{
	float A,B,C,D;

	inline float distance(const Vect3f& p)
	{
		return A*p.x+B*p.y+C*p.z+D;
	}
};
*/
struct APolygons
{
	std::vector<Vect3f> points;
	std::vector<std::vector<int> > faces;
};

struct CMesh
{
	std::vector<CVertex> V;
	std::vector<CEdge> E;
	std::vector<CFace> F;
public:
	CMesh(AMesh& mesh);	
	CMesh();
	void Set(AMesh& mesh);

	int Clip(sPlane4f clipplane);

	void BuildPolygon(APolygons& p);
protected:
	float epsilon;
	bool GetOpenPolyline(const CFace& face,int& start,int& final);
};
