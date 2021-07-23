#include "tweaks.h"
#include <windows.h>
#include <stdio.h>
#include <cstdint>
#include <assert.h>

#ifdef PERIMETER_FFMPEG
#include "AVWrapper.h"
#else
#include <vfw.h>		// AVI include
#include <math.h>
#include <sys/types.h>
#include "xutil.h"
#endif

#ifdef _WIN32
//For _open etc
#include <io.h>
#endif

//#include <setjmp.h>		// JPG include
#include "xutil.h"
#include "FileImage.h"

#include <fcntl.h>
#include <sys/stat.h>


#if (!defined(_FINAL_VERSION_) || defined(_DEBUG)) && !defined(NASSERT)
#include <iostream>
#endif

int ResourceFileRead(const char *fname,char *&buf,int &size);

#ifndef ABS
#define ABS(a)										((a)>=0?(a):-(a))
#endif // ABS
#ifndef SIGN
#define SIGN(a)										((a)>0?1:((a)<0)?-1:0)
#endif // SIGN

extern bool ResourceIsZIP();

void SetExtension(const char *fnameOld,const char *extension,char *fnameNew)
{
	strcpy(fnameNew,fnameOld);
	int l;
	for(l=strlen(fnameNew)-1;l>=0 && fnameNew[l]!='\\' && fnameNew[l]!='/';l--)
		if(fnameNew[l]=='.')
			break;
	if(l>=0&&fnameNew[l]=='.') 
		strcpy(&fnameNew[l+1],extension);
	else
		strcat(fnameNew,extension);
}


