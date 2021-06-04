#include "stdafxTr.h"

#include "sur_scr.h"

/* ----------------------------- EXTERN SECTION ---------------------------- */
/* --------------------------- PROTOTYPE SECTION --------------------------- */
/* --------------------------- DEFINITION SECTION -------------------------- */
//BitMap placeBMP;
//BitMap mosaicBMP(1);

sVBitMapMosaic VBitMapMosaic;
sVBitMap VBitMap;


extern int* xRad[MAX_RADIUS_CIRCLEARR + 1];
extern int* yRad[MAX_RADIUS_CIRCLEARR + 1];
extern int maxRad[MAX_RADIUS_CIRCLEARR + 1];



//////////////////////////////////////////////////////////////////////////////

#define MIN(a,b)	(((a) < (b))?(a):(b))
#define MAX(a,b)	(((a) > (b))?(a):(b))
/*
void BitMap::place(char* name,int x,int y,int _force,int _mode,int _border,int _level,int _size,int _Kmod,int _modeC2H,int alpha,int ter_type)
{
	force = _force;
	mode = _mode;
	border = _border;
	level = _level;
	size = _size;
	modeC2H = _modeC2H;
	Kmod=_Kmod;


	load(name);
	if (!modeC2H) convert();

//	int x = curGMap -> CX;
//	int y = curGMap -> CY;
//	unsigned char** lt = vMap -> lineT;
	unsigned char* bmp = data;
//	unsigned char* pa,*pa0;
//	unsigned char* pv,*pv0;
	int dzBMP;

	if(alpha==90){
		x=vMap.XCYCL(x- sx/2);
		y=vMap.YCYCL(y - sy/2);

	//	vMap -> increase(y,y + sy - 1);

		register int i,j;
		int v,xx,yy,vv;
		for(j = 0;j < sy;j++){
			yy = vMap.YCYCL(y + j);
			//if(!pv0) continue;
			for(i = 0;i < sx;i++,bmp++){
				//pv = pv0 + (xx = vMap.XCYCL(x + i));
				xx = vMap.XCYCL(x + i);
				//v = *pv;
				v=vMap.GetAlt(xx,yy);
				if(force || *bmp){
					dzBMP=round((float)(*bmp) * ((float)Kmod/100.0));
					if(border > 0 && *bmp == 0xFF)
						vv = 0;
					else switch(mode){
						case 0:
							vv = dzBMP;
							break;
						case 1:
							vv = MAX(dzBMP,v);
							break;
						case 2:
							vv = MIN(dzBMP,v);
							break;
						case 3:
							vv = (dzBMP + v) >> 1;
							break;
						case 4:
							vv = v + dzBMP;
							break;
					}
				vv += level;
#ifdef _SURMAP_
				vMap.voxSet(xx,yy,vv - v);
#else
				vMap.voxSet(xx,yy,vv - v,ter_type);
#endif
				}
			}
		}
		vMap.regRender(x,y,x + sx,y + sy);
	}
	else {
		int ddx=round(sin((double)alpha*3.1415926535/180)*(1<<16));
		int ddy=round(cos((double)alpha*3.1415926535/180)*(1<<16));
		int xbegr,xbeg=x*(1<<16) - (sx/2)*ddx + (sy/2)*ddy;
		int ybegr,ybeg=y*(1<<16) - (sy/2)*ddx - (sx/2)*ddy;
		xbegr=xbeg;ybegr=ybeg;

		int xt,yt;
		int xpp=0,ypp=0;

		register int i,j;
		int v,xx,yy,vv;
		for(j = 0;j < sy;j++){
			xt=xbeg; yt=ybeg;
			for(i = 0;i < sx;i++,bmp++){
				xx=vMap.XCYCL(((xt+(1 << 15))>>16)+xpp); yy=vMap.YCYCL(((yt+(1 << 15))>>16)+ypp);
				v=vMap.GetAlt(xx,yy);
				if(force || *bmp){
					dzBMP=(*bmp)*Kmod;
					if(border > 0 && *bmp == 0xFF)
						vv = 0;
					else
						switch(mode){
							case 0:
								vv = dzBMP;
								break;
							case 1:
								vv = MAX(dzBMP,v);
								break;
							case 2:
								vv = MIN(dzBMP,v);
								break;
							case 3:
								vv = (dzBMP + v) >> 1;
								break;
							case 4:
								vv = v + dzBMP;
								break;
							}
					vv += level;
#ifdef _SURMAP_
					vMap.voxSet(xx,yy,vv - v);
#else
					vMap.voxSet(xx,yy,vv - v,ter_type);
#endif
					}
				xt+=ddx; yt+=ddy;
				}
			//xbeg-=ddy;ybeg+=ddx;
			xbegr-=ddy;ybegr+=ddx;
			int abs_ddx,abs_ddy;
			char fl=0;
			abs_ddx = (ddx>=0) ? ddx : -ddx;
			abs_ddy = (ddy>=0) ? ddy : -ddy;
			if(abs_ddx >= abs_ddy){ //bmp-=sx; 
				if(ddx>=0) { if(ybegr>((ypp<<16)+ybeg)){ ypp++; fl=1; if(ddy>0)ybeg-=ddy; else ybeg+=ddy; if(ybegr>((ypp<<16)+ybeg)){bmp-=sx;sy++;xbegr+=ddy;ybegr-=ddx;}}}//{ ybeg=ybeg+((__int64)1<<32);}
				else { if(ybegr<((ypp<<16)+ybeg)) { ypp--; fl=1; if(ddy>0)ybeg+=ddy; else ybeg-=ddy; if(ybegr<((ypp<<16)+ybeg)){bmp-=sx;sy++;xbegr+=ddy;ybegr-=ddx;}}}//{ ybeg=ybeg-((__int64)1<<32);}
				if(ddy>0){
					if( (xbegr < xbeg) && (fl) ) {
						if(ddx>0) xbeg-=ddx;
						else xbeg+=ddx;
					}
				}
				else{//ddy<0
					if( (xbegr > xbeg) && (fl) ) {
						if(ddx>0) xbeg+=ddx;
						else xbeg-=ddx;
					}
				}
			}
			else{
				if(ddy>=0) { if(xbegr<((xpp<<16)+xbeg)){ xpp--; fl=1; if(ddx>0)xbeg+=ddx; else xbeg-=ddx; if(xbegr<((xpp<<16)+xbeg)){bmp-=sx;sy++;ybegr-=ddx;xbegr+=ddy;}}}//{ ybeg=ybeg+((__int64)1<<32);}
				else { if(xbegr>((xpp<<16)+xbeg)) { xpp++; fl=1; if(ddx>0)xbeg-=ddx; else xbeg+=ddx; if(xbegr>((xpp<<16)+xbeg)){bmp-=sx;sy++;ybegr-=ddx;xbegr+=ddy;}}}//{ ybeg=ybeg-((__int64)1<<32);}
				if(ddx>0){
					if( (ybegr > ybeg) && (fl) ) {
						if(ddy>0) ybeg+=ddy;
						else ybeg-=ddy;
					}
				}
				else{//ddy<0
					if( (ybegr < ybeg) && (fl) ) {
						if(ddy>0) ybeg-=ddy;
						else ybeg+=ddy;
					}
				}
			}

			
		}
	};
}
*/
/*
void PutTrackPoints(int num_track)
{
	char* script_name = "track_s";//.sst";
	XBuffer buft;
	char tmpstr[10];
	buft < script_name < itoa(num_track%10,tmpstr,10) < ".sst";

	sur_scr.load_scr(GetTargetName(buft));
	sur_scr.load_data();
	int i,t_x,t_y,t_a,ter_type;
	char* t_name;
	for(i=0; i<sur_scr.numbers_cell; i++){
		sur_scr.row[0].get_el(i,t_x);
		sur_scr.row[1].get_el(i,t_y);
		sur_scr.row[2].get_el(i,t_a);
		sur_scr.row[3].get_el(i,t_name);
		sur_scr.row[4].get_el(i,ter_type);
		placeBMP.place(GetTargetName(t_name),t_x,t_y,0,4,0,0,0,0,0,t_a,ter_type);
	}
	sur_scr.delete_data();
}*/


