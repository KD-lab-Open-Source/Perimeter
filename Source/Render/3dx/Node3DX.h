#pragma once
#include "Static3dx.h"

struct cNode3dx
{
	MatXf pos;
	float phase;
	BYTE chain;
	BYTE index_scale;
	BYTE index_position;
	BYTE index_rotation;
	BYTE additional_transform;//255 - нет добавки
	bool IsAdditionalTransform(){return additional_transform!=255;}

	cNode3dx();
};

class cObject3dx:public cIUnkClass,public cEmitter3dObject
{//root объект через который происходит всё взаимодействие
public:
	cObject3dx(cStatic3dx* pStatic);
	~cObject3dx();

	void SetPosition(const Se3f& pos);
	void SetPosition(const MatXf& Matrix);
	const MatXf& GetPosition() const;

	void SetScale(float scale);
	void SetPhase(float phase);
	float GetScale()const{return scale;}

	float GetBoundRadius();
	void GetBoundBox(sBox6f& Box);

	void PreDraw(cCamera *UCamera);
	void Draw(cCamera *UCamera);
	void Animate(float dt);

	void DrawLine(cCamera* pCamera);

	int GetAnimationGroup(const char* name);
	int GetAnimationGroupNumber();
	const char* GetAnimationGroupName(int ianimationgroup);
	void SetAnimationGroupPhase(int ianimationgroup,float phase);
	void SetAnimationGroupChain(int ianimationgroup,const char* chain_name);
	void SetAnimationGroupChain(int ianimationgroup,int chain_index);
	int GetAnimationGroupChain(int ianimationgroup);

	bool SetVisibilityGroup(const char* name);
	cStaticVisibilityChainGroup* GetVisibilityGroup(){return pGroup;}

	int GetChainNumber();
	cAnimationChain* GetChain(int i);
	bool SetChain(const char* chain);

	//Intersect - Стала в текущей версии существенно медленнее за счёт скиннинга
	//не абсолютно точная функция, если объект выйдет за пределы своего bound даст ошибку.
	bool Intersect(const Vect3f& p0,const Vect3f& p1);
	const char* GetFileName();

	cStatic3dx* GetStatic(){return pStatic;}//только для чтения

	//nodeindex - индекс из массива cStatic3dx::nodes
	int FindNode(const char* node_name);//индекс в cStatic3dx::nodes
	const MatXf& GetNodePosition(int nodeindex);//Валидные данные только после первого Update, возвращает положение относительно глобальной системы координат
	void SetUserTransform(int nodeindex,const MatXf& pos);//Не оптимально написанна, если трансформация бодет для многих node
	//RestoreNodeMatrix отменяет действие SetNodeUserPosition
	void RestoreUserTransform(int nodeindex);

	void SetSkinColor(sColor4f skin_color);//Дорогая функция, грузит текстуры
	sColor4c GetSkinColor(){return skin_color;};
	void SetColor(const sColor4f *ambient,const sColor4f *diffuse,const sColor4f *specular=0);
	void GetColor(sColor4f *ambient,sColor4f *diffuse,sColor4f *specular=0) const;

	void GetLocalBorder(int *nVertex,Vect3f **Vertex,int *nIndex,short **Index);

	void DrawLogic();
	void DrawLogicBound();

	void Update();

	void AddLink(ObserverLink* link){observer.AddLink(link);}
	void BreakLink(ObserverLink* link){observer.BreakLink(link);}

	void GetAllPoints(vector<Vect3f>& point);
	void GetAllNormals(vector<Vect3f>& point);
	void GetEmitterMaterial(struct cObjMaterial& material);

	void GetAllPointsParam(vector<Vect3f>& point,bool zero_pos);
protected:
	cStatic3dx* pStatic;
	cStaticVisibilityChainGroup* pGroup;
	float scale;
	MatXf position;
	sBox6f box;
	float radius;
	//порядок хранения cNode3dx таков, что 
	//child всегда находится после parent
	vector<cNode3dx> nodes;

	sColor4c skin_color;
	vector<cTexture*> skin_texture;//Индекы такие-же как у материала.

	sColor4f ambient,diffuse,specular;

	struct AdditionalTransformation
	{
		int nodeindex;
		MatXf mat;
	};
	vector<AdditionalTransformation> additional_transformations;
	struct EffectData
	{
		cEffect* pEffect;
		BYTE index_visibility;
		float prev_phase;
		EffectData():pEffect(0),index_visibility(255){}//255 - неинициализированн.
	};
	vector<EffectData> effects;
	Observer observer;

	void UpdateMatrix();
	void CalcBoundingBox();

	int FindChain(const char* chain_name);
	void LoadTexture(bool skinned);
	void ProcessEffect(cCamera *pCamera);

	bool IntersectTriangle(const Vect3f& p0,const Vect3f& p1);
};