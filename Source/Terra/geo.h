#ifndef __GEO_H__
#define __GEO_H__

#include "xmath.h"

const char Path2TTerraResource[]="RESOURCE\\Tools\\";

class CGeoInfluence {
	//unsigned char* inVxG;
	//unsigned char* inAtr;
	unsigned short* inVx;
	unsigned char* inSur;
	int* tmpltGeo;
	unsigned char* tmpltSur;
	int x,y,sx,sy;
	int h_begin;
	int time;
	int kmNewVx;
	void prepTmplt(void);
public:
	CGeoInfluence(int _x, int _y, int _sx, int _sy);
	~CGeoInfluence();
	int quant(int deltaTime=1);
};

/*
	static CGeoInfluence * gI;
	static char flagg=0;
	if(flagg==0){
		flagg=1;
		gI=new CGeoInfluence(mapCrd.x, mapCrd.y, 128, 128);
	}
	else{
		if(gI->quant()==0){
			delete gI;
			flagg=0;
		}
	}
*/

///////////////////////////////////////////////////////////////////////////////////////////////
//                                         ЧЕРВЯК
///////////////////////////////////////////////////////////////////////////////////////////////
class CWormOut {
	short x_,y_;
	short sx_,sy_;
	unsigned short* inVx;
	int *tmpltVx;
	int h_begin;
	int time;
	int kmNewVx;
public:
	CWormOut(int x, int y);
	bool quant();
};

extern void worms(int x, int y);
static const int WSXSY=64;
static const int WSXSY05=WSXSY>>1;
typedef int tFormWorm[WSXSY][WSXSY];
struct CWormFormLib {
	sVBitMap VBitMap1;
	~CWormFormLib(){
			VBitMap1.release();
	};
	CWormFormLib(){
		XBuffer tb;
		//tb < dirBitMap   < "\\" < fname;
		tb < "RESOURCE\\Tools\\wormform.tga";
		TGAHEAD tgahead;
		if(tgahead.loadHeader(tb)){
			if((tgahead.PixelDepth!=8) || (tgahead.ImageType!=3)) {
				ErrH.Abort("CWormFormLib TGA - not correct!");
				//AfxMessageBox("Не поддерживаемый тип TGA (необходим монохромный не компрессованный)");
				return;
			}
			VBitMap1.create(tgahead.Width, tgahead.Height);
			tgahead.load2buf(VBitMap1.data);
		}
		else{
			VBitMap1.create(WSXSY, WSXSY);
			VBitMap1.clear();

		}
	};
	void put2WF(float alpha, tFormWorm* FormWormsNew){
		VBitMap1.set_alphaR(alpha, WSXSY05, WSXSY05);
		int i,j;
		for(i=0; i<WSXSY; i++){
			float y=(float)(i-WSXSY05)/(float)WSXSY05;
			for(j=0; j<WSXSY; j++){
				//float x=(float)(j-WSXSY05)/(float)WSXSY05; 
				//float f=exp(-(fabsf(x*x)+fabsf(y*y))/(0.4*0.4));
				//(*FormWormsNew)[i][j]=round(f*(float)(1<<16));//(1<<16)+XRnd(1<<14);
				int hh=VBitMap1.getPreciseColor(j, i);
				(*FormWormsNew)[i][j]=hh<<8;
			}
/*			for(j=WSXSY>>1; j<WSXSY; j++){
				int t=(WSXSY-j)*FormWorms[WSXSY-1][j]/(WSXSY>>1);
				FormWorms[i][j]=-(t>>2);
			}
*/
		}
	};
	void put2WF2(tFormWorm* FormWormsNew){
		int i,j;
		for(i=0; i<WSXSY; i++){
			float y=(float)(i-WSXSY05)/(float)WSXSY05;
			for(j=0; j<WSXSY; j++){
				float x=(float)(j-WSXSY05)/(float)WSXSY05; 
				float f=expf(-(fabsf(x*x)+fabsf(y*y))/(0.4f*0.4f));
				(*FormWormsNew)[i][j]=round(f*(float)(1<<16));//(1<<16)+XRnd(1<<14);
			}
		}
	};
};

class CGeoWorm {
	static CWormFormLib wormFormLib;
	tFormWorm FormWorms1;
	tFormWorm FormWorms2;
	tFormWorm* FormWormsNew;
	tFormWorm* FormWormsOld;
	Vect3f cPosition;
	Vect3f tPosition;
	float cDirection;
	float cSpeed;
	int xOld, yOld;//данные для step-а
	int counter;
public:
	CGeoWorm(int xBeg, int yBeg, int xTrgt=0, int yTrgt=0);
	int quant();
	void step(int x, int y, float angle=0);
	static Vect2s getSize(void){
		return Vect2s(WSXSY,WSXSY);
	}


};

///////////////////////////////////////////////////////////////////////////////////////////////
//                                     ТРЕЩИНЫ(РАЗЛОМЫ)
///////////////////////////////////////////////////////////////////////////////////////////////

enum eReturnQuantResult{
	END_QUANT=0,
	HEAD_IN_FINAL_POINT=1,
	CONTINUE_QUANT=2
};

const int WidthOfBreak=20;
const int DENSITY_NOISE=6; //один излом на 4-е точки
const int LENGHT_TAIL=50; //Длинна хвоста(в сегментах)
const int DELTA_WIDTH=(1<<11)+(1<<10);//Прибавляемая ширина к трещине (сдвигается на 16)
const int DELTA_H_BREAK_GEOBREAK=50;
const int DELTA_H_STEP_BREAK_GEOBREAK=5;

struct elementGeoBreak {
	int xbeg, ybeg, xend, yend;
	//int sx, sy;
	int dx_step, dy_step;
	float alpha;

