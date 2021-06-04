#pragma once

struct ConstShaderInfo
{
	char* name;
	int begin_register;
	int num_register;
	void go()
	{
		int k=0;
	}
};

struct SHADER_HANDLE
{
	int begin_register;
	int num_register;
	SHADER_HANDLE(){begin_register=0;num_register=0;}
};

class cUnkLight;

class cShader
{
public:
	static vector<cShader*> all_shader;

	cShader();
	virtual ~cShader();
	virtual void Restore()=0;
	virtual void Delete()=0;
};

class cVertexShader:public cShader
{
public:
	cVertexShader();
	~cVertexShader();

	void Delete();
	virtual void SetTextureTransform(MatXf& mat);
	virtual void Restore();

	inline void SetMatrix(const SHADER_HANDLE& h,const D3DXMATRIX* mat);
	inline void SetVector(const SHADER_HANDLE& h,const D3DXVECTOR4* mat);
	inline void SetFloat(const SHADER_HANDLE& h,const float f);

	inline void SetMatrix4x4(const SHADER_HANDLE& h,int index,const D3DXMATRIX* mat);
	inline void SetMatrix4x3(const SHADER_HANDLE& h,int index,const D3DXMATRIX* mat);
protected:
	ConstShaderInfo* pShaderInfo;
	int ShaderInfoSize;

	struct SHADER
	{
		LPDIRECT3DVERTEXSHADER9 pShader;
	};

	vector<SHADER> pShader;
	void Select(int num=0);
	virtual void RestoreShader()=0;
	HRESULT Compile(const char* name,const DWORD* shader);
	void GetVariableByName(SHADER_HANDLE& h,const char* name);
	void GetConstantByName(SHADER_HANDLE& h,const char* name);
	void CompileAndFound(const char* name,const DWORD* shader);
	virtual void GetHandle();
};

class PixelShader:public cShader
{
	LPDIRECT3DPIXELSHADER9 pShader;
public:
	PixelShader();
	~PixelShader();

	void Select();
	void Delete();
protected:
	HRESULT Compile(const char* name,const DWORD* shader);
};

class PSShowMap:public PixelShader
{
public:
	virtual void Restore();
};

class VSShadow:public cVertexShader
{
public:
	virtual void Select(const MatXf& world=MatXf::ID);
};

class VSScene:public cVertexShader
{
public:
	virtual void Select(const D3DXMATRIX* matlight,float shadow_map_size,const MatXf* world,vector<cUnkLight*>* light)=0;
	virtual void SetMaterial(sDataRenderMaterial *Data){};
	void SetLight(SHADER_HANDLE& pos,SHADER_HANDLE& color,SHADER_HANDLE& param,cUnkLight* l);
	virtual void SetWorldSize(Vect2f sz){};//для tilemap
};

////////////////////////////9700/////////////////////////////////////
class VS9700TileMapShadow:public VSShadow
{
protected:
	SHADER_HANDLE mWVP;
public:
	virtual void Select(const MatXf& world=MatXf::ID);
	virtual void RestoreShader();
	virtual void GetHandle();
};

class PS9700TileMapShadow:public PixelShader
{
public:
	virtual void Restore();
};

class VS9700TileMapScene:public VSScene
{
protected:
	SHADER_HANDLE mWVP;
	SHADER_HANDLE mWorldToLight;
	SHADER_HANDLE mShadow;
	SHADER_HANDLE mWorld;
	SHADER_HANDLE mView;

	SHADER_HANDLE fInvWorldSize;
public:
	virtual void Select(const D3DXMATRIX* matlight,float shadow_map_size,const MatXf* world,vector<cUnkLight*>* light);
	virtual void RestoreShader();
	virtual void GetHandle();
	virtual void SetWorldSize(Vect2f sz);
};

class PS9700TileMapScene:public PixelShader
{
public:
	virtual void Restore();
};

