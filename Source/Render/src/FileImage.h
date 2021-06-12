#ifndef __FILEIMAGE_H__
#define __FILEIMAGE_H__

class cFileImage
{
public:
	cFileImage()																{ x=y=length=bpp=time=fmt=0; }
	virtual ~cFileImage()														{}
	virtual int load(void *pointer,int size)									{ return -1; }
	virtual int load(const char *fname)												{ return -1; }
	virtual int save(const char *fname,void *pointer,int bpp,int x,int y,int length=1,int time=0){ return -1; }
	virtual int close()															{ return -1; }
	virtual int GetTextureAlpha(void *pointer,int time,int bpp,int bpl,
		int aBitCount=8,int aBitShift=24,int xSize=-1,int ySize=-1)				{ return -1; }
	virtual int GetTexture(void *pointer,int time,int bpp,int bpl,
		int rBitCount,int gBitCount,int bBitCount,int aBitCount,
		int rBitShift,int gBitShift,int bBitShift,int aBitShift,
		int xSize=-1,int ySize=-1)												{ return -1; }
	inline int GetX()															{ if(x>=0) return x; return -x; }
	inline int GetY()															{ if(y>=0) return y; return -y; }
	inline int GetFmt()															{ return fmt; }
	inline int GetTime()														{ return time; }
	inline int GetLength()														{ return length; }
	inline int GetBitPerPixel()													{ return bpp; }
	static void InitFileImage();
	static void DoneFileImage();
	static cFileImage* Create(const char *fname);
protected:
	int	x,y,bpp,length,time,fmt;
};
class cAviScaleFileImage : public cFileImage
{
	UINT* dat;
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
	virtual int GetTexture(void *pointer,int time,int bpp,int bpl,
		int rBitCount,int gBitCount,int bBitCount,int aBitCount,
		int rBitShift,int gBitShift,int bBitShift,int aBitShift,
		int xSize=-1,int ySize=-1);
};

extern void cFileImage_GetFrame(void *pDst,int bppDst,int bplDst,int rcDst,int rsDst,int gcDst,int gsDst,int bcDst,int bsDst,int xDst,int yDst,
								void *pSrc,int bppSrc,int bplSrc,int rcSrc,int rsSrc,int gcSrc,int gsSrc,int bcSrc,int bsSrc,int xSrc,int ySrc);
extern void cFileImage_GetFrameAlpha(void *pDst,int bppDst,int bplDst,int acDst,int asDst,int xDst,int yDst,
								void *pSrc,int bppSrc,int bplSrc,int acSrc,int asSrc,int xSrc,int ySrc);

extern void GetFileName(const char *FullName,char *fname);				// возвращает имя файла, обрезает каталог
extern void GetFilePath(const char *FullName,char *path);				// возвращает каталог
extern void GetFileVirginName(const char *FullName,char *name);			// возвращает имя файла, обрезает каталог, удаляет расширение и номер в имени
extern int CmpFileVirginName(const char *fname1,const char *fname2);	// сравнивает имена файлов, без учета номеров

void SetExtension(const char *fnameOld,const char *extension,char *fnameNew);

bool SaveTga(const char* filename,int width,int height,unsigned char* buf,int byte_per_pixel);
bool LoadTGA(const char* filename,int& dx,int& dy,unsigned char*& buf,int& byte_per_pixel);
extern void GetDimTexture(int& dx,int& dy,int& count);

#endif //__FILEIMAGE_H__