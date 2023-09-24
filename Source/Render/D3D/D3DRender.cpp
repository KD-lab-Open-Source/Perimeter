#ifndef _WIN32
//For SDL_WINDOW_VULKAN constant
#include <SDL_video.h>
#endif
#include "StdAfxRD.h"
#include "D3DRender.h"
#include "Font.h"
#include "files/files.h"
#include "DrawBuffer.h"
#include "RenderTracker.h"
#include "DebugUtil.h"
#include "SystemUtil.h"

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#include <windows.h>
#define execv _execv
//Needed for extracting HWND from SDL_Window, in Linux it gives conflict due to XErrorHandler
#include <SDL_syswm.h>
#include <commdlg.h>
#endif

static uint32_t ColorConvertARGB(const sColor4c& c) { return CONVERT_COLOR_TO_ARGB(c.v); };

cD3DRender *gb_RenderDevice3D = nullptr;

void IsDeleteAllDefaultTextures();

FILE* fRD= nullptr;

char* GetErrorText(HRESULT hr)
{
    switch(hr)
    {
        case D3D_OK :
            return "No error occurred. ";
        case D3DERR_CONFLICTINGRENDERSTATE :
            return "The currently set render states cannot be used together. ";
        case D3DERR_CONFLICTINGTEXTUREFILTER :
            return "The current texture filters cannot be used together. ";
        case D3DERR_CONFLICTINGTEXTUREPALETTE :
            return "The current textures cannot be used simultaneously. This generally occurs when a multitexture device requires that all palletized textures simultaneously enabled also share the same palette. ";
        case D3DERR_DEVICELOST :
            return "The device is lost and cannot be restored at the current time, so rendering is not possible. ";
        case D3DERR_DEVICENOTRESET :
            return "The device cannot be reset. ";
        case D3DERR_DRIVERINTERNALERROR :
            return "Internal driver error. ";
        case D3DERR_INVALIDCALL :
#ifdef E_INVALIDCALL
        case E_INVALIDCALL:
#endif
            return "The method call is invalid. For example, a method's parameter may have an invalid value. ";
        case D3DERR_INVALIDDEVICE :
            return "The requested device type is not valid. ";
        case D3DERR_MOREDATA :
            return "There is more data available than the specified buffer size can hold. ";
        case D3DERR_NOTAVAILABLE :
            return "This device does not support the queried technique. ";
        case D3DERR_NOTFOUND :
            return "The requested item was not found. ";
        case D3DERR_OUTOFVIDEOMEMORY :
            return "Direct3D does not have enough display memory to perform the operation. ";
        case D3DERR_TOOMANYOPERATIONS :
            return "The application is requesting more texture-filtering operations than the device supports. ";
        case D3DERR_UNSUPPORTEDALPHAARG :
            return "The device does not support a specified texture-blending argument for the alpha channel. ";
        case D3DERR_UNSUPPORTEDALPHAOPERATION :
            return "The device does not support a specified texture-blending operation for the alpha channel. ";
        case D3DERR_UNSUPPORTEDCOLORARG :
            return "The device does not support a specified texture-blending argument for color values. ";
        case D3DERR_UNSUPPORTEDCOLOROPERATION :
            return "The device does not support a specified texture-blending operation for color values. ";
        case D3DERR_UNSUPPORTEDFACTORVALUE :
            return "The device does not support the specified texture factor value. ";
        case D3DERR_UNSUPPORTEDTEXTUREFILTER :
            return "The device does not support the specified texture filter. ";
        case D3DERR_WRONGTEXTUREFORMAT :
            return "The pixel format of the texture surface is not valid. ";
        case E_FAIL :
            return "An undetermined error occurred inside the Direct3D subsystem. ";
        case E_INVALIDARG :
            return "An invalid parameter was passed to the returning function ";
        case E_OUTOFMEMORY :
            return "Direct3D could not allocate sufficient memory to complete the call. ";
        case E_NOTIMPL :
            return "Not implemented. ";
        case E_NOINTERFACE :
            return "No object interface is available. ";
        default:
            fprintf(stderr, "Unknown D3D error: %d\n", hr);
            return "Unknown D3D error. ";
    }
};

void RDOpenLog(char *fname="RenderDevice.!!!")
{
#ifndef _FINAL_VERSION_
    fRD=fopen(convert_path_content(fname, true).c_str(),"wt");
	fprintf(fRD,"----------------- Compilation data: %s time: %s -----------------\n",__DATE__,__TIME__);
#endif
}
int RDWriteLog(HRESULT err,char *exp,char *file,int line)
{
#ifndef _FINAL_VERSION_
    if (fRD==nullptr) RDOpenLog();
	fprintf(fRD,"%s line: %i - %s = 0x%X , %s\n",file,line,exp,err,GetErrorText(err));
	fflush(fRD);
#endif
    return err;
}
void RDWriteLog(char *exp,int size)
{
#ifndef _FINAL_VERSION_
    if (fRD==nullptr) RDOpenLog();
	if(size==-1)
		size=strlen(exp);
	fwrite(exp,size,1,fRD);
	fprintf(fRD,"\n");
	fflush(fRD);
#endif
}

cD3DRender::cD3DRender() : cInterfaceRenderDevice()
{
    ConvertColor = ColorConvertARGB;
    NumberPolygon=0;
    NumDrawObject=0;
    RenderMode=0;
    xScrMin=yScrMin=xScrMax=yScrMax=0;

    bActiveScene=0;
    nSupportTexture=0;

    DrawNode=0;
    DefaultFont=CurrentFont=0;
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

	texture_interpolation=D3DTEXF_LINEAR;
}

cD3DRender::~cD3DRender()
{
	Done();
    VISASSERT(RenderMode==0);
    VISASSERT(ScreenSize.x==0&&ScreenSize.y==0&&xScrMin==0&&yScrMin==0&&xScrMax==0&&yScrMax==0);
    VISASSERT(ScreenSize.x==0&&ScreenSize.y==0);
}

uint32_t cD3DRender::GetD3DFVFFromFormat(vertex_fmt_t fmt) {
    switch (fmt) {
        case sVertexXYZ::fmt:
            return D3DFVF_XYZ;
        case sVertexXYZT1::fmt:
            return D3DFVF_XYZ|D3DFVF_TEX1;
        case sVertexXYZDT1::fmt:
            return D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1;
        case sVertexXYZDT2::fmt:
            return D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX2;
        case sVertexXYZNT1::fmt:
            return D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX1;
        case sVertexDot3::fmt:
            return D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX4
                  |D3DFVF_TEXCOORDSIZE2(0)|D3DFVF_TEXCOORDSIZE3(1)|D3DFVF_TEXCOORDSIZE3(2)|D3DFVF_TEXCOORDSIZE3(3);
        default:
            xassert(0);
            return fmt;
    }
}

uint32_t cD3DRender::GetWindowCreationFlags() const {
    uint32_t flags = cInterfaceRenderDevice::GetWindowCreationFlags();
#ifndef _WIN32
    //On non Windows we use dxvk which uses Vulkan
    flags |= SDL_WINDOW_VULKAN;
#endif
    return flags;
}