	static int unengagedID; //инициализируется в cpp
	int ownerID;
	int ID;
	//int bLong;
	//int time;
	//int lifetime;
	int numSections;
	int headSection;
	int * xp;
	int * yp;
	unsigned char beginHeight;
	unsigned char oldHeight;
	elementGeoBreak(int _xbeg, int _ybeg, float _alpha, float _lenght, int _ownerID=0){//
		//_ownerID=0 означает что нет родителей
		ID=unengagedID++;
		ownerID=_ownerID;
		if(_lenght<DENSITY_NOISE)_lenght=DENSITY_NOISE; //Минимальная длинна

		alpha=_alpha;
		xend=xbeg=_xbeg; yend=ybeg=_ybeg;
		//sx=_sx; sy=_sy;
		//time=0;
		//bLong=sqrt(sx*sx+sy*sy);
		numSections=round(_lenght/(float)DENSITY_NOISE);//bLong/DENSITY_NOISE;
		dx_step=round(_lenght*cosf(alpha))*(1<<16)/numSections;//sx*(1<<16)/numSections;
		dy_step=round(_lenght*sinf(alpha))*(1<<16)/numSections;//sy*(1<<16)/numSections;
		xp=new int [numSections+1];//1 это начальная точка
		yp=new int [numSections+1];
		xp[0]=xbeg;
		yp[0]=ybeg;
		headSection=0;
		beginHeight=oldHeight=vMap.GVBuf[vMap.offsetGBuf(vMap.XCYCLG(xbeg>>kmGrid), vMap.YCYCLG(ybeg>>kmGrid))];
	};
	~elementGeoBreak(){
		delete [] yp;
		delete [] xp;
	};
	inline void geoLine(int xbeg, int ybeg, int sx, int sy, int width=1, int fl_new_width=0){

		int x_cur=xbeg<<16;
		int y_cur=ybeg<<16;

		if(abs(sx)>=abs(sy)){
			int stepdy=sy*(1<<16)/abs(sx);
			int stepdx;
			if(sx>0)stepdx=1; else stepdx=-1;
			for(int i=0; i!=sx; i+=stepdx, x_cur+=stepdx<<16, y_cur+=stepdy){
				vMap.voxSet(vMap.XCYCL(x_cur>>16), vMap.YCYCL(y_cur>>16), -6);

				if(fl_new_width){
					for(int k=10+XRnd(30);k >0; k--){
						vMap.SetTer(vMap.XCYCL(x_cur>>16), vMap.YCYCL((y_cur>>16)-(k+1)), vMap.GetTer(vMap.XCYCL(x_cur>>16), vMap.YCYCL((y_cur>>16)-k)) );
						vMap.VxGBuf[vMap.offsetBuf(vMap.XCYCL(x_cur>>16), vMap.YCYCL((y_cur>>16)-(k+1)))] = vMap.VxGBuf[vMap.offsetBuf(vMap.XCYCL(x_cur>>16), vMap.YCYCL((y_cur>>16)-k))] ;
						vMap.AtrBuf[vMap.offsetBuf(vMap.XCYCL(x_cur>>16), vMap.YCYCL((y_cur>>16)-(k+1)))] = vMap.AtrBuf[vMap.offsetBuf(vMap.XCYCL(x_cur>>16), vMap.YCYCL((y_cur>>16)-k))] ;

						vMap.SetTer(vMap.XCYCL(x_cur>>16), vMap.YCYCL((y_cur>>16)+(k+1)), vMap.GetTer(vMap.XCYCL(x_cur>>16), vMap.YCYCL((y_cur>>16)+k)) );
						vMap.VxGBuf[vMap.offsetBuf(vMap.XCYCL(x_cur>>16), vMap.YCYCL((y_cur>>16)+(k+1)))] = vMap.VxGBuf[vMap.offsetBuf(vMap.XCYCL(x_cur>>16), vMap.YCYCL((y_cur>>16)+k))] ;
						vMap.AtrBuf[vMap.offsetBuf(vMap.XCYCL(x_cur>>16), vMap.YCYCL((y_cur>>16)+(k+1)))] = vMap.AtrBuf[vMap.offsetBuf(vMap.XCYCL(x_cur>>16), vMap.YCYCL((y_cur>>16)+k))] ;
					}
				}

				for(int k=1; k<width; k++){
					///vMap.voxSet(vMap.XCYCL(x_cur>>16), vMap.YCYCL((y_cur>>16)-k), -6);
					///vMap.voxSet(vMap.XCYCL(x_cur>>16), vMap.YCYCL((y_cur>>16)+k), -6);
					vMap.SDig(vMap.XCYCL(x_cur>>16), vMap.YCYCL((y_cur>>16)-k), 6);
					vMap.SDig(vMap.XCYCL(x_cur>>16), vMap.YCYCL((y_cur>>16)+k), 6);
				}
			}
		}
		else { // sy_real> sx_real
			if(abs(sy)>abs(sx)){
				int stepdx=sx*(1<<16)/abs(sy);
				int stepdy;
				if(sy>0)stepdy=1; else stepdy=-1;
				for(int i=0; i!=sy; i+=stepdy, y_cur+=stepdy<<16, x_cur+=stepdx){
					///vMap.voxSet(vMap.XCYCL(x_cur>>16), vMap.YCYCL(y_cur>>16), -6);
					vMap.SDig(vMap.XCYCL(x_cur>>16), vMap.YCYCL(y_cur>>16), 6);
					for(int k=1; k<width; k++){
						///vMap.voxSet(vMap.XCYCL((x_cur>>16)-k), vMap.YCYCL(y_cur>>16), -6);
						///vMap.voxSet(vMap.XCYCL((x_cur>>16)+k), vMap.YCYCL(y_cur>>16), -6);
						vMap.SDig(vMap.XCYCL((x_cur>>16)-k), vMap.YCYCL(y_cur>>16), 6);
						vMap.SDig(vMap.XCYCL((x_cur>>16)+k), vMap.YCYCL(y_cur>>16), 6);
					}
/*					vMap.voxSet(vMap.XCYCL((x_cur>>16)+1), vMap.YCYCL(y_cur>>16), -200);
					vMap.voxSet(vMap.XCYCL((x_cur>>16)+1), vMap.YCYCL(y_cur>>16), -200);
*/				}
			}
		}
		int xL, yT, xR, yD;
		if(sx>0) { xL=vMap.XCYCL(xbeg-width); xR=vMap.XCYCL(xbeg+sx+width); }
		else{ xL=vMap.XCYCL(xbeg+sx-width); xR=vMap.XCYCL(xbeg+width); }
		if(sy>0) { yT=vMap.YCYCL(ybeg-width); yD=vMap.YCYCL(ybeg+sy+width); }
		else { yT=vMap.YCYCL(ybeg+sy-width); yD=vMap.YCYCL(ybeg+width); }
		vMap.recalcArea2Grid(xL, yT, xR, yD );
		//extern int STATUS_DEBUG_RND;
		//STATUS_DEBUG_RND=1;
		vMap.regRender(xL, yT, xR, yD);
		//STATUS_DEBUG_RND=0;
	};

