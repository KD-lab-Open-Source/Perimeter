#pragma once

#include "SlotManager.h"
#include "..\shader\shaders.h"
#include "DrawType.h"

enum
{
	RENDERSTATE_MAX=210,
	TEXTURE_MAX=8,
	TEXTURESTATE_MAX=33,
	SAMPLERSTATE_MAX=14,
};

inline DWORD F2DW( FLOAT f ) { return *((DWORD*)&f); }
inline FLOAT DW2F( DWORD f ) { return *((FLOAT*)&f); }

struct sSlotVB
{
	LPDIRECT3DVERTEXBUFFER9 p;
	char			dynamic;
	bool			init;
	int				fmt;
	short			VertexSize;
	int				NumberVertex;
};
struct sSlotIB
{
	LPDIRECT3DINDEXBUFFER9 p;
	bool			init;
};

__forceinline void cD3DRender_SetMatrix(D3DXMATRIX &mat,const MatXf &m)
{
	mat._11=m.rot()[0][0],	mat._12=m.rot()[1][0],	mat._13=m.rot()[2][0],	mat._14=0;
	mat._21=m.rot()[0][1],	mat._22=m.rot()[1][1],	mat._23=m.rot()[2][1],	mat._24=0;
	mat._31=m.rot()[0][2],	mat._32=m.rot()[1][2],	mat._33=m.rot()[2][2],	mat._34=0;
	mat._41=m.trans().x,	mat._42=m.trans().y,	mat._43=m.trans().z,	mat._44=1;
}

#include "PoolManager.h"
#include "OcclusionQuery.h"


class cD3DRender : public cURenderDevice
{
	friend class DrawType;
public:
	cD3DRender();
	~cD3DRender();

	virtual int Init(int xScr,int yScr,int mode,void *hWnd=0,int RefreshRateInHz=0);
	virtual bool ChangeSize(int xScr,int yScr,int mode);
	virtual int GetClipRect(int *xmin,int *ymin,int *xmax,int *ymax);
	virtual int SetClipRect(int xmin,int ymin,int xmax,int ymax);
	virtual int Done();
	virtual bool CheckDeviceType(int xscr,int yscr,int Mode);

	virtual int BeginScene();
	virtual int EndScene();
	virtual int Fill(int r,int g,int b,int a=255);
	virtual int Flush(HWND wnd=NULL);
	virtual int SetGamma(float fGamma,float fStart=0.f,float fFinish=1.f);

	VertexPoolManager* GetVertexPool(){return &vertex_pool_manager;};
	IndexPoolManager* GetIndexPool(){return &index_pool_manager;};
	
	virtual void CreateVertexBuffer(struct sPtrVertexBuffer &vb,int NumberVertex,int fmt,int dynamic=0);
	void CreateVertexBufferBySize(struct sPtrVertexBuffer &vb,int NumberVertex,int size,int dynamic);
	virtual void DeleteVertexBuffer(sPtrVertexBuffer &vb);
	virtual void* LockVertexBuffer(sPtrVertexBuffer &vb);
	virtual void UnlockVertexBuffer(sPtrVertexBuffer &vb);
	virtual void CreateIndexBuffer(sPtrIndexBuffer& ib,int NumberVertex,int size=sizeof(sPolygon));
	virtual void DeleteIndexBuffer(sPtrIndexBuffer &ib);
	virtual sPolygon* LockIndexBuffer(sPtrIndexBuffer &ib);
	virtual void UnlockIndexBuffer(sPtrIndexBuffer &ib);
	virtual int CreateTexture(class cTexture *Texture,class cFileImage *FileImage,int dxout,int dyout,bool enable_assert=true);
	virtual int DeleteTexture(class cTexture *Texture);
	void* LockTexture(class cTexture *Texture,int& Pitch);
	void* LockTexture(class cTexture *Texture,int& Pitch,Vect2i lock_min,Vect2i lock_size);
	void UnlockTexture(class cTexture *Texture);

	virtual int CreateCubeTexture(class cTexture *Texture,LPCSTR fname);
	virtual int CreateBumpTexture(class cTexture *Texture);

