#pragma once
#include "observer.h"

class cAnimChannelNode;
class cScene;
class cAllMeshBank;
class cObjectNodeRoot;
class cMeshBank;
class cObjMesh;
class cObjLight;

enum eAttributeObjectNode
{
	ATTRNODE_ENABLEROTATEMATRIX	=	1<<0,
	ATTRNODE_ISGROUP			=	1<<1,
	ATTRNODE_IGNORE				=	1<<2,
	ATTRNODE_UPDATEMATRIX		=	1<<3,
	ATTRNODE_CURFRAME_ALPHA		=	1<<5,
	ATTRNODE_EFFECT				=	1<<6,
	ATTRNODE_EFFECT_CYCLED		=	1<<7,
	ATTRNODE_COPYBASE			=	1<<29,
};

struct sBound
{
	vector<Vect3f>		Vertex;
	vector<sPolygon>	Poly;
};

class cObjectGroup : public cBaseNode<class cObjectGroup>
{
protected:
	/*
		Введён для ускорения работы. 
		Так как груп значительно меньше, чем объектов.
	*/
	friend class cObjectNode;
	friend class cObjectNodeRoot;
	int			ChannelNumber;			// номер текущей анимационной цепочки
	float 		phase;
	cObjectNode* lod;
public:
	cObjectGroup();
	
protected:
	void SetPhase(float phase,bool recursive);
	void SetCurrentChannel(int nChannel);
};

class cObjectNode : public cIUnkObj, public cBaseNode <class cObjectNode>
{ // класс группы объектов
protected:
	friend class cObjLibrary;
	friend class cObjectNodeRoot;

	sAttribute			NodeAttribute;
	cAllMeshBank*		root;
	cObjectNodeRoot*	RootNode;

	cAnimChannelNode*	AnimChannel;
	cObjectGroup*		group;

	Mat3f				RotateMatrix;
	sBox6f				GlobalBound;
	class ShadowVolume* shadow;
public:

	cObjectNode(int kind=KIND_OBJ_NODE);
	virtual ~cObjectNode();
	// общие интерфейсные функции унаследованы от cUnkObj
	virtual void SetPosition(const MatXf& Matrix);

	//SetRotate(NULL) - запретить вращение, предпочтительнее, чем SetRotate(&Mat3f::ID);
	virtual void SetRotate(Mat3f* rotate);

	//specular->a -> 0 - цвет по умолчанию, 1 - устанавливаемый цвет.
	virtual void SetColor(const sColor4f *ambient,const sColor4f *diffuse,const sColor4f *specular=0);
	virtual float GetBoundRadius();

	virtual void GetBoundBox(sBox6f& Box);
	virtual cObjectNode* FindSubObject(const char *name);

	virtual const char* GetName() const;
	virtual cIUnkObj* BuildCopy();

	inline float GetPhase() const		{return group->phase;}
	virtual void SetPhase(float StartPhase,bool recursive=true);

	virtual int SetChannel(int NumberChain,bool recursive);
	virtual int SetChannel(const char *NameChainMask,bool recursive);
	virtual int GetCurrentChannel(int *nChannel,char *NameChain,float *Time=0);
	inline  int GetCurrentChannel()const{return group->ChannelNumber;};

	virtual int GetChannel(int nChannel,char *NameChain,float *Time=0);
	virtual int GetNumberChannel();
	const char* GetCurrentChannelName() const;
	
	virtual void ClearAttr(int attribute);
	virtual void SetAttr(int attribute);

	virtual void SetTexture(cTexture *Tex1,cTexture *Tex2);
	virtual void CalcObj();

	// функции cObjectNode
	void CalcMatrix();
	cObjectNode* FindObject(const char *name);						// поиск группы по имени объекта в группе или имени самой группы
	virtual cIUnkClass* NextObject(cIUnkClass *UObj);
	
	const char* GetNameObj() const;
	void SetGroup(bool is_group);

	// инлайновые функции доступа к переменным
	inline cAnimChannelNode* GetAnimChannel()					{ return AnimChannel; }
	void SetAnimChannel(cAnimChannelNode* p)					{ AnimChannel=p; }

	// общие интерфейсные функции унаследованы от cUnkObj
	void AttachChild(cObjectNode *child);						// присоединение ребенка к дереву 
	void DetachChild(cObjectNode *child);						// отсоединение ребенка от дерева 
	
	inline cObjectNode* GetSibling() const						{ return Sibling; }
	inline cObjectNode* GetChild() const						{ return Child; }
	inline cObjectNode* GetParentNode()	const					{ return Parent; }
	inline cObjectNodeRoot* GetRootNode() const					{ return RootNode; }

	inline const char* GetParentNodeName()						{ if(GetParentNode()) return GetParentNode()->GetNameObj(); return 0; }

	virtual void PreDraw(cCamera *UCamera);

	virtual bool Intersect(const Vect3f& p0,const Vect3f& p1);

	virtual void DrawShadow(cCamera *DrawNode);
	inline bool IsGroup() {return NodeAttribute.GetAttribute(ATTRNODE_ISGROUP);}

	virtual void ChangeBank(cAllMeshBank* new_root);
	cAllMeshBank* GetRoot(){return root;}
protected:
	void SetRoot(cAllMeshBank* p){VISASSERT(p && root==NULL);root=p;}
	inline void SetParent(cObjectNode *parent)					{ Parent=parent; }
	void SetRootNode(cObjectNodeRoot* p)						{ RootNode=p; }
	bool IntersectSphere(const Vect3f& p0,const Vect3f& p1);

	virtual void CalcBorder();
	virtual void CalcBorder(Vect3f& Min,Vect3f& Max);
	
	virtual void GetBoundingBox(Vect3f &min,Vect3f &max);

