#pragma once
class cTileMap;
enum eDrawID
{
	DT_UNKNOWN=0,
	DT_FIXEDPIPELINE,
	DT_RADEON9700,
	DT_RADEON8500,
	DT_GEFORCE3,
	DT_GEFORCEFX,
	DT_MIXED,
};

class DrawType
{
public:
	DrawType():pShadowMap(0),pZBuffer(0),pLightMap(0){}
	virtual ~DrawType();

	virtual eDrawID GetID()=0;
	virtual void BeginDraw(bool use_shadow)=0;
	virtual void EndDraw()=0;
	virtual void BeginDrawShadow()=0;
	virtual void EndDrawShadow()=0;
	virtual void SetSimplyMaterial(cObjMesh *Mesh,sDataRenderMaterial *Data)=0;
	virtual void DrawNoMaterial(cObjMesh *Mesh,sDataRenderMaterial *Data)=0;
	virtual void SetSimplyMaterialShadow(cObjMesh *Mesh,cTexture *Texture)=0;
	virtual void DrawNoMaterialShadow(cObjMesh *Mesh)=0;

	virtual bool CreateShadowTexture(int xysize)=0;
	virtual void DeleteShadowTexture();
	virtual cTexture* GetShadowMap()							{return pShadowMap;}
	virtual LPDIRECT3DSURFACE9 GetZBuffer()						{return pZBuffer;}
	virtual cTexture* GetLightMap()								{return pLightMap;}
	

	virtual void SetMaterialTilemap(cTileMap *TileMap)=0;
	virtual void SetMaterialTilemapShadow()=0;

	virtual IDirect3DTexture9* GetTZBuffer(){return NULL;}
	virtual IDirect3DTexture9* GetTilemapShadow0(){return NULL;}

	virtual void SetTileColor(sColor4f color)=0;//r,g,b=[0..2], a=ignored etc

	//Если Shadow map устанавливается как нулевая текстура (для Geforce 3)
	virtual int GetOffsetTextureNumber(bool tilemap=false){return 0;}

	virtual void SetFog(const sColor4f &color,const Vect2f &v){}
	virtual D3DXVECTOR4 GetFogParam(){return D3DXVECTOR4(0,1,0,0);}
protected:
	cTexture*		pShadowMap;
	LPDIRECT3DSURFACE9	pZBuffer;
	cTexture*		pLightMap;

	void SetStream(cObjMesh *Mesh);
	void DrawPrimitive(cObjMesh *Mesh);

	eBlendMode SetMaterialSimply(float Phase,cTexture *Texture0,cTexture *Texture1,sDataRenderMaterial *Data,int texture_offset=0);
	bool IsBump(sDataRenderMaterial *Data){return (Data->mat&MAT_BUMP) && Option_EnableBump;}
};

class DrawTypeFixedPipeline:public DrawType
{
public:
	DrawTypeFixedPipeline();
	~DrawTypeFixedPipeline();

	virtual eDrawID GetID(){return DT_FIXEDPIPELINE;};
	virtual void BeginDraw(bool use_shadow);
	virtual void EndDraw();
	virtual void BeginDrawShadow(){};
	virtual void EndDrawShadow(){};
	virtual void SetSimplyMaterial(cObjMesh *Mesh,sDataRenderMaterial *Data);
	virtual void DrawNoMaterial(cObjMesh *Mesh,sDataRenderMaterial *Data);
	virtual void SetSimplyMaterialShadow(cObjMesh *Mesh,cTexture *Texture);
	virtual void DrawNoMaterialShadow(cObjMesh *Mesh);

	virtual bool CreateShadowTexture(int xysize);
	virtual void SetMaterialTilemap(cTileMap *TileMap);
	virtual void SetMaterialTilemapShadow();
	virtual void SetTileColor(sColor4f color);
protected:
	void SetPointLight(vector<class cUnkLight*>& light);
	void SetMaterial(float Phase,cTexture *Texture0,cTexture *Texture1,sDataRenderMaterial *Data);
};


class DrawTypeRadeon9700:public DrawType
{
	cCamera* pShadow;
	DWORD min_filter;
	DWORD mag_filter;

	PSShowMap* pShowMap;
	VSShadow* pVSObjectShadow;
	PixelShader* pPSObjectShadow;
	PixelShader* pPSObjectShadowAlphaTest;

	VSScene* pVSObjectScene;
	VSScene* pVSObjectSceneLight;
	PixelShader* pPSObjectScene;
	VSScene* pVSObjectSceneT;//+texture transform

	VS9700ObjectSceneLight2* pVSObjectSceneLight2;
	PS9700ObjectSceneLight2* pPSObjectScene2;

	VSShadow* pVSTileMapShadow;
	PixelShader* pPSTileMapShadow;
	VSScene*  pVSTileMapScene;
	PixelShader*  pPSTileMapScene;
	PixelShader*  pPSTileMapSceneInv;