	virtual void SetDrawNode(class cCamera *DrawNode);
	virtual void SetGlobalFog(const sColor4f &color,const Vect2f &v);
	virtual void SetGlobalLight(Vect3f *vLight,sColor4f *Ambient,sColor4f *Diffuse,sColor4f *Specular);

	virtual void Draw(class cScene *Scene);

	virtual void Draw(class FieldDispatcher *rd);
	virtual void CreateFFDData(class FieldDispatcher *rd);
	virtual void DeleteFFDData(class FieldDispatcher *rd);

	virtual void Draw(class FieldDispatcher *rd,BYTE transparent);

	virtual void Draw(class ElasticSphere *es);

	virtual void DrawBound(const MatXf &Matrix,Vect3f &min,Vect3f &max,bool wireframe=0,sColor4c &Color=sColor4c(255,255,255,255));

	virtual int Create(class cTileMap *TileMap);

	virtual void PreDraw(cTileMap *TileMap);
	virtual void Draw(cTileMap *TileMap,eBlendMode MatMode,TILEMAP_DRAW tile_draw,bool shadow);
	virtual int Delete(class cTileMap *TileMap);

	virtual int SetRenderState(eRenderStateOption option,int value);

	// вспомогательные функции, могут быть не реализованы
	virtual int	GetSizeX()														{ return xScr; }
	virtual int	GetSizeY()														{ return yScr; }
	virtual int GetScrFormatData(sTextureFormatData &TexFmt)					{ return -1; }
	virtual void DrawLine(int x1,int y1,int x2,int y2,sColor4c color);
	virtual void DrawPixel(int x1,int y1,sColor4c color);
	virtual void DrawRectangle(int x,int y,int dx,int dy,sColor4c color,bool outline=false);
	virtual void FlushPrimitive2D();

	virtual void OutText(int x,int y,const char *string,const sColor4f& color,int align=-1,eBlendMode blend_mode=ALPHA_BLEND);
	virtual void OutTextRect(int x,int y,const char *string,int align,Vect2f& bmin,Vect2f& bmax);
	virtual void OutText(int x,int y,const char *string,const sColor4f& color,int align,eBlendMode blend_mode,
				cTexture* pTexture,eColorMode mode,Vect2f uv,Vect2f duv,float phase=0,float lerp_factor=1);
	virtual float GetFontLength(const char *string);
	virtual float GetCharLength(const char c);

	virtual void OutText(int x,int y,const char *string,int r=255,int g=255,int b=255);
	virtual void OutText(int x,int y,const char *string,int r,int g,int b,char *FontName="Arial",int size=12,int bold=0,int italic=0,int underline=0);
	virtual HWND GetWindowHandle()												{ return hWnd;}
	virtual bool SetScreenShot(const char *fname);
	virtual void DrawSprite(int x,int y,int dx,int dy,float u,float v,float du,float dv,
		cTexture *Texture,sColor4c &ColorMul=sColor4c(255,255,255,255),float phase=0,eBlendMode mode=ALPHA_NONE);
	virtual void DrawSprite2(int x,int y,int dx,int dy,float u,float v,float du,float dv,
		cTexture *Tex1,cTexture *Tex2,sColor4c &ColorMul=sColor4c(255,255,255,255),float phase=0);
	virtual void DrawSprite2(int x,int y,int dx,int dy,float u,float v,float du,float dv,float u1,float v1,float du1,float dv1,
		cTexture *Tex1,cTexture *Tex2,sColor4c &ColorMul=sColor4c(255,255,255,255),float phase=0,eColorMode mode=COLOR_MOD,eBlendMode blend_mode=ALPHA_NONE);
	virtual void DrawSprite2(int x,int y,int dx,int dy,float u,float v,float du,float dv,float u1,float v1,float du1,float dv1,
		cTexture *Tex1,cTexture *Tex2,float lerp_factor,float alpha=1,float phase=0,eColorMode mode=COLOR_MOD,eBlendMode blend_mode=ALPHA_NONE);