class PS9700TileMapScene4x4:public PixelShader
{
public:
	virtual void Restore();
};

class VS9700ObjectShadow:public VS9700TileMapShadow
{
public:
	virtual void RestoreShader();
};

class PS9700AlphaTestShadow:public PixelShader
{
public:
	virtual void Restore();
};

class PS9700ObjectShadow:public PixelShader
{
public:
	virtual void Restore();
};

class PS9700ObjectSceneLight:public PixelShader
{
public:
	virtual void Restore();
};

class PS9700ObjectSceneLight2:public PixelShader
{
public:
	virtual void Restore();
};

class PS9700ObjectSceneBump:public PixelShader
{
public:
	virtual void Restore();
};

class PS9700ObjectSceneBumpNoShadow:public PixelShader
{
public:
	virtual void Restore();
};
///////////////////////////////////only light/////////////////////
class VSObjectLight:public VSScene
{
public:
	virtual void RestoreShader();
};

class PSObjectLight:public PixelShader
{
public:
	virtual void Restore();
};


class VS9700ObjectScene:public VS9700TileMapScene
{
protected:
	SHADER_HANDLE vPointPos0;
	SHADER_HANDLE vPointColor0;
	SHADER_HANDLE vPointParam0;
	SHADER_HANDLE vPointPos1;
	SHADER_HANDLE vPointColor1;
	SHADER_HANDLE vPointParam1;
public:
	VS9700ObjectScene();
	~VS9700ObjectScene();

	void RestoreShader();
protected:
	virtual void GetHandle();
};

class VS9700ObjectSceneLight:public VS9700ObjectScene
{
protected:
	SHADER_HANDLE vAmbient;
	SHADER_HANDLE vDiffuse;
	SHADER_HANDLE vSpecular;
	SHADER_HANDLE fSpecularPower;
	SHADER_HANDLE vCameraPos;
	SHADER_HANDLE vLightDirection;
public:
	void Select(const D3DXMATRIX* matlight,float shadow_map_size,const MatXf* world,vector<cUnkLight*>* light);
	void RestoreShader();
	void GetHandle();
	void SetMaterial(sDataRenderMaterial *Data);
};

class VS9700ObjectSceneLight2:public VS9700ObjectSceneLight
{
protected:
	SHADER_HANDLE vUtrans;
	SHADER_HANDLE vVtrans;
public:
	void SetTextureTransform(MatXf& mat);
	void GetHandle();
	void RestoreShader();
};

class VS9700ObjectSceneLightT:public VS9700ObjectSceneLight2
{
public:
	void RestoreShader();
};

class VS9700ObjectSceneBump:public VS9700ObjectScene
{
protected:
	SHADER_HANDLE mInvWorld;
	SHADER_HANDLE vCameraPos;
	SHADER_HANDLE vLightDirection;
	SHADER_HANDLE vLightDirectionInvWorld;
public:
	void SetMaterial(sDataRenderMaterial *Data);
	virtual void Select(const D3DXMATRIX* matlight,float shadow_map_size,const MatXf* world,vector<cUnkLight*>* light);
	virtual void RestoreShader();
	void GetHandle();
};

class VS9700ObjectSceneBumpNoShadow:public VS9700ObjectSceneBump
{
public:
	virtual void RestoreShader();
};


/////////////////////////////ps.1.4///////////////////////////////////

class VS14Scene:public VSScene
{
public:
	SHADER_HANDLE mWVP;
	SHADER_HANDLE mWorldToLight;
	SHADER_HANDLE mShadow0;
	SHADER_HANDLE mShadow1;
	SHADER_HANDLE mShadow2;
	SHADER_HANDLE mShadow3;
	SHADER_HANDLE vZBias;
	SHADER_HANDLE mWorld;
	SHADER_HANDLE mView;
public:
	virtual void Select(const D3DXMATRIX* matlight,float shadow_map_size,const MatXf* world,vector<cUnkLight*>* light);
	virtual void GetHandle();
};