int cD3DRender::Init(int xscr,int yscr,int Mode, SDL_Window* wnd, int RefreshRateInHz)
{
    RenderSubmitEvent(RenderEvent::INIT, "D3D9 start");
    Done();
    int ret = cInterfaceRenderDevice::Init(xscr, yscr, Mode, wnd, RefreshRateInHz);
    if (ret != 0) return ret;
    if (wnd == nullptr) return 1;
    gb_RenderDevice3D = this;

	memset(CurrentTexture,0,sizeof(CurrentTexture));
	memset(ArrayRenderState,0xEF,sizeof(ArrayRenderState));

#ifdef _WIN32
    //Get HWND from SDL window
    SDL_SysWMinfo wm_info;
    SDL_VERSION(&wm_info.version);
    SDL_GetWindowWMInfo(sdlWindow, &wm_info);
    
    this->hWnd = hWndVisGeneric = wm_info.info.win.window;
#else
    //dxvk-native uses HWND as SDL2 window handle, so this is allowed
    this->hWnd = static_cast<HWND>(sdl_window);
#endif

	if(!lpD3D)
		RDERR((lpD3D=Direct3DCreate9(D3D_SDK_VERSION))==0);
		//RDERR((lpD3D=Direct3DCreate9(D3D9b_SDK_VERSION))==0);
		
	if(lpD3D==0) return 2;
	D3DDISPLAYMODE d3ddm;
    
    IniManager ini("Perimeter.ini", false);
    int adapter_int = D3DADAPTER_DEFAULT; 
    ini.getInt("Graphics", "Adapter", adapter_int);
    check_command_line_parameter("d3d_adapter", adapter_int);
    Adapter = adapter_int;
    
	RDERR(lpD3D->GetAdapterDisplayMode(Adapter,&d3ddm));
	if(Mode&RENDERDEVICE_MODE_WINDOW) {
		if(d3ddm.Format==D3DFMT_X8R8G8B8||d3ddm.Format==D3DFMT_R8G8B8||d3ddm.Format==D3DFMT_A8R8G8B8)
			RenderMode&=~RENDERDEVICE_MODE_RGB16,RenderMode|=RENDERDEVICE_MODE_RGB32;
		else
			RenderMode&=~RENDERDEVICE_MODE_RGB32,RenderMode|=RENDERDEVICE_MODE_RGB16;
    }

	if(!(RenderMode&RENDERDEVICE_MODE_RGB32))
		RenderMode|=RENDERDEVICE_MODE_RGB16;

	RDCALL(lpD3D->GetDeviceCaps(Adapter,D3DDEVTYPE_HAL,&DeviceCaps));
    memset(&d3dpp, 0, sizeof(d3dpp));
    d3dpp.MultiSampleType			= D3DMULTISAMPLE_NONE;
    d3dpp.MultiSampleQuality		= 0;
    d3dpp.SwapEffect				= D3DSWAPEFFECT_COPY;
    d3dpp.EnableAutoDepthStencil	= TRUE;
    d3dpp.Flags						= D3DPRESENTFLAG_DISCARD_DEPTHSTENCIL;
    d3dpp.PresentationInterval		= D3DPRESENT_INTERVAL_IMMEDIATE;
	UpdateRenderMode();

	bSupportVertexShaderHardware=bSupportVertexShader=(D3DSHADER_VERSION_MAJOR(DeviceCaps.VertexShaderVersion)>=1);

	uint32_t mt=D3DCREATE_FPU_PRESERVE;

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

	if(lpD3DDevice==nullptr) return 3;

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

	SetClipRect(xScrMin=0,yScrMin=0,xScrMax=ScreenSize.x,yScrMax=ScreenSize.y);

	RDCALL(lpD3DDevice->GetDeviceCaps(&DeviceCaps));
	dwSuportMaxSizeTextureX=DeviceCaps.MaxTextureWidth;
	dwSuportMaxSizeTextureY=DeviceCaps.MaxTextureHeight;
	bSupportVertexShader=(D3DSHADER_VERSION_MAJOR(DeviceCaps.VertexShaderVersion)>=1)?1:0;
	bSupportVertexFog=(DeviceCaps.RasterCaps&D3DPRASTERCAPS_FOGVERTEX)?1:0;
	bSupportTableFog=(DeviceCaps.RasterCaps&D3DPRASTERCAPS_FOGTABLE)?1:0;
	CurrentMod4 = (DeviceCaps.TextureOpCaps&D3DTEXOPCAPS_MODULATE4X)?D3DTOP_MODULATE4X:((DeviceCaps.TextureOpCaps&D3DTEXOPCAPS_MODULATE2X)?D3DTOP_MODULATE2X:D3DTOP_MODULATE);

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
	Fill(0,0,0,255);
	BeginScene();
	DrawRectangle(0,0,ScreenSize.x-1,ScreenSize.y-1,sColor4c(0,0,0,255));
	EndScene();

	Flush();

	dtFixed=new DrawTypeFixedPipeline;

    /*
     * These don't work properly under Vulkan in MacOS, disable them for now
     * Bump mapping and Self shadow are disabled by not loading shaders
     * Bump chaos is disabled manually elsewhere
     */
#ifndef __APPLE__
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
#endif

	SetAdvance();

	if(bSupportTableFog)
	{
		if(!(dtAdvanceOriginal && (dtAdvanceOriginal->GetID()!=DT_GEFORCE3 && dtAdvanceOriginal->GetID()!=DT_GEFORCEFX)))
			bSupportTableFog=false;
	}
    
    gb_RenderDevice3D = this;
    
    //Workaround for some distros (Ubuntu?) setting window bigger than originally requested
    //Since Steam Linux Runtime is based on ubuntu it affects there too
    if (sdl_window && (RenderMode & RENDERDEVICE_MODE_WINDOW)) {
        Vect2i size;
        SDL_GetWindowSize(sdl_window, &size.x, &size.y);
        if (size != ScreenSize) {
            //Set correct size
            SDL_SetWindowSize(sdl_window, ScreenSize.x, ScreenSize.y);
            //Put on center again
            int screen = SDL_GetWindowDisplayIndex(sdl_window);
            int windowPos = SDL_WINDOWPOS_CENTERED_DISPLAY(screen);
            SDL_SetWindowPosition(sdlWindow, windowPos, windowPos);
        }
    }

    RenderSubmitEvent(RenderEvent::INIT, "D3D9 end");
	return 0;
}

