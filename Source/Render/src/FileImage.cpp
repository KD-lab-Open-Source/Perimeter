#include "tweaks.h"
#include <stdio.h>
#include <cstdint>
#include <memory>
#include "xmath.h"

#ifdef PERIMETER_FFMPEG
#include "AVWrapper.h"
#else // PERIMETER_FFMPEG
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#include <windows.h>
#include <vfw.h>		// AVI include
#endif //_WIN32
#include <math.h>
#include <sys/types.h>
#include "xutil.h"
#endif //PERIMETER_FFMPEG

#include "xutil.h"
#include "FileImage.h"

#include <fcntl.h>
#include <sys/stat.h>

#include <SDL_image.h>

#include "files/files.h"
#include "ANIFile.h"


#if (!defined(_FINAL_VERSION_) || defined(_DEBUG)) && !defined(NASSERT)
#include <iostream>
#endif

int ResourceFileRead(const char *fname,char *&buf,int &size);

#define FI_ABS(a)										((a)>=0?(a):-(a))
#define FI_SIGN(a)										((a)>0?1:((a)<0)?-1:0)

extern bool ResourceIsZIP();

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
		int ofsCurDst=ofsDst+jDst*bplDst,ofsCurSrc=ofsSrc+bplSrc*(FI_ABS(jSrc)>>16)*FI_SIGN(dj);
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
		int ofsCurDst=ofsDst+jDst*bplDst,ofsCurSrc=ofsSrc+bplSrc*(FI_ABS(jSrc)>>16)*FI_SIGN(dj);
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
#pragma pack(push,1)

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

#pragma pack(pop)

bool SaveTGA(const char* filename,int width,int height,unsigned char* buf,int byte_per_pixel)
{
	bool bHasAlpha=false;
	int file= file_open(filename, _O_WRONLY | _O_TRUNC | _O_CREAT | _O_BINARY, _S_IREAD | _S_IWRITE);
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

	uint32_t Numbytes=Hdr.width*Hdr.height*(Hdr.bitsPerPixel>>3);

	_write(file,&Hdr,18);
	_write(file,buf,Numbytes);
	_close(file);
    scan_resource_paths(filename);

	return true;
}

