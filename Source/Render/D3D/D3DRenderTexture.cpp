#include "StdAfxRD.h"
#include "FileImage.h"
#include "files/files.h"
#include "SystemUtil.h"

LPDIRECT3DTEXTURE9 cD3DRender::CreateSurface(int x, int y, eSurfaceFormat TextureFormat, int MipMap, bool enable_assert, uint32_t attribute)
{
	LPDIRECT3DTEXTURE9 lpTexture=0;

#ifndef PERIMETER_EXODUS //TODO We don't support DXT textures yet
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

	if(attribute&TEXTURE_D3DPOOL_DEFAULT)
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

inline uint8_t ByteInterpolate(uint8_t a, uint8_t b, uint8_t factor)
{
//	float f=factor/255.0f;
//	return xm::round(a+(int(b-a))*f);
	return a+(int(b-a))*int(factor)/255;
}

void ApplySkinColor(uint32_t* buffer,int dx,int dy,sColor4c skin_color)
{
	uint32_t* cur=buffer;
	for(int y=0;y<dy;y++)
	{
		for(int x=0;x<dx;x++,cur++)
		{
			sColor4c& c=*(sColor4c*)cur;
			c.r=ByteInterpolate(c.r,skin_color.r,c.a);
			c.g=ByteInterpolate(c.g,skin_color.g,c.a);
			c.b=ByteInterpolate(c.b,skin_color.b,c.a);
			c.a=255;
		}
	}
}

int cD3DRender::CreateTexture(class cTexture *Texture,class cFileImage *FileImage,bool enable_assert)
{ // только создает в памяти поверхности 
	sTextureFormatData &tfd = TexFmtData[Texture->GetFmt()];
	if(Texture->GetX()==0||tfd.FormatID==0) return 1;

	if(Texture->GetWidth()>dwSuportMaxSizeTextureX) Texture->SetWidth(dwSuportMaxSizeTextureX);
	if(Texture->GetHeight()>dwSuportMaxSizeTextureY) Texture->SetHeight(dwSuportMaxSizeTextureY);
	
	int dx=Texture->GetWidth(),dy=Texture->GetHeight();
	int dxy=dx*dy;


	VISASSERT((Texture->GetNumberMipMap()>=1));
	uint32_t dither= (RenderMode & RENDERDEVICE_MODE_RGB16) ? D3DX_FILTER_DITHER : 0;
	bool is_alpha_test=false;
	bool is_alpha_blend=false;
	bool is_skin=Texture->skin_color.a==255;

	for(int i=0;i<Texture->GetNumberFrame();i++)
	{
        IDirect3DTexture9*& tex = Texture->GetFrameImage(i).d3d;
		if (tex) tex->Release(); 
		if (tex == nullptr) {
            tex = CreateSurface(
                    dx, dy,
                    Texture->GetFmt(), Texture->GetNumberMipMap(), enable_assert,
                    Texture->GetAttribute()
            );
        }
		if(tex == nullptr) return 2;
		if(FileImage==0) continue;

		uint32_t* lpBuf = new uint32_t[dxy];
		memset(lpBuf,0xFF,dxy*sizeof(lpBuf[0]));
		FileImage->GetTexture(lpBuf,i*Texture->GetTimePerFrame(),4,4*dx,
			8,8,8,8, 16,8,0,24, dx, dy );
		if(Texture->IsAlpha() || Texture->IsAlphaTest())// загрузка только прозрачности
		{
			FileImage->GetTextureAlpha(lpBuf,i*Texture->GetTimePerFrame(),4,4*dx,
				8, 24, dx, dy );
			int n;
			for(n=0; n<dxy; n++ )
				if( (lpBuf[n]>>24)!=255 && (lpBuf[n]>>24)!=0 )
					break;
			if( n>=dxy )
				is_alpha_test=true;
			else
				is_alpha_blend=true;
		}

		if(is_skin)
		{
			ApplySkinColor(lpBuf,dx,dy,Texture->skin_color);
		}

        //We need to convert grayscale bumpmap to normalmap
		if(Texture->GetAttribute(TEXTURE_BUMP) && !Texture->GetAttribute(TEXTURE_NORMAL))
		{
            //Get key of cache for this texture file and the file to store/retrieve the cache
            std::string key;
            filesystem_entry* entry = get_content_entry(Texture->GetName());
            if (entry) key = entry->key;
            if (key.empty()) key = convert_path_native(Texture->GetName());
            key = string_to_lower(key.c_str());
            string_replace_all(key, PATH_SEP_STR, "_");
            key = std::string("cache") + PATH_SEP + "bump" + PATH_SEP + key +  ".bin";
            std::string path = convert_path_content(key, true);
            xassert(!path.empty());

            //Attempt to use cache since it takes some time to convert big files
            int64_t len = static_cast<int64_t>(dxy * sizeof(uint32_t));
            XStream ff(0);
            bool usable = !path.empty() && ff.open(path, XS_IN) && 0 < ff.size();
            if (usable) {
                std::string sourcefile = convert_path_content(Texture->GetName());
                //If empty then the texture was loaded from .pak so assume is older than cache
                if (!sourcefile.empty()) {
                    auto cachetime = std::filesystem::last_write_time(std::filesystem::u8path(path));
                    auto sourcetime = std::filesystem::last_write_time(std::filesystem::u8path(sourcefile));
                    //Mark as not usable if source file is modified after cache creation
                    usable = sourcetime <= cachetime;
                }
            }
            if (usable) {
                //Load file and uncompress it
                XBuffer buf(ff.size(), true);
                XBuffer dest = XBuffer(len, true);
                ff.read(buf.address(), ff.size());
                if (buf.uncompress(dest) == 0 && dest.tell() == len) {
                    memcpy(lpBuf, dest.address(), len);
                } else {
                    usable = false;
                }
            }
            
            if (!usable) {
                //Convert grayscale bump map into normal map and cache it
                ConvertDot3(lpBuf, dx, dy, 1.0e-2f * Texture->bump_scale);
                if (ff.open(path, XS_OUT)) {
                    XBuffer buf(lpBuf, len);
                    buf.set(len);
                    XBuffer compressed(len, true);
                    if (buf.compress(compressed) == 0) {
                        ff.write(compressed, compressed.tell());
                    }
                }
            }
            ff.close();            
		}

		RECT rect={0,0,dx,dy};

		LPDIRECT3DTEXTURE9& lpD3DTexture=Texture->GetFrameImage(i).d3d;
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
				unsigned int *lpBufNext = new unsigned int [rect.right*rect.bottom];

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
	for(int nFrame=0;nFrame<Texture->GetNumberFrame();nFrame++) {
        IDirect3DTexture9*& tex = Texture->GetFrameImage(nFrame).d3d;
        if (tex) {
            tex->Release();
            tex = nullptr;
        }
    }
	return 0;
}
bool cD3DRender::SetScreenShot(const char *fname)
{
#ifndef PERIMETER_EXODUS
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

void* cD3DRender::LockTexture(class cTexture *Texture, int& Pitch)
{
	D3DLOCKED_RECT d3dLockRect;
	IDirect3DTexture9*& lpSurface=Texture->GetFrameImage(0).d3d;
	RDCALL(lpSurface->LockRect(0,&d3dLockRect,0,0));

	Pitch=d3dLockRect.Pitch;
	return d3dLockRect.pBits;
}

void cD3DRender::UnlockTexture(class cTexture *Texture)
{
	IDirect3DTexture9*& lpSurface=Texture->GetFrameImage(0).d3d;
	RDCALL(lpSurface->UnlockRect(0));
}

void cD3DRender::ConvertDot3(uint32_t* ibuf,int dx,int dy,float h_mul)
{
	const int byte_per_pixel=4;
	uint8_t* buf=(uint8_t*)ibuf;
#define GET(x,y) buf[(clamp(x,0,dx-1)+dx*clamp(y,0,dy-1))*byte_per_pixel]
    uint32_t* out=new uint32_t[dx*dy];

	for(int y=0;y<dy;y++)
	for(int x=0;x<dx;x++)
	{
		Vect3f dl,dn,dsum(0,0,0);
		int h00=GET(x,y);
		int h01=GET(x+1,y);
		int h0m=GET(x-1,y);
		int h10=GET(x,y+1);
		int hm0=GET(x,y-1);

		dl.set(0,1,(h01-h00)*h_mul);
		dn.set(0,-dl.z,dl.y);
		dsum+=dn.normalize();

		dl.set(0,1,(h00-h0m)*h_mul);
		dn.set(0,-dl.z,dl.y);
		dsum+=dn.normalize();

		dl.set(1,0,(h10-h00)*h_mul);
		dn.set(-dl.z,0,dl.x);
		dsum+=dn.normalize();

		dl.set(1,0,(h00-hm0)*h_mul);
		dn.set(-dl.z,0,dl.x);
		dsum+=dn.normalize();

		out[x+y*dx]=ColorByNormal(dsum.normalize());
	}

	memcpy(buf,out,dx*dy*byte_per_pixel);
	delete[] out;
#undef GET
}
