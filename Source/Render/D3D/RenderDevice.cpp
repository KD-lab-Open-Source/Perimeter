#include "StdAfxRD.h"
#include "xutil.h"

#include "Font.h"

#pragma comment(lib,"dxguid.lib")
//#pragma comment(lib,"ddraw.lib")
#pragma comment(lib,"d3dx9.lib")
#pragma comment(lib,"d3d9.lib")

FILE* fRD=NULL;

char* GetErrorText(HRESULT hr)
{
switch(hr)
{
case D3D_OK :
return "No error occurred. ";
case D3DERR_CONFLICTINGRENDERSTATE :
return "The currently set render states cannot be used together. ";
case D3DERR_CONFLICTINGTEXTUREFILTER :
return "The current texture filters cannot be used together. ";
case D3DERR_CONFLICTINGTEXTUREPALETTE :
return "The current textures cannot be used simultaneously. This generally occurs when a multitexture device requires that all palletized textures simultaneously enabled also share the same palette. ";
case D3DERR_DEVICELOST :
return "The device is lost and cannot be restored at the current time, so rendering is not possible. ";
case D3DERR_DEVICENOTRESET :
return "The device cannot be reset. ";
case D3DERR_DRIVERINTERNALERROR :
return "Internal driver error. ";
case D3DERR_INVALIDCALL :
return "The method call is invalid. For example, a method's parameter may have an invalid value. ";
case D3DERR_INVALIDDEVICE :
return "The requested device type is not valid. ";
case D3DERR_MOREDATA :
return "There is more data available than the specified buffer size can hold. ";
case D3DERR_NOTAVAILABLE :
return "This device does not support the queried technique. ";
case D3DERR_NOTFOUND :
return "The requested item was not found. ";
case D3DERR_OUTOFVIDEOMEMORY :
return "Direct3D does not have enough display memory to perform the operation. ";
case D3DERR_TOOMANYOPERATIONS :
return "The application is requesting more texture-filtering operations than the device supports. ";
case D3DERR_UNSUPPORTEDALPHAARG :
return "The device does not support a specified texture-blending argument for the alpha channel. ";
case D3DERR_UNSUPPORTEDALPHAOPERATION :
return "The device does not support a specified texture-blending operation for the alpha channel. ";
case D3DERR_UNSUPPORTEDCOLORARG :
return "The device does not support a specified texture-blending argument for color values. ";
case D3DERR_UNSUPPORTEDCOLOROPERATION :
return "The device does not support a specified texture-blending operation for color values. ";
case D3DERR_UNSUPPORTEDFACTORVALUE :
return "The device does not support the specified texture factor value. ";
case D3DERR_UNSUPPORTEDTEXTUREFILTER :
return "The device does not support the specified texture filter. ";
case D3DERR_WRONGTEXTUREFORMAT :
return "The pixel format of the texture surface is not valid. ";
case E_FAIL :
return "An undetermined error occurred inside the Direct3D subsystem. ";
case E_INVALIDARG :
return "An invalid parameter was passed to the returning function ";
//case E_INVALIDCALL :
//return "The method call is invalid. For example, a method's parameter may have an invalid value. ";
case E_OUTOFMEMORY :
return "Direct3D could not allocate sufficient memory to complete the call. ";
}
return "Unknown error";
};

void RDOpenLog(char *fname="RenderDevice.!!!")
{
	fRD=fopen(fname,"wt");
	fprintf(fRD,"----------------- Compilation data: %s time: %s -----------------\n",__DATE__,__TIME__);
}
int RDWriteLog(HRESULT err,char *exp,char *file,int line)
{
#ifndef _FINAL_VERSION_
	if(fRD==0) RDOpenLog();
	fprintf(fRD,"%s line: %i - %s = 0x%X , %s\n",file,line,exp,err,GetErrorText(err));
	fflush(fRD);
#endif
	return err;
}
void RDWriteLog(char *exp,int size)
{
#ifndef _FINAL_VERSION_
	if(fRD==0) RDOpenLog();
	if(size==-1)
		size=strlen(exp);
	fwrite(exp,size,1,fRD);
	fprintf(fRD,"\n");
	fflush(fRD);
#endif
}

