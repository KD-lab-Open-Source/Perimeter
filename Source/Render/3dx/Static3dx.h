#pragma once

#include "Interpolator3DX.h"

struct cStaticNodeChain
{
	Interpolator3dx<1> scale;
	Interpolator3dx<3> position;
	Interpolator3dx<4> rotation;
	Interpolator3dxBool visibility;
};

struct cStaticNode
{
	string name;
	int inode;
	int iparent;
	vector<cStaticNodeChain> chains;
	void LoadNode(CLoadDirectory ld);
	void LoadNodeChain(CLoadDirectory ld);
};

struct cAnimationChain
{
	string name;
	float time;
};

struct cStaticMesh
{
	int		offset_polygon;
	int		offset_vertex;
	int		num_polygon;
	int		num_vertex;

	int		inode;
	int		imaterial;
};

struct cStaticIndex
{
	int		offset_polygon;
	int		num_polygon;

	int		offset_vertex;
	int		num_vertex;

	int		imaterial;

	enum
	{
		max_index=20,//Максимальное количество матриц
	};
	//node_index с какими матрицами связанны текущие полигоны,
	//не более max_index
	vector<int> node_index;
};

struct AnimationGroup
{
	string name;
	vector<int> nodes;
	vector<string> temp_nodes_name;

	void Load(CLoadDirectory rd);
};

struct cStaticMaterial
{
	string name;

	sColor4f ambient;
	sColor4f diffuse;
	sColor4f specular;
	float opacity;
	float specular_power;

	string tex_diffuse;
	bool is_skinned;
	cTexture* pBumpTexture;

	cStaticMaterial();
	~cStaticMaterial();
	void Load(CLoadDirectory rd,const char* path_name);
};

struct cTempBone
{
	enum
	{
		max_bones=4,
	};

	float weight[max_bones];
	int inode[max_bones];
};

struct cTempMesh3dx
{
	vector<Vect3f> vertex_pos;
	vector<Vect3f> vertex_norm;
	vector<Vect2f> vertex_uv;
	vector<sPolygon> polygons;
	vector<cTempBone> bones;

	int inode;
	int imaterial;

	vector<int> inode_array;

	void Load(CLoadDirectory rd);

	int CalcMaxBonesPerVertex();
};

struct cStaticBasement
{
	vector<sPolygon> polygons;
	vector<Vect3f> vertex;

	void Load(CLoadDirectory rd);
};

struct cStaticLogicBound
{
	int inode;
	sBox6f bound;

	cStaticLogicBound()
	{
		inode=0;
		bound.min=Vect3f::ZERO;
		bound.max=Vect3f::ZERO;
	}

	void Load(CLoadIterator it);
};

struct cStaticVisibilityChainGroup
{
	string name;

	vector<cStaticMesh> meshes;
	int					ib_polygon;//Количество полигонов в ib
	int					vb_size;//Количество вертексов в vb
	sPtrIndexBuffer		ib;
	sPtrVertexBuffer	vb;
	int			blend_indices;//количество костей в vb
	vector<cStaticIndex> skin_group;

	vector<string>		temp_invisible_object;
	bool bump;

	cStaticVisibilityChainGroup();

	void BuildSkinGroup(vector<cTempMesh3dx*>& temp_mesh);
	int GetBlendWeight()
	{
		if(blend_indices==1)
			return 0;
		return blend_indices;
	}

	void CalcBumpST();
};

struct cStaticEffect
{
	int node;
	bool is_cycled;
	class EffectKey* effect_key;
};

//Класс, хранящий в себе неизменные данные для всех 3dx объектов.
class cStatic3dx
{
public:
	cStatic3dx(bool is_logic_,const char* file_name_);
	~cStatic3dx();

	void Load(CLoadDirectory& rd);
	string file_name;

	vector<cStaticNode> nodes;
	vector<cAnimationChain> animation_chain;
	vector<cStaticVisibilityChainGroup*> visibility_groups;

	vector<cStaticMaterial> materials;
	vector<AnimationGroup> animation_group;

	cStaticBasement basement;
	cStaticLogicBound logic_bound;
	bool is_logic;
	vector<cStaticEffect> effects;

	class cVisError& errlog();
protected:

	void LoadChainData(CLoadDirectory rd);
	void LoadGroup(CLoadDirectory rd);
	void LoadChain(CLoadDirectory rd);
	void LoadChainGroup(CLoadDirectory rd);

	void LoadNodes(CLoadDirectory rd);

	void LoadMeshes(CLoadDirectory rd);
	void LoadMesh(CLoadDirectory rd);

	int LoadMaterialsNum(CLoadDirectory rd);
	void LoadMaterials(CLoadDirectory rd,int num_materials);
	void LoadMaterial(CLoadDirectory rd);

	void SplitMesh(cTempMesh3dx* mesh,vector<cTempMesh3dx*>& split_mesh);

	void BuildChainGroup(vector<cTempMesh3dx*>& temp_mesh,cStaticVisibilityChainGroup* chain_group);
	void DummyVisibilityGroup();
	void ParseEffect();
};