bool LoadTGA(const char* filename,int& dx,int& dy,unsigned char*& buf,
			 int& byte_per_pixel)
{
	int file= file_open(filename, _O_RDONLY | _O_BINARY);
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

    uint32_t Numbytes=Hdr.width*Hdr.height*(Hdr.bitsPerPixel>>3);

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
    uint8_t	  *ImageData;
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
		if(tga) { delete[] tga; tga=0; }
		else if(ImageData) { delete[] ImageData; } ImageData=0;
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
		tga=static_cast<TGAHeader*>(pointer);
		x=tga->width;
		y=tga->height;
		bpp=tga->bitsPerPixel;
		int colormapsize=(tga->countColorMapEntries*tga->numberOfBitsPerColorMapEntry)/8;
		ImageData=(18+colormapsize+static_cast<uint8_t*>(pointer));
		return 0;
	}
	virtual int save(char *fname,void *pointer,int bpp,int x,int y,int length=1,int time=0)
	{
		if(!SaveTGA(fname,x,y,(unsigned char*)pointer,bpp/8))
			return -1;
		return 0;
	}
	
	int GetTextureAlpha(void *pointer,int time,int bppDst,int bplDst,int acDst,int asDst,int xDst,int yDst) override
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
	
	int GetTextureRGB(void *pointer,int time,int bppDst,int bplDst,int rc,int gc,int bc,int rs,int gs,int bs,int xDst,int yDst) override
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
//			SaveTga("save.tga",GetX(),GetY(),(uint8_t*)ImageData,1);

			for(int y=0;y<GetY();y++)
			{
				uint32_t* out=(uint32_t*)(bplDst*y+(uint8_t*)pointer);
				uint8_t* in;

				if(tga->flags&0x20)
					in=y*GetX()+(uint8_t*)ImageData;
				else
					in=(GetY()-1-y)*GetX()+(uint8_t*)ImageData;

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
    int tpf = 1; //Cached time per frame
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
        std::string aviname = convert_path_content(fname);
        if (aviname.empty()) {
            //VisError <<"cAVIImage File not found:"<<aviname<<VERR_END;
            return 1; // Couldn't open file
        }

        // Open video file
        int ret = wrapper.open(aviname, AVWrapperType::Video);
        if (ret) {
            return ret;
        }

        x = wrapper.getVideoWidth();
        y = wrapper.getVideoHeight();
        
        //Set time (milliseconds of total duration)
        time = static_cast<int>(xm::round(
            wrapper.getDuration() * 1000.0f
        ));
        
        //Set bpp
        bpp = wrapper.getVideoBPP();

        //Read all wrapper packets
        while (!wrapper.end) {
            wrapper.readPacket();
        }
        //Discard any audio frames
        wrapper.audioFrames.clear();
        //Order video frames
        wrapper.videoFrames.sort(AVWrapperFrame_compare);
        for (auto frame : wrapper.videoFrames) {
            //Read video frame and make buffer
            uint8_t* buffer = nullptr;
            frame->copyBuffer(&buffer);
            frames.emplace_back(buffer);
        }
        wrapper.close();
        length = static_cast<int>(frames.size());

        if (length <= 1) {
            tpf = 1;
        } else {
            tpf = (time - 1) / (length - 1);
        }
        
        return 0;
    }

    virtual int save(char *fname,void *pointer,int bpp,int x,int y,int length=1,int time=0) {
        return 0;
    }

    uint8_t* getFrameDataFromTime(int t) {
        int i = t / tpf;
        if (i >= frames.size()) {
            ErrH.Abort("Attempted to read frame " + std::to_string(i) + " which is out of bounds " + std::to_string(frames.size()));
        }
        return frames[i];
    }

    int GetTextureAlpha(void *pointer,int t,int bppDst,int bplDst,int acDst,int asDst,int xDst,int yDst) override {
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

    int GetTextureRGB(void *pointer,int t,int bppDst,int bplDst,int rc,int gc,int bc,int rs,int gs,int bs,int xDst,int yDst) override {
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
	IGetFrame	*Frame = nullptr;
	IAVIStream	*pavi = nullptr;
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
        
        std::string aviname = convert_path_content(fname);
        if (aviname.empty()) {
            return 1; // Couldn't find file
        }

		HRESULT hr=AVIStreamOpenFromFileA(&pavi,aviname.c_str(),streamtypeVIDEO,0,OF_READ,NULL);
		if (hr) {
            fprintf(stderr, "cAVIImage couldn't open: %s %s\n", fname, aviname.c_str());
            return 1;
        }
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
		std::remove(fname);
		err = AVIFileOpenA(&fAVI,fname,OF_CREATE|OF_WRITE,NULL);
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
	
	int GetTextureAlpha(void *pointer,int t,int bppDst,int bplDst,int acDst,int asDst,int xDst,int yDst) override
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
	
	int GetTextureRGB(void *pointer,int t,int bppDst,int bplDst,int rc,int gc,int bc,int rs,int gs,int bs,int xDst,int yDst) override
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
///////////////////////////////////////////////
class cAVIXImage : public cFileImage
{
	struct AVIX
	{
		uint32_t avix;
		uint32_t X;
		uint32_t Y;
		uint32_t bpp;
		uint32_t numframe;
		uint32_t time;
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
		std::string name = fname;
        if (endsWith(name, ".avi")) {
            name += 'x';
        }
		int ret=ResourceFileRead(name.c_str(),buf,size);
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
	
	
	int GetTextureAlpha(void *pointer,int t,int bppDst,int bplDst,int acDst,int asDst,int xDst,int yDst) override
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
	
	
	int GetTextureRGB(void *pointer,int t,int bppDst,int bplDst,int rc,int gc,int bc,int rs,int gs,int bs,int xDst,int yDst) override
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

///////////////////////////////////////////////
///FileImage wrapper that uses SDL_image for image loading 
///////////////////////////////////////////////

#if SDL_IMAGE_VERSION_ATLEAST(2, 0, 7)
//On version 2.0.7 and onwards should support 24 bits ICO/CUR images
#define SDL_IMAGE_ICOCUR_24
#endif

class cSDLImage : public cFileImage
{
#ifndef SDL_IMAGE_ICOCUR_24
public:
    //Method copied from SDL_image to support 24 bits CUR
    static SDL_Surface* LoadICOCUR_RW_Custom(SDL_RWops * src, int type, int freesrc) {
        SDL_bool was_error;
        long fp_offset;
        int bmpPitch;
        int i, pad;
        SDL_Surface *surface;
        Uint8 *bits;
        int ExpandBMP;
        int maxCol = 0;
        int icoOfs = 0;
        Uint32 palette[256];
        /* The Win32 ICO file header (14 bytes) */
        Uint16 bfReserved;
        Uint16 bfType;
        Uint16 bfCount;

        /* The Win32 BITMAPINFOHEADER struct (40 bytes) */
        Uint32 biSize;
        Sint32 biWidth;
        Sint32 biHeight;
        Uint16 biBitCount;
        Uint32 biCompression;
        Uint32 biClrUsed;

        /* Make sure we are passed a valid data source */
        surface = NULL;
        was_error = SDL_FALSE;
        if (src == NULL) {
            was_error = SDL_TRUE;
            goto done;
        }

        /* Read in the ICO file header */
        fp_offset = SDL_RWtell(src);
        SDL_ClearError();

        bfReserved = SDL_ReadLE16(src);
        bfType = SDL_ReadLE16(src);
        bfCount = SDL_ReadLE16(src);
        if ((bfReserved != 0) || (bfType != type) || (bfCount == 0)) {
            IMG_SetError("File is not a Windows %s file", type == 1 ? "ICO" : "CUR");
            was_error = SDL_TRUE;
            goto done;
        }

        /* Read the Win32 Icon Directory */
        for (i = 0; i < bfCount; i++) {
            /* Icon Directory Entries */
            int bWidth = SDL_ReadU8(src);    /* Uint8, but 0 = 256 ! */
            int bHeight = SDL_ReadU8(src);   /* Uint8, but 0 = 256 ! */
            int bColorCount = SDL_ReadU8(src);       /* Uint8, but 0 = 256 ! */
            SDL_ReadU8(src);
            SDL_ReadLE16(src); //Hotspot X
            SDL_ReadLE16(src); //Hotspot Y
            SDL_ReadLE32(src);
            Uint32 dwImageOffset = SDL_ReadLE32(src);

            if (!bWidth)
                bWidth = 256;
            if (!bHeight)
                bHeight = 256;
            if (!bColorCount)
                bColorCount = 256;

            //printf("%dx%d@%d - %08x\n", bWidth, bHeight, bColorCount, dwImageOffset);
            if (bColorCount > maxCol) {
                maxCol = bColorCount;
                icoOfs = dwImageOffset;
                //printf("marked\n");
            }
        }

        /* Advance to the DIB Data */
        if (SDL_RWseek(src, icoOfs, RW_SEEK_SET) < 0) {
            SDL_Error(SDL_EFSEEK);
            was_error = SDL_TRUE;
            goto done;
        }

        /* Read the Win32 BITMAPINFOHEADER */
        biSize = SDL_ReadLE32(src);
        if (biSize == 40) {
            biWidth = SDL_ReadLE32(src);
            biHeight = SDL_ReadLE32(src);
            SDL_ReadLE16(src);
            biBitCount = SDL_ReadLE16(src);
            biCompression = SDL_ReadLE32(src);
            SDL_ReadLE32(src);
            SDL_ReadLE32(src);
            SDL_ReadLE32(src);
            biClrUsed = SDL_ReadLE32(src);
            SDL_ReadLE32(src);
        } else {
            IMG_SetError("Unsupported ICO bitmap format");
            was_error = SDL_TRUE;
            goto done;
        }

        /* Check for read error */
        if (SDL_strcmp(SDL_GetError(), "") != 0) {
            was_error = SDL_TRUE;
            goto done;
        }

        /* We don't support any BMP compression right now */
        switch (biCompression) {
            case 0:
                /* Default values for the BMP format */
                switch (biBitCount) {
                    case 1:
                    case 4:
                        ExpandBMP = biBitCount;
                        biBitCount = 8;
                        break;
                    case 8:
                        ExpandBMP = 8;
                        break;
                    case 24:
                        ExpandBMP = 24;
                        break;
                    case 32:
                        ExpandBMP = 0;
                        break;
                    default:
                        IMG_SetError("ICO file with unsupported bit count");
                        was_error = SDL_TRUE;
                        goto done;
                }
                break;
            default:
                IMG_SetError("Compressed ICO files not supported");
                was_error = SDL_TRUE;
                goto done;
        }

        /* Create a RGBA surface */
        biHeight = biHeight >> 1;
        //printf("%d x %d\n", biWidth, biHeight);
        surface =
                SDL_CreateRGBSurface(0, biWidth, biHeight, 32, 0x00FF0000,
                                     0x0000FF00, 0x000000FF, 0xFF000000);
        if (surface == NULL) {
            was_error = SDL_TRUE;
            goto done;
        }

        /* Load the palette, if any */
        //printf("bc %d bused %d\n", biBitCount, biClrUsed);
        if (biBitCount <= 8) {
            if (biClrUsed == 0) {
                biClrUsed = 1 << biBitCount;
            }
            for (i = 0; i < (int) biClrUsed; ++i) {
                SDL_RWread(src, &palette[i], 4, 1);
            }
        }

        /* Read the surface pixels.  Note that the bmp image is upside down */
        bits = (Uint8 *) surface->pixels + (surface->h * surface->pitch);
        switch (ExpandBMP) {
            case 1:
                bmpPitch = (biWidth + 7) >> 3;
                pad = (((bmpPitch) % 4) ? (4 - ((bmpPitch) % 4)) : 0);
                break;
            case 4:
                bmpPitch = (biWidth + 1) >> 1;
                pad = (((bmpPitch) % 4) ? (4 - ((bmpPitch) % 4)) : 0);
                break;
            case 8:
                bmpPitch = biWidth;
                pad = (((bmpPitch) % 4) ? (4 - ((bmpPitch) % 4)) : 0);
                break;
            case 24:
                bmpPitch = biWidth * 3;
                pad = (((bmpPitch) % 4) ? (4 - ((bmpPitch) % 4)) : 0);
                break;
            default:
                bmpPitch = biWidth * 4;
                pad = 0;
                break;
        }
        while (bits > (Uint8 *) surface->pixels) {
            bits -= surface->pitch;
            switch (ExpandBMP) {
                case 1:
                case 4:
                case 8:
                {
                    Uint8 pixel = 0;
                    int shift = (8 - ExpandBMP);
                    for (i = 0; i < surface->w; ++i) {
                        if (i % (8 / ExpandBMP) == 0) {
                            if (!SDL_RWread(src, &pixel, 1, 1)) {
                                IMG_SetError("Error reading from ICO");
                                was_error = SDL_TRUE;
                                goto done;
                            }
                        }
                        *((Uint32 *) bits + i) = (palette[pixel >> shift]);
                        pixel <<= ExpandBMP;
                    }
                }
                    break;
                case 24:
                {
                    Uint32 pixel;
                    Uint8 channel;
                    for (i = 0; i < surface->w; ++i) {
                        pixel = 0;
                        for (int j = 0; j < 3; ++j) {
                            //Load each color channel into pixel
                            if (!SDL_RWread(src, &channel, 1, 1)) {
                                IMG_SetError("Error reading from ICO");
                                was_error = SDL_TRUE;
                                goto done;
                            }
                            pixel |= (channel << (j * 8));
                            
                        }
                        *((Uint32 *) bits + i) = pixel;
                    }
                }
                    break;

                default:
                    if (SDL_RWread(src, bits, 1, surface->pitch)
                        != surface->pitch) {
                        SDL_Error(SDL_EFREAD);
                        was_error = SDL_TRUE;
                        goto done;
                    }
                    break;
            }
            /* Skip padding bytes, ugh */
            if (pad) {
                Uint8 padbyte;
                for (i = 0; i < pad; ++i) {
                    SDL_RWread(src, &padbyte, 1, 1);
                }
            }
        }
        /* Read the mask pixels.  Note that the bmp image is upside down */
        bits = (Uint8 *) surface->pixels + (surface->h * surface->pitch);
        ExpandBMP = 1;
        bmpPitch = (biWidth + 7) >> 3;
        pad = (((bmpPitch) % 4) ? (4 - ((bmpPitch) % 4)) : 0);
        while (bits > (Uint8 *) surface->pixels) {
            Uint8 pixel = 0;
            int shift = (8 - ExpandBMP);

            bits -= surface->pitch;
            for (i = 0; i < surface->w; ++i) {
                if (i % (8 / ExpandBMP) == 0) {
                    if (!SDL_RWread(src, &pixel, 1, 1)) {
                        IMG_SetError("Error reading from ICO");
                        was_error = SDL_TRUE;
                        goto done;
                    }
                }
                *((Uint32 *) bits + i) |= ((pixel >> shift) ? 0 : 0xFF000000);
                pixel <<= ExpandBMP;
            }
            /* Skip padding bytes, ugh */
            if (pad) {
                Uint8 padbyte;
                for (i = 0; i < pad; ++i) {
                    SDL_RWread(src, &padbyte, 1, 1);
                }
            }
        }
        done:
        if (was_error) {
            if (src) {
                SDL_RWseek(src, fp_offset, RW_SEEK_SET);
            }
            if (surface) {
                SDL_FreeSurface(surface);
            }
            surface = NULL;
        }
        if (freesrc && src) {
            SDL_RWclose(src);
        }
        return (surface);
    }
#endif
    
protected:
    SDL_Surface* image;
    
public:
    cSDLImage() {
    }

    virtual ~cSDLImage() {
        close();
    }

    virtual int close() {
        if (image) {
            SDL_FreeSurface(image);
            image = nullptr;
        }
        return 0;
    }

    ///Loads from file
    virtual int load(const char *fname)
    {
        std::string file_path = convert_path_content(fname);
        if (file_path.empty()) {
            return 1;
        }
        SDL_RWops* src = SDL_RWFromFile(file_path.c_str(), "rb");
        int ret = loadRW(src);
        if (ret) {
            fprintf(stderr, "Error loading image file: %s\n", file_path.c_str());
        }
        return ret;
    }
    
    ///Loads from provided RWops
    int loadRW(SDL_RWops* src) {
#ifdef SDL_IMAGE_ICOCUR_24
        image = IMG_Load_RW(src, 1);
#else
        bool isICO = IMG_isICO(src);
        if (isICO || IMG_isCUR(src)) {
            image = LoadICOCUR_RW_Custom(src, isICO ? 1 : 2, 1);
        } else {
            image = IMG_Load_RW(src, 1);
        }
#endif
        if(!image) {
            fprintf(stderr, "IMG_Load_RW: %s\n", IMG_GetError());
            return 1;
        }

        //All cursors are 32x32
        x = image->h;
        y = image->w;
        time = 0;
        length = 1;
        bpp = image->format->BitsPerPixel;
        
        return 0;
    }

    virtual int save(char *fname,void *pointer,int bpp,int x,int y,int length=1,int time=0) {
        return 0;
    }

    int GetTextureAlpha(void *pointer,int t,int bppDst,int bplDst,int acDst,int asDst,int xDst,int yDst) override {
        uint8_t* pbmi = static_cast<uint8_t*>(image->pixels);
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

    int GetTextureRGB(void *pointer,int t,int bppDst,int bplDst,int rc,int gc,int bc,int rs,int gs,int bs,int xDst,int yDst) override {
        uint8_t* pbmi = static_cast<uint8_t*>(image->pixels);
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

    static void Init() {
        IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG);
    }

    static void Done() {
        IMG_Quit();
    }
};

///////////////////////////////////////////////
///ANI
///////////////////////////////////////////////
//Since we can't use animated system cursors outside win32, we load them as animated textures and render normally

class cANIImage : public cFileImage
{
    int tpf; //Time per frame in ms

    //Frames used by this animation, we store individually to free them later
    std::vector<cSDLImage*> frames;
    //Frame for each slice that will be used to draw, may contain duplicates of frames
    std::vector<cSDLImage*> slices;

public:
    cANIImage() {
    }
    
    virtual ~cANIImage() {
        close();
    }
    
    virtual int close() {
        for (auto frame : frames) {
            int ret = frame->close();
            if (ret) {
                return ret;
            }
            delete frame;
        }
        frames.clear();
        slices.clear();
        return 0;
    }
    
    virtual int load(const char *fname)
    {
        x = y = 0;
        ANIFile ani;
        int err = ani.load(fname, true);
        if (err) {
            close();
            return 1;
        }

        //Create image from data in mem and add to frames
        frames.clear();
        for (auto& frame : ani.frames) {
            SDL_RWops* ops = SDL_RWFromConstMem(frame.data, static_cast<int>(frame.data_len));
            cSDLImage* frame_image = new cSDLImage();
            err = frame_image->loadRW(ops);
            if (err) {
                delete frame_image;
                close();
                return 1;
            }
            x = max(x, frame_image->GetX());
            y = max(y, frame_image->GetX());
            frames.emplace_back(frame_image);
        }
        
        time = ani.time;
        length = ani.length;
        tpf = ani.tpf;

        //ICO/CUR are always 32 bits per pixel surfaces
        bpp = 32;
        
        //Get each image for slice and populate slices
        slices.clear();
        for (auto& index : ani.slices) {
            cSDLImage* ptr = frames[index];
            slices.emplace_back(ptr);
        }

        return 0;
    }
    
    virtual int save(char *fname,void *pointer,int bpp,int x,int y,int length=1,int time=0) {
        return 0;
    }

    cSDLImage* getFrameFromTime(int t) {
        int i = t / tpf;
#ifdef PERIMETER_DEBUG_ASSERT
        if (i >= slices.size()) {
            ErrH.Abort("Attempted to read slice " + std::to_string(i) + " which is out of bounds " + std::to_string(slices.size()));
        }
#endif
        return slices[i];
    }

    virtual int GetTextureAlpha(void *pointer,int t,int bppDst,int bplDst,int acDst,int asDst,int xDst,int yDst) {
        return getFrameFromTime(t)->GetTextureAlpha(pointer, t, bppDst, bplDst, acDst, asDst, xDst, yDst);
    }

    virtual int GetTextureRGB(void *pointer,int t,int bppDst,int bplDst,int rc,int gc,int bc,int rs,int gs,int bs,int xDst,int yDst) {
        return getFrameFromTime(t)->GetTextureRGB(pointer, t, bppDst, bplDst, rc, gc, bc, rs, gs, bs, xDst, yDst);
    }
};

//////////////////////////////////////////////////////////////////////////////////////////
// реализация интерфейса cFileImage
cFileImage* cFileImage::Create(const std::string& fname)
{
    std::string path = string_to_lower(fname.c_str());
	if(endsWith(path,".tga")) {
        return new cTGAImage;
    } else if(endsWith(path,".avi")) {
        if (ResourceIsZIP() && convert_path_content(path).empty()) {
            return new cAVIXImage;
        } else {
            return new cAVIImage;
        }
    } else if(endsWith(path,".avix")) {
        return new cAVIXImage;
    } else if(endsWith(path,".cur") || endsWith(path,".ani")) {
	    //Since we don't know which ".cur" files are actually CUR or ANI... we do runtime checking
        if (isANIFile(path.c_str())) {
            return new cANIImage;
        } else {
            return new cSDLImage;
        }
    } else if(endsWith(path,".jpg") || endsWith(path,".jpe") || endsWith(path,".png")) {
        return new cSDLImage;
    } else {
        fprintf(stderr, "Unknown image type %s\n", fname.c_str());
    }
	return nullptr;
}
void cFileImage::InitFileImage()
{
	cAVIImage::Init();
    cSDLImage::Init();
}
void cFileImage::DoneFileImage()
{
	cAVIImage::Done();
    cSDLImage::Done();
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

		dat = new uint32_t[x * y * n_count];
		if (dat)	
		{
			uint32_t* lpBuf = new  uint32_t[dx * dy];
			if (lpBuf) 
			{
				int time=0;
				for(int i=0;i<n_count;++i)
				{
					FileImage->GetTextureRGB(lpBuf,time,4,4*dx,
							8,8,8, 16,8,0, dx, dy );
					FileImage->GetTextureAlpha(lpBuf,time,4,4*dx,
							8, 24, dx, dy );
					const int offset_y = rdy*(i/x_count)*this->x + rdx*(i%x_count);
					for(int y=0; y<dy; ++y)
					{
						uint32_t* p = dat + (int)((y + 1) * x) + offset_y;
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

int cAviScaleFileImage::GetTextureRGB(void *pointer,int t,int bppDst,int bplDst,int rc,int gc,int bc,int rs,int gs,int bs,int xDst,int yDst)
{
	xassert(dat!=0);
//	cFileImage_GetFrame(pointer,bppDst,bplDst,rc,rs,gc,gs,bc,bs,xDst,yDst,
//				dat,4,GetX()*4,8,16,8,8,8,0,GetX(),-y);
	xassert(sizeof(*dat)==4);
	memcpy(pointer,dat,x*y*sizeof(*dat));
	return 0; 
}
//////////////////////////////////////////////////////////////////////