	virtual void DrawSpriteScale(int x,int y,int dx,int dy,float u,float v,
		cTextureScale *Texture,sColor4c &ColorMul=sColor4c(255,255,255,255),float phase=0,eBlendMode mode=ALPHA_NONE);
	virtual void DrawSpriteScale2(int x,int y,int dx,int dy,float u,float v,
		cTextureScale *Tex1,cTextureScale *Tex2,sColor4c &ColorMul=sColor4c(255,255,255,255),float phase=0);
	virtual void DrawSpriteScale2(int x,int y,int dx,int dy,float u,float v,float u1,float v1,
		cTextureScale *Tex1,cTextureScale *Tex2,sColor4c &ColorMul=sColor4c(255,255,255,255),float phase=0,eColorMode mode=COLOR_MOD);

	virtual cVertexBuffer<sVertexXYZDT1>* GetBufferXYZDT1(){return &BufferXYZDT1;};
	virtual cVertexBuffer<sVertexXYZDT2>* GetBufferXYZDT2(){return &BufferXYZDT2;};
	virtual cVertexBuffer<sVertexXYZD>* GetBufferXYZD(){return &BufferXYZD;};
	virtual cVertexBuffer<sVertexXYZWD>* GetBufferXYZWD(){return &BufferXYZWD;};
	virtual cQuadBuffer<sVertexXYZDT1>* GetQuadBufferXYZDT1(){return &QuadBufferXYZDT1;}
	virtual cQuadBuffer<sVertexXYZWDT1>* GetQuadBufferXYZWDT1(){return &QuadBufferXYZWDT1;}
	virtual cVertexBuffer<sVertexXYZWDT1>* GetBufferXYZWDT1(){return &BufferXYZWDT1;};
	virtual cVertexBuffer<sVertexXYZWDT2>* GetBufferXYZWDT2(){return &BufferXYZWDT2;};

	virtual bool IsFullScreen() {return (RenderMode&RENDERDEVICE_MODE_WINDOW)?false:true;}

	void SetWorldMatrix(const MatXf& pos)
	{
		SetMatrix(D3DTS_WORLD,pos);
	}

	__forceinline void SetMatrix(int type,const MatXf &m)
	{
		D3DXMATRIX mat;
		cD3DRender_SetMatrix(mat,m);
		RDCALL(lpD3DDevice->SetTransform((D3DTRANSFORMSTATETYPE)type,&mat));
	}

	int GetZDepth()
	{
		int f=d3dpp.AutoDepthStencilFormat;
		switch(f)
		{
		case D3DFMT_D16:
			return 16;
		case D3DFMT_D24S8:
			return 24;
		}
		VISASSERT(0);
		return 16;
	}


	virtual void SaveStates(const char* fname="states.txt");
	D3DCAPS9					DeviceCaps;

	DrawType*	dtFixed;
	DrawType*	dtAdvance;
	DrawType*	dtAdvanceOriginal;

	virtual bool IsEnableSelfShadow();
	void SetAdvance();//Вызывать при изменении Option_ShadowType Option_EnableBump

	bool PossibleAnisotropic();
	void SetAnisotropic(bool enable);
	bool GetAnisotropic();

	bool PossibilityBump(){	return dtAdvanceOriginal!=NULL;}

	virtual void SetDialogBoxMode(bool enable);
private:
	DrawTypeMixed dtMixed;
public:

//private:
	HWND						hWnd;

	LPDIRECT3D9					lpD3D;
	LPDIRECT3DDEVICE9			lpD3DDevice;
    LPDIRECT3DSURFACE9			lpBackBuffer,lpZBuffer;
	D3DPRESENT_PARAMETERS		d3dpp;
	IDirect3DBaseTexture9*		CurrentTexture[TEXTURE_MAX];
	bool						bSupportVertexShader;
	bool						bSupportVertexShaderHardware;
	bool						bSupportVertexFog;
	bool						bSupportTableFog;
	int							dwSuportMaxSizeTextureX,dwSuportMaxSizeTextureY;
	int							MaxTextureAspectRatio;

	D3DTEXTUREFILTERTYPE		texture_interpolation;

	bool SetFocus(bool wait,bool focus_error=true);
	int KillFocus();
	LPDIRECT3DTEXTURE9 CreateSurface(int x,int y,eSurfaceFormat TextureFormat,int MipMap,bool enable_assert,DWORD attribute);

