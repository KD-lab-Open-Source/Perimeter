#include "StdAfxRD.h"
#include "FileImage.h"
#include "files/files.h"

#ifdef PERIMETER_D3D9
#include "ddraw.h"
#include "D3DRender.h"
#endif
#ifdef PERIMETER_SOKOL
#include <sokol_gfx.h>
#include "sokol/SokolResources.h"
#endif

#ifdef TEXTURE_NOTFREE
struct BeginNF
{

	BeginNF()
	{
		FILE* f=NULL;
		f=fopen("texture_notfree1.txt","wt");
		f=fopen("obj_notfree1.txt","wt");
		if(f)fclose(f);
	}
};
static BeginNF begin_nf;
#endif

cTexLibrary* GetTexLibrary()
{
	return gb_RenderDevice->GetTexLibrary();
}

cTexLibrary::cTexLibrary()
{
	enable_error=true;
	cFileImage::InitFileImage();
}
cTexLibrary::~cTexLibrary()
{
	Free();
	cFileImage::DoneFileImage();
}

bool cTexLibrary::EnableError(bool enable)
{
	bool b=enable_error;
	enable_error=enable;
	return b;
}

void cTexLibrary::FreeOne(FILE* f)
{
	bool close=false;
#ifdef TEXTURE_NOTFREE
	if(!f)
	{
		f=fopen("texture_notfree.txt","at");
		close=true;
	}
#endif 
	if(f)fprintf(f,"Texture freeing\n");

	int compacted=0;
	std::vector<cTexture*>::iterator it;
	FOR_EACH(textures,it)
	{
		cTexture*& p=*it;
		if(p && p->GetRef()==1)
		{
			p->Release();
			p=NULL;
			compacted++;
		}else
		{
//			VISASSERT(p->GetRef()==1);
			if(f)fprintf(f,"%s - %i\n",p->GetName(),p->GetRef());
		}
	}

	if(f)fprintf(f,"Texture free %i, not free %i\n",compacted,textures.size()-compacted);

	if(f)
	{
		if(close)fclose(f);
		else fflush(f);
	}
}

void cTexLibrary::Compact(FILE* f)
{
	FreeOne(f);
	remove_null_element(textures);
}

void cTexLibrary::Free(FILE* f)
{
	FreeOne(f);
	textures.clear();
}

cTexture* cTexLibrary::CreateRenderTexture(int width, int height, uint32_t attr, bool enable_assert)
{
	MTAuto mtenter(&lock);
	if(!attr)
		attr=TEXTURE_RENDER16;
	attr|=TEXTURE_POOL_DEFAULT;
	cTexture *Texture=new cTexture("");
	Texture->New(1);
	Texture->SetTimePerFrame(0);	
	Texture->SetWidth(width);
	Texture->SetHeight(height);
	Texture->SetNumberMipMap(1);
	Texture->SetAttribute(attr);

	int err=gb_RenderDevice->CreateTexture(Texture,NULL,enable_assert);
	if(err) { Texture->Release(); return 0; }
	textures.push_back(Texture); Texture->IncRef();
	return Texture;
}

cTexture* cTexLibrary::CreateTexture(int sizex,int sizey,bool alpha,bool default_pool)
{
	cTexture *Texture=new cTexture;
	Texture->SetNumberMipMap(1);
	Texture->SetAttribute(TEXTURE_32);
	if(alpha)
		Texture->SetAttribute(TEXTURE_ALPHA_BLEND);
	if(default_pool)
		Texture->SetAttribute(TEXTURE_POOL_DEFAULT);

	Texture->frames.resize(1);
	Texture->frames[0].ptr=nullptr;
	Texture->SetWidth(sizex);
	Texture->SetHeight(sizey);

	if(gb_RenderDevice->CreateTexture(Texture,NULL))
	{
		delete Texture;
		return NULL;
	}

	return Texture;
}

cTexture* cTexLibrary::CreateTextureDefaultPool(int sizex,int sizey,bool alpha)
{
	return CreateTexture(sizex,sizey,alpha,true);
}

cTexture* cTexLibrary::CreateTexture(int sizex,int sizey,bool alpha)
{
	return CreateTexture(sizex,sizey,alpha,false);
}

cTexture* cTexLibrary::GetElementAviScale(const char* TextureName,char *pMode)
{
	MTAuto mtenter(&lock);
	if(TextureName==0||TextureName[0]==0) return 0; // имя текстуры пустое

	for(int i=0;i<GetNumberTexture();i++)
	{
		cTexture* cur=GetTexture(i);
		xassert(cur->GetX()>=0 && cur->GetX()<=15);
		xassert(cur->GetY()>=0 && cur->GetY()<=15);
		if( cur && stricmp(cur->GetName(),TextureName)==0)
		{
			cur->IncRef();
			return cur;
		}
	}

	cAviScaleFileImage avi_images;
	std::string fName = TextureName;
	if (avi_images.Init(fName.c_str())==false)
		return NULL;

	cTextureAviScale* Texture=new cTextureAviScale(TextureName);
	Texture->SetNumberMipMap(1);
	Texture->SetAttribute(TEXTURE_ALPHA_TEST/*TEXTURE_ALPHA_BLEND*/);
	Texture->New(1);
	Texture->Init(avi_images.GetFramesCount(), avi_images.GetXFrame(), avi_images.GetYFrame(), avi_images.GetX(), avi_images.GetY());
	Texture->SetTimePerFrame(0);
	if (gb_VisGeneric->GetRenderDevice()->CreateTexture(Texture,&avi_images)!=0)
	{
		delete Texture;
		return NULL;
	}

	textures.push_back(Texture); Texture->IncRef();
	return Texture;
}

