#pragma once
#include "AnimationData.h"
struct NextLevelNode
{
	IGameNode* node;
	IGameNode* pParent;
	int current;
	int parent;
};

class RootExport
{
public:
	RootExport();
	~RootExport();
	void Init(IGameScene * pIgame);

	bool LoadData(const char* filename);
	void Export(const char* filename,bool logic);

	IGameNode* Find(const char* name);

	int GetNumFrames();

	int FindMaterialIndex(IGameMaterial* mat);

	int FindNodeIndex(IGameNode* node);
public:
	IGameScene * pIgame;
	AnimationData animation_data;
protected:
	CSaver saver;
	bool export_logic;

	TimeValue time_start,time_end,time_frame;
	vector<IGameMaterial*> materials;
	IGameNode* node_base_;

	typedef map<IGameNode*,int> NodeMap;
	NodeMap node_map;
	vector<NextLevelNode> all_nodes;

	void SaveNodes();

	void ExportNode(IGameNode * pGameNode,IGameNode* pParent,int node,int parent);
	void ExportMatrix(IGameNode* node,IGameNode* pParent,int interval_begin,int interval_size,bool cycled);

	IGameNode* FindRecursive(IGameNode* pGameNode,const char* name);

	bool IsIgnore(IGameNode* pGameNode,bool root);
	void SaveMaterials();
	void CalcNodeMap();
	void SaveBasement();
	void SaveMeshes();
	void SaveLogicBound();

	void SaveVisibility(vector<bool>& visibility,int interval_begin,int interval_size,bool cycled);
};

extern class RootExport* pRootExport;