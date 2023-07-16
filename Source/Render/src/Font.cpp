#include "StdAfxRD.h"
#include "Font.h"
#include "FileImage.h"

#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include "SystemUtil.h"

#ifndef ASSERT
#define ASSERT(x) VISASSERT(x)
#endif 

#include "../gemsiii/filter.h"
#include "files/files.h"

static char* cache_dir="cache/font";

class cFontImage:public cFileImage
{
	uint8_t* ImageData;
public:

	cFontImage()
	{
		ImageData=NULL;
	}
	virtual ~cFontImage()
	{
		delete[] ImageData;
	}

	int GetTextureAlpha(void *pointer,int time,int bppDst,int bplDst,int acDst,int asDst,int xDst,int yDst) override
	{
		cFileImage_GetFrameAlpha(pointer,bppDst,bplDst,acDst,asDst,xDst,yDst,
								ImageData,1,GetX(),8,0,GetX(),GetY());
		return 0;
	}
	int GetTextureRGB(void *pointer,int time,int bppDst,int bplDst,int rc,int gc,int bc,int rs,int gs,int bs,int xDst,int yDst) override
	{ 
		memset(pointer,0xFF,yDst*bplDst);
		return 0;
	}

	//Если убрать &, то начинает падать в release в VC7.1 (Microsoft Visual C++ .NET   69586-335-0000007-18787) 
	virtual void Create(uint8_t* gray_in, const Vect2i& size)
	{
		x=size.x;
		y=size.y;
		bpp=8;

		if(ImageData) delete[] ImageData;
		size_t sz2=size.x*size.y;
		ImageData=new uint8_t[sz2];
		for(int i=0;i<sz2;i++)
			ImageData[i]=gray_in[i];
	}

	bool Save(const char* fname);
	bool Load(const char* fname);
};

cFontInternal::cFontInternal(int kind) : cUnknownClass(kind)
{
	pTexture=NULL;

	Font.resize(256);
	FontHeight=0;
	statement_height=0;
	for(int i=0;i<256;i++)
		Font[i].set(0,0,0);

}
cFontInternal::~cFontInternal()
{
	RELEASE(pTexture);
}