void cFileImage_GetFrameAlpha(void *pDst,int bppDst,int bplDst,int acDst,int asDst,int xDst,int yDst,
							  void *pSrc,int bppSrc,int bplSrc,int acSrc,int asSrc,int xSrc,int ySrc)
{
	assert(bppSrc<=4&&bppDst<=4&&bppSrc>0&&bppDst>0);
	int xSignSrc,ySignSrc,ofsDst=0,ofsSrc=0;
	if(xSrc<0) 
	{ 
		xSrc=-xSrc; 
		xSignSrc=-1; 
		ofsSrc+=(xSrc-1)*bppSrc;
	} 
	else 
		xSignSrc=1;
	if(ySrc<0)
	{ 
		ySrc=-ySrc; 
		ySignSrc=-1; 
		ofsSrc+=(ySrc-1)*bplSrc;
	} 
	else
		ySignSrc=1;
	if(xDst<0) xDst=xSrc;
	if(yDst<0) yDst=ySrc;
	int di=(xSrc<<16)/xDst,dj=(ySrc<<16)/yDst;
	int ke=(di>>16)?di>>16:1,le=(dj>>16)?dj>>16:1,inv_dkl=(1<<16)/(ke*le);
	di*=xSignSrc; dj*=ySignSrc; le*=xSignSrc; ke*=ySignSrc;
	unsigned int amSrc=(1<<acSrc)-1,amDst=(1<<acDst)-1,amDstInv=0xFFFFFFFF^(amDst<<asDst),acDstInv=24-acDst,acSrcInv=8-acSrc;
	char *dst=(char*)pDst,*src=(char*)pSrc;
	for(int jDst=0,jSrc=0;jDst<yDst;jDst++,jSrc+=dj)
	{
		int ofsCurDst=ofsDst+jDst*bplDst,ofsCurSrc=ofsSrc+bplSrc*(ABS(jSrc)>>16)*SIGN(dj);
		for(int iDst=0,iSrc=0;iDst<xDst;iDst++,iSrc+=di)
		{
			unsigned int alpha=0,color=0;
			assert(0<=(ofsCurDst+iDst*bppDst)&&(ofsCurDst+iDst*bppDst+bppDst-1)<bplDst*yDst);
			memcpy(&color,&dst[ofsCurDst+iDst*bppDst],bppDst);
			for(int k=0;k!=ke;k+=ySignSrc)
				for(int l=0;l!=le;l+=xSignSrc)
				{
					unsigned int color=0;
					assert(0<=(ofsCurSrc+((iSrc>>16)+l)*bppSrc+k*bplSrc)&&(ofsCurSrc+((iSrc>>16)+l)*bppSrc+k*bplSrc+bppSrc-1)<bplSrc*ySrc);
					memcpy(&color,&src[ofsCurSrc+((iSrc>>16)+l)*bppSrc+k*bplSrc],bppSrc);
					alpha+=(color>>asSrc)&amSrc;
				}
			alpha=(alpha<<acSrcInv)*inv_dkl;
			color=(color&amDstInv)|((alpha>>acDstInv)<<asDst);
			assert((0<=ofsCurDst+iDst*bppDst)&&(ofsCurDst+iDst*bppDst<bplDst*yDst));
			memcpy(&dst[ofsCurDst+iDst*bppDst],&color,bppDst);
		}
	}
}
void cFileImage_GetFrame(void *pDst,int bppDst,int bplDst,int rcDst,int rsDst,int gcDst,int gsDst,int bcDst,int bsDst,int xDst,int yDst,
						 void *pSrc,int bppSrc,int bplSrc,int rcSrc,int rsSrc,int gcSrc,int gsSrc,int bcSrc,int bsSrc,int xSrc,int ySrc)
{
	assert(bppSrc<=4&&bppDst<=4&&bppSrc>0&&bppDst>0);
	int xSignSrc,ySignSrc,ofsDst=0,ofsSrc=0;
	if(xSrc<0) 
	{ 
		xSrc=-xSrc; 
		xSignSrc=-1; 
		ofsSrc+=(xSrc-1)*bppSrc;
	} 
	else 
		xSignSrc=1;
	if(ySrc<0)
	{ 
		ySrc=-ySrc; 
		ySignSrc=-1; 
		ofsSrc+=(ySrc-1)*bplSrc;
	} 
	else
		ySignSrc=1;
	if(xDst<0) xDst=xSrc;
	if(yDst<0) yDst=ySrc;
	int di=(xSrc<<16)/xDst,dj=(ySrc<<16)/yDst;
	int ke=(di>>16)?di>>16:1,le=(dj>>16)?dj>>16:1,inv_dkl=(1<<16)/(ke*le);
	di*=xSignSrc; dj*=ySignSrc; le*=xSignSrc; ke*=ySignSrc;
	unsigned int rmSrc=(1<<rcSrc)-1,rmDst=(1<<rcDst)-1,rcDstInv=24-rcDst,rcSrcInv=8-rcSrc;
	unsigned int gmSrc=(1<<gcSrc)-1,gmDst=(1<<gcDst)-1,gcDstInv=24-gcDst,gcSrcInv=8-gcSrc;
	unsigned int bmSrc=(1<<bcSrc)-1,bmDst=(1<<bcDst)-1,bcDstInv=24-bcDst,bcSrcInv=8-bcSrc;
	unsigned int rgbmDstInv=0xFFFFFFFF^((rmDst<<rsDst)|(gmDst<<gsDst)|(bmDst<<bsDst));
	char *dst=(char*)pDst,*src=(char*)pSrc;
	for(int jDst=0,jSrc=0;jDst<yDst;jDst++,jSrc+=dj)
	{
		int ofsCurDst=ofsDst+jDst*bplDst,ofsCurSrc=ofsSrc+bplSrc*(ABS(jSrc)>>16)*SIGN(dj);
		for(int iDst=0,iSrc=0;iDst<xDst;iDst++,iSrc+=di)
		{
			unsigned int r=0,g=0,b=0,color=0;
			assert(0<=(ofsCurDst+iDst*bppDst)&&(ofsCurDst+iDst*bppDst+bppDst-1)<bplDst*yDst);
			memcpy(&color,&dst[ofsCurDst+iDst*bppDst],bppDst);
			for(int k=0;k!=ke;k+=ySignSrc)
				for(int l=0;l!=le;l+=xSignSrc)
				{
					unsigned int color=0;
					assert(0<=(ofsCurSrc+((iSrc>>16)+l)*bppSrc+k*bplSrc)&&(ofsCurSrc+((iSrc>>16)+l)*bppSrc+k*bplSrc+bppSrc-1)<bplSrc*ySrc);
					memcpy(&color,&src[ofsCurSrc+((iSrc>>16)+l)*bppSrc+k*bplSrc],bppSrc);
					r+=(color>>rsSrc)&rmSrc;
					g+=(color>>gsSrc)&gmSrc;
					b+=(color>>bsSrc)&bmSrc;
				}
			r=(r<<rcSrcInv)*inv_dkl, g=(g<<gcSrcInv)*inv_dkl, b=(b<<bcSrcInv)*inv_dkl;
			color=(color&rgbmDstInv)|((r>>rcDstInv)<<rsDst)|((g>>gcDstInv)<<gsDst)|((b>>bcDstInv)<<bsDst);
			assert((0<=ofsCurDst+iDst*bppDst)&&(ofsCurDst+iDst*bppDst<bplDst*yDst));
			memcpy(&dst[ofsCurDst+iDst*bppDst],&color,bppDst);
		}
	}
}

void GetDimTexture(int& dx,int& dy,int& count)
{
	int tx,ty;
	tx=ty=0;
	bool cx = false;
	for(int x=512,y=512; x*y>0;)
	{
		if((x/dx)*(y/dy)>=count)
		{
			tx = x;
			ty = y;
		}
		cx = !cx;
		if (cx) x>>=1;
		else y>>=1;
	}
	if (tx*ty==0)
	{
		tx = ty = 512;
		count = (int)(tx/dx)*(int)(ty/dy);
	}
	dx = tx;
	dy = ty;
}