	eReturnQuantResult quant(void){
		headSection+=1;
		if(headSection <= numSections){//Идет рост трещины
			int x_recomend=xbeg+(dx_step*headSection>>16);
			int y_recomend=ybeg+(dy_step*headSection>>16);

			int offGrid=vMap.offsetGBuf(vMap.XCYCLG(x_recomend>>kmGrid), vMap.YCYCLG(y_recomend>>kmGrid));
			int curHeight=vMap.GVBuf[offGrid];
			if( abs(curHeight-beginHeight)>DELTA_H_BREAK_GEOBREAK || abs(curHeight-oldHeight)>DELTA_H_STEP_BREAK_GEOBREAK || 
				(vMap.GABuf[offGrid] & GRIDAT_MASK_HARDNESS) == 0 ) numSections=headSection;//return END_QUANT;
			oldHeight=curHeight;

			int sx_real=x_recomend - xp[headSection-1];
			int sy_real=y_recomend - yp[headSection-1];
			int x_cur=xp[headSection-1]<<16;
			int y_cur=yp[headSection-1]<<16;
			//Noising
			//double alpha=atan((double)sy_real/(double)sx_real);
			float ss=sqrtf(float(sy_real*sy_real+sx_real*sx_real));
			float cosAlpha=(float)sx_real/ss;
			float sinAlpha=(float)sy_real/ss;
			int nx=(DENSITY_NOISE>>1) + XRnd(DENSITY_NOISE>>1); //DENSITY_NOISE;//
			int ny=XRnd(8)-4;
			sx_real=round((float)nx*cosAlpha+ (float)ny*sinAlpha);
			sy_real=round(-(-(float)nx*sinAlpha+ (float)ny*cosAlpha));
			//sx_real=sx_real+ XRnd(DENSITY_NOISE<<1)- (DENSITY_NOISE<<1);// if(sx_real<0) sx_real=0;
			//sy_real=sy_real+ XRnd(DENSITY_NOISE<<1)- (DENSITY_NOISE<<1);// if(sy_real<0) sy_real=0;
			//DrawLine
			geoLine(xp[headSection-1], yp[headSection-1], sx_real, sy_real);
			vMap.pointDamagingBuildings(xp[headSection-1], yp[headSection-1]);
			vMap.pointDamagingBuildings(xp[headSection-1]+sx_real, yp[headSection-1]+sy_real);
			x_cur+=sx_real<<16;
			y_cur+=sy_real<<16;

			xend=xp[headSection]=x_cur>>16;
			yend=yp[headSection]=y_cur>>16;
			
		}
		// расширение трещины
		int width=1<<16;
		//int fl=0;
		int endSection=headSection-LENGHT_TAIL;
		if(endSection<0)endSection=0; 
		int begSection=headSection-1;
		int d;
		for(d=begSection; d>endSection; d--){
			if(d<=numSections){
				geoLine(xp[d-1], yp[d-1], xp[d]-xp[d-1], yp[d]-yp[d-1], width>>16, 0);//fl);
			}
			//fl=0;
			//int oldwidth=width;
			width+=(1<<11)+(1<<10);//(1<<12);
			//if( (width>>16) > (oldwidth>>16)) fl=1;
		}
		if(headSection == numSections) return HEAD_IN_FINAL_POINT;
		if( d >= numSections) return END_QUANT;
		return CONTINUE_QUANT;
	};

};


const double pi = 3.1415926535;
const int MAX_BEGIN_BREAKS=9;
const int MAX_LENGHT_ELEMENTGEOBREAK=100;
const int MAX_BRANCHINGS_BREAKS=5;
const float CORNER_DISPERSION_BREAKS=(float)(pi+pi/2);

struct geoBreak1{ //точечный разлом
	list<elementGeoBreak*> elGB;
	geoBreak1(int x, int y, int rad=MAX_LENGHT_ELEMENTGEOBREAK, int beginNumBreaks=0); //0-случайное кол-во
	list<elementGeoBreak*>::iterator delEementGeoBreak(list<elementGeoBreak*>::iterator pp);
	int quant(void);
};


struct singleGeoBreak {
	int xbeg, ybeg, xend, yend;
	int sx, sy;

	int dx_step, dy_step;
	//float alpha;

