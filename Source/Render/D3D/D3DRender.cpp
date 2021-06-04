#include "StdAfxRD.h"
#include "Font.h"

int sVertexXYZ::fmt		=	D3DFVF_XYZ;
int sVertexXYZD::fmt	=	D3DFVF_XYZ|D3DFVF_DIFFUSE;
int sVertexXYZT1::fmt	=	D3DFVF_XYZ|D3DFVF_TEX1;
int sVertexXYZDS::fmt	=	D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_SPECULAR;
int sVertexXYZDST1::fmt	=	D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_SPECULAR|D3DFVF_TEX1;
int sVertexXYZDST2::fmt	=	D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_SPECULAR|D3DFVF_TEX2;
int sVertexXYZDT1::fmt	=	D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1;
int sVertexXYZDT2::fmt	=	D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX2;
int sVertexXYZN::fmt	=	D3DFVF_XYZ|D3DFVF_NORMAL;
int sVertexXYZND::fmt	=	D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_DIFFUSE;
int sVertexXYZNDS::fmt	=	D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_DIFFUSE|D3DFVF_SPECULAR;
int sVertexXYZNDST1::fmt=	D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_DIFFUSE|D3DFVF_SPECULAR|D3DFVF_TEX1;
int sVertexXYZNDST2::fmt=	D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_DIFFUSE|D3DFVF_SPECULAR|D3DFVF_TEX2;
int sVertexXYZNT1::fmt	=	D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX1;
int sVertexXYZW::fmt	=	D3DFVF_XYZRHW;
int sVertexXYZWD::fmt	=	D3DFVF_XYZRHW|D3DFVF_DIFFUSE;
int sVertexXYZWT1::fmt	=	D3DFVF_XYZRHW|D3DFVF_TEX1;
int sVertexXYZWDT1::fmt	=	D3DFVF_XYZRHW|D3DFVF_DIFFUSE|D3DFVF_TEX1;
int sVertexXYZWDT2::fmt	=	D3DFVF_XYZRHW|D3DFVF_DIFFUSE|D3DFVF_TEX2;
int sVertexXYZDP::fmt	=	D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_PSIZE;
int sVertexDot3::fmt	=	D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX4|D3DFVF_TEXCOORDSIZE2(0)| D3DFVF_TEXCOORDSIZE3(1)|D3DFVF_TEXCOORDSIZE3(2)|D3DFVF_TEXCOORDSIZE3(3);

int sVertexXYZINT1::fmt	=	D3DFVF_XYZ|D3DFVF_XYZB1|D3DFVF_LASTBETA_UBYTE4|D3DFVF_NORMAL|D3DFVF_TEX1;
int sVertexXYZW4INT1::fmt	=	D3DFVF_XYZ|D3DFVF_XYZB5|D3DFVF_LASTBETA_UBYTE4|D3DFVF_NORMAL|D3DFVF_TEX1;

BYTE& cSkinVertex::GetWeight(int idx)
{
	VISASSERT(num_weight>0);
	BYTE* b=(BYTE*)(7+cur);
	switch(idx)
	{
	case 0:
		return b[2];
	case 1:
		return b[1];
	case 2:
		return b[0];
	case 3:
		return b[3];
	}

	VISASSERT(0);
	return b[0];
}
int cSkinVertex::GetFormat()
{
	//int fmt=D3DFVF_XYZ|D3DFVF_LASTBETA_UBYTE4|D3DFVF_NORMAL|D3DFVF_TEX1;
	int fmt=D3DFVF_XYZ|D3DFVF_LASTBETA_D3DCOLOR|D3DFVF_NORMAL;
	
	switch(num_weight+1)
	{
	case 1:
		fmt|=D3DFVF_XYZB1;
		break;
	case 2:
		xassert(0);
	case 3:
	case 4:
	case 5:
		fmt|=D3DFVF_XYZB1|D3DFVF_DIFFUSE;
		break;
	default:
		xassert(0);
	}

	if(bump)
		fmt|=D3DFVF_TEX4|D3DFVF_TEXCOORDSIZE2(0)|D3DFVF_TEXCOORDSIZE3(1)|D3DFVF_TEXCOORDSIZE3(2)|D3DFVF_TEXCOORDSIZE3(3);
	else
		fmt|=D3DFVF_TEX1;
	return fmt;
}

class cD3DRender *gb_RenderDevice3D=NULL;
void IsDeleteAllDefaultTextures();

int GetMaskBitCount(unsigned int BitMask)
{
	int BitCount;
    for(BitCount=0;BitMask;BitMask>>=1)
        BitCount+=(BitMask&0x1);
	return BitCount;
}
int GetMaskBitShift(unsigned int BitMask)
{
	if(BitMask==0) return 0;
	int BitShift;
    for(BitShift=0;(BitMask&1)==0;BitMask>>=1)
        BitShift++;
	return BitShift;
}

cD3DRender::cD3DRender()
{
	gb_RenderDevice3D=this;
	hWnd=0;
	
	lpD3D=0;
	lpD3DDevice=0;
    lpZBuffer=lpBackBuffer=0;

	TexFmtData[SURFMT_COLOR].Set( 0, 0,0,0,0, -1,-1,-1,-1 );
	TexFmtData[SURFMT_COLORALPHA].Set( 0, 0,0,0,0, -1,-1,-1,-1 );
	TexFmtData[SURFMT_RENDERMAP16].Set( 0, 0,0,0,0, -1,-1,-1,-1 );
	TexFmtData[SURFMT_RENDERMAP32].Set( 0, 0,0,0,0, -1,-1,-1,-1 );
	TexFmtData[SURFMT_BUMP].Set( 0, 0,0,0,0, -1,-1,-1,-1 );
	TexFmtData[SURFMT_GRAYALPHA].Set( 0, 0,0,0,0, -1,-1,-1,-1 );
	TexFmtData[SURFMT_UV].Set( 0, 0,0,0,0, -1,-1,-1,-1 );
	TexFmtData[SURFMT_U16V16].Set( 0, 0,0,0,0, -1,-1,-1,-1 );

	MaxTextureAspectRatio=0;

	dtFixed=NULL;
	dtAdvance=NULL;
	dtAdvanceOriginal=NULL;
	current_bump_scale=1.0f;
	texture_interpolation=D3DTEXF_LINEAR;
}

cD3DRender::~cD3DRender()
{
	Done();
}

bool cD3DRender::CheckDeviceType(int xscr,int yscr,int Mode)
{
	DWORD Adapter=0;
	if(!lpD3D)
	{
		lpD3D=Direct3DCreate9(D3D9b_SDK_VERSION);//Временно, когда появится специфичное для 9.0c переправить обратно
		//lpD3D=Direct3DCreate9(D3D_SDK_VERSION);
		if(!lpD3D)
			return false;
	}

	if(Mode&RENDERDEVICE_MODE_WINDOW)
		return true;

	D3DFORMAT BackBufferFormat=GetBackBufferFormat(Mode);
	UINT modes=lpD3D->GetAdapterModeCount(Adapter,BackBufferFormat);

	for(int i=0;i<modes;i++)
	{
		D3DDISPLAYMODE mode;
		if(FAILED(lpD3D->EnumAdapterModes(Adapter,
				BackBufferFormat,
				i,
				&mode
		)))
			return false;

		if(mode.Width==xscr && mode.Height==yscr && 
			mode.Format==BackBufferFormat)
			return true;
	}

	return false;
}


int cD3DRender::Init(int xscr,int yscr,int Mode,void *lphWnd,int RefreshRateInHz)
{ 
	if(lphWnd==0) return 1;
	Done();

	memset(CurrentTexture,0,sizeof(CurrentTexture));
	memset(ArrayRenderState,0xEF,sizeof(ArrayRenderState));

	hWnd=(HWND)lphWnd;
	RenderMode=Mode;

	if(!lpD3D)
		//RDERR((lpD3D=Direct3DCreate9(D3D_SDK_VERSION))==0);
		RDERR((lpD3D=Direct3DCreate9(D3D9b_SDK_VERSION))==0);
		
	if(lpD3D==0) return 2;
	D3DDISPLAYMODE d3ddm;
	DWORD Adapter=0/*D3DADAPTER_DEFAULT*/;
	RDERR(lpD3D->GetAdapterDisplayMode(Adapter,&d3ddm));
	if(Mode&RENDERDEVICE_MODE_WINDOW)
		if(d3ddm.Format==D3DFMT_X8R8G8B8||d3ddm.Format==D3DFMT_R8G8B8||d3ddm.Format==D3DFMT_A8R8G8B8)
			RenderMode&=~RENDERDEVICE_MODE_RGB16,RenderMode|=RENDERDEVICE_MODE_RGB32;
		else
			RenderMode&=~RENDERDEVICE_MODE_RGB32,RenderMode|=RENDERDEVICE_MODE_RGB16;

	if(!(RenderMode&RENDERDEVICE_MODE_RGB32))
		RenderMode|=RENDERDEVICE_MODE_RGB16;
		
//	if(TexFmtData[SURFMT_RENDERMAP].TexFmtD3D==D3DFMT_UNKNOWN||TexFmtData[SURFMT_COLORALPHA].TexFmtD3D==D3DFMT_UNKNOWN||TexFmtData[SURFMT_COLOR].TexFmtD3D==D3DFMT_UNKNOWN)
//		return 5; // don't support render to texture
	RDCALL(lpD3D->GetDeviceCaps(Adapter,D3DDEVTYPE_HAL,&DeviceCaps));
	ZeroMemory(&d3dpp,sizeof(d3dpp));
	d3dpp.BackBufferWidth			= xScr=xscr;
	d3dpp.BackBufferHeight			= yScr=yscr;
	d3dpp.MultiSampleType			= D3DMULTISAMPLE_NONE;
	d3dpp.MultiSampleQuality		= 0;
	d3dpp.SwapEffect				= D3DSWAPEFFECT_DISCARD;
	d3dpp.hDeviceWindow				= (HWND)lphWnd;

	d3dpp.EnableAutoDepthStencil	= TRUE;
	d3dpp.Flags						= D3DPRESENTFLAG_DISCARD_DEPTHSTENCIL;
	d3dpp.FullScreen_RefreshRateInHz= (Mode&RENDERDEVICE_MODE_WINDOW)?0:RefreshRateInHz;
    d3dpp.PresentationInterval		= D3DPRESENT_INTERVAL_IMMEDIATE;
	UpdateRenderMode();

	bSupportVertexShaderHardware=bSupportVertexShader=(D3DSHADER_VERSION_MAJOR(DeviceCaps.VertexShaderVersion)>=1);

	DWORD mt=D3DCREATE_FPU_PRESERVE;

	if(RenderMode&RENDERDEVICE_MODE_MULTITHREAD)
		mt|=D3DCREATE_MULTITHREADED;

	if(RenderMode&RENDERDEVICE_MODE_REF)
	{
		RDERR(lpD3D->CreateDevice(Adapter,D3DDEVTYPE_REF,hWnd,D3DCREATE_SOFTWARE_VERTEXPROCESSING|mt,&d3dpp,&lpD3DDevice));
	}else
	if(bSupportVertexShaderHardware)
	{
		if(lpD3D->CreateDevice(Adapter,D3DDEVTYPE_HAL,hWnd,D3DCREATE_HARDWARE_VERTEXPROCESSING|mt,&d3dpp,&lpD3DDevice))
			if(lpD3D->CreateDevice(Adapter,D3DDEVTYPE_HAL,hWnd,D3DCREATE_MIXED_VERTEXPROCESSING|mt,&d3dpp,&lpD3DDevice))
				RDERR(lpD3D->CreateDevice(Adapter,D3DDEVTYPE_HAL,hWnd,D3DCREATE_SOFTWARE_VERTEXPROCESSING|mt,&d3dpp,&lpD3DDevice));
	}
	else
		if(lpD3D->CreateDevice(Adapter,D3DDEVTYPE_HAL,hWnd,D3DCREATE_MIXED_VERTEXPROCESSING|mt,&d3dpp,&lpD3DDevice))
			RDERR(lpD3D->CreateDevice(Adapter,D3DDEVTYPE_HAL,hWnd,D3DCREATE_SOFTWARE_VERTEXPROCESSING|mt,&d3dpp,&lpD3DDevice));

	if(lpD3DDevice==0) return 3;

	if(Mode&RENDERDEVICE_MODE_WINDOW)
	{
		D3DDISPLAYMODE mode;
		if(SUCCEEDED(lpD3DDevice->GetDisplayMode(0, &mode)))
		{
			if(mode.Format==D3DFMT_X8R8G8B8||mode.Format==D3DFMT_R8G8B8||mode.Format==D3DFMT_A8R8G8B8)
				RenderMode&=~RENDERDEVICE_MODE_RGB16,RenderMode|=RENDERDEVICE_MODE_RGB32;
			else
				RenderMode&=~RENDERDEVICE_MODE_RGB32,RenderMode|=RENDERDEVICE_MODE_RGB16;
		}

	}

	SetClipRect(xScrMin=0,yScrMin=0,xScrMax=xScr,yScrMax=yScr);

	RDCALL(lpD3DDevice->GetDeviceCaps(&DeviceCaps));
	dwSuportMaxSizeTextureX=DeviceCaps.MaxTextureWidth;
	dwSuportMaxSizeTextureY=DeviceCaps.MaxTextureHeight;
	bSupportVertexShader=(D3DSHADER_VERSION_MAJOR(DeviceCaps.VertexShaderVersion)>=1)?1:0;
	bSupportVertexFog=(DeviceCaps.RasterCaps&D3DPRASTERCAPS_FOGVERTEX)?1:0;
	bSupportTableFog=(DeviceCaps.RasterCaps&D3DPRASTERCAPS_FOGTABLE)?1:0;
	CurrentMod4 = (DeviceCaps.TextureOpCaps&D3DTEXOPCAPS_MODULATE4X)?D3DTOP_MODULATE4X:((DeviceCaps.TextureOpCaps&D3DTEXOPCAPS_MODULATE2X)?D3DTOP_MODULATE2X:D3DTOP_MODULATE);
	CurrentBumpMap = DeviceCaps.TextureOpCaps&D3DTEXOPCAPS_BUMPENVMAP ? D3DTOP_BUMPENVMAP : 0;

	MaxTextureAspectRatio=DeviceCaps.MaxTextureAspectRatio;
	nSupportTexture=min(DeviceCaps.MaxTextureBlendStages,TEXTURE_MAX-1);

	SetFocus(false);
/*
	void *lpBuffer=new char[3*xScr*yScr];
	ScreenShot(lpBuffer,xScr*yScr*3);
	cFileImage *f=cFileImage::Create("test.tga");
	f->save("test.tga",lpBuffer,24,xScr,yScr);
	delete f;
	delete lpBuffer;
*/
	InitVertexBuffers();
	Fill(0,0,0,255);
	BeginScene();
	DrawRectangle(0,0,xScr-1,yScr-1,sColor4c(0,0,0,255));
	EndScene();
	
	VISASSERT(gb_RenderDevice==this);

	Flush();

	dtFixed=new DrawTypeFixedPipeline;
	if(DeviceCaps.PixelShaderVersion>= D3DPS_VERSION(2,0) && lpD3D->CheckDeviceFormat(Adapter,D3DDEVTYPE_HAL,d3ddm.Format,0,D3DRTYPE_TEXTURE,D3DFMT_D16)==0)
		dtAdvanceOriginal=new DrawTypeGeforceFX;
	else
	if( DeviceCaps.PixelShaderVersion>= D3DPS_VERSION(2,0) && 
		lpD3D->CheckDeviceFormat(Adapter,D3DDEVTYPE_HAL,d3ddm.Format,D3DUSAGE_RENDERTARGET,D3DRTYPE_TEXTURE,D3DFMT_R32F)==0)
		dtAdvanceOriginal=new DrawTypeRadeon9700;
	else
	if(lpD3D->CheckDeviceFormat(Adapter,D3DDEVTYPE_HAL,d3ddm.Format,0,D3DRTYPE_TEXTURE,D3DFMT_D16)==0)
		dtAdvanceOriginal=new DrawTypeGeforce3;
	else
	if( DeviceCaps.PixelShaderVersion>= D3DPS_VERSION(1,4))
		dtAdvanceOriginal=new DrawTypeRadeon8500;

	VISASSERT(occlusion_query.empty());

	SetAdvance();

	InitStandartIB();

	if(bSupportTableFog)
	{
		if(!(dtAdvanceOriginal && (dtAdvanceOriginal->GetID()!=DT_GEFORCE3 && dtAdvanceOriginal->GetID()!=DT_GEFORCEFX)))
			bSupportTableFog=false;
	}

	return 0;
}

