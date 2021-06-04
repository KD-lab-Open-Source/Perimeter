#include "StdAfxRD.h"
#include "Font.h"
#include "FileImage.h"
#include "..\..\PluginMAX\src\StreamBuffer.h"

#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <io.h>
#include <stdio.h>
#include <direct.h>
#include "FileImage.h"

#ifndef ASSERT
#define ASSERT(x) VISASSERT(x)
#endif 

#include "..\gemsiii\filter.h"
//#include "..\..\Terra\tgai.h"

static char* cache_dir="cache_font";

class cFontImage:public cFileImage
{
	BYTE* ImageData;
public:

	cFontImage()
	{
		ImageData=NULL;
	}
	virtual ~cFontImage()
	{
		delete ImageData;
	}

	virtual int GetTextureAlpha(void *pointer,int time,int bppDst,int bplDst,int acDst,int asDst,int xDst,int yDst)
	{
		cFileImage_GetFrameAlpha(pointer,bppDst,bplDst,acDst,asDst,xDst,yDst,
								ImageData,1,GetX(),8,0,GetX(),GetY());
		return 0;
	}
	virtual int GetTexture(void *pointer,int time,int bppDst,int bplDst,int rc,int gc,int bc,int ac,int rs,int gs,int bs,int as,int xDst,int yDst)
	{ 
		memset(pointer,0xFF,yDst*bplDst);
		return 0;
	}

