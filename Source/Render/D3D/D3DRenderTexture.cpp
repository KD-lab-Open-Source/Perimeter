#include "StdAfxRD.h"
#include "FileImage.h"

LPDIRECT3DTEXTURE9 cD3DRender::CreateSurface(int x,int y,eSurfaceFormat TextureFormat,int MipMap,bool enable_assert,DWORD attribute)
{
	LPDIRECT3DTEXTURE9 lpTexture=0;

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

	HRESULT hr=lpD3DDevice->CreateTexture(x,y,MipMap,Usage,TexFmtData[TextureFormat].TexFmtD3D,Pool,&lpTexture,NULL);
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

inline BYTE ByteInterpolate(BYTE a,BYTE b,BYTE factor)
{
//	float f=factor/255.0f;
//	return round(a+(int(b-a))*f);
	return a+(int(b-a))*int(factor)/255;
}

void ApplySkinColor(DWORD* buffer,int dx,int dy,sColor4c skin_color)
{
	DWORD* cur=buffer;
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

int cD3DRender::CreateTexture(class cTexture *Texture,class cFileImage *FileImage,int dxout,int dyout,bool enable_assert)
{ // только создает в памяти поверхности 
	sTextureFormatData &tfd = TexFmtData[Texture->GetFmt()];
	if(Texture->GetX()==0||tfd.TexFmtD3D==0) return 1;

	if(Texture->GetWidth()>dwSuportMaxSizeTextureX) Texture->SetWidth(dwSuportMaxSizeTextureX);
	if(Texture->GetHeight()>dwSuportMaxSizeTextureY) Texture->SetHeight(dwSuportMaxSizeTextureY);
	
	int dx=Texture->GetWidth(),dy=Texture->GetHeight();
	int dxy=dx*dy;

	int dx_surface,dy_surface;

	if(dxout<0)
	{
		dx_surface=dxout=dx;
	}else
	{
		dx_surface=Power2up(dxout);
	}

	if(dyout<0)
	{
		dy_surface=dyout=dy;
	}else
	{
		dy_surface=Power2up(dyout);
	}


	VISASSERT((dx==dxout && dy==dyout)||(Texture->GetNumberMipMap()==1) );
	bool resample=!(dx==dxout && dy==dyout);
	DWORD dither=(RenderMode&RENDERDEVICE_MODE_RGB16)?D3DX_FILTER_DITHER:0;
	bool is_alpha_test=false;
	bool is_alpha_blend=false;
	bool is_skin=Texture->skin_color.a==255;

	for(int i=0;i<Texture->GetNumberFrame();i++)
	{
		if(Texture->GetDDSurface(i)) 
			Texture->GetDDSurface(i)->Release(); 
		if(Texture->BitMap[i]==0) 
			Texture->BitMap[i]=CreateSurface(
						dx_surface,dy_surface,
						Texture->GetFmt(),Texture->GetNumberMipMap(),enable_assert,
						Texture->GetAttribute());
		if(Texture->BitMap[i]==0) 
			return 2;
		if(FileImage==0) continue;

		unsigned int *lpBuf = new unsigned int [dxy];
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
			ApplySkinColor((DWORD*)lpBuf,dx,dy,Texture->skin_color);
		}

		if(Texture->GetAttribute(TEXTURE_BUMP))
		{
			ConvertDot3(lpBuf,dx,dy,1.0e-2f*current_bump_scale);
		}

		RECT rect={0,0,dx,dy};

		LPDIRECT3DTEXTURE9 lpD3DTexture=Texture->GetDDSurface(i);
		LPDIRECT3DSURFACE9 lpSurface = NULL;
		RDCALL( lpD3DTexture->GetSurfaceLevel( 0, &lpSurface ) );

		RECT rect_out={0,0,dxout,dyout};
		RDCALL( D3DXLoadSurfaceFromMemory( lpSurface, NULL, &rect_out,
			lpBuf, D3DFMT_A8R8G8B8, 4*rect.right, NULL, &rect, 
			(resample?D3DX_FILTER_TRIANGLE:D3DX_FILTER_POINT)|dither
			, 0 ));

		if(Texture->GetNumberMipMap()>1) // построение мип мапов
			for(int nMipMap=1;nMipMap<Texture->GetNumberMipMap();nMipMap++)
			{
				LPDIRECT3DSURFACE9 lpSurfaceNext = NULL;
				RDCALL( lpD3DTexture->GetSurfaceLevel( nMipMap, &lpSurfaceNext ) );
				RECT rect={0,0,dx>>nMipMap,dy>>nMipMap};
				RECT rect_out={0,0,dxout>>nMipMap,dyout>>nMipMap};
				unsigned int *lpBufNext = new unsigned int [rect.right*rect.bottom];

				{
					BuildMipMap( rect.right,rect.bottom,4, 8*rect.right,lpBuf, 4*rect.right,lpBufNext, 
						8,8,8,8, 16,8,0,24, Texture->GetAttribute(TEXTURE_MIPMAP_POINT|TEXTURE_MIPMAPBLUR|TEXTURE_BLURWHITE));
					RDCALL( D3DXLoadSurfaceFromMemory( lpSurfaceNext, NULL, &rect_out,
						lpBufNext, D3DFMT_A8R8G8B8, 4*rect.right, NULL, &rect, 
						(resample?D3DX_FILTER_TRIANGLE:D3DX_FILTER_POINT)|dither
						, 0 ));
				}
				delete lpBuf; lpSurface->Release();
				lpBuf = lpBufNext; lpSurface = lpSurfaceNext;
			}
		lpSurface->Release();
		delete lpBuf;
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

int cD3DRender::CreateCubeTexture(class cTexture *Texture,LPCSTR fname)
{
	LPDIRECT3DCUBETEXTURE9 pCubeTexture=NULL;
	if(FAILED(D3DXCreateCubeTextureFromFile(lpD3DDevice,fname,&pCubeTexture)))
		return 1;

	Texture->BitMap.push_back((IDirect3DTexture9*)pCubeTexture);
		
	return 0;
}

int cD3DRender::CreateBumpTexture(class cTexture *Texture)
{
	IDirect3DTexture9* lpTexture=NULL;
	RDCALL(lpD3DDevice->CreateTexture(Texture->GetWidth(),Texture->GetHeight(),
					1,0,D3DFMT_V8U8 ,D3DPOOL_MANAGED,&lpTexture,NULL))
	Texture->BitMap[0]=lpTexture;
	return 0;
}

int cD3DRender::CreateTextureU16V16(class cTexture *Texture,bool defaultpool)
{
	IDirect3DTexture9* lpTexture=NULL;
	RDCALL(lpD3DDevice->CreateTexture(Texture->GetWidth(),Texture->GetHeight(),
		1,0,D3DFMT_V16U16 ,defaultpool?D3DPOOL_DEFAULT:D3DPOOL_MANAGED,&lpTexture,NULL))
	Texture->BitMap[0]=lpTexture;
	return 0;
}


int cD3DRender::DeleteTexture(cTexture *Texture)
{ // только освобождает в памяти поверхности 
	for(int nFrame=0;nFrame<Texture->GetNumberFrame();nFrame++)
		if(Texture->GetDDSurface(nFrame)) 
		{
			Texture->GetDDSurface(nFrame)->Release();
			Texture->GetDDSurface(nFrame)=0;
		}
	return 0;
}
bool cD3DRender::SetScreenShot(const char *fname)
{
	LPDIRECT3DSURFACE9 lpRenderSurface=0;
	RDCALL(lpD3DDevice->GetRenderTarget(0,&lpRenderSurface));
	HRESULT hr=D3DXSaveSurfaceToFile(fname,D3DXIFF_BMP,lpRenderSurface,NULL,NULL);
	//HRESULT hr=D3DXSaveSurfaceToFile(fname,D3DXIFF_PNG,lpRenderSurface,NULL,NULL);
	
	RELEASE(lpRenderSurface);
	return SUCCEEDED(hr);
}

void* cD3DRender::LockTexture(class cTexture *Texture,int& Pitch)
{
	D3DLOCKED_RECT d3dLockRect;
	IDirect3DTexture9* lpSurface=Texture->GetDDSurface(0);
	RDCALL(lpSurface->LockRect(0,&d3dLockRect,0,0));

	Pitch=d3dLockRect.Pitch;
	return d3dLockRect.pBits;
}

void* cD3DRender::LockTexture(class cTexture *Texture,int& Pitch,Vect2i lock_min,Vect2i lock_size)
{
	D3DLOCKED_RECT d3dLockRect;
	IDirect3DTexture9* lpSurface=Texture->GetDDSurface(0);
	RECT rc;
	rc.left=lock_min.x;
	rc.top=lock_min.y;
	rc.right=lock_min.x+lock_size.x;
	rc.bottom=lock_min.y+lock_size.y;

	RDCALL(lpSurface->LockRect(0,&d3dLockRect,&rc,0));

	Pitch=d3dLockRect.Pitch;
	return d3dLockRect.pBits;
}

void cD3DRender::UnlockTexture(class cTexture *Texture)
{
	IDirect3DTexture9* lpSurface=Texture->GetDDSurface(0);
	RDCALL(lpSurface->UnlockRect(0));
}

unsigned int ColorByNormal(Vect3f n)
{
	unsigned int x=((n.x+1)*127.5f);
	unsigned int y=((n.y+1)*127.5f);
	unsigned int z=((n.z+1)*127.5f);

	return z+(x<<8)+(y<<16);
}
Vect3f NormalByColor(DWORD d)
{
	Vect3f v;
	v.y = ((d>> 16) & 0xFF);
	v.x = ((d>> 8) & 0xFF);
	v.z = ((d) & 0xFF);
	v*= (1/127.5f);
	v-=Vect3f(1,1,1);
	return v;
}

void cD3DRender::ConvertDot3(unsigned int* ibuf,int dx,int dy,float h_mul)
{
	const int byte_per_pixel=4;
	BYTE* buf=(BYTE*)ibuf;
#define GET(x,y) buf[(clamp(x,0,dx-1)+dx*clamp(y,0,dy-1))*byte_per_pixel]
	unsigned int* out=new unsigned int[dx*dy];

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
	delete out;
#undef GET
}
