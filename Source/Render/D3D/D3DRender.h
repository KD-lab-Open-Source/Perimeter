#pragma once

#include "../shader/shaders.h"
#include "DrawType.h"
#include "VertexFormat.h"

int RDWriteLog(HRESULT err,char *exp,char *file,int line);
void RDWriteLog(char *exp,int size=-1);

#define RDCALL(exp)									{ HRESULT hr=exp; if(hr!=D3D_OK) RDWriteLog(hr,#exp,__FILE__,__LINE__); VISASSERT(SUCCEEDED(hr)); }
#define RDERR(exp)									{ HRESULT hr=exp; if(hr!=D3D_OK) return RDWriteLog(hr,#exp,__FILE__,__LINE__); }

enum
{
	RENDERSTATE_MAX=210,
	TEXTURE_MAX=8,
	TEXTURESTATE_MAX=33,
	SAMPLERSTATE_MAX=14,
};

inline uint32_t F2DW( float f ) { return *((uint32_t*)&f); }

class cD3DRender : public cInterfaceRenderDevice
{
private:
    friend class DrawType;

    DrawTypeMixed dtMixed;

    sTextureFormatData			TexFmtData[SURFMT_NUMBER];

    int							NumberPolygon,NumDrawObject;
    int                         xScrMin,yScrMin,xScrMax,yScrMax;
    
    bool isOrthographicProjSet = false;

    void UpdateD3DVertexBuffer(VertexBuffer* vb, size_t len);
    void UpdateD3DIndexBuffer(IndexBuffer* ib, size_t len);
    void OutText(int x,int y,const char *string,int r=255,int g=255,int b=255);
    void OutText(int x,int y,const char *string,int r,int g,int b,char *FontName="Arial",int size=12,int bold=0,int italic=0,int underline=0);

public:
    bool				bActiveScene;
    uint32_t				nSupportTexture;

    float				kShadow;

    D3DCAPS9					DeviceCaps;

    DrawType*	dtFixed = nullptr;
    DrawType*	dtAdvance = nullptr;
    DrawType*	dtAdvanceOriginal = nullptr;
    DrawType*   dtDrawActive = nullptr;
    DrawType*   dtDrawShadowActive = nullptr;

    HWND						hWnd;
    
    uint32_t Adapter = 0;
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
    
	cD3DRender();
	~cD3DRender() override;
    
    virtual void SaveStates(const char* fname="states.txt");

    // //// cInterfaceRenderDevice impls start ////

    eRenderDeviceSelection GetRenderSelection() const override {
        return DEVICE_D3D9;
    }

    uint32_t GetWindowCreationFlags() const override;
	int Init(int xScr,int yScr,int mode, SDL_Window* wnd=nullptr, int RefreshRateInHz=0) override;
	bool ChangeSize(int xScr,int yScr,int mode) override;
	int GetClipRect(int *xmin,int *ymin,int *xmax,int *ymax) override;
	int SetClipRect(int xmin,int ymin,int xmax,int ymax) override;
	int Done() override;

	int BeginScene() override;
	int EndScene() override;
	int Fill(int r,int g,int b,int a=255) override;
    void ClearZBuffer() override;
	int Flush(bool wnd=false) override;
	int SetGamma(float fGamma,float fStart=0.f,float fFinish=1.f) override;

    int CreateTilemap(cTileMap *TileMap) override;
    int DeleteTilemap(cTileMap *TileMap) override;
	
	void DeleteVertexBuffer(VertexBuffer &vb) override;
	void DeleteIndexBuffer(IndexBuffer &ib) override;
    void SetActiveDrawBuffer(class DrawBuffer* db) override;
    void SubmitDrawBuffer(class DrawBuffer* db, DrawRange* range) override;
    void SubmitBuffers(ePrimitiveType primitive, class VertexBuffer* vb, size_t vertices, class IndexBuffer* ib, size_t indices, DrawRange* range) override;
	int CreateTexture(class cTexture *Texture,class cFileImage *FileImage,bool enable_assert=true) override;
	int DeleteTexture(class cTexture *Texture) override;
	void* LockTexture(class cTexture *Texture, int& Pitch) override;
    void* LockTextureRect(class cTexture* Texture, int& Pitch, Vect2i pos, Vect2i size) override;
	void UnlockTexture(class cTexture *Texture) override;
    void SetTextureImage(uint32_t slot, struct TextureImage* texture_image) override;
    uint32_t GetMaxTextureSlots() override { return nSupportTexture; }

	void SetDrawNode(class cCamera *DrawNode) override;
	void SetGlobalFog(const sColor4f &color,const Vect2f &v) override;

    void SetGlobalLight(Vect3f *vLight, sColor4f *Ambient = nullptr,
                        sColor4f *Diffuse = nullptr, sColor4f *Specular = nullptr) override;