	//Для void SetTexture(cTexture *Tex1,cTexture *Tex2);
	struct AnotherMaterial
	{
		class cMeshBank* pOriginalMaterial;
		class cMeshBank* pCurMaterial;
	};

	class AllAnotherMaterial
	{
		vector<AnotherMaterial> materials;
	public:
		cTexture *pTex1,*pTex2;

		AllAnotherMaterial(cTexture *Tex1,cTexture *Tex2);
		~AllAnotherMaterial();
		cMeshBank* Find(cMeshBank* pOriginalMaterial);
		cMeshBank* Add(cMeshBank* pOriginalMaterial);
	};
	virtual void RestoreOriginalMaterial();
	virtual void SetAnotherMaterial(cTexture *Tex1,cTexture *Tex2,AllAnotherMaterial* aroot);

	virtual void SetCopy(cIUnkObj* UObj);

	void AddChild(vector<cObjectNode*>& all_child);

	int GetNumGroup();
	void SetGroup(int& cur_num,vector<cObjectGroup>& groups,cObjectGroup* cur_group);
	void UpdateMatrix();

	virtual void AddShadow(MatXf mat,class cMeshTri* Tri);
	virtual void BuildShadow();

	bool prev_visible;
	class cEffect* effect;
};

class cMeshSortingPhase
{
	cObjMesh*	pMeshFirst;
public:
	cMeshBank*	pBank;
	int	channel;
	int	attribute;

	float		phase;
	DWORD		diffuse;
	DWORD		ambient;

	cObjectNodeRoot* root;

	inline cMeshSortingPhase(){pMeshFirst=NULL;}
	inline void Attach(cObjMesh* p);
	inline cObjMesh* GetFront(){return pMeshFirst;};

	void GetMaterial(struct sDataRenderMaterial* data);
};
struct cMeshSortingBank
{
	cMeshBank*	pBank;
	vector<cMeshSortingPhase> phase;
};

class cObjectNodeRoot : public cObjectNode,public cEmitter3dObject
{
protected:
	friend class cObjLibrary;
	AllAnotherMaterial* AnnoterRoot;
	string				fname;		// имя файла из которого загружен
	sBound*				Base;
	Vect3f				Scale;	// масштаб
	vector<cObjectNode*> all_child;
	vector<cObjMesh*>	 mesh_child;
	vector<cObjLight*>	 light_child;
	double			MaterialAnimTime;

	vector<cObjectGroup> groups;

	sColor4f			Diffuse;
	sColor4f			Specular;
	sColor4f			Ambient;
	sColor4f			SkinColor;

	vector<cMeshSortingBank*> DrawSort;

	cObjectNodeRoot* RootLod;

	sColor4f LightIntensity;
	vector<class cUnkLight*> point_light;

	Observer observer;
public:
	cObjectNodeRoot();
	~cObjectNodeRoot();
	
	virtual void SetPosition(const MatXf& Matrix);
	virtual void Animate(float dt);
	virtual void PreDraw(cCamera *DrawNode);
	virtual void GetLocalBorder(int *nVertex,Vect3f **Vertex,int *nIndex,short **Index);
	
	virtual const Vect3f& GetScale() const		{ return Scale; }
	virtual void SetScale(const Vect3f& scale);
	virtual cIUnkObj* BuildCopy();

	void SetTexture(cTexture *Tex1,cTexture *Tex2);

	void SetBase(sBound *Base);
	inline sBound* GetBase(){return Base;}

	inline double GetMaterialAnimTime(){return MaterialAnimTime;}

	inline const sColor4f& GetDiffuse(){return Diffuse;};
	inline const sColor4f& GetSpecular(){return Specular;};
	inline const sColor4f& GetAmbient(){return Ambient;};
	inline const sColor4f& GetSkinColor(){return SkinColor;}
	virtual void SetColor(const sColor4f *ambient,const sColor4f *diffuse,const sColor4f *specular=0);
	
	void SetSkinColor(const sColor4f& color);
	const sColor4f& GetSkinColor() const { return SkinColor; }

	//info
	void SetFileName(const char *fName);
	inline const char* GetFileName() const		{ return fname.c_str(); }
	int GetMaterialNum();
	int GetObjectNum();

	virtual void ClearAttr(int attribute);
	virtual void SetAttr(int attribute);

	virtual void Update();

	inline vector<cObjMesh*>& GetMeshChild(){return mesh_child;}

	void RootLodRelease(){RELEASE(RootLod);}

	//Функции упрощённого доступа к геометрии объекта
	void GetAllPoints(vector<Vect3f>& point);
	void GetAllNormals(vector<Vect3f>& point);
	void GetAllTriangle(vector<Vect3f>& point,vector<sPolygon>& polygon);

	void GetEmitterMaterial(cObjMaterial& material);

	/////////////////////////////////
	virtual void DrawShadow(cCamera *DrawNode);
	virtual void DrawBadUV(cCamera *DrawNode);

	inline const sColor4f& GetLightIntensity(){return LightIntensity;}
	void AddLightIntensity(const sColor4f& c){LightIntensity=c;if(RootLod)RootLod->AddLightIntensity(c);}
	void AddLight(cUnkLight* light);
	vector<class cUnkLight*>& GetLight(){return point_light;}

	void AddLink(ObserverLink* link){observer.AddLink(link);}
	void BreakLink(ObserverLink* link){observer.BreakLink(link);}

	void ChangeBank(cAllMeshBank* new_root);

	void OcclusionTest();

protected:
	virtual void SetCopy(cIUnkObj* UObj);
	void BuildChild();
	void BuildGroup();
	void PreDrawSort(cCamera *DrawNode);
	void ClearDrawSort();
	void BuildShadow();
	void SynchronizeGroupLod();
};