class VS14TileMapShadow:public VSShadow
{
public:
	virtual void RestoreShader();
};

class PS14TileMapShadow:public PixelShader
{
public:
	virtual void Restore();
};

class VS14TileMapScene:public VS14Scene
{
protected:
	SHADER_HANDLE fInvWorldSize;
public:
	virtual void RestoreShader();
	virtual void GetHandle();
	virtual void SetWorldSize(Vect2f sz);
};

class PS14TileMapScene:public PixelShader
{
public:
	virtual void Restore();
};

class VS14ObjectShadow: public VSShadow
{
public:
	virtual void RestoreShader();
};

class PS14ObjectShadow: public PixelShader
{
public:
	virtual void Restore();
};

class PS14ObjectSceneLight:public PixelShader
{
public:
	virtual void Restore();
};

class VS14ObjectSceneLight:public VS14Scene
{
protected:
	SHADER_HANDLE vAmbient;
	SHADER_HANDLE vDiffuse;
	SHADER_HANDLE vSpecular;
	SHADER_HANDLE fSpecularPower;
	SHADER_HANDLE vCameraPos;
	SHADER_HANDLE vLightDirection;

	SHADER_HANDLE vPointPos0;
	SHADER_HANDLE vPointColor0;
	SHADER_HANDLE vPointParam0;
	SHADER_HANDLE vPointPos1;
	SHADER_HANDLE vPointColor1;
	SHADER_HANDLE vPointParam1;
public:
	virtual void Select(const D3DXMATRIX* matlight,float shadow_map_size,const MatXf* world,vector<cUnkLight*>* light);
	virtual void RestoreShader();
	virtual void GetHandle();
	virtual void SetMaterial(sDataRenderMaterial *Data);
};

class PS14ObjectScene:public PixelShader
{
public:
	virtual void Restore();
};

class VS14ObjectScene:public VS14Scene
{
protected:
	SHADER_HANDLE vDiffuse;
public:
	virtual void SetMaterial(sDataRenderMaterial *Data);
	virtual void GetHandle();
	virtual void RestoreShader();
};

class PS14ObjectSceneLight2:public PixelShader
{
public:
	virtual void Restore();
};

class VS14ObjectSceneLight2:public VS14ObjectSceneLight
{
protected:
	SHADER_HANDLE vUtrans;
	SHADER_HANDLE vVtrans;
public:
	virtual void RestoreShader();
	virtual void GetHandle();
	virtual void SetTextureTransform(MatXf& mat);
};

class PS14ObjectSceneBump:public PixelShader
{
public:
	virtual void Restore();
};

class VS14ObjectSceneBump:public VS14ObjectSceneLight
{
protected:
	SHADER_HANDLE mInvWorld;
	SHADER_HANDLE vCameraPos;
	SHADER_HANDLE vLightDirection;
	SHADER_HANDLE vLightDirectionInvWorld;
	SHADER_HANDLE c05;
public:
	void SetMaterial(sDataRenderMaterial *Data);
	virtual void Select(const D3DXMATRIX* matlight,float shadow_map_size,const MatXf* world,vector<cUnkLight*>* light);
	virtual void GetHandle();
	virtual void RestoreShader();
};

class PS14ObjectSceneBumpNoShadow:public PixelShader
{
public:
	virtual void Restore();
};

class VS14ObjectSceneBumpNoShadow:public VS14ObjectSceneBump
{
public:
	virtual void RestoreShader();
};

////////////////////////////Geforce3///////////////////////////////////
class VSGeforce3Scene:public VSScene
{
protected:
	SHADER_HANDLE	mWVP;
	SHADER_HANDLE	mShadow;
	SHADER_HANDLE	mWorld;
	SHADER_HANDLE	mView;

	SHADER_HANDLE vFog;
	SHADER_HANDLE mWorldView;