    void Draw(class ElasticSphere *es) override;
    
    uint32_t GetRenderState(eRenderStateOption option) override;
	int SetRenderState(eRenderStateOption option,uint32_t value) override;

	void OutText(int x,int y,const char *string,const sColor4f& color,int align=-1,eBlendMode blend_mode=ALPHA_BLEND) override;
	void OutText(int x,int y,const char *string,const sColor4f& color,int align,eBlendMode blend_mode,
				cTexture* pTexture,eColorMode mode,Vect2f uv,Vect2f duv,float phase=0,float lerp_factor=1) override;

    void FlushPrimitive2D() override;

    void FlushPrimitive3D() override;

	bool SetScreenShot(const char *fname) override;
	void DrawSprite2(int x,int y,int dx,int dy,float u,float v,float du,float dv,float u1,float v1,float du1,float dv1,
		cTexture *Tex1,cTexture *Tex2,float lerp_factor,float alpha=1,float phase=0,eColorMode mode=COLOR_MOD,eBlendMode blend_mode=ALPHA_NONE) override;

    bool IsEnableSelfShadow() override;

    void SetNoMaterial(eBlendMode blend,float Phase=0,cTexture *Texture0=0,cTexture *Texture1=0,eColorMode color_mode=COLOR_MOD) override;
    void SetBlendState(eBlendMode blend) override;


    void UseOrthographicProjection() override;
    void SetDrawTransform(class cCamera *DrawNode) override;
    void SetWorldMat4f(const Mat4f* matrix) override;

    void BeginDrawMesh(bool obj_mesh, bool use_shadow) override;
    void EndDrawMesh() override;
    void SetSimplyMaterialMesh(cObjMesh* mesh, sDataRenderMaterial* data) override;
    void DrawNoMaterialMesh(cObjMesh* mesh, sDataRenderMaterial* data) override;

    void BeginDrawShadow(bool shadow_map) override;
    void EndDrawShadow() override;
    void SetSimplyMaterialShadow(cObjMesh* mesh, cTexture* texture) override;
    void DrawNoMaterialShadow(cObjMesh* mesh) override;
    
    void SetMaterialTilemap(cTileMap *TileMap) override;
    void SetMaterialTilemapShadow() override;
    void SetTileColor(sColor4f color) override;

    // //// cInterfaceRenderDevice impls end ////

    ///Converts flag based vertex format to D3D9 FVF format
    static uint32_t GetD3DFVFFromFormat(vertex_fmt_t fmt) ;
    
	void SetAdvance();//Вызывать при изменении Option_ShadowType Option_EnableBump

	bool PossibleAnisotropic();
	void SetAnisotropic(bool enable);
	bool GetAnisotropic();

	bool hasAdvanceDrawType(){	return dtAdvanceOriginal != NULL;}

	bool SetFocus(bool wait,bool focus_error=true);
	int KillFocus();
	LPDIRECT3DTEXTURE9 CreateSurface(int x, int y, eSurfaceFormat TextureFormat, int MipMap, bool enable_assert, uint32_t attribute);

	inline IDirect3DBaseTexture9* GetTextureD3D(int dwStage) {
		VISASSERT( dwStage<nSupportTexture );
		return CurrentTexture[dwStage];
	}
    
    void FlushActiveDrawBuffer();

	inline void SetVertexShader(IDirect3DVertexShader9 * Handle)
	{
		if(Handle!=CurrentVertexShader)
		{
            FlushActiveDrawBuffer();
			RDCALL(lpD3DDevice->SetVertexShader(CurrentVertexShader=Handle));
		}
	}

	inline void SetPixelShader(IDirect3DPixelShader9 * Handle)
	{
		if(Handle!=CurrentPixelShader)
		{
            FlushActiveDrawBuffer();
			RDCALL(lpD3DDevice->SetPixelShader(CurrentPixelShader=Handle));
		}
	}