	int lenght;
	//int time;
	//int lifetime;
	int numSections;
	int headSection;
	int * xp;
	int * yp;
	unsigned char beginHeight;
	unsigned char oldHeight;
	singleGeoBreak(int _xbeg, int _ybeg, int _sx, int _sy){//
		sx=_sx; sy=_sy;

		lenght=round(sqrtf((float)(sx*sx+sy*sy)));
		//if(lenght<DENSITY_NOISE)lenght=DENSITY_NOISE; //Минимальная длинна

		//alpha=_alpha;
		xend=xbeg=_xbeg; yend=ybeg=_ybeg;

		numSections=round(lenght/(float)DENSITY_NOISE);
		dx_step=_sx*(1<<16)/numSections;//sx*(1<<16)/numSections;
		dy_step=_sy*(1<<16)/numSections;//sy*(1<<16)/numSections;
		xp=new int [numSections+1];//1 это начальная точка
		yp=new int [numSections+1];
		xp[0]=xbeg;
		yp[0]=ybeg;
		headSection=0;
	}
	~singleGeoBreak(){
		delete [] yp;
		delete [] xp;
	}
	inline void geoLine(int xbeg, int ybeg, int sx, int sy, int width=1, int fl_new_width=0){

		int x_cur=xbeg<<16;
		int y_cur=ybeg<<16;

		if(abs(sx)>=abs(sy)){
			if(sx!=0){
				int stepdy=sy*(1<<16)/abs(sx);
				int stepdx;
				if(sx>0)stepdx=1; else stepdx=-1;
				for(int i=0; i!=sx; i+=stepdx, x_cur+=stepdx<<16, y_cur+=stepdy){
					vMap.voxSet(vMap.XCYCL(x_cur>>16), vMap.YCYCL(y_cur>>16), -6);

					if(fl_new_width){
						for(int k=10+XRnd(30);k >0; k--){
							vMap.SetTer(vMap.XCYCL(x_cur>>16), vMap.YCYCL((y_cur>>16)-(k+1)), vMap.GetTer(vMap.XCYCL(x_cur>>16), vMap.YCYCL((y_cur>>16)-k)) );
							vMap.VxGBuf[vMap.offsetBuf(vMap.XCYCL(x_cur>>16), vMap.YCYCL((y_cur>>16)-(k+1)))] = vMap.VxGBuf[vMap.offsetBuf(vMap.XCYCL(x_cur>>16), vMap.YCYCL((y_cur>>16)-k))] ;
							vMap.AtrBuf[vMap.offsetBuf(vMap.XCYCL(x_cur>>16), vMap.YCYCL((y_cur>>16)-(k+1)))] = vMap.AtrBuf[vMap.offsetBuf(vMap.XCYCL(x_cur>>16), vMap.YCYCL((y_cur>>16)-k))] ;

							vMap.SetTer(vMap.XCYCL(x_cur>>16), vMap.YCYCL((y_cur>>16)+(k+1)), vMap.GetTer(vMap.XCYCL(x_cur>>16), vMap.YCYCL((y_cur>>16)+k)) );
							vMap.VxGBuf[vMap.offsetBuf(vMap.XCYCL(x_cur>>16), vMap.YCYCL((y_cur>>16)+(k+1)))] = vMap.VxGBuf[vMap.offsetBuf(vMap.XCYCL(x_cur>>16), vMap.YCYCL((y_cur>>16)+k))] ;
							vMap.AtrBuf[vMap.offsetBuf(vMap.XCYCL(x_cur>>16), vMap.YCYCL((y_cur>>16)+(k+1)))] = vMap.AtrBuf[vMap.offsetBuf(vMap.XCYCL(x_cur>>16), vMap.YCYCL((y_cur>>16)+k))] ;
						}
					}

					for(int k=1; k<width; k++){
						///vMap.voxSet(vMap.XCYCL(x_cur>>16), vMap.YCYCL((y_cur>>16)-k), -6);
						///vMap.voxSet(vMap.XCYCL(x_cur>>16), vMap.YCYCL((y_cur>>16)+k), -6);
						vMap.SDig(vMap.XCYCL(x_cur>>16), vMap.YCYCL((y_cur>>16)-k), 6);
						vMap.SDig(vMap.XCYCL(x_cur>>16), vMap.YCYCL((y_cur>>16)+k), 6);
					}
				}
			}
			else vMap.SDig(vMap.XCYCL(x_cur>>16), vMap.YCYCL(y_cur>>16), 6);
		}
		else { // sy_real> sx_real
			if(abs(sy)>abs(sx)){
				int stepdx=sx*(1<<16)/abs(sy);
				int stepdy;
				if(sy>0)stepdy=1; else stepdy=-1;
				for(int i=0; i!=sy; i+=stepdy, y_cur+=stepdy<<16, x_cur+=stepdx){
					///vMap.voxSet(vMap.XCYCL(x_cur>>16), vMap.YCYCL(y_cur>>16), -6);
					vMap.SDig(vMap.XCYCL(x_cur>>16), vMap.YCYCL(y_cur>>16), 6);
					for(int k=1; k<width; k++){
						///vMap.voxSet(vMap.XCYCL((x_cur>>16)-k), vMap.YCYCL(y_cur>>16), -6);
						///vMap.voxSet(vMap.XCYCL((x_cur>>16)+k), vMap.YCYCL(y_cur>>16), -6);
						vMap.SDig(vMap.XCYCL((x_cur>>16)-k), vMap.YCYCL(y_cur>>16), 6);
						vMap.SDig(vMap.XCYCL((x_cur>>16)+k), vMap.YCYCL(y_cur>>16), 6);
					}
/*					vMap.voxSet(vMap.XCYCL((x_cur>>16)+1), vMap.YCYCL(y_cur>>16), -200);
					vMap.voxSet(vMap.XCYCL((x_cur>>16)+1), vMap.YCYCL(y_cur>>16), -200);
*/				}
			}
		}
		int xL, yT, xR, yD;
		if(sx>0) { xL=vMap.XCYCL(xbeg-width); xR=vMap.XCYCL(xbeg+sx+width); }
		else{ xL=vMap.XCYCL(xbeg+sx-width); xR=vMap.XCYCL(xbeg+width); }
		if(sy>0) { yT=vMap.YCYCL(ybeg-width); yD=vMap.YCYCL(ybeg+sy+width); }
		else { yT=vMap.YCYCL(ybeg+sy-width); yD=vMap.YCYCL(ybeg+width); }
		vMap.recalcArea2Grid(xL, yT, xR, yD );
		//extern int STATUS_DEBUG_RND;
		//STATUS_DEBUG_RND=1;
		vMap.regRender(xL, yT, xR, yD);
		//STATUS_DEBUG_RND=0;
	}


	void geoLine2(int xbeg, int ybeg, int sx, int sy, int deltaH);
	void breakGeoLine3(int * xp, int * yp, int maxPnt, int deltaH, int dx, int dy, int cntLine);

	eReturnQuantResult quant(void);

};