D3DFORMAT cD3DRender::GetBackBufferFormat(int Mode)
{
	DWORD Adapter=0;
	D3DFORMAT BackBufferFormat=D3DFMT_X8R8G8B8;
	if(Mode&RENDERDEVICE_MODE_WINDOW)
	{
		D3DDISPLAYMODE d3ddm;
		DWORD Adapter=0;
		RDCALL(lpD3D->GetAdapterDisplayMode(Adapter,&d3ddm));
		BackBufferFormat = d3ddm.Format;
	}else
	{
		if(Mode&RENDERDEVICE_MODE_RGB32)
		{
			if(lpD3D->GetAdapterModeCount(Adapter,D3DFMT_X8R8G8B8)>0)
				BackBufferFormat = D3DFMT_X8R8G8B8;
			else
				BackBufferFormat = D3DFMT_R8G8B8;
		}else
		{
			if(lpD3D->GetAdapterModeCount(Adapter,D3DFMT_R5G6B5)>0)
				BackBufferFormat = D3DFMT_R5G6B5;
			else
			if(lpD3D->GetAdapterModeCount(Adapter,D3DFMT_X1R5G5B5)>0)
				BackBufferFormat = D3DFMT_X1R5G5B5;
			else
				BackBufferFormat = D3DFMT_A1R5G5B5;
		}
	}

	if(Mode&RENDERDEVICE_MODE_ALPHA)
		BackBufferFormat=D3DFMT_A8R8G8B8;
	return BackBufferFormat;
}

void cD3DRender::UpdateRenderMode()
{
	if(!(RenderMode&RENDERDEVICE_MODE_RGB32))
		RenderMode|=RENDERDEVICE_MODE_RGB16;

	bool is32zbuffer=false;
	if(RenderMode&(RENDERDEVICE_MODE_STRENCIL|RENDERDEVICE_MODE_Z24))
	{
		is32zbuffer=true;
	}else
	{
		is32zbuffer=(RenderMode&RENDERDEVICE_MODE_RGB32)?true:false;
	}

	d3dpp.AutoDepthStencilFormat= is32zbuffer?D3DFMT_D24S8:D3DFMT_D16;
	d3dpp.BackBufferFormat=GetBackBufferFormat(RenderMode);

	d3dpp.BackBufferCount			= (RenderMode&RENDERDEVICE_MODE_WINDOW)?1:2;
	d3dpp.Windowed					= (RenderMode&(RENDERDEVICE_MODE_WINDOW|RENDERDEVICE_MODE_ONEBACKBUFFER))?TRUE:FALSE;

	D3DDISPLAYMODE d3ddm;
	DWORD Adapter=0;
	RDCALL(lpD3D->GetAdapterDisplayMode(Adapter,&d3ddm));
	if(RenderMode&RENDERDEVICE_MODE_COMPRESS)
	{
		if(lpD3D->CheckDeviceFormat(Adapter,D3DDEVTYPE_HAL,d3ddm.Format,0,D3DRTYPE_TEXTURE,D3DFMT_DXT5)==0)
			TexFmtData[SURFMT_COLOR].Set(4,8,8,8,8,16,8,0,24,D3DFMT_DXT5);
		if(lpD3D->CheckDeviceFormat(Adapter,D3DDEVTYPE_HAL,d3ddm.Format,0,D3DRTYPE_TEXTURE,D3DFMT_DXT3)==0)
			TexFmtData[SURFMT_COLOR].Set(4,8,8,8,8,16,8,0,24,D3DFMT_DXT3);
		if(lpD3D->CheckDeviceFormat(Adapter,D3DDEVTYPE_HAL,d3ddm.Format,0,D3DRTYPE_TEXTURE,D3DFMT_DXT4)==0)
			TexFmtData[SURFMT_COLORALPHA].Set(4,8,8,8,8,16,8,0,24,D3DFMT_DXT4);
		if(lpD3D->CheckDeviceFormat(Adapter,D3DDEVTYPE_HAL,d3ddm.Format,0,D3DRTYPE_TEXTURE,D3DFMT_DXT2)==0)
			TexFmtData[SURFMT_COLORALPHA].Set(4,8,8,8,8,16,8,0,24,D3DFMT_DXT2);
    }
	else if(RenderMode&RENDERDEVICE_MODE_RGB32)
	{
		if(lpD3D->CheckDeviceFormat(Adapter,D3DDEVTYPE_HAL,d3ddm.Format,0,D3DRTYPE_TEXTURE,D3DFMT_A8R8G8B8)==0)
			TexFmtData[SURFMT_COLOR].Set(4,8,8,8,8,16,8,0,24,D3DFMT_A8R8G8B8);
		if(lpD3D->CheckDeviceFormat(Adapter,D3DDEVTYPE_HAL,d3ddm.Format,0,D3DRTYPE_TEXTURE,D3DFMT_X8R8G8B8)==0)
			TexFmtData[SURFMT_COLOR].Set(4,8,8,8,8,16,8,0,24,D3DFMT_X8R8G8B8);
		if(lpD3D->CheckDeviceFormat(Adapter,D3DDEVTYPE_HAL,d3ddm.Format,0,D3DRTYPE_TEXTURE,D3DFMT_R8G8B8)==0)
			TexFmtData[SURFMT_COLOR].Set(3,8,8,8,0,16,8,0,0,D3DFMT_R8G8B8);
		if(lpD3D->CheckDeviceFormat(Adapter,D3DDEVTYPE_HAL,d3ddm.Format,0,D3DRTYPE_TEXTURE,D3DFMT_A8R8G8B8)==0)
			TexFmtData[SURFMT_COLORALPHA].Set(4,8,8,8,8,16,8,0,24,D3DFMT_A8R8G8B8);
	}
	else // if(RenderMode&RENDERDEVICE_MODE_RGB16)
	{
		if(lpD3D->CheckDeviceFormat(Adapter,D3DDEVTYPE_HAL,d3ddm.Format,0,D3DRTYPE_TEXTURE,D3DFMT_A4R4G4B4)==0)
			TexFmtData[SURFMT_COLOR].Set(2,4,4,4,4,8,4,0,12,D3DFMT_A4R4G4B4);
		if(lpD3D->CheckDeviceFormat(Adapter,D3DDEVTYPE_HAL,d3ddm.Format,0,D3DRTYPE_TEXTURE,D3DFMT_X4R4G4B4)==0)
			TexFmtData[SURFMT_COLOR].Set(2,4,4,4,4,8,4,0,12,D3DFMT_X4R4G4B4);
		if(lpD3D->CheckDeviceFormat(Adapter,D3DDEVTYPE_HAL,d3ddm.Format,0,D3DRTYPE_TEXTURE,D3DFMT_X1R5G5B5)==0)
			TexFmtData[SURFMT_COLOR].Set(2,5,5,5,1,10,5,0,15,D3DFMT_X1R5G5B5);
		if(lpD3D->CheckDeviceFormat(Adapter,D3DDEVTYPE_HAL,d3ddm.Format,0,D3DRTYPE_TEXTURE,D3DFMT_R5G6B5)==0)
			TexFmtData[SURFMT_COLOR].Set(2,5,6,5,0,11,5,0,0,D3DFMT_R5G6B5);
		if(lpD3D->CheckDeviceFormat(Adapter,D3DDEVTYPE_HAL,d3ddm.Format,0,D3DRTYPE_TEXTURE,D3DFMT_A4R4G4B4)==0)
			TexFmtData[SURFMT_COLORALPHA].Set(2,4,4,4,4,8,4,0,12,D3DFMT_A4R4G4B4);
	}

	//32
	if(lpD3D->CheckDeviceFormat(Adapter,D3DDEVTYPE_HAL,d3ddm.Format,0,D3DRTYPE_TEXTURE,D3DFMT_A8R8G8B8)==0)
		TexFmtData[SURFMT_COLOR32].Set(4,8,8,8,8,16,8,0,24,D3DFMT_A8R8G8B8);
	if(lpD3D->CheckDeviceFormat(Adapter,D3DDEVTYPE_HAL,d3ddm.Format,0,D3DRTYPE_TEXTURE,D3DFMT_X8R8G8B8)==0)
		TexFmtData[SURFMT_COLOR32].Set(4,8,8,8,8,16,8,0,24,D3DFMT_X8R8G8B8);
	if(lpD3D->CheckDeviceFormat(Adapter,D3DDEVTYPE_HAL,d3ddm.Format,0,D3DRTYPE_TEXTURE,D3DFMT_A8R8G8B8)==0)
		TexFmtData[SURFMT_COLORALPHA32].Set(4,8,8,8,8,16,8,0,24,D3DFMT_A8R8G8B8);

	// bump map format
	if(lpD3D->CheckDeviceFormat(Adapter,D3DDEVTYPE_HAL,d3ddm.Format,0,D3DRTYPE_TEXTURE,D3DFMT_Q8W8V8U8)==0)
		TexFmtData[SURFMT_BUMP].Set(4,8,8,8,8,16,8,0,24,D3DFMT_Q8W8V8U8);
		
	// render map format
	if(lpD3D->CheckDeviceFormat(Adapter,D3DDEVTYPE_HAL,d3ddm.Format,D3DUSAGE_RENDERTARGET,D3DRTYPE_TEXTURE,D3DFMT_X8R8G8B8)==0)
		TexFmtData[SURFMT_RENDERMAP32].Set(4,8,8,8,8,16,8,0,24,D3DFMT_X8R8G8B8);
	else
	if(lpD3D->CheckDeviceFormat(Adapter,D3DDEVTYPE_HAL,d3ddm.Format,D3DUSAGE_RENDERTARGET,D3DRTYPE_TEXTURE,D3DFMT_A8R8G8B8)==0)
		TexFmtData[SURFMT_RENDERMAP32].Set(4,8,8,8,8,16,8,0,24,D3DFMT_A8R8G8B8);
	else
	if(lpD3D->CheckDeviceFormat(Adapter,D3DDEVTYPE_HAL,d3ddm.Format,0,D3DRTYPE_TEXTURE,D3DFMT_R8G8B8)==0)
		TexFmtData[SURFMT_RENDERMAP32].Set(3,8,8,8,0,16,8,0,0,D3DFMT_R8G8B8);

	if(lpD3D->CheckDeviceFormat(Adapter,D3DDEVTYPE_HAL,d3ddm.Format,D3DUSAGE_RENDERTARGET,D3DRTYPE_TEXTURE,D3DFMT_R5G6B5)==0)
		TexFmtData[SURFMT_RENDERMAP16].Set(2,5,6,5,0,11,5,0,0,D3DFMT_R5G6B5);
	else
	if(lpD3D->CheckDeviceFormat(Adapter,D3DDEVTYPE_HAL,d3ddm.Format,D3DUSAGE_RENDERTARGET,D3DRTYPE_TEXTURE,D3DFMT_A4R4G4B4)==0)
		TexFmtData[SURFMT_RENDERMAP16].Set(2,4,4,4,4,8,4,0,12,D3DFMT_A4R4G4B4);
	else
	if(lpD3D->CheckDeviceFormat(Adapter,D3DDEVTYPE_HAL,d3ddm.Format,D3DUSAGE_RENDERTARGET,D3DRTYPE_TEXTURE,D3DFMT_X1R5G5B5)==0)
		TexFmtData[SURFMT_RENDERMAP16].Set(2,5,5,5,1,10,5,0,15,D3DFMT_X1R5G5B5);

	TexFmtData[SURFMT_RENDERMAP_FLOAT].Set(0,0,0,0,0,0,0,0,0,0);
	if(lpD3D->CheckDeviceFormat(Adapter,D3DDEVTYPE_HAL,d3ddm.Format,D3DUSAGE_RENDERTARGET,D3DRTYPE_TEXTURE,D3DFMT_R32F)==0)
		TexFmtData[SURFMT_RENDERMAP_FLOAT].Set(0,0,0,0,0,0,0,0,0,D3DFMT_R32F);

	{
		if(lpD3D->CheckDeviceFormat(Adapter,D3DDEVTYPE_HAL,d3ddm.Format,0,D3DRTYPE_TEXTURE,D3DFMT_A8)==0)
		{
			TexFmtData[SURFMT_GRAYALPHA].Set(1, 0,0,0,8, 0,0,0,0,D3DFMT_A8);
		}else
		if(lpD3D->CheckDeviceFormat(Adapter,D3DDEVTYPE_HAL,d3ddm.Format,0,D3DRTYPE_TEXTURE,D3DFMT_A8L8)==0)
		{
			TexFmtData[SURFMT_GRAYALPHA].Set(2, 8,8,8,8, 0,0,0,8,D3DFMT_A8L8);
		}else
		{
			if(RenderMode&RENDERDEVICE_MODE_RGB32)
				TexFmtData[SURFMT_GRAYALPHA].Set(4,8,8,8,8,16,8,0,24,D3DFMT_A8R8G8B8);
			else
				TexFmtData[SURFMT_GRAYALPHA].Set(2,4,4,4,4,8,4,0,12,D3DFMT_A4R4G4B4);
		}

	}

	if(lpD3D->CheckDeviceFormat(Adapter,D3DDEVTYPE_HAL,d3ddm.Format,0,D3DRTYPE_TEXTURE,D3DFMT_V8U8)==0)
	{
		TexFmtData[SURFMT_UV].Set(2, 8,8,8,8, 0,0,0,8,D3DFMT_V8U8);
	}
	if(lpD3D->CheckDeviceFormat(Adapter,D3DDEVTYPE_HAL,d3ddm.Format,0,D3DRTYPE_TEXTURE,D3DFMT_V16U16)==0)
	{
		TexFmtData[SURFMT_U16V16].Set(4, 16,16,16,16, 0,0,0,16,D3DFMT_V16U16);
	}
}