	FORCEINLINE void SetRenderState(D3DRENDERSTATETYPE State, unsigned int Value)
	{
		VISASSERT(0<=State&&State<RENDERSTATE_MAX);
//		DWORD value;
//		RDCALL(lpD3DDevice->GetRenderState((D3DRENDERSTATETYPE)State,&value));
//		VISASSERT(ArrayRenderState[State]==value || ArrayRenderState[State]==0xefefefef);

		if(ArrayRenderState[State]!=Value) 
		{
            FlushActiveDrawBuffer();
			RDCALL(lpD3DDevice->SetRenderState(State,ArrayRenderState[State]=Value));
		}
	}
	inline uint32_t GetRenderState(D3DRENDERSTATETYPE State)
	{
		VISASSERT(0<=State && State<RENDERSTATE_MAX);
		return ArrayRenderState[State];
	}
	FORCEINLINE void SetTextureStageState(unsigned int Stage, D3DTEXTURESTAGESTATETYPE Type, unsigned int Value)
	{
		VISASSERT(Stage<TEXTURE_MAX);
		VISASSERT(0<=Type && Type<TEXTURESTATE_MAX);

//		DWORD value;
///		RDCALL(lpD3DDevice->GetTextureStageState(Stage,(D3DTEXTURESTAGESTATETYPE)Type,&value));
//		VISASSERT(ArrayTextureStageState[Stage][Type]==value || ArrayTextureStageState[Stage][Type]==0xefefefef);

		if(ArrayTextureStageState[Stage][Type]!=Value) 
		{
            FlushActiveDrawBuffer();
			RDCALL(lpD3DDevice->SetTextureStageState(Stage,Type,ArrayTextureStageState[Stage][Type]=Value));
		}
	}

	FORCEINLINE uint32_t GetTextureStageState(unsigned int Stage, D3DTEXTURESTAGESTATETYPE Type)
	{
		return ArrayTextureStageState[Stage][Type];
	}

	inline void SetSamplerState(uint32_t Stage, D3DSAMPLERSTATETYPE Type, uint32_t Value)
	{
		VISASSERT(Stage<TEXTURE_MAX);
		VISASSERT(0<=Type && Type<SAMPLERSTATE_MAX);
		if(ArraytSamplerState[Stage][Type]!=Value)
		{
            FlushActiveDrawBuffer();
			ArraytSamplerState[Stage][Type]=Value;
			RDCALL(lpD3DDevice->SetSamplerState(Stage,Type,Value));
		}
	}

	inline uint32_t GetSamplerState(uint32_t Stage, D3DSAMPLERSTATETYPE Type)
	{
		VISASSERT(Stage<TEXTURE_MAX);
		VISASSERT(0<=Type && Type<SAMPLERSTATE_MAX);
		return ArraytSamplerState[Stage][Type];
	}

	void SetVertexShaderConstant(int StartRegister,const Mat4f *pMat);
	void SetVertexShaderConstant(int StartRegister,const Vect4f *pVect);
	void SetPixelShaderConstant(int StartRegister,const Vect4f *pVect);

	inline void SetTextureTransform(int Stage,Mat4f *matTexSpace)
	{
        FlushActiveDrawBuffer();
		float det=1;
		Mat4f mat,matViewInv;	// matViewWorld=matWorld*matView=matView, because matWorld==ID
		Mat4fInverse(&matViewInv,&det,&DrawNode->matView);
        mat = matViewInv * *matTexSpace;
		RDCALL(lpD3DDevice->SetTransform(D3DTRANSFORMSTATETYPE(D3DTS_TEXTURE0+Stage),
                                         reinterpret_cast<const D3DMATRIX*>(&mat)));
	}

	void SetRenderTarget(cTexture* target,LPDIRECT3DSURFACE9 pZBuffer);
	void RestoreRenderTarget();

	LPDIRECT3DTEXTURE9 CreateTextureFromMemory(void* pSrcData, uint32_t SrcData)
	{
		LPDIRECT3DTEXTURE9 pTexture=NULL;
		HRESULT hr=D3DXCreateTextureFromFileInMemory(lpD3DDevice,
			pSrcData,SrcData,&pTexture);

		if(FAILED(hr))
			return NULL;
		return pTexture;
	}

	void RestoreShader();

protected:
    IDirect3DIndexBuffer9*		CurrentIndexBuffer;
	IDirect3DVertexShader9* 	CurrentVertexShader;	// vertex shader
	IDirect3DPixelShader9*		CurrentPixelShader;
	uint32_t					CurrentFVF;
	int							CurrentMod4; // поддерживаемые тип текстурных операций

    std::vector<VertexBuffer*> LibVB;
    std::vector<IndexBuffer*> LibIB;
	
	uint32_t				ArrayRenderState[RENDERSTATE_MAX];
	uint32_t				ArrayTextureStageState[TEXTURE_MAX][TEXTURESTATE_MAX];
	uint32_t				ArraytSamplerState[TEXTURE_MAX][SAMPLERSTATE_MAX];
	
	void DeleteShader();

	void DeleteDynamicBuffers();
	void RestoreDeviceIfLost();

	std::vector<class cTileMapRender*> tilemaps;
	void ClearTilemapPool();
	void RestoreTilemapPool();

	//void GetTilemapTextureMemory(int& total,int& free);

	void UpdateRenderMode();

	D3DFORMAT GetBackBufferFormat(uint32_t Mode);

	friend class cTileMapRender;
};

int GetTextureFormatSize(D3DFORMAT f);

extern cD3DRender* gb_RenderDevice3D;