//////////////////////////////////////////////////////////////////////////////////////////
// реализация интерфейса cTGAImage
//////////////////////////////////////////////////////////////////////////////////////////
#if defined(_MSC_VER) || defined(__GNUC__)
#pragma pack(push,1)
#endif

struct TGAHeader
{
uint8_t idFieldLength;
uint8_t colorMapType;
uint8_t imageType;
uint16_t indexFirstColorMapEntry;
uint16_t countColorMapEntries;
uint8_t numberOfBitsPerColorMapEntry;
uint16_t startX;
uint16_t startY;
uint16_t width;
uint16_t height;
uint8_t bitsPerPixel;
uint8_t flags;
};
#if defined(_MSC_VER) || defined(__GNUC__)
#pragma pack(pop)
#endif

bool SaveTga(const char* filename,int width,int height,unsigned char* buf,int byte_per_pixel)
{
	bool bHasAlpha=false;
	int file=_open(filename,_O_WRONLY|_O_TRUNC|_O_CREAT|_O_BINARY,_S_IREAD|_S_IWRITE);
	if(file==-1)
	{
		return false;
	}

	TGAHeader Hdr;
	Hdr.idFieldLength=0;
	Hdr.colorMapType=0;
	Hdr.imageType=byte_per_pixel==1?3:2;
	Hdr.indexFirstColorMapEntry=0;
	Hdr.countColorMapEntries=0;
	Hdr.numberOfBitsPerColorMapEntry=0;
	Hdr.startX=0;
	Hdr.startY=0;
	Hdr.width=(unsigned short)width;
	Hdr.height=(unsigned short)height;
	Hdr.bitsPerPixel=(unsigned char)(byte_per_pixel*8);
	Hdr.flags=(bHasAlpha?8:0)|0x20;

	unsigned long Numbytes=Hdr.width*Hdr.height*(Hdr.bitsPerPixel>>3);

	_write(file,&Hdr,18);
	_write(file,buf,Numbytes);
	_close(file);
    scan_resource_paths(filename);

	return true;
}

bool LoadTGA(const char* filename,int& dx,int& dy,unsigned char*& buf,
			 int& byte_per_pixel)
{
	int file=_open(filename,_O_RDONLY|_O_BINARY);
	if(file==-1)
	{
		return false;
	}

	TGAHeader Hdr;

	_read(file,&Hdr,18);

	byte_per_pixel=Hdr.bitsPerPixel/8;

	_lseek(file,18,SEEK_SET);
	dx=Hdr.width;
	dy=Hdr.height;

	unsigned long Numbytes=Hdr.width*Hdr.height*(Hdr.bitsPerPixel>>3);

	buf=new unsigned char[Hdr.width*Hdr.height*byte_per_pixel];
	_read(file,buf,Numbytes);

	_close(file);

	bool updown=(Hdr.flags&0x20)?false:true;

	if(updown)
	{
		int maxy=dy/2;
		int size=dx*byte_per_pixel;
		unsigned char* tmp=new unsigned char[size];

		for(int y=0;y<maxy;y++)
		{
			unsigned char* p1=buf+y*size;
			unsigned char* p2=buf+(dy-1-y)*size;
			memcpy(tmp,p1,size);
			memcpy(p1,p2,size);
			memcpy(p2,tmp,size);
		}

		delete[] tmp;
	}

	return true;
}