bool cD3DRender::ChangeSize(int xscr,int yscr,int mode)
{
	MTTexObjAutoLock lock;
	KillFocus();
	int mode_mask=RENDERDEVICE_MODE_ALPHA|RENDERDEVICE_MODE_WINDOW|
				  RENDERDEVICE_MODE_RGB16|RENDERDEVICE_MODE_RGB32;

	RenderMode&=~mode_mask;
	RenderMode|=mode;
	if(RenderMode&RENDERDEVICE_MODE_WINDOW)
	{
		D3DDISPLAYMODE mode;
		if(SUCCEEDED(lpD3DDevice->GetDisplayMode(0, &mode)))
		{
			if(mode.Format==D3DFMT_X8R8G8B8||mode.Format==D3DFMT_R8G8B8||mode.Format==D3DFMT_A8R8G8B8)
				RenderMode&=~RENDERDEVICE_MODE_RGB16,RenderMode|=RENDERDEVICE_MODE_RGB32;
			else
				RenderMode&=~RENDERDEVICE_MODE_RGB32,RenderMode|=RENDERDEVICE_MODE_RGB16;
		}

	}

	d3dpp.BackBufferWidth	= xScr=xscr;
	d3dpp.BackBufferHeight	= yScr=yscr;
	UpdateRenderMode();

	return SetFocus(false,(mode&RENDERDEVICE_MODE_RETURNERROR)?false:true);
}


void cD3DRender::SetAdvance()
{
	bool self_shadow=Option_ShadowType==SHADOW_MAP_SELF;
	if(dtAdvanceOriginal)
	{
		dtAdvance=self_shadow?dtAdvanceOriginal:dtFixed;
		if(!self_shadow && Option_EnableBump)
			dtAdvance=&dtMixed;
	}else
		dtAdvance=dtFixed;
}

bool cD3DRender::IsEnableSelfShadow()
{
	return dtAdvanceOriginal!=NULL;
}


int cD3DRender::Done()
{
	KillFocus();

	if(dtFixed)
	{
		delete dtFixed;
		dtFixed=NULL;
	}

	dtAdvance=NULL;

	if(dtAdvanceOriginal)
	{
		delete dtAdvanceOriginal;
		dtAdvanceOriginal=NULL;
	}

	DeleteIndexBuffer(standart_ib);
	BufferXYZDT1.Destroy();
	BufferXYZDT2.Destroy();
	BufferXYZWD.Destroy();
	BufferXYZD.Destroy();
	BufferXYZWDT1.Destroy();
	BufferXYZWDT2.Destroy();
	QuadBufferXYZDT1.Destroy();
	QuadBufferXYZWDT1.Destroy();
	QuadBufferXYZWDT2.Destroy();
	BufferXYZOcclusion.Destroy();
/*
	for(int i=0;i<LibVB.size();i++)
	{
		sSlotVB& s=LibVB[i];
		VISASSERT(!s.init);
	}

	for(i=0;i<LibIB.size();i++)
	{
		sSlotIB& s=LibIB[i];
		VISASSERT(!s.init);
	}
*/	
	LibVB.clear();
	LibIB.clear();
	TexLibrary.Free();
	
	bActiveScene=0;
	RELEASE(lpD3DDevice);
	RELEASE(lpD3D);
	
	xScr=yScr=xScrMin=yScrMin=xScrMax=yScrMax=0;
	hWnd=0;	RenderMode=0;

	return 0;
}
int cD3DRender::GetClipRect(int *xmin,int *ymin,int *xmax,int *ymax)
{
	if(lpD3DDevice==0) return -1;
	D3DVIEWPORT9 vp;
	RDCALL(lpD3DDevice->GetViewport(&vp));
	*xmin=xScrMin=vp.X; *xmax=xScrMax=vp.X+vp.Width;
	*ymin=yScrMin=vp.Y; *ymax=yScrMax=vp.Y+vp.Height;
	return 0;
}
int cD3DRender::SetClipRect(int xmin,int ymin,int xmax,int ymax)
{
	if(lpD3DDevice==0) return -1;
	xScrMin=xmin;
	yScrMin=ymin;
	xScrMax=xmax;
	yScrMax=ymax;

	D3DVIEWPORT9 vp={xScrMin,yScrMin,xScrMax-xScrMin,yScrMax-yScrMin,0.0f,1.0f};
	RDCALL(lpD3DDevice->SetViewport(&vp));
	return 0;
}

int cD3DRender::Fill(int r,int g,int b,int a)
{ 
	if(bActiveScene) EndScene();
	if(lpD3DDevice==0) return -1;
	RestoreDeviceIfLost();

	D3DVIEWPORT9 vp={0,0,xScr,yScr,0.0f,1.0f};
	RDCALL(lpD3DDevice->SetViewport(&vp));
	RDCALL(lpD3DDevice->Clear(0,NULL,D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER|((RenderMode&RENDERDEVICE_MODE_STRENCIL)?D3DCLEAR_STENCIL:0),
		D3DCOLOR_RGBA(r,g,b,a),1,0));

	return 0;
}

void cD3DRender::RestoreDeviceIfLost()
{
	int hr;
	while(FAILED(hr=lpD3DDevice->TestCooperativeLevel()))
    { // Test the cooperative level to see if it's okay to render
        if( D3DERR_DEVICELOST == hr )
		{
			MSG msg;
			if(PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE)){
				if(!GetMessage(&msg, NULL, 0, 0))
					break;
				TranslateMessage( &msg );
				DispatchMessage( &msg );
			}else
				WaitMessage();
//			VISASSERT(0 && D3DERR_DEVICELOST);
//          return;
		}

        if(hr==D3DERR_DEVICENOTRESET)
		{
			MTTexObjAutoLock lock;
			KillFocus(); // free all, If the device was lost, do not render until we get it back
			SetFocus(false);//true
		}
    }
}

