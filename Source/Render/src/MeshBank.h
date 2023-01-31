#ifndef _MESHBANK_H_
#define _MESHBANK_H_

#include "AnimChannel.h"


class cMeshTri;

struct cMeshStatic:public cUnknownClass
{
	DrawBuffer				db;
	std::string					MaterialName;
	std::vector<cMeshTri*>		meshes;
	sAttribute				Material;
	cAnimChannelMaterial    AnimChannelMat;
	//ATTRUNKOBJ_COLLISIONTRACE ATTRUNKOBJ_NOLIGHT
	sAttribute				ObjectAttribute;
    std::vector<sVertexXYZNT1> vertexes;
    std::vector<sPolygon> polygons;

	//////////////////////////////
	explicit cMeshStatic(const char* materialname);
	~cMeshStatic();

	void BeginBuildMesh();
	cMeshTri* AddMesh(std::vector<Vect3f> &Vertex, std::vector<sPolygon> &Polygon, std::vector<sPolygon> &TexPoly, std::vector<Vect2f> &Texel);
	cMeshTri* AddMesh(std::vector<Vect3f> &Vertex, std::vector<sPolygon> &Polygon, std::vector<Vect3f> &Normal, std::vector<Vect2f> &Texel);
	void EndBuildMesh(bool bump);
    
protected:
	void SortPolygon(sPolygon* polygon,int n_polygon);
	void DeleteSingularPolygon(std::vector<Vect3f> &Vertex, std::vector<sPolygon> &Polygon,
		std::vector<sPolygon> &TexPoly, std::vector<Vect2f> &Texel);
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
	cMeshTri* AddMesh(std::vector<Vect3f> &Vertex, std::vector<sPolygon> &Polygon, std::vector<sPolygon> &TexPoly, std::vector<Vect2f> &Texel);
	cMeshTri* AddMesh(std::vector<Vect3f> &Vertex, std::vector<sPolygon> &Polygon, std::vector<Vect3f> &Normal, std::vector<Vect2f> &Texel);
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
	std::string filename;
	std::string texture_path;
	std::string def_texture_path;

	std::vector<cMeshBank*> meshes;
	std::vector<sChannelHeader> Channel;
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

#endif //_MESHBANK_H_