class cTGAImage : public cFileImage
{
	void	  *ImageData;
	TGAHeader *tga;
public:
	cTGAImage()	
	{
		ImageData=NULL;
		tga=NULL;
		length=1;
	}
	virtual ~cTGAImage()										{ close(); }
	virtual int close()
	{
		if(tga) { delete tga; tga=0; }
		else if(ImageData) { delete ImageData; } ImageData=0;
		return 0;
	}
	virtual int load(const char *fname)
	{
		int size=0;
		char *buf=0;
		if(ResourceFileRead(fname,buf,size)) return 1;
		return load(buf,size);
	}
	virtual int load(void *pointer,int size)
	{
		close();
		tga=(TGAHeader*)pointer;
		x=tga->width;
		y=tga->height;
		bpp=tga->bitsPerPixel;
		int colormapsize=(tga->countColorMapEntries*tga->numberOfBitsPerColorMapEntry)/8;
		ImageData=(void*)(18+colormapsize+(char*)pointer);
		int BytePerPixel=bpp/8;
		return 0;
	}
	virtual int save(char *fname,void *pointer,int bpp,int x,int y,int length=1,int time=0)
	{
		if(!SaveTga(fname,x,y,(unsigned char*)pointer,bpp/8))
			return -1;
		return 0;
	}
	virtual int GetTextureAlpha(void *pointer,int time,int bppDst,int bplDst,int acDst,int asDst,int xDst,int yDst)
	{
		int dy=(tga->flags&0x20)?GetY():-GetY();
		if(GetBitPerPixel()==24)
			cFileImage_GetFrameAlpha(pointer,bppDst,bplDst,acDst,asDst,xDst,yDst,
									ImageData,3,GetX()*3,8,0,GetX(),dy);
		else if(GetBitPerPixel()==32)
			cFileImage_GetFrameAlpha(pointer,bppDst,bplDst,acDst,asDst,xDst,yDst,
									ImageData,4,GetX()*4,8,24,GetX(),dy);
		else if(GetBitPerPixel()==16)
			cFileImage_GetFrameAlpha(pointer,bppDst,bplDst,acDst,asDst,xDst,yDst,
									ImageData,2,GetX()*2,31,0,GetX(),dy);
		return 0;
	}
	virtual int GetTexture(void *pointer,int time,int bppDst,int bplDst,int rc,int gc,int bc,int ac,int rs,int gs,int bs,int as,int xDst,int yDst)
	{ 
		int dy=(tga->flags&0x20)?GetY():-GetY();
		if(GetBitPerPixel()==24)
			cFileImage_GetFrame(pointer,bppDst,bplDst,rc,rs,gc,gs,bc,bs,xDst,yDst,
								ImageData,3,GetX()*3,8,16,8,8,8,0,GetX(),dy);
		else if(GetBitPerPixel()==32)
			cFileImage_GetFrame(pointer,bppDst,bplDst,rc,rs,gc,gs,bc,bs,xDst,yDst,
								ImageData,4,GetX()*4,8,16,8,8,8,0,GetX(),dy);
		else if(GetBitPerPixel()==16)
			cFileImage_GetFrame(pointer,bppDst,bplDst,rc,rs,gc,gs,bc,bs,xDst,yDst,
								ImageData,2,GetX()*2,5,10,5,5,5,0,GetX(),dy);
		else if(GetBitPerPixel()==8)
			cFileImage_GetFrame(pointer,bppDst,bplDst,rc,rs,gc,gs,bc,bs,xDst,yDst,
								ImageData,1,GetX()  ,8,0,8,0,8,0,GetX(),dy);
/*		{
//			SaveTga("save.tga",GetX(),GetY(),(BYTE*)ImageData,1);

			for(int y=0;y<GetY();y++)
			{
				DWORD* out=(DWORD*)(bplDst*y+(BYTE*)pointer);
				BYTE* in;

				if(tga->flags&0x20)
					in=y*GetX()+(BYTE*)ImageData;
				else
					in=(GetY()-1-y)*GetX()+(BYTE*)ImageData;

				for(int x=0;x<GetX();x++,out++,in++)
				{
					*out=RGB(*in,*in,*in);
				}
			}
		}
*/						 
		return 0;
	}
};

//////////////////////////////////////////////////////////////////////////////////////////
// реализация интерфейса cAVIImage
//////////////////////////////////////////////////////////////////////////////////////////
#ifdef PERIMETER_FFMPEG

// compatability with newer libavcodec
#if LIBAVCODEC_VERSION_MAJOR < 57
#define AV_FRAME_ALLOC avcodec_alloc_frame
#define AV_PACKET_UNREF av_free_packet
#else
#define AV_FRAME_ALLOC av_frame_alloc
#define AV_PACKET_UNREF av_packet_unref
#endif

#define AV_CODEC_PAR (LIBAVCODEC_VERSION_INT >= AV_VERSION_INT(57, 33, 100))

class cAVIImage : public cFileImage
{
    AVWrapper wrapper;
    int tpf; //Cached time per frame
    std::vector<uint8_t*> frames;
    
public:
    cAVIImage() = default;
    
    ~cAVIImage() override {
        close();
    }
    
    virtual int close() {
        for (uint8_t* frame : frames) {
            delete[] frame;
        }
        frames.clear();
        return wrapper.close();
    }
    
    virtual int load(const char *fname) {
        std::string aviname = convert_path_resource(fname);
        if (aviname.empty()) {
            //VisError <<"cAVIImage File not found:"<<aviname<<VERR_END;
            return 1; // Couldn't open file
        }

        // Open video file
        int ret = wrapper.open(aviname, AVWrapperType::Video);
        if (ret) {
            return ret;
        }

        x = wrapper.videoCodecCtx->width;
        y = wrapper.videoCodecCtx->height;
        
        //Set time (milliseconds of total duration)
        time = static_cast<int>(round(
            static_cast<float>(wrapper.formatCtx->duration) / AV_TIME_BASE * 1000.0f
        ));
        
        //Set bpp
        bpp = wrapper.videoCodecCtx->bits_per_coded_sample;

        //Set length (amount of frames), this seems to be the only way to get for now
        bool reading = true;
        while (reading) {
            AVWrapperType type = wrapper.readPacket();
            switch (type) {
                case None:
                    reading = false;
                    break;
                case Audio:
                    break;
                case Video:
                case AudioVideo:
                    uint8_t* buffer = nullptr;
                    wrapper.getVideoFrameBuffer(&buffer);
                    frames.emplace_back(buffer);
                    break;
            }
        }
        length = static_cast<int>(frames.size());
        
        tpf = (time-1)/(length-1);
        
        return 0;
    }