cTexture* cTexLibrary::GetElement(const char* TextureName,char *pMode)
{
	MTAuto mtenter(&lock);
	if(TextureName==nullptr||TextureName[0]==0) return nullptr; // имя текстуры пустое
    std::string path = convert_path_native(TextureName);

	for(int i=0;i<GetNumberTexture();i++)
	{
		cTexture* cur=GetTexture(i);
		xassert(cur->GetX()>=0 && cur->GetX()<=15);
		xassert(cur->GetY()>=0 && cur->GetY()<=15);
		if( cur && stricmp(cur->GetName(),path.c_str())==0)
		{
			cur->IncRef();
			return cur;
		}
	}

	cTexture *Texture=new cTexture(path.c_str());

	if(!LoadTexture(Texture,pMode))
	{
		return nullptr;
	}

	textures.push_back(Texture); Texture->IncRef();
	return Texture;
}

bool cTexLibrary::LoadTexture(cTexture* Texture,char *pMode)
{
	// тест наличия текстуры
	if(pMode&&strstr((char*)pMode,"NoMipMap"))
		Texture->SetNumberMipMap(1);
	else 
		Texture->SetNumberMipMap(Option_MipMapLevel);

	if(Option_MipMapBlur && pMode && strstr(pMode,"MipMapBlur") && strstr(pMode,"MipMapNoBlur")==0)
	{
		Texture->SetAttribute(TEXTURE_MIPMAPBLUR);
		if(pMode&&strstr(pMode,"MipMapBlurWhite")) 
			Texture->SetAttribute(TEXTURE_BLURWHITE);
	}

	if(pMode && strstr(pMode, "UVBump")) {
        Texture->SetAttribute(TEXTURE_UVBUMP);
    }

    if(pMode&&strstr(pMode,"Bump")) {        
        Texture->SetAttribute(MAT_BUMP);
        Texture->bump_scale = current_bump_scale;
        
        //If file with _normal name is found, set attribute for normal
        std::string textpathstr = convert_path_content(Texture->GetName());
        if (textpathstr.empty()) textpathstr = convert_path_native(Texture->GetName());
        std::filesystem::path texpath = std::filesystem::u8path(textpathstr);
        std::string normal_path = texpath.parent_path().u8string();
        std::string extension = texpath.filename().extension().u8string();
        std::string filename = texpath.filename().u8string();
        string_replace_all(filename, extension, "");
        string_replace_all(filename, "_bump", "");
        normal_path += PATH_SEP + filename + "_normal" + extension;
        if (std::filesystem::exists(std::filesystem::u8path(normal_path))) {
            Texture->SetName(normal_path.c_str());
            Texture->SetAttribute(MAT_NORMAL);
        }
    }

    if(pMode&&strstr(pMode,"Normal"))
        Texture->SetAttribute(MAT_NORMAL);

	return ReLoadTexture(Texture);
}

