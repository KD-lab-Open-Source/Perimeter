#include "StdAfxRD.h"
#include "FileImage.h"
#include "files/files.h"

#ifdef PERIMETER_D3D9
#ifdef _WIN32
#include "ddraw.h"
#endif
#include "D3DRender.h"
#endif //PERIMETER_D3D9

#ifdef PERIMETER_SOKOL
#include "sokol/SokolIncludes.h"
#include "sokol/SokolResources.h"
#endif //PERIMETER_SOKOL

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
#endif //TEXTURE_NOTFREE

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
			if (f) fprintf(f,"%s - %" PRIi64 "\n",p->GetName().c_str(),p->GetRef());
		}
	}

	if(f)fprintf(f,"Texture free %i, not free %" PRIsize "\n",compacted,textures.size()-compacted);

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

cTexture* cTexLibrary::GetElementAviScale(const char* TextureName,const char *pMode)
{
	MTAuto mtenter(&lock);
	if(TextureName==0||TextureName[0]==0) return 0; // имя текстуры пустое

	for(int i=0;i<GetNumberTexture();i++)
	{
		cTexture* cur=GetTexture(i);
		xassert(cur->GetX()>=0 && cur->GetX()<=15);
		xassert(cur->GetY()>=0 && cur->GetY()<=15);
		if( cur && stricmp(cur->GetName().c_str(),TextureName)==0)
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

cTexture* cTexLibrary::GetElement(const char* TextureName,const char *pMode)
{
	MTAuto mtenter(&lock);
	if(TextureName==nullptr||TextureName[0]==0) return nullptr; // имя текстуры пустое
    std::string path = convert_path_native(TextureName);

	for(int i=0;i<GetNumberTexture();i++)
	{
		cTexture* cur=GetTexture(i);
		xassert(cur);
		if( cur && stricmp(cur->GetName().c_str(),path.c_str())==0)
		{
			xassert(cur->GetX() >= 0 && cur->GetX() <= 15);
			xassert(cur->GetY() >= 0 && cur->GetY() <= 15);
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

bool cTexLibrary::LoadTexture(cTexture* Texture,const char *pMode)
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
    
	//Get path for file and open it
    std::string path = Texture->GetName();
    if (get_content_entry(path)) {
        path = convert_path_content(path);
    } else {
        if (endsWith(path, ".avi")) {
            if (get_content_entry(path + "x")) {
                //Use AVIX if available when AVI is absent
                path = convert_path_content(path + "x");
            }
        } else if (endsWith(path, ".avix")) {
            std::string path_avi = path.substr(0, path.length() - 1);
            if (get_content_entry(path_avi)) {
                //Use AVI if available when AVIX is absent
                path = convert_path_content(path_avi);
            }
        }
	}
	
	cFileImage* FileImage = path.length() > 0 ? cFileImage::Create(path.c_str()) : nullptr;
	if(!FileImage) {
        bool err;
#ifdef PERIMETER_D3D9
	    //If the file extension is not recognized, try open it using DirectX 
        err = ReLoadDDS(Texture);
#else
        err = true;
#endif
        if (err) {
            Error(Texture);
            Texture->Release();
        }
#ifdef PERIMETER_DEBUG
        fprintf(stderr, "ReLoadDDS %s %d\n", path.c_str(), err);
#endif
        return err;
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
        VisError << "Error: cTexLibrary::GetElement()\r\nTexture is bad: " << Texture->GetName().c_str() << "."
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
		if (!p->GetName().empty() && p->GetName()[0]) {
			ReLoadTexture(p);
		}
	}
}

#ifdef PERIMETER_D3D9
bool cTexLibrary::ReLoadDDS(cTexture* Texture)
{
    if (!gb_RenderDevice3D) {
        return false;
    }
	char* buf = nullptr;
	int size = 0;
	//0 - alpha_none, 1- alpha_test, 2 - alpha_blend
	int ret=ResourceFileRead(Texture->GetName().c_str(),buf,size);
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

#ifdef _WIN32
	DDSURFACEDESC2* ddsd=(DDSURFACEDESC2*)(1+(uint32_t*)buf);
	if(ddsd->ddsCaps.dwCaps2&DDSCAPS2_CUBEMAP) {
        return false;
	}
#endif
    IDirect3DTexture9* pTexture=gb_RenderDevice3D->CreateTextureFromMemory(buf,size);
    if(!pTexture)
    {
        return false;
    }

    D3DSURFACE_DESC desc;
    RDCALL(pTexture->GetLevelDesc(0,&desc));
    Texture->SetWidth(desc.Width);
    Texture->SetHeight(desc.Height);

    Texture->frames.emplace_back().d3d = pTexture;
    return true;
}
#endif