    virtual int save(char *fname,void *pointer,int bpp,int x,int y,int length=1,int time=0) {
        //TODO is this func even necessary?
        return 0;
    }

    uint8_t* getFrameDataFromTime(int t) {
        int i = t / tpf;
        if (i >= frames.size()) {
            ErrH.Abort("Attempted to read frame " + std::to_string(i) + " which is out of bounds " + std::to_string(frames.size()));
        }
        return frames[i];
    }

    virtual int GetTextureAlpha(void *pointer,int t,int bppDst,int bplDst,int acDst,int asDst,int xDst,int yDst) {
        uint8_t* data = getFrameDataFromTime(t);
        if(GetBitPerPixel()==24)
            cFileImage_GetFrameAlpha(pointer,bppDst,bplDst,acDst,asDst,xDst,yDst,
                                     data,3,GetX()*3,8,0,GetX(),GetY());
        else if(GetBitPerPixel()==32)
            cFileImage_GetFrameAlpha(pointer,bppDst,bplDst,acDst,asDst,xDst,yDst,
                                     data,4,GetX()*4,8,24,GetX(),GetY());
        else if(GetBitPerPixel()==16)
            cFileImage_GetFrameAlpha(pointer,bppDst,bplDst,acDst,asDst,xDst,yDst,
                                     data,2,GetX()*2,31,0,GetX(),GetY());
        return 0;
    }

    virtual int GetTexture(void *pointer,int t,int bppDst,int bplDst,int rc,int gc,int bc,int ac,int rs,int gs,int bs,int as,int xDst,int yDst) {
        uint8_t* data = getFrameDataFromTime(t);
        if(GetBitPerPixel()==24)
            cFileImage_GetFrame(pointer,bppDst,bplDst,rc,rs,gc,gs,bc,bs,xDst,yDst,
                                data,3,GetX()*3,8,16,8,8,8,0,GetX(),GetY());
        else if(GetBitPerPixel()==32)
            cFileImage_GetFrame(pointer,bppDst,bplDst,rc,rs,gc,gs,bc,bs,xDst,yDst,
                                data,4,GetX()*4,8,16,8,8,8,0,GetX(),GetY());
        else if(GetBitPerPixel()==16)
            cFileImage_GetFrame(pointer,bppDst,bplDst,rc,rs,gc,gs,bc,bs,xDst,yDst,
                                data,2,GetX()*2,5,10,5,5,5,0,GetX(),GetY());
        return 0;
    }

    static void Init() {
        AVWrapper::init();
    }

