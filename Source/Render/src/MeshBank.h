#ifndef _MESHBANK_H_
#define _MESHBANK_H_

#include "AnimChannel.h"


class cMeshTri;

struct cMeshStatic:public cUnknownClass
{
	sPtrIndexBuffer				ib;
	sPtrVertexBuffer			vb;
	int							ib_polygon;//Количество полигонов в ib
	string						MaterialName;
	vector<cMeshTri*>			meshes;
	sAttribute				Material;
	cAnimChannelMaterial    AnimChannelMat;
	//ATTRUNKOBJ_COLLISIONTRACE ATTRUNKOBJ_NOLIGHT
	sAttribute				ObjectAttribute;
	struct TEMP
	{
		vector<sVertexXYZNT1> vertex;
		vector<sPolygon> polygons;
	};

	TEMP* temp;

	//////////////////////////////
	cMeshStatic(const char* materialname);
	~cMeshStatic();

	void BeginBuildMesh();
	cMeshTri* AddMesh(vector<Vect3f> &Vertex,vector<sPolygon> &Polygon,vector<sPolygon> &TexPoly,vector<Vect2f> &Texel);
	cMeshTri* AddMesh(vector<Vect3f> &Vertex,vector<sPolygon> &Polygon,vector<Vect3f> &Normal,vector<Vect2f> &Texel);

	void EndBuildMesh(bool bump);
protected:
	inline sVertexXYZNT1& GetVertex(void *pVertex,int n)	{ return ((sVertexXYZNT1*)((char*)pVertex+n*vb.size))[0]; }
	void SortPolygon(sPolygon* polygon,int n_polygon);
	void DeleteSingularPolygon(vector<Vect3f> &Vertex,vector<sPolygon> &Polygon,
								vector<sPolygon> &TexPoly,vector<Vect2f> &Texel);
};


class cMeshBank
{
	cMeshStatic*	bank;
	cAllMeshBank*	parent;
	cTexture		*Texture[NUMBER_OBJTEXTURE];

	cMeshBank();
public:
	cMeshBank(const char* materialname,cAllMeshBank* parent);
	virtual ~cMeshBank();

	void BeginBuildMesh();
	cMeshTri* AddMesh(vector<Vect3f> &Vertex,vector<sPolygon> &Polygon,vector<sPolygon> &TexPoly,vector<Vect2f> &Texel);
	cMeshTri* AddMesh(vector<Vect3f> &Vertex,vector<sPolygon> &Polygon,vector<Vect3f> &Normal,vector<Vect2f> &Texel);
	void EndBuildMesh();

	inline const char* GetMaterialName(){return bank->MaterialName.c_str();}
	inline float GetRealAlpha(int nChannel,float phase,const sColor4f& Diffuse);

	void GetMaterial(int nChannel,float phase,int attribute,
		const sColor4f& Ambient,const sColor4f& Diffuse,const sColor4f& Specular,
		cObjMaterial& Mat);
	
	void GetMaterial(int nChannel,float phase,double MaterialAnimTime,int attribute,
					const sColor4f& Ambient,const sColor4f& Diffuse,const sColor4f& Specular,
					sDataRenderMaterial& data);

	void GetMaterial(int nChannel,float phase,double MaterialAnimTime,
					cObjMaterial& Mat,
					sDataRenderMaterial& data);

	void SetTexture(int n,cTexture* texture,int attr=0);
	cTexture* GetTexture(int n){VISASSERT(n>=0 && n<NUMBER_OBJTEXTURE);return Texture[n];}
	sAttribute* GetMaterial(){return &bank->Material;};
	cAnimChannelMaterial* GetAnimChannelMat(){return &bank->AnimChannelMat;};
	sAttribute&	GetObjectAttribute(){return bank->ObjectAttribute;};

	cMeshBank* BuildStaticCopy();//Копия материала без анимации
	cMeshBank* BuildCopyWithAnotherTexture(const char* texture_path,const char* def_texture_path,cAllMeshBank* parent);
	bool IsBump();
	cMeshStatic* GetBank(){return bank;}
};

/*
	Класс в котором хранится общая по загруженному M3D информация.
*/
class cAllMeshBank : public cUnknownClass
{
	string filename;
	string texture_path;
	string def_texture_path;

	vector<cMeshBank*> meshes;
	vector<sChannelHeader> Channel;
public:
	cObjectNodeRoot* root;

	cAllMeshBank(const char* file_name,const char* texture_path);
	virtual ~cAllMeshBank();

	//Функции загрузки
	void SetFrame(class cMeshScene *MeshScene);
	void BeginLoad();
	void EndLoad();

	cMeshBank* FindUnical(const char* materialname,sAttribute ObjectAttribute,sAttribute MatAttribute);
	cMeshBank* CreateUnical(const char* materialname,sAttribute ObjectAttribute,sAttribute MatAttribute);
	//Функции доступа
	const char* GetFileName(){return filename.c_str();};
	const char* GetTexturePath(){return texture_path.c_str();};
	const char* GetDefTexturePath(){return def_texture_path.c_str();};
	inline int GetNumberChannel()					{ return Channel.size(); }
	inline void SetNumberChannel(int nChannel)		{ Channel.resize(nChannel); }
	inline sChannelHeader& GetChannel(int nChannel)	{ return Channel[nChannel]; }

	inline int GetNumberMeshBank()					{return meshes.size();}
	inline cMeshBank* GetMeshBank(int i)			{return meshes[i];}

	cAllMeshBank* BuildCopyWithAnotherTexture(const char* texture_path,const char* def_texture_path);
protected:
	void Free();
};

/////////////////////inline/////////////////////
inline float cMeshBank::GetRealAlpha(int nChannel,float phase,const sColor4f& Diffuse)
{
	float a;
	cAnimChainMaterial* pChannel=bank->AnimChannelMat.GetChannel(nChannel);
	a=pChannel->GetTransparency(phase);
	return Diffuse.a*a;
}

cTexture* TextureWithAnnoterPath(cTexture* pTexture,const char* annoter_path,const char* def_texture_path);

#endif  _MESHBANK_H_