int cD3DRender::Flush(HWND wnd)
{ 
	if(bActiveScene) EndScene();
	MTG();
	RestoreDeviceIfLost();

	//RDCALL(lpD3DDevice->Present(NULL,NULL,wnd,NULL))
	lpD3DDevice->Present(NULL,NULL,wnd,NULL);

	if(Option_DrawNumberPolygon) 
	{
		char str[256];
		sprintf(str,"poly=%i, obj=%i",NumberPolygon,NumDrawObject);
		OutText(10,100,str);
		
		int dbg_MemVertexBuffer=0,dbg_NumVertexBuffer=0;

		for(int i=0; i<LibVB.size(); i++ )
			if( LibVB[i]->p )
			{
				D3DVERTEXBUFFER_DESC dsc;
				LibVB[i]->p->GetDesc( &dsc );
				dbg_MemVertexBuffer += dsc.Size;
				dbg_NumVertexBuffer++;
			}
		sprintf(str,"vb=%i,nvb=%i",dbg_MemVertexBuffer,dbg_NumVertexBuffer);
		OutText(10,80,str);

		int dbg_MemIndexBuffer=0,dbg_NumIndexBuffer=0;
		int i;
		for(i=0; i<LibIB.size(); i++ )
			if( LibIB[i]->p )
			{
				D3DINDEXBUFFER_DESC dsc;
				LibIB[i]->p->GetDesc( &dsc );
				dbg_MemIndexBuffer += dsc.Size;
				dbg_NumIndexBuffer++;
			}
		sprintf(str,"ib=%i,nib=%i",dbg_MemIndexBuffer,dbg_NumIndexBuffer);
		OutText(10,60,str);

		int dbg_MemTexture=0;
		for( i=0; i<TexLibrary.GetNumberTexture(); i++ )
		{
			cTexture* pTexture=TexLibrary.GetTexture(i);
			if(pTexture)
			for(int nFrame=0;nFrame<pTexture->GetNumberFrame();nFrame++)
			if( pTexture->GetDDSurface(nFrame)) 
			{
				D3DSURFACE_DESC dsc;
				pTexture->GetDDSurface(nFrame)->GetLevelDesc( 0, &dsc );
				int sz=GetTextureFormatSize(dsc.Format);
				int size=dsc.Width*dsc.Height*sz;
				dbg_MemTexture += (size/8);
			}
		}
		sprintf(str,"tex=%i",dbg_MemTexture);
		OutText(10,40,str);

		dbg_MemTexture=0;
		vector<cFontInternal*>::iterator it;
		FOR_EACH(gb_VisGeneric->fonts,it)
		{
			cTexture* pTexture=(*it)->GetTexture();
			if(pTexture)
			for(int nFrame=0;nFrame<pTexture->GetNumberFrame();nFrame++)
			if( pTexture->GetDDSurface(nFrame)) 
			{
				D3DSURFACE_DESC dsc;
				pTexture->GetDDSurface(nFrame)->GetLevelDesc( 0, &dsc );
				int sz=GetTextureFormatSize(dsc.Format);
				int size=dsc.Width*dsc.Height*sz;
				dbg_MemTexture += (size/8);
			}
		}

		sprintf(str,"font_mem=%i",dbg_MemTexture);
		OutText(10,180,str);

		int FreeTileMemoty,TotalTileMemory;
		vertex_pool_manager.GetUsedMemory(TotalTileMemory,FreeTileMemoty);
		
		if(TotalTileMemory)
			sprintf(str,"TileMemoty=%i byte, slack=%f%%",TotalTileMemory,FreeTileMemoty*100.0f/TotalTileMemory);
		else
			sprintf(str,"TileMemoty=0 byte, slack=0%%");
		OutText(10,120,str);

		{
			int total=0,free=0;
			GetTilemapTextureMemory(total,free);

			if(total)
				sprintf(str,"TileTexture=%i byte, slack=%f%%",total,free*100.0f/total);
			else
				sprintf(str,"TileTexture=0 byte, slack=0%%");
			OutText(10,140,str);
		}

		index_pool_manager.GetUsedMemory(TotalTileMemory,FreeTileMemoty);
		if(TotalTileMemory)
			sprintf(str,"TileIndex=%i byte, slack=%f%%",TotalTileMemory,FreeTileMemoty*100.0f/TotalTileMemory);
		else
			sprintf(str,"TileIndex=0 byte, slack=0%%");
		OutText(10,160,str);
/*
		extern void CalcNumParticleClass(int& num_object,int& num_particle);
		int num_particle_obj,num_particle;
		CalcNumParticleClass(num_particle_obj,num_particle);

		sprintf(str,"PObj=%i, Particles=%i",num_particle_obj,num_particle);
		OutText(10,120,str);
/**/
	}

	return 0;
}
int cD3DRender::BeginScene()
{ 
	if(lpD3DDevice==0) return -1;
	if(bActiveScene) return 1; bActiveScene=1;
	HRESULT hr=lpD3DDevice->BeginScene();
	
	NumberPolygon=0;
	NumDrawObject=0;
	CurrentIndexBuffer=NULL;
	CurrentVertexShader=NULL;;
	CurrentPixelShader=NULL;
	CurrentFVF=0;
	
    for(int i=0;i<RENDERSTATE_MAX;i++)
		ArrayRenderState[i]=0xEFEFEFEF;
    for(int j=0;j<TEXTURE_MAX;j++)
	{
		int i;
	    for(i=0;i<TEXTURESTATE_MAX;i++)
			ArrayTextureStageState[j][i]=0xEFEFEFEF;
		for(i=0;i<SAMPLERSTATE_MAX;i++)
			ArraytSamplerState[j][i]=0xEFEFEFEF;
	}

	for( int nPasses=0; nPasses<nSupportTexture; nPasses++ ) 
	{
		lpD3DDevice->SetTexture( nPasses, CurrentTexture[nPasses]=0 );
		SetTextureStageState( nPasses, D3DTSS_COLOROP,	 D3DTOP_DISABLE );
		SetTextureStageState( nPasses, D3DTSS_COLORARG1, D3DTA_TEXTURE );
		SetTextureStageState( nPasses, D3DTSS_COLORARG2, nPasses ? D3DTA_CURRENT : D3DTA_DIFFUSE );

		SetTextureStageState( nPasses, D3DTSS_ALPHAOP,	 D3DTOP_DISABLE );
		SetTextureStageState( nPasses, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
		SetTextureStageState( nPasses, D3DTSS_ALPHAARG2, nPasses ? D3DTA_CURRENT : D3DTA_DIFFUSE );

  		SetSamplerState( nPasses, D3DSAMP_MINFILTER, texture_interpolation );	// D3DTEXF_POINT
		SetSamplerState( nPasses, D3DSAMP_MAGFILTER, texture_interpolation );
		SetSamplerState( nPasses, D3DSAMP_MIPFILTER, texture_interpolation );	// trilinear

		SetTextureStageState( nPasses, D3DTSS_BUMPENVMAT00, F2DW(0.5f) );
		SetTextureStageState( nPasses, D3DTSS_BUMPENVMAT01, F2DW(0.0f) );
		SetTextureStageState( nPasses, D3DTSS_BUMPENVMAT10, F2DW(0.0f) );
		SetTextureStageState( nPasses, D3DTSS_BUMPENVMAT11, F2DW(0.5f) );
        SetTextureStageState( nPasses, D3DTSS_BUMPENVLSCALE, F2DW(4.0f) );
        SetTextureStageState( nPasses, D3DTSS_BUMPENVLOFFSET, F2DW(0.0f) );
		SetTextureStageState( nPasses, D3DTSS_TEXCOORDINDEX, nPasses<=1 ? nPasses : 1 );
	}

//	SetRenderState(D3DRS_TEXTUREPERSPECTIVE,TRUE);
//	SetRenderState(D3DRS_ANTIALIAS,D3DANTIALIAS_NONE);
	SetRenderState(D3DRS_ZENABLE,D3DZB_TRUE);
	SetRenderState(D3DRS_FILLMODE,bWireFrame==0?FILL_SOLID:FILL_WIREFRAME);
	SetRenderState(D3DRS_SHADEMODE,D3DSHADE_GOURAUD);
	SetRenderState(D3DRS_ZWRITEENABLE,TRUE);
	SetRenderState(D3DRS_ALPHATESTENABLE,FALSE);
	SetRenderState(D3DRS_LASTPIXEL,TRUE);
	SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCALPHA);
	SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);
	SetRenderState(D3DRS_CULLMODE,CurrentCullMode=D3DCULL_CW);
	SetRenderState(D3DRS_ZFUNC,D3DCMP_LESSEQUAL);
	SetRenderState(D3DRS_ALPHAREF,0);
	SetRenderState(D3DRS_ALPHAFUNC,D3DCMP_GREATER); //D3DCMP_ALWAYS

	bool hicolor=(RenderMode&RENDERDEVICE_MODE_RGB16)?true:false;
	SetRenderState(D3DRS_DITHERENABLE,hicolor);
	SetRenderState(D3DRS_ALPHABLENDENABLE,FALSE);
	SetRenderState(D3DRS_SPECULARENABLE,FALSE);
	SetRenderState(D3DRS_DEPTHBIAS,0);
	SetRenderState(D3DRS_TEXTUREFACTOR,0xFFFFFFFF);

	SetRenderState(D3DRS_CLIPPING,TRUE);
	SetRenderState(D3DRS_LIGHTING,FALSE);

	SetRenderState(D3DRS_AMBIENT,0xFFFFFFFF);
	SetRenderState(D3DRS_COLORVERTEX,FALSE);
	SetRenderState(D3DRS_LOCALVIEWER,FALSE);
	SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE,D3DMCS_COLOR1);
	SetRenderState(D3DRS_SPECULARMATERIALSOURCE,D3DMCS_MATERIAL);
	SetRenderState(D3DRS_AMBIENTMATERIALSOURCE,D3DMCS_MATERIAL);
	SetRenderState(D3DRS_EMISSIVEMATERIALSOURCE,D3DMCS_MATERIAL);
	SetRenderState(D3DRS_VERTEXBLEND,D3DVBF_DISABLE);
	SetRenderState(D3DRS_CLIPPLANEENABLE,FALSE);

	// FOG RenderState
	SetRenderState(D3DRS_FOGENABLE,FALSE);
	SetRenderState(D3DRS_RANGEFOGENABLE,FALSE);
	SetRenderState(D3DRS_FOGSTART,0);
	SetRenderState(D3DRS_FOGEND,0);
	SetRenderState(D3DRS_FOGDENSITY,0);
	if(bSupportTableFog && (dtAdvance && (dtAdvance->GetID()!=DT_GEFORCE3 && dtAdvance->GetID()!=DT_GEFORCEFX)))
		SetRenderState( D3DRS_FOGTABLEMODE,   D3DFOG_LINEAR );
	else 
	if(bSupportVertexFog)
	{
		SetRenderState( D3DRS_FOGVERTEXMODE,  D3DFOG_LINEAR );
	}else
		SetRenderState( D3DRS_FOGTABLEMODE,  D3DFOG_NONE ),
		SetRenderState( D3DRS_FOGVERTEXMODE,  D3DFOG_NONE );
	return hr;
}

int cD3DRender::EndScene()
{ 
	if(lpD3DDevice==0) return -1;
	if(!bActiveScene) return 1; 

//	D3DVIEWPORT9 vpall={0,0,xScr,yScr,0.0f,1.0f};
//	RDCALL(lpD3DDevice->SetViewport(&vpall));
	FlushPrimitive2D();
	FlushPrimitive3D();

	bActiveScene=0;
	HRESULT hr=lpD3DDevice->EndScene();
	return hr;
}

void cD3DRender::FlushPrimitive2D()
{
	FlushPixel();
	FlushLine();
	FlushFilledRect();
}

void cD3DRender::SetGlobalLight(Vect3f *vLight,sColor4f *Ambient,sColor4f *Diffuse,sColor4f *Specular)
{
	if(Ambient==0||Diffuse==0||Specular==0)	
	{
		RDCALL(lpD3DDevice->LightEnable(0,FALSE));
		return;
	}

	D3DLIGHT9 GlobalLight;
	memset(&GlobalLight,0,sizeof(GlobalLight));
	GlobalLight.Type = D3DLIGHT_DIRECTIONAL;
	if(vLight)
		memcpy(&GlobalLight.Direction.x,&vLight->x,sizeof(GlobalLight.Direction));
	else
		memcpy(&GlobalLight.Direction.x,&Vect3f(0,0,1),sizeof(GlobalLight.Direction));
	
	memcpy(&GlobalLight.Ambient.r,&Ambient->r,sizeof(GlobalLight.Ambient));
	memcpy(&GlobalLight.Diffuse.r,&Diffuse->r,sizeof(GlobalLight.Diffuse));
	memcpy(&GlobalLight.Specular.r,&Specular->r,sizeof(GlobalLight.Specular));
	
	GlobalLight.Range=1000.f;
	GlobalLight.Attenuation0=0.0f;
	GlobalLight.Attenuation1=1.0f;
	GlobalLight.Attenuation2=0.0f;

	lpD3DDevice->SetLight(0,&GlobalLight);
	lpD3DDevice->LightEnable(0,TRUE);
}
int cD3DRender::SetRenderState(eRenderStateOption option,int value)
{ 
    if(lpD3DDevice==0||!bActiveScene) return 1;
	switch(option)
	{
		case RS_FILLMODE:
			bWireFrame=(value==FILL_WIREFRAME);
			break;
		case RS_ZWRITEENABLE:
			if(value) SetRenderState(D3DRS_CULLMODE,CurrentCullMode);
			else SetRenderState(D3DRS_CULLMODE,D3DCULL_NONE);
			break;
		case RS_CULLMODE:
			if(value<0) value=CurrentCullMode;
			break;
		case RS_BILINEAR:
			if(value)
			{
				SetSamplerState(0, D3DSAMP_MINFILTER, texture_interpolation );
				SetSamplerState(0, D3DSAMP_MAGFILTER, texture_interpolation );
				SetSamplerState(0, D3DSAMP_MIPFILTER, texture_interpolation );
			}else
			{
				SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT );
				SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT );
				SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT );
			}
			return 0;
	}
	SetRenderState((D3DRENDERSTATETYPE)option,value);
	return 0; 
}

void cD3DRender::DrawLine(int x1,int y1,int x2,int y2,sColor4c color)
{ 
	VISASSERT(bActiveScene);
	if(x1<=x2) { if(x2<xScrMin||x1>xScrMax) return; }
	else if(x1<xScrMin||x2>xScrMax) return;
	if(y1<=y2) { if(y2<yScrMin||y1>yScrMax) return; }
	else if(y1<yScrMin||y2>yScrMax) return;

	PointStruct v;
	v.x=(float)x1;
	v.y=(float)y1;
	v.diffuse=color;
	lines.push_back(v);
	v.x=(float)x2;
	v.y=(float)y2;
	lines.push_back(v);
}