    static void Done() {
    }
};
#else
class cAVIImage : public cFileImage
{
	IGetFrame	*Frame;
	IAVIStream	*pavi;
public:
	cAVIImage()															{ Frame=0; pavi=0; }
	virtual ~cAVIImage()												{ close(); }
	virtual int close()
	{
		if(Frame) AVIStreamGetFrameClose(Frame); Frame=0;
		if(pavi) AVIStreamRelease(pavi); pavi=0;
		return 0;
	}
	virtual int load(const char *fname)
	{
		AVISTREAMINFO FAR psi;

		HRESULT hr=AVIStreamOpenFromFile(&pavi,fname,streamtypeVIDEO,0,OF_READ,NULL);
		if(hr) return 1;
		time=AVIStreamLengthTime(pavi);
		AVIStreamInfo(pavi,&psi,sizeof(AVISTREAMINFO FAR));
		x=psi.rcFrame.right-psi.rcFrame.left;
		y=psi.rcFrame.bottom-psi.rcFrame.top;
		length=psi.dwLength;
		Frame=AVIStreamGetFrameOpen(pavi,NULL);
		BITMAPINFO *pbmi=(BITMAPINFO*)AVIStreamGetFrame(Frame,AVIStreamTimeToSample(pavi,0));
		bpp=pbmi->bmiHeader.biBitCount;
		return 0;
	}
	virtual int save(char *fname,void *pointer,int bpp,int x,int y,int length=1,int time=0)
	{ 
		int err;
		BITMAPINFOHEADER bmh;
		memset(&bmh,0,sizeof(BITMAPINFOHEADER));
		bmh.biSize        = sizeof(BITMAPINFOHEADER);
		bmh.biWidth       = x;
		bmh.biHeight      = y;
		bmh.biPlanes      = 1;
		bmh.biBitCount    = bpp;
		bmh.biCompression = BI_RGB;
		bmh.biSizeImage   = x*y*bpp/8;
		if(bpp!=32) return -2;
		PAVISTREAM pcomp=0;
		PAVIFILE fAVI=0;
		remove(fname);
		err = AVIFileOpen(&fAVI,fname,OF_CREATE|OF_WRITE,NULL);
		if(err)
			return 1;
		AVISTREAMINFO avi;
		memset(&avi,0,sizeof(AVISTREAMINFO));
		avi.fccType    = streamtypeVIDEO;
		avi.fccHandler = mmioFOURCC('D', 'I', 'B', ' ');
		avi.dwScale    = 1;
		avi.dwRate     = length*1000/time;
		avi.dwQuality  = 0;
		avi.dwLength   = length;
		err = AVIFileCreateStream(fAVI,&pavi,&avi);
		if(err)
			return 2;
		AVICOMPRESSOPTIONS compOptions;
		memset(&compOptions, 0, sizeof(AVICOMPRESSOPTIONS));
		compOptions.dwFlags         = AVICOMPRESSF_VALID | AVICOMPRESSF_KEYFRAMES;
		compOptions.fccType         = streamtypeVIDEO;
		compOptions.fccHandler      = avi.fccHandler;
		compOptions.dwQuality       = avi.dwQuality;
		compOptions.dwKeyFrameEvery = 15;
		err=AVIMakeCompressedStream(&pcomp, pavi, &compOptions, NULL);
		if(err)
			return 3;
		err=AVIStreamSetFormat(pcomp,0,&bmh,bmh.biSize);
		if(err)
			return 4;
		unsigned char *buf=new unsigned char[bmh.biSizeImage];
		for(int i=0;i<length;i++)
		{
			for(int j=0;j<y;j++)
				memcpy(&buf[(y-j-1)*bmh.biSizeImage/y],&((LPBYTE)pointer)[i*bmh.biSizeImage+j*bmh.biSizeImage/y],bmh.biSizeImage/y);
			err=AVIStreamWrite(pcomp,i,1,buf,bmh.biSizeImage,AVIIF_KEYFRAME,NULL,NULL);
			if(err)
				return 5;
		}
		delete buf;
		if(pcomp) AVIStreamRelease(pcomp); pcomp=0;
		if(pavi) AVIStreamRelease(pavi); pavi=0;
		if(fAVI) AVIFileRelease(fAVI); fAVI=0;
		return 0; 
	}
	virtual int GetTextureAlpha(void *pointer,int t,int bppDst,int bplDst,int acDst,int asDst,int xDst,int yDst)
	{
		BITMAPINFO *pbmi=(BITMAPINFO*)AVIStreamGetFrame(Frame,AVIStreamTimeToSample(pavi,t%time));
		if(GetBitPerPixel()==24)
			cFileImage_GetFrameAlpha(pointer,bppDst,bplDst,acDst,asDst,xDst,yDst,
									((unsigned char*)pbmi->bmiColors),3,GetX()*3,8,0,GetX(),-y);
		else if(GetBitPerPixel()==32)
			cFileImage_GetFrameAlpha(pointer,bppDst,bplDst,acDst,asDst,xDst,yDst,
									((unsigned char*)pbmi->bmiColors),4,GetX()*4,8,24,GetX(),-y);
		else if(GetBitPerPixel()==16)
			cFileImage_GetFrameAlpha(pointer,bppDst,bplDst,acDst,asDst,xDst,yDst,
									((unsigned char*)pbmi->bmiColors),2,GetX()*2,31,0,GetX(),-y);
		return 0;
	}
	virtual int GetTexture(void *pointer,int t,int bppDst,int bplDst,int rc,int gc,int bc,int ac,int rs,int gs,int bs,int as,int xDst,int yDst)
	{
		BITMAPINFO *pbmi=(BITMAPINFO*)AVIStreamGetFrame(Frame,AVIStreamTimeToSample(pavi,t%time));
		if(pbmi->bmiHeader.biCompression) return 1;
		if(GetBitPerPixel()==24)
			cFileImage_GetFrame(pointer,bppDst,bplDst,rc,rs,gc,gs,bc,bs,xDst,yDst,
								((unsigned char*)pbmi->bmiColors),3,GetX()*3,8,16,8,8,8,0,GetX(),-y);
		else if(GetBitPerPixel()==32)
			cFileImage_GetFrame(pointer,bppDst,bplDst,rc,rs,gc,gs,bc,bs,xDst,yDst,
								((unsigned char*)pbmi->bmiColors),4,GetX()*4,8,16,8,8,8,0,GetX(),-y);
		else if(GetBitPerPixel()==16)
			cFileImage_GetFrame(pointer,bppDst,bplDst,rc,rs,gc,gs,bc,bs,xDst,yDst,
								((unsigned char*)pbmi->bmiColors),2,GetX()*2,5,10,5,5,5,0,GetX(),-y);
		return 0;
	}
	static void Init()
	{ 
		AVIFileInit(); /*opens AVIFile library*/ 
	}
	static void Done()
	{ 
		AVIFileExit(); /*closes AVIFile library*/ 
	}
};
#endif