///////////////////////////////////////////////////////////////////////////////////////////////
//                                     ОПОЛЗЕНЬ
///////////////////////////////////////////////////////////////////////////////////////////////
struct srBmp{
	int stpX, stpY;
	int bsx, bsy;
	int begShiftX, begShiftY;
	float alpha;
	srBmp(float _alpha, int _sx, int _sy){
		alpha=_alpha;
		float ssY=tanf(_alpha)*1.0f;
		if(fabs(ssY)<=1.0){//step dx=1 or -1
			if(cos(_alpha)>=0) { stpX=1<<16; stpY=round(ssY*(1<<16)); }
			else { stpX=-1<<16; stpY=round(ssY*(-1<<16)); }
			begShiftX=-stpX*round((_sy/2)*cos(_alpha)*sin(_alpha))+ (_sx/2)*stpX ;
			begShiftY=(stpX*(_sy/2))-stpY*round((_sy/2)*cos(_alpha)*sin(_alpha))+ (_sx/2)*stpY ;
			begShiftX=-begShiftX>>16; begShiftY=-begShiftY>>16;
			begShiftX<<=16; begShiftY<<=16;
		}
		else { //step dy=1 or -1
			if(sin(_alpha)>=0){ stpY=1<<16; stpX=round((1<<16)*1.0/ssY); }
			else { stpY=-1<<16; stpX=round((-1<<16)*1.0/ssY); }
			begShiftX=(-stpY*(_sy/2))+stpX*round((_sy/2)*sin(_alpha)*cos(_alpha)) + (_sx/2)*stpX;
			begShiftY=stpY*round((_sy/2)*sin(_alpha)*cos(_alpha)) + (_sx/2)*stpY;
			begShiftX=-begShiftX>>16; begShiftY=-begShiftY>>16;
			begShiftX<<=16; begShiftY<<=16;
		}
	};
	inline void setStr(int y){
		if(stpX==(1<<16)|| stpX==(-1<<16)){
			bsx=-stpX*round(y*cos(alpha)*sin(alpha));
			bsy=(stpX*y)-stpY*round(y*cos(alpha)*sin(alpha));
		}
		else {
			bsx=(-stpY*y)+stpX*round(y*sin(alpha)*cos(alpha));
			bsy=stpY*round(y*sin(alpha)*cos(alpha));
		}
	};
/*	inline int getX(int x){
			return begShiftX + ((bsx+x*stpX)>>16);
	}
	inline int getY(int x){
			return begShiftY + ((bsy+x*stpY)>>16);
	}*/
	inline int getX(int x){
			return begShiftX + (bsx+x*stpX);
	}
	inline int getY(int x){
			return begShiftY + (bsy+x*stpY);
	}

};

struct CLandslip {
	unsigned short* deltaVx;
	short * surVx;
	short * surVxB;
	int* tmpltGeo;
	int x,y,sx,sy;
	int xPrec, yPrec;//float 
	int hmin, hmax;
	int deltaHLS, hminLS;
	float direction;
	float speed;
	int cntQuant;
	srBmp rbmp;
	CLandslip(int _x, int _y, int _sx, int _sy, int _hmin, int _hmax, float _direction);
	~CLandslip(void);
	void prepTmplt(void);
	int quant(void);
	int geoQuant(void);
};

///////////////////////////////////////////////////////////////////////////////////////
//                              Пузырь и торпеда
///////////////////////////////////////////////////////////////////////////////////////
struct sToolzDate{ 
	sToolzDate(short _x, short _y, short _r) {
		x=_x; y=_y; r=_r;
	};
	short x, y, r;
};

struct sTBubble {
	static int numPreImage;
	static unsigned short* preImage;
	short* array;
	short* inVx;
	short* tmpVx;
	unsigned char* mask;
	unsigned char* maxVx;
	short* substare;
	int h_begin;
	int x, y, sx, sy;
	int vMin;
	int frame;
	int previsionKP;
	int currentKP;
	int currentKPFrame;
	short* pCurKP;
	short* pPrevKP;
	bool flag_sleep;
	bool flag_occurrenceGeo;
	~sTBubble(){
		delete [] array;
		delete [] tmpVx;
		delete [] inVx;
		delete [] mask;
		delete [] maxVx;
		delete [] substare;
	};
	static void free(){
		if(preImage!=0) { delete [] preImage; preImage=0; }
	};
	sTBubble(int _x, int _y, int _sx, int _sy, bool _flag_occurrenceGeo=0);
	int quant(void);
};

const int CTORPEDO_MAX_EL_ARR=200;
struct sTorpedo {
	int step;
	//float begAngle;
	Vect2f direction;
	int num_el_arr;
	int beginX, beginY;
	float curX, curY;
	sTBubble * bubArr[CTORPEDO_MAX_EL_ARR];

	list<sToolzDate> toolzDateLst;
	individualToolzer<T2TE_CHANGING_TERRAIN_HEIGHT> toolzerChangeTerHeight ;

	sTorpedo(int x, int y, Vect2f& _direction);
	sTorpedo(int x, int y);
	void init(int x, int y, Vect2f& _direction);
	int findFreeEl(void){
		if(num_el_arr >= CTORPEDO_MAX_EL_ARR) return -1;
		for(int i=0; i<CTORPEDO_MAX_EL_ARR; i++) if(bubArr[i]==0) return i;
		return -1;
	};
	bool insert2Arr(int _x, int _y, int _sx, int _sy, bool _flag_occurrenceGeo=0){

		int bxg, byg, exg, eyg;
		bxg=_x>>kmGrid;
		byg=_y>>kmGrid;
		exg=(_x+_sx)>>kmGrid;
		eyg=(_y+_sy)>>kmGrid;
		///int i,j;
		///for(i=byg; i<=eyg; i++){
		///	for(j=bxg; j<=exg; j++){
		///		if((vMap.GABuf[vMap.offsetGBufC(j, i)]&GRIDAT_BUILDING) != 0) return 0;
		///	}
		///}

		int idx=findFreeEl();
		if(idx<0) return 0;
		else{
			bubArr[idx]=new sTBubble(_x, _y, _sx, _sy, _flag_occurrenceGeo);
			num_el_arr++;
			return 1;
		}
	}
	void deleteEl(int idx){
		delete bubArr[idx];
		bubArr[idx]=0;
		num_el_arr--;
	};
	bool quant(void);
};


///////////////////////////////////////////////////////////////////////////////////////////
//                                Разломы
///////////////////////////////////////////////////////////////////////////////////////////
struct point4UP{
	short x;
	short y;
	short z;
};

const short SUPOLIGON_NON_POLIGON=-MAX_VX_HEIGHT;

struct sUPoligon {
	point4UP pArr[3];
	int leftBorder, rightBorder;
	int upBorder, downBorder;
	int sx, sy;
	int xc, yc;

	int hBegin;
	int vMin;
	int vMax;