bool cFontInternal::CreateImage(const char* filename, const char* fontname, int height, class cFontImage* FontImage)
{
	ZIPStream rd;
	if(!rd.open(convert_path_content(filename).c_str()))
	{
		return false;
	}

	char signature[4];

	int  real_height;
    int16_t char_min,char_max;
	rd.read(signature,4);
	if(signature[0]!='f' || signature[1]!='o' || 
		signature[2]!='n' || signature[3]!='t')
	{
		return false;
	}

	rd>real_height;
	rd>char_min;
	rd>char_max;

	VISASSERT(char_min<char_max);
	VISASSERT(char_min<=256);//VISASSERT(char_min>=0 && char_min<=256);
	VISASSERT(char_max<=256);//VISASSERT(char_max>=0 && char_max<=256);
#ifdef _MSC_VER
#pragma pack(push,1)
#endif
	struct OneChar
	{
		int width;
		int real_width;
		uint8_t* bits;
	};
#ifdef _MSC_VER
#pragma pack(pop,1)
#endif
	OneChar chars[256];
	int i;
	for(i=0;i<256;i++)
	{
		chars[i].width=0;
		chars[i].bits=NULL;
	}

	for(i=char_min;i<char_max;i++)
	{
		OneChar& c=chars[i];
		rd.read(&c.width,4);
		c.real_width=(c.width+7)/8;
		int size_size=c.real_width*real_height;
		c.bits=new uint8_t[size_size];
		rd.read(c.bits,size_size);
	}
	rd.close();

	//Рассчитываем величину текстуры
	Vect2i sizes[]={
		Vect2i(128,128),
		Vect2i(256,128),
		Vect2i(256,256),
		Vect2i(512,256),
		Vect2i(512,512),
		Vect2i(1024,512),
		Vect2i(1024,1024),
		Vect2i(2048,1024),
		Vect2i(2048,2048),
		Vect2i(4096,2048),
		Vect2i(4096,4096),
		Vect2i(8192,4096),
		Vect2i(8192,8192),
	};
	const int sizes_size= sizeof(sizes)/sizeof(sizes[0]);
	Vect2i size(0,0);

	float mul=height/float(real_height);
	int yborder=max((int) xm::round(2 * mul), 2);
	int sz;
	for(sz=0;sz<sizes_size;sz++)
	{
		size=sizes[sz];

		int x=0,y=0;
		for(i=char_min;i<char_max;i++)
		{

			int dx=(int) xm::round(chars[i].width * mul + 2);
			if(x+dx>size.x)
			{
				y+=(height+yborder);
				x=0;
			}

			x+=dx;
		}

		if(y+height+yborder<=size.y && x<=size.x)
			break;
	}

	if(sz>=sizes_size)
	{
		for(i=0;i<256;i++)
			delete[] chars[i].bits;
		ErrH.Abort("Couldn't find big enough image for this font", XERR_USER, sz, fontname);
		return false;
	}

	FontHeight=height/float(size.y);

	//Создаём текстуру
	Vect2i real_size((int) xm::round(size.x / mul), (int) xm::round(size.y / mul));
	uint8_t* gray_in=new uint8_t[real_size.x * real_size.y];
	memset(gray_in,0,real_size.x*real_size.y);

	int x=0,y=0;
	for(i=char_min;i<char_max;i++)
	{

		int w=chars[i].width;
		int dx=(int) xm::round(w * mul + 2);
		if(x+dx>size.x)
		{
			y+=(height+yborder);
			x=0;
		}

		//Рисуем
		int realx=int(x/mul),realy=int(y/mul);
		for(int yy=0;yy<real_height;yy++)
		for(int xx=0;xx<w;xx++)
		{
			int cury=yy+realy;
			int curx=xx+realx;

			int bit=xx+yy*chars[i].real_width*8;
			int b=chars[i].bits[bit>>3]&(1<<((7-bit)&7));

			if(curx<real_size.x && cury<real_size.y)
				gray_in[curx+cury*real_size.x]=b?255:0;
		}

		Font[i].x=x/double(size.x);
		Font[i].y=y/double(size.y);
		Font[i].z=(dx)/double(size.x);

		x+=dx;
	}

	for(i=0;i<256;i++)
		delete[] chars[i].bits;

	uint8_t* gray_out=new uint8_t[size.x * size.y];

//	TGAHEAD h;
//	h.save3layers("file_in.tga",real_size.x,real_size.y,gray_in,gray_in,gray_in);

	resample(gray_in,real_size.x,real_size.y,gray_out,size.x,size.y,'l');
	delete[] gray_in;
	
	FontImage->Create(gray_out,size);

//	h.save3layers("file.tga",size.x,size.y,gray_out,gray_out,gray_out);
	delete[] gray_out;
	
	Save(fontname,*FontImage);
	return true;
}

bool cFontInternal::CreateTexture(const char* fontname, const char* filename, int height)
{
	if(pTexture)
		delete pTexture;
	pTexture=NULL;

/*
	height=14;//22;//18;
	filename="Arial";
*/
	cFontImage FontImage;
	if(!Load(fontname,FontImage))
	if(!CreateImage(filename,fontname,height,&FontImage))
	{
		return false;
	}

	pTexture=new cTexture;
	pTexture->SetNumberMipMap(1);
	pTexture->SetAttribute(TEXTURE_ALPHA_BLEND|TEXTURE_GRAY);
	pTexture->New(1);
	pTexture->SetTimePerFrame(0);
	pTexture->SetWidth(FontImage.GetX());
	pTexture->SetHeight(FontImage.GetY());

	bool err=false;
	err=err || gb_VisGeneric->GetRenderDevice()->CreateTexture(pTexture,&FontImage)!=0;

	if(err)
	{
		RELEASE(pTexture);
		return false;
	}

	return true;
}