void DrawBWCyrcle(int r, unsigned char * bitmap, int xcenter, int ycenter, int sizex, int sizey)
{
	int k;
	for(k=0; k< sizey*(sizex>>3); k++) bitmap[k]=0;

	if(r >MAX_RADIUS_CIRCLEARR) r=MAX_RADIUS_CIRCLEARR; 
	int j;
	int* xx,*yy;
	int max = maxRad[r];
	xx = xRad[r];
	yy = yRad[r];
	for(j = 0;j < max;j++) {
		int x=xcenter + xx[j];
		if(x<0)continue; if(x>=sizex) continue;//x=sizex-1;
		int y=ycenter + yy[j];
		if(y<0)continue; if(y>=sizey) continue;
		unsigned char mask=x&0x07; mask=0x80>>mask;
		bitmap[(x>>3) + (y*(sizex>>3))] |=mask;
	}
	//Ставим точку в центре
	int x=xcenter; int y=ycenter;
	unsigned char mask=x&0x07; mask=0x80>>mask;
	bitmap[(x>>3) + (y*(sizex>>3))] |=mask;
}

void DrawBWQuadrate(int r, unsigned char * bitmap, int xcenter, int ycenter, int sizex, int sizey)
{
	int k;
	for(k=0; k< sizey*(sizex>>3); k++) bitmap[k]=0;

	if(r >MAX_RADIUS_CIRCLEARR) r=MAX_RADIUS_CIRCLEARR; 
	int j;
	int* xx,*yy;
	int max = maxRad[r];
	xx = xRad[r];
	yy = yRad[r];
	for(j = -r;j < r;j++) {
		int x=xcenter + j; if(x<0)continue; if(x>=sizex) continue;//x=sizex-1;
		int y=ycenter + -r; if(y<0)continue; if(y>=sizey) continue;
		unsigned char mask=x&0x07; mask=0x80>>mask;
		bitmap[(x>>3) + (y*(sizex>>3))] |=mask;
		///////
		x=xcenter + j; if(x<0)continue; if(x>=sizex) continue;//x=sizex-1;
		y=ycenter + +r; if(y<0)continue; if(y>=sizey) continue;
		mask=x&0x07; mask=0x80>>mask;
		bitmap[(x>>3) + (y*(sizex>>3))] |=mask;
		/////////
		x=xcenter + +r; if(x<0)continue; if(x>=sizex) continue;//x=sizex-1;
		y=ycenter + j; if(y<0)continue; if(y>=sizey) continue;
		mask=x&0x07; mask=0x80>>mask;
		bitmap[(x>>3) + (y*(sizex>>3))] |=mask;
		//////////
		x=xcenter + -r; if(x<0)continue; if(x>=sizex) continue;//x=sizex-1;
		y=ycenter + j; if(y<0)continue; if(y>=sizey) continue;
		mask=x&0x07; mask=0x80>>mask;
		bitmap[(x>>3) + (y*(sizex>>3))] |=mask;
	}
	//Ставим точку в центре
	int x=xcenter; int y=ycenter;
	unsigned char mask=x&0x07; mask=0x80>>mask;
	bitmap[(x>>3) + (y*(sizex>>3))] |=mask;
}