void RDWriteLog(DDSURFACEDESC2 &ddsd)
{
#ifndef _FINAL_VERSION_
	if(fRD==0) RDOpenLog();
	fprintf(fRD,"DDSURFACEDESC2\n{\n");
	fprintf(fRD,"   dwSize            = %i\n",ddsd.dwSize);
	fprintf(fRD,"   dwFlags           = 0x%X\n",ddsd.dwFlags);
	fprintf(fRD,"   dwHeight          = %i\n",ddsd.dwHeight);;
	fprintf(fRD,"   dwWidth           = %i\n",ddsd.dwWidth);
	fprintf(fRD,"   lPitch            = %i\n",ddsd.lPitch);
	fprintf(fRD,"   dwBackBufferCount = %i\n",ddsd.dwBackBufferCount);
	fprintf(fRD,"   dwMipMapCount     = %i\n",ddsd.dwMipMapCount);
	fprintf(fRD,"   dwAlphaBitDepth   = %i\n",ddsd.dwAlphaBitDepth);
	fprintf(fRD,"   dwReserved        = %i\n",ddsd.dwReserved);
	fprintf(fRD,"   lpSurface         = 0x%X\n",(int)ddsd.lpSurface);
	fprintf(fRD,"   ddckCKDestOverlay = %i - %i\n",ddsd.ddckCKDestOverlay.dwColorSpaceLowValue,ddsd.ddckCKDestOverlay.dwColorSpaceHighValue);
	fprintf(fRD,"   ddckCKDestBlt     = %i - %i\n",ddsd.ddckCKDestBlt.dwColorSpaceLowValue,ddsd.ddckCKDestBlt.dwColorSpaceHighValue);
	fprintf(fRD,"   ddckCKSrcOverlay  = %i - %i\n",ddsd.ddckCKSrcOverlay.dwColorSpaceLowValue,ddsd.ddckCKSrcOverlay.dwColorSpaceHighValue);
	fprintf(fRD,"   ddckCKSrcBlt      = %i - %i\n",ddsd.ddckCKSrcBlt.dwColorSpaceLowValue,ddsd.ddckCKSrcBlt.dwColorSpaceHighValue);
/*
	DDPIXELFORMAT ddpfPixelFormat;
		DDSCAPS2      ddsCaps;
*/
	fprintf(fRD,"   dwTextureStage    = %i\n",ddsd.dwTextureStage);
	fprintf(fRD,"}\n");
	fflush(fRD);
#endif
}

cURenderDevice::cURenderDevice()
{ 
	NumberPolygon=0; 
	NumDrawObject=0;
	Buffer.New(600000);//-> Buffer=600000,MaxVertexSize=10*4,VertexNumMin=Buffer/MaxVertexSize=600000/40=15000
	RenderMode=0;
	xScr=yScr=0;
	xScrMin=yScrMin=xScrMax=yScrMax=0;

	bActiveScene=0;
	bWireFrame=0;
	nSupportTexture=0;
	
	DrawNode=0;
	DefaultFont=CurrentFont=0;
}

cURenderDevice::~cURenderDevice()
{ 
	Buffer.Delete();
	VISASSERT(RenderMode==0);
	VISASSERT(xScr==0&&yScr==0&&xScrMin==0&&yScrMin==0&&xScrMax==0&&yScrMax==0);
	VISASSERT(xScr==0&&yScr==0);
	VISASSERT(CurrentFont==0 || CurrentFont==DefaultFont);
}

void cURenderDevice::SetFont(cFont *pFont)
{
	CurrentFont=pFont;
	if(!CurrentFont)
		CurrentFont=DefaultFont;
}

void cURenderDevice::SetDefaultFont(cFont *pFont)
{
	DefaultFont=pFont;
	if(!CurrentFont)
		CurrentFont=DefaultFont;
}

int cURenderDevice::GetSizeFromFmt(int fmt)
{
	int size=0;

	int position=fmt&D3DFVF_POSITION_MASK;
	switch(position)
	{
	case D3DFVF_XYZ:
		size+=3*sizeof(float);
		break;
	case D3DFVF_XYZRHW:
	case D3DFVF_XYZW:
		size+=4*sizeof(float);
		break;
	case D3DFVF_XYZB1:
		size+=4*sizeof(float);
		break;
	case D3DFVF_XYZB2:
		size+=5*sizeof(float);
		break;
	case D3DFVF_XYZB3:
		size+=6*sizeof(float);
		break;
	case D3DFVF_XYZB4:
		size+=7*sizeof(float);
		break;
	case D3DFVF_XYZB5:
		size+=8*sizeof(float);
		break;
	default:
		xassert(0);
	}

	if(fmt&D3DFVF_NORMAL) size+=3*sizeof(float);
	if(fmt&D3DFVF_DIFFUSE) size+=sizeof(unsigned int);
	if(fmt&D3DFVF_SPECULAR) size+=sizeof(unsigned int);

	size += ((fmt & D3DFVF_TEXCOUNT_MASK) >> D3DFVF_TEXCOUNT_SHIFT) * 2 * sizeof(float);
	for(int i=0;i<4;i++)
	{
		if((fmt&D3DFVF_TEXCOORDSIZE3(i))==D3DFVF_TEXCOORDSIZE3(i))
			size+=sizeof(float);
	}
	return size;
}