void cD3DRender::DrawRectangle(int x1,int y1,int dx,int dy,sColor4c color,bool outline)
{ 
   	VISASSERT(bActiveScene);
	int x2=x1+dx,y2=y1+dy;
	if(dx>=0) { if(x2<xScrMin||x1>xScrMax) return; }
	else if(x1<xScrMin||x2>xScrMax) return;
	if(dy>=0) { if(y2<yScrMin||y1>yScrMax) return; }
	else if(y1<yScrMin||y2>yScrMax) return;

	if(outline)
	{
		PointStruct v;
		v.diffuse=color;
		v.x=(float)x1; v.y=(float)y1; lines.push_back(v);
		v.x=(float)x2; v.y=(float)y1; lines.push_back(v);

		v.x=(float)x2; v.y=(float)y1; lines.push_back(v);
		v.x=(float)x2; v.y=(float)y2; lines.push_back(v);

		v.x=(float)x2; v.y=(float)y2; lines.push_back(v);
		v.x=(float)x1; v.y=(float)y2; lines.push_back(v);

		v.x=(float)x1; v.y=(float)y2; lines.push_back(v);
		v.x=(float)x1; v.y=(float)y1; lines.push_back(v);
	}else
	{
		RectStruct s;
		s.x1=x1;s.x2=x2;
		s.y1=y1;s.y2=y2;
		s.diffuse=color;
		rectangles.push_back(s);
	}
}

void cD3DRender::FlushPrimitive3D()
{
	VISASSERT(bActiveScene);
	FlushLine3D();
	FlushPoint3D();
}


void cD3DRender::FlushLine3D()
{
	if(lines3d.empty())return;

	DWORD prev_zfunc=GetRenderState(D3DRS_ZFUNC);
	DWORD prev_fog=GetRenderState(D3DRS_FOGENABLE);
	SetRenderState(D3DRS_ZFUNC,D3DCMP_ALWAYS);
	SetRenderState(D3DRS_FOGENABLE,FALSE);
	SetNoMaterial(ALPHA_BLEND);
	SetMatrix(D3DTS_WORLD,MatXf::ID);


	VISASSERT((lines3d.size()&1)==0);
	int npoint=0;
	sVertexXYZD* v=BufferXYZD.Lock();
	vector<sVertexXYZD>::iterator it;
	FOR_EACH(lines3d,it)
	{
		v[npoint]=*it;
		npoint++;
		if(((npoint&1)==0) && npoint>=BufferXYZD.GetSize()-2)
		{
			BufferXYZD.Unlock(npoint);
			BufferXYZD.DrawPrimitive(PT_LINELIST,npoint/2);
			v=BufferXYZD.Lock();
			npoint=0;
		}
	}

	BufferXYZD.Unlock(npoint);
	if(npoint)
		BufferXYZD.DrawPrimitive(PT_LINELIST,npoint/2);

	lines3d.clear();
	SetRenderState(D3DRS_ZFUNC,prev_zfunc);
	SetRenderState(D3DRS_FOGENABLE,prev_fog);
}

void cD3DRender::FlushPoint3D()
{
	if(points3d.empty())return;

	DWORD prev_zfunc=GetRenderState(D3DRS_ZFUNC);
	DWORD prev_fog=GetRenderState(D3DRS_FOGENABLE);
	SetRenderState(D3DRS_ZFUNC,D3DCMP_ALWAYS);
	SetRenderState(D3DRS_FOGENABLE,FALSE);
	SetNoMaterial(ALPHA_BLEND);
	SetMatrix(D3DTS_WORLD,MatXf::ID);


	int npoint=0;
	sVertexXYZD* v=BufferXYZD.Lock();
	vector<sVertexXYZD>::iterator it;
	FOR_EACH(points3d,it)
	{
		v[npoint]=*it;
		npoint++;
		if(npoint>=BufferXYZD.GetSize()-2)
		{
			BufferXYZD.Unlock(npoint);
			BufferXYZD.DrawPrimitive(PT_POINTLIST,npoint);
			v=BufferXYZD.Lock();
			npoint=0;
		}
	}

	BufferXYZD.Unlock(npoint);
	if(npoint)
		BufferXYZD.DrawPrimitive(PT_POINTLIST,npoint);

	points3d.clear();
	SetRenderState(D3DRS_ZFUNC,prev_zfunc);
	SetRenderState(D3DRS_FOGENABLE,prev_fog);
}

void cD3DRender::DrawLine(const Vect3f &v1,const Vect3f &v2,sColor4c color)
{
	VISASSERT(bActiveScene);
	sVertexXYZD v;
	v.pos=v1;
	v.diffuse=color;
	lines3d.push_back(v);
	v.pos=v2;
	lines3d.push_back(v);
}

void cD3DRender::DrawPoint(const Vect3f &v1,sColor4c color)
{
	VISASSERT(bActiveScene);
	sVertexXYZD v;
	v.pos=v1;
	v.diffuse=color;
	points3d.push_back(v);
}

void cD3DRender::DrawPixel(int x1,int y1,sColor4c color)
{ 
    if(!bActiveScene) return;
	if(x1<xScrMin||x1>xScrMax||y1<yScrMin||y1>yScrMax) return;
	PointStruct v;
	v.x=(float)x1;
	v.y=(float)y1;
	v.diffuse=color;
	points.push_back(v);
	return; 
}

void cD3DRender::FlushPixel()
{
	if(points.empty())return;
	SetNoMaterial(ALPHA_BLEND);
	SetRenderState(D3DRS_ZWRITEENABLE, FALSE); 
	SetRenderState(D3DRS_ZFUNC,D3DCMP_ALWAYS);
	SetRenderState(D3DRS_ALPHAFUNC,D3DCMP_ALWAYS);
	SetRenderState(D3DRS_ALPHATESTENABLE,FALSE);

	int npoint=0;
	sVertexXYZWD* v=BufferXYZWD.Lock();
	vector<PointStruct>::iterator it;
	FOR_EACH(points,it)
	{
		PointStruct& p=*it;
		sVertexXYZWD& cv=v[npoint];
		cv.x=p.x;
		cv.y=p.y;
		cv.z=0.001f;
		cv.w=0.001f;
		cv.diffuse=p.diffuse;

		npoint++;
		if(npoint>=BufferXYZWD.GetSize())
		{
			BufferXYZWD.Unlock(npoint);
			BufferXYZWD.DrawPrimitive(PT_POINTLIST,npoint);
			v=BufferXYZWD.Lock();
			npoint=0;
		}
	}

	BufferXYZWD.Unlock(npoint);
	if(npoint)
		BufferXYZWD.DrawPrimitive(PT_POINTLIST,npoint);

	points.clear();
}

void cD3DRender::FlushLine()
{
	if(lines.empty())return;
	SetNoMaterial(ALPHA_BLEND);

	VISASSERT((lines.size()&1)==0);
	int npoint=0;
	sVertexXYZWD* v=BufferXYZWD.Lock();
	vector<PointStruct>::iterator it;
	FOR_EACH(lines,it)
	{
		PointStruct& p=*it;
		sVertexXYZWD& cv=v[npoint];
		cv.x=p.x;
		cv.y=p.y;
		cv.z=0.001f;
		cv.w=0.001f;
		cv.diffuse=p.diffuse;

		npoint++;
		if(((npoint&1)==0) && npoint>=BufferXYZWD.GetSize()-2)
		{
			BufferXYZWD.Unlock(npoint);
			BufferXYZWD.DrawPrimitive(PT_LINELIST,npoint/2);
			v=BufferXYZWD.Lock();
			npoint=0;
		}
	}

	BufferXYZWD.Unlock(npoint);
	if(npoint)
		BufferXYZWD.DrawPrimitive(PT_LINELIST,npoint/2);

	lines.clear();
}

void cD3DRender::FlushFilledRect()
{
	if(rectangles.empty())return;
	SetNoMaterial(ALPHA_BLEND);

	int npoint=0;
	sVertexXYZWD* v=BufferXYZWD.Lock();
	vector<RectStruct>::iterator it;
	FOR_EACH(rectangles,it)
	{
		RectStruct& p=*it;

		sVertexXYZWD* pv=v+npoint;
		pv[0].x=p.x1; pv[0].y=p.y1; pv[0].z=0.001f; pv[0].w=0.001f; pv[0].diffuse=p.diffuse;
		pv[1].x=p.x1; pv[1].y=p.y2; pv[1].z=0.001f; pv[1].w=0.001f; pv[1].diffuse=p.diffuse;
		pv[2].x=p.x2; pv[2].y=p.y1; pv[2].z=0.001f; pv[2].w=0.001f; pv[2].diffuse=p.diffuse;

		pv[3].x=p.x2; pv[3].y=p.y1; pv[3].z=0.001f; pv[3].w=0.001f; pv[3].diffuse=p.diffuse;
		pv[4].x=p.x1; pv[4].y=p.y2; pv[4].z=0.001f; pv[4].w=0.001f; pv[4].diffuse=p.diffuse;
		pv[5].x=p.x2; pv[5].y=p.y2; pv[5].z=0.001f; pv[5].w=0.001f; pv[5].diffuse=p.diffuse;

		npoint+=6;
		if(npoint>=BufferXYZWD.GetSize()-6)
		{
			BufferXYZWD.Unlock(npoint);
			BufferXYZWD.DrawPrimitive(PT_TRIANGLELIST,npoint/3);
			v=BufferXYZWD.Lock();
			npoint=0;
		}
	}

	BufferXYZWD.Unlock(npoint);
	if(npoint)
		BufferXYZWD.DrawPrimitive(PT_TRIANGLELIST,npoint/3);

	rectangles.clear();
}

void cD3DRender::DrawSprite(int x1,int y1,int dx,int dy,float u1,float v1,float du,float dv,
		cTexture *Texture,sColor4c &ColorMul,float phase,eBlendMode mode)
{
    VISASSERT(bActiveScene);
	int x2=x1+dx,y2=y1+dy;
	if(dx>=0) { if(x2<xScrMin||x1>xScrMax) return; }
	else if(x1<xScrMin||x2>xScrMax) return;
	if(dy>=0) { if(y2<yScrMin||y1>yScrMax) return; }
	else if(y1<yScrMin||y2>yScrMax) return;

	bool alpha=ColorMul.a<255 || Texture->IsAlpha();
	if(mode<=ALPHA_TEST && alpha)
		mode=ALPHA_BLEND;
	SetNoMaterial(mode,phase,Texture);

	sVertexXYZWDT1* v=BufferXYZWDT1.Lock(4);
	v[0].z=v[1].z=v[2].z=v[3].z=0.001f;
	v[0].w=v[1].w=v[2].w=v[3].w=0.001f;
	v[0].diffuse=v[1].diffuse=v[2].diffuse=v[3].diffuse=ColorMul;
	v[0].x=v[1].x=-0.5f+(float)x1; v[0].y=v[2].y=-0.5f+(float)y1; 
	v[3].x=v[2].x=-0.5f+(float)x2; v[1].y=v[3].y=-0.5f+(float)y2; 
	v[0].u1()=u1;    v[0].v1()=v1;
	v[1].u1()=u1;    v[1].v1()=v1+dv;
	v[2].u1()=u1+du; v[2].v1()=v1;
	v[3].u1()=u1+du; v[3].v1()=v1+dv;
	BufferXYZWDT1.Unlock(4);

	BufferXYZWDT1.DrawPrimitive(PT_TRIANGLESTRIP,2);
}

void cD3DRender::DrawSprite2(int x,int y,int dx,int dy,float u,float v,float du,float dv,
		cTexture *Tex1,cTexture *Tex2,sColor4c &ColorMul,float phase)
{
	DrawSprite2(x,y,dx,dy,u,v,du,dv,u,v,du,dv,
		Tex1,Tex2,ColorMul,phase);
}