	short * arrayVx;
	bool * arrayA; 
	sUPoligon(point4UP* _pArr);
	~sUPoligon(void);
	float curAngleG;
	bool quant(){
		curAngleG+=3;
		quant(curAngleG);
		if(curAngleG>35) return 0;
		else return 1;
	}
	void quant(float angle_grad);
	void smoothPoligon(Vect2f a, Vect2f b, Vect2f c);

};

struct AETRecord {
	int x;
	int dx;
	char idxFrom;
	char idxTo;
	char dir;
};

const int SUPOLIGONN_MAX_BASE_POINT=3;
const int SUPOLIGONN_MAX_ADDING_POINT=20;
const int SUPOLIGONN_ALL_POINT_IN_POLIGON=SUPOLIGONN_MAX_ADDING_POINT+2;//(2-это 3 минус 1)
struct sUPoligonN {
	point4UP basePntArr[SUPOLIGONN_MAX_BASE_POINT];
	point4UP addPntArr[SUPOLIGONN_MAX_ADDING_POINT];
	int nElAddPntArr;
	Vect3f allPntArr[SUPOLIGONN_ALL_POINT_IN_POLIGON];
	int nElAllPntArr;
	int critPntArr[SUPOLIGONN_ALL_POINT_IN_POLIGON];
	int nElCritPntArr;
	AETRecord AET[SUPOLIGONN_ALL_POINT_IN_POLIGON]; //после отладки поменять на list
	int nElAET;

	int leftBorder, rightBorder;
	int upBorder, downBorder;
	int sx, sy;
	int xc, yc;

	int hBegin;
	int vMin;
	int vMax;

	Vect2f oldPoint1, oldPoint2;

	short * arrayVx;
	bool * arrayA; 
	sUPoligonN(point4UP* _basePntArr, int maxAddPnt, point4UP* _addPntArr);
	~sUPoligonN(void);
	float curAngleG;
	bool quant(void);
	bool quant(float angle_grad);
	void smoothPoligon(Vect2f a, Vect2f b, Vect2f c);
	void buldCPArr(void);
	void clearAET() {
		nElAET=0;
	};
	void addAETRecord(char idxFrom, char idxTo, char dir) {
		int dx=round((1<<16)*(allPntArr[idxTo].x-allPntArr[idxFrom].x)/(allPntArr[idxTo].y-allPntArr[idxFrom].y));
		int checkX=round(allPntArr[idxFrom].x*(1<<16)) + dx;
		int i;
		for(i=0; i<nElAET; i++) {
			if(   checkX < (AET[i].x+AET[i].dx) ) break;
		}
		int k;
		for(k=i; k<nElAET; k++) AET[k+1]=AET[k];
		nElAET++;
		AET[i].x=round(allPntArr[idxFrom].x*(1<<16));
		AET[i].dx=dx;
		AET[i].dir=dir;
		AET[i].idxFrom=idxFrom;
		AET[i].idxTo=idxTo;
	};
	void delAETRecord(char idxAET){
		xassert(nElAET>0 && idxAET<nElAET);
		int k;
		for(k=idxAET; k<nElAET-1; k++) AET[k]=AET[k+1];
		nElAET--;
	};

};

struct sGeoFault {
	vector<Vect2f> pointArr;
	list<sUPoligonN*> poligonArr;
	int step;
	float fstep;
	sGeoFault(const Vect2f& begPoint, float begAngle, float aLenght);
	bool quant(void);
};


struct sUPoligonNSpec : public sUPoligonN {
	bool flagBegin;
	sUPoligonNSpec(point4UP* _basePntArr, int maxAddPnt, point4UP* _addPntArr)
		:sUPoligonN(_basePntArr, maxAddPnt, _addPntArr){
		flagBegin=0;
	};
};
struct sGeoSwelling {
	list<sUPoligonNSpec*> poligonArr;
	int step;
	sGeoSwelling(int x, int y);
	bool quant(void);
};

///////////////////////////////////////////////////////////////////////////////////////////////
//                   Появление моделей из земли
///////////////////////////////////////////////////////////////////////////////////////////////

struct faceM2VM{
	unsigned short v1, v2, v3;
};

struct vrtxM2VM{
	Vect3f xyz;
	//float x, y, z;
	float z1;
	//float nx, ny, nz;
	//float light;
	float sx, sy;
	//int isx, isy, iz1;
	int x, y, z;
};

#define MAX_NAME_MESH_M2VM_LENGHT 20
struct meshM2VM{
	unsigned short numVrtx;	/* Vertice count */
	vrtxM2VM * vrtx;			/* List of vertices */
	unsigned short numFace;	/* Face count */
	faceM2VM *face;			/* List of faces */
	Mat3f matrix;
	Vect3f position;
	Vect3f scaling;

	short sizeX05;
	short sizeY05;
	float kScale;

	char* fname3DS;
	const char* getFName3DS(void);

	///char name[MAX_NAME_MESH_M2VM_LENGHT];
	bool load(const char* fname);//, int numMesh);
	void rotateAndScaling(int XA, int YA, int ZA, float kX, float kY, float kZ);
	void rotateAndScaling(Vect3f& orientation, Vect3f& scaling);
	bool put2KF(int quality, short * KFArr, int sxKF, int syKF, bool flag_inverse=0, short addH=0);

	Vect2s calcSizeXY(void);

	void releaseBuf(void);

	meshM2VM(void){
		numFace=numVrtx=0;
		vrtx=0; face=0;
		position=Vect3f::ZERO;
		fname3DS=0;
	};
	~meshM2VM(void){
		releaseBuf();
	};

};


struct s_commandInformation{
	int numKF;
	int time;
	string names3DS;
	bool flag_inverse;
	short addH;
	int KF2Loop;
	int loopCount;
	short * RasterData;
	meshM2VM* pMesh;
	Vect3f orientation;
	Vect3f scale;
};


struct s_Mesh2VMapDate {
	short * KFAnimationArr;
	short * timeKFArr;
	unsigned short sizeX;
	unsigned short sizeY;
	int amountKF;