	PixelShader*  pPSObjectSceneBump;
	VSScene*	  pVSObjectSceneBump;

	PixelShader* pPSObjectSceneBumpNoShadow;
	VSScene* pVSObjectSceneBumpNoShadow;

	bool use_shadow;

	VSScene*     last_vs;
	PixelShader* last_ps;
public:
	DrawTypeRadeon9700();
	~DrawTypeRadeon9700();

	virtual eDrawID GetID(){return DT_RADEON9700;};
	virtual void BeginDraw(bool use_shadow);
	virtual void EndDraw();
	virtual void BeginDrawShadow(){BeginDraw(true);};
	virtual void EndDrawShadow(){EndDraw();};
	virtual void SetSimplyMaterial(cObjMesh *Mesh,sDataRenderMaterial *Data);
	virtual void DrawNoMaterial(cObjMesh *Mesh,sDataRenderMaterial *Data);
	virtual void SetSimplyMaterialShadow(cObjMesh *Mesh,cTexture *Texture);
	virtual void DrawNoMaterialShadow(cObjMesh *Mesh);

	virtual bool CreateShadowTexture(int xysize);
	virtual void SetMaterialTilemap(cTileMap *TileMap);
	virtual void SetMaterialTilemapShadow();

	virtual void SetTileColor(sColor4f color);
protected:
	void SetMaterial(float Phase,cTexture *Texture0,cTexture *Texture1,sDataRenderMaterial *Data);
};


class DrawTypeRadeon8500:public DrawType
{
	cCamera* pShadow;
	DWORD min_filter;
	DWORD mag_filter;
	DWORD dither_enable;
	cTexture* pLookupMap;

	VSShadow* pVSTileMapShadow;
	PixelShader* pPSTileMapShadow;

	VSScene*  pVSTileMapScene;
	PixelShader*  pPSTileMapScene;

	PixelShader* pPSObjectShadow;
	VSShadow* pVSObjectShadow;

	PixelShader* pPSObjectScene;
	VSScene* pVSObjectSceneLight;

	PixelShader* pPSObjectSceneNoLight;
	VSScene* pVSObjectScene;

	PixelShader* pPSObjectScene2;
	VSScene* pVSObjectSceneLight2;

	PixelShader* pPSObjectSceneBump;
	VSScene* pVSObjectSceneBump;

	PixelShader* pPSObjectSceneBumpNoShadow;
	VSScene* pVSObjectSceneBumpNoShadow;

	bool use_shadow;

	VSScene*     last_vs;
	PixelShader* last_ps;
public:
	DrawTypeRadeon8500();
	~DrawTypeRadeon8500();

	virtual eDrawID GetID(){return DT_RADEON8500;};
	virtual void BeginDraw(bool use_shadow);
	virtual void EndDraw();
	virtual void BeginDrawShadow();
	virtual void EndDrawShadow();
	virtual void SetSimplyMaterial(cObjMesh *Mesh,sDataRenderMaterial *Data);
	virtual void DrawNoMaterial(cObjMesh *Mesh,sDataRenderMaterial *Data);
	virtual void SetSimplyMaterialShadow(cObjMesh *Mesh,cTexture *Texture);
	virtual void DrawNoMaterialShadow(cObjMesh *Mesh);

	virtual bool CreateShadowTexture(int xysize);
	virtual void SetMaterialTilemap(cTileMap *TileMap);
	virtual void SetMaterialTilemapShadow();
	virtual IDirect3DTexture9* GetTilemapShadow0(){return pLookupMap->GetDDSurface(0);}
	virtual void SetTileColor(sColor4f color);
protected:
	void SetMaterial(float Phase,cTexture *Texture0,cTexture *Texture1,sDataRenderMaterial *Data);
};


class DrawTypeGeforce3:public DrawTypeFixedPipeline
{
	IDirect3DTexture9* ptZBuffer;
	cCamera* pShadow;
	DWORD min_filter,mag_filter,mip_filter;

	VSScene* pVSObjectScene;
	VSScene* pVSObjectSceneLight;
	PixelShader* pPSObjectSceneNoLight;
	PixelShader* pPSObjectScene;
	VSScene* pVSObjectSceneLightT;

	VSScene* pVSObjectSceneLight2;
	PixelShader* pPSObjectScene2;

	VSScene*  pVSTileMapScene;
	PixelShader*  pPSTileMapScene;

	PixelShader* pPSObjectSceneBump;
	VSScene* pVSObjectSceneBump;

	PixelShader* pPSObjectSceneBumpNoShadow;
	VSScene* pVSObjectSceneBumpNoShadow;

	bool use_shadow;

	VSScene*     last_vs;
	PixelShader* last_ps;

	D3DXVECTOR4 fog_param;
public:
	DrawTypeGeforce3();
	~DrawTypeGeforce3();