#ifdef _SURMAP_
#include "..\Surmap5\utl\3d_shape.h"
int S3Dinverse=0;
void S3Danalyze(int S3Dmode, int S3Dlevel, int S3DnoiseLevel, int S3DnoiseAmp, int S3Dside)
{
	//unsigned char** lt = vMap -> lineT;
	int x = shape_x;
	int y = shape_y;
	int xysize = shape_size,v,vv,xx,yy,empty;
	unsigned short* p;
	//unsigned char* pv0;
	//unsigned char* pv;
	unsigned short* surface;
	if(!S3Dside) surface = upper_buffer, empty = 0;
	else surface = lower_buffer, empty = MAX_VX_HEIGHT;

	vMap.UndoDispatcher_PutPreChangedArea(x,y,x + xysize,y + xysize);

	register int i,j;
	for(j = 0;j < xysize;j++){
		p = surface + (j << shape_shift);
		yy = vMap.YCYCL(y + j);
		for(i = 0;i < xysize;i++,p++)
			if(*p != empty){
				v = S3Dinverse ? -*p : *p;
				v += S3Dlevel;
				if(S3DnoiseLevel && S3DnoiseAmp)
					if((int)XRnd(100) < S3DnoiseLevel) v += S3DnoiseAmp - XRnd((S3DnoiseAmp << 1) + 1);
				//pv = pv0 + (xx = vMap.XCYCL(x + i));
				xx = vMap.XCYCL(x + i);
				switch(S3Dmode){
					case 0:
						vv = v;
						break;
					case 1:
						vv = MAX(v,vMap.GetAlt(xx,yy));//*pv);
						if(vv != v) continue;
						break;
					case 2:
						vv = MIN(v,vMap.GetAlt(xx,yy));//*pv);
						if(vv != v) continue;
						break;
					case 3:
						vv = (v + vMap.GetAlt(xx,yy))>>1;//*pv >> 1;
						break;
					case 4:
						vv = v + vMap.GetAlt(xx,yy);//*pv;
						break;
					}
				vMap.voxSet(xx,yy,vv - vMap.GetAlt(xx,yy));//*pv);
				}
		}

	//release_shape();
	vMap.regRender(x,y,x + xysize,y + xysize);
}