	inline IDirect3DBaseTexture9* GetTexture(int dwStage)
	{
		VISASSERT( dwStage<nSupportTexture );
		return CurrentTexture[dwStage];
	}

	inline void SetTexture(cTexture *Texture,float Phase,int dwStage=0)
	{
		if(Texture==0) 
		{ 
			SetTexture(dwStage,NULL);
			return; 
		}
		VISASSERT( dwStage<nSupportTexture );

		int nAllFrame=Texture->GetNumberFrame();
		int nFrame ;
		if(nAllFrame>1)
			nFrame= (int)( 0.999f*Phase*nAllFrame);
		else
			nFrame=0;

		VISASSERT(0<=nFrame&&nFrame<Texture->GetNumberFrame()&&bActiveScene);
		SetTexture(dwStage,Texture->GetDDSurface(nFrame));
	}

	inline void SetTexture(DWORD dwStage,IDirect3DBaseTexture9 *pTexture)
	{
		VISASSERT(dwStage<TEXTURE_MAX);
		if(CurrentTexture[dwStage]!=pTexture)
			RDCALL(lpD3DDevice->SetTexture(dwStage,CurrentTexture[dwStage]=pTexture));
	}

	inline void SetVertexShader(IDirect3DVertexShader9 * Handle)
	{
		if(Handle!=CurrentVertexShader)
		{
			RDCALL(lpD3DDevice->SetVertexShader(CurrentVertexShader=Handle));
		}
	}

	inline void SetPixelShader(IDirect3DPixelShader9 * Handle)
	{
		if(Handle!=CurrentPixelShader)
		{
			RDCALL(lpD3DDevice->SetPixelShader(CurrentPixelShader=Handle));
		}
	}

	inline void SetFVF(DWORD handle)
	{
		xassert(handle);
		if(handle!=CurrentFVF)
		{
			RDCALL(lpD3DDevice->SetFVF(CurrentFVF=handle));
		}
	}

	inline void SetFVF(sPtrVertexBuffer& vb)
	{
		SetFVF(vb.ptr->fmt);
	}

	__forceinline void SetRenderState(D3DRENDERSTATETYPE State,unsigned int Value)
	{
		VISASSERT(0<=State&&State<RENDERSTATE_MAX);
//		DWORD value;
//		RDCALL(lpD3DDevice->GetRenderState((D3DRENDERSTATETYPE)State,&value));
//		VISASSERT(ArrayRenderState[State]==value || ArrayRenderState[State]==0xefefefef);

		if(ArrayRenderState[State]!=Value) 
		{
			RDCALL(lpD3DDevice->SetRenderState(State,ArrayRenderState[State]=Value));
		}
	}
	inline unsigned int GetRenderState(D3DRENDERSTATETYPE State)
	{
		VISASSERT(0<=State && State<RENDERSTATE_MAX);
		return ArrayRenderState[State];
	}
	__forceinline void SetTextureStageState(unsigned int Stage,D3DTEXTURESTAGESTATETYPE Type,unsigned int Value)
	{
		VISASSERT(Stage<TEXTURE_MAX);
		VISASSERT(0<=Type && Type<TEXTURESTATE_MAX);

//		DWORD value;
///		RDCALL(lpD3DDevice->GetTextureStageState(Stage,(D3DTEXTURESTAGESTATETYPE)Type,&value));
//		VISASSERT(ArrayTextureStageState[Stage][Type]==value || ArrayTextureStageState[Stage][Type]==0xefefefef);

		if(ArrayTextureStageState[Stage][Type]!=Value) 
		{
			RDCALL(lpD3DDevice->SetTextureStageState(Stage,Type,ArrayTextureStageState[Stage][Type]=Value));
		}
	}

	__forceinline DWORD GetTextureStageState(unsigned int Stage,D3DTEXTURESTAGESTATETYPE Type)
	{
		return ArrayTextureStageState[Stage][Type];
	}

	inline void SetSamplerState(DWORD Stage,D3DSAMPLERSTATETYPE Type,DWORD Value)
	{
		VISASSERT(Stage<TEXTURE_MAX);
		VISASSERT(0<=Type && Type<SAMPLERSTATE_MAX);
		if(ArraytSamplerState[Stage][Type]!=Value)
		{
			ArraytSamplerState[Stage][Type]=Value;
			RDCALL(lpD3DDevice->SetSamplerState(Stage,Type,Value));
		}
	}