	list<s_commandInformation *> commandList_immediately;
	list<s_commandInformation *>::iterator CLIt;
	void resetCommandList();
	void command_setKeyFrame(const int numKF, const char* names3DS, const int time, const Vect3f& orientation, const Vect3f& scale, bool flag_inverse, short addH, int KF2Loop=0, int loopCount=0);
	void compilingCommandList();



	s_Mesh2VMapDate(){
		KFAnimationArr=0;
		amountKF=0;
		sizeX=sizeY=0;
		timeKFArr=0;
	};
	~s_Mesh2VMapDate(){
		if(KFAnimationArr) delete [] KFAnimationArr;
		if(timeKFArr) delete [] timeKFArr;
		resetCommandList();
	};

	void init(int _amountKF, short* _timeKFArr, int sx, int sy);
	void setKFDate(int numKF, const char* names3DS);

};

struct s_EarthUnit {
	s_Mesh2VMapDate* meshDate;

	short xL, yL;
	short nxL, nyL;
	bool flag_move;

	//short previsionKF;
	//short currentKF;

	list<s_commandInformation *>::iterator curKFIt;
	list<s_commandInformation *>::iterator nextKFIt;

	short currentBetweenFrame;

	int frame;

	int vMin;

	short* pCurKFRD;
	short* pNextKFRD;

	//short* array;
	short* inVx;
	short* tmpVx;
	unsigned char* mask;
	short* substare;
	int h_begin;

	bool flag_loop;

	s_EarthUnit(s_Mesh2VMapDate* _meshDate){
		meshDate=_meshDate;
		setLoop();
		mask=0;
		inVx=0;
		tmpVx=0;
		substare=0;
	};

	~s_EarthUnit(){
		if(mask) delete [] mask;
		if(inVx) delete [] inVx;
		if(tmpVx) delete [] tmpVx;
		if(substare) delete [] substare;
	}


	void setLoop(){
		flag_loop=1;
	}
	void resetLoop(){
		flag_loop=0;
	}

	void init(int xMap, int yMap);

	void relativeMove(int dx, int dy);
	void absolutMove(int x, int y);

	bool quant();
};

struct s_Mesh2VMapDispather {
	s_Mesh2VMapDispather(void);
	~s_Mesh2VMapDispather(void);
	vector<s_Mesh2VMapDate*> M2VMDateArr;
	vector<s_EarthUnit*> EUArr;

	//s_EarthUnit* getEarthUnit(const int ID);
	s_EarthUnit* getEarthUnit(s_Mesh2VMapDate * mmDate);

	void deleteEarthUnit(s_EarthUnit* eu, bool autoDeleteMVMDate=1);

	list<meshM2VM*> meshList;
	meshM2VM* getMeshFrom3DS(const char * name3DS);

};




extern s_Mesh2VMapDispather mesh2VMapDispather;

///////////////////////////////////////////////////////////////////////////////////////////////
//                   Муравей
///////////////////////////////////////////////////////////////////////////////////////////////

const int ANT_MAX_MESH2VMAPDATE=36;
class c3DSGeoAction
{
	s_EarthUnit* pEarthUnit;
protected:
	friend class c3DSGeoActionCreator;
	c3DSGeoAction(s_EarthUnit* pEarthUnit_):pEarthUnit(pEarthUnit_){ }
public:
	bool quant(void){
		if(pEarthUnit!=0){
			if(pEarthUnit->quant()==0){
				delEarthUnit();
				return 0;
			}
			else return 1;
		}
		return 0;
	}

	void delEarthUnit(void){
		if(pEarthUnit!=0){
			mesh2VMapDispather.deleteEarthUnit(pEarthUnit, false);
			pEarthUnit=0;
		}
	}

	~c3DSGeoAction(){
		delEarthUnit();
	};
};

struct s3DSGeoParameter
{
	struct CommandSet
	{
		int numKF;
		string names3DS;
		int time;
		bool flag_inverse;
		short addH;
		int KF2Loop;
		int loopCount;
		Vect3f scale;

		CommandSet()
		{
			numKF=0;
			time=0;
			flag_inverse=false;
			addH=0;
			KF2Loop=0;
			loopCount=0;
		}

		CommandSet(const int numKF_, const char* names3DS_, const int time_,
			 const Vect3f& scale_, bool flag_inverse_, short addH_, int KF2Loop_=0, int loopCount_=0)
		{
			numKF=numKF_;
			names3DS=names3DS_;
			time=time_;
			scale=scale_;
			flag_inverse=flag_inverse_;
			addH=addH_;
			KF2Loop=KF2Loop_;
			loopCount=loopCount_;
		}
	};

	vector<CommandSet> command;
};

class c3DSGeoActionCreator
{
public:
	c3DSGeoActionCreator();
	~c3DSGeoActionCreator();

	static c3DSGeoAction* Build(short xc, short yc, float orientation,
		s3DSGeoParameter* command
		//Для разных адресов command образуется разный кеш 
		//Предполагается что это будет лежать в prm
		);

	static c3DSGeoActionCreator* instance();
protected:
	c3DSGeoAction* BuildD(short xc, short yc, float orientation,
		s3DSGeoParameter* command
		);

	friend c3DSGeoAction;
	struct Cache
	{
		s3DSGeoParameter* command;
		vector<s_Mesh2VMapDate*> MMDateArr;
	};
	vector<Cache> array_cache;
};