///////////////////////////////////////////////
///AVIX
class cAVIXImage : public cFileImage
{
	struct AVIX
	{
		DWORD avix;
		DWORD X;
		DWORD Y;
		DWORD bpp;
		DWORD numframe;
		DWORD time;
		char p[1];
	}* data;
public:
	cAVIXImage()														{ data=0; }
	virtual ~cAVIXImage()												{ close(); }
	virtual int close()
	{
		if(data) delete data; data=NULL;
		return 0;
	}
	virtual int load(const char *fname)
	{
		char* buf;
		int size;
		char name[512];
		strcpy(name,fname);
		strcat(name,"x");
		int ret=ResourceFileRead(name,buf,size);
		if(ret)
			return ret;
		data=(AVIX*)buf;

		time=data->time;
		x=data->X;
		y=data->Y;
		length=data->numframe;
		bpp=data->bpp;
		return 0;
	}
	virtual int save(char *fname,void *pointer,int bpp,int x,int y,int length=1,int time=0)
	{ 
		return 0; 
	}
	virtual int GetTextureAlpha(void *pointer,int t,int bppDst,int bplDst,int acDst,int asDst,int xDst,int yDst)
	{
		void *pbmi=GetFrameByte((t/GetTimePerFrame())%length);
		if(GetBitPerPixel()==24)
			cFileImage_GetFrameAlpha(pointer,bppDst,bplDst,acDst,asDst,xDst,yDst,
									pbmi,3,GetX()*3,8,0,GetX(),y);
		else if(GetBitPerPixel()==32)
			cFileImage_GetFrameAlpha(pointer,bppDst,bplDst,acDst,asDst,xDst,yDst,
									pbmi,4,GetX()*4,8,24,GetX(),y);
		else if(GetBitPerPixel()==16)
			cFileImage_GetFrameAlpha(pointer,bppDst,bplDst,acDst,asDst,xDst,yDst,
									pbmi,2,GetX()*2,31,0,GetX(),y);
		return 0;
	}
	virtual int GetTexture(void *pointer,int t,int bppDst,int bplDst,int rc,int gc,int bc,int ac,int rs,int gs,int bs,int as,int xDst,int yDst)
	{
		void *pbmi=GetFrameByte((t/GetTimePerFrame())%length);
		if(GetBitPerPixel()==24)
			cFileImage_GetFrame(pointer,bppDst,bplDst,rc,rs,gc,gs,bc,bs,xDst,yDst,
								pbmi,3,GetX()*3,8,16,8,8,8,0,GetX(),y);
		else if(GetBitPerPixel()==32)
			cFileImage_GetFrame(pointer,bppDst,bplDst,rc,rs,gc,gs,bc,bs,xDst,yDst,
								pbmi,4,GetX()*4,8,16,8,8,8,0,GetX(),y);
		else if(GetBitPerPixel()==16)
			cFileImage_GetFrame(pointer,bppDst,bplDst,rc,rs,gc,gs,bc,bs,xDst,yDst,
								pbmi,2,GetX()*2,5,10,5,5,5,0,GetX(),y);
		return 0;
	}
	static void Init()													{ }
	static void Done()													{ }

	void* GetFrameByte(int n)
	{
		int fsize=x*y*(bpp>>3);
		return data->p+fsize*n;
	}

	int GetTimePerFrame()
	{
		if(GetLength()<=1) 
			return 1;
		else
			return (GetTime()-1)/(GetLength()-1);
	}
};

