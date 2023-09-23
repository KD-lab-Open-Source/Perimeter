#include "StdAfxRD.h"
#include "D3DRender.h"
#include "FileImage.h"
#include "files/files.h"
#include "SystemUtil.h"

LPDIRECT3DTEXTURE9 cD3DRender::CreateSurface(int x, int y, eSurfaceFormat TextureFormat, int MipMap, bool enable_assert, uint32_t attribute)
{
	LPDIRECT3DTEXTURE9 lpTexture=0;

#ifndef PERIMETER_EXODUS //We don't support DXT textures in our D3DXLoadSurfaceFromMemory
	if((TextureFormat==SURFMT_COLOR || TextureFormat==SURFMT_COLORALPHA || TextureFormat==SURFMT_GRAYALPHA) && Option_FavoriteLoadDDS)
	{
		if(TextureFormat==SURFMT_GRAYALPHA)
		{
			RDCALL(lpD3DDevice->CreateTexture(x,y,MipMap,0,
				D3DFMT_DXT3,
				D3DPOOL_MANAGED,&lpTexture,NULL));
		}else
		{
			RDCALL(lpD3DDevice->CreateTexture(x,y,MipMap,0,
				(TextureFormat==SURFMT_COLOR)?D3DFMT_DXT1:D3DFMT_DXT5,
				D3DPOOL_MANAGED,&lpTexture,NULL));
		}
		VISASSERT(lpTexture);
		return lpTexture;
	}
#endif

	VISASSERT(x&&y&&TextureFormat>=0&&TextureFormat<SURFMT_NUMBER);
	int Usage=0;
	D3DPOOL Pool=D3DPOOL_MANAGED;
	if(TextureFormat==SURFMT_RENDERMAP16 || TextureFormat==SURFMT_RENDERMAP32 || TextureFormat==SURFMT_RENDERMAP_FLOAT)
	{
		Usage=D3DUSAGE_RENDERTARGET;
		Pool=D3DPOOL_DEFAULT;
	}

	if(attribute&TEXTURE_POOL_DEFAULT)
	{
		Pool=D3DPOOL_DEFAULT;
	}

    D3DFORMAT fmt = static_cast<D3DFORMAT>(TexFmtData[TextureFormat].FormatID);
	HRESULT hr=lpD3DDevice->CreateTexture(x, y, MipMap, Usage, fmt, Pool, &lpTexture, NULL);
	if(FAILED(hr))
	{
		if(enable_assert)
		{
			VISASSERT(0 && "Cannot create texture");
		}
		return NULL;
	}
	VISASSERT(lpTexture);
	return lpTexture;
}

