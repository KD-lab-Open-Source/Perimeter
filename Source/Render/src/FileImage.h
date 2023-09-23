#ifndef __FILEIMAGE_H__
#define __FILEIMAGE_H__

class cFileImage
{
protected:
    int	x,y; //Size
    int bpp; //Bits per pixel
    int length; //Amount of frames if is animated or 1
    int time; //Total time in ms if animated or 0
public:
	cFileImage()																{ x=y=length=bpp=time=0; }
	virtual ~cFileImage()														{}
	virtual int load(void *pointer,int size)									{ return -1; }
	virtual int load(const char *fname)												{ return -1; }
	virtual int save(const char *fname,void *pointer,int bpp,int x,int y,int length=1,int time=0){ return -1; }
	virtual int close()															{ return -1; }
	virtual int GetTextureAlpha(void *pointer,int time,int bpp,int bpl,
		int aBitCount=8,int aBitShift=24,int xSize=-1,int ySize=-1)				{ return -1; }
	virtual int GetTextureRGB(void *pointer,int time,int bpp,int bpl,
		int rBitCount,int gBitCount,int bBitCount,
		int rBitShift,int gBitShift,int bBitShift,
		int xSize=-1,int ySize=-1)												{ return -1; }
	inline int GetX()															{ if(x>=0) return x; return -x; }
	inline int GetY()															{ if(y>=0) return y; return -y; }
	inline int GetTime()														{ return time; }
	inline int GetLength()														{ return length; }
	inline int GetBitPerPixel()													{ return bpp; }
	static void InitFileImage();
	static void DoneFileImage();
	static cFileImage* Create(const std::string& fname);
};
class cAviScaleFileImage : public cFileImage
{
	uint32_t* dat;
	int dx;
	int dy;
	int n_count;
public:
	cAviScaleFileImage();
	~cAviScaleFileImage();
	bool Init(const char* fName);
	int GetXFrame()		{return dx+2;}
	int GetYFrame()		{return dy+2;}
	int GetFramesCount()	{return n_count;}
	virtual int GetTextureAlpha(void *pointer,int time,int bpp,int bpl,
		int aBitCount=8,int aBitShift=24,int xSize=-1,int ySize=-1)
	{
		return 0;
	}
	virtual int GetTextureRGB(void *pointer,int time,int bpp,int bpl,
		int rBitCount,int gBitCount,int bBitCount,
		int rBitShift,int gBitShift,int bBitShift,
		int xSize=-1,int ySize=-1);
};

extern void cFileImage_GetFrame(void *pDst,int bppDst,int bplDst,int rcDst,int rsDst,int gcDst,int gsDst,int bcDst,int bsDst,int xDst,int yDst,
								void *pSrc,int bppSrc,int bplSrc,int rcSrc,int rsSrc,int gcSrc,int gsSrc,int bcSrc,int bsSrc,int xSrc,int ySrc);
extern void cFileImage_GetFrameAlpha(void *pDst,int bppDst,int bplDst,int acDst,int asDst,int xDst,int yDst,
								void *pSrc,int bppSrc,int bplSrc,int acSrc,int asSrc,int xSrc,int ySrc);

extern void GetFileName(const char *FullName,char *fname);				// возвращает имя файла, обрезает каталог
extern void GetFilePath(const char *FullName,char *path);				// возвращает каталог

bool SaveTGA(const char* filename,int width,int height,unsigned char* buf,int byte_per_pixel);
bool LoadTGA(const char* filename,int& dx,int& dy,unsigned char*& buf,int& byte_per_pixel);
extern void GetDimTexture(int& dx,int& dy,int& count);

#endif //__FILEIMAGE_H__