void cD3DRender::DrawSprite2(int x1,int y1,int dx,int dy,
							 float u0,float v0,float du0,float dv0,
							 float u1,float v1,float du1,float dv1,
		cTexture *Tex1,cTexture *Tex2,sColor4c &ColorMul,float phase,
		eColorMode mode,eBlendMode blend_mode)
{
    VISASSERT(bActiveScene);
	int x2=x1+dx,y2=y1+dy;
	if(dx>=0) { if(x2<xScrMin||x1>xScrMax) return; }
	else if(x1<xScrMin||x2>xScrMax) return;
	if(dy>=0) { if(y2<yScrMin||y1>yScrMax) return; }
	else if(y1<yScrMin||y2>yScrMax) return;

	bool alpha=ColorMul.a<255;
	if(blend_mode==ALPHA_NONE)
		SetNoMaterial(alpha?ALPHA_BLEND:ALPHA_NONE,phase,Tex1,Tex2,mode);
	else
		SetNoMaterial(blend_mode,phase,Tex1,Tex2,mode);

	sVertexXYZWDT2* v=BufferXYZWDT2.Lock(4);
	v[0].z=v[1].z=v[2].z=v[3].z=0.001f;
	v[0].w=v[1].w=v[2].w=v[3].w=0.001f;
	v[0].diffuse=v[1].diffuse=v[2].diffuse=v[3].diffuse=ColorMul;
	v[0].x=v[1].x=-0.5f+(float)x1; v[0].y=v[2].y=-0.5f+(float)y1; 
	v[3].x=v[2].x=-0.5f+(float)x2; v[1].y=v[3].y=-0.5f+(float)y2; 

	v[0].u1()=u0;     v[0].v1()=v0;
	v[1].u1()=u0;     v[1].v1()=v0+dv0;
	v[2].u1()=u0+du0; v[2].v1()=v0;
	v[3].u1()=u0+du0; v[3].v1()=v0+dv0;

	v[0].u2()=u1;     v[0].v2()=v1;
	v[1].u2()=u1;     v[1].v2()=v1+dv1;
	v[2].u2()=u1+du1; v[2].v2()=v1;
	v[3].u2()=u1+du1; v[3].v2()=v1+dv1;
	BufferXYZWDT2.Unlock(4);
	
	BufferXYZWDT2.DrawPrimitive(PT_TRIANGLESTRIP,2);
}

void cD3DRender::DrawSprite2(int x1,int y1,int dx,int dy,
							 float u0,float v0,float du0,float dv0,
							 float u1,float v1,float du1,float dv1,
		cTexture *Tex1,cTexture *Tex2,float lerp_factor,float alpha,float phase,
		eColorMode mode,eBlendMode blend_mode)
{
    VISASSERT(bActiveScene);
	int x2=x1+dx,y2=y1+dy;
	if(dx>=0) { if(x2<xScrMin||x1>xScrMax) return; }
	else if(x1<xScrMin||x2>xScrMax) return;
	if(dy>=0) { if(y2<yScrMin||y1>yScrMax) return; }
	else if(y1<yScrMin||y2>yScrMax) return;

	sColor4c ColorMul(sColor4f(1,1,1,alpha));
	if(blend_mode==ALPHA_NONE)
		SetNoMaterial((ColorMul.a<255)?ALPHA_BLEND:ALPHA_NONE,phase,Tex2,Tex1,mode);
	else
		SetNoMaterial(blend_mode,phase,Tex2,Tex1,mode);

	DWORD index1=GetTextureStageState(1,D3DTSS_TEXCOORDINDEX);
	SetTextureStageState(0,D3DTSS_TEXCOORDINDEX,0);
	SetTextureStageState(1,D3DTSS_TEXCOORDINDEX,1);

	sColor4c lerp(255*lerp_factor,255*lerp_factor,255*lerp_factor,255*(1-lerp_factor));

	{
		SetTextureStageState(0,D3DTSS_COLOROP,D3DTOP_MODULATECOLOR_ADDALPHA);
		SetTextureStageState(0,D3DTSS_COLORARG1,D3DTA_TFACTOR);
		SetTextureStageState(0,D3DTSS_COLORARG2,D3DTA_TEXTURE);
		SetRenderState(D3DRS_TEXTUREFACTOR,lerp.RGBA());

		SetTextureStageState(1,D3DTSS_ALPHAOP,D3DTOP_MODULATE);
		SetTextureStageState(1,D3DTSS_ALPHAARG1,D3DTA_TEXTURE);
		SetTextureStageState(1,D3DTSS_ALPHAARG2,D3DTA_DIFFUSE);

//		SetTextureStageState(1,D3DTSS_COLOROP,D3DTOP_MODULATE);
		SetTextureStageState(1,D3DTSS_COLORARG1,D3DTA_TEXTURE);
		SetTextureStageState(1,D3DTSS_COLORARG2,D3DTA_CURRENT);
	}

	sVertexXYZWDT2* v=BufferXYZWDT2.Lock(4);
	v[0].z=v[1].z=v[2].z=v[3].z=0.001f;
	v[0].w=v[1].w=v[2].w=v[3].w=0.001f;
	v[0].diffuse=v[1].diffuse=v[2].diffuse=v[3].diffuse=ColorMul;
	v[0].x=v[1].x=-0.5f+(float)x1; v[0].y=v[2].y=-0.5f+(float)y1; 
	v[3].x=v[2].x=-0.5f+(float)x2; v[1].y=v[3].y=-0.5f+(float)y2; 

	v[0].u1()=u1;     v[0].v1()=v1;
	v[1].u1()=u1;     v[1].v1()=v1+dv1;
	v[2].u1()=u1+du1; v[2].v1()=v1;
	v[3].u1()=u1+du1; v[3].v1()=v1+dv1;

	v[0].u2()=u0;     v[0].v2()=v0;
	v[1].u2()=u0;     v[1].v2()=v0+dv0;
	v[2].u2()=u0+du0; v[2].v2()=v0;
	v[3].u2()=u0+du0; v[3].v2()=v0+dv0;
	BufferXYZWDT2.Unlock(4);
	
	BufferXYZWDT2.DrawPrimitive(PT_TRIANGLESTRIP,2);

	SetTextureStageState(0,D3DTSS_TEXCOORDINDEX,0);
	SetTextureStageState(1,D3DTSS_TEXCOORDINDEX,index1);

	SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE);
	SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);

	SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
	SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
	SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );

	SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );
	SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	SetTextureStageState( 1, D3DTSS_COLORARG2, D3DTA_CURRENT );

	SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
	SetTextureStageState( 1, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
	SetTextureStageState( 1, D3DTSS_ALPHAARG2, D3DTA_CURRENT );

}


void cD3DRender::OutText(int x,int y,const char *string,int r,int g,int b,char *FontName,int size,int bold,int italic,int underline)
{
	HDC hDC=0;
    HFONT hFont=CreateFont(size,0,0,0,bold?FW_BOLD:FW_NORMAL,italic,underline,0, ANSI_CHARSET,
			OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,VARIABLE_PITCH,FontName);
	if(hFont==0) return;
	if(hWnd==0) return;

	if((hDC=GetDC(hWnd))==0) return;
    HFONT hFontOld=(HFONT)SelectObject( hDC, hFont );
    SetTextColor(hDC,RGB(r,g,b));
	SetBkMode(hDC,TRANSPARENT);
	RECT rect={xScrMin,yScrMin,xScrMax,yScrMax};
	RDCALL(!ExtTextOut(hDC,x,y,ETO_CLIPPED,&rect,string,lstrlen(string),0));
	SelectObject(hDC,hFontOld);
	DeleteObject(hFont);
	ReleaseDC(hWnd,hDC);

}

void cD3DRender::OutText(int x,int y,const char *string,int r,int g,int b)
{
	if(hWnd==0) return;
	HDC hDC=GetDC(hWnd);
	if(hDC==0) return;
	SetTextColor(hDC,RGB(r,g,b));
	SetBkMode(hDC,TRANSPARENT);
	RECT rect={xScrMin,yScrMin,xScrMax,yScrMax};
	RDCALL(!ExtTextOut(hDC,x,y,ETO_CLIPPED,&rect,string,lstrlen(string),0));
	ReleaseDC(hWnd,hDC);
}

int cD3DRender::SetGamma(float fGamma,float fStart,float fFinish)
{
	D3DGAMMARAMP gamma;
	if(fGamma<=0) fGamma=1.f; else fGamma=1/fGamma;
    for(int i=0;i<256;i++)
    {
		int dwGamma=round(65536*(fStart+(fFinish-fStart)*pow(i/255.f,fGamma)));
		if(dwGamma<0) dwGamma=0; else if(dwGamma>65535) dwGamma=65535;
        gamma.red[i]=gamma.green[i]=gamma.blue[i]=dwGamma;
    }
	lpD3DDevice->SetGammaRamp(0,D3DSGR_NO_CALIBRATION,&gamma);
	return 0;
}

void cD3DRender::CreateVertexBuffer(sPtrVertexBuffer &vb,int NumberVertex,int format,int dynamic)
{
	int size = GetSizeFromFmt(format);
	CreateVertexBufferBySizeFormat(vb,NumberVertex,size,format,dynamic);
}

void cD3DRender::CreateVertexBufferBySize(struct sPtrVertexBuffer &vb,int NumberVertex,int size,int dynamic)
{
	CreateVertexBufferBySizeFormat(vb,NumberVertex,size,0,dynamic);
}

void cD3DRender::CreateVertexBufferBySizeFormat(struct sPtrVertexBuffer &vb,int NumberVertex,int size,int format,int dynamic)
{
	xassert(NumberVertex>=0 || NumberVertex<=65536);
	vb.ptr=LibVB.NewSlot();
	vb.size = size;

	sSlotVB& s=*vb.ptr;
	s.VertexSize=size;
	s.fmt=format;
	s.dynamic = dynamic;
	s.NumberVertex=NumberVertex;
	s.p=NULL;
	if(dynamic)
		RDCALL(lpD3DDevice->CreateVertexBuffer(NumberVertex*vb.size,D3DUSAGE_DYNAMIC|D3DUSAGE_WRITEONLY,format,D3DPOOL_DEFAULT,&s.p,NULL))
	else
		RDCALL(lpD3DDevice->CreateVertexBuffer(NumberVertex*vb.size,D3DUSAGE_WRITEONLY,format,D3DPOOL_MANAGED,&s.p,NULL))
}

void cD3DRender::DeleteVertexBuffer(sPtrVertexBuffer &vb)
{
	MTG();
	if(!vb.IsInit()) return;
	sSlotVB& s=*vb.ptr;
	VISASSERT(s.init);
	RELEASE(s.p); 
	LibVB.DeleteSlot(vb.ptr);
	vb.ptr=NULL;
}

void cD3DRender::DeleteDynamicVertexBuffer()
{
	for(int i=0;i<LibVB.size();i++)
	{
		sSlotVB& s=*LibVB[i];
		if(s.init && s.dynamic)
		{
			RELEASE(s.p);
		}
	}
}

void cD3DRender::RestoreDynamicVertexBuffer()
{
	for(int i=0;i<LibVB.size();i++)
	{
		sSlotVB& s=*LibVB[i];
		if(s.init && s.dynamic)
		{
			VISASSERT(s.p==NULL);
			RDCALL(lpD3DDevice->CreateVertexBuffer(s.NumberVertex*s.VertexSize,D3DUSAGE_DYNAMIC|D3DUSAGE_WRITEONLY,s.fmt,D3DPOOL_DEFAULT,&s.p,NULL))
		}
	}
}

void* cD3DRender::LockVertexBuffer(sPtrVertexBuffer &vb)
{
	void *p=0;
	VISASSERT( vb.IsInit() && vb.ptr->p );
	RDCALL(vb.ptr->p->Lock(0,0,&p, vb.ptr->dynamic ? D3DLOCK_NOSYSLOCK|D3DLOCK_DISCARD : 0 ));
	return p;
}
void cD3DRender::UnlockVertexBuffer(sPtrVertexBuffer &vb)
{
	VISASSERT( vb.IsInit() && vb.ptr->p );
	RDCALL(vb.ptr->p->Unlock());
}
void cD3DRender::CreateIndexBuffer(sPtrIndexBuffer& ib,int NumberPolygon,int size)
{
	DeleteIndexBuffer(ib);

	sSlotIB* slot=LibIB.NewSlot();
	RDCALL(lpD3DDevice->CreateIndexBuffer(NumberPolygon*size,D3DUSAGE_WRITEONLY,D3DFMT_INDEX16,D3DPOOL_MANAGED,&slot->p,NULL));
	ib.ptr=slot;
}
void cD3DRender::DeleteIndexBuffer(sPtrIndexBuffer &ib)
{
	if(!ib.IsInit()) return;
	sSlotIB& s=*ib.ptr;
	VISASSERT(s.init);
	RELEASE(s.p);
	LibIB.DeleteSlot(ib.ptr);
	ib.ptr=NULL;
}
sPolygon* cD3DRender::LockIndexBuffer(sPtrIndexBuffer &ib)
{
	void *p=0;
	VISASSERT( ib.IsInit() && ib.ptr->p );
	ib.ptr->p->Lock(0,0,&p,0);
	return (sPolygon*)p;
}
void cD3DRender::UnlockIndexBuffer(sPtrIndexBuffer &ib)
{
	VISASSERT( ib.IsInit() && ib.ptr->p );
	ib.ptr->p->Unlock();
}