int cD3DRender::CreateTexture(class cTexture *Texture,class cFileImage *FileImage,bool enable_assert)
{ // только создает в памяти поверхности
#ifdef PERIMETER_RENDER_TRACKER_RESOURCES
    RenderSubmitEvent(RenderEvent::CREATE_TEXTURE, "", Texture);
#endif
	sTextureFormatData &tfd = TexFmtData[Texture->GetFmt()];
	if(Texture->GetX()==0||tfd.FormatID==0) return 1;

	if(Texture->GetWidth()>dwSuportMaxSizeTextureX) Texture->SetWidth(dwSuportMaxSizeTextureX);
	if(Texture->GetHeight()>dwSuportMaxSizeTextureY) Texture->SetHeight(dwSuportMaxSizeTextureY);
	
	int dx=Texture->GetWidth();
    int dy=Texture->GetHeight();
    size_t tex_len = dx * dy * 4;


	VISASSERT((Texture->GetNumberMipMap()>=1));
	uint32_t dither= (RenderMode & RENDERDEVICE_MODE_RGB16) ? D3DX_FILTER_DITHER : 0;
	bool is_alpha_test=false;
	bool is_alpha_blend=false;
	bool is_skin=Texture->skin_color.a==255;

	for(int i=0;i<Texture->GetNumberFrame();i++) {
        IDirect3DTexture9*& tex = Texture->GetFrameImage(i)->d3d;
		if (tex) tex->Release(); 
		if (tex == nullptr) {
            tex = CreateSurface(
                    dx, dy,
                    Texture->GetFmt(), Texture->GetNumberMipMap(), enable_assert,
                    Texture->GetAttribute()
            );
        }
		if (tex == nullptr) return 2;
		if (FileImage == nullptr) continue;

		uint8_t* lpBuf = new uint8_t[tex_len];
		memset(lpBuf, 0xFF, tex_len);
        //Load in BGRA
		FileImage->GetTextureRGB(lpBuf,i*Texture->GetTimePerFrame(),4,4*dx,
                                 8,8,8, 16,8,0, dx, dy );

		if(Texture->IsAlpha() || Texture->IsAlphaTest())// загрузка только прозрачности
		{
            uint32_t* buf32 = reinterpret_cast<uint32_t*>(lpBuf);
			FileImage->GetTextureAlpha(lpBuf,i*Texture->GetTimePerFrame(),4,4*dx,
				8, 24, dx, dy );
            
            // We need to check alpha color bytes
            bool finished_early = false;
            for (size_t n = 3; n < tex_len; n += 4) {
                uint8_t alpha = lpBuf[n];
                if (alpha != 255 && alpha != 0) {
                    finished_early = true;
                }
            }

            if (finished_early) {
                is_alpha_blend = true;
            } else {
                is_alpha_test = true;
            }
		}

		if(is_skin)
		{
			ApplySkinColor(lpBuf,dx,dy,Texture->skin_color);
		}

        //We need to convert grayscale bumpmap to normalmap
        if(Texture->GetAttribute(TEXTURE_BUMP) && !Texture->GetAttribute(TEXTURE_NORMAL)) {
            Texture->ConvertBumpToNormal(lpBuf);
        }

		RECT rect={0,0,dx,dy};

		LPDIRECT3DTEXTURE9& lpD3DTexture=Texture->GetFrameImage(i)->d3d;
		LPDIRECT3DSURFACE9 lpSurface = NULL;
		RDCALL( lpD3DTexture->GetSurfaceLevel( 0, &lpSurface ) );

		RECT rect_out={0,0,dx,dy};
		RDCALL( D3DXLoadSurfaceFromMemory( lpSurface, NULL, &rect_out,
			lpBuf, D3DFMT_A8R8G8B8, 4*rect.right, NULL, &rect, 
			D3DX_FILTER_POINT|dither
			, 0 ));

		if(Texture->GetNumberMipMap()>1) // построение мип мапов
			for(int nMipMap=1;nMipMap<Texture->GetNumberMipMap();nMipMap++)
			{
				LPDIRECT3DSURFACE9 lpSurfaceNext = NULL;
				RDCALL( lpD3DTexture->GetSurfaceLevel( nMipMap, &lpSurfaceNext ) );
				RECT rect={0,0,dx>>nMipMap,dy>>nMipMap};
				uint8_t *lpBufNext = new uint8_t[rect.right * rect.bottom * 4];

				{
					BuildMipMap( rect.right,rect.bottom,4, 8*rect.right,lpBuf, 4*rect.right,lpBufNext, 
						8,8,8,8, 16,8,0,24, Texture->GetAttribute(TEXTURE_MIPMAP_POINT|TEXTURE_MIPMAPBLUR|TEXTURE_BLURWHITE));
					RDCALL( D3DXLoadSurfaceFromMemory( lpSurfaceNext, NULL, &rect,
						lpBufNext, D3DFMT_A8R8G8B8, 4*rect.right, NULL, &rect, 
						D3DX_FILTER_POINT|dither
						, 0 ));
				}
				delete[] lpBuf; lpSurface->Release();
				lpBuf = lpBufNext; lpSurface = lpSurfaceNext;
			}
		lpSurface->Release();
		delete[] lpBuf;
	}

	if(is_skin)
	{
		Texture->ClearAttribute(TEXTURE_ALPHA_BLEND|TEXTURE_ALPHA_TEST);
	}else
	if(is_alpha_test && !is_alpha_blend)
	{
		Texture->ClearAttribute(TEXTURE_BLURWHITE|TEXTURE_MIPMAPBLUR|TEXTURE_ALPHA_BLEND);
		Texture->SetAttribute(TEXTURE_MIPMAP_POINT|TEXTURE_ALPHA_TEST);
	}
    return 0;
}

