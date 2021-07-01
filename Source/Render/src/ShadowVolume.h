#pragma once

class ShadowVolume:public cUnknownClass
{
	struct sv_triangle
	{
		int v[3];//vertex
		sPlane4f n;
	};

	struct sv_edge
	{
		int e[2];  // vertex index
		int w[2];  // triangle index: for "open" models, w[1] == -1 on open edges
	};

	std::vector<Vect3f> vertex;
	std::vector<sv_triangle> triangle;
	std::vector<sv_edge> edge;
public:
	ShadowVolume();
	~ShadowVolume();

	bool Add(MatXf& mat,class cMeshTri* pTri);
	void EndAdd();

	void Draw(cCamera *camera,const MatXf& m,Vect3f light_dir,bool line);
protected:
	void DeleteRepeatedVertex(int offset_vertex,int size_vertex,int offset_poly,int size_poly);
	int ComputeWingedEdges();
	void AddEdge(sv_edge& we);

	void DrawEdge(cCamera *camera,const MatXf& m,Vect3f light_dir);
	void DrawVolume(cCamera *camera,const MatXf& m,Vect3f light_dir,bool line);
};