	inline DWORD GetSamplerState(DWORD Stage,D3DSAMPLERSTATETYPE Type)
	{
		VISASSERT(Stage<TEXTURE_MAX);
		VISASSERT(0<=Type && Type<SAMPLERSTATE_MAX);
		return ArraytSamplerState[Stage][Type];
	}

	virtual void SetNoMaterial(eBlendMode blend,float Phase=0,cTexture *Texture0=0,cTexture *Texture1=0,eColorMode color_mode=COLOR_MOD);

	void SetBlendState(eBlendMode blend);

	void SetVertexShaderConstant(int StartRegister,const D3DXMATRIX *pMat);
	void SetVertexShaderConstant(int StartRegister,const D3DXVECTOR4 *pVect);
	void SetPixelShaderConstant(int StartRegister,const D3DXVECTOR4 *pVect);

	inline void SetTextureTransform(int Stage,D3DXMATRIX *matTexSpace)
	{
		float det=1;
		D3DXMATRIX mat,matViewInv;	// matViewWorld=matWorld*matView=matView, because matWorld==ID
		D3DXMatrixInverse(&matViewInv,&det,DrawNode->matView);
		D3DXMatrixMultiply(&mat,&matViewInv,matTexSpace);
		RDCALL(lpD3DDevice->SetTransform( D3DTRANSFORMSTATETYPE(D3DTS_TEXTURE0+Stage), &mat ));
	}
	virtual void DrawIndexedPrimitive(sPtrVertexBuffer &vb,int OfsVertex,int nVertex,const sPtrIndexBuffer& ib,int nOfsPolygon,int nPolygon)
	{
		SetFVF(vb);
		SetIndices(ib);
		SetStreamSource(vb);
		RDCALL(lpD3DDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,0,OfsVertex,nVertex,3*nOfsPolygon,nPolygon));
		NumberPolygon+=nPolygon;
	}
	inline void DrawPrimitiveUP(D3DPRIMITIVETYPE Type,UINT Count,void* pVertex,UINT Size)
	{
		RDCALL(lpD3DDevice->DrawPrimitiveUP(Type,Count,pVertex,Size));
	}

	__forceinline LPDIRECT3DVERTEXBUFFER9 GetVB(const sPtrVertexBuffer& vb)
	{
		return vb.ptr->p;
	}

	inline void SetIndices(IDirect3DIndexBuffer9 *pIndexData)
	{
		if(CurrentIndexBuffer!=pIndexData)
		{
			CurrentIndexBuffer=pIndexData;
			RDCALL(lpD3DDevice->SetIndices(pIndexData));
		}
	}

	inline void SetIndices(const sPtrIndexBuffer& ib)
	{
		SetIndices(ib.ptr->p);
	}

	inline void SetStreamSource(const sPtrVertexBuffer& vb)
	{
		RDCALL(lpD3DDevice->SetStreamSource(0,vb.ptr->p,0,vb.size));
	}

	virtual void DrawNoMaterial(cObjMesh *Mesh,sDataRenderMaterial *Data);
	virtual void DrawNoMaterialShadow(cObjMesh *Mesh);
	virtual void DrawNoMaterialShadowNoWorld(cObjMesh *Mesh);

	void SetRenderTarget(cTexture* target,LPDIRECT3DSURFACE9 pZBuffer);
	void RestoreRenderTarget();
	void SetDrawTransform(class cCamera *DrawNode);

	virtual void DrawLine(const Vect3f &v1,const Vect3f &v2,sColor4c color);
	virtual void DrawPoint(const Vect3f &v1,sColor4c color);
	void FlushPrimitive3D();

	sPtrIndexBuffer& GetStandartIB(){return standart_ib;}

	LPDIRECT3DTEXTURE9 CreateTextureFromMemory(void* pSrcData, UINT SrcData)
	{
		LPDIRECT3DTEXTURE9 pTexture=NULL;
		HRESULT hr=D3DXCreateTextureFromFileInMemory(lpD3DDevice,
			pSrcData,SrcData,&pTexture);

		if(FAILED(hr))
			return NULL;
		return pTexture;
	}

	int CreateTextureU16V16(class cTexture *Texture,bool defaultpool);

	void SetCurrentConvertDot3Mul(float scale){current_bump_scale=scale;};
	bool ReinitOcclusion();
	bool PossibilityOcclusion();

	void RestoreShader();
