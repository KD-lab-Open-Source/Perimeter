#pragma once

#include "../inc/IRenderDevice.h"
#include "../src/TexLibrary.h"

int RDWriteLog(HRESULT err,char *exp,char *file,int line);
void RDWriteLog(char *exp,int size=-1);

#define RDCALL(exp)									{ HRESULT hr=exp; if(hr!=D3D_OK) RDWriteLog(hr,#exp,__FILE__,__LINE__); VISASSERT(SUCCEEDED(hr)); }
#define RDERR(exp)									{ HRESULT hr=exp; if(hr!=D3D_OK) return RDWriteLog(hr,#exp,__FILE__,__LINE__); }

//Возвращает минимальное число,являющееся степенью двойки и не меньше, чем n
inline int Power2up(int n)
{
	int i=1;
	while(i<n)
		i=i<<1;
	return i;
}

const int POLYGONMAX=1024;

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
	virtual int CreateCubeTexture(class cTexture *Texture, const char* fname)			{ return -1;};
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

	virtual LPDIRECT3DTEXTURE9 CreateTextureFromMemory(void* pSrcData, uint32_t SrcData) {return NULL;}

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
void calcVisMap(cCamera *DrawNode, Vect2i TileNumber, Vect2i TileSize, uint8_t* visMap, bool clear);
void calcVisMap(cCamera *DrawNode, Vect2i TileNumber,Vect2i TileSize,Mat3f& direction,sBox6f& box);

enum TILEMAP_DRAW
{
	TILEMAP_ALL,
	TILEMAP_ZEROPLAST,
	TILEMAP_NOZEROPLAST,
};