void cD3DRender::SetGlobalFog(const sColor4f &color,const Vect2f &v)
{ 
	if(v.x<0 || v.y<0)
	{
		SetRenderState(RS_FOGENABLE,FALSE);
		return;
	}

	SetRenderState(RS_FOGENABLE,TRUE);
	SetRenderState(RS_FOGCOLOR,color.GetRGB());
	SetRenderState(RS_FOGSTART,F2DW(v.x));
	SetRenderState(RS_FOGEND,F2DW(v.y));

	dtAdvance->SetFog(color,v);
}
int cD3DRender::KillFocus()
{
	if(lpD3DDevice==0) return 1;

	{
		vector<cOcclusionQuery*>::iterator it;
		FOR_EACH(occlusion_query,it)
			(*it)->Done();
	}

	RELEASE(lpZBuffer);
	RELEASE(lpBackBuffer);
	dtFixed->DeleteShadowTexture();
	dtAdvance->DeleteShadowTexture();
	if(dtAdvanceOriginal)
		dtAdvanceOriginal->DeleteShadowTexture();

	for(int i=0;i<TexLibrary.GetNumberTexture();i++)
	{
		cTexture* pTexture=TexLibrary.GetTexture(i);
		if(pTexture&&pTexture->GetAttribute(TEXTURE_D3DPOOL_DEFAULT))
			DeleteTexture(pTexture);
	}
//	IsDeleteAllDefaultTextures();

	DeleteDynamicVertexBuffer();
	DeleteShader();
		
	ClearTilemapPool();
	vertex_pool_manager.Clear();
	index_pool_manager.Clear();
	return 0;
}

bool cD3DRender::SetFocus(bool wait,bool focus_error)
{
	{
		HRESULT hres=D3D_OK;
		int imax=wait?10:1;
		for(int i=0;i<imax;i++)
		{
			hres=lpD3DDevice->Reset(&d3dpp);
			if(hres!=D3D_OK && wait)
				Sleep(1000);
		}
		if(hres!=D3D_OK)
		{
			if(focus_error)
			{
				VisError<<"Internal error. Cannot reinitialize graphics"<<VERR_END;
			}
			return false;
		}
	}

	RDCALL(lpD3DDevice->EvictManagedResources());
	 
	for(int i=0;i<TexLibrary.GetNumberTexture();i++)
	{
		cTexture* pTexture=TexLibrary.GetTexture(i);
		if(pTexture && pTexture->GetAttribute(TEXTURE_D3DPOOL_DEFAULT))
		{
			if(pTexture->GetRef()>1)
				CreateTexture(pTexture,0,-1,-1);
		}
	}

	RELEASE(lpZBuffer);
	RELEASE(lpBackBuffer);

	RDCALL(lpD3DDevice->GetDepthStencilSurface(&lpZBuffer));
	RDCALL(lpD3DDevice->GetRenderTarget(0,&lpBackBuffer)); 

	RestoreShader();
	RestoreDynamicVertexBuffer();
	RestoreTilemapPool();

	ReinitOcclusion();
	return true;
}

void cD3DRender::DeleteShader()
{
	vector<cShader*>::iterator it;
	FOR_EACH(cShader::all_shader,it)
		(*it)->Delete();
}

void cD3DRender::RestoreShader()
{
	vector<cShader*>::iterator it;
	FOR_EACH(cShader::all_shader,it)
		(*it)->Restore();
}

void cD3DRender::SetBlendState(eBlendMode blend)
{
	SetRenderState(D3DRS_ALPHATESTENABLE,blend!=ALPHA_NONE);
	SetRenderState(D3DRS_ALPHABLENDENABLE,blend>ALPHA_TEST);

	switch(blend)
	{
	case ALPHA_SUBBLEND:
		SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCALPHA);
		SetRenderState(D3DRS_DESTBLEND,D3DBLEND_ONE);
		SetRenderState(D3DRS_BLENDOP,D3DBLENDOP_REVSUBTRACT);
		break;
	case ALPHA_ADDBLENDALPHA:
		SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCALPHA);
		SetRenderState(D3DRS_DESTBLEND,D3DBLEND_ONE);
		SetRenderState(D3DRS_BLENDOP,D3DBLENDOP_ADD);
		break;
	case ALPHA_ADDBLEND:
		SetRenderState(D3DRS_SRCBLEND,D3DBLEND_ONE);
		SetRenderState(D3DRS_DESTBLEND,D3DBLEND_ONE);
		SetRenderState(D3DRS_BLENDOP,D3DBLENDOP_ADD);
		break;
	case ALPHA_BLEND:
		SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCALPHA);
		SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);
		SetRenderState(D3DRS_BLENDOP,D3DBLENDOP_ADD);
		break;
	case ALPHA_MUL:
		SetRenderState(D3DRS_SRCBLEND,D3DBLEND_DESTCOLOR);
		SetRenderState(D3DRS_DESTBLEND,D3DBLEND_ZERO);
		SetRenderState(D3DRS_BLENDOP,D3DBLENDOP_ADD);
		break;
	}
}

void cD3DRender::SetNoMaterial(eBlendMode blend,float Phase,cTexture *Texture0,
							   cTexture *Texture1,eColorMode color_mode)
{
	SetTexture(Texture0,Phase,0);
	SetTexture(Texture1,Phase,1);
	SetTexture(NULL,Phase,2);

	if(Texture0)
	{
		if(blend==ALPHA_NONE && Texture0->IsAlphaTest())
			blend=ALPHA_TEST;
		if(!(blend>ALPHA_TEST) && Texture0->IsAlpha())
			blend=ALPHA_BLEND;
	}
	SetBlendState(blend);
	
	SetRenderState(D3DRS_SPECULARENABLE,FALSE);
	SetRenderState(D3DRS_LIGHTING,FALSE);
	SetRenderState(D3DRS_NORMALIZENORMALS,FALSE);


	SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE );
	SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 0 );

	SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_MODULATE );

	if(blend>ALPHA_NONE)
		SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_MODULATE );
	else
		SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_DISABLE );
	if(Texture1)
	{
		SetTextureStageState( 1, D3DTSS_TEXCOORDINDEX, 1 ),
		SetTextureStageState( 1, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE );

		switch(color_mode)
		{
		case COLOR_ADD:
			SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_ADD);
			break;
		case COLOR_MOD2:
			SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_MODULATE2X );
			break;
		case COLOR_MOD4:
			SetTextureStageState( 1, D3DTSS_COLOROP, CurrentMod4 );
			break;
		case COLOR_MOD:
			SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_MODULATE );
			break;
		default:
			VISASSERT(0);
			break;
		}
	}
	else 
		SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_DISABLE );

	SetTextureStageState( 2, D3DTSS_COLOROP, D3DTOP_DISABLE );
}

void cD3DRender::InitVertexBuffers()
{
	const int size=32768;
	const int sizemin=8192;
	BufferXYZDT1.Create(size);
	BufferXYZDT2.Create(sizemin);
	BufferXYZWD.Create(sizemin);
	BufferXYZD.Create(sizemin);
	QuadBufferXYZDT1.Create();
	QuadBufferXYZWDT1.Create();
	QuadBufferXYZWDT2.Create();

	BufferXYZWDT1.Create(sizemin);
	BufferXYZWDT2.Create(sizemin);
	BufferXYZOcclusion.Create(sizemin);
}
////////////////////////////////////////////////////////////

void sPtrVertexBuffer::Destroy()
{
	if(ptr)
		gb_RenderDevice->DeleteVertexBuffer(*this);
	ptr=NULL;
}

sPtrIndexBuffer::~sPtrIndexBuffer()
{
	if(ptr)
		gb_RenderDevice->DeleteIndexBuffer(*this);
	ptr=NULL;
}

////////////////////////////////////////////////////////////
cVertexBufferInternal::cVertexBufferInternal()
{
	MTG();
	numvertex=0;
	cur_min_vertex=0;
	start_vertex=0;
}

void cVertexBufferInternal::Destroy()
{
	MTG();
	vb.Destroy();
}

cVertexBufferInternal::~cVertexBufferInternal()
{
}

BYTE* cVertexBufferInternal::Lock(int minvertex)
{
	MTG();
	void* min_index=NULL;

	LPDIRECT3DVERTEXBUFFER9 pvb=gb_RenderDevice3D->GetVB(vb);
	if(GetSize()>minvertex)
	{
		RDCALL(pvb->Lock(cur_min_vertex*vb.size,GetSize()*vb.size,
			&min_index,D3DLOCK_NOOVERWRITE));
	}else
	{
		RDCALL(pvb->Lock(0,0,&min_index,D3DLOCK_DISCARD));
		cur_min_vertex=0;
	}

	return (BYTE*)min_index;
}

void cVertexBufferInternal::Unlock(int num_write_vertex)
{
	MTG();
	cD3DRender* rd=gb_RenderDevice3D;
	RDCALL(rd->GetVB(vb)->Unlock());

	start_vertex=cur_min_vertex;
	cur_min_vertex+=num_write_vertex;
	VISASSERT(cur_min_vertex<=numvertex);
}

void cVertexBufferInternal::Create(int bytesize,int vertexsize,int _fmt)
{
	MTG();
	numvertex=bytesize/vertexsize;
	fmt=_fmt;
	gb_RenderDevice3D->CreateVertexBuffer(vb,numvertex,fmt,TRUE);
	xassert(vertexsize==vb.size);
}

void cVertexBufferInternal::DrawPrimitive(PRIMITIVETYPE Type,UINT Count,const MatXf &m)
{
	MTG();
	gb_RenderDevice3D->SetMatrix(D3DTS_WORLD,m);
	DrawPrimitive(Type,Count);
}

void cVertexBufferInternal::DrawPrimitive(PRIMITIVETYPE Type,UINT Count)
{
	MTG();
	cD3DRender* rd=gb_RenderDevice3D;
	rd->SetFVF(fmt);
	rd->SetStreamSource(vb);
	RDCALL(rd->lpD3DDevice->DrawPrimitive((D3DPRIMITIVETYPE)Type,start_vertex,Count));
}

void cVertexBufferInternal::DrawIndexedPrimitive(UINT Count)
{
	MTG();
	cD3DRender* rd=gb_RenderDevice3D;
	rd->SetFVF(fmt);
	rd->SetStreamSource(vb);
	rd->SetIndices(rd->GetStandartIB());
	RDCALL(rd->lpD3DDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,
		0, start_vertex, Count*2, (start_vertex>>2)*6, Count ));
}

/////////////////////////cQuadBufferInternal///////////////////////////////

cQuadBufferInternal::cQuadBufferInternal()
{
	start_vertex=NULL;
}

cQuadBufferInternal::~cQuadBufferInternal()
{
}

void cQuadBufferInternal::Destroy()
{
	cVertexBufferInternal::Destroy();
}

void cQuadBufferInternal::Create(int vertexsize,int _fmt)
{
	cVertexBufferInternal::Create(vertexsize*POLYGONMAX*2,vertexsize,_fmt);
}

void cQuadBufferInternal::SetMatrix(const MatXf &m)
{
	gb_RenderDevice3D->SetMatrix(D3DTS_WORLD,m);
}

void cQuadBufferInternal::BeginDraw()
{
	VISASSERT(start_vertex==NULL);
	start_vertex=cVertexBufferInternal::Lock(4);
	vertex_index=0;
}

void* cQuadBufferInternal::Get()
{
	if(vertex_index+4>=GetSize())
	{
		EndDraw();
		BeginDraw();
	}

	BYTE* cur=start_vertex+vertex_index*vb.size;
	vertex_index+=4;
	return cur;
}

void cQuadBufferInternal::EndDraw()
{
	Unlock(vertex_index);
	if(vertex_index>0)
		DrawIndexedPrimitive(vertex_index>>1);
	start_vertex=NULL;
}

void DrawStrip::Begin()
{
	gb_RenderDevice3D->SetMatrix(D3DTS_WORLD,MatXf::ID);
	buf=gb_RenderDevice->GetBufferXYZDT1();
	pointer=buf->Lock(8);
	num=0;
}

void DrawStrip::End()
{
	buf->Unlock(num);
	if(num>=4)
	{
		buf->DrawPrimitive(PT_TRIANGLESTRIP,num-2);
	}
}

void DrawStripT2::Begin(const MatXf& mat)
{
	gb_RenderDevice3D->SetMatrix(D3DTS_WORLD,mat);
	buf=gb_RenderDevice->GetBufferXYZDT2();
	pointer=buf->Lock(8);
	num=0;
}

void DrawStripT2::End()
{
	buf->Unlock(num);
	if(num>=4)
	{
		buf->DrawPrimitive(PT_TRIANGLESTRIP,num-2);
	}
}

void cD3DRender::InitStandartIB()
{
	CreateIndexBuffer(standart_ib,POLYGONMAX);
	sPolygon* polygon=LockIndexBuffer(standart_ib);

	for(int nPolygon=0; nPolygon<POLYGONMAX; nPolygon+=2 )
	{
		polygon[nPolygon+0].set(2*nPolygon+0,2*nPolygon+1,2*nPolygon+2);
		polygon[nPolygon+1].set(2*nPolygon+2,2*nPolygon+1,2*nPolygon+3);
	}

	UnlockIndexBuffer(standart_ib);
}