	SHADER_HANDLE vPointPos0;
	SHADER_HANDLE vPointColor0;
	SHADER_HANDLE vPointParam0;
	SHADER_HANDLE vPointPos1;
	SHADER_HANDLE vPointColor1;
	SHADER_HANDLE vPointParam1;
public:
	virtual void Select(const D3DXMATRIX* matlight,float shadow_map_size,const MatXf* world,vector<cUnkLight*>* light);
protected:
	void SetFog();
	virtual void GetHandle();
};

class VSGeforce3TileMapScene:public VSGeforce3Scene
{
protected:
	SHADER_HANDLE fInvWorldSize;
	virtual void GetHandle();
public:
	virtual void RestoreShader();
	virtual void SetWorldSize(Vect2f sz);
};

class PSGeforce3TileMapScene:public PixelShader
{
public:
	virtual void Restore();
};

class VSGeforce3ObjectSceneLight:public VSGeforce3Scene
{
protected:
	SHADER_HANDLE	vAmbient;
	SHADER_HANDLE	vDiffuse;
	SHADER_HANDLE	vSpecular;
	SHADER_HANDLE	fSpecularPower;
	SHADER_HANDLE	vCameraPos;
	SHADER_HANDLE	vLightDirection;
public:
	virtual void Select(const D3DXMATRIX* matlight,float shadow_map_size,const MatXf* world,vector<cUnkLight*>* light);
	virtual void RestoreShader();
	virtual void GetHandle();
	void SetMaterial(sDataRenderMaterial *Data);
};

class PSGeforce3ObjectSceneLight:public PixelShader
{
public:
	virtual void Restore();
};

class VSGeforce3ObjectSceneLightT:public VSGeforce3ObjectSceneLight
{
protected:
	SHADER_HANDLE vUtrans;
	SHADER_HANDLE vVtrans;
public:
	virtual void GetHandle();
	virtual void SetTextureTransform(MatXf& mat);
	virtual void RestoreShader();
};

class VSGeforce3ObjectSceneLight2:public VSGeforce3ObjectSceneLightT
{
public:
	virtual void RestoreShader();
};

class PSGeforce3ObjectSceneLight2:public PixelShader
{
public:
	virtual void Restore();
};


class VSGeforce3ObjectScene:public VSGeforce3Scene
{
public:
	virtual void RestoreShader();
};

class PSGeforce3ObjectScene:public PixelShader
{
public:
	virtual void Restore();
};


class VSGeforce3ObjectSceneBump:public VSGeforce3Scene
{
protected:
	SHADER_HANDLE mInvWorld;
	SHADER_HANDLE vCameraPos;
	SHADER_HANDLE vLightDirection;
	SHADER_HANDLE vLightDirectionInvWorld;
	void GetHandle();
public:
	virtual void Select(const D3DXMATRIX* matlight,float shadow_map_size,const MatXf* world,vector<cUnkLight*>* light);
	virtual void RestoreShader();
	virtual void SetMaterial(sDataRenderMaterial *Data);
};

class PSGeforce3ObjectSceneBump:public PixelShader
{
public:
	virtual void Restore();
};

class VSGeforce3ObjectSceneBumpNoShadow:public VSGeforce3ObjectSceneBump
{
public:
	virtual void RestoreShader();
};

class PSGeforce3ObjectSceneBumpNoShadow:public PixelShader
{
public:
	virtual void Restore();
};


////////////////////////////GeforceFX///////////////////////////////////
class VSGeforceFXScene:public VSScene
{
protected:
	SHADER_HANDLE	mWVP;
	SHADER_HANDLE	mShadow;
	SHADER_HANDLE	mWorld;
	SHADER_HANDLE	mView;

	SHADER_HANDLE vFog;
	SHADER_HANDLE mWorldView;