/////////////////////////////////////////////////////////////////////////////////////

void sVBitMap::put(int VBMmode, int VBMlevel, int VBMnoiseLevel, int VBMnoiseAmp, bool VBMinverse)
{
	int x,y;
	int sizeMX=round((float)sx/kX);
	int sizeMY=round((float)sy/kY);
	int sizeMXY;
	if(sizeMX>sizeMY)sizeMXY=sizeMX;
	else sizeMXY=sizeMY;
	int addx=mX-(sizeMXY>>1);
	int addy=mY-(sizeMXY>>1);

	vMap.UndoDispatcher_PutPreChangedArea(addx, addy, addx + sizeMXY, addy + sizeMXY);

	for(y=0; y<sizeMXY; y++){
		int yy=vMap.YCYCL(addy+y);
		for(x=0; x<sizeMXY; x++){
			int xx=vMap.XCYCL(addx+x);
			int c=getPreciseColor(xx,yy);//getColor(xx,yy);//
			//if(c!=-1) *b=c;
			if(c>0) {
				int vv,v=round((float)c/kZ);
				v=VBMinverse ? -v : v;
				v+=VBMlevel;
				if(VBMnoiseLevel && VBMnoiseAmp){
					if((int)XRnd(100) < VBMnoiseLevel) v += VBMnoiseAmp - XRnd((VBMnoiseAmp << 1) + 1);
				}
				switch(VBMmode){
				case 0:
					vv = v;
					break;
				case 1:
					vv = MAX(v,vMap.GetAlt(xx,yy));//*pv);
					if(vv != v) continue;
					break;
				case 2:
					vv = MIN(v,vMap.GetAlt(xx,yy));//*pv);
					if(vv != v) continue;
					break;
				case 3:
					vv = (v + vMap.GetAlt(xx,yy))>>1;//*pv >> 1;
					break;
				case 4:
					vv = v + vMap.GetAlt(xx,yy);//*pv;
					break;
				}
				vMap.voxSet(xx,yy,vv - vMap.GetAlt(xx,yy));//*pv);
			}
		}
	}
	vMap.regRender(addx, addy, addx + sizeMXY, addy + sizeMXY);
}

#endif

sTerrainMetod TerrainMetod;
void sTerrainMetod::put(int xx, int yy, int v)
{
	int vv;
	//v=inverse ? -v : v;
	v+=level;
	if(noiseLevel && noiseAmp){
		if((int)XRnd(100) < noiseLevel) v += noiseAmp - XRnd((noiseAmp << 1) + 1);
	}
	switch(mode){
	case 0:
		vv = v;
		break;
	case 1:
		vv = MAX(v,vMap.GetAlt(xx,yy));//*pv);
		if(vv != v) return;
		break;
	case 2:
		vv = MIN(v,vMap.GetAlt(xx,yy));//*pv);
		if(vv != v) return;
		break;
	case 3:
		vv = (v + vMap.GetAlt(xx,yy))>>1;//*pv >> 1;
		break;
	case 4:
		vv = v + vMap.GetAlt(xx,yy);//*pv;
		break;
	}
	vMap.voxSet(xx,yy,vv - vMap.GetAlt(xx,yy));//*pv);
}