D3DFORMAT cD3DRender::GetBackBufferFormat(uint32_t Mode)
{
	D3DFORMAT BackBufferFormat=D3DFMT_X8R8G8B8;
	if(Mode&RENDERDEVICE_MODE_WINDOW)
	{
		D3DDISPLAYMODE d3ddm;
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

    D3DDISPLAYMODE d3ddm;
    RDCALL(lpD3D->GetAdapterDisplayMode(Adapter,&d3ddm));
    MaxScreenSize.x = max(MaxScreenSize.x, static_cast<int>(d3ddm.Width));
    MaxScreenSize.y = max(MaxScreenSize.y, static_cast<int>(d3ddm.Height));
    
	d3dpp.AutoDepthStencilFormat    = is32zbuffer ? D3DFMT_D24S8 : D3DFMT_D16;
	d3dpp.BackBufferFormat          = GetBackBufferFormat(RenderMode);
    d3dpp.hDeviceWindow				= hWnd;
    d3dpp.Windowed					= (RenderMode&RENDERDEVICE_MODE_WINDOW)?TRUE:FALSE;
    d3dpp.FullScreen_RefreshRateInHz= d3dpp.Windowed ? 0 : ScreenHZ;
	d3dpp.BackBufferCount			= (d3dpp.Windowed | (RenderMode&RENDERDEVICE_MODE_ONEBACKBUFFER)) ? 1 : 2;

    //Set biggest size the window can have so we don't have to reinit render device each time user resizes the window
    d3dpp.BackBufferWidth = MaxScreenSize.x;
    d3dpp.BackBufferHeight = MaxScreenSize.y;
    fprintf(stdout, "D3D Backbuffer size: %dx%d\n", d3dpp.BackBufferWidth, d3dpp.BackBufferHeight);

	if(RenderMode&RENDERDEVICE_MODE_COMPRESS)
	{
		if(lpD3D->CheckDeviceFormat(Adapter,D3DDEVTYPE_HAL,d3ddm.Format,0,D3DRTYPE_TEXTURE,D3DFMT_DXT5)==0)
			TexFmtData[SURFMT_COLOR].Set(4,8,8,8,8,16,8,0,24,D3DFMT_DXT5);
		else if(lpD3D->CheckDeviceFormat(Adapter,D3DDEVTYPE_HAL,d3ddm.Format,0,D3DRTYPE_TEXTURE,D3DFMT_DXT3)==0)
			TexFmtData[SURFMT_COLOR].Set(4,8,8,8,8,16,8,0,24,D3DFMT_DXT3);
		
		if(lpD3D->CheckDeviceFormat(Adapter,D3DDEVTYPE_HAL,d3ddm.Format,0,D3DRTYPE_TEXTURE,D3DFMT_DXT4)==0)
			TexFmtData[SURFMT_COLORALPHA].Set(4,8,8,8,8,16,8,0,24,D3DFMT_DXT4);
		else if(lpD3D->CheckDeviceFormat(Adapter,D3DDEVTYPE_HAL,d3ddm.Format,0,D3DRTYPE_TEXTURE,D3DFMT_DXT2)==0)
			TexFmtData[SURFMT_COLORALPHA].Set(4,8,8,8,8,16,8,0,24,D3DFMT_DXT2);
    }
	else if(RenderMode&RENDERDEVICE_MODE_RGB32)
	{
		if(lpD3D->CheckDeviceFormat(Adapter,D3DDEVTYPE_HAL,d3ddm.Format,0,D3DRTYPE_TEXTURE,D3DFMT_A8R8G8B8)==0)
			TexFmtData[SURFMT_COLOR].Set(4,8,8,8,8,16,8,0,24,D3DFMT_A8R8G8B8);
		else if(lpD3D->CheckDeviceFormat(Adapter,D3DDEVTYPE_HAL,d3ddm.Format,0,D3DRTYPE_TEXTURE,D3DFMT_X8R8G8B8)==0)
			TexFmtData[SURFMT_COLOR].Set(4,8,8,8,8,16,8,0,24,D3DFMT_X8R8G8B8);
		else if(lpD3D->CheckDeviceFormat(Adapter,D3DDEVTYPE_HAL,d3ddm.Format,0,D3DRTYPE_TEXTURE,D3DFMT_R8G8B8)==0)
			TexFmtData[SURFMT_COLOR].Set(3,8,8,8,0,16,8,0,0,D3DFMT_R8G8B8);
        
		if(lpD3D->CheckDeviceFormat(Adapter,D3DDEVTYPE_HAL,d3ddm.Format,0,D3DRTYPE_TEXTURE,D3DFMT_A8R8G8B8)==0)
			TexFmtData[SURFMT_COLORALPHA].Set(4,8,8,8,8,16,8,0,24,D3DFMT_A8R8G8B8);
	}
	else // if(RenderMode&RENDERDEVICE_MODE_RGB16)
	{
		if(lpD3D->CheckDeviceFormat(Adapter,D3DDEVTYPE_HAL,d3ddm.Format,0,D3DRTYPE_TEXTURE,D3DFMT_A4R4G4B4)==0)
			TexFmtData[SURFMT_COLOR].Set(2,4,4,4,4,8,4,0,12,D3DFMT_A4R4G4B4);
		else if(lpD3D->CheckDeviceFormat(Adapter,D3DDEVTYPE_HAL,d3ddm.Format,0,D3DRTYPE_TEXTURE,D3DFMT_X4R4G4B4)==0)
			TexFmtData[SURFMT_COLOR].Set(2,4,4,4,4,8,4,0,12,D3DFMT_X4R4G4B4);
		else if(lpD3D->CheckDeviceFormat(Adapter,D3DDEVTYPE_HAL,d3ddm.Format,0,D3DRTYPE_TEXTURE,D3DFMT_X1R5G5B5)==0)
			TexFmtData[SURFMT_COLOR].Set(2,5,5,5,1,10,5,0,15,D3DFMT_X1R5G5B5);
		else if(lpD3D->CheckDeviceFormat(Adapter,D3DDEVTYPE_HAL,d3ddm.Format,0,D3DRTYPE_TEXTURE,D3DFMT_R5G6B5)==0)
			TexFmtData[SURFMT_COLOR].Set(2,5,6,5,0,11,5,0,0,D3DFMT_R5G6B5);
		
		if(lpD3D->CheckDeviceFormat(Adapter,D3DDEVTYPE_HAL,d3ddm.Format,0,D3DRTYPE_TEXTURE,D3DFMT_A4R4G4B4)==0)
			TexFmtData[SURFMT_COLORALPHA].Set(2,4,4,4,4,8,4,0,12,D3DFMT_A4R4G4B4);
	}

	//32
	if(lpD3D->CheckDeviceFormat(Adapter,D3DDEVTYPE_HAL,d3ddm.Format,0,D3DRTYPE_TEXTURE,D3DFMT_A8R8G8B8)==0)
		TexFmtData[SURFMT_COLOR32].Set(4,8,8,8,8,16,8,0,24,D3DFMT_A8R8G8B8);
	else if(lpD3D->CheckDeviceFormat(Adapter,D3DDEVTYPE_HAL,d3ddm.Format,0,D3DRTYPE_TEXTURE,D3DFMT_X8R8G8B8)==0)
		TexFmtData[SURFMT_COLOR32].Set(4,8,8,8,8,16,8,0,24,D3DFMT_X8R8G8B8);
    
    if(lpD3D->CheckDeviceFormat(Adapter,D3DDEVTYPE_HAL,d3ddm.Format,0,D3DRTYPE_TEXTURE,D3DFMT_A8R8G8B8)==0)
		TexFmtData[SURFMT_COLORALPHA32].Set(4,8,8,8,8,16,8,0,24,D3DFMT_A8R8G8B8);

	// bump map format
	if(lpD3D->CheckDeviceFormat(Adapter,D3DDEVTYPE_HAL,d3ddm.Format,0,D3DRTYPE_TEXTURE,D3DFMT_Q8W8V8U8)==0)
		TexFmtData[SURFMT_BUMP].Set(4,8,8,8,8,16,8,0,24,D3DFMT_Q8W8V8U8);
		
	// render map format
	if(lpD3D->CheckDeviceFormat(Adapter,D3DDEVTYPE_HAL,d3ddm.Format,D3DUSAGE_RENDERTARGET,D3DRTYPE_TEXTURE,D3DFMT_X8R8G8B8)==0)
		TexFmtData[SURFMT_RENDERMAP32].Set(4,8,8,8,8,16,8,0,24,D3DFMT_X8R8G8B8);
	else if(lpD3D->CheckDeviceFormat(Adapter,D3DDEVTYPE_HAL,d3ddm.Format,D3DUSAGE_RENDERTARGET,D3DRTYPE_TEXTURE,D3DFMT_A8R8G8B8)==0)
		TexFmtData[SURFMT_RENDERMAP32].Set(4,8,8,8,8,16,8,0,24,D3DFMT_A8R8G8B8);
	else if(lpD3D->CheckDeviceFormat(Adapter,D3DDEVTYPE_HAL,d3ddm.Format,0,D3DRTYPE_TEXTURE,D3DFMT_R8G8B8)==0)
		TexFmtData[SURFMT_RENDERMAP32].Set(3,8,8,8,0,16,8,0,0,D3DFMT_R8G8B8);

	if(lpD3D->CheckDeviceFormat(Adapter,D3DDEVTYPE_HAL,d3ddm.Format,D3DUSAGE_RENDERTARGET,D3DRTYPE_TEXTURE,D3DFMT_R5G6B5)==0)
		TexFmtData[SURFMT_RENDERMAP16].Set(2,5,6,5,0,11,5,0,0,D3DFMT_R5G6B5);
	else if(lpD3D->CheckDeviceFormat(Adapter,D3DDEVTYPE_HAL,d3ddm.Format,D3DUSAGE_RENDERTARGET,D3DRTYPE_TEXTURE,D3DFMT_A4R4G4B4)==0)
		TexFmtData[SURFMT_RENDERMAP16].Set(2,4,4,4,4,8,4,0,12,D3DFMT_A4R4G4B4);
	else if(lpD3D->CheckDeviceFormat(Adapter,D3DDEVTYPE_HAL,d3ddm.Format,D3DUSAGE_RENDERTARGET,D3DRTYPE_TEXTURE,D3DFMT_X1R5G5B5)==0)
		TexFmtData[SURFMT_RENDERMAP16].Set(2,5,5,5,1,10,5,0,15,D3DFMT_X1R5G5B5);

	TexFmtData[SURFMT_RENDERMAP_FLOAT].Set(0,0,0,0,0,0,0,0,0,0);
	if(lpD3D->CheckDeviceFormat(Adapter,D3DDEVTYPE_HAL,d3ddm.Format,D3DUSAGE_RENDERTARGET,D3DRTYPE_TEXTURE,D3DFMT_R32F)==0)
		TexFmtData[SURFMT_RENDERMAP_FLOAT].Set(0,0,0,0,0,0,0,0,0,D3DFMT_R32F);

	{
		if(lpD3D->CheckDeviceFormat(Adapter,D3DDEVTYPE_HAL,d3ddm.Format,0,D3DRTYPE_TEXTURE,D3DFMT_A8)==0) {
            TexFmtData[SURFMT_GRAYALPHA].Set(1, 0, 0, 0, 8, 0, 0, 0, 0, D3DFMT_A8);
        } else if(lpD3D->CheckDeviceFormat(Adapter,D3DDEVTYPE_HAL,d3ddm.Format,0,D3DRTYPE_TEXTURE,D3DFMT_A8L8)==0) {
			TexFmtData[SURFMT_GRAYALPHA].Set(2, 8,8,8,8, 0,0,0,8,D3DFMT_A8L8);
		} else {
			if(RenderMode&RENDERDEVICE_MODE_RGB32) {
                TexFmtData[SURFMT_GRAYALPHA].Set(4, 8, 8, 8, 8, 16, 8, 0, 24, D3DFMT_A8R8G8B8);
            } else {
                TexFmtData[SURFMT_GRAYALPHA].Set(2, 4, 4, 4, 4, 8, 4, 0, 12, D3DFMT_A4R4G4B4);
            }
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

    orthoVP = Mat4f::ZERO;
    SetOrthographic(orthoVP, ScreenSize.x, -ScreenSize.y, 10, -10);
    if (isOrthographicProjSet) {
        isOrthographicProjSet = false;
        UseOrthographicProjection();
    }
}

bool cD3DRender::ChangeSize(int xscr, int yscr, int mode)
{
	MTTexObjAutoLock lock;
    
    int mode_mask=RENDERDEVICE_MODE_ALPHA|RENDERDEVICE_MODE_WINDOW
                 |RENDERDEVICE_MODE_RGB16|RENDERDEVICE_MODE_RGB32;

    bool same_size = ScreenSize.x == xscr && ScreenSize.y == yscr;
    ScreenSize.x = xscr;
    ScreenSize.y = yscr;
    
    if (!same_size && ((RenderMode&mode_mask) == (mode&mode_mask)) 
    && ScreenSize.x <= MaxScreenSize.x && ScreenSize.y <= MaxScreenSize.y) {
        //We can change window size without reinitializing graphics
        UpdateRenderMode();
        return true;
    }

    MaxScreenSize.x = max(MaxScreenSize.x, ScreenSize.x);
    MaxScreenSize.y = max(MaxScreenSize.y, ScreenSize.y);
	
	KillFocus();
    
    uint32_t mode_old = RenderMode;
	RenderMode&=~mode_mask;
	RenderMode|=(mode&mode_mask);
    fprintf(stdout, "RenderMode %d new %d with %d\n", mode_old, RenderMode, mode);
    
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

    UpdateRenderMode();

    fprintf(stdout, "d3dpp %dx%d Count %d Hz %d\n", d3dpp.BackBufferWidth, d3dpp.BackBufferHeight, d3dpp.BackBufferCount, d3dpp.FullScreen_RefreshRateInHz);

	bool result = SetFocus(false,(mode&RENDERDEVICE_MODE_RETURNERROR)?false:true);
    if (!result) {
        RestoreDeviceIfLost();
        result = SetFocus(false,(mode&RENDERDEVICE_MODE_RETURNERROR)?false:true);
    }
    
    return result;
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
    RenderSubmitEvent(RenderEvent::DONE, "D3D9 start");
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
/*
	for(int i=0;i<LibVB.size();i++)
	{
		sSlotVB& s=LibVB[i];
		VISASSERT(!s.init);
	}
*/
    LibVB.clear();
    LibIB.clear();

    int ret = cInterfaceRenderDevice::Done();
	
	bActiveScene=0;
	RELEASE(lpD3DDevice);
	RELEASE(lpD3D);

    ScreenSize.x=ScreenSize.y=xScrMin=yScrMin=xScrMax=yScrMax=0;
	hWnd=0;	RenderMode=0;
    
    gb_RenderDevice3D=nullptr;

    RenderSubmitEvent(RenderEvent::DONE, "D3D9 end");
	return ret;
}
int cD3DRender::GetClipRect(int *xmin,int *ymin,int *xmax,int *ymax)
{
	if (lpD3DDevice == nullptr) return -1;
	D3DVIEWPORT9 vp;
	RDCALL(lpD3DDevice->GetViewport(&vp));
	*xmin=xScrMin=vp.X; *xmax=xScrMax=vp.X+vp.Width;
	*ymin=yScrMin=vp.Y; *ymax=yScrMax=vp.Y+vp.Height;
	return 0;
}
int cD3DRender::SetClipRect(int xmin,int ymin,int xmax,int ymax)
{
	if (lpD3DDevice == nullptr) return -1;
    if (xScrMin==xmin && yScrMin==ymin && xScrMax==xmax && yScrMax==ymax) {
        return 0;
    }
    FlushActiveDrawBuffer();
    
	xScrMin=xmin;
	yScrMin=ymin;
	xScrMax=xmax;
	yScrMax=ymax;

    sRect vp = { xScrMin, yScrMin, xScrMax, yScrMax };
	RDCALL(lpD3DDevice->SetScissorRect(reinterpret_cast<RECT*>(&vp)));
	return 0;
}

int cD3DRender::Fill(int r,int g,int b,int a)
{ 
	if(bActiveScene) EndScene();
	if(lpD3DDevice==0) return -1;
	RestoreDeviceIfLost();

	D3DVIEWPORT9 vp={0,0,ScreenSize.x,ScreenSize.y,0.0f,1.0f};
	RDCALL(lpD3DDevice->SetViewport(&vp));
	RDCALL(lpD3DDevice->Clear(0,NULL,D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER|((RenderMode&RENDERDEVICE_MODE_STRENCIL)?D3DCLEAR_STENCIL:0),
		D3DCOLOR_RGBA(r,g,b,a),1,0));

	return 0;
}

void cD3DRender::ClearZBuffer() {
    RDCALL(lpD3DDevice->Clear(0, NULL, D3DCLEAR_ZBUFFER, 0xFFFFFFFF, 1, 0));
}

void cD3DRender::RestoreDeviceIfLost()
{
	int hr;
	while(FAILED(hr=lpD3DDevice->TestCooperativeLevel()))
    { // Test the cooperative level to see if it's okay to render
#ifdef _WIN32
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
#endif

        if(hr==D3DERR_DEVICENOTRESET)
		{
			MTTexObjAutoLock lock;
			KillFocus(); // free all, If the device was lost, do not render until we get it back
			SetFocus(false);//true
		}
    }
}

int cD3DRender::Flush(bool wnd)
{ 
	if(bActiveScene) EndScene();
	MTG();
	RestoreDeviceIfLost();

	RECT rect { 0, 0, ScreenSize.x, ScreenSize.y };
	lpD3DDevice->Present(&rect, &rect, wnd ? hWnd : nullptr, nullptr);

	if(Option_DrawNumberPolygon) 
	{
		char str[256];
		sprintf(str,"poly=%i, obj=%i",NumberPolygon,NumDrawObject);
		OutText(10,100,str);

        /*
		int dbg_MemVertexBuffer=0,dbg_NumVertexBuffer=0;

        for (int i = 0; i < LibVB.size(); i++) {
            if (LibVB[i]->d3d) {
                D3DVERTEXBUFFER_DESC dsc;
                LibVB[i]->d3d->GetDesc(&dsc);
                dbg_MemVertexBuffer += dsc.Size;
                dbg_NumVertexBuffer++;
            };
        }
		sprintf(str,"vb=%i,nvb=%i",dbg_MemVertexBuffer,dbg_NumVertexBuffer);
		OutText(10,80,str);

		int dbg_MemIndexBuffer=0,dbg_NumIndexBuffer=0;
		int i;
        for (i = 0; i < LibIB.size(); i++) {
            if (LibIB[i]->d3d) {
                D3DINDEXBUFFER_DESC dsc;
                LibIB[i]->d3d->GetDesc(&dsc);
                dbg_MemIndexBuffer += dsc.Size;
                dbg_NumIndexBuffer++;
            };
        }
		sprintf(str,"ib=%i,nib=%i",dbg_MemIndexBuffer,dbg_NumIndexBuffer);
		OutText(10,60,str);
        */

		int dbg_MemTexture=0;
		for(int i=0; i<TexLibrary->GetNumberTexture(); i++ )
		{
			cTexture* pTexture=TexLibrary->GetTexture(i);
            if (pTexture) {
                for (int nFrame = 0; nFrame < pTexture->GetNumberFrame(); nFrame++) {
                    TextureImage* tf = pTexture->GetFrameImage(nFrame);
                    if (tf->d3d) {
                        D3DSURFACE_DESC dsc;
                        tf->d3d->GetLevelDesc(0, &dsc);
                        int sz = GetTextureFormatSize(dsc.Format);
                        int size = dsc.Width * dsc.Height * sz;
                        dbg_MemTexture += (size / 8);
                    }
                }
            }
		}
		sprintf(str,"tex=%i",dbg_MemTexture);
		OutText(10,40,str);

		dbg_MemTexture=0;
		std::vector<cFontInternal*>::iterator it;
		FOR_EACH(gb_VisGeneric->fonts,it)
		{
			cTexture* pTexture=(*it)->GetTexture();
			if(pTexture)
			for(int nFrame=0;nFrame<pTexture->GetNumberFrame();nFrame++) {
                IDirect3DTexture9* tex = pTexture->GetFrameImage(nFrame)->d3d;
                if (!tex) continue;
                
                D3DSURFACE_DESC dsc;
                tex->GetLevelDesc(0, &dsc);
                int sz = GetTextureFormatSize(dsc.Format);
                int size = dsc.Width * dsc.Height * sz;
                dbg_MemTexture += (size / 8);
            }
		}

		sprintf(str,"font_mem=%i",dbg_MemTexture);
		OutText(10,180,str);

        /*
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
        */
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
    
    int ret = cInterfaceRenderDevice::BeginScene();
    if (ret) return ret;
	
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
    SetRenderState(D3DRS_SHADEMODE,D3DSHADE_GOURAUD);
	SetRenderState(D3DRS_LASTPIXEL,TRUE);
    
    SetRenderState(D3DRS_ZENABLE,D3DZB_TRUE);
    SetRenderState(D3DRS_ALPHAFUNC,D3DCMP_GREATER);
    
    bool hicolor=(RenderMode&RENDERDEVICE_MODE_RGB16)?true:false;
    SetRenderState(D3DRS_DITHERENABLE,hicolor);    
	SetRenderState(D3DRS_SPECULARENABLE,FALSE);
	SetRenderState(D3DRS_DEPTHBIAS,0);
	SetRenderState(D3DRS_TEXTUREFACTOR,0xFFFFFFFF);

    SetRenderState(D3DRS_CLIPPING,TRUE);
    SetRenderState(D3DRS_SCISSORTESTENABLE,TRUE);
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
    
    D3DVIEWPORT9 vp = {0,0,ScreenSize.x,ScreenSize.y,0.0f,1.0f};
    RDCALL(lpD3DDevice->SetViewport(&vp));
    
	return hr;
}

int cD3DRender::EndScene()
{ 
	if(lpD3DDevice==0) return -1;
	if(!bActiveScene) return 1;
    
    FlushActiveDrawBuffer();

//	D3DVIEWPORT9 vpall={0,0,xScr,yScr,0.0f,1.0f};
//	RDCALL(lpD3DDevice->SetViewport(&vpall));

    int ret = cInterfaceRenderDevice::EndScene();
    if (ret) return ret;

	bActiveScene=0;
	HRESULT hr=lpD3DDevice->EndScene();
	return hr;
}

void cD3DRender::FlushPrimitive2D() {
    VISASSERT(bActiveScene);
    FlushActiveDrawBuffer();
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
	else {
        Vect3f v = Vect3f(0,0,1);
        memcpy(&GlobalLight.Direction.x, v, sizeof(GlobalLight.Direction));
    }
	
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
uint32_t cD3DRender::GetRenderState(eRenderStateOption option) {

    switch (option) {
        default:
            break;
        case RS_WIREFRAME:
            option = static_cast<eRenderStateOption>(D3DRS_FILLMODE);
            break;
        case RS_ALPHA_TEST_MODE:
            option = static_cast<eRenderStateOption>(D3DRS_ALPHAREF);
            break;
    }
    uint32_t value = GetRenderState(static_cast<D3DRENDERSTATETYPE>(option));
    switch (option) {
        default:
            break;
        case RS_ALPHA_TEST_MODE:
            if (GetRenderState(D3DRS_ALPHATESTENABLE)) {
                value = ALPHATEST_NONE;
            } else {
                switch (value) {
                    default:
                        value = ALPHATEST_GT_0;
                        break;
                    case 1:
                        value = ALPHATEST_GT_1;
                        break;
                    case 254:
                        value = ALPHATEST_GT_254;
                        break;
                }
            }
            break;
        case RS_ZENABLE:
            value = value != D3DZB_FALSE;
            break;
        case RS_WIREFRAME:
            value = value == D3DFILL_WIREFRAME;
            break;
        case RS_CULLMODE:
            switch (value) {
                default:
                    break;
                case D3DCULL_NONE:
                    value = CULL_NONE;
                    break;
                case D3DCULL_CW:
                    value = CULL_CW;
                    break;
                case D3DCULL_CCW:
                    value = CULL_CCW;
                    break;
            }
            break;
        case RS_ZFUNC:
            switch (value) {
                default:
                    break;
                case D3DCMP_LESSEQUAL:
                    value = CMP_LESSEQUAL;
                    break;
                case D3DCMP_GREATER:
                    value = CMP_GREATER;
                    break;
                case D3DCMP_GREATEREQUAL:
                    value = CMP_GREATEREQUAL;
                    break;
                case D3DCMP_ALWAYS:
                    value = CMP_ALWAYS;
                    break;
            }
            break;
    }
    return value;
}
int cD3DRender::SetRenderState(eRenderStateOption option,uint32_t value)
{ 
    if(lpD3DDevice==0) {
        xassert(0);
        return 1;
    }
	switch (option) {
        default:
            break;
        case RS_ALPHA_TEST_MODE:
            SetRenderState(D3DRS_ALPHATESTENABLE, value != ALPHATEST_NONE);
            option = static_cast<eRenderStateOption>(D3DRS_ALPHAREF);
            switch (value) {
                default:
                case ALPHATEST_GT_0:
                    value = 0;
                    break;
                case ALPHATEST_GT_1:
                    value = 1;
                    break;
                case ALPHATEST_GT_254:
                    value = 254;
                    break;
            }
            break;
        case RS_ZENABLE:
            value = value ? D3DZB_TRUE : D3DZB_FALSE;
            break;
		case RS_WIREFRAME:
            WireframeMode = value != 0;
            option = static_cast<eRenderStateOption>(D3DRS_FILLMODE);
            value = value ? D3DFILL_WIREFRAME : D3DFILL_SOLID;
			break;
		case RS_ZWRITEENABLE:
			if (value) {
                SetRenderState(RS_CULLMODE, CameraCullMode);
            } else {
                SetRenderState(RS_CULLMODE, CULL_NONE);
            }
			break;
		case RS_CULLMODE:
            switch (value) {
                default:
                    xassert(0);
                case CULL_CAMERA:
                    return SetRenderState(RS_CULLMODE, CameraCullMode);
                case CULL_NONE:
                    value = D3DCULL_NONE;
                    break;
                case CULL_CW:
                    value = D3DCULL_CW;
                    break;
                case CULL_CCW:
                    value = D3DCULL_CCW;
                    break;
            }
			break;
        case RS_ZFUNC:
            switch (value) {
                default:
                    break;
                case CMP_LESSEQUAL:
                    value = D3DCMP_LESSEQUAL;
                    break;
                case CMP_GREATER:
                    value = D3DCMP_GREATER;
                    break;
                case CMP_GREATEREQUAL:
                    value = D3DCMP_GREATEREQUAL;
                    break;
                case CMP_ALWAYS:
                    value = D3DCMP_ALWAYS;
                    break;
            }
            break;
		case RS_BILINEAR:
            FlushActiveDrawBuffer();
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
	SetRenderState(static_cast<D3DRENDERSTATETYPE>(option),value);
	return 0; 
}

void cD3DRender::FlushPrimitive3D() {
	VISASSERT(bActiveScene);
    FlushActiveDrawBuffer();
}

void cD3DRender::OutText(int x,int y,const char *string,int r,int g,int b,char *FontName,int size,int bold,int italic,int underline)
{
    if(hWnd==0) return;
#ifdef _WIN32
	HDC hDC=0;
    HFONT hFont=CreateFontA(size,0,0,0,bold?FW_BOLD:FW_NORMAL,italic,underline,0, ANSI_CHARSET,
			OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,VARIABLE_PITCH,FontName);
	if(hFont==0) return;

	if((hDC=GetDC(hWnd))==0) return;
    HFONT hFontOld=(HFONT)SelectObject( hDC, hFont );
    SetTextColor(hDC,RGB(r,g,b));
	SetBkMode(hDC,TRANSPARENT);
	RECT rect={xScrMin,yScrMin,xScrMax,yScrMax};
	RDCALL(!ExtTextOutA(hDC,x,y,ETO_CLIPPED,&rect,string,lstrlenA(string),0));
	SelectObject(hDC,hFontOld);
	DeleteObject(hFont);
	ReleaseDC(hWnd,hDC);
#endif
}

void cD3DRender::OutText(int x,int y,const char *string,int r,int g,int b)
{
	if(hWnd==0) return;
#ifdef _WIN32
	HDC hDC=GetDC(hWnd);
	if(hDC==0) return;
	SetTextColor(hDC,RGB(r,g,b));
	SetBkMode(hDC,TRANSPARENT);
	RECT rect={xScrMin,yScrMin,xScrMax,yScrMax};
	RDCALL(!ExtTextOutA(hDC,x,y,ETO_CLIPPED,&rect,string,lstrlenA(string),0));
	ReleaseDC(hWnd,hDC);
#endif
}

int cD3DRender::SetGamma(float fGamma,float fStart,float fFinish)
{
	D3DGAMMARAMP gamma;
	if(fGamma<=0) fGamma=1.f; else fGamma=1/fGamma;
    for(int i=0;i<256;i++)
    {
		int dwGamma= xm::round(65536 * (fStart + (fFinish - fStart) * xm::pow(i / 255.f, fGamma)));
		if(dwGamma<0) dwGamma=0; else if(dwGamma>65535) dwGamma=65535;
        gamma.red[i]=gamma.green[i]=gamma.blue[i]=dwGamma;
    }
	lpD3DDevice->SetGammaRamp(0,D3DSGR_NO_CALIBRATION,&gamma);
	return 0;
}

void cD3DRender::DeleteDynamicBuffers() {
    MTG();
    for (auto vb : LibVB) {
        if (vb && vb->d3d) {
            auto d3d = vb->d3d;
            vb->d3d = nullptr;
            d3d->Release();
        }
    }
    LibVB.clear();
    for (auto ib : LibIB) {
        if (ib && ib->d3d) {
            auto d3d = ib->d3d;
            ib->d3d = nullptr;
            d3d->Release();
        }
    }
    LibIB.clear();
}

const uint32_t D3D_LOCK_FLAGS_STATIC = D3DLOCK_NOSYSLOCK | D3DLOCK_NO_DIRTY_UPDATE;
const uint32_t D3D_LOCK_FLAGS_DYNAMIC = D3D_LOCK_FLAGS_STATIC | D3DLOCK_DISCARD;

void cD3DRender::UpdateD3DVertexBuffer(VertexBuffer* vb, size_t len) {
    xassert(!vb->burned);
    vb->burned = true;
    xassert(len <= vb->data_len);
    if (!vb->d3d) {
        MTG();
        uint32_t fvf = GetD3DFVFFromFormat(vb->fmt);
        uint32_t flags = D3DUSAGE_WRITEONLY;
        if (vb->dynamic) {
            flags |= D3DUSAGE_DYNAMIC;
        }

        RDCALL(lpD3DDevice->CreateVertexBuffer(
                vb->dynamic ? vb->data_len : len,
                flags,
                fvf,
                vb->dynamic ? D3DPOOL_DEFAULT : D3DPOOL_MANAGED,
                &vb->d3d,
                nullptr
        ));
        xassert(vb->d3d);
        
        if (vb->dynamic) {
            LibVB.emplace_back(vb);
        }
    }
    VISASSERT( vb->d3d );
    vb->dirty = false;
    void* lock_ptr = nullptr;
    uint32_t flags = vb->dynamic ? D3D_LOCK_FLAGS_DYNAMIC : D3D_LOCK_FLAGS_STATIC;
    RDCALL(vb->d3d->Lock(0, len, &lock_ptr, flags));
    memcpy(lock_ptr, vb->data, len);
    vb->d3d->Unlock();
}

void cD3DRender::UpdateD3DIndexBuffer(IndexBuffer* ib, size_t len) {
    xassert(!ib->burned);
    ib->burned = true;
    xassert(len <= ib->data_len);
    if (!ib->d3d) {
        MTG();
        uint32_t flags = D3DUSAGE_WRITEONLY;
        if (ib->dynamic) {
            flags |= D3DUSAGE_DYNAMIC;
        }

        RDCALL(lpD3DDevice->CreateIndexBuffer(
                ib->dynamic ? ib->data_len : len,
                flags,
                D3DFMT_INDEX16,
                ib->dynamic ? D3DPOOL_DEFAULT : D3DPOOL_MANAGED,
                &ib->d3d,
                nullptr
        ));
        xassert(ib->d3d);
        
        if (ib->dynamic) {
            LibIB.emplace_back(ib);
        }
    }
    VISASSERT( ib->d3d );
    ib->dirty = false;
    void* lock_ptr = nullptr;
    uint32_t flags = ib->dynamic ? D3D_LOCK_FLAGS_DYNAMIC : D3D_LOCK_FLAGS_STATIC;
    RDCALL(ib->d3d->Lock(0, len, &lock_ptr, flags));
    memcpy(lock_ptr, ib->data, len);
    ib->d3d->Unlock();
}

void cD3DRender::DeleteVertexBuffer(VertexBuffer &vb) {
#ifdef PERIMETER_RENDER_TRACKER_RESOURCES
    RenderSubmitEvent(RenderEvent::DELETE_VERTEXBUF, "", &vb);
#endif
    if (vb.d3d) {
        MTG();
        if (vb.dynamic) {
            auto removed = std::remove(LibVB.begin(), LibVB.end(), &vb);
            LibVB.erase(removed, LibVB.end());
        }
        vb.d3d->Release();
        vb.d3d = nullptr;
    }
    cInterfaceRenderDevice::DeleteVertexBuffer(vb);
}

void cD3DRender::DeleteIndexBuffer(IndexBuffer &ib) {
#ifdef PERIMETER_RENDER_TRACKER_RESOURCES
    RenderSubmitEvent(RenderEvent::DELETE_INDEXBUF, "", &ib);
#endif
    if (ib.d3d) {
        MTG();
        if (ib.dynamic) {
            auto removed = std::remove(LibIB.begin(), LibIB.end(), &ib);
            LibIB.erase(removed, LibIB.end());
        }
        ib.d3d->Release();
        ib.d3d = nullptr;
    }
    cInterfaceRenderDevice::DeleteIndexBuffer(ib);
}

void cD3DRender::FlushActiveDrawBuffer() {
    if (activeDrawBuffer) {
        DrawBuffer* db = activeDrawBuffer;
        activeDrawBuffer = nullptr;
        if (db->IsLocked()) {
            db->Unlock();
        }
        db->Draw();
    }
}

void cD3DRender::SetActiveDrawBuffer(DrawBuffer* db) {
    //printf("%p -> %p\n", activeDrawBuffer, db);
    if (activeDrawBuffer && activeDrawBuffer != db) {
        FlushActiveDrawBuffer();
    }
    cInterfaceRenderDevice::SetActiveDrawBuffer(db);
}

void cD3DRender::SubmitDrawBuffer(DrawBuffer* db, DrawRange* range) {
    if (activeDrawBuffer) {
        if (activeDrawBuffer == db) {
            //Avoid drawing twice
            activeDrawBuffer = nullptr;
        } else {
            FlushActiveDrawBuffer();
        }
    }
    if (!db->written_vertices) {
        xassert(0);
        return;
    }
    
    SubmitBuffers(db->primitive, &db->vb, db->written_vertices, &db->ib, db->written_indices, range);
    
    db->PostDraw();
}

void cD3DRender::SubmitBuffers(ePrimitiveType primitive, VertexBuffer* vb, size_t vertices, IndexBuffer* ib, size_t indices, DrawRange* range) {
    if (vb->dirty) {
        UpdateD3DVertexBuffer(vb, vertices * vb->VertexSize);
    } else if (!vb->d3d) {
        xassert(0);
        return;
    }

    if (0 < indices && ib->dirty) {
        UpdateD3DIndexBuffer(ib, indices * sizeof(indices_t));
    }

    uint32_t fvf = GetD3DFVFFromFormat(vb->fmt);
    RDCALL(lpD3DDevice->SetFVF(fvf));
    RDCALL(lpD3DDevice->SetStreamSource(0, vb->d3d, 0, vb->VertexSize));
    D3DPRIMITIVETYPE d3dType;
    switch (primitive) {
        default:
        case PT_TRIANGLESTRIP:
            d3dType = D3DPT_TRIANGLESTRIP;
            break;
        case PT_TRIANGLES:
            d3dType = D3DPT_TRIANGLELIST;
            break;
    }
    
    size_t offset = range ? range->offset : 0;
    if (indices) {
        if (range) {
            xassert((range->offset + range->len) <= indices);
            indices = range->len;
        }
        RDCALL(lpD3DDevice->SetIndices(ib->d3d));
        size_t polys = (primitive == PT_TRIANGLESTRIP
                        ? (indices - 2)
                        : static_cast<size_t>(xm::floor(static_cast<double>(indices) / sPolygon::PN)));
        RDCALL(gb_RenderDevice3D->lpD3DDevice->DrawIndexedPrimitive(
                d3dType,
                0, 0, vertices,
                offset, polys
        ));
        NumberPolygon += polys;
    } else {
        if (range) {
            xassert((range->offset + range->len) <= vertices);
            vertices = range->len;
        }
        vertices = range ? range->len : vertices;
        xassert(0);
        RDCALL(gb_RenderDevice3D->lpD3DDevice->DrawPrimitive(d3dType, offset, vertices));
    }
    if (vb->dynamic) vb->burned = false;
    if (ib->dynamic) ib->burned = false;
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
	if (lpD3DDevice== nullptr) return 1;

	RELEASE(lpZBuffer);
	RELEASE(lpBackBuffer);
	dtFixed->DeleteShadowTexture();
	dtAdvance->DeleteShadowTexture();
	if(dtAdvanceOriginal)
		dtAdvanceOriginal->DeleteShadowTexture();

	for(int i=0;i<TexLibrary->GetNumberTexture();i++) {
		cTexture* pTexture=TexLibrary->GetTexture(i);
		if(pTexture&&pTexture->GetAttribute(TEXTURE_POOL_DEFAULT)) {
            DeleteTexture(pTexture);
        }
	}
//	IsDeleteAllDefaultTextures();

    DeleteDynamicBuffers();
	DeleteShader();
		
	ClearTilemapPool();
	//vertex_pool_manager.Clear();
	//index_pool_manager.Clear();
	return 0;
}

bool cD3DRender::SetFocus(bool wait,bool focus_error)
{
    MTG();
	{
        HRESULT hrescoop=D3D_OK;
        HRESULT hres=D3D_OK;
		int imax=wait?10:1;
		for(int i=0;i<imax;i++)
		{
			hres=lpD3DDevice->Reset(&d3dpp);
			if (FAILED(hres)) {
                hrescoop=lpD3DDevice->TestCooperativeLevel();
                fprintf(stderr, "D3D::SetFocus coop level after reset %s\n", GetErrorText(hrescoop));
                if (wait) {
                    Sleep(1000);
                }
            }
		}
		if (FAILED(hres)) {
            fprintf(stderr, "D3D::SetFocus failed reset %s\n", GetErrorText(hres));
			if(focus_error)
			{
				VisError<<"Internal error. Cannot reinitialize graphics"<<VERR_END;
			}
			return false;
		}
	}

	RDCALL(lpD3DDevice->EvictManagedResources());
	 
	for(int i=0;i<TexLibrary->GetNumberTexture();i++)
	{
		cTexture* pTexture=TexLibrary->GetTexture(i);
		if(pTexture && pTexture->GetAttribute(TEXTURE_POOL_DEFAULT)) {
			if (pTexture->GetRef()>1) {
                CreateTexture(pTexture, 0);
            }
		}
	}

	RELEASE(lpZBuffer);
	RELEASE(lpBackBuffer);

	RDCALL(lpD3DDevice->GetDepthStencilSurface(&lpZBuffer));
	RDCALL(lpD3DDevice->GetRenderTarget(0,&lpBackBuffer)); 

	RestoreShader();
	RestoreTilemapPool();

	return true;
}

void cD3DRender::DeleteShader()
{
	std::vector<cShader*>::iterator it;
	FOR_EACH(cShader::all_shader,it)
		(*it)->Delete();
}

void cD3DRender::RestoreShader()
{
	std::vector<cShader*>::iterator it;
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
	SetTexture(0, Texture0, Phase);
	SetTexture(1, Texture1, Phase);
	SetTextureImage(2, nullptr);

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

void cD3DRender::SaveStates(const char* fname)
{
	FILE* f=fopen(convert_path_content(fname).c_str(),"wt");
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

void cD3DRender::UseOrthographicProjection() {
    if (isOrthographicProjSet) return;
    FlushActiveDrawBuffer();
    isOrthographicProjSet = true;
    RDCALL(lpD3DDevice->SetTransform(D3DTS_WORLD, reinterpret_cast<const D3DMATRIX*>(&Mat4f::ID)));
    RDCALL(lpD3DDevice->SetTransform(D3DTS_VIEW, reinterpret_cast<const D3DMATRIX*>(&Mat4f::ID)));
    RDCALL(lpD3DDevice->SetTransform(D3DTS_PROJECTION, reinterpret_cast<const D3DMATRIX*>(&orthoVP)));
    //SetDrawTransform may have changed viewport, set screen viewport just in case
    D3DVIEWPORT9 vp={0,0,ScreenSize.x,ScreenSize.y,0.0f,1.0f};
    RDCALL(lpD3DDevice->SetViewport(&vp));
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

#ifndef _FINAL_VERSION_
void IsDeleteAllDefaultTextures()
{
	int num_undeleted=0,num_uncorrect=0;
	for(cCheckDelete* cur=cCheckDelete::GetDebugRoot();cur;cur=cur->GetDebugNext())
	{
		cTexture* p=dynamic_cast<cTexture*>(cur);
		if(p)
		{
			if(p->GetAttribute(TEXTURE_POOL_DEFAULT))
			{
				for(int nFrame=0;nFrame<p->GetNumberFrame();nFrame++)
				if(p->GetFrameImage(nFrame)) 
				{
					dprintf("D3DPOOL_DEFAULT size=(%i,%i)",p->GetWidth(),p->GetHeight());
					num_undeleted++;
				}
			}else
			{
				for(int nFrame=0;nFrame<p->GetNumberFrame();nFrame++)
				if(p->GetFrameImage(nFrame)) 
				{
					IDirect3DTexture9* surface=p->GetFrameImage(nFrame);
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