void BuildBumpMap(int xs,int ys,void *pSrc,void *pDst,int fmtBumpMap)
{
	int xm=xs-1,ym=ys-1;
	unsigned char *dst=(unsigned char*)pDst,*src=(unsigned char*)pSrc;
    for( int y=0; y<ys; y++ )
        for( int x=0; x<xs; x++ )
        {
			int v00 = src[4*(x+y*xs)+0];
            int v01 = src[4*(((x+1)&xm)+y*xs)+0];
            int vM1 = src[4*(((x-1)&xm)+y*xs)+0];
            int v10 = src[4*(x+((y+1)&ym)*xs)+0];
            int v1M = src[4*(x+((y-1)&ym)*xs)+0];
            int iDu = vM1-v01, iDv = v1M-v10;
            if( v00<vM1 && v00<v01 )
            {
                iDu = vM1-v00;
                if( iDu < v00-v01 ) iDu = v00-v01;
            }
            int uL = ( v00>1 ) ? 63 : 127;
            switch( fmtBumpMap )
            {
                case D3DFMT_V8U8:
                    *dst++ = (BYTE)iDu;
                    *dst++ = (BYTE)iDv;
                    break;

                case D3DFMT_L6V5U5:
                    *(WORD*)dst  = (WORD)( ( (iDu>>3) & 0x1f ) <<  0 ) 
						| (WORD)( ( (iDv>>3) & 0x1f ) <<  5 )
						| (WORD)( ( ( uL>>2) & 0x3f ) << 10 );
                    dst += 2;
                    break;

                case D3DFMT_X8L8V8U8:
                    *dst++ = (BYTE)iDu;
                    *dst++ = (BYTE)iDv;
                    *dst++ = (BYTE)uL;
                    *dst++ = (BYTE)0L;
                    break;
            }
        }
}
void BuildDot3Map(int xs,int ys,void *pSrc,void *pDst)
{ // трансформация карты цветов в карту нормалей
	float scale=255*2*0.1f;
	Vect3f TexNormal(0.5f*255,0.5f*255,0.5f*255);
	unsigned int *dst=(unsigned int *)pDst,*src=(unsigned int *)pSrc;
	int xm=xs-1,ym=ys-1;
	for(int j=0;j<ys;j++)
		for(int i=0;i<xs;i++)
		{
			unsigned int hl=src[((i-1)&xm)+j*xs]&255;
			unsigned int hr=src[((i+1)&xm)+j*xs]&255;
			unsigned int hd=src[i+((j-1)&ym)*xs]&255;
			unsigned int hu=src[i+((j+1)&ym)*xs]&255;
			Vect3f a(scale,0,hr-hl),b(0,scale,hu-hd);
			Vect3f n; n.cross(a,b); n.normalize(); 
			n=(n+Vect3f(1,1,1))*TexNormal;
			dst[i+j*xs]=(round(n.x)<<16)|(round(n.y)<<8)|(round(n.z)<<0);
		}
}

unsigned int ColorByNormal(Vect3f n);
Vect3f NormalByColor(DWORD d);

