#pragma once

class cMeshTri;
class cMeshBank;

class cObjMesh : public cObjectNode
{
	cMeshTri*		Tri;// геометрия объекта, хранится в cMeshBank
	cMeshBank*		pBank;
	cMeshBank*		pOriginalBank;

	friend class cMeshSortingPhase;
	cObjMesh*		pSortingNext;//используется в cMeshSortingBank,cMeshSortingPhase
public:
	
	cObjMesh();
	virtual ~cObjMesh();
	// общие интерфейсные функции унаследованы от cUnkObj
	void PreDraw(cCamera *UCamera) override;
	void Draw(cCamera *UCamera) override;
	cIUnkObj* BuildCopy() override;
	void SetColor(const sColor4f *ambient,const sColor4f *diffuse,const sColor4f *specular=0) override;
	void CalcObj() override;
	// общие интерфейсные функции унаследованы от cUnkTile
	void GetBoundingBox(Vect3f &min,Vect3f &max) override;
	// инлайновые функции доступа к переменным
	inline void SetTri(cMeshTri	*MeshTri)							{ Tri=MeshTri; }
	inline cMeshTri* GetTri()										{ return Tri; }
	inline cMeshBank* GetBank()										{return pBank;};
	void SetBank(cMeshBank*	pBank,bool is_original);
	inline void RestoreOriginalBank()								{pBank=pOriginalBank;};

	void CalcBorder(Vect3f& Min,Vect3f& Max) override;

	void RestoreOriginalMaterial() override;
	void SetAnotherMaterial(cTexture *Tex1,cTexture *Tex2,AllAnotherMaterial* aroot) override;
	inline const sColor4f& GetDiffuse(){return RootNode->GetDiffuse();};
	inline const sColor4f& GetSpecular(){return RootNode->GetSpecular();};
	inline const sColor4f& GetAmbient(){return RootNode->GetAmbient();};

	cObjMesh* GetNextSorting(){return pSortingNext;}

	bool Intersect(const Vect3f& p0,const Vect3f& p1) override;

	void GetAllPoints(std::vector<Vect3f>& point);
	void GetAllNormals(std::vector<Vect3f>& point);
	int GetAllTriangle(std::vector<Vect3f>& point, std::vector<indices_t>& indices);
	void ChangeBank(cAllMeshBank* new_root) override;
	virtual void DrawBadUV(cCamera *DrawNode);
protected:
	void BuildShadow() override;
};