bool cFontImage::Save(const char* fname)
{
	return SaveTGA(fname,GetX(),GetY(),ImageData,GetBitPerPixel()/8);
}

bool cFontImage::Load(const char* fname)
{
	int byte_per_pixel;
	if(!LoadTGA(fname,x,y,ImageData,byte_per_pixel))
		return false;
	if(byte_per_pixel!=1)
		return false;
	bpp=8;
	return true;
}

bool cFontInternal::Save(const char* fname,cFontImage& fnt)
{
	std::string ftga,ffont;
    std::string cache_path = convert_path_content(cache_dir, false);
    xassert(!cache_path.empty());
	ftga=cache_path;ftga+=PATH_SEP;
	ffont=cache_path;ffont+=PATH_SEP;
	ftga+=fname; ftga+=".tga";
	ffont+=fname;ffont+=".xfont";

	if(!fnt.Save(ftga.c_str()))
		return false;
	int file= file_open(ffont.c_str(), _O_WRONLY | _O_TRUNC | _O_CREAT | _O_BINARY, _S_IREAD | _S_IWRITE);
	if(file==-1)
		return false;

	uint32_t size=Font.size();
	_write(file,&FontHeight,sizeof(FontHeight));
	_write(file,&size,sizeof(size));
	_write(file,&Font[0],size*sizeof(Vect3f));
	_close(file);
    scan_resource_paths(cache_path);
	return true;
}

bool cFontInternal::Load(const char* fname,cFontImage& fnt)
{
	std::string ftga,ffont;
    std::string cache_path = convert_path_content(cache_dir, true);
	ftga=cache_path;ftga+=PATH_SEP;
	ffont=cache_path;ffont+=PATH_SEP;
	ftga+=fname; ftga+=".tga";
	ffont+=fname;ffont+=".xfont";

	if(!fnt.Load(ftga.c_str()))
		return false;

	int file= file_open(ffont.c_str(), _O_RDONLY | _O_BINARY);
	if(file==-1)
		return false;

	uint32_t size=0;
	_read(file,&FontHeight,sizeof(FontHeight));
	_read(file,&size,sizeof(size));
	Font.resize(size);
	_read(file,&Font[0],size*sizeof(Vect3f));
	_close(file);
	return true;
}

void str_replace_slash(char* str)
{
	for(char* p=str;*p;p++)
		if(*p=='/' || *p=='\\')
			*p='_';
}

bool cFontInternal::Create(const std::string& root_dir, const std::string& locale_, const std::string& fname, int h, bool silentErr)
{
	int ScreenY=gb_RenderDevice->GetSizeY();
    xassert(0<=ScreenY);

	int height=(int) xm::round((float) (h * ScreenY) / 768.0f);
	statement_height=h;
    locale=locale_;
    font_name=string_to_lower(fname.c_str());

    std::string font_path = locale + PATH_SEP + "Fonts" + PATH_SEP;
    
    //Create texture name for caching
    std::string texture_name = font_path + font_name + "-" + std::to_string(height);
    texture_name = string_to_lower(texture_name.c_str());
    str_replace_slash(texture_name.data());
    
    //Get path for font
    font_path = root_dir + font_path + font_name + ".font";

    if (gb_RenderDevice->GetRenderSelection() == DEVICE_HEADLESS) {
        return true;
    }
    
	if(!CreateTexture(texture_name.c_str(),font_path.c_str(),height))
	{
		if(!silentErr) VisError<<"Cannot load font: "<< font_path <<VERR_END;
		return false;
	}

	return true;
}

bool cFontInternal::Reload(const char* root_dir)
{
    std::string l=locale;
	std::string f=font_name;
	return Create(root_dir,l,f,GetStatementHeight());
}