void BuildMipMap(int x,int y,int bpp,int bplSrc,void *pSrc,int bplDst,void *pDst,
				 int rc,int gc,int bc,int ac,int rs,int gs,int bs,int as,int Attr)
{
	DDSCAPS2 ddsCaps;
	ddsCaps.dwCaps  = DDSCAPS_TEXTURE | DDSCAPS_MIPMAP;
	ddsCaps.dwCaps2 = 0;
	ddsCaps.dwCaps3 = 0;
	ddsCaps.dwCaps4 = 0;
	char *Src=(char*)pSrc,*Dst=(char*)pDst;
	int ofsDst=bplDst-x*bpp, ofsSrc=bplSrc-2*x*bpp;
	int rm=(1<<rc)-1,gm=(1<<gc)-1,bm=(1<<bc)-1,am=(1<<ac)-1,xm=x-1,ym=y-1;

	if(Attr&TEXTURE_BUMP)
	{
		for(int j=0;j<y;j++,Dst+=ofsDst,Src+=ofsSrc+bplSrc)
		for(int i=0,color;i<x;i++,Dst+=bpp,Src+=2*bpp)
		{
			unsigned int p00=*((unsigned int*)&Src[0]),p01=*((unsigned int*)&Src[bpp]),
				p10=*((unsigned int*)&Src[bplSrc]),p11=*((unsigned int*)&Src[bplSrc+bpp]);
			Vect3f v=NormalByColor(p00)+NormalByColor(p01)+
					 NormalByColor(p10)+NormalByColor(p11);
			v.normalize();
			color=ColorByNormal(v);
			memcpy(Dst,&color,bpp);
		}
	}else
	if((Attr&TEXTURE_MIPMAPBLUR)==0)
		for(int j=0;j<y;j++,Dst+=ofsDst,Src+=ofsSrc+bplSrc)
			for(int i=0,color;i<x;i++,Dst+=bpp,Src+=2*bpp)
			{
				if( (Attr&TEXTURE_MIPMAP_POINT)==0 )
				{ // bilinear/trilinear mipmap
					unsigned int p00=*((unsigned int*)&Src[0]),p01=*((unsigned int*)&Src[bpp]),
						p10=*((unsigned int*)&Src[bplSrc]),p11=*((unsigned int*)&Src[bplSrc+bpp]);
					int r=(((p00>>rs)&rm)+((p01>>rs)&rm)+((p10>>rs)&rm)+((p11>>rs)&rm))>>2;
					int g=(((p00>>gs)&gm)+((p01>>gs)&gm)+((p10>>gs)&gm)+((p11>>gs)&gm))>>2;
					int b=(((p00>>bs)&bm)+((p01>>bs)&bm)+((p10>>bs)&bm)+((p11>>bs)&bm))>>2;
					int a=(((p00>>as)&am)+((p01>>as)&am)+((p10>>as)&am)+((p11>>as)&am))>>2;
					color=(r<<rs)|(g<<gs)|(b<<bs)|(a<<as);
				}
				else // nearest mipmap
					color=*((unsigned int*)&Src[0]);
				memcpy(Dst,&color,bpp);
			}
	else 
	{
		unsigned char *rgba=new unsigned char [4*x*y];
		unsigned char *buf=rgba;
		int j;
		for(j=0;j<y;j++,Src+=ofsSrc+bplSrc)
			for(int i=0;i<x;i++,buf+=4,Src+=2*bpp)
			{
				unsigned int p00=*((unsigned int*)&Src[0]),p01=*((unsigned int*)&Src[bpp]),
					p10=*((unsigned int*)&Src[bplSrc]),p11=*((unsigned int*)&Src[bplSrc+bpp]);
				buf[0]=(((p00>>rs)&rm)+((p01>>rs)&rm)+((p10>>rs)&rm)+((p11>>rs)&rm))>>2;
				buf[1]=(((p00>>gs)&gm)+((p01>>gs)&gm)+((p10>>gs)&gm)+((p11>>gs)&gm))>>2;
				buf[2]=(((p00>>bs)&bm)+((p01>>bs)&bm)+((p10>>bs)&bm)+((p11>>bs)&bm))>>2;
				buf[3]=(((p00>>as)&am)+((p01>>as)&am)+((p10>>as)&am)+((p11>>as)&am))>>2;
			}
		const int	k00=4,  k01=8,	k02=8,  k03=8,  k04=4;
		const int	k10=8,  k11=16,	k12=16, k13=16, k14=8;
		const int	k20=8,  k21=16,	k22=16, k23=16, k24=8;
		const int	k30=8,  k31=16,	k32=16, k33=16, k34=8;
		const int	k40=4,  k41=8,	k42=8,  k43=8,  k44=4;
		if((Attr&TEXTURE_BLURWHITE)==0) 
			for(j=0;j<y;j++,Dst+=ofsDst)
				for(int i=0;i<x;i++,Dst+=bpp)
				{
					int iMin2=4*((i-2)&xm),iMin1=4*((i-1)&xm),iNull=4*i,iMax1=4*((i+1)&xm),iMax2=4*((i+2)&xm),
						jMin2=4*x*((j+2)&ym),jMin1=4*x*((j+1)&ym),jNull=4*x*j,jMax1=4*x*((j+1)&ym),jMax2=4*x*((j+2)&ym);
					unsigned char *p00=&rgba[iMin2+jMin2],*p01=&rgba[iMin1+jMin2],*p02=&rgba[iNull+jMin2],*p03=&rgba[iMax1+jMin2],*p04=&rgba[iMax2+jMin2],
						*p10=&rgba[iMin2+jMin1],*p11=&rgba[iMin1+jMin1],*p12=&rgba[iNull+jMin1],*p13=&rgba[iMax1+jMin1],*p14=&rgba[iMax2+jMin1],
						*p20=&rgba[iMin2+jNull],*p21=&rgba[iMin1+jNull],*p22=&rgba[iNull+jNull],*p23=&rgba[iMax1+jNull],*p24=&rgba[iMax2+jNull],
						*p30=&rgba[iMin2+jMax1],*p31=&rgba[iMin1+jMax1],*p32=&rgba[iNull+jMax1],*p33=&rgba[iMax1+jMax1],*p34=&rgba[iMax2+jMax1],
						*p40=&rgba[iMin2+jMax2],*p41=&rgba[iMin1+jMax2],*p42=&rgba[iNull+jMax2],*p43=&rgba[iMax1+jMax2],*p44=&rgba[iMax2+jMax2];
					int r =(p00[0]*k00+p01[0]*k01+p02[0]*k02+p03[0]*k03+p04[0]*k04+
							p10[0]*k10+p11[0]*k11+p12[0]*k12+p13[0]*k13+p14[0]*k14+
							p20[0]*k20+p21[0]*k21+p22[0]*k22+p23[0]*k23+p24[0]*k24+
							p30[0]*k30+p31[0]*k31+p32[0]*k32+p33[0]*k33+p34[0]*k34+
							p40[0]*k40+p41[0]*k41+p42[0]*k42+p43[0]*k43+p44[0]*k44)>>8;
					int g =(p00[1]*k00+p01[1]*k01+p02[1]*k02+p03[1]*k03+p04[1]*k04+
							p10[1]*k10+p11[1]*k11+p12[1]*k12+p13[1]*k13+p14[1]*k14+
							p20[1]*k20+p21[1]*k21+p22[1]*k22+p23[1]*k23+p24[1]*k24+
							p30[1]*k30+p31[1]*k31+p32[1]*k32+p33[1]*k33+p34[1]*k34+
							p40[1]*k40+p41[1]*k41+p42[1]*k42+p43[1]*k43+p44[1]*k44)>>8;
					int b =(p00[2]*k00+p01[2]*k01+p02[2]*k02+p03[2]*k03+p04[2]*k04+
							p10[2]*k10+p11[2]*k11+p12[2]*k12+p13[2]*k13+p14[2]*k14+
							p20[2]*k20+p21[2]*k21+p22[2]*k22+p23[2]*k23+p24[2]*k24+
							p30[2]*k30+p31[2]*k31+p32[2]*k32+p33[2]*k33+p34[2]*k34+
							p40[2]*k40+p41[2]*k41+p42[2]*k42+p43[2]*k43+p44[2]*k44)>>8;
					int a =(p00[3]*k00+p01[3]*k01+p02[3]*k02+p03[3]*k03+p04[3]*k04+
							p10[3]*k10+p11[3]*k11+p12[3]*k12+p13[3]*k13+p14[3]*k14+
							p20[3]*k20+p21[3]*k21+p22[3]*k22+p23[3]*k23+p24[3]*k24+
							p30[3]*k30+p31[3]*k31+p32[3]*k32+p33[3]*k33+p34[3]*k34+
							p40[3]*k40+p41[3]*k41+p42[3]*k42+p43[3]*k43+p44[3]*k44)>>8;
					int color=(r<<rs)|(g<<gs)|(b<<bs)|(a<<as);
					memcpy(Dst,&color,bpp);
				}
		else
			for(j=0;j<y;j++,Dst+=ofsDst)
				for(int i=0;i<x;i++,Dst+=bpp)
				{
					const int AddWhite=2*256+255;
					int iMin2=4*((i-2)&xm),iMin1=4*((i-1)&xm),iNull=4*i,iMax1=4*((i+1)&xm),iMax2=4*((i+2)&xm),
						jMin2=4*x*((j+2)&ym),jMin1=4*x*((j+1)&ym),jNull=4*x*j,jMax1=4*x*((j+1)&ym),jMax2=4*x*((j+2)&ym);
					unsigned char *p00=&rgba[iMin2+jMin2],*p01=&rgba[iMin1+jMin2],*p02=&rgba[iNull+jMin2],*p03=&rgba[iMax1+jMin2],*p04=&rgba[iMax2+jMin2],
						*p10=&rgba[iMin2+jMin1],*p11=&rgba[iMin1+jMin1],*p12=&rgba[iNull+jMin1],*p13=&rgba[iMax1+jMin1],*p14=&rgba[iMax2+jMin1],
						*p20=&rgba[iMin2+jNull],*p21=&rgba[iMin1+jNull],*p22=&rgba[iNull+jNull],*p23=&rgba[iMax1+jNull],*p24=&rgba[iMax2+jNull],
						*p30=&rgba[iMin2+jMax1],*p31=&rgba[iMin1+jMax1],*p32=&rgba[iNull+jMax1],*p33=&rgba[iMax1+jMax1],*p34=&rgba[iMax2+jMax1],
						*p40=&rgba[iMin2+jMax2],*p41=&rgba[iMin1+jMax2],*p42=&rgba[iNull+jMax2],*p43=&rgba[iMax1+jMax2],*p44=&rgba[iMax2+jMax2];
					int r =(p00[0]*k00+p01[0]*k01+p02[0]*k02+p03[0]*k03+p04[0]*k04+
							p10[0]*k10+p11[0]*k11+p12[0]*k12+p13[0]*k13+p14[0]*k14+
							p20[0]*k20+p21[0]*k21+p22[0]*k22+p23[0]*k23+p24[0]*k24+
							p30[0]*k30+p31[0]*k31+p32[0]*k32+p33[0]*k33+p34[0]*k34+
							p40[0]*k40+p41[0]*k41+p42[0]*k42+p43[0]*k43+p44[0]*k44+AddWhite)>>8;
					int g =(p00[1]*k00+p01[1]*k01+p02[1]*k02+p03[1]*k03+p04[1]*k04+
							p10[1]*k10+p11[1]*k11+p12[1]*k12+p13[1]*k13+p14[1]*k14+
							p20[1]*k20+p21[1]*k21+p22[1]*k22+p23[1]*k23+p24[1]*k24+
							p30[1]*k30+p31[1]*k31+p32[1]*k32+p33[1]*k33+p34[1]*k34+
							p40[1]*k40+p41[1]*k41+p42[1]*k42+p43[1]*k43+p44[1]*k44+AddWhite)>>8;
					int b =(p00[2]*k00+p01[2]*k01+p02[2]*k02+p03[2]*k03+p04[2]*k04+
							p10[2]*k10+p11[2]*k11+p12[2]*k12+p13[2]*k13+p14[2]*k14+
							p20[2]*k20+p21[2]*k21+p22[2]*k22+p23[2]*k23+p24[2]*k24+
							p30[2]*k30+p31[2]*k31+p32[2]*k32+p33[2]*k33+p34[2]*k34+
							p40[2]*k40+p41[2]*k41+p42[2]*k42+p43[2]*k43+p44[2]*k44+AddWhite)>>8;
					int a =(p00[3]*k00+p01[3]*k01+p02[3]*k02+p03[3]*k03+p04[3]*k04+
							p10[3]*k10+p11[3]*k11+p12[3]*k12+p13[3]*k13+p14[3]*k14+
							p20[3]*k20+p21[3]*k21+p22[3]*k22+p23[3]*k23+p24[3]*k24+
							p30[3]*k30+p31[3]*k31+p32[3]*k32+p33[3]*k33+p34[3]*k34+
							p40[3]*k40+p41[3]*k41+p42[3]*k42+p43[3]*k43+p44[3]*k44)>>8;
					if(r>rm)r=rm; if(g>gm)g=gm;	if(b>bm)b=bm;
					int color=(r<<rs)|(g<<gs)|(b<<bs)|(a<<as);
					memcpy(Dst,&color,bpp);
				}
		delete rgba;
	}
}

cURenderDevice *gb_RenderDevice=0;

cInterfaceRenderDevice* CreateIRenderDevice()
{
	return gb_RenderDevice=new cD3DRender;
}
