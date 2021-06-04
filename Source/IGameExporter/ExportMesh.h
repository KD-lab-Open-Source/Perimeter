#pragma once

class ExportMesh
{
public:
	ExportMesh(CSaver& saver,const char* name_);

	void Export(IGameMesh* pobject,int inode,Matrix3 node_matrix);
protected:
	CSaver& saver;
	IGameMesh* pobject;
	const char* name;
	int inode_current;

	Matrix3 node_matrix;
	Matrix3 inv_node_matrix;
	Matrix3 inv_node_norm;

	bool ExportOneMesh();

	enum
	{
		max_bones=4,
	};

	struct sVertex
	{
		Vect3f pos;
		Vect3f norm;
		Vect2f uv;

		float bones[max_bones];
		int bones_inode[max_bones];
	};

	struct sPolygon
	{
		int p[3];
	};

	void DeleteSingularPolygon(vector<FaceEx>& faces);
	void SortPolygon(sPolygon* polygon,int n_polygon);

	int NumCashed(sPolygon* polygon,int n_polygon);
};