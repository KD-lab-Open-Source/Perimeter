#pragma once

#include "..\inc\IRenderDevice.h"
#include "ddraw.h"
#include "..\src\TexLibrary.h"

int RDWriteLog(HRESULT err,char *exp,char *file,int line);
void RDWriteLog(DDSURFACEDESC2 &ddsd);
void RDWriteLog(char *exp,int size=-1);

#define RDCALL(exp)									{ HRESULT hr=exp; if(hr!=DD_OK) RDWriteLog(hr,#exp,__FILE__,__LINE__); VISASSERT(SUCCEEDED(hr)); }
#define RDERR(exp)									{ HRESULT hr=exp; if(hr!=DD_OK) return RDWriteLog(hr,#exp,__FILE__,__LINE__); }

//Возвращает минимальное число,являющееся степенью двойки и не меньше, чем n
inline int Power2up(int n)
{
	int i=1;
	while(i<n)
		i=i<<1;
	return i;
}

const int POLYGONMAX=1024;

enum eMaterialMode
{
	MAT_COLOR_ADD_SPECULAR	=1<<4,	

	MAT_ALPHA_ADDBLENDALPHA	=1<<5,	
	MAT_ALPHA_BLEND			=1<<6,	
	MAT_ALPHA_ADDBLEND		=1<<7,	
	MAT_ALPHA_SUBBLEND		=1<<29,
	MAT_BUMP				=1<<9,

	MAT_ALPHA_TEST			=1<<8,

	MAT_IS_BLEND			= MAT_ALPHA_ADDBLENDALPHA|MAT_ALPHA_BLEND|MAT_ALPHA_ADDBLEND|MAT_ALPHA_SUBBLEND,
	// only d3d version
	// render type
	MAT_TEXMATRIX_STAGE1	=1<<16,
	MAT_TEXNORMAL_STAGE2	=1<<18,
	
	MAT_RENDER_SPHEREMAP	=1<<22,
	MAT_LIGHT				=1<<31
};

enum eRenderStateCullMode
{
    RENDERSTATE_CULL_NONE	=	1,
    RENDERSTATE_CULL_CW		=	2,
    RENDERSTATE_CULL_CCW	=	3,
    RENDERSTATE_CULL_FORCE	=	0x7fffffff,
};
enum eRenderStateTextureAddress 
{
    TADDRESS_WRAP			= 1,
    TADDRESS_MIRROR			= 2,
    TADDRESS_CLAMP			= 3,
    TADDRESS_BORDER			= 4,
    TADDRESS_FORCE_DWORD	= 0x7fffffff, 
};

struct sTextureFormatData
{
	int rBitCount,gBitCount,bBitCount,aBitCount;
	int rBitShift,gBitShift,bBitShift,aBitShift;
	int BytePerPixel;
	D3DFORMAT TexFmtD3D;
	
	sTextureFormatData& Set(int bpp,int rcount,int gcount,int bcount,int acount,int rshift,int gshift,int bshift,int ashift,int texFmtD3D=D3DFMT_UNKNOWN)
	{
		BytePerPixel=bpp;
		rBitCount=rcount; gBitCount=gcount; bBitCount=bcount; aBitCount=acount;
		rBitShift=rshift; gBitShift=gshift;	bBitShift=bshift; aBitShift=ashift;
		TexFmtD3D=(D3DFORMAT)texFmtD3D;
		return *this;
	}
	void Get(int &bpp,int &rcount,int &gcount,int &bcount,int &acount,int &rshift,int &gshift,int &bshift,int &ashift)
	{
		bpp=BytePerPixel;
		rcount=rBitCount; gcount=gBitCount; bcount=bBitCount; acount=aBitCount;
		rshift=rBitShift; gshift=gBitShift;	bshift=bBitShift; ashift=aBitShift;
	}
};

struct sDataRenderMaterial
{
	sColor4f	Ambient;
	sColor4f	Diffuse;
	sColor4f	Specular;
	sColor4f	Emissive;
	float		Power;

	float Phase;
	int			mat;//eMaterialMode
	cTexture	*Tex[2];
	MatXf		TexMatrix;
	float		MaterialAnimPhase;

	sDataRenderMaterial()			{ Phase=0; MaterialAnimPhase=0; }
};

class cIUnkClass;
class cObjMesh;

class cURenderDevice : public cInterfaceRenderDevice
{
public:
	cURenderDevice();
	~cURenderDevice();