void cD3DRender::SaveStates(const char* fname)
{
	FILE* f=fopen(fname,"wt");
	fprintf(f,"Render state\n");
#define W(s) {DWORD d;RDCALL(lpD3DDevice->GetRenderState(s,&d));fprintf(f,"%s=%x\n",#s,d); }
	W(D3DRS_ZENABLE);
    W(D3DRS_FILLMODE);
    W(D3DRS_SHADEMODE);
    W(D3DRS_ZWRITEENABLE);
    W(D3DRS_ALPHATESTENABLE);
    W(D3DRS_LASTPIXEL);
    W(D3DRS_SRCBLEND);
    W(D3DRS_DESTBLEND);
    W(D3DRS_CULLMODE);
    W(D3DRS_ZFUNC);
    W(D3DRS_ALPHAREF);
    W(D3DRS_ALPHAFUNC);
    W(D3DRS_DITHERENABLE);
    W(D3DRS_ALPHABLENDENABLE);
    W(D3DRS_FOGENABLE);
    W(D3DRS_SPECULARENABLE);
    W(D3DRS_FOGCOLOR);
    W(D3DRS_FOGTABLEMODE);
    W(D3DRS_FOGSTART);
    W(D3DRS_FOGEND);
    W(D3DRS_FOGDENSITY);
    W(D3DRS_RANGEFOGENABLE);
    W(D3DRS_STENCILENABLE);
    W(D3DRS_STENCILFAIL);
    W(D3DRS_STENCILZFAIL);
    W(D3DRS_STENCILPASS);
    W(D3DRS_STENCILFUNC);
    W(D3DRS_STENCILREF);
    W(D3DRS_STENCILMASK);
    W(D3DRS_STENCILWRITEMASK);
    W(D3DRS_TEXTUREFACTOR);
    W(D3DRS_WRAP0);
    W(D3DRS_WRAP1);
    W(D3DRS_WRAP2);
    W(D3DRS_WRAP3);
    W(D3DRS_WRAP4);
    W(D3DRS_WRAP5);
    W(D3DRS_WRAP6);
    W(D3DRS_WRAP7);
    W(D3DRS_CLIPPING);
    W(D3DRS_LIGHTING);
    W(D3DRS_AMBIENT);
    W(D3DRS_FOGVERTEXMODE);
    W(D3DRS_COLORVERTEX);
    W(D3DRS_LOCALVIEWER);
    W(D3DRS_NORMALIZENORMALS);
    W(D3DRS_DIFFUSEMATERIALSOURCE);
    W(D3DRS_SPECULARMATERIALSOURCE);
    W(D3DRS_AMBIENTMATERIALSOURCE);
    W(D3DRS_EMISSIVEMATERIALSOURCE);
    W(D3DRS_VERTEXBLEND);
    W(D3DRS_CLIPPLANEENABLE);
    W(D3DRS_POINTSIZE);
    W(D3DRS_POINTSIZE_MIN);
    W(D3DRS_POINTSPRITEENABLE);
    W(D3DRS_POINTSCALEENABLE);
    W(D3DRS_POINTSCALE_A);
    W(D3DRS_POINTSCALE_B);
    W(D3DRS_POINTSCALE_C);
    W(D3DRS_MULTISAMPLEANTIALIAS);
    W(D3DRS_MULTISAMPLEMASK);
    W(D3DRS_PATCHEDGESTYLE);
    W(D3DRS_DEBUGMONITORTOKEN);
    W(D3DRS_POINTSIZE_MAX);
    W(D3DRS_INDEXEDVERTEXBLENDENABLE);
    W(D3DRS_COLORWRITEENABLE);
    W(D3DRS_TWEENFACTOR);
    W(D3DRS_BLENDOP);
    W(D3DRS_POSITIONDEGREE);
    W(D3DRS_NORMALDEGREE);
    W(D3DRS_SCISSORTESTENABLE);
    W(D3DRS_SLOPESCALEDEPTHBIAS);
    W(D3DRS_ANTIALIASEDLINEENABLE);
    W(D3DRS_MINTESSELLATIONLEVEL);
    W(D3DRS_MAXTESSELLATIONLEVEL);
    W(D3DRS_ADAPTIVETESS_X);
    W(D3DRS_ADAPTIVETESS_Y);
    W(D3DRS_ADAPTIVETESS_Z);
    W(D3DRS_ADAPTIVETESS_W);
//    W(D3DRS_ENABLEADAPTIVETESSELATION);
    W(D3DRS_TWOSIDEDSTENCILMODE);
    W(D3DRS_CCW_STENCILFAIL);
    W(D3DRS_CCW_STENCILZFAIL);
    W(D3DRS_CCW_STENCILPASS);
    W(D3DRS_CCW_STENCILFUNC);
    W(D3DRS_COLORWRITEENABLE1);
    W(D3DRS_COLORWRITEENABLE2);
    W(D3DRS_COLORWRITEENABLE3);
    W(D3DRS_BLENDFACTOR);
    W(D3DRS_SRGBWRITEENABLE);
    W(D3DRS_DEPTHBIAS);
    W(D3DRS_WRAP8);
    W(D3DRS_WRAP9);
    W(D3DRS_WRAP10);
    W(D3DRS_WRAP11);
    W(D3DRS_WRAP12);
    W(D3DRS_WRAP13);
    W(D3DRS_WRAP14);
    W(D3DRS_WRAP15);
    W(D3DRS_SEPARATEALPHABLENDENABLE);
    W(D3DRS_SRCBLENDALPHA);
    W(D3DRS_DESTBLENDALPHA);
    W(D3DRS_BLENDOPALPHA);
#undef W

	fprintf(f,"\nSampler state\n");
#define W(i,s) {DWORD d;RDCALL(lpD3DDevice->GetSamplerState(i,s,&d));fprintf(f,"[%i]%s=%x\n",i,#s,d); }
	for(int i=0;i<4;i++)
	{
		W(i,D3DSAMP_ADDRESSU);
		W(i,D3DSAMP_ADDRESSV);
		W(i,D3DSAMP_ADDRESSW);
		W(i,D3DSAMP_BORDERCOLOR);
		W(i,D3DSAMP_MAGFILTER);
		W(i,D3DSAMP_MINFILTER);
		W(i,D3DSAMP_MIPFILTER);
		W(i,D3DSAMP_MIPMAPLODBIAS);
		W(i,D3DSAMP_MAXMIPLEVEL);
		W(i,D3DSAMP_MAXANISOTROPY);
		W(i,D3DSAMP_SRGBTEXTURE);
		W(i,D3DSAMP_ELEMENTINDEX);
		W(i,D3DSAMP_DMAPOFFSET);
		fprintf(f,"\n");
	}
#undef W

	fprintf(f,"\nTexture stage state\n");
#define W(i,s) {DWORD d;RDCALL(lpD3DDevice->GetTextureStageState(i,s,&d));fprintf(f,"[%i]%s=%x\n",i,#s,d); }
	int i;
	for(i=0;i<4;i++)
	{
		W(i,D3DTSS_COLOROP);
		W(i,D3DTSS_COLORARG1);
		W(i,D3DTSS_COLORARG2);
		W(i,D3DTSS_ALPHAOP);
		W(i,D3DTSS_ALPHAARG1);
		W(i,D3DTSS_ALPHAARG2);
		W(i,D3DTSS_BUMPENVMAT00);
		W(i,D3DTSS_BUMPENVMAT01);
		W(i,D3DTSS_BUMPENVMAT10);
		W(i,D3DTSS_BUMPENVMAT11);
		W(i,D3DTSS_TEXCOORDINDEX);
		W(i,D3DTSS_BUMPENVLSCALE);
		W(i,D3DTSS_BUMPENVLOFFSET);
		W(i,D3DTSS_TEXTURETRANSFORMFLAGS);
		W(i,D3DTSS_COLORARG0);
		W(i,D3DTSS_ALPHAARG0);
		W(i,D3DTSS_RESULTARG);
		W(i,D3DTSS_CONSTANT);
		fprintf(f,"\n");
	}
#undef W
	fclose(f);
}


void cD3DRender::DrawSpriteScale(int x,int y,int dx,int dy,float u,float v,
	cTextureScale *Texture,sColor4c &ColorMul,float phase,eBlendMode mode)
{
	float du,dv;
	Texture->ConvertUV(u,v);
	Texture->DXY2DUV(dx,dy,du,dv);
	DrawSprite(x,y,dx,dy,u,v,du,dv,
		Texture,ColorMul,phase,mode);
}

void cD3DRender::DrawSpriteScale2(int x,int y,int dx,int dy,float u,float v,
		cTextureScale *Tex1,cTextureScale *Tex2,sColor4c &ColorMul,float phase)
{
	DrawSpriteScale2(x,y,dx,dy,u,v,u,v,
		Tex1,Tex2,ColorMul,phase);
}

void cD3DRender::DrawSpriteScale2(int x,int y,int dx,int dy,float u,float v,float u1,float v1,
		cTextureScale *Tex1,cTextureScale *Tex2,sColor4c &ColorMul,float phase,eColorMode mode)
{
	float du,dv,du1,dv1;
	Tex1->ConvertUV(u,v);
	Tex2->ConvertUV(u1,v1);
	Tex1->DXY2DUV(dx,dy,du,dv);
	Tex2->DXY2DUV(dx,dy,du1,dv1);

	DrawSprite2(x,y,dx,dy,u,v,du,dv,u1,v1,du1,dv1,
		Tex1,Tex2,ColorMul,phase,mode);

}

bool cD3DRender::PossibleAnisotropic()
{
	D3DCAPS9 caps;
	RDCALL(lpD3DDevice->GetDeviceCaps(&caps));
	return caps.TextureFilterCaps&D3DPTFILTERCAPS_MINFANISOTROPIC;
}

void cD3DRender::SetAnisotropic(bool enable)
{
	if(PossibleAnisotropic())
		texture_interpolation=enable?D3DTEXF_ANISOTROPIC:D3DTEXF_LINEAR;
	else
		texture_interpolation=D3DTEXF_LINEAR;
}

bool cD3DRender::GetAnisotropic()
{
	return texture_interpolation==D3DTEXF_ANISOTROPIC;
}

bool cD3DRender::ReinitOcclusion()
{
	bool ok=true;
	vector<cOcclusionQuery*>::iterator it;
	FOR_EACH(occlusion_query,it)
	{
		bool b=(*it)->Init();
		ok = ok && b;
	}

	return ok;
}

bool cD3DRender::PossibilityOcclusion()
{
	HRESULT hr;
	IDirect3DQuery9* pQuery=NULL;
	hr=gb_RenderDevice3D->lpD3DDevice->CreateQuery(D3DQUERYTYPE_OCCLUSION,&pQuery);
	RELEASE(pQuery);
	return SUCCEEDED(hr);
}

#ifndef _FINAL_VERSION_
void IsDeleteAllDefaultTextures()
{
	int num_undeleted=0,num_uncorrect=0;
	for(cCheckDelete* cur=cCheckDelete::GetDebugRoot();cur;cur=cur->GetDebugNext())
	{
		cTexture* p=dynamic_cast<cTexture*>(cur);
		if(p)
		{
			if(p->GetAttribute(TEXTURE_D3DPOOL_DEFAULT))
			{
				for(int nFrame=0;nFrame<p->GetNumberFrame();nFrame++)
				if(p->GetDDSurface(nFrame)) 
				{
					dprintf("D3DPOOL_DEFAULT size=(%i,%i)",p->GetWidth(),p->GetHeight());
					num_undeleted++;
				}
			}else
			{
				for(int nFrame=0;nFrame<p->GetNumberFrame();nFrame++)
				if(p->GetDDSurface(nFrame)) 
				{
					IDirect3DTexture9* surface=p->GetDDSurface(nFrame);
					D3DSURFACE_DESC desc;
					RDCALL(surface->GetLevelDesc(0,&desc));
					if(desc.Pool==D3DPOOL_DEFAULT)
					{
						xassert(0);
						dprintf("D3DPOOL_DEFAULT size=(%i,%i)",p->GetWidth(),p->GetHeight());
						num_uncorrect++;
					}
				}
			}
		}
	}

	if(num_undeleted)
		dprintf("D3DPOOL_DEFAULT not delete=%i",num_undeleted);
	if(num_uncorrect)
		dprintf("D3DPOOL_DEFAULT not correct=%i",num_uncorrect);
}
#endif 

void cD3DRender::SetDialogBoxMode(bool enable)
{
	if(RenderMode&RENDERDEVICE_MODE_WINDOW)
		return;
	if(enable)
		RenderMode|=RENDERDEVICE_MODE_ONEBACKBUFFER;
	else
		RenderMode&=~(DWORD)RENDERDEVICE_MODE_ONEBACKBUFFER;

	KillFocus();
	SetFocus(false);
}