	virtual eDrawID GetID(){return DT_GEFORCE3;};
	virtual void BeginDraw(bool use_shadow);
	virtual void EndDraw();
	virtual void BeginDrawShadow();
	virtual void EndDrawShadow();
	virtual void SetSimplyMaterial(cObjMesh *Mesh,sDataRenderMaterial *Data);
	virtual void DrawNoMaterial(cObjMesh *Mesh,sDataRenderMaterial *Data);
	virtual void SetSimplyMaterialShadow(cObjMesh *Mesh,cTexture *Texture);
	virtual void DrawNoMaterialShadow(cObjMesh *Mesh);

	virtual bool CreateShadowTexture(int xysize);
	virtual void SetMaterialTilemap(cTileMap *TileMap);
	virtual void SetMaterialTilemapShadow();
	virtual void DeleteShadowTexture();
	virtual IDirect3DTexture9* GetTZBuffer(){return ptZBuffer;}
	virtual void SetTileColor(sColor4f color);
	virtual int GetOffsetTextureNumber(bool tilemap=false);
	virtual void SetFog(const sColor4f &color,const Vect2f &v);
	virtual D3DXVECTOR4 GetFogParam(){return fog_param;}
protected:
	void SetMaterial(float Phase,cTexture *Texture0,cTexture *Texture1,sDataRenderMaterial *Data);
};


class DrawTypeMixed:public DrawType//Для bump без теней
{
public:
	DrawTypeMixed();
	~DrawTypeMixed();

	virtual eDrawID GetID(){return DT_MIXED;};
	virtual void BeginDraw(bool use_shadow);
	virtual void EndDraw();
	virtual void BeginDrawShadow(){};
	virtual void EndDrawShadow(){};
	virtual void SetSimplyMaterial(cObjMesh *Mesh,sDataRenderMaterial *Data);
	virtual void DrawNoMaterial(cObjMesh *Mesh,sDataRenderMaterial *Data);
	virtual void SetSimplyMaterialShadow(cObjMesh *Mesh,cTexture *Texture);
	virtual void DrawNoMaterialShadow(cObjMesh *Mesh);

	virtual bool CreateShadowTexture(int xysize);
	virtual void SetMaterialTilemap(cTileMap *TileMap);
	virtual void SetMaterialTilemapShadow();
	virtual void SetTileColor(sColor4f color);

	virtual cTexture* GetShadowMap();
	virtual LPDIRECT3DSURFACE9 GetZBuffer();
	virtual cTexture* GetLightMap();
};


class DrawTypeGeforceFX:public DrawTypeFixedPipeline
{
	IDirect3DTexture9* ptZBuffer;
	cCamera* pShadow;
	DWORD min_filter,mag_filter,mip_filter;

	VSScene* pVSObjectScene;
	VSScene* pVSObjectSceneLight;
	PixelShader* pPSObjectSceneNoLight;
	PixelShader* pPSObjectScene;
	VSScene* pVSObjectSceneLightT;

	VSScene* pVSObjectSceneLight2;
	PixelShader* pPSObjectScene2;

	VSScene*  pVSTileMapScene;
	PixelShader*  pPSTileMapScene;

	PixelShader* pPSObjectSceneBump;
	VSScene* pVSObjectSceneBump;

	PixelShader* pPSObjectSceneBumpNoShadow;
	VSScene* pVSObjectSceneBumpNoShadow;

	bool use_shadow;

	VSScene*     last_vs;
	PixelShader* last_ps;

	D3DXVECTOR4 fog_param;
public:
	DrawTypeGeforceFX();
	~DrawTypeGeforceFX();

	virtual eDrawID GetID(){return DT_GEFORCEFX;};
	virtual void BeginDraw(bool use_shadow);
	virtual void EndDraw();
	virtual void BeginDrawShadow();
	virtual void EndDrawShadow();
	virtual void SetSimplyMaterial(cObjMesh *Mesh,sDataRenderMaterial *Data);
	virtual void DrawNoMaterial(cObjMesh *Mesh,sDataRenderMaterial *Data);
	virtual void SetSimplyMaterialShadow(cObjMesh *Mesh,cTexture *Texture);
	virtual void DrawNoMaterialShadow(cObjMesh *Mesh);

	virtual bool CreateShadowTexture(int xysize);
	virtual void SetMaterialTilemap(cTileMap *TileMap);
	virtual void SetMaterialTilemapShadow();
	virtual void DeleteShadowTexture();
	virtual IDirect3DTexture9* GetTZBuffer(){return ptZBuffer;}
	virtual void SetTileColor(sColor4f color);
	virtual int GetOffsetTextureNumber(bool tilemap=false);
	virtual void SetFog(const sColor4f &color,const Vect2f &v);
	virtual D3DXVECTOR4 GetFogParam(){return fog_param;}
protected:
	void SetMaterial(float Phase,cTexture *Texture0,cTexture *Texture1,sDataRenderMaterial *Data);
};