bool cTexLibrary::ReLoadTexture(cTexture* Texture)
{
	{
		for (auto frame : Texture->frames)
		{
#ifdef PERIMETER_D3D9
            if (gb_RenderDevice->GetRenderSelection() == DEVICE_D3D9) {
                frame.d3d->Release();
            }
#endif
#ifdef PERIMETER_SOKOL
            if (gb_RenderDevice->GetRenderSelection() == DEVICE_SOKOL) {
                delete frame.sg;
            }
#endif
            frame.ptr = nullptr;
		}
		Texture->frames.clear();
	}
		
	bool bump=Texture->GetAttribute(MAT_BUMP)?true:false;
	bool pos_bump=gb_VisGeneric->PossibilityBump();

	if(bump && !pos_bump)
	{
#if 0 //#ifdef PERIMETER_D3D9
        //TODO what is this? is even need?
		//Эта текстура никогда не должна использоваться, make small texture.
		cTexture* pMini=GetElement("RESOURCE\\Models\\Main\\TEXTURES\\028r.tga");
		if(pMini && !pMini->frames.empty() && pMini->frames.front().d3d)
		{
			Texture->frames.push_back(pMini->frames.front());
			Texture->frames[0].d3d->AddRef();
		}
		RELEASE(pMini);
#endif
		return true;
	}
/*
	if(Option_FavoriteLoadDDS && !bump)
	{
		char drive[_MAX_DRIVE];
		char dir[_MAX_DIR];
		char fname[_MAX_FNAME];
		char ext[_MAX_EXT];
		_splitpath( Texture->GetName(), drive, dir, fname, ext );

		char path_buffer[MAX_PATH];
		_makepath(path_buffer, drive, dir, fname, "dds" );

		void* buf;
		int size;
		//0 - alpha_none, 1- alpha_test, 2 - alpha_blend
		int ret=ResourceFileRead(path_buffer,buf,size);///Утечка памяти
		if(!ret)
		{
			BYTE alpha_type=((BYTE*)buf)[size-1];
			switch(alpha_type)
			{
			case 0:
				break;
			case 1:
				Texture->SetAttribute(TEXTURE_ALPHA_TEST);
				break;
			case 2:
				Texture->SetAttribute(TEXTURE_ALPHA_BLEND);
				break;
			default:
				//Неправильный dds файл. В конце должен быть байт с alpha.
				Error(Texture);
				Texture->Release();
				return false;
			}

			LPDIRECT3DTEXTURE9 pTexture=gb_RenderDevice->CreateTextureFromMemory(buf,size-1);
			if(!pTexture)
			{
				Error(Texture);
				Texture->Release();
				return false;
			}

			D3DSURFACE_DESC desc;
			RDCALL(pTexture->GetLevelDesc(0,&desc));
			Texture->SetWidth(desc.Width);
			Texture->SetHeight(desc.Height);

			Texture->BitMap.push_back(pTexture);
			return true;
		}
	}
*/
	//Get path for file and open it
	std::string path = convert_path_content(Texture->GetName());
	if (path.empty()) {
        path = Texture->GetName();
        if (endsWith(path, ".avi") && !convert_path_content(path + "x").empty()) {
            //Use AVIX if available when AVI is absent
            path += "x";
        }
	}
	
	cFileImage* FileImage = cFileImage::Create(path.c_str());
	if(!FileImage) {
#ifdef PERIMETER_D3D9
	    //If the file extension is not recognized, try open it using DirectX 
		return ReLoadDDS(Texture);
#else
        return false;
#endif
	}
	
	if(FileImage->load(path.c_str()))
	{
		delete FileImage;
		Error(Texture);
		Texture->Release();
		return false;
	}
	if(FileImage->GetBitPerPixel()==32) Texture->SetAttribute(TEXTURE_ALPHA_BLEND);

	Texture->New(FileImage->GetLength());
	if(FileImage->GetLength()<=1) 
		Texture->SetTimePerFrame(0);
	else
		Texture->SetTimePerFrame((FileImage->GetTime()-1)/(FileImage->GetLength()-1));
	Texture->SetWidth(FileImage->GetX()),Texture->SetHeight(FileImage->GetY());

	int err=gb_RenderDevice->CreateTexture(Texture,FileImage);

	delete FileImage;
	if(err)
	{
		Error(Texture);
		Texture->Release();
		return false; 
	}

	return true;
}

void cTexLibrary::Error(cTexture* Texture) {
	if(enable_error) {
        VisError << "Error: cTexLibrary::GetElement()\r\nTexture is bad: " << Texture->GetName() << "."
                 << VERR_END;
    }
}

void cTexLibrary::ReloadAllTexture()
{
	MTAuto mtenter(&lock);

	std::vector<cTexture*>::iterator it;
	FOR_EACH(textures,it)
	{
		cTexture* p=*it;
		if(p->GetName() && p->GetName()[0])
		{
			ReLoadTexture(p);
		}
	}
}

#ifdef PERIMETER_D3D9
bool cTexLibrary::ReLoadDDS(cTexture* Texture)
{
	char* buf=NULL;
	int size;
	//0 - alpha_none, 1- alpha_test, 2 - alpha_blend
	int ret=ResourceFileRead(Texture->GetName(),buf,size);
	if(ret)
		return false;

	class CAutoDelete
	{
		char* buf;
	public:
		CAutoDelete(char* buf_):buf(buf_){};
		~CAutoDelete()
		{
			delete[] buf;
		}
	} auto_delete(buf);

	DDSURFACEDESC2* ddsd=(DDSURFACEDESC2*)(1+(uint32_t*)buf);
	if(ddsd->ddsCaps.dwCaps2&DDSCAPS2_CUBEMAP) {
        Error(Texture);
        Texture->Release();
        return false;
	} else {
		LPDIRECT3DTEXTURE9 pTexture=gb_RenderDevice3D->CreateTextureFromMemory(buf,size);
		if(!pTexture)
		{
			Error(Texture);
			Texture->Release();
			return false;
		}

		D3DSURFACE_DESC desc;
		RDCALL(pTexture->GetLevelDesc(0,&desc));
		Texture->SetWidth(desc.Width);
		Texture->SetHeight(desc.Height);

		Texture->frames.emplace_back().d3d = pTexture;
		return true;
	}
}
#endif