int cD3DRender::DeleteTexture(cTexture *Texture)
{ // только освобождает в памяти поверхности 
#ifdef PERIMETER_RENDER_TRACKER_RESOURCES
    RenderSubmitEvent(RenderEvent::DELETE_TEXTURE, "", Texture);
#endif
	for(int nFrame=0;nFrame<Texture->GetNumberFrame();nFrame++) {
        IDirect3DTexture9*& tex = Texture->GetFrameImage(nFrame)->d3d;
        if (tex) {
            tex->Release();
            tex = nullptr;
        }
    }
	return 0;
}
bool cD3DRender::SetScreenShot(const char *fname)
{
#ifdef _WIN32
	LPDIRECT3DSURFACE9 lpRenderSurface=0;
	RDCALL(lpD3DDevice->GetRenderTarget(0,&lpRenderSurface));
	HRESULT hr=D3DXSaveSurfaceToFileA(fname,D3DXIFF_BMP,lpRenderSurface,NULL,NULL);
	
	RELEASE(lpRenderSurface);
	return SUCCEEDED(hr);
#else
    //TODO implement this
    fprintf(stderr, "Called unimplemented cD3DRender::SetScreenShot\n");
    return false;
#endif
}

void* cD3DRender::LockTexture(cTexture* Texture, int& Pitch)
{
#ifdef PERIMETER_RENDER_TRACKER_LOCKS
    RenderSubmitEvent(RenderEvent::LOCK_TEXTURE);
#endif
	D3DLOCKED_RECT d3dLockRect;
	IDirect3DTexture9*& lpSurface=Texture->GetFrameImage(0)->d3d;
	RDCALL(lpSurface->LockRect(0,&d3dLockRect,nullptr,0));

	Pitch=d3dLockRect.Pitch;
	return d3dLockRect.pBits;
}

void* cD3DRender::LockTextureRect(cTexture* Texture, int& Pitch, Vect2i pos, Vect2i size)
{
#ifdef PERIMETER_RENDER_TRACKER_LOCKS
    RenderSubmitEvent(RenderEvent::LOCK_TEXTURE_RECT);
#endif
    D3DLOCKED_RECT d3dLockRect;
    IDirect3DTexture9* lpSurface=Texture->GetFrameImage(0)->d3d;
    RECT rc;
    rc.left=pos.x;
    rc.top=pos.y;
    rc.right=pos.x+size.x;
    rc.bottom=pos.y+size.y;
    RDCALL(lpSurface->LockRect(0,&d3dLockRect,&rc,0));

    Pitch=d3dLockRect.Pitch;
    return d3dLockRect.pBits;
}

void cD3DRender::UnlockTexture(class cTexture *Texture)
{
#ifdef PERIMETER_RENDER_TRACKER_LOCKS
    RenderSubmitEvent(RenderEvent::UNLOCK_TEXTURE);
#endif
	IDirect3DTexture9*& lpSurface=Texture->GetFrameImage(0)->d3d;
	RDCALL(lpSurface->UnlockRect(0));
}

void cD3DRender::SetTextureImage(uint32_t slot, TextureImage* texture_image) {
    VISASSERT(slot<GetMaxTextureSlots());
    IDirect3DBaseTexture9* pTexture = texture_image ? texture_image->d3d : nullptr;
    if(CurrentTexture[slot]!=pTexture) {
        FlushActiveDrawBuffer();
        RDCALL(lpD3DDevice->SetTexture(slot, CurrentTexture[slot] = pTexture));
    }
}
