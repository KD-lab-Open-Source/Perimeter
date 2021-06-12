#pragma once

class cObjLight : public cObjectNode
{
	float			FarAttenuation;
	float			Intensity;
	class cAnimChannelMaterial  *AnimChannelMat;

	cTexture		*Texture;
	sColor4f		Diffuse;

	double fade;
	double prev_time;
	cOcclusionQuery occlusion;
public:
	cObjLight();
	virtual ~cObjLight();
	// общие интерфейсные функции унаследованы от cUnkObj
	virtual void PreDraw(cCamera *UCamera);
	virtual void Draw(cCamera *UCamera);
	virtual cIUnkObj* BuildCopy();
	virtual void SetColor(const sColor4f *ambient,const sColor4f *diffuse,const sColor4f *specular=0);
	virtual void SetAttr(int attribute);
	// общие интерфейсные функции унаследованы от cUnkTile
	virtual void GetBoundingBox(Vect3f &min,Vect3f &max);
	// инлайновые функции доступа к переменным
	__forceinline float& GetFarAttenuation()								{ return FarAttenuation; }
	__forceinline float& GetIntensity()										{ return Intensity; }
	
	cAnimChannelMaterial* GetAnimChannelMat()								{return AnimChannelMat;}
	void SetAnimChannelMat(cAnimChannelMaterial* p)							{AnimChannelMat=p;}

	inline const sColor4f& GetDiffuse()	const 								{ return Diffuse; }
	virtual void CalcBorder(Vect3f& Min,Vect3f& Max);

	inline cTexture* GetTexture()											{ return Texture; }
	void SetTextureLight(cTexture *pTexture,int attr=0);

	virtual void ChangeBank(cAllMeshBank* new_root);

	void OcclusionTest();
};