	SHADER_HANDLE vPointPos0;
	SHADER_HANDLE vPointColor0;
	SHADER_HANDLE vPointParam0;
	SHADER_HANDLE vPointPos1;
	SHADER_HANDLE vPointColor1;
	SHADER_HANDLE vPointParam1;
public:
	virtual void Select(const D3DXMATRIX* matlight,float shadow_map_size,const MatXf* world,vector<cUnkLight*>* light);
protected:
	void SetFog();
	virtual void GetHandle();
};

class VSGeforceFXTileMapScene:public VSGeforceFXScene
{
protected:
	SHADER_HANDLE fInvWorldSize;
	virtual void GetHandle();
public:
	virtual void RestoreShader();
	virtual void SetWorldSize(Vect2f sz);
};

class PSGeforceFXTileMapScene:public PixelShader
{
public:
	virtual void Restore();
};

class VSGeforceFXObjectSceneLight:public VSGeforceFXScene
{
protected:
	SHADER_HANDLE	vAmbient;
	SHADER_HANDLE	vDiffuse;
	SHADER_HANDLE	vSpecular;
	SHADER_HANDLE	fSpecularPower;
	SHADER_HANDLE	vCameraPos;
	SHADER_HANDLE	vLightDirection;
public:
	virtual void Select(const D3DXMATRIX* matlight,float shadow_map_size,const MatXf* world,vector<cUnkLight*>* light);
	virtual void RestoreShader();
	virtual void GetHandle();
	void SetMaterial(sDataRenderMaterial *Data);
};

class PSGeforceFXObjectSceneLight:public PixelShader
{
public:
	virtual void Restore();
};

class VSGeforceFXObjectSceneLightT:public VSGeforceFXObjectSceneLight
{
protected:
	SHADER_HANDLE vUtrans;
	SHADER_HANDLE vVtrans;
public:
	virtual void GetHandle();
	virtual void SetTextureTransform(MatXf& mat);
	virtual void RestoreShader();
};

class VSGeforceFXObjectSceneLight2:public VSGeforceFXObjectSceneLightT
{
public:
	virtual void RestoreShader();
};

class PSGeforceFXObjectSceneLight2:public PixelShader
{
public:
	virtual void Restore();
};


class VSGeforceFXObjectScene:public VSGeforceFXScene
{
public:
	virtual void RestoreShader();
};

class PSGeforceFXObjectScene:public PixelShader
{
public:
	virtual void Restore();
};


class VSGeforceFXObjectSceneBump:public VSGeforceFXScene
{
protected:
	SHADER_HANDLE mInvWorld;
	SHADER_HANDLE vCameraPos;
	SHADER_HANDLE vLightDirection;
	SHADER_HANDLE vLightDirectionInvWorld;
	void GetHandle();
public:
	virtual void Select(const D3DXMATRIX* matlight,float shadow_map_size,const MatXf* world,vector<cUnkLight*>* light);
	virtual void RestoreShader();
	virtual void SetMaterial(sDataRenderMaterial *Data);
};

class PSGeforceFXObjectSceneBump:public PixelShader
{
public:
	virtual void Restore();
};

class VSGeforceFXObjectSceneBumpNoShadow:public VSGeforceFXObjectSceneBump
{
public:
	virtual void RestoreShader();
};

class PSGeforceFXObjectSceneBumpNoShadow:public PixelShader
{
public:
	virtual void Restore();
};


/////////////////////Chaos///////////////////////////
class VSChaos:public cVertexShader
{
	SHADER_HANDLE mWVP;
	SHADER_HANDLE mUV;
	SHADER_HANDLE mUVBump;
	SHADER_HANDLE mWorldView;
	SHADER_HANDLE vFog;
	void SetFog();
public:
	virtual void Select(float umin,float vmin,float umin2,float vmin2,
		float umin_b0,float vmin_b0,float umin_b1,float vmin_b1);
	void RestoreShader();
protected:
	void GetHandle();
};