	//Если убрать &, то начинает падать в release в VC7.1 (Microsoft Visual C++ .NET   69586-335-0000007-18787) 
	virtual void Create(BYTE* gray_in,const Vect2i& size)
	{
		x=size.x;
		y=size.y;
		bpp=8;

		if(ImageData)delete ImageData;
		int sz2=size.x*size.y;
		ImageData=new BYTE[sz2];
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

bool cFontInternal::CreateImage(LPCSTR filename,LPCSTR fontname,int height,class cFontImage* FontImage)
{
	ZIPStream rd(filename);
	if(!!rd)
	{
		return false;
	}

	char signature[4];

	int  real_height;
	WORD char_min,char_max;
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
#pragma pack(push,1)
	struct OneChar
	{
		int width;
		int real_width;
		BYTE* bits;
	};
#pragma pack(pop,1)
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
		c.bits=new BYTE[size_size];
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
	};
	const int sizes_size= sizeof(sizes)/sizeof(sizes[0]);
	Vect2i size(0,0);

	float mul=height/float(real_height);
	int yborder=max(round(2*mul),2);
	int sz;
	for(sz=0;sz<sizes_size;sz++)
	{
		size=sizes[sz];

		int x=0,y=0;
		for(i=char_min;i<char_max;i++)
		{

			int dx=round(chars[i].width*mul+2);
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
			delete chars[i].bits;
		return false;
	}

	FontHeight=height/float(size.y);

	//Создаём текстуру
	Vect2i real_size(round(size.x/mul),round(size.y/mul));
	BYTE* gray_in=new BYTE[real_size.x*real_size.y];
	memset(gray_in,0,real_size.x*real_size.y);

	int x=0,y=0;
	for(i=char_min;i<char_max;i++)
	{

		int w=chars[i].width;
		int dx=round(w*mul+2);
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
			int b=chars[i].bits[bit>>3]&(1<<(7-bit&7));

			if(curx<real_size.x && cury<real_size.y)
				gray_in[curx+cury*real_size.x]=b?255:0;
		}

		Font[i].x=x/double(size.x);
		Font[i].y=y/double(size.y);
		Font[i].z=(dx)/double(size.x);

		x+=dx;
	}

	for(i=0;i<256;i++)
		delete chars[i].bits;

	BYTE* gray_out=new BYTE[size.x*size.y];

//	TGAHEAD h;
//	h.save3layers("file_in.tga",real_size.x,real_size.y,gray_in,gray_in,gray_in);

	resample(gray_in,real_size.x,real_size.y,gray_out,size.x,size.y,'l');
	delete gray_in;
	
	FontImage->Create(gray_out,size);

//	h.save3layers("file.tga",size.x,size.y,gray_out,gray_out,gray_out);
	delete gray_out;
	
	Save(fontname,*FontImage);
	return true;
}

bool cFontInternal::CreateTexture(LPCSTR fontname,LPCSTR filename,int height)
{
	if(height>100)
	{
		return false;
	}

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
	err=err || gb_VisGeneric->GetRenderDevice()->CreateTexture(pTexture,&FontImage,-1,-1)!=0;

	if(err)
	{
		RELEASE(pTexture);
		return false;
	}

	return true;
}

bool cFontImage::Save(const char* fname)
{
	return SaveTga(fname,GetX(),GetY(),ImageData,GetBitPerPixel()/8);
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
	string ftga,ffont;
	ftga=cache_dir;ftga+='\\';
	ffont=cache_dir;ffont+='\\';
	ftga+=fname; ftga+=".tga";
	ffont+=fname;ffont+=".xfont";
	mkdir(cache_dir);

	if(!fnt.Save(ftga.c_str()))
		return false;
	int file=_open(ffont.c_str(),_O_WRONLY|_O_TRUNC|_O_CREAT|_O_BINARY,_S_IREAD|_S_IWRITE);
	if(file==-1)
		return false;

	DWORD size=Font.size();
	_write(file,&FontHeight,sizeof(FontHeight));
	_write(file,&size,sizeof(size));
	_write(file,&Font[0],size*sizeof(Vect3f));
	_close(file);
	return true;
}

bool cFontInternal::Load(const char* fname,cFontImage& fnt)
{
	string ftga,ffont;
	ftga=cache_dir;ftga+='\\';
	ffont=cache_dir;ffont+='\\';
	ftga+=fname; ftga+=".tga";
	ffont+=fname;ffont+=".xfont";

	if(!fnt.Load(ftga.c_str()))
		return false;

	int file=_open(ffont.c_str(),_O_RDONLY|_O_BINARY);
	if(file==-1)
		return false;

	DWORD size=0;
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

void str_add_slash(char* str)
{
	int len=strlen(str);
	if(len>0)
	{
		if(str[len-1]!='/' && str[len-1]!='\\')
		{
			str[len]='\\';
			str[len+1]=0;
		}
	}
}

bool cFontInternal::Create(LPCSTR root_dir,LPCSTR language_dir,LPCSTR fname,int h,bool silentErr)
{
	int ScreenY=gb_RenderDevice->GetSizeY();

	int height=round((h*ScreenY)/768.0f);
	statement_height=h;

	char prefix[_MAX_PATH];
	char texture_name[_MAX_PATH];
	char fontname[_MAX_PATH];
	prefix[0]=0;
	texture_name[0]=0;

	if(root_dir)
	{
		strcpy(prefix,root_dir);
		str_add_slash(prefix);
		str_replace_slash(prefix);
		_strlwr(prefix);
	}
/*
	if(root_dir)
	{
		strcpy(texture_name,root_dir);
		str_add_slash(texture_name);
	}
*/
	if(language_dir)
	{
		strcat(texture_name,language_dir);
		str_add_slash(texture_name);
	}
	

	strcpy(fontname,texture_name);
	strcat(fontname,fname);
	strcat(fontname,".font");

	int len=strlen(texture_name);
	sprintf(texture_name+len,"%s-%i",fname,height);
	_strlwr(texture_name);
	str_replace_slash(texture_name);
	// @caiiiycuk: need to check
	char *p;
	char *c;
	for(p=prefix,c=texture_name;*p;p++,c++)
	{
		if(*p!=*c)
			break;
	}

	if(!CreateTexture(c,fontname,height))
	{
		if(!silentErr) VisError<<"Cannot load font: "<< fontname <<VERR_END;
		return false;
	}

	font_name=fname;
	_strlwr((char*)font_name.c_str());
	return true;
}

bool cFontInternal::Reload(LPCSTR root_dir,LPCSTR language_dir)
{
	string f=font_name;
	return Create(root_dir,language_dir,f.c_str(),GetStatementHeight());
}