const float ANT_RAD_IN_CELL_ARR=(2*M_PI)/ANT_MAX_MESH2VMAPDATE;
/*
struct s_AntBirthGeoAction {
	static s_Mesh2VMapDate* MMDateArr[ANT_MAX_MESH2VMAPDATE];
	s_EarthUnit* pEarthUnit;
	s_AntBirthGeoAction(short xc, short yc, float orientation);
	bool quant(void){
		if(pEarthUnit!=0){
			if(pEarthUnit->quant()==0){
				delEarthUnit();
				return 0;
			}
			else return 1;
		}
		return 0;
	};
	void delEarthUnit(void){
		if(pEarthUnit!=0){
			mesh2VMapDispather.deleteEarthUnit(pEarthUnit, false);
			pEarthUnit=0;
		}
	};
	~s_AntBirthGeoAction(){
		delEarthUnit();
	};
};

struct s_AntDeathGeoAction {
	static s_Mesh2VMapDate* MMDateArr[ANT_MAX_MESH2VMAPDATE];
	s_EarthUnit* pEarthUnit;
	s_AntDeathGeoAction(short xc, short yc, float orientation);
	void delEarthUnit(void){
		if(pEarthUnit!=0){
			mesh2VMapDispather.deleteEarthUnit(pEarthUnit, false);
			pEarthUnit=0;
		}
	};
	bool quant(void){
		if(pEarthUnit!=0){
			if(pEarthUnit->quant()==0){
				delEarthUnit();
				return 0;
			}
			else return 1;
		}
		return 0;
	};
	~s_AntDeathGeoAction(void){
		delEarthUnit();
	};
};
*/
///////////////////////////////////////////////////////////////////////////////////////////////
//                   Head
///////////////////////////////////////////////////////////////////////////////////////////////
const unsigned int HEAD_GEOACTION_DAMAGE_RADIUS=90;
struct s_HeadGeoAction {
	s_EarthUnit* pEarthUnit;
	s_HeadGeoAction();
	void init(int x, int y,float radius);
	void finit(void);
	void delEarthUnit(void){
		if(pEarthUnit!=0){
			mesh2VMapDispather.deleteEarthUnit(pEarthUnit, true);
			pEarthUnit=0;
		}
	};
	bool quant(int x, int y){
		if(pEarthUnit!=0){
			pEarthUnit->absolutMove(x,y);
			bool result=pEarthUnit->quant();
			damagingBuildingsTolzer(x, y, HEAD_GEOACTION_DAMAGE_RADIUS);
			//if(!result)delEarthUnit();
			return result;
		}
		return 0;
	};
	~s_HeadGeoAction(void){
		delEarthUnit();
	};
};

extern s_HeadGeoAction headGeoAction;
///////////////////////////////////////////////////////////////////////////////////////////////
//                   Оса
///////////////////////////////////////////////////////////////////////////////////////////////
const int WASP_QUANT_FOR_GEOBREAK=25;
const int WASP_SHIFT_QUANT_FOR_GEOBREAK=5;
struct s_WaspBirthGeoAction {
	short x, y;
	short r;
	geoBreak1* pGeoBrk1;
	int quantCount;
	int radiusCount;
	s_WaspBirthGeoAction(short _xc, short _yc, short _r){
		if(_r > MAX_RADIUS_CIRCLEARR){
			xassert(0&&"exceeding max radius in WaspBirthGeoAction ");
			_r=MAX_RADIUS_CIRCLEARR;
		}
		x=_xc;
		y=_yc;
		r=_r;
		pGeoBrk1=new geoBreak1(x, y, 50, 6);
		quantCount=0;
		radiusCount=0;
	};
	~s_WaspBirthGeoAction(){
		if(pGeoBrk1) delete pGeoBrk1;
	}
	bool quant(void){
		quantCount++;

		if((radiusCount<r) && (pGeoBrk1)) {
			if(pGeoBrk1->quant()==0){
				delete pGeoBrk1;
				pGeoBrk1=0;
			}
		}
		if(quantCount>=WASP_SHIFT_QUANT_FOR_GEOBREAK){
			int i, j;
			const int BORDER=1;
			if(radiusCount < r){
				i = radiusCount;
				int max = maxRad[i];
				int* xx = xRad[i];
				int* yy = yRad[i];
				for(j = 0;j < max;j++) {
					int offB=vMap.offsetBuf(vMap.XCYCL(x + xx[j]), vMap.YCYCL(y + yy[j]));
					vMap.SPutAlt(offB, 1<<VX_FRACTION);
				}
				for(i = 0;i <= radiusCount; i++){
					int max = maxRad[i];
					int* xx = xRad[i];
					int* yy = yRad[i];
					for(j = 0;j < max;j++) {
						int offB=vMap.offsetBuf(vMap.XCYCL(x + xx[j]), vMap.YCYCL(y + yy[j]));
						vMap.AtrBuf[offB]=vMap.AtrBuf[offB]&(~At_SOOTMASK) | At_SOOT;
					}
				}

			}
			else {
				if(radiusCount<r+BORDER){
					for(i = 0; i <= radiusCount; i++){
						i = radiusCount;
						int max = maxRad[i];
						int* xx = xRad[i];
						int* yy = yRad[i];
						for(j = 0;j < max;j++) {
							int offB=vMap.offsetBuf(vMap.XCYCL(x + xx[j]), vMap.YCYCL(y + yy[j]));
							vMap.AtrBuf[offB]=vMap.AtrBuf[offB]&(~At_SOOTMASK) | At_SOOT;
						}
					}
				}
				else {
					damagingBuildingsTolzer(x, y, r);
					return 0;
				}
			}
			radiusCount++;
			vMap.recalcArea2Grid( vMap.XCYCL(x-radiusCount), vMap.YCYCL(y-radiusCount), vMap.XCYCL(x+radiusCount), vMap.YCYCL(y+radiusCount) );
			vMap.regRender( vMap.XCYCL(x-radiusCount), vMap.YCYCL(y-radiusCount), vMap.XCYCL(x+radiusCount), vMap.YCYCL(y+radiusCount) );

		}
		return 1;
	
	};
};

/////////////////////////////////////////////////////////////////////////////////////
//                  Wave
/////////////////////////////////////////////////////////////////////////////////////


struct sGeoWave {
	short x,y;
	short maxRadius;

	int step;

	sGeoWave(short _x, short _y, short _maxRadius);
	bool quant(void);
	float calcMaxAmpWave(short curBegRad);


};

///////////////////////////////////////////////////////////////////////////////////////
//					Volcano
///////////////////////////////////////////////////////////////////////////////////////
struct sTVolcano {
	unsigned short* array;
	unsigned short* inVx;
	int h_begin;
	int x, y, sx, sy;
	int kmNewVx;
	int vMin;
	int iKScaling;
	int frame;
	int currentKP;
	int currentKPFrame;
	unsigned short* pCurKP;
	unsigned short* pPrevKP;
	unsigned int max_template_volcano_height;
	~sTVolcano();
	sTVolcano(int _x, int _y, int _sx, int _sy);
	int quant(void);
};

#endif //__GEO_H__