class PSChaos:public PixelShader
{
public:
	virtual void Restore();
};

//////////////////////Skin/////////////////////////////
class VSSkinBase:public cVertexShader
{
public:
	virtual void Select(const D3DXMATRIX* matlight,float shadow_map_size,
		vector<MatXf>& world,vector<cUnkLight*>* light,int blend_num)=0;
	virtual void SetMaterial(sDataRenderMaterial *Data)=0;
};

class VSSkin:public VSSkinBase
{
protected:
	SHADER_HANDLE	mVP;
	SHADER_HANDLE	mWorldM;
	SHADER_HANDLE	mView;

	SHADER_HANDLE vPointPos0;
	SHADER_HANDLE vPointColor0;
	SHADER_HANDLE vPointParam0;
	SHADER_HANDLE vPointPos1;
	SHADER_HANDLE vPointColor1;
	SHADER_HANDLE vPointParam1;

	SHADER_HANDLE	vAmbient;
	SHADER_HANDLE	vDiffuse;
	SHADER_HANDLE	vSpecular;
	SHADER_HANDLE	fSpecularPower;
	SHADER_HANDLE	vCameraPos;
	SHADER_HANDLE	vLightDirection;
public:
	void Select(const D3DXMATRIX* matlight,float shadow_map_size,
		vector<MatXf>& world,vector<cUnkLight*>* light,int blend_num);
	void SetMaterial(sDataRenderMaterial *Data);
protected:
	void SetFog();
	virtual void GetHandle();
	virtual void RestoreShader();
};

class PSSkin:public PixelShader
{
public:
	virtual void Restore();
};

class VSSkinBump:public VSSkinBase
{
protected:
	SHADER_HANDLE	mVP;
	SHADER_HANDLE	mWorldM;
	SHADER_HANDLE	mView;

	SHADER_HANDLE vPointPos0;
	SHADER_HANDLE vPointColor0;
	SHADER_HANDLE vPointParam0;
	SHADER_HANDLE vPointPos1;
	SHADER_HANDLE vPointColor1;
	SHADER_HANDLE vPointParam1;

	SHADER_HANDLE	vAmbient;
	SHADER_HANDLE	vDiffuse;
	SHADER_HANDLE	vSpecular;
	SHADER_HANDLE	fSpecularPower;
	SHADER_HANDLE	vCameraPos;
	SHADER_HANDLE	vLightDirection;
public:
	void Select(const D3DXMATRIX* matlight,float shadow_map_size,
		vector<MatXf>& world,vector<cUnkLight*>* light,int blend_num);
	void SetMaterial(sDataRenderMaterial *Data);
protected:
	void SetFog();
	virtual void GetHandle();
	virtual void RestoreShader();
};

class PSSkinBump:public PixelShader
{
public:
	void SetMaterial(sDataRenderMaterial *Data);
	virtual void Restore();
};

////////////////////////Water//////////////////////////
class VSWater:public cVertexShader
{
protected:
	int technique;
	SHADER_HANDLE	mVP;
	SHADER_HANDLE	uvScaleOffset;
	SHADER_HANDLE	uvScaleOffset1;
	SHADER_HANDLE	worldScale;
	SHADER_HANDLE	uvScaleOffsetSky;
	SHADER_HANDLE	vCameraPos;
public:
	VSWater();
	void SetSpeed(Vect2f scale,Vect2f offset);
	void SetSpeed1(Vect2f scale,Vect2f offset);
	void SetSpeedSky(Vect2f scale,Vect2f offset);
	void SetWorldScale(Vect2f scale);
	void Select();
	void SetTechnique(int t);
	int GetTechnique(){return technique;}
protected:
	virtual void GetHandle();
	virtual void RestoreShader();
};

class PSWater:public PixelShader
{
	int technique;
public:
	PSWater();
	void SetTechnique(int t);
	virtual void Restore();
};