protected:
	void FlushLine3D();
	void FlushPoint3D();
	void CreateVertexBufferBySizeFormat(struct sPtrVertexBuffer &vb,int NumberVertex,int size,int format,int dynamic);

	IDirect3DIndexBuffer9 *		CurrentIndexBuffer;
	IDirect3DVertexShader9 *	CurrentVertexShader;	// vertex shader
	IDirect3DPixelShader9 *		CurrentPixelShader;
	DWORD						CurrentFVF;
	int							CurrentCullMode;
	int							CurrentBumpMap,CurrentMod4; // поддерживаемые тип текстурных операций

	cSlotManagerInit<sSlotVB>	LibVB;
	cSlotManagerInit<sSlotIB>	LibIB;
	
	DWORD				ArrayRenderState[RENDERSTATE_MAX];
	DWORD				ArrayTextureStageState[TEXTURE_MAX][TEXTURESTATE_MAX];
	DWORD				ArraytSamplerState[TEXTURE_MAX][SAMPLERSTATE_MAX];
	
	VertexPoolManager vertex_pool_manager;
	IndexPoolManager index_pool_manager;

	struct PointStruct
	{
		float x,y;
		sColor4c diffuse;
	};
	struct RectStruct
	{
		float x1,y1;
		float x2,y2;
		sColor4c diffuse;
	};
	vector<PointStruct> points;
	vector<PointStruct> lines;
	vector<RectStruct>	rectangles;

	vector<sVertexXYZD> lines3d;
	vector<sVertexXYZD> points3d;

	cVertexBuffer<sVertexXYZDT1> BufferXYZDT1;
	cVertexBuffer<sVertexXYZDT2> BufferXYZDT2;
	cVertexBuffer<sVertexXYZWD>	 BufferXYZWD;
	cVertexBuffer<sVertexXYZD>	 BufferXYZD;
	cVertexBuffer<sVertexXYZWDT1>	 BufferXYZWDT1;
	cVertexBuffer<sVertexXYZWDT2>	 BufferXYZWDT2;
	cQuadBuffer<sVertexXYZDT1>	 QuadBufferXYZDT1;
	cQuadBuffer<sVertexXYZWDT1>	 QuadBufferXYZWDT1;
	cQuadBuffer<sVertexXYZWDT2>	 QuadBufferXYZWDT2;

	cVertexBuffer<sVertexXYZ>	 BufferXYZOcclusion;

	sPtrIndexBuffer standart_ib;
	float current_bump_scale;

	vector<cOcclusionQuery*> occlusion_query;

	void InitVertexBuffers();

	void FlushPixel();
	void FlushLine();
	void FlushFilledRect();

	void DeleteShader();

	void DeleteDynamicVertexBuffer();
	void RestoreDynamicVertexBuffer();
	void RestoreDeviceIfLost();

	vector<class cTileMapRender*> tilemaps;
	void ClearTilemapPool();
	void RestoreTilemapPool();

	void ChangeTextColor(const char* &str,sColor4c& diffuse);

	void InitStandartIB();
	void GetTilemapTextureMemory(int& total,int& free);

	void ConvertDot3(unsigned int* buf,int dx,int dy,float h_mul);

	void UpdateRenderMode();

	D3DFORMAT GetBackBufferFormat(int Mode);

	friend class cTileMapRender;
	friend class cOcclusionQuery;
};

__forceinline int VectorToRGBA(Vect3f &v,int a=255)
{
    int r=round(127.0f*v.x)+128,g=round(127.0f*v.y)+128,b=round(127.0f*v.z)+128;
    return (a<<24)+(r<<16)+(g<<8)+(b<<0);
}

int GetTextureFormatSize(D3DFORMAT f);

extern class cD3DRender *gb_RenderDevice3D;