//////////////////////////////////////////////////////////////////////////////////////////
// реализация интерфейса cFileImage
cFileImage* cFileImage::Create(const char *fname)
{
	_strlwr((char*)fname);
	if(strstr(fname,".tga"))
		return new cTGAImage;
	else if(strstr(fname,".avi"))
		return ResourceIsZIP()?(cFileImage*)new cAVIXImage:(cFileImage*)new cAVIImage;
	return 0;
}
void cFileImage::InitFileImage()
{
	cAVIImage::Init();
}
void cFileImage::DoneFileImage()
{
	cAVIImage::Done();
}
void GetFileName(const char *FullName,char *fname)
{
	fname[0]=0;
	if(FullName==0||FullName[0]==0) return;
	int l=strlen(FullName)-1;
	while(l>=0 && FullName[l]!='\\' && FullName[l]!='/') 
		l--;
	strcpy(fname,&FullName[l+1]);
}
void GetFilePath(const char *FullName,char *path)
{
	path[0]=0;
	if(FullName==0||FullName[0]==0) return;
	int l=strlen(FullName)-1;
	while(l>=0 && FullName[l]!='\\' && FullName[l]!='/') 
		l--;
	memcpy(path,FullName,l);
	path[l]=0;
}
void GetFileVirginName(const char *FullName,char *name)
{
	name[0]=0;
	GetFileName(FullName,name);
	if(name==0||name[0]==0) return;
	int l=strlen(name)-1;
	while(l>=0&&name[l]!='.') 
		l--;
	name[l]=0; l--;
	while(l>=0&&'0'<=name[l]&&name[l]<='9')
		l--;
	name[l+1]=0;
}
int CmpFileVirginName(const char *fname1,const char *fname2)
{
	char name1[1024],name2[1024];
	GetFileName(fname1,name1); _strlwr(name1);
	GetFileName(fname2,name2); _strlwr(name2);
	int l1=strlen(name1),l2=strlen(name2);
	if(l1!=l2) return 0;
	while(l1>=0&&(name1[l1]==name2[l1]||('0'<=name1[l1]&&name1[l1]<='9'&&'0'<=name2[l1]&&name2[l1]<='9')))
		l1--;
	return l1<0;
}
#ifdef _UTILTVA_
int ResourceFileRead(const char *filename,char *&buf,int &size)
{
	buf=0; size=0;
	int file=_open(filename,_O_RDONLY|_O_BINARY);
	if(file==-1) return -1;
	

	size=_lseek(file,0,SEEK_END);
	if(size<=0) { _close(file); return -2; }
	_lseek(file,0,SEEK_SET);
	buf=new char[size];
	_read(file,buf,size);
	_close(file);
	return 0;
}
bool ResourceIsZIP()
{
	return false;
}

#endif// _UTILTVA_

////////////////////// Реализация cAviScaleFileImage ///////////////////////////

cAviScaleFileImage::cAviScaleFileImage():cFileImage()
{
	dat = NULL;
}

bool cAviScaleFileImage::Init(const char* fName)
{
	char name[1024];
	strcpy(name,fName);
	cFileImage* FileImage=cFileImage::Create(name);
	if (FileImage && !FileImage->load(fName))
	{
		dx = FileImage->GetX();
		dy = FileImage->GetY();
		int rdx = dx+2;
		int rdy = dy+2;
		int t1 = dx+2;
		int t2 = dx+2;
		int t3 = FileImage->GetLength();
		GetDimTexture(t1,t2,t3);//change parameters
		x = t1;
		y = t2;
		n_count = t3;
		const int x_count = x/rdx;

		dat = new UINT[x*y*n_count];
		if (dat)	
		{
			UINT* lpBuf = new  UINT[dx*dy];
			if (lpBuf) 
			{
				int time=0;
				for(int i=0;i<n_count;++i)
				{
					FileImage->GetTexture(lpBuf,time,4,4*dx,
							8,8,8,8, 16,8,0,24, dx, dy );
					FileImage->GetTextureAlpha(lpBuf,time,4,4*dx,
							8, 24, dx, dy );
					const int offset_y = rdy*(i/x_count)*this->x + rdx*(i%x_count);
					for(int y=0; y<dy; ++y)
					{
						UINT* p = dat + (int)((y+1)*x) + offset_y;
						memcpy(p+1, lpBuf + (int)(y*dx), dx*sizeof(*lpBuf));
						*(p) = lpBuf[(int)(y*dx)];
						*(p+(int)dx+1)= lpBuf[(int)(y*dx+dx-1)];
					}
					memcpy(dat + offset_y, dat + (int)(x) + offset_y, (dx+2)*sizeof(*lpBuf));
					memcpy(dat + (int)((dy+1)*x) + offset_y, dat + (int)(dy*x) + offset_y, (dx+2)*sizeof(*lpBuf));
					time+=FileImage->GetTime()/FileImage->GetLength();
				}
				delete[] lpBuf;
			}else return false;
		}else return false;
	}
	delete[] FileImage;

	return true;
}
cAviScaleFileImage::~cAviScaleFileImage()
{
	delete[] dat;
}

int cAviScaleFileImage::GetTexture(void *pointer,int t,int bppDst,int bplDst,int rc,int gc,int bc,int ac,int rs,int gs,int bs,int as,int xDst,int yDst)
{
	xassert(dat!=0);
//	cFileImage_GetFrame(pointer,bppDst,bplDst,rc,rs,gc,gs,bc,bs,xDst,yDst,
//				dat,4,GetX()*4,8,16,8,8,8,0,GetX(),-y);
	xassert(sizeof(*dat)==4);
	memcpy(pointer,dat,x*y*sizeof(*dat));
	return 0; 
}
//////////////////////////////////////////////////////////////////////