	//D3DRender only
	virtual void SetNoMaterial(eBlendMode blend,float Phase=0,cTexture *Texture0=0,cTexture *Texture1=0,eColorMode color_mode=COLOR_MOD){};
	virtual int CreateBumpTexture(class cTexture *Texture) {return -1;};

	/////
	virtual int GetSizeX(){return xScr;};
	virtual int GetSizeY(){return yScr;};

	virtual void SetFont(cFont *pFont);
	virtual void SetDefaultFont(cFont *pFont);

	virtual int CreateTexture(class cTexture *Texture,class cFileImage *FileImage,int dxout,int dyout,bool enable_assert=true)	{ return -1; }
	virtual int CreateCubeTexture(class cTexture *Texture,LPCSTR fname)			{ return -1;};
	virtual int DeleteTexture(class cTexture *Texture)							{ return -1; }
	virtual void* LockTexture(class cTexture *Texture,int& Pitch)				{VISASSERT(0);return NULL;}
	virtual void UnlockTexture(class cTexture *Texture)							{VISASSERT(0);}

	virtual void SetDrawNode(class cCamera *DrawNode)							{ }
	inline  cCamera *GetDrawNode()												{return DrawNode;}
	virtual void SetGlobalFog(const sColor4f &color,const Vect2f &v)			{ }
	virtual void SetGlobalLight(Vect3f *vLight,sColor4f *Ambient=0,sColor4f *Diffuse=0,sColor4f *Specular=0) { }

	virtual void* GetStripBuffer()												{return &Buffer[0];}
	virtual int GetStripBufferLen()												{return Buffer.length();}

	virtual void Draw(class cScene *Scene)										{ }

	virtual void Draw(class FieldDispatcher *ffd)							{ }
	virtual void CreateFFDData(class FieldDispatcher *rd)					{ }
	virtual void DeleteFFDData(class FieldDispatcher *rd)					{ }

	virtual void Draw(class ElasticSphere *es)									{ }
	virtual float GetFontLength(const char *string)							{ return 0;}

	virtual int SetRenderState(eRenderStateOption option,int value)				{ return -1; }

	virtual int GetDrawNumberPolygon()											{return NumberPolygon;};
	virtual void FlushPrimitive3D()												{ }
	virtual void DrawPoint(const Vect3f &v1,sColor4c color)						{ }
	virtual void DrawLine(const Vect3f &v1,const Vect3f &v2,sColor4c color)		{ }

	virtual void RestoreDeviceIfLost()											{ }

	DWORD GetRenderMode(){return RenderMode;}

	virtual LPDIRECT3DTEXTURE9 CreateTextureFromMemory(void* pSrcData, UINT SrcData) {return NULL;}

	virtual void SetDialogBoxMode(bool enable)=0;
//protected:
	

	inline cTexture* GetTexture(int n)					{ return TexLibrary.GetTexture(n); }
	int GetSizeFromFmt(int fmt);

	cTexLibrary			TexLibrary;

	bool				bActiveScene;
	bool				bWireFrame;
	DWORD				nSupportTexture;

	float				kShadow;
protected:
	cBaseDynArray<char>			Buffer;
	
	class cCamera				*DrawNode;
	
	sTextureFormatData			TexFmtData[SURFMT_NUMBER];
	class cFont					*CurrentFont;
	class cFont					*DefaultFont;

	int							NumberPolygon,NumDrawObject;
	int							RenderMode,xScrMin,yScrMin,xScrMax,yScrMax;
	int							xScr,yScr;//Величина экрана

};

void BuildMipMap(int x,int y,int bpp,int bplSrc,void *pSrc,int bplDst,void *pDst,
			 int rc,int gc,int bc,int ac,int rs,int gs,int bs,int as,int Blur=0);
void BuildDot3Map(int x,int y,void *pSrc,void *pDst);
void BuildBumpMap(int x,int y,void *pSrc,void *pDst,int fmtBumpMap);

extern class cURenderDevice *gb_RenderDevice;

//Величина visMap должна быть TileMap->GetTileNumber().x*visMapDy=TileMap->GetTileNumber().y
void calcVisMap(cCamera *DrawNode, Vect2i TileNumber,Vect2i TileSize,BYTE* visMap,bool clear);
void calcVisMap(cCamera *DrawNode, Vect2i TileNumber,Vect2i TileSize,Mat3f& direction,sBox6f& box);

enum TILEMAP_DRAW
{
	TILEMAP_ALL,
	TILEMAP_ZEROPLAST,
	TILEMAP_NOZEROPLAST,
};
