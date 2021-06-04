#include "stdafxTr.h"

///NOISE FUNCTION II
#include <math.h>
//#include <list.h>

#include "pnint.h"


///#include "3dsftk.h"

#include "limits.h"

#include <algorithm>

#include "auxInlineFuction.h"

#ifdef _PERIMETER_
#include "..\\Render\\inc\\IRenderDevice.h"
#endif //_PERIMETER_

extern float noise3(float vec[]);
static float turbulenceG(float *v, float freq)
{
	float t, vec[3];

	for (t = 0. ; freq >= 1. ; freq /= 2) {
		vec[0] = freq * v[0];
		vec[1] = freq * v[1];
		vec[2] = freq * v[2];
		t += (float) fabs(noise3(vec)) / freq;
	}
	return t;
}

static float bias(float a, float b)
{
	return (float)powf(a, log(b) / log(0.5));
}

static float gain(float a, float b)
{
	float p = (float)( log(1. - b) / log(0.5) );

	if (a < .001)
		return 0.;
	else if (a > .999)
		return 1.;
	if (a < 0.5)
		return (float) (powf(2 * a, p) / 2);
	else
		return (float) (1. - powf(2 * (1. - a), p) / 2);
}

////////////////////////////////////////////////////////////////////////////////////
// Служебные функции
float FRnd(void)
{
	return (float)XRnd(0xFFF)/(float)(0xFFF);//(float)(RAND_MAX+1);
}

static inline int GetAlt4G(int off){
	int v;
	if(vMap.VxDBuf[off]!=0) {
		v=vMap.VxDBuf[off];
	}
	else {
		v=vMap.VxGBuf[off];
	}
	v=(v<<VX_FRACTION)|(vMap.AtrBuf[off]&VX_FRACTION_MASK);
	return v;
}

static inline void PutAlt4G(int h, int off, int oldV){
	if(vMap.VxDBuf[off]==0) { //был гео слой
		vMap.VxGBuf[off]=h>>VX_FRACTION;
		vMap.AtrBuf[off]=(h&VX_FRACTION_MASK) | (vMap.AtrBuf[off]& (~(VX_FRACTION_MASK|At_NOTPURESURFACE)));
	}
	else {//был дам слой
		if( (oldV>h) && ((h>>VX_FRACTION) < vMap.VxGBuf[off] ) ){//выступил гео
			//vMap.VxDBuf[off]=0;//Уменьшение гео сразу не предусмотрено
			//vMap.SetTer(off, f3d.calc(off& vMap.clip_mask_x, off>>vMap.H_SIZE_POWER, vMap.VxGBuf[off]));
			vMap.VxGBuf[off]=vMap.VxDBuf[off]=h>>VX_FRACTION;
			vMap.AtrBuf[off]=(h&VX_FRACTION_MASK) | (vMap.AtrBuf[off]& (~(VX_FRACTION_MASK|At_NOTPURESURFACE)));
		}
		else { //Dam остался
			vMap.VxDBuf[off]=h>>VX_FRACTION;
			vMap.AtrBuf[off]=(h&VX_FRACTION_MASK) | (vMap.AtrBuf[off]& (~(VX_FRACTION_MASK|At_NOTPURESURFACE)));
		}
	}
}

static inline void SPutAlt4G(int off, int h){
	if(vMap.VxDBuf[off]==0) { //был гео слой
		vMap.VxGBuf[off]=h>>VX_FRACTION;
		vMap.AtrBuf[off]=(h&VX_FRACTION_MASK) | (vMap.AtrBuf[off]& (~(VX_FRACTION_MASK|At_NOTPURESURFACE)));
	}
	else {//был дам слой
		vMap.VxDBuf[off]=h>>VX_FRACTION;
		vMap.AtrBuf[off]=(h&VX_FRACTION_MASK) | (vMap.AtrBuf[off]& (~(VX_FRACTION_MASK|At_NOTPURESURFACE)));
	}
}

static inline int GetWholeAlt4G(int off){
	int v;
	if(vMap.VxDBuf[off]!=0) v=vMap.VxDBuf[off];
	else v=vMap.VxGBuf[off];
	return v;
}

//Gauss 
static void gaussFilter(int * alt_buff, double filter_scaling, int xy_size)
{
	int border_mask=xy_size-1;
	const int H = 4;//4
	double filter_array[2*H][2*H];
	int x,y;
	double f,norma = 0;
	double filter_scaling_inv_2 = sqr(1/filter_scaling);
	for(y = -H;y < H;y++)
		for(x = -H;x < H;x++){
			f = exp(-(sqr((double)x) + sqr((double)y))*filter_scaling_inv_2);
			norma += f;
			filter_array[H + y][H + x] = f;
			}
	double norma_inv = 1/norma;
	//cout << "Gaussian filter, factor: " << filter_scaling <<endl;

	int* new_alt_buff = new int[xy_size*xy_size];

	int xx,yy;
	for(yy = 0;yy < (int)xy_size;yy++){
		for(xx = 0;xx < (int)xy_size;xx++){
			f = 0;
			for(y = -H;y < H;y++)
				for(x = -H;x < H;x++){
					f += filter_array[H + y][H + x]*double(alt_buff[((yy + y)&border_mask)*xy_size + ((xx + x)&border_mask)]);
					}
			unsigned char c = round(f*norma_inv);
			new_alt_buff[((yy)*xy_size) + (xx)] = round(f*norma_inv);
			}
	}
	//cout <<endl<<endl;
	memcpy(alt_buff,new_alt_buff,xy_size*xy_size*sizeof(int));
	delete new_alt_buff;
}

// end Gauss




extern int* xRad[MAX_RADIUS_CIRCLEARR + 1];
extern int* yRad[MAX_RADIUS_CIRCLEARR + 1];
extern int maxRad[MAX_RADIUS_CIRCLEARR + 1];

/*
float turbulence2(float point[2], float lofreq, float hifreq) 
{ 
	float freq, t, p[2]; 

	p[0] = point[0] + 123.456; 
	p[1] = point[1]; 

	t = 0; 
	for (freq = lofreq ; freq < hifreq ; freq *= 2.) {
extern float noise2(float vec[2]);
		t += fabs(noise2(p)) / freq; 
		p[0] *= 2.; 
		p[1] *= 2.; 
	} 
	return t - 0.3;
}*/

const int rad =64;//64;//128;//256;//32;
const unsigned int kmGG=0;
const int XYGG_SIZE05=rad>>kmGG;// половина
const int XYGG_SIZE=XYGG_SIZE05<<1; // 
const unsigned int clip_mask_GG=XYGG_SIZE-1;
int GeoGrid[XYGG_SIZE][XYGG_SIZE];
const int MAX_H=(1<<16)-1;//40<<VX_FRACTION;
const float a90=(float)3.1415/2;
int dh=1<<VX_FRACTION;
void geoInfluence(int x,int y)
{
	register int i,j;
//	int max;
//	int* xx,*yy;

	static char flag_first=0;
	flag_first=0;
	if(flag_first==0){//инициализация
		flag_first=1;
		for(i=0; i<XYGG_SIZE; i++){
			float y=(float)(i-XYGG_SIZE05)/(float)XYGG_SIZE05;
			for(j=0; j<XYGG_SIZE; j++){
				float x=(float)(j-XYGG_SIZE05)/(float)XYGG_SIZE05; 
				float f=exp(-(fabsf(x*x*x)+fabsf(y*y*y))/(0.4*0.4));
				GeoGrid[i][j]=round(f*(float)(1<<16));//(1<<16)+XRnd(1<<14);
			}
		}
		//float da=a90/(XYGG_SIZE05-(XYGG_SIZE05>>2));
		float a=0;
/*		for(i = 0;i <= XYGG_SIZE05-10;i++){
			max = maxRad[i];
			xx = xRad[i];
			yy = yRad[i];
			for(j = 0; j < max; j++) {
				GeoGrid[(XYGG_SIZE05 + yy[j]) & clip_mask_GG][(XYGG_SIZE05 + xx[j]) & clip_mask_GG]=(XRnd(max)<<13)+(1<<15);//round((float)MAX_H*((XRnd(XYGG_SIZE05-i)+1)/(i+1)) )//*cos(a))>>15;
			}
			//a+=da;
			//if(a>a90)a=a90;
		}*/

/*		const float modD=20;
		const int obD=16;
		int jMax;
extern double noise1(double arg);
		static float count=XRnd(1000);
		static float last_cout=count;
		count=last_cout+2;
		for(i = 0; i < XYGG_SIZE; i++){
			jMax=obD + noise1((float)(i+count)/modD)*modD;//XRnd(modD)
			for(j = 0; j < jMax; j++) {
				GeoGrid[i][j]=0;
			}
		}
		count+=i;
		for(i = 0; i < XYGG_SIZE; i++){
			jMax=obD + noise1((float)(i+count)/modD)*modD;//XRnd(modD)
			for(j = 0; j < jMax; j++) {
				GeoGrid[j][i]=0;
			}
		}
		count+=i;
		for(i = 0; i < XYGG_SIZE; i++){
			jMax=obD + noise1((float)(i+count)/modD)*modD;//XRnd(modD)
			for(j = 0; j < jMax; j++) {
				GeoGrid[i][XYGG_SIZE-j-1]=0;
			}
		}
		count+=i;
		for(i = 0; i < XYGG_SIZE; i++){
			jMax=obD + noise1((float)(i+count)/modD)*modD;//XRnd(modD)
			for(j = 0; j < jMax; j++) {
				GeoGrid[XYGG_SIZE-j-1][i]=0;
			}
		}
		count+=i;
*/
		//Гаусс размывка
		//gaussFilter(&GeoGrid[0][0], 800.0, XYGG_SIZE);
		
	}

/*	for(i = 0; i < rad<<1; i++){
		for(j = 0; j < rad<<1; j++) {
			int V=GeoGrid[i][j]>>6;
			int offset=vMap.offsetBuf((x+j-rad) & vMap.clip_mask_x, (y+i-rad) & vMap.clip_mask_y);
			vMap.VxGBuf[offset]=V >>VX_FRACTION;
			vMap.AtrBuf[offset]=(V &VX_FRACTION_MASK) | (vMap.AtrBuf[offset]&=~VX_FRACTION_MASK);
		}
	}*/




	int r = rad;
//	double d = 1.0/(rad - r + 1),dd,ds,s;
//	int v,h,k,mean;


/*	for(i = 0;i <= r;i++){
		max = maxRad[i];
		xx = xRad[i];
		yy = yRad[i];
		for(j = 0;j < max;j++) {
			VAR[0]=(float)j/kmx;//(vMap.H_SIZE>>5);
			VAR[1]=(float)i/kmy;//(vMap.V_SIZE>>5);
			dh=turbulence2(VAR,0.2,1)*100;
			vMap.voxSet((x + xx[j]) & vMap.clip_mask_x,(y + yy[j]) & vMap.clip_mask_y,dh);
		}
	}*/
	static float kmx=50;
	static float kmy=50;
	static float kmt=50;//10;
	//kmx+=1;
	//kmy+=1;
	//kmt+=1;

	float VAR[3];
	static float time=2;
	time+=0.5;
extern float turbulence(float point[3], float lofreq, float hifreq);
	static int tV=20;
	tV+=10;
	int d=r<<1;
	int xxx=0;//x%d;
	int yyy=0;//y%d;
	for(i = 0; i < d; i++){
		for(j = 0; j < d; j++) {
			VAR[0]=(float)(j+xxx)/kmx;
			//float var0p=(float)((r<<1)-j)/kmx;
			VAR[1]=(float)(i+yyy)/kmy;
			//float var1p=(float)((r<<1)-i)/kmy;
			VAR[2]=time/kmt;
			float tmp;
			//tmp=turbulence(VAR,4)*256*2;
			//tmp=((float)sin(VAR[2]*VAR[0]*VAR[1]+turbulence(VAR,8))+1)*127.5; 
			/////tmp=(float) (1+sin(VAR[0]*VAR[1]*VAR[2]+turbulence(VAR,0.1,1)))*127.5; 
			tmp=((float)sin(VAR[2]*VAR[0]*VAR[1]+turbulence(VAR, 0.1f, 1.6f))+1)*127.5f; 
			dh=(tmp*tV)/200.0;
			//vMap.voxSet((x+j-r) & vMap.clip_mask_x,(y+i-r) & vMap.clip_mask_y,dh);
			int V=dh*GeoGrid[i>>kmGG][j>>kmGG]>>(16);
			V+=(GeoGrid[i>>kmGG][j>>kmGG]>>7)*tV/200.0;
			int offset=vMap.offsetBuf((x+j-r) & vMap.clip_mask_x, (y+i-r) & vMap.clip_mask_y);
			//V+=vMap.VxGBuf[offset]<<VX_FRACTION;
			//V+=vMap.AtrBuf[offset]&VX_FRACTION_MASK;
			vMap.VxGBuf[offset]=V >>VX_FRACTION;
			vMap.AtrBuf[offset]=(V &VX_FRACTION_MASK) | (vMap.AtrBuf[offset]&=~(VX_FRACTION_MASK));
			//vMap.SurBuf[offset]=tmp;

		}
	}
	vMap.regRender(x - rad*2,y - rad,x + rad,y + rad);
}



///////////////////////////////////////////////////////////////////////////////////////////////
/*
const int tgi_arraySX=256;
const int tgi_arraySY=256;
const int tgi_numFrames=10;
const int tgi_frameTime=10;

struct sTemplateGeoInfluence {
	unsigned short* array;
	sTemplateGeoInfluence(){
		int kmNewVx=20;
		int time=2;
		array=new unsigned short[tgi_arraySX*tgi_arraySY*tgi_numFrames];

		int cnt=0;
		int k;
		for(k=0; k<tgi_numFrames; k++){
			const int kmx=50;
			const int kmy=50;
			const int kmt=100;//50;
			//kmx+=1;
			//kmy+=1;
			//kmt+=1;
			///float VAR[3];
			int VAR[3];

			time+=tgi_frameTime;//deltaTime;
			//static int kmNewVx=20;
			const int delta_kmNewVx=10;
			kmNewVx+=delta_kmNewVx*tgi_frameTime;
			int xxx=0;//x%d;
			int yyy=0;//y%d;
			int i,j,cnt=0;
			for(i = 0; i < tgi_arraySY; i++){
				for(j = 0; j < tgi_arraySX; j++) {
					VAR[0]=(j<<NOISE_FRACTION)/kmx;
					VAR[1]=(i<<NOISE_FRACTION)/kmy;
					VAR[2]=(time<<NOISE_FRACTION)/kmt;

					float tmp;
					float var=(VAR[2]*NOISE_DIVIDER)*(VAR[0]*NOISE_DIVIDER)*(VAR[1]*NOISE_DIVIDER);
					tmp=((float)sin(var+turbulence01_08(VAR))+1)*127.5f;
					int dh=round((tmp*kmNewVx)/200.0);
					int V=dh*tmpltGeo[cnt]>>(16);
					V+=(tmpltGeo[cnt]>>7)*kmNewVx/200;
					//V+=h_begin;
					array[cnt]=V;
				}
			}
		}
	};
};
*/
const int BEGIN_DEEP=30<<VX_FRACTION;
const int DELTA_NOT_DEEP=30<<VX_FRACTION;
CGeoInfluence::CGeoInfluence(int _x, int _y, int _sx, int _sy)
{
	time=2;
	kmNewVx=20;
	x=_x; y=_y;
	sx=_sx; sy=_sy;
	//inVxG=new unsigned char[sx*sy];
	//inAtr=new unsigned char[sx*sy];
	inVx=new unsigned short[sx*sy];
	//inSur=new unsigned char[sx*sy];
	tmpltGeo=new int[sx*sy];
	tmpltSur=new unsigned char[sx*sy];
	prepTmplt();
	unsigned char* geo=vMap.VxGBuf;
	unsigned char* dam=vMap.VxDBuf;
	unsigned char* atr=vMap.AtrBuf;
	int i,j,cnt=0;
	int vmin=MAX_VX_HEIGHT;
	int vmax=0;
	for(i=0; i<sy; i++){
		int offy=vMap.offsetBuf(0, vMap.YCYCL(y+i));
		for(j=0; j<sx; j++){
			int off=offy+vMap.XCYCL(x+j);
			int v;
			if(*(dam+off)==0) v= *(geo+off) <<VX_FRACTION;//v=( *(inVxG+cnt)= *(geo+off) )<<VX_FRACTION;
			else v= *(dam+off) <<VX_FRACTION;//v=( *(inVxG+cnt)= *(dam+off) )<<VX_FRACTION;
			v|=*(atr+off) &VX_FRACTION_MASK;//v|=( *(inAtr+cnt) = *(atr+off) )&VX_FRACTION_MASK;
			*(inVx+cnt)=v;
			if( v < vmin) vmin=v;
			if( v > vmax) vmax=v;
			tmpltSur[cnt]=0;//vMap.GetGeoType(off,XRnd(128<<VX_FRACTION)+tmpltGeo[cnt]>>(11-VX_FRACTION))&(~0x1);//GetGeoType(off,tmpltGeo[cnt]>>8);
			cnt++;
		}
	}
	if((vmax-vmin)>DELTA_NOT_DEEP) h_begin=vmin;
	else h_begin=vmin-BEGIN_DEEP;

	//Придание уникальности каждой горе - для полной поддержки надо ввести в класс noise3 и необходимые ему данные
	///static rndVal=0;
	///rndVal=rand();
	///pnintInit(rndVal);
}
CGeoInfluence::~CGeoInfluence()
{
	//delete [] inVxG;
	//delete [] inAtr;
	delete [] inVx;
	//delete [] inSur;
	delete [] tmpltGeo;
	delete [] tmpltSur;
	sx=sy=0;
}

void CGeoInfluence::prepTmplt(void)
{
	int i,j,cnt=0;
	float sy05=sy/2.0f;
	float sx05=sx/2.0f;
	for(i=0; i<sy; i++){
		float y=(float)(i-sy05)/(float)sy05;
		for(j=0; j<sx; j++){
			float x=(float)(j-sx05)/(float)sx05; 
			float f=exp(-(fabsf(x*x*x)+fabsf(y*y*y))/(0.4*0.4));
			tmpltGeo[cnt]=round(f*(float)(1<<16));//(1<<16)+XRnd(1<<14);
			cnt++;
		}
	}
}

//extern float turbulence(float point[3], float lofreq, float hifreq);
//extern float turbulence01_08(float point[3]);
int CGeoInfluence::quant(int deltaTime)
{
	const int kmx=50;
	const int kmy=50;
	const int kmt=100;//50;
	//kmx+=1;
	//kmy+=1;
	//kmt+=1;
	///float VAR[3];
	int VAR[3];

	time+=deltaTime;//0.5;
	//static int kmNewVx=20;
	const int delta_kmNewVx=10;
	kmNewVx+=delta_kmNewVx;
	int xxx=0;//x%d;
	int yyy=0;//y%d;
	int i,j,cnt=0;
	for(i = 0; i < sy; i++){
		for(j = 0; j < sx; j++) {

			int Vold=inVx[cnt];
			int V=(tmpltGeo[cnt]>>7)*kmNewVx/200;
			int const addHeight=127*kmNewVx/200*tmpltGeo[cnt]>>(16);
			if( h_begin + addHeight + V > Vold) { //BEGIN_DEEP/2
				///VAR[0]=(float)(j+xxx)/kmx;
				///VAR[1]=(float)(i+yyy)/kmy;
				///VAR[2]=time/kmt;
				VAR[0]=(j<<NOISE_FRACTION)/kmx;
				VAR[1]=(i<<NOISE_FRACTION)/kmy;
				VAR[2]=(time<<NOISE_FRACTION)/kmt;

				float tmp;
				//tmp=turbulence(VAR,4)*256*2;
				//tmp=((float)sin(VAR[2]*VAR[0]*VAR[1]+turbulence(VAR,8))+1)*127.5; 
				/////tmp=(float) (1+sin(VAR[0]*VAR[1]*VAR[2]+turbulence(VAR,0.1,1)))*127.5; 
				//tmp=((float)sin(VAR[2]*VAR[0]*VAR[1]+turbulence(VAR, 0.1f, 1.6f))+1)*127.5f; 

				///float var=VAR[2]*VAR[0]*VAR[1];
				float var=(VAR[2]*NOISE_DIVIDER)*(VAR[0]*NOISE_DIVIDER)*(VAR[1]*NOISE_DIVIDER);
				tmp=((float)sin(var+((float)turbulence01_08(VAR)) )+1)*127.5f;
				int dh=round((tmp*kmNewVx)/200.0);

				//vMap.voxSet((x+j-r) & vMap.clip_mask_x,(y+i-r) & vMap.clip_mask_y,dh);
				V+=dh*tmpltGeo[cnt]>>(16);
			}
			else V+=addHeight;
			
			int offset=vMap.offsetBuf((x+j) & vMap.clip_mask_x, (y+i) & vMap.clip_mask_y);

			V+=h_begin;

/*			if(tmpltSur[cnt]&0x1){ //если порода прорезалась
				vMap.VxGBuf[offset]=V >>VX_FRACTION;
				vMap.AtrBuf[offset]=(V &VX_FRACTION_MASK) | (vMap.AtrBuf[offset]&=~VX_FRACTION_MASK);
				vMap.SurBuf[offset]=vMap.GetGeoType(offset,V);
			}
			else { //если порода не прорезалась
				int Vold =vMap.VxGBuf[offset]<<VX_FRACTION; ///
				//Vold+=vMap.AtrBuf[offset]&VX_FRACTION_MASK; ///
				if(V>Vold){//=
					vMap.VxGBuf[offset]=V >>VX_FRACTION;
					vMap.AtrBuf[offset]=(V &VX_FRACTION_MASK) | (vMap.AtrBuf[offset]&=~VX_FRACTION_MASK);
					tmpltSur[cnt]|=0x1;
				}
			}*/
			//V+=inAtr[cnt]&0xF;
			static char fl=0;
			static int k_dh[256];
			if(fl==0){
				fl=1;
				for(int m=0; m<256; m++){
					float x=(float)m/128;//Диапазон от 0 до 2
					k_dh[m]=round((-0.1f+exp(-fabsf((x-1)*(x-1)*(x-1))/(0.4f*0.4f)))*(1<<16));
				}

			}
			//int Vold =inVxG[cnt]<<VX_FRACTION; ///
			//Vold+=inAtr[cnt]&VX_FRACTION_MASK; ///
			if(Vold > V){
				//int inx=(Vold-V)>>3;
				int inx=(Vold-V)>>2;//>>3;
				if(inx>255)inx=255;
				//V=Vold + ((V-h_begin)*k_dh[inx]>>(16+1) );
				int dV=V-h_begin; if (dV>(10<<VX_FRACTION))dV=10<<VX_FRACTION;
				V=Vold + ((dV)*k_dh[inx]>>(16) );
				if(V>MAX_VX_HEIGHT)V=MAX_VX_HEIGHT;
				if(V<0) V=0;
			}
			else {
				if(vMap.VxDBuf[offset]!=0){
					vMap.VxDBuf[offset]=0;
					vMap.SurBuf[offset]=vMap.GetGeoType(offset,V);
				}
			}

			//для нормальной границы при  разрушении зеропласта
			if( (vMap.VxDBuf[offset]==0 && vMap.VxGBuf[offset]==(V >>VX_FRACTION)) || (vMap.VxDBuf[offset]==(V >>VX_FRACTION)) ){ 
				vMap.AtrBuf[offset]=(V &VX_FRACTION_MASK) | (vMap.AtrBuf[offset]&=~VX_FRACTION_MASK);
			}
			else {
				if(vMap.VxDBuf[offset]==0) vMap.VxGBuf[offset]=V >>VX_FRACTION;
				else vMap.VxDBuf[offset]=V >>VX_FRACTION;
				//нужно сохранить только признак тени, признак At_ZEROPLASTPRESENT при любом изменении удаляется
				vMap.AtrBuf[offset]=(V &VX_FRACTION_MASK) | (vMap.AtrBuf[offset]&=~(VX_FRACTION_MASK|At_NOTPURESURFACE));
				//if(vMap.VxDBuf[offset]< V >>VX_FRACTION)vMap.VxDBuf[offset]=0;
			}

			cnt++;
		}
	}
	vMap.recalcArea2Grid(vMap.XCYCL(x-1), vMap.YCYCL(y-1), vMap.XCYCL(x + sx+1), vMap.YCYCL(y + sy+1) );
	vMap.regRender(x, y, x+sx, y+sy);

	const int MAX_TIME=70;
	if(time>=MAX_TIME) return 0;
	else return 1;
}

///////////////////////////////////////////////////////////////////////////////////////////////
//                                         ЧЕРВЯК
///////////////////////////////////////////////////////////////////////////////////////////////
const unsigned int CWORMOUT_SX=30;
const unsigned int CWORMOUT_SY=30;
const unsigned int CWORMOUT_DH_NOT_DEEP=20<<VX_FRACTION;
const unsigned int CWORMOUT_BEGIN_DEEP=20<<VX_FRACTION;

static int dhcc;
CWormOut::CWormOut(int _x, int _y)
{
	x_=_x-CWORMOUT_SX/2; y_=_y-CWORMOUT_SY/2;
	sx_=CWORMOUT_SX; sy_=CWORMOUT_SY;

	inVx=new unsigned short[sx_*sy_];
	tmpltVx=new int[sx_*sy_];

	//подготовка подложки
	int i,j,cnt=0;
	float sy05=sy_/2.0f;
	float sx05=sx_/2.0f;
	for(i=0; i<sy_; i++){
		float y=(float)(i-sy05)/(float)sy05;
		for(j=0; j<sx_; j++){
			float x=(float)(j-sx05)/(float)sx05; 
			float f0=exp(-(fabsf(x*x*x)+fabsf(y*y*y))/(0.4*0.4));
			//tmpltVx[cnt]=round(f0*(float)(1<<16));//(1<<16)+XRnd(1<<14);

			float f=1.0f*(1.0f-sqrt(x*x+y*y));
			if(f<0)f=0;
			f=(f0+f)/2;
			tmpltVx[cnt]=round(f*(float)(1<<16));

			cnt++;
		}
	}

	unsigned char* geo=vMap.VxGBuf;
	unsigned char* dam=vMap.VxDBuf;
	unsigned char* atr=vMap.AtrBuf;
	//int i,j,cnt=0;
	cnt=0;
	int vmin=MAX_VX_HEIGHT;
	int vmax=0;
	for(i=0; i<sy_; i++){
		int offy=vMap.offsetBuf(0, vMap.YCYCL(y_+i));
		for(j=0; j<sx_; j++){
			int off=offy+vMap.XCYCL(x_+j);
			int v;
			if(*(dam+off)==0) v= *(geo+off) <<VX_FRACTION;//v=( *(inVxG+cnt)= *(geo+off) )<<VX_FRACTION;
			else v= *(dam+off) <<VX_FRACTION;//v=( *(inVxG+cnt)= *(dam+off) )<<VX_FRACTION;
			v|=*(atr+off) &VX_FRACTION_MASK;//v|=( *(inAtr+cnt) = *(atr+off) )&VX_FRACTION_MASK;
			*(inVx+cnt)=v;
			if( v < vmin) vmin=v;
			if( v > vmax) vmax=v;
			cnt++;
		}
	}
	if((vmax-vmin)>CWORMOUT_DH_NOT_DEEP) h_begin=vmin;
	else h_begin=vmin-CWORMOUT_BEGIN_DEEP;

	time=0;
	kmNewVx=20;
	dhcc=0;
}

bool CWormOut::quant()
{
	const unsigned int deltaTime=1;
	time+=deltaTime;
	const int delta_kmNewVx=50;//10;//1<<VX_FRACTION;
	kmNewVx+=delta_kmNewVx;
	int xxx=0;
	int yyy=0;
	int i,j,cnt=0;
	for(i = 0; i < sy_; i++){
		for(j = 0; j < sx_; j++) {

/*			int Vold=inVx[cnt];
			int V=(tmpltVx[cnt]>>7)*kmNewVx/200;
			int const addHeight=127*kmNewVx/200*tmpltVx[cnt]>>(16);
			if( h_begin + addHeight + V > Vold) {

				int dh=round(((30)*kmNewVx)/200.0);
				//int dh=( ((40<<VX_FRACTION)*tmpltVx[cnt])>>16)+ kmNewVx;//
				//if(tmpltVx[cnt]) {
					V+=dh;
				//}
			}
			else V+=addHeight;

			V+=h_begin;
*/			
			int Vold=inVx[cnt];
			int V=(tmpltVx[cnt]>>7)*kmNewVx/200;
			int const addHeight=127*kmNewVx/200*tmpltVx[cnt]>>(16);
			if( h_begin + addHeight + V > Vold) {

				float tmp=10<<VX_FRACTION;

				int dh=round((tmp*kmNewVx)/200.0);

				V+=(dh*tmpltVx[cnt]>>16);
			}
			else V+=addHeight;
			
			int offset=vMap.offsetBuf((x_+j) & vMap.clip_mask_x, (y_+i) & vMap.clip_mask_y);

			V+=h_begin;

			if(tmpltVx[cnt]) V+=dhcc;


			//V+=inAtr[cnt]&0xF;
			static char fl=0;
			static int k_dh[256];
			if(fl==0){
				fl=1;
				for(int m=0; m<256; m++){
					float x=(float)m/128;//Диапазон от 0 до 2
					k_dh[m]=round((-0.1f+exp(-fabsf((x-1)*(x-1)*(x-1))/(0.4f*0.4f)))*(1<<16));
				}

			}
			//int Vold =inVxG[cnt]<<VX_FRACTION; ///
			//Vold+=inAtr[cnt]&VX_FRACTION_MASK; ///

			if(Vold >= V){
				//int inx=(Vold-V)>>3;
				int inx=(Vold-V)>>2;//>>3;
				if(inx>255)inx=255;
				//V=Vold + ((V-h_begin)*k_dh[inx]>>(16+1) );
				int dV=V-h_begin; if (dV>(10<<VX_FRACTION))dV=10<<VX_FRACTION;
				V=Vold + ((dV)*k_dh[inx]>>(16) );
				if(V>MAX_VX_HEIGHT)V=MAX_VX_HEIGHT;
				if(V<0) V=0;
			}
			else {
				V=Vold;
			}

			//для нормальной границы при  разрушении зеропласта
			if( (vMap.VxDBuf[offset]==0 && vMap.VxGBuf[offset]==(V >>VX_FRACTION)) || (vMap.VxDBuf[offset]==(V >>VX_FRACTION)) ){ 
				vMap.AtrBuf[offset]=(V &VX_FRACTION_MASK) | (vMap.AtrBuf[offset]&=~VX_FRACTION_MASK);
			}
			else {
				if(vMap.VxDBuf[offset]==0) vMap.VxGBuf[offset]=V >>VX_FRACTION;
				else vMap.VxDBuf[offset]=V >>VX_FRACTION;
				//нужно сохранить только признак тени, признак At_ZEROPLASTPRESENT при любом изменении удаляется
				vMap.AtrBuf[offset]=(V &VX_FRACTION_MASK) | (vMap.AtrBuf[offset]&=~(VX_FRACTION_MASK|At_NOTPURESURFACE));
				//if(vMap.VxDBuf[offset]< V >>VX_FRACTION)vMap.VxDBuf[offset]=0;
			}

			cnt++;
		}
	}
	vMap.recalcArea2Grid(vMap.XCYCL(x_-1), vMap.YCYCL(y_-1), vMap.XCYCL(x_+ sx_+1), vMap.YCYCL(y_ + sy_+1) );
	vMap.regRender(x_, y_, x_+sx_, y_+sy_);

	//dhcc+=1<<VX_FRACTION;

	const int MAX_TIME=16;
	if(time>=MAX_TIME) return 0;
	else return 1;
}

///////////////////////////////////////

CWormFormLib CGeoWorm::wormFormLib;

//const int WSXSY=64;
//const int WSXSY05=WSXSY>>1;
//typedef int tFormWorm[WSXSY][WSXSY];
tFormWorm FormWorms1;
tFormWorm FormWorms2;
tFormWorm* FormWormsNew=&FormWorms1;
tFormWorm* FormWormsOld=&FormWorms1;
/*int backupFormWorms0[WSXSY][WSXSY];
int backupFormWorms1[WSXSY][WSXSY];

int * bForm0[WSXSY][WSXSY];
int * bForm1[WSXSY][WSXSY];
*/
struct s_ter{
	int x,y;
	int ter[WSXSY][WSXSY];
	void buckup(int _x, int _y){
		x=_x; y=_y;
		int i,j;
		for(i=0; i<WSXSY; i++){
			for(j=0; j<WSXSY; j++){
				int offset=vMap.offsetBuf((x+j) & vMap.clip_mask_x, (y+i) & vMap.clip_mask_y);
				int V=vMap.VxGBuf[offset]<<VX_FRACTION;
				V+=vMap.AtrBuf[offset]&VX_FRACTION_MASK;
				ter[i][j]=V;
			}
		}
	};
	void restore(void){
		int i,j;
		for(i=0; i<WSXSY; i++){
			for(j=0; j<WSXSY; j++){
				int V=ter[i][j];
				int offset=vMap.offsetBuf((x+j) & vMap.clip_mask_x, (y+i) & vMap.clip_mask_y);
				vMap.VxGBuf[offset]=V>>VX_FRACTION;
				//vMap.AtrBuf[offset]=V&VX_FRACTION_MASK;
			}
		}
	};
};
s_ter ter;
struct CWormForm {
	sVBitMap VBitMap;
	CWormForm(){
		FormWormsNew=FormWormsOld=&FormWorms1;
		XBuffer tb;
		//tb < dirBitMap   < "\\" < fname;
		tb < Path2TTerraResource < "wormform.tga"; //"RESOURCE\\Tools\\wormform.tga";
		TGAHEAD tgahead;
		if(tgahead.loadHeader(tb)){
			if((tgahead.PixelDepth!=8) || (tgahead.ImageType!=3)) {
				ErrH.Abort("FormWorms TGA - not correct!");
				//AfxMessageBox("Не поддерживаемый тип TGA (необходим монохромный не компрессованный)");
				return;
			}
			VBitMap.create(tgahead.Width, tgahead.Height);
			tgahead.load2buf(VBitMap.data);
			put2WF(0);
		}
		else{
			VBitMap.create(WSXSY, WSXSY);
		}
	};
	void put2WF(float alpha){
		VBitMap.set_alphaR(alpha, WSXSY05, WSXSY05);
		int i,j;
		for(i=0; i<WSXSY; i++){
			float y=(float)(i-WSXSY05)/(float)WSXSY05;
			for(j=0; j<WSXSY; j++){
				//float x=(float)(j-WSXSY05)/(float)WSXSY05; 
				//float f=exp(-(fabsf(x*x)+fabsf(y*y))/(0.4*0.4));
				//(*FormWormsNew)[i][j]=round(f*(float)(1<<16));//(1<<16)+XRnd(1<<14);
				int hh=VBitMap.getPreciseColor(j, i);
				(*FormWormsNew)[i][j]=hh<<8;
			}
/*			for(j=WSXSY>>1; j<WSXSY; j++){
				int t=(WSXSY-j)*FormWorms[WSXSY-1][j]/(WSXSY>>1);
				FormWorms[i][j]=-(t>>2);
			}
*/
		}
	};
	void put2WF2(){
		int i,j;
		for(i=0; i<WSXSY; i++){
			float y=(float)(i-WSXSY05)/(float)WSXSY05;
			for(j=0; j<WSXSY; j++){
				float x=(float)(j-WSXSY05)/(float)WSXSY05; 
				float f=exp(-(fabsf(x*x)+fabsf(y*y))/(0.4*0.4));
				(*FormWormsNew)[i][j]=round(f*(float)(1<<16));//(1<<16)+XRnd(1<<14);
			}
		}
	};
	~CWormForm(){
			VBitMap.release();
	};
};
CWormForm wormForm;
void worms(int x, int y)
{
	static int xold=0, yold=0;
	static int* FB;
	static char flag_first=0;
	int i,j;

	//Определение координат перекрытия с предыдущим кадром
	int XL0=xold, XR0=xold+WSXSY, YT0=yold, YD0=yold+WSXSY;
	int XL1=x,    XR1=x+WSXSY,    YT1=y,    YD1=y+WSXSY;

	int B0xL, B0xR, B0yT, B0yD;
	int E0xL, E0xR, E0yT, E0yD;
	int PAxL, PAxR, PAyT, PAyD;
	int B1xL, B1xR, B1yT, B1yD;
	int E1xL, E1xR, E1yT, E1yD;

	int ChangeAreaXL, ChangeAreaXR, ChangeAreaYT, ChangeAreaYD;

	static char fl=0;
	static int k_dh[256];
	if(fl==0){
		fl=1;
		for(int m=0; m<256; m++){
			float x=(float)m/256;//Диапазон от 0 до 1
			k_dh[m]=(0.99+0.1*sin(x*50))* exp(-fabsf((x)*(x)*(x))/(0.4f*0.4f))*(1<<16);//(1.2+0.05*sin(2*tan(x*50)))
		}
	}

	if(flag_first==0){//инициализация
		flag_first=1;
//		for(i=0; i<WSXSY; i++){
//			float y=(float)(i-WSXSY05)/(float)WSXSY05;
//			for(j=0; j<WSXSY; j++){
//				float x=(float)(j-WSXSY05)/(float)WSXSY05; 
//				float f=exp(-(fabsf(x*x)+fabsf(y*y))/(0.4*0.4));
//				FormWorms[i][j]=round(f*(float)(1<<16));//(1<<16)+XRnd(1<<14);
//			}
/*			for(j=WSXSY>>1; j<WSXSY; j++){
				int t=(WSXSY-j)*FormWorms[WSXSY-1][j]/(WSXSY>>1);
				FormWorms[i][j]=-(t>>2);
			}
*/
//		}

/*		wormForm.put2WF(0.);
		for(i = 0; i < WSXSY; i++){
			for(j = 0; j < WSXSY; j++) {
				int offset=vMap.offsetBuf((j+x) & vMap.clip_mask_x, (i+y) & vMap.clip_mask_y);
				//int V=vMap.VxGBuf[offset]<<VX_FRACTION;
				//V+=vMap.AtrBuf[offset]&VX_FRACTION_MASK;
				int V, Vold;
				Vold=V=GetAlt4G(offset);
				//V+=FormWorms[i-y][j-x]>>8;

				dh=(*FormWormsNew)[i][j];
				int idx=(V-0)>>3; if(idx>255)idx=255;
				V+=((dh)>>7)*k_dh[idx]>>16; //8

				if(V>MAX_VX_HEIGHT)V=MAX_VX_HEIGHT;

				if( (Vold >>VX_FRACTION)==(V >>VX_FRACTION) ){
					vMap.AtrBuf[offset]=(V &VX_FRACTION_MASK) | (vMap.AtrBuf[offset]&=~VX_FRACTION_MASK);
				}
				else {
					//vMap.VxGBuf[offset]=V >>VX_FRACTION;
					//нужно сохранить только признак тени, признак At_ZEROPLASTPRESENT при любом изменении удаляется
					//vMap.AtrBuf[offset]=(V &VX_FRACTION_MASK) | (vMap.AtrBuf[offset]&=~(VX_FRACTION_MASK|At_ZEROPLASTPRESENT));
					PutAlt4G(V, offset, Vold);
				}
			}
		}
*/
		goto loc_notOverlapped;
	}
	else{
		if(FormWormsNew==&FormWorms1){ FormWormsNew=&FormWorms2; FormWormsOld=&FormWorms1;}
		else { FormWormsNew=&FormWorms1; FormWormsOld=&FormWorms2;}
		float curAlpha=atan2f(x-xold, yold-y);
		static float alpha=0;
		float dAlpha=alpha-curAlpha;
		alpha=curAlpha;
//		if(fabs(dAlpha) > (5.0*(3.14/180.0)) ){
			wormForm.put2WF2();
//		}
//		else {
//////			wormForm.put2WF(alpha);
//		}
		//ter.restore();


/*		if(bForm0 == backupFormWorms0){
			bForm0 == backupFormWorms1;
			bForm1 == backupFormWorms0;
		}
		else {
			bForm0 == backupFormWorms0;
			bForm1 == backupFormWorms1;
		}*/
		//ПРоверка на перекрывание области не нужна т.к. они всегда должны перекрываться

		B0xL=XL0; B0xR=XR0;
		B1xL=XL1; B1xR=XR1;
		//определение Y координат базовых блоков
		if(YT0 <= YT1){ // 0 блок выше
			if(YD0 < YT1) goto loc_notOverlapped;
			B0yT=YT0; B0yD=YT1;
			B1yT=YD0; B1yD=YD1;
			E1yT=E0yT=PAyT=YT1; E1yD=E0yD=PAyD=YD0;
			ChangeAreaYT=YT1; ChangeAreaYD=YD0;
		}
		else { // YT1 < YT0 // 1 блок выше
			if(YD1 < YT0) goto loc_notOverlapped;
			B1yT=YT1; B1yD=YT0;
			B0yT=YD1; B0yD=YD0;
			E1yT=E0yT=PAyT=YT0; E1yD=E0yD=PAyD=YD1;
			ChangeAreaYT=YT0; ChangeAreaYD=YD1;
		}
		//Определение X координат экстендед блоков
		if(XL0 <= XL1){ //0 блок левее
			if(XR0 < XL1) goto loc_notOverlapped;
			E0xL=XL0; E0xR=XL1;
			PAxL=XL1; PAxR=XR0;
			E1xL=XR0; E1xR=XR1;
			ChangeAreaXL=XL0; ChangeAreaXR=XR1;
		}
		else{ // XL1 < XL0 // 1 блок левее
			if(XR1 < XL0) goto loc_notOverlapped;
			E1xL=XL1; E1xR=XL0;
			PAxL=XL0; PAxR=XR1;
			E0xL=XR1; E0xR=XR0;
			ChangeAreaXL=XL1; ChangeAreaXR=XR0;
		}
	}
	//ter.buckup(x, y);
	int minV;
	minV=MAX_VX_HEIGHT;
	for(i = 0; i < WSXSY; i++){
		for(j = 0; j < WSXSY; j++) {
			int offset=vMap.offsetBuf(vMap.XCYCL(j+x), vMap.YCYCL(i+y));
			//if(minV > vMap.VxGBuf[offset]<<VX_FRACTION)minV=vMap.VxGBuf[offset]<<VX_FRACTION;
			int tvx=GetWholeAlt4G(offset)<<VX_FRACTION;
			if(minV > tvx) minV=tvx;
		}
	}


	int dh;
	//Для области PA идет падение и одновременно поднятие грунта
	for(i = PAyT; i < PAyD; i++){
		for(j = PAxL; j < PAxR; j++) {
			int offset=vMap.offsetBuf((j) & vMap.clip_mask_x, (i) & vMap.clip_mask_y);
			//int V=vMap.VxGBuf[offset]<<VX_FRACTION;
			//V+=vMap.AtrBuf[offset]&VX_FRACTION_MASK;
			int V, Vold;
			Vold=V=GetAlt4G(offset);
			dh=(*FormWormsOld)[i-yold][j-xold]-(*FormWormsNew)[i-y][j-x];
			int idx=(V-minV)>>3; if(idx>255)idx=255;
			if(dh>0) V-=(dh>>6)*k_dh[idx]>>16; //7
			else V+=((-dh)>>7)*k_dh[idx]>>16; //8

			//V-=FormWorms[i-yold][j-xold]>>7;
			//V+=FormWorms[i-y][j-x]>>8;

			if(V>MAX_VX_HEIGHT)V=MAX_VX_HEIGHT;
			if(V<0)V=0;

			if( (Vold >>VX_FRACTION)==(V >>VX_FRACTION) ){
				vMap.AtrBuf[offset]=(V &VX_FRACTION_MASK) | (vMap.AtrBuf[offset]&=~VX_FRACTION_MASK);
			}
			else {
				//vMap.VxGBuf[offset]=V >>VX_FRACTION;
				//нужно сохранить только признак тени, признак At_ZEROPLASTPRESENT при любом изменении удаляется
				//vMap.AtrBuf[offset]=(V &VX_FRACTION_MASK) | (vMap.AtrBuf[offset]&=~(VX_FRACTION_MASK|At_ZEROPLASTPRESENT));
				PutAlt4G(V, offset, Vold);
			}

		}
	}

	//Для областей B0 и Е0 идет падение(осыпание) грунта
	for(i = B0yT; i < B0yD; i++){
		for(j = B0xL; j < B0xR; j++) {
			int offset=vMap.offsetBuf((j) & vMap.clip_mask_x, (i) & vMap.clip_mask_y);
			//int V=vMap.VxGBuf[offset]<<VX_FRACTION;
			//V+=vMap.AtrBuf[offset]&VX_FRACTION_MASK;
			int V, Vold;
			Vold=V=GetAlt4G(offset);
			//V-=FormWorms[i-yold][j-xold]>>7;

			dh=(*FormWormsOld)[i-yold][j-xold];
			int idx=(V-minV)>>3; if(idx>255)idx=255;
			V-=(dh>>6)*k_dh[idx]>>16; //7

			if(V<0)V=0;

			if( (Vold >>VX_FRACTION)==(V >>VX_FRACTION) ){
				vMap.AtrBuf[offset]=(V &VX_FRACTION_MASK) | (vMap.AtrBuf[offset]&=~VX_FRACTION_MASK);
			}
			else {
				//vMap.VxGBuf[offset]=V >>VX_FRACTION;
				//нужно сохранить только признак тени, признак At_ZEROPLASTPRESENT при любом изменении удаляется
				//vMap.AtrBuf[offset]=(V &VX_FRACTION_MASK) | (vMap.AtrBuf[offset]&=~(VX_FRACTION_MASK|At_ZEROPLASTPRESENT));
				PutAlt4G(V, offset, Vold);
			}

		}
	}
	for(i = E0yT; i < E0yD; i++){
		for(j = E0xL; j < E0xR; j++) {
			int offset=vMap.offsetBuf((j) & vMap.clip_mask_x, (i) & vMap.clip_mask_y);
			//int V=vMap.VxGBuf[offset]<<VX_FRACTION;
			//V+=vMap.AtrBuf[offset]&VX_FRACTION_MASK;
			int V, Vold;
			Vold=V=GetAlt4G(offset);
			//V-=FormWorms[i-yold][j-xold]>>7;

			dh=(*FormWormsOld)[i-yold][j-xold];
			int idx=(V-minV)>>3; if(idx>255)idx=255;
			V-=(dh>>6)*k_dh[idx]>>16; //7

			if(V<0)V=0;

			if( (Vold >>VX_FRACTION)==(V >>VX_FRACTION) ){
				vMap.AtrBuf[offset]=(V &VX_FRACTION_MASK) | (vMap.AtrBuf[offset]&=~VX_FRACTION_MASK);
			}
			else {
				//vMap.VxGBuf[offset]=V >>VX_FRACTION;
				//нужно сохранить только признак тени, признак At_ZEROPLASTPRESENT при любом изменении удаляется
				//vMap.AtrBuf[offset]=(V &VX_FRACTION_MASK) | (vMap.AtrBuf[offset]&=~(VX_FRACTION_MASK|At_ZEROPLASTPRESENT));
				PutAlt4G(V, offset, Vold);
			}
		}
	}

	//Для областей B1 и Е1 идет выпячивание грунта грунта
	for(i = B1yT; i < B1yD; i++){
		for(j = B1xL; j < B1xR; j++) {
			int offset=vMap.offsetBuf((j) & vMap.clip_mask_x, (i) & vMap.clip_mask_y);
			//int V=vMap.VxGBuf[offset]<<VX_FRACTION;
			//V+=vMap.AtrBuf[offset]&VX_FRACTION_MASK;
			int V, Vold;
			Vold=V=GetAlt4G(offset);
			//V+=FormWorms[i-y][j-x]>>8;

			dh=(*FormWormsNew)[i-y][j-x];
			int idx=(V-minV)>>3; if(idx>255)idx=255;
			V+=((dh)>>7)*k_dh[idx]>>16; //8

			if(V>MAX_VX_HEIGHT)V=MAX_VX_HEIGHT;

			if( (Vold >>VX_FRACTION)==(V >>VX_FRACTION) ){
				vMap.AtrBuf[offset]=(V &VX_FRACTION_MASK) | (vMap.AtrBuf[offset]&=~VX_FRACTION_MASK);
			}
			else {
				//vMap.VxGBuf[offset]=V >>VX_FRACTION;
				//нужно сохранить только признак тени, признак At_ZEROPLASTPRESENT при любом изменении удаляется
				//vMap.AtrBuf[offset]=(V &VX_FRACTION_MASK) | (vMap.AtrBuf[offset]&=~(VX_FRACTION_MASK|At_ZEROPLASTPRESENT));
				PutAlt4G(V, offset, Vold);
			}
		}
	}
	for(i = E1yT; i < E1yD; i++){
		for(j = E1xL; j < E1xR; j++) {
			int offset=vMap.offsetBuf((j) & vMap.clip_mask_x, (i) & vMap.clip_mask_y);
			//int V=vMap.VxGBuf[offset]<<VX_FRACTION;
			//V+=vMap.AtrBuf[offset]&VX_FRACTION_MASK;
			int V, Vold;
			Vold=V=GetAlt4G(offset);
			//V+=FormWorms[i-y][j-x]>>8;

			dh=(*FormWormsNew)[i-y][j-x];
			int idx=(V-minV)>>3; if(idx>255)idx=255;
			V+=((dh)>>7)*k_dh[idx]>>16; //8

			if(V>MAX_VX_HEIGHT)V=MAX_VX_HEIGHT;

			if( (Vold >>VX_FRACTION)==(V >>VX_FRACTION) ){
				vMap.AtrBuf[offset]=(V &VX_FRACTION_MASK) | (vMap.AtrBuf[offset]&=~VX_FRACTION_MASK);
			}
			else {
				//vMap.VxGBuf[offset]=V >>VX_FRACTION;
				//нужно сохранить только признак тени, признак At_ZEROPLASTPRESENT при любом изменении удаляется
				//vMap.AtrBuf[offset]=(V &VX_FRACTION_MASK) | (vMap.AtrBuf[offset]&=~(VX_FRACTION_MASK|At_ZEROPLASTPRESENT));
				PutAlt4G(V, offset, Vold);
			}
		}
	}



//	vMap.recalcArea2Grid(vMap.XCYCL(x-10), vMap.YCYCL(y-10), vMap.XCYCL(x + WSXSY+10), vMap.YCYCL(y + WSXSY+10) );
//	vMap.regRender(vMap.XCYCL(x-100), vMap.YCYCL(y-100), vMap.XCYCL(x + WSXSY+100), vMap.YCYCL(y + WSXSY+100) );
	vMap.recalcArea2Grid(vMap.XCYCL(ChangeAreaXL), vMap.YCYCL(ChangeAreaYT), vMap.XCYCL(ChangeAreaXR), vMap.YCYCL(ChangeAreaYD) );
	vMap.regRender(vMap.XCYCL(ChangeAreaXL), vMap.YCYCL(ChangeAreaYT), vMap.XCYCL(ChangeAreaXR), vMap.YCYCL(ChangeAreaYD) );

loc_notOverlapped: ;

	xold=x; yold=y;
}
///////////////////////////////////////////////////////////////////////////////////////
static const float STEP_WORM=5.0;
static const float K_FRND=2.0;
static const float MIN_SPEED=0.5;
static const float MAX_TURN_ANGLE=30*(M_PI/180.0);
CGeoWorm::CGeoWorm(int xBeg, int yBeg, int xTrgt, int yTrgt)
{
	FormWormsNew=&FormWorms1;
	FormWormsOld=&FormWorms1;
	wormFormLib.put2WF2(FormWormsNew);
	xOld=xBeg; yOld=yBeg;
	counter=0;
	cPosition=tPosition=Vect3f::ZERO;
	cDirection=FRnd()* M_PI * 2.0f;
	cSpeed=MIN_SPEED + K_FRND*FRnd();
}

static const int MIN_WORM_PERIOD=10;
static const int MAX_WORM_ADDPERIOD=50;
extern float noise1(float arg);
int CGeoWorm::quant()
{
	static float addAngle=0;
	static int period=0;
	period--;
	if(period<=0){
		period=MIN_WORM_PERIOD+XRnd(MAX_WORM_ADDPERIOD);
	}
LGWQ_1:
	cDirection=addAngle+noise1(counter/40.0)* M_PI * 2.0f;
	cSpeed=0.5 + 0.5*noise1(counter/10.0);
	float x, y;
	float dd=cSpeed*STEP_WORM;
	x=xOld+cos(cDirection)*dd;
	y=yOld+sin(cDirection)*dd;
	if(x<0 || (x+WSXSY)>vMap.H_SIZE || y<0 || (y+WSXSY)>vMap.V_SIZE){
		if(addAngle>0) addAngle=0;
		else addAngle=M_PI;
		goto LGWQ_1;
	}
	step(x,y);
	if(counter>30){
		cDirection+=MAX_TURN_ANGLE;
	}
	counter++;
	return 1;
}


void CGeoWorm::step(int x, int y, float angle)
{
	int i,j;

	//Определение координат перекрытия с предыдущим кадром
	int XL0=xOld, XR0=xOld+WSXSY, YT0=yOld, YD0=yOld+WSXSY;
	int XL1=x,    XR1=x+WSXSY,    YT1=y,    YD1=y+WSXSY;

	damagingBuildingsTolzerS(XL1, YT1, XR1, YD1);

	int B0xL, B0xR, B0yT, B0yD;
	int E0xL, E0xR, E0yT, E0yD;
	int PAxL, PAxR, PAyT, PAyD;
	int B1xL, B1xR, B1yT, B1yD;
	int E1xL, E1xR, E1yT, E1yD;

	int ChangeAreaXL, ChangeAreaXR, ChangeAreaYT, ChangeAreaYD;

	static char fl=0;
	static int k_dh[256];
	if(fl==0){
		fl=1;
		for(int m=0; m<256; m++){
			float x=(float)m/256;//Диапазон от 0 до 1
			k_dh[m]=round( (0.99+0.1*sin(x*50))* exp(-fabsf((x)*(x)*(x))/(0.4f*0.4f))*(1<<16) );//(1.2+0.05*sin(2*tan(x*50)))
		}
	}

	if(counter==0){//инициализация
//		for(i=0; i<WSXSY; i++){
//			float y=(float)(i-WSXSY05)/(float)WSXSY05;
//			for(j=0; j<WSXSY; j++){
//				float x=(float)(j-WSXSY05)/(float)WSXSY05; 
//				float f=exp(-(fabsf(x*x)+fabsf(y*y))/(0.4*0.4));
//				FormWorms[i][j]=round(f*(float)(1<<16));//(1<<16)+XRnd(1<<14);
//			}
/*			for(j=WSXSY>>1; j<WSXSY; j++){
				int t=(WSXSY-j)*FormWorms[WSXSY-1][j]/(WSXSY>>1);
				FormWorms[i][j]=-(t>>2);
			}
*/
//		}

/*		wormForm.put2WF(0.);
		for(i = 0; i < WSXSY; i++){
			for(j = 0; j < WSXSY; j++) {
				int offset=vMap.offsetBuf((j+x) & vMap.clip_mask_x, (i+y) & vMap.clip_mask_y);
				//int V=vMap.VxGBuf[offset]<<VX_FRACTION;
				//V+=vMap.AtrBuf[offset]&VX_FRACTION_MASK;
				int V, Vold;
				Vold=V=GetAlt4G(offset);
				//V+=FormWorms[i-y][j-x]>>8;

				dh=(*FormWormsNew)[i][j];
				int idx=(V-0)>>3; if(idx>255)idx=255;
				V+=((dh)>>7)*k_dh[idx]>>16; //8

				if(V>MAX_VX_HEIGHT)V=MAX_VX_HEIGHT;

				if( (Vold >>VX_FRACTION)==(V >>VX_FRACTION) ){
					vMap.AtrBuf[offset]=(V &VX_FRACTION_MASK) | (vMap.AtrBuf[offset]&=~VX_FRACTION_MASK);
				}
				else {
					//vMap.VxGBuf[offset]=V >>VX_FRACTION;
					//нужно сохранить только признак тени, признак At_ZEROPLASTPRESENT при любом изменении удаляется
					//vMap.AtrBuf[offset]=(V &VX_FRACTION_MASK) | (vMap.AtrBuf[offset]&=~(VX_FRACTION_MASK|At_ZEROPLASTPRESENT));
					PutAlt4G(V, offset, Vold);
				}
			}
		}
*/
		goto loc_notOverlapped;
	}
	else{
		if(FormWormsNew==&FormWorms1){ FormWormsNew=&FormWorms2; FormWormsOld=&FormWorms1;}
		else { FormWormsNew=&FormWorms1; FormWormsOld=&FormWorms2;}
		float curAlpha=angle;//atan2(x-xOld, yOld-y);
		static float alpha=0;
		float dAlpha=alpha-curAlpha;
		alpha=curAlpha;
//		if(fabs(dAlpha) > (15.0*(3.14/180.0)) ){
			wormFormLib.put2WF2(FormWormsNew);
//		}
//		else {
//			wormFormLib.put2WF(alpha+M_PI/2, FormWormsNew);
//		}
		//ter.restore();


/*		if(bForm0 == backupFormWorms0){
			bForm0 == backupFormWorms1;
			bForm1 == backupFormWorms0;
		}
		else {
			bForm0 == backupFormWorms0;
			bForm1 == backupFormWorms1;
		}*/
		//ПРоверка на перекрывание области не нужна т.к. они всегда должны перекрываться

		B0xL=XL0; B0xR=XR0;
		B1xL=XL1; B1xR=XR1;
		//определение Y координат базовых блоков
		if(YT0 <= YT1){ // 0 блок выше
			if(YD0 < YT1) goto loc_notOverlapped;
			B0yT=YT0; B0yD=YT1;
			B1yT=YD0; B1yD=YD1;
			E1yT=E0yT=PAyT=YT1; E1yD=E0yD=PAyD=YD0;
			ChangeAreaYT=YT1; ChangeAreaYD=YD0;
		}
		else { // YT1 < YT0 // 1 блок выше
			if(YD1 < YT0) goto loc_notOverlapped;
			B1yT=YT1; B1yD=YT0;
			B0yT=YD1; B0yD=YD0;
			E1yT=E0yT=PAyT=YT0; E1yD=E0yD=PAyD=YD1;
			ChangeAreaYT=YT0; ChangeAreaYD=YD1;
		}
		//Определение X координат экстендед блоков
		if(XL0 <= XL1){ //0 блок левее
			if(XR0 < XL1) goto loc_notOverlapped;
			E0xL=XL0; E0xR=XL1;
			PAxL=XL1; PAxR=XR0;
			E1xL=XR0; E1xR=XR1;
			ChangeAreaXL=XL0; ChangeAreaXR=XR1;
		}
		else{ // XL1 < XL0 // 1 блок левее
			if(XR1 < XL0) goto loc_notOverlapped;
			E1xL=XL1; E1xR=XL0;
			PAxL=XL0; PAxR=XR1;
			E0xL=XR1; E0xR=XR0;
			ChangeAreaXL=XL1; ChangeAreaXR=XR0;
		}
	}
	//ter.buckup(x, y);
	int minV;
	minV=MAX_VX_HEIGHT;
	for(i = 0; i < WSXSY; i++){
		for(j = 0; j < WSXSY; j++) {
			int offset=vMap.offsetBuf(vMap.XCYCL(j+x), vMap.YCYCL(i+y));
			//if(minV > vMap.VxGBuf[offset]<<VX_FRACTION)minV=vMap.VxGBuf[offset]<<VX_FRACTION;
			int tvx=GetWholeAlt4G(offset)<<VX_FRACTION;
			if(minV > tvx) minV=tvx;
		}
	}


	int dh;
	//Для области PA идет падение и одновременно поднятие грунта
	for(i = PAyT; i < PAyD; i++){
		for(j = PAxL; j < PAxR; j++) {
			int offset=vMap.offsetBuf((j) & vMap.clip_mask_x, (i) & vMap.clip_mask_y);
			//int V=vMap.VxGBuf[offset]<<VX_FRACTION;
			//V+=vMap.AtrBuf[offset]&VX_FRACTION_MASK;
			int V, Vold;
			Vold=V=GetAlt4G(offset);
			dh=(*FormWormsOld)[i-yOld][j-xOld]-(*FormWormsNew)[i-y][j-x];
			int idx=(V-minV)>>3; if(idx>255)idx=255; if(idx<0)idx=0;// <0-не обязательно
			if(dh>0) V-=(dh>>6)*k_dh[idx]>>16; //7
			else V+=((-dh)>>7)*k_dh[idx]>>16; //8

			//V-=FormWorms[i-yold][j-xold]>>7;
			//V+=FormWorms[i-y][j-x]>>8;

			if(V>MAX_VX_HEIGHT)V=MAX_VX_HEIGHT;
			if(V<0)V=0;

			if( (Vold >>VX_FRACTION)==(V >>VX_FRACTION) ){
				vMap.AtrBuf[offset]=(V &VX_FRACTION_MASK) | (vMap.AtrBuf[offset]&=~VX_FRACTION_MASK);
			}
			else {
				//vMap.VxGBuf[offset]=V >>VX_FRACTION;
				//нужно сохранить только признак тени, признак At_ZEROPLASTPRESENT при любом изменении удаляется
				//vMap.AtrBuf[offset]=(V &VX_FRACTION_MASK) | (vMap.AtrBuf[offset]&=~(VX_FRACTION_MASK|At_ZEROPLASTPRESENT));
				PutAlt4G(V, offset, Vold);
			}

		}
	}

	//Для областей B0 и Е0 идет падение(осыпание) грунта
	for(i = B0yT; i < B0yD; i++){
		for(j = B0xL; j < B0xR; j++) {
			int offset=vMap.offsetBuf((j) & vMap.clip_mask_x, (i) & vMap.clip_mask_y);
			//int V=vMap.VxGBuf[offset]<<VX_FRACTION;
			//V+=vMap.AtrBuf[offset]&VX_FRACTION_MASK;
			int V, Vold;
			Vold=V=GetAlt4G(offset);
			//V-=FormWorms[i-yold][j-xold]>>7;

			dh=(*FormWormsOld)[i-yOld][j-xOld];
			int idx=(V-minV)>>3; if(idx>255)idx=255; if(idx<0)idx=0;
			V-=(dh>>6)*k_dh[idx]>>16; //7

			if(V<0)V=0;

			if( (Vold >>VX_FRACTION)==(V >>VX_FRACTION) ){
				vMap.AtrBuf[offset]=(V &VX_FRACTION_MASK) | (vMap.AtrBuf[offset]&=~VX_FRACTION_MASK);
			}
			else {
				//vMap.VxGBuf[offset]=V >>VX_FRACTION;
				//нужно сохранить только признак тени, признак At_ZEROPLASTPRESENT при любом изменении удаляется
				//vMap.AtrBuf[offset]=(V &VX_FRACTION_MASK) | (vMap.AtrBuf[offset]&=~(VX_FRACTION_MASK|At_ZEROPLASTPRESENT));
				PutAlt4G(V, offset, Vold);
			}

		}
	}
	for(i = E0yT; i < E0yD; i++){
		for(j = E0xL; j < E0xR; j++) {
			int offset=vMap.offsetBuf((j) & vMap.clip_mask_x, (i) & vMap.clip_mask_y);
			//int V=vMap.VxGBuf[offset]<<VX_FRACTION;
			//V+=vMap.AtrBuf[offset]&VX_FRACTION_MASK;
			int V, Vold;
			Vold=V=GetAlt4G(offset);
			//V-=FormWorms[i-yold][j-xold]>>7;

			dh=(*FormWormsOld)[i-yOld][j-xOld];
			int idx=(V-minV)>>3; if(idx>255)idx=255; if(idx<0)idx=0;
			V-=(dh>>6)*k_dh[idx]>>16; //7

			if(V<0)V=0;

			if( (Vold >>VX_FRACTION)==(V >>VX_FRACTION) ){
				vMap.AtrBuf[offset]=(V &VX_FRACTION_MASK) | (vMap.AtrBuf[offset]&=~VX_FRACTION_MASK);
			}
			else {
				//vMap.VxGBuf[offset]=V >>VX_FRACTION;
				//нужно сохранить только признак тени, признак At_ZEROPLASTPRESENT при любом изменении удаляется
				//vMap.AtrBuf[offset]=(V &VX_FRACTION_MASK) | (vMap.AtrBuf[offset]&=~(VX_FRACTION_MASK|At_ZEROPLASTPRESENT));
				PutAlt4G(V, offset, Vold);
			}
		}
	}

	//Для областей B1 и Е1 идет выпячивание грунта грунта
	for(i = B1yT; i < B1yD; i++){
		for(j = B1xL; j < B1xR; j++) {
			int offset=vMap.offsetBuf((j) & vMap.clip_mask_x, (i) & vMap.clip_mask_y);
			//int V=vMap.VxGBuf[offset]<<VX_FRACTION;
			//V+=vMap.AtrBuf[offset]&VX_FRACTION_MASK;
			int V, Vold;
			Vold=V=GetAlt4G(offset);
			//V+=FormWorms[i-y][j-x]>>8;

			dh=(*FormWormsNew)[i-y][j-x];
			int idx=(V-minV)>>3; if(idx>255)idx=255; if(idx<0)idx=0;
			V+=((dh)>>7)*k_dh[idx]>>16; //8

			if(V>MAX_VX_HEIGHT)V=MAX_VX_HEIGHT;

			if( (Vold >>VX_FRACTION)==(V >>VX_FRACTION) ){
				vMap.AtrBuf[offset]=(V &VX_FRACTION_MASK) | (vMap.AtrBuf[offset]&=~VX_FRACTION_MASK);
			}
			else {
				//vMap.VxGBuf[offset]=V >>VX_FRACTION;
				//нужно сохранить только признак тени, признак At_ZEROPLASTPRESENT при любом изменении удаляется
				//vMap.AtrBuf[offset]=(V &VX_FRACTION_MASK) | (vMap.AtrBuf[offset]&=~(VX_FRACTION_MASK|At_ZEROPLASTPRESENT));
				PutAlt4G(V, offset, Vold);
			}
		}
	}
	for(i = E1yT; i < E1yD; i++){
		for(j = E1xL; j < E1xR; j++) {
			int offset=vMap.offsetBuf((j) & vMap.clip_mask_x, (i) & vMap.clip_mask_y);
			//int V=vMap.VxGBuf[offset]<<VX_FRACTION;
			//V+=vMap.AtrBuf[offset]&VX_FRACTION_MASK;
			int V, Vold;
			Vold=V=GetAlt4G(offset);
			//V+=FormWorms[i-y][j-x]>>8;

			dh=(*FormWormsNew)[i-y][j-x];
			int idx=(V-minV)>>3; if(idx>255)idx=255; if(idx<0)idx=0;
			V+=((dh)>>7)*k_dh[idx]>>16; //8

			if(V>MAX_VX_HEIGHT)V=MAX_VX_HEIGHT;

			if( (Vold >>VX_FRACTION)==(V >>VX_FRACTION) ){
				vMap.AtrBuf[offset]=(V &VX_FRACTION_MASK) | (vMap.AtrBuf[offset]&=~VX_FRACTION_MASK);
			}
			else {
				//vMap.VxGBuf[offset]=V >>VX_FRACTION;
				//нужно сохранить только признак тени, признак At_ZEROPLASTPRESENT при любом изменении удаляется
				//vMap.AtrBuf[offset]=(V &VX_FRACTION_MASK) | (vMap.AtrBuf[offset]&=~(VX_FRACTION_MASK|At_ZEROPLASTPRESENT));
				PutAlt4G(V, offset, Vold);
			}
		}
	}



//	vMap.recalcArea2Grid(vMap.XCYCL(x-10), vMap.YCYCL(y-10), vMap.XCYCL(x + WSXSY+10), vMap.YCYCL(y + WSXSY+10) );
//	vMap.regRender(vMap.XCYCL(x-100), vMap.YCYCL(y-100), vMap.XCYCL(x + WSXSY+100), vMap.YCYCL(y + WSXSY+100) );
	vMap.recalcArea2Grid(vMap.XCYCL(ChangeAreaXL), vMap.YCYCL(ChangeAreaYT), vMap.XCYCL(ChangeAreaXR), vMap.YCYCL(ChangeAreaYD) );
	vMap.regRender(vMap.XCYCL(ChangeAreaXL), vMap.YCYCL(ChangeAreaYT), vMap.XCYCL(ChangeAreaXR), vMap.YCYCL(ChangeAreaYD) );

loc_notOverlapped: ;
	counter++; //!!!
	xOld=x; yOld=y;
}


///////////////////////////////////////////////////////////////////////////////////////////////
//                                     ТРЕЩИНЫ(РАЗЛОМЫ)
///////////////////////////////////////////////////////////////////////////////////////////////
int elementGeoBreak::unengagedID=0; //Инициализация уникального ида элемента трещины для поиска родителей при завершении

geoBreak1::geoBreak1(int x, int y, int rad, int beginNumBreaks){ //0-случайное кол-во
	elGB.erase(elGB.begin(), elGB.end());//очистка списка элементов трещин
	if(beginNumBreaks==0) beginNumBreaks=2+XRnd(MAX_BEGIN_BREAKS-2);//Диапазон от 2 до MAX_BEGIN_BREAKS

	float range_corner=2*pi/((float)beginNumBreaks);
	float begin_corner=0;
	for(int i=0; i<beginNumBreaks; i++){
		float alpha1=begin_corner+(FRnd()*range_corner);
		for(;alpha1>2*pi; alpha1-=2*pi);
		float lenght=MAX_LENGHT_ELEMENTGEOBREAK+XRnd(MAX_LENGHT_ELEMENTGEOBREAK);
		elementGeoBreak* el=new elementGeoBreak(x, y, alpha1, lenght);
		elGB.push_back(el);
		begin_corner+=range_corner;
	}
};

list<elementGeoBreak*>::iterator geoBreak1::delEementGeoBreak(list<elementGeoBreak*>::iterator pp)
{
	int _ownID=(*pp)->ownerID;
	if(_ownID!=0){
		list<elementGeoBreak*>::iterator pp2;
		int counterChildren=0;
		for(pp2 = elGB.begin(); pp2 != elGB.end(); pp2++) if((*pp2)->ownerID==_ownID)counterChildren++;
		if(counterChildren==1){
			for(pp2 = elGB.begin(); pp2 != elGB.end(); ){
				if((*pp2)->ID==_ownID) pp2=delEementGeoBreak(pp2);
				else pp2++;
			}
		}
	}
	delete (*pp);
	return(elGB.erase(pp));
};

int geoBreak1::quant(void){
	list<elementGeoBreak*>::iterator pp;
	for(pp = elGB.begin(); pp != elGB.end(); ){
		eReturnQuantResult result=(*pp)->quant();

		if(result==HEAD_IN_FINAL_POINT) {
			//int branhings=1+XRnd(MAX_BRANCHINGS_BREAKS);
			//for(int i=0; i<branhings; i++)
			//выбор количества разломо(такой способ используется для того что-бы MAX_BRANCHINGS_BREAKS выбор был наименее вероятен)
			int branchings;
			for(branchings=0; branchings<MAX_BRANCHINGS_BREAKS; branchings++) if(XRnd(2)==0) break;
			if(branchings==0){
				pp=delEementGeoBreak(pp);
				continue;
			}
			float range_corner=CORNER_DISPERSION_BREAKS/((float)branchings);
			float begin_corner=(*pp)->alpha-(CORNER_DISPERSION_BREAKS/2);
			for(int i=0; i<branchings; i++){
				float alpha1=begin_corner+(FRnd()*range_corner);
				for(;alpha1>2*pi; alpha1-=2*pi);
				float lenght=MAX_LENGHT_ELEMENTGEOBREAK+XRnd(MAX_LENGHT_ELEMENTGEOBREAK);
				elementGeoBreak* el=new elementGeoBreak((*pp)->xend, (*pp)->yend, alpha1, lenght, (*pp)->ID);
				elGB.push_back(el);
				begin_corner+=range_corner;
			}
			//if(i==0){
				//delete (*pp);
				//pp=elGB.erase(pp);
			//	pp=delEementGeoBreak(pp);
			//}
		}
		else if(result==END_QUANT) { //Удаление элемента трещины
				//delete (*pp);
				//pp=elGB.erase(pp);
				pp=delEementGeoBreak(pp);
				continue;
		}
		pp++; 
	}
	if(elGB.size()==0) return 0;
	else return 1;
};


// SAMPL
void gBreak(int xbeg, int ybeg, char fl_first)
{
	static int x,y;
	//static int fl_first=0;
	static geoBreak1 * gb=0;//, * gb1, *gb2, *gb3, *gb4, *gb5, *gb6;
	if(fl_first==1){
		srand( 7 );
		x=xbeg; y=ybeg;
		if(!gb) delete gb;
		gb=new geoBreak1(xbeg, ybeg);
	}
	else{
		if(gb)
			if(!gb->quant()){
				delete gb;
				gb=NULL;
			}
	}
//
}

//////////////////////////////////////////////////////////////////////////////////////////

void singleGeoBreak::geoLine2(int xbeg, int ybeg, int sx, int sy, int deltaH)
{

	int x_cur=xbeg<<16;
	int y_cur=ybeg<<16;

	if(abs(sx)>=abs(sy)){
		int stepdy=sy*(1<<16)/abs(sx);
		int stepdx;
		if(sx>0)stepdx=1; else stepdx=-1;
		for(int i=0; i!=sx; i+=stepdx, x_cur+=stepdx<<16, y_cur+=stepdy){
			vMap.SDig(vMap.XCYCL(x_cur>>16), vMap.YCYCL(y_cur>>16), deltaH);
		}
	}
	else { // sy_real> sx_real
		if(abs(sy)>abs(sx)){
			int stepdx=sx*(1<<16)/abs(sy);
			int stepdy;
			if(sy>0)stepdy=1; else stepdy=-1;
			for(int i=0; i!=sy; i+=stepdy, y_cur+=stepdy<<16, x_cur+=stepdx){
				///vMap.voxSet(vMap.XCYCL(x_cur>>16), vMap.YCYCL(y_cur>>16), -6);
				vMap.SDig(vMap.XCYCL(x_cur>>16), vMap.YCYCL(y_cur>>16), deltaH);
			}
		}
	}
	int xL, yT, xR, yD;
	if(sx>0) { xL=vMap.XCYCL(xbeg); xR=vMap.XCYCL(xbeg+sx+1); }
	else{ xL=vMap.XCYCL(xbeg+sx+1); xR=vMap.XCYCL(xbeg); }
	if(sy>0) { yT=vMap.YCYCL(ybeg); yD=vMap.YCYCL(ybeg+sy+1); }
	else { yT=vMap.YCYCL(ybeg+sy+1); yD=vMap.YCYCL(ybeg); }
	vMap.recalcArea2Grid(xL, yT, xR, yD );
	//extern int STATUS_DEBUG_RND;
	//STATUS_DEBUG_RND=1;
	vMap.regRender(xL, yT, xR, yD);
	//STATUS_DEBUG_RND=0;
}


void singleGeoBreak::breakGeoLine3(int * xp, int * yp, int maxPnt, int deltaH, int dx, int dy, int cntLine)
{
	short dh=deltaH>>VX_FRACTION;
	if(dy>0){
		int d;
		for(d=1; d<maxPnt; d++){
			//geoLine2(xp[d-1], yp[d-1]-k+beg_offfsety, xp[d]-xp[d-1], yp[d]-yp[d-1], DH);
			int xbeg=xp[d-1]/*+dx*/;
			int ybeg=yp[d-1]+dy;
			int x_cur=xbeg<<16;
			int y_cur=ybeg<<16;
			int sx=xp[d]-xp[d-1];
			int sy=yp[d]-yp[d-1];

			if(sx<=0) continue;

			int stepdy=sy*(1<<16)/abs(sx);
			int stepdx;
			if(sx>0)stepdx=1; else stepdx=-1;
			for(int i=0; i!=sx; i+=stepdx, x_cur+=stepdx<<16, y_cur+=stepdy){
				for(int j=0; j<cntLine; j++){
					//vMap.voxSet(vMap.XCYCL(x_cur>>16), vMap.YCYCL(j+(y_cur>>16)), -deltaH);
					//vMap.SDig(vMap.XCYCL(x_cur>>16), vMap.YCYCL(j+(y_cur>>16)), deltaH);
					unsigned int off=vMap.offsetBufC(x_cur>>16, j+(y_cur>>16));
					short vg=vMap.VxGBuf[off];
					short vd=vMap.VxDBuf[off];
					if(vd==0) { 
						vg-=dh; 
						if(vg<0){ vg=0; vMap.AtrBuf[off]&=~VX_FRACTION_MASK; }
						vMap.VxGBuf[off]=vg;
					}
					else {
						vd-=dh;
						if(vd<0) { vd=0; vMap.AtrBuf[off]&=~VX_FRACTION_MASK;}
						if(vd < vg) { 
							vMap.VxDBuf[off]=0; vMap.VxGBuf[off]=vd;
							vMap.SetTer(off, vMap.GetGeoType(off, (vd<<VX_FRACTION)|(vMap.AtrBuf[off]&VX_FRACTION_MASK) )); 
						}
						else { vMap.VxDBuf[off]=vd; }
					}
				}
			}

			int xL, yT, xR, yD;
			if(sx>0) { xL=vMap.XCYCL(xbeg); xR=vMap.XCYCL(xbeg+sx); }
			else{ xL=vMap.XCYCL(xbeg+sx); xR=vMap.XCYCL(xbeg); }
			if(sy>0) { yT=vMap.YCYCL(ybeg); yD=vMap.YCYCL(ybeg+sy+cntLine); }
			else { yT=vMap.YCYCL(ybeg+sy); yD=vMap.YCYCL(ybeg+cntLine); }
			vMap.recalcArea2Grid(xL, yT, xR, yD );
			vMap.regRender(xL, yT, xR, yD);
		}
	}
	else if(dy<0){
	}
	else if(dx>0){
	}
	else if(dx<0){
	}

/*
	int x_cur=xbeg<<16;
	int y_cur=ybeg<<16;

	if(abs(sx)>=abs(sy)){
		int stepdy=sy*(1<<16)/abs(sx);
		int stepdx;
		if(sx>0)stepdx=1; else stepdx=-1;
		for(int i=0; i!=sx; i+=stepdx, x_cur+=stepdx<<16, y_cur+=stepdy){
			vMap.SDig(vMap.XCYCL(x_cur>>16), vMap.YCYCL(y_cur>>16), deltaH);
		}
	}
	else { // sy_real> sx_real
		if(abs(sy)>abs(sx)){
			int stepdx=sx*(1<<16)/abs(sy);
			int stepdy;
			if(sy>0)stepdy=1; else stepdy=-1;
			for(int i=0; i!=sy; i+=stepdy, y_cur+=stepdy<<16, x_cur+=stepdx){
				///vMap.voxSet(vMap.XCYCL(x_cur>>16), vMap.YCYCL(y_cur>>16), -6);
				vMap.SDig(vMap.XCYCL(x_cur>>16), vMap.YCYCL(y_cur>>16), deltaH);
			}
		}
	}
	int xL, yT, xR, yD;
	if(sx>0) { xL=vMap.XCYCL(xbeg); xR=vMap.XCYCL(xbeg+sx+1); }
	else{ xL=vMap.XCYCL(xbeg+sx+1); xR=vMap.XCYCL(xbeg); }
	if(sy>0) { yT=vMap.YCYCL(ybeg); yD=vMap.YCYCL(ybeg+sy+1); }
	else { yT=vMap.YCYCL(ybeg+sy+1); yD=vMap.YCYCL(ybeg); }
	vMap.recalcArea2Grid(xL, yT, xR, yD );
	//extern int STATUS_DEBUG_RND;
	//STATUS_DEBUG_RND=1;
	vMap.regRender(xL, yT, xR, yD);
	//STATUS_DEBUG_RND=0;
*/
}



eReturnQuantResult singleGeoBreak::quant(void)
{
	headSection+=1;
	static int beg_offfsety=0;
	if(headSection <= numSections){//Идет рост трещины
		const float dispersionAngle=40.f*M_PI/180.f;
		const float dispersionAngle05=dispersionAngle/2.f;
		const float dispersionAngle025=dispersionAngle/4.f;
		int x_recomend=xbeg+(dx_step*headSection>>16);
		int y_recomend=ybeg+(dy_step*headSection>>16);

		int sx_real=x_recomend - xp[headSection-1];
		int sy_real=y_recomend - yp[headSection-1];
		int x_cur=xp[headSection-1]<<16;
		int y_cur=yp[headSection-1]<<16;
		if(headSection < numSections){
			float ss=sqrtf(sy_real*sy_real+sx_real*sx_real);
			float cosAlpha=(float)sx_real/ss;
			float sinAlpha=(float)sy_real/ss;
			//Noising
			double alpha=atan(sinAlpha/cosAlpha);//atan((double)sy_real/(double)sx_real);
			alpha=alpha+(frnd(dispersionAngle05)-dispersionAngle05);
			//int nx=(DENSITY_NOISE>>1) + XRnd(DENSITY_NOISE>>1); //DENSITY_NOISE;//
			//int ny=XRnd(8)-4;
			//sx_real=(float)nx*cosAlpha+ (float)ny*sinAlpha;
			//sy_real=-(-(float)nx*sinAlpha+ (float)ny*cosAlpha);
			//sx_real=sx_real+ XRnd(DENSITY_NOISE<<1)- (DENSITY_NOISE<<1);// if(sx_real<0) sx_real=0;
			//sy_real=sy_real+ XRnd(DENSITY_NOISE<<1)- (DENSITY_NOISE<<1);// if(sy_real<0) sy_real=0;
			sx_real=sx_real/2+ cos(alpha)*(XRnd(DENSITY_NOISE<<1)+ (DENSITY_NOISE<<1))/2;// if(sx_real<0) sx_real=0;
			sy_real=sy_real/2+ sin(alpha)*(XRnd(DENSITY_NOISE<<1)+ (DENSITY_NOISE<<1))/2;// if(sy_real<0) sy_real=0;
		}
		//DrawLine
		geoLine(xp[headSection-1], yp[headSection-1]+beg_offfsety, sx_real, sy_real);
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

	if(headSection > numSections){
		const int MAX_WIDTH=40;
		const int DH=MAX_VX_HEIGHT/MAX_WIDTH;

		static int curWitch=1;
		static int beg_offfsety=1;

		breakGeoLine3(xp, yp, numSections+1, DH, 0, 1+beg_offfsety-curWitch, curWitch);

/*
		for(int k=0; k<curWitch; k++){
			int d;
			for(d=numSections; d>0; d--){
				geoLine2(xp[d-1], yp[d-1]-k+beg_offfsety, xp[d]-xp[d-1], yp[d]-yp[d-1], DH);
			}
			if(k>=2) break;
		}
*/
		beg_offfsety++;
		curWitch=beg_offfsety;
		if(curWitch>MAX_WIDTH){
			curWitch=MAX_WIDTH;
		}

	}

	if(headSection == numSections) return HEAD_IN_FINAL_POINT;
	if( d >= numSections) return CONTINUE_QUANT;//END_QUANT;
	return CONTINUE_QUANT;
}



//////////////////////////////////////////////////////////////////////////////////////////
static void gaussFilter4LS(unsigned short * alt_buff, double filter_scaling, int x_size, int y_size)
{
	int border_mask_x=x_size-1;
	int border_mask_y=y_size-1;
	const int H = 4;//4
	double filter_array[2*H][2*H];
	int x,y;
	double f,norma = 0;
	double filter_scaling_inv_2 = sqr(1/filter_scaling);
	for(y = -H;y < H;y++)
		for(x = -H;x < H;x++){
			f = exp(-(sqr((double)x) + sqr((double)y))*filter_scaling_inv_2);
			norma += f;
			filter_array[H + y][H + x] = f;
			}
	double norma_inv = 1/norma;
	//cout << "Gaussian filter, factor: " << filter_scaling <<endl;

	unsigned short* new_alt_buff = new unsigned short[x_size*y_size];

	int xx,yy;
	for(yy = 0;yy < (int)y_size;yy++){
		for(xx = 0;xx < (int)x_size;xx++){
			f = 0;
			for(y = -H;y < H;y++)
				for(x = -H;x < H;x++){
					int h;
					if( ((yy + y)< y_size) && ((yy + y) >= 0) && ((xx + x)< x_size) &&((xx + x)>= 0) )
						h=alt_buff[(yy + y)*x_size + (xx + x)];
					else h=0;
					f += filter_array[H + y][H + x]*double(h);
					}
			unsigned char c = round(f*norma_inv);
			new_alt_buff[((yy)*x_size) + (xx)] = round(f*norma_inv);
			}
	}
	//cout <<endl<<endl;
	memcpy(alt_buff,new_alt_buff,x_size*y_size*sizeof(unsigned short));
	delete new_alt_buff;
}
/////////////////////////////////
static void gaussFilter4LS(short * in_buff, short * out_buff, double filter_scaling, int x_size, int y_size)
{
	int border_mask_x=x_size-1;
	int border_mask_y=y_size-1;
	const int H = 4;//4
	double filter_array[2*H][2*H];
	int x,y;
	double f,norma = 0;
	double filter_scaling_inv_2 = sqr(1/filter_scaling);
	for(y = -H;y < H;y++)
		for(x = -H;x < H;x++){
			f = exp(-(sqr((double)x) + sqr((double)y))*filter_scaling_inv_2);
			norma += f;
			filter_array[H + y][H + x] = f;
			}
	double norma_inv = 1/norma;
	//cout << "Gaussian filter, factor: " << filter_scaling <<endl;

	//unsigned short* new_alt_buff = new unsigned short[x_size*y_size];

	int xx,yy;
	for(yy = 0;yy < (int)y_size;yy++){
		for(xx = 0;xx < (int)x_size;xx++){
			f = 0;
			for(y = -H;y < H;y++)
				for(x = -H;x < H;x++){
					int h;
					if( ((yy + y)< y_size) && ((yy + y) >= 0) && ((xx + x)< x_size) &&((xx + x)>= 0) )
						h=in_buff[(yy + y)*x_size + (xx + x)];
					else h=0;
					f += filter_array[H + y][H + x]*double(h);
					}
			unsigned char c = round(f*norma_inv);
			out_buff[((yy)*x_size) + (xx)] = round(f*norma_inv);
			}
	}
}


/////////////////////////////////
static void gaussFilter4LS(int begx, int begy,  int x_size, int y_size, short * mask, double filter_scaling)
{
	int border_mask_x=x_size-1;
	int border_mask_y=y_size-1;
	const int H = 4;//4
	double filter_array[2*H][2*H];
	int x,y;
	double f,norma = 0;
	double filter_scaling_inv_2 = sqr(1/filter_scaling);
	for(y = -H;y < H;y++)
		for(x = -H;x < H;x++){
			f = exp(-(sqr((double)x) + sqr((double)y))*filter_scaling_inv_2);
			norma += f;
			filter_array[H + y][H + x] = f;
			}
	double norma_inv = 1/norma;

	int xx,yy;
	for(yy = 0;yy < (int)y_size;yy++){
		for(xx = 0;xx < (int)x_size;xx++){
			f = 0;
			if( mask[yy*x_size + xx]!=0 ){
				for(y = -H;y < H;y++){
					for(x = -H;x < H;x++){
						int off=vMap.offsetBuf( vMap.XCYCL(xx+x+begx), vMap.YCYCL(yy+y+begy));
						//f += filter_array[H + y][H + x]*(double)vMap.GetAltDam(off);
						f += filter_array[H + y][H + x]*(double)vMap.SGetAlt(off);
					}
				}
				unsigned short v = round(f*norma_inv);
				int off=vMap.offsetBuf( vMap.XCYCL(xx+begx), vMap.YCYCL(yy+begy));
				if(vMap.VxDBuf[off]==0) vMap.SPutAltGeo(off, v);
				else vMap.SPutAltDam(off, v);
			}
		}
	}
}



static const int DELTA_BREAKAWAY=9;
static const int DELTA_BREAKAWAY2=9;
static const int DEEP_SCAN_BREAKAWAY=10;// в единицах сетки 4x4
static const int MIN_DELTAH_LANDSLIP=16;
struct mP{
	short x,y;
};
static const int COUNT_DIRECT_TSTBRK=8;
static const mP SDir[COUNT_DIRECT_TSTBRK]={+1,0, +1,-1, 0,-1, -1,-1, -1,0, -1,+1, 0,+1, +1,+1}; //восемь направлений против часовой стрелки
static const int KDir[COUNT_DIRECT_TSTBRK]={1<<16,0xB505,1<<16,0xB505,1<<16,0xB505,1<<16,0xB505};//B505 это коэф. 0.707
//возвращает направление (0-7)или -1
CLandslip* testBreakaway(int xg, int yg, int radLS)
{
	int i;
	short maxDH=0;
	unsigned char maxDir=(BYTE)-1;
	short startH=vMap.GVBuf[vMap.offsetGBuf(xg, yg)];
	for(i=0; i<COUNT_DIRECT_TSTBRK; i++){
		short h=vMap.GVBuf[vMap.offsetGBufC(xg+SDir[i].x, yg+SDir[i].y)];
		short cMaxDH=(int)abs(h-startH)*KDir[i]>>16;
		if( cMaxDH > maxDH) { maxDH=cMaxDH; maxDir=i; }
	}
	if(maxDH < DELTA_BREAKAWAY) return NULL;
	//сканирование по направлению с ограничением конусом на 90 градусов
	unsigned char begDir=maxDir;//сохранение первоначального направления для обратного скана
	short curH=startH;
	short DH=curH-vMap.GVBuf[vMap.offsetGBufC(xg+SDir[maxDir].x, yg+SDir[maxDir].y)];
	//char signDH;
	//if(DH>0) signDH=1; else signDH=-1;
	short curX=xg;
	short curY=yg;
	unsigned char direct=maxDir;
	direct=(direct-1)%COUNT_DIRECT_TSTBRK;
	for(i=0; i<DEEP_SCAN_BREAKAWAY; i++){
		short k;
		maxDH=0;
		for(k=0; k<3; k++){
			unsigned char d=(direct+k)%COUNT_DIRECT_TSTBRK;
			short dh=curH-vMap.GVBuf[vMap.offsetGBufC(curX+SDir[d].x, curY+SDir[d].y)];
			dh=(int)dh*KDir[d]>>16;
			if(DH>0){ if(dh>maxDH) { maxDH=dh; maxDir=d;} } //можно с оптимизить разложив на на 2 цикла
			else { if(dh<maxDH) { maxDH=dh; maxDir=d;} }
		}
		if(abs(maxDH) < DELTA_BREAKAWAY2) break;
		curX+=SDir[maxDir].x;
		curY+=SDir[maxDir].y;
		curH=vMap.GVBuf[vMap.offsetGBufC(curX, curY)];
	}
	int begLSX=vMap.XCYCLG(curX);
	int begLSY=vMap.YCYCLG(curY);
	//сканирование в обратном направлении с ограничением по конусу на 90 градусов
	maxDir=(begDir+COUNT_DIRECT_TSTBRK/2)%COUNT_DIRECT_TSTBRK;//поворот на 90
	curX=xg;
	curY=yg;
	direct=maxDir;
	direct=(direct-1)%COUNT_DIRECT_TSTBRK;
	for(i=0; i<DEEP_SCAN_BREAKAWAY; i++){
		short k;
		maxDH=0;
		for(k=0; k<3; k++){
			unsigned char d=(direct+k)%COUNT_DIRECT_TSTBRK;
			short dh=curH-vMap.GVBuf[vMap.offsetGBufC(curX+SDir[d].x, curY+SDir[d].y)];
			dh=(int)dh*KDir[d]>>16;
			if(DH>0){ if(dh>maxDH) {maxDH=dh; maxDir=d;} } //можно с оптимизить разложив на на 2 цикла
			else { if(dh<maxDH) {maxDH=dh; maxDir=d;} }
		}
		if(abs(maxDH) < DELTA_BREAKAWAY2) break;
		curX=curX+SDir[maxDir].x;
		curY=curY+SDir[maxDir].y;
		curH=vMap.GVBuf[vMap.offsetGBufC(curX, curY)];
	}
	int endLSX=vMap.XCYCLG(curX);
	int endLSY=vMap.YCYCLG(curY);
	short endH=vMap.GVBuf[vMap.offsetGBuf(endLSX, endLSY)];
	short begH=vMap.GVBuf[vMap.offsetGBuf(begLSX, begLSY)];
	if(abs(endH-begH)<MIN_DELTAH_LANDSLIP) return NULL;
	CLandslip* lsp;
	if(begH>endH){ //обвал идет с beg point
		float al=atan2f( endLSY-begLSY, endLSX-begLSX);
		lsp=new CLandslip((begLSX<<kmGrid)+2, (begLSY<<kmGrid)+2, radLS/*40*/, radLS/*40*/, endH<<VX_FRACTION, begH<<VX_FRACTION, (float)M_PI+al );
	}
	else{ //обвал идет с end point
		float al=atan2f( begLSY-endLSY, begLSX-endLSX);
		lsp=new CLandslip((endLSX<<kmGrid)+2, (endLSY<<kmGrid)+2, radLS/*40*/, radLS/*40*/, begH<<VX_FRACTION, endH<<VX_FRACTION, (float)M_PI+al );
	}
	return lsp;
}
CLandslip* landslipSearch(int beg_x, int beg_y, int r)
{
	int bx=beg_x>>kmGrid;
	int by=beg_y>>kmGrid;
	int x=0, y=0;
	int k;
	CLandslip* lsp;
	for(k=0; k<r; k++){
		for(x; x< k; x++) {
			lsp=testBreakaway(vMap.XCYCLG(bx+x),vMap.YCYCLG(by+y), r);
			if(lsp!=NULL) goto LSFound;
		}
		for(y; y< k; y++){
			lsp=testBreakaway(vMap.XCYCLG(bx+x),vMap.YCYCLG(by+y), r);
			if(lsp!=NULL) goto LSFound;
		}
		for(x; x>-k; x--){
			lsp=testBreakaway(vMap.XCYCLG(bx+x),vMap.YCYCLG(by+y), r);
			if(lsp!=NULL) goto LSFound;
		}
		for(y; y>-k; y--){
			lsp=testBreakaway(vMap.XCYCLG(bx+x),vMap.YCYCLG(by+y), r);
			if(lsp!=NULL) goto LSFound;
		}
	}
	return NULL;
LSFound:
	return lsp;
}

const float KH_LANDSLIP=0.5f; //0-нет осыпания
const float BEGIN_SPEED_LANDSLIP=1.f;//0.f;//=1.f;//1.f;
const float ACCELERATION_LANDSLIP=0;//0.3f;//0

const int THRESHOLD=(1<<VX_FRACTION)+ (1<<(VX_FRACTION-1));


CLandslip::CLandslip(int _x, int _y, int _sx, int _sy, int _hmin, int _hmax, float _direction)
: rbmp(_direction, _sx, _sy)
{
	x=_x; y=_y;
	sx=_sx; sy=_sy;
	hmin=_hmin; hmax=_hmax;
	deltaHLS=round(KH_LANDSLIP*(hmax-hmin));
	hminLS=hmax-deltaHLS;
	direction=_direction;//_direction*M_PI/180.0f;
	speed=BEGIN_SPEED_LANDSLIP;
	cntQuant=0;
	xPrec=x<<16; yPrec=y<<16;
	deltaVx=new unsigned short[sx*sy];
	surVx=new short[sx*sy];
	surVxB=new short[sx*sy];
	tmpltGeo=new int[sx*sy];
	prepTmplt();
	unsigned char* geo=vMap.VxGBuf;
	unsigned char* dam=vMap.VxDBuf;
	unsigned char* atr=vMap.AtrBuf;
	unsigned char* sur=vMap.SurBuf;
	int i,j,cnt=0;
	int vmin=MAX_VX_HEIGHT;
	int vmax=0;
	for(i=0; i<sy; i++){
		rbmp.setStr(i);
		for(j=0; j<sx; j++){
			int off=vMap.offsetBuf(vMap.XCYCL((rbmp.getX(j)+xPrec)>>16), vMap.YCYCL((rbmp.getY(j)+yPrec)>>16));
			int v;
			if(*(dam+off)==0) v= *(geo+off) <<VX_FRACTION;
			else v= *(dam+off) <<VX_FRACTION;
			v|=*(atr+off) &VX_FRACTION_MASK;
			float xx=(float)rbmp.getX(j)/(sx/2)/(1<<16);
			float yy=(float)rbmp.getY(j)/(sy/2)/(1<<16);

//			if(j<sx/2) { xx=0; yy=(float)rbmp.getY(sx/2)/(sy/2)/(1<<16);}
//			float f=exp(-(fabsf(xx*xx*xx*xx)+fabsf(yy*yy*yy*yy))/(0.4*0.4));
			float f=(float)tmpltGeo[cnt]/(float)(1<<16);
			//int curV=hmax-(deltaHLS*tmpltGeo[cnt]>>16);
			int dh=round(deltaHLS*f);
			surVx[cnt]=-1; //Обнуление
			if(dh>0){
				int curV=hmax-round(deltaHLS*f);
				if(curV<0)curV=0;
				if(v>curV) {
					*(deltaVx+cnt)=v-curV ;
					//if((v-curV)>(1<<VX_FRACTION))
					surVx[cnt]=sur[off];
					if(*(dam+off)!=0) surVx[cnt]|=256;//2^8
				}
				else *(deltaVx+cnt)=0;
			}
			else *(deltaVx+cnt)=0;
//			deltaVx[cnt]=((20<<VX_FRACTION))*f;//+XRnd(1<<VX_FRACTION) //показ формы ковша
			if( v < vmin) vmin=v;
			if( v > vmax) vmax=v;
			cnt++;
		}
	}
	//for(j=0; j<sx; j++)*(deltaVx+j)=-30<<VX_FRACTION; //показательная полоса
	//if((vmax-vmin)>DELTA_NOT_DEEP) h_begin=vmin;
	//else h_begin=vmin-BEGIN_DEEP;
}

CLandslip::~CLandslip()
{
	delete [] deltaVx;
	delete [] surVxB;
	delete [] surVx;
	delete [] tmpltGeo;
	sx=sy=0;
}

void CLandslip::prepTmplt(void)
{
	int i,j,cnt=0;
	float sy05=sy/2.0f;
	float sx05=sx/2.0f;
	for(i=0; i<sy; i++){
		float y=(float)(i-sy05)/(float)sy05;
		for(j=0; j<sx; j++){
			float x;
			if(j<sx/2) x=0;
			else x=(float)(j-sx05)/(float)sx05; 
			float f=exp(-(fabsf(x*x*x)+fabsf(y*y*y))/(0.3*0.3));
			if(j<sx/2) {
				f=f+0.5f*(float)(sx/2-j)/(sx/2);
			}
			tmpltGeo[cnt]=round(f*(float)(1<<16));//(1<<16)+XRnd(1<<14);
			cnt++;
		}
	}
}

int CLandslip::geoQuant(void)
{
//Отладка вращающегося битмапа постоянной высоты
/*	static float al=0;//M_PI/4.0;//+M_PI/18.0;
	srBmp rbmp(al, sx, sy);
	al=al+M_PI/18.0;
	unsigned char* geo=vMap.VxGBuf;
	unsigned char* dam=vMap.VxDBuf;
	unsigned char* atr=vMap.AtrBuf;
	int i,j,cnt=0;
	for(i=-2*sy; i<2*sy; i++){
		int offy=vMap.offsetBuf(0, vMap.YCYCL(y+i));
		for(j=-2*sx; j<2*sx; j++){
			int off=offy+vMap.XCYCL(x+j);
			int v=70<<VX_FRACTION;
			*(geo+off)=v>>VX_FRACTION;
			*(atr+off)= (*(atr+off)&(~VX_FRACTION_MASK)) | (v&VX_FRACTION_MASK);
		}
	}
	cnt=0;
//	int bsx, bsy;
	for(i=0; i<sy; i++){
//		if(rbmp.stpX==(1<<16)||rbmp.stpX==(-1<<16)){
//			bsx=-rbmp.stpX*round(i*cos(rbmp.alpha)*sin(rbmp.alpha));
//			bsy=(rbmp.stpX*i)-rbmp.stpY*round(i*cos(rbmp.alpha)*sin(rbmp.alpha));
//		}
//		else {
//			bsx=(-rbmp.stpY*i)+rbmp.stpX*round(i*sin(rbmp.alpha)*cos(rbmp.alpha));
//			bsy=rbmp.stpY*round(i*sin(rbmp.alpha)*cos(rbmp.alpha));
//		}
		rbmp.setStr(i);
		for(j=0; j<sx; j++){
//			int cx=(bsx+j*rbmp.stpX) >>16;
//			int cy=(bsy+j*rbmp.stpY) >>16;
//			int off=vMap.offsetBuf(vMap.XCYCL(rbmp.begShiftX + x+cx), vMap.YCYCL(rbmp.begShiftY + y+cy));
			int off=vMap.offsetBuf(vMap.XCYCL(rbmp.getX(j)+ x), vMap.YCYCL(rbmp.getY(j)+ y));
			int v;
			if(*(dam+off)==0) v= *(geo+off) <<VX_FRACTION;
			else v= *(dam+off) <<VX_FRACTION;
			v|=*(atr+off) &VX_FRACTION_MASK;
			v+=10<<VX_FRACTION;
			if(j==sx-1 && i==0) v=200<<VX_FRACTION;
			if(j==sx/2 && i==sy/2) v=200<<VX_FRACTION;
			//*(inVx+cnt)=v;
			//fullVx[cnt]=v;
			//if(atrVx[cnt]!=2) v-=(int)(*(deltaVx+cnt));
			*(geo+off)=v>>VX_FRACTION;
			*(atr+off)= (*(atr+off)&(~VX_FRACTION_MASK)) | (v&VX_FRACTION_MASK);
			cnt++;
		}
	}
	vMap.regRender(vMap.XCYCL(x-200), vMap.YCYCL(y-200), vMap.XCYCL(x + sx+100), vMap.YCYCL(y + sy+100) );
*/

	//Обработка имиджа в буфере
//	gaussFilter4LS(deltaVx, 1.4, sx, sy);
//	gaussFilter4LS(deltaVx, 0.5, sx, sy);

	int xoldPrec=xPrec, yoldPrec=yPrec;

	srBmp rbmpT(direction , sx, sy);//+ (M_PI/180)*(2-XRnd(4))
//расчет максимального битмапа
	rbmpT.setStr(0);
	int x1=abs(rbmpT.getX(0));
	int y1=abs(rbmpT.getY(0));
	int x2=abs(rbmpT.getX(sx-1));
	int y2=abs(rbmpT.getY(sy-1));
	if(x1 < x2) x1=x2;
	if(y1 < y2) y1=y2;
	int bsx=((x1>>16)+8)*2;//8 - граница
	int bsy=((y1>>16)+8)*2;//8 - граница
	int bsx05=bsx>>1; int bsy05=bsy>>1;
	int shiftBX=xPrec>>16;
	int shiftBY=yPrec>>16;
	int bxoldPrec=(bsx05<<16)+xoldPrec-(shiftBX<<16);
	int byoldPrec=(bsy05<<16)+yoldPrec-(shiftBY<<16);
	int bufSize=bsx*bsy;
	short* buf=new short[bufSize];
	short* outBuf=new short[bufSize];
	for(int k=0; k<bufSize; k++) { buf[k]=0; } //outBuf[k]=0; }

	unsigned char* geo=vMap.VxGBuf;
	unsigned char* dam=vMap.VxDBuf;
	unsigned char* atr=vMap.AtrBuf;
	unsigned char* sur=vMap.SurBuf;

/*
//Показ инструмента
	int i,j,cnt=0;
	for(i=0; i<sy; i++){
		rbmpT.setStr(i);
		for(j=0; j<sx; j++){
			int off=vMap.offsetBuf(vMap.XCYCL((rbmpT.getX(j)+xPrec)>>16), vMap.YCYCL((rbmpT.getY(j)+yPrec)>>16));
			int offb=((rbmpT.getX(j)+bxoldPrec)>>16)+ ((rbmpT.getY(j)+byoldPrec)>>16)*bsx;
			int v;
			if(*(dam+off)==0) v= *(geo+off) <<VX_FRACTION;
			else v= *(dam+off) <<VX_FRACTION;
			v|=*(atr+off) &VX_FRACTION_MASK;
			//*(inVx+cnt)=v;
			fullVx[cnt]=v;
			v+=(int)deltaVx[cnt];
			buf[offb]+=deltaVx[cnt];
			if(v<0)v=0;
			*(geo+off)=v>>VX_FRACTION;
			*(atr+off)= (*(atr+off)&(~VX_FRACTION_MASK)) | (v&VX_FRACTION_MASK);
			cnt++;
		}
	}
*/
	int i,j,cnt=0;
	for(i=0; i<sy; i++){
		rbmpT.setStr(i);
		for(j=0; j<sx; j++){
			int off=vMap.offsetBuf(vMap.XCYCL((rbmpT.getX(j)+xPrec)>>16), vMap.YCYCL((rbmpT.getY(j)+yPrec)>>16));
			int offb=((rbmpT.getX(j)+bxoldPrec)>>16)+ ((rbmpT.getY(j)+byoldPrec)>>16)*bsx;
//			if( ((rbmpT.getX(j)+bxoldPrec)>>16)>=bsx || ((rbmpT.getY(j)+byoldPrec)>>16)>=bsy 
//				|| ((rbmpT.getX(j)+bxoldPrec)>>16)<0 || ((rbmpT.getY(j)+byoldPrec)>>16)<0 ){
//				int a=5;
//			}
			int v;
			if(*(dam+off)==0) v= *(geo+off) <<VX_FRACTION;
			else v= *(dam+off) <<VX_FRACTION;
			v|=*(atr+off) &VX_FRACTION_MASK;

			v-=(int)deltaVx[cnt];
			buf[offb]-=deltaVx[cnt];
			outBuf[offb]=1;

			if(v<0)v=0;
			if(dam[off]!=0) dam[off]=v>>VX_FRACTION;
			else *(geo+off)=v>>VX_FRACTION;
			*(atr+off)= (*(atr+off)&(~VX_FRACTION_MASK)) | (v&VX_FRACTION_MASK);
			cnt++;
		}
	}

	xPrec=xPrec+ -rbmpT.stpX; //speed*cos(M_PI+direction);
	yPrec=yPrec+ -rbmpT.stpY; //speed*sin(M_PI+direction);
	x=vMap.XCYCL(xPrec>>16);//vMap.XCYCL(round(xPrec));
	y=vMap.YCYCL(yPrec>>16);//vMap.YCYCL(round(yPrec));

	int bxPrec=bxoldPrec + -rbmpT.stpX;
	int byPrec=byoldPrec + -rbmpT.stpY;

	//Обработка имиджа в буфере
//	gaussFilter4LS(deltaVx, 1.4, sx, sy);
//	gaussFilter4LS(deltaVx, 0.5, sx, sy);
	
	cnt=0;
	unsigned char flad_driving=0;
	for(i=0; i<sy; i++){
		//int newOffy=vMap.offsetBuf(0, vMap.YCYCL(y+i));
		//int oldOffy=vMap.offsetBuf(0, vMap.YCYCL(yold+i));
		rbmpT.setStr(i);
		for(j=0; j<sx; j++){
			//int newOff=newOffy+vMap.XCYCL(x+j);
			//int oldOff=oldOffy+vMap.XCYCL(xold+j);
			int newOff=vMap.offsetBuf(vMap.XCYCL((rbmpT.getX(j)+xPrec)>>16), vMap.YCYCL((rbmpT.getY(j)+yPrec)>>16));
			int oldOff=vMap.offsetBuf(vMap.XCYCL((rbmpT.getX(j)+xoldPrec)>>16), vMap.YCYCL((rbmpT.getY(j)+yoldPrec)>>16));
			int oldOffB=((rbmpT.getX(j)+bxoldPrec)>>16)+ ((rbmpT.getY(j)+byoldPrec)>>16)*bsx;
			int newOffB=((rbmpT.getX(j)+bxPrec)>>16)+ ((rbmpT.getY(j)+byPrec)>>16)*bsx;

//			if( ((rbmpT.getX(j)+bxoldPrec)>>16)>=bsx || ((rbmpT.getY(j)+byoldPrec)>>16)>=bsy 
//				|| ((rbmpT.getX(j)+bxoldPrec)>>16)<0 || ((rbmpT.getY(j)+byoldPrec)>>16)<0 ){
//				int a=5;
//			}
//			if( ((rbmpT.getX(j)+bxPrec)>>16)>=bsx || ((rbmpT.getY(j)+byPrec)>>16)>=bsy 
//				|| ((rbmpT.getX(j)+bxPrec)>>16)<0 || ((rbmpT.getY(j)+byPrec)>>16)<0 ){
//				int a=5;
//			}
			int newV;
			if( dam[newOff]==0) newV= geo[newOff] <<VX_FRACTION;
			else newV= dam[newOff] <<VX_FRACTION;
			newV|= atr[newOff] &VX_FRACTION_MASK;
			int oldV;
			if( dam[oldOff]==0) oldV= geo[oldOff] <<VX_FRACTION;
			else oldV= dam[oldOff] <<VX_FRACTION;
			oldV|= atr[oldOff] &VX_FRACTION_MASK;

//			if(newV >= fullVx[cnt]){ 
//				//остановка по препятствию
//				oldV+=deltaVx[cnt];
//				buf[oldOffB]+=deltaVx[cnt];
//				deltaVx[cnt]=0;
//				geo[oldOff]=oldV>>VX_FRACTION;
//				atr[oldOff]= (atr[oldOff]&(~VX_FRACTION_MASK)) | (oldV&VX_FRACTION_MASK);
//				if(surVx[cnt]!=-1) {
//					sur[oldOff]=surVx[cnt];
//					surVx[cnt]=-1;
//				}
//			}
//			else {
				int threshold=XRnd(1<<VX_FRACTION);//XRnd(2<<VX_FRACTION);//THRESHOLD;//XRnd(1<<VX_FRACTION);//THRESHOLD+
				int dv=oldV-newV;
				if(dv>threshold ){
					newV+=deltaVx[cnt];
					buf[newOffB]+=deltaVx[cnt];
					if(surVx[cnt]>255) dam[newOff]=newV>>VX_FRACTION;
					else geo[newOff]=newV>>VX_FRACTION;
					atr[newOff]= (atr[newOff]&(~VX_FRACTION_MASK)) | (newV&VX_FRACTION_MASK);
					if(surVx[cnt]!=-1) {
//						sur[newOff]=surVx[cnt];
						outBuf[newOffB]=1;
					}
					flad_driving|=1;
				}
				else {
					int idv=threshold-dv;//deltaVx[cnt]>>1;//
					if(deltaVx[cnt]>idv) {
						deltaVx[cnt]-=idv;
						newV+= deltaVx[cnt];
						buf[newOffB]+=deltaVx[cnt];

						if(surVx[cnt]>255) dam[newOff]=newV>>VX_FRACTION;
						else geo[newOff]=newV>>VX_FRACTION;
						atr[newOff]= (atr[newOff]&(~VX_FRACTION_MASK)) | (newV&VX_FRACTION_MASK);
						oldV+=idv;//>>1;
						buf[oldOffB]+=idv;
						//oldVNL+=idv>>1;
						if(surVx[cnt]!=-1){
//							sur[oldOff]=surVx[cnt];
//							sur[newOff]=surVx[cnt];
							outBuf[newOffB]=1;
							outBuf[oldOffB]=1;
						}
					}
					else {
						oldV+=deltaVx[cnt]; 
						buf[oldOffB]+=deltaVx[cnt];
						deltaVx[cnt]=0;
						if(surVx[cnt]!=-1) {
							sur[oldOff]=surVx[cnt];
							surVx[cnt]=-1;
							outBuf[oldOffB]=1;
						}
					}
					if(surVx[cnt]>255) dam[oldOff]=oldV>>VX_FRACTION;
					else geo[oldOff]=oldV>>VX_FRACTION;
					atr[oldOff]= (atr[oldOff]&(~VX_FRACTION_MASK)) | (oldV&VX_FRACTION_MASK);
					
					//geo[newNLOff]=oldVNL>>VX_FRACTION;
					//atr[newNLOff]= (atr[newNLOff]&(~VX_FRACTION_MASK)) | (oldVNL&VX_FRACTION_MASK);
				}

//			}
			cnt++;
		}
	}


	gaussFilter4LS(shiftBX-bsx05, shiftBY-bsy05,  bsx, bsy, buf, 1.4);
//	gaussFilter4LS(shiftBX-bsx05, shiftBY-bsy05,  bsx, bsy, outBuf, 2.0);

/*	gaussFilter4LS(buf, outBuf, 2.0, bsx, bsy);//1.5

	//сброс расхождений после смуса на карту
	cnt=0;
	for(i=0; i<bsy; i++){
		for(j=0; j<bsx; j++){
			int off=vMap.offsetBuf(vMap.XCYCL(shiftBX-bsx05+j), vMap.YCYCL(shiftBY-bsy05+i));
			int v;
			if(*(dam+off)==0) v= *(geo+off) <<VX_FRACTION;
			else v= *(dam+off) <<VX_FRACTION;
			v|=*(atr+off) &VX_FRACTION_MASK;
			int dsm=outBuf[cnt]-buf[cnt];
			v+=dsm;
			if( abs(dsm)< ((1<<VX_FRACTION)+(1<<(VX_FRACTION-1)) ) ){
				if(dam[off]!=0) *(dam+off)=v>>VX_FRACTION;
				else *(geo+off)=v>>VX_FRACTION;
				*(atr+off)= (*(atr+off)&(~VX_FRACTION_MASK)) | (v&VX_FRACTION_MASK);
			}
			cnt++;
		}
	}*/
	delete [] outBuf;
	delete [] buf;


	return flad_driving;
}
int CLandslip::quant(void)
{


	unsigned char flad_driving=0;
	int oldX=x;
	int oldY=y;
//	geoQuant();
	speed+=ACCELERATION_LANDSLIP;
	cntQuant++;
	for(int i=0; i<cntQuant; i++) flad_driving=geoQuant();
	int nx=calcDX(oldX, x);
	int ny=calcDY(oldY, y);
	int begx, begy;
	if(nx>0) { begx=oldX; }
	else { begx=x; nx=-nx; }
	if(ny>0) { begy=oldY; }
	else { begy=y; ny=-ny; }

	//нахождение реальных размеров измененной области
	rbmp.setStr(0);
	int x1=abs(rbmp.getX(0));
	int y1=abs(rbmp.getY(0));
	int x2=abs(rbmp.getX(sx-1));
	int y2=abs(rbmp.getY(sy-1));
	if(x1 < x2) x1=x2;
	if(y1 < y2) y1=y2;
	int bsx=((x1>>16)+2)*2;//8 - граница
	int bsy=((y1>>16)+2)*2;//8 - граница
	int bsx05=bsx>>1; int bsy05=bsy>>1;

	vMap.recalcArea2Grid( vMap.XCYCL(begx-bsx05), vMap.YCYCL(begy-bsy05), vMap.XCYCL(begx+bsx05+nx), vMap.YCYCL(begy+bsy05+ny) );
	vMap.regRender( vMap.XCYCL(begx-bsx05), vMap.YCYCL(begy-bsy05), vMap.XCYCL(begx+bsx05+nx), vMap.YCYCL(begy+bsy05+ny) );

	//vMap.recalcArea2Grid(vMap.XCYCL(x-10), vMap.YCYCL(y-10), vMap.XCYCL(x + sx +10), vMap.YCYCL(y + sy +10) );
	//vMap.regRender(vMap.XCYCL(x-100), vMap.YCYCL(y-100), vMap.XCYCL(x + sx+100), vMap.YCYCL(y + sy+100) );

	return flad_driving;
}

/////////////////////////////////////////////////////////
//		Volcano
/////////////////////////////////////////////////////////
const int BEGIN_VOLCANO_DEEP = 10<<VX_FRACTION;
const int DELTA_VOLCANO_NOT_DEEP = 20<<VX_FRACTION;
static const int tv_arraySX = 128;//256;
static const int tv_arraySY = 128;//256;
static const int tv_keyPoints = 2;
static const short tv_keyPointsTime[tv_keyPoints]={20, 15};//{10,15};//20,15

sTVolcano::~sTVolcano()
{
	delete [] array;
	delete [] inVx;
}

static const int VOLCANO_CENTER_X=121;
static const int VOLCANO_CENTER_Y=123;
sTVolcano::sTVolcano(int _x, int _y, int _sx, int _sy)
{
	array = new unsigned short[tv_arraySX*tv_arraySY*tv_keyPoints];

	iKScaling = 1<<16;
	currentKP = 0;
	frame = 0;
	currentKPFrame = 0;
	pPrevKP = 0; 
	pCurKP = array;

	kmNewVx = 1;

	x = _x-VOLCANO_CENTER_X/2; 
	y = _y-VOLCANO_CENTER_Y/2; 
	sx = _sx; 
	sy = _sy;
	inVx = new unsigned short[sx*sy];
	unsigned char* geo=vMap.VxGBuf;
	unsigned char* dam=vMap.VxDBuf;
	unsigned char* atr=vMap.AtrBuf;
	int cnt = 0;
	int vmin = MAX_VX_HEIGHT;
	int vmax = 0;
	int i;
	for(i=0; i<sy; i++){
		int offy=vMap.offsetBuf(0, vMap.YCYCL(y+i));
		for(int j=0; j<sx; j++){
			int off = offy+vMap.XCYCL(x+j);
			int v = *(dam+off) == 0 ? *(geo + off) << VX_FRACTION : *(dam + off) << VX_FRACTION;
			v |= *(atr+off) & VX_FRACTION_MASK;
			inVx[cnt] = v;
			if(v < vmin) 
				vmin=v;
			if(v > vmax) 
				vmax=v;
			cnt++;
		}
	}

	h_begin = (vmax-vmin) > DELTA_VOLCANO_NOT_DEEP ? vmin : vmin - BEGIN_VOLCANO_DEEP;
	vMin = vmin;

//	int kScl;
//	if(h_begin > MAX_VX_HEIGHT/3) kScl=((MAX_VX_HEIGHT-MAX_VX_HEIGHT*2/3)<<16)/(MAX_VX_HEIGHT);
//	else kScl=((MAX_VX_HEIGHT-h_begin)<<16)/(MAX_VX_HEIGHT);


	const int kFirstScale = round((0.55f+frnd(0.15f))*(float)(1<<16));
	max_template_volcano_height = 0;
	XBuffer tfb;
	tfb < Path2TTerraResource < "volcano.dat";
	XStream fo(tfb, XS_IN);
	unsigned short buf[256*2];
	cnt = 0;
	for(i = 0; i < tv_arraySY*tv_keyPoints; i++){
		//fo.read(array+i*tv_arraySX, tv_arraySX*sizeof(unsigned short));
		fo.read(buf, 256*sizeof(unsigned short)*2);
		for(int j = 0; j < tv_arraySX; j++) {
			//array[cnt]=(buf[j*2]*kScl)>>16;
			array[cnt]=((int)buf[j*2]*kFirstScale) >> 16;
			if(max_template_volcano_height < array[cnt]) 
				max_template_volcano_height = array[cnt];
			//array[cnt]=array[cnt]>>1 - (5<<VX_FRACTION);
			//if(array[cnt]<0)array[cnt]=0;
			cnt++;
		}
	}

	int kScl=1<<16;
	//if(h_begin > MAX_VX_HEIGHT/3) kScl=((MAX_VX_HEIGHT-MAX_VX_HEIGHT*2/3)<<16)/(MAX_VX_HEIGHT);
	//else kScl=((MAX_VX_HEIGHT-h_begin)<<16)/(MAX_VX_HEIGHT);
	if((MAX_VX_HEIGHT-h_begin) < max_template_volcano_height ){
		kScl=(1<<16)*(MAX_VX_HEIGHT-h_begin)/max_template_volcano_height;
	}

	cnt = 0;
	for(i = 0; i < tv_arraySY*tv_keyPoints; i++){
		for(int j = 0; j < tv_arraySX; j++) {
			array[cnt]=((int)array[cnt]*kScl)>>16;
			cnt++;
		}
	}
}

const unsigned int T_VOLCANO_DAMAGE_RADIUS=64;
int sTVolcano::quant()
{
	int curKScale = ((currentKPFrame+1)<<16)/tv_keyPointsTime[currentKP];
	int prevKScale = (1<<16) - curKScale;
	const int delta_kmNewVx = 5;
	kmNewVx += delta_kmNewVx;
	int xxx = 0;//x%d;
	int yyy = 0;//y%d;
	int cnt = 0;
	for(int i = 0; i < sy; i++){
		for(int j = 0; j < sx; j++){
			int Vold = inVx[cnt];
			//int V=array[i*tv_arraySX+j]*kmNewVx/256;
			int prevH = pPrevKP != 0 ? prevKScale*pPrevKP[i*tv_arraySX+j] : 0;
			int V = (prevH + pCurKP[i*tv_arraySX+j]*curKScale) >>(16) ;
			
			int offset = vMap.offsetBuf((x+j) & vMap.clip_mask_x, (y+i) & vMap.clip_mask_y);
			V += h_begin;

			static char fl = 0;
			static int k_dh[256];
			if(fl==0){
				fl=1;
				for(int m=0; m < 256; m++){
					float x = (float)m/128;//Диапазон от 0 до 2
					k_dh[m] = round((-0.1f+exp(-fabsf((x-1)*(x-1)*(x-1))/(0.4f*0.4f)))*(1<<16));
				}
			}

			if(Vold > V){
				//int inx=(Vold-V)>>3;
				int inx = (Vold - V) >> 2;//>>3;
				if(inx > 255)
					inx = 255;
				//V=Vold + ((V-h_begin)*k_dh[inx]>>(16+1) );
				int dV = V - h_begin; 
				if(dV > (10 << VX_FRACTION))
					dV = 10 << VX_FRACTION;

				V = Vold + (dV*k_dh[inx] >> 16);
				if(V > MAX_VX_HEIGHT)
					V = MAX_VX_HEIGHT;
				if(V < 0) 
					V = 0;
			}
			else {
				if(vMap.VxDBuf[offset]!=0){
					vMap.VxDBuf[offset]=0;
					vMap.SurBuf[offset]=vMap.GetGeoType(offset,V);
				}
			}

			//для нормальной границы при  разрушении зеропласта
			if((vMap.VxDBuf[offset] == 0 && vMap.VxGBuf[offset] == (V >> VX_FRACTION)) || (vMap.VxDBuf[offset] == (V >> VX_FRACTION))){ 
				vMap.AtrBuf[offset] = (V & VX_FRACTION_MASK) | (vMap.AtrBuf[offset] &= ~VX_FRACTION_MASK);
			}
			else {
				if(vMap.VxDBuf[offset]==0) 
					vMap.VxGBuf[offset] = V >> VX_FRACTION;
				else 
					vMap.VxDBuf[offset] = V >> VX_FRACTION;
				//нужно сохранить только признак тени, признак At_ZEROPLASTPRESENT при любом изменении удаляется
				vMap.AtrBuf[offset] = (V & VX_FRACTION_MASK) | (vMap.AtrBuf[offset] &= ~(VX_FRACTION_MASK|At_NOTPURESURFACE));
				//if(vMap.VxDBuf[offset]< V >>VX_FRACTION)vMap.VxDBuf[offset]=0;
			}
			cnt++;
		}
	}
	damagingBuildingsTolzer(vMap.XCYCL(x+(sx>>1)), vMap.YCYCL(y+(sy>>1)), T_VOLCANO_DAMAGE_RADIUS);
	//if(h_begin < vMin) h_begin=h_begin + (1<<VX_FRACTION);
	vMap.recalcArea2Grid(vMap.XCYCL(x-1), vMap.YCYCL(y-1), vMap.XCYCL(x + sx+1), vMap.YCYCL(y + sy+1) );
	vMap.regRender(x, y, x+sx, y+sy);

	frame++; currentKPFrame++;
	if(currentKPFrame >= tv_keyPointsTime[currentKP] ) {
		currentKP++;
		currentKPFrame = 0;
		pPrevKP = pCurKP;
		pCurKP += tv_arraySX*tv_arraySY;
		if(currentKP >= tv_keyPoints) {
			currentKP = 0;
			pPrevKP = 0; 
			pCurKP = array;
			return 0;
		}
	}
	return 1;
}

//void volcano(int x, int y)
//{
//	static sTVolcano v(x,y,128,128);
//	v.quant();
//}

//////////////////////////////////////////////////////////////////////////////
const int analyze_terrain_upper_dz_max=16<<VX_FRACTION;
const int analyze_terrain_lower_dz_min=-16<<VX_FRACTION;
static void analyzeTerrain(Vect3f& position, float radius, Vect3f& outOrientation)
{
	int D = round(radius);// >> kmGrid;
	if(!D) D = 1;
	int x0 = round(position.x);// >> kmGrid;
	int y0 = round(position.y);// >> kmGrid;
	int z0 = round(position.z*(1<<VX_FRACTION));

	int Sz = 0, Sxz = 0, Syz = 0;
	for(int y = -D; y <= D; y++)
		for(int x = -D; x <= D; x++){
			//int z = vMap.GVBuf[vMap.offsetGBufC(x + x0, y + y0)];
			int z = vMap.SGetAlt(vMap.XCYCL(x + x0), vMap.YCYCL(y + y0));
			int dz = z - z0;
			if(dz > analyze_terrain_upper_dz_max)
				z = z0 + analyze_terrain_upper_dz_max;
			else{
				if(dz < analyze_terrain_lower_dz_min)
					z = z0 + analyze_terrain_lower_dz_min;
				}

			Sz += z;
			Sxz += x*z;
			Syz += y*z;
			}

	float norm=1.f/(float)(1<<VX_FRACTION);
	float t3 = D*D;
	float t6 = 4.0*t3;
	float t5 = 3.0/(1.0+8.0*t3+(5.0+t6)*D)/D;
	float Z = Sz*norm/(t6+4.0*D+1.0);
	float A = Sxz*norm*t5;
	float B = Syz*norm*t5;

	Vect3f z_axis(A, B, 1 );//<< kmGrid
	//z_axis.Normalize();

	position.z = Z;
	outOrientation=z_axis;
}
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
//RandomGenerator myRnd;
const int BEGIN_BUBBLE_DEEP=0<<VX_FRACTION;
const int DELTA_BUBBLE_NOT_DEEP=0<<VX_FRACTION;
static const int tb_arraySX=128;//256;
static const int tb_arraySY=128;//256;
static const int tb_keyPoints=2;
static const short tb_keyPointsTime[tb_keyPoints]={4,2};//5,2//20,15

unsigned short* sTBubble::preImage=0;
int sTBubble::numPreImage=0;

sTBubble::sTBubble(int _x, int _y, int _sx, int _sy, bool _flag_occurrenceGeo)
{
	if(preImage==0){
		const int NUMBER_PRE_IMAGE=2;
		numPreImage=NUMBER_PRE_IMAGE;
		preImage=new unsigned short[tb_arraySX*tb_arraySY*tb_keyPoints*numPreImage];

		char cb[MAX_PATH];
		GetCurrentDirectory(MAX_PATH, cb);
		strcat(cb, "\\"); strcat(cb, Path2TTerraResource); strcat(cb, "bub.dat");//"\\RESOURCE\\Tools\\bub.dat"
		XStream fi;
		fi.open(cb, XS_IN);
		fi.seek(0,XS_BEG);
		//unsigned short buf[][256*2];
		fi.read(preImage, tb_keyPoints * tb_arraySY*tb_arraySX*sizeof(unsigned short)*numPreImage);
		fi.close();
	};

	x=_x-_sx/2; y=_y-_sy/2; sx=_sx; sy=_sy;
	flag_sleep=0;//_flag_sleep;
	flag_occurrenceGeo=_flag_occurrenceGeo;

	array=new short[sx*sy*tb_keyPoints];
	mask=new unsigned char[sx*sy];
	maxVx=new unsigned char[sx*sy];

	previsionKP=-1;
	currentKP=0;
	frame=0;
	currentKPFrame=0;
	pPrevKP=0; 
	pCurKP=array;

	inVx=new short[sx*sy];
	tmpVx=new short[sx*sy];
	substare=new short[sx*sy];
	unsigned char* geo=vMap.VxGBuf;
	unsigned char* dam=vMap.VxDBuf;
	unsigned char* atr=vMap.AtrBuf;
	int i,j,cnt=0;
	int vmin=MAX_VX_HEIGHT;
	int vmax=0;
	for(i=0; i<sy; i++){
		int offy=vMap.offsetBuf(0, vMap.YCYCL(y+i));
		for(j=0; j<sx; j++){
			int off=offy+vMap.XCYCL(x+j);
			int v;
			if(*(dam+off)==0) 
				v = *(geo+off) <<VX_FRACTION;
			else 
				v = *(dam+off) <<VX_FRACTION;
			v |= *(atr+off) & VX_FRACTION_MASK;
			*(inVx+cnt)=v;
			*(tmpVx+cnt)=v;
			if( v < vmin) vmin=v;
			if( v > vmax) vmax=v;
			mask[cnt]=0;
			maxVx[cnt]=0;
			cnt++;
		}
	}
	//if((vmax-vmin)>DELTA_BUBBLE_NOT_DEEP) h_begin=vmin;
	//else h_begin=vmin-BEGIN_BUBBLE_DEEP;
	h_begin=vmin+(vmax-vmin)/3;//+(vmax-vmin)/4;
	vMin=vmin;
	h_begin=(vmin+vmax)/2;

/*		//int kScl=((MAX_VX_HEIGHT-h_begin)<<16)/(MAX_VX_HEIGHT);
	int kScl=round(1.5f*(float)(1<<16));
	XStream fo;
	fo.open("bub.dat", XS_IN);
	fo.seek(0,XS_BEG);
	unsigned short buf[256*2];
	cnt=0;
	for(i = 0; i < tb_arraySY*tb_keyPoints; i++){
		//fo.read(array+i*tb_arraySX, tb_arraySX*sizeof(unsigned short));
		//fo.read(buf, 256*sizeof(unsigned short)*2);
		fo.read(buf, tb_arraySX*sizeof(unsigned short));
		for(j = 0; j < tb_arraySX; j++) {
			//array[cnt]=(buf[j*2]*kScl)>>16;
			array[cnt]=(buf[j]*kScl)>>16;
			//array[cnt]=array[cnt]>>1 - (5<<VX_FRACTION);
			//if(array[cnt]<0)array[cnt]=0;
			cnt++;
		}
	}*/
	
	Vect3f position(x, y, (float)h_begin/(float)(1<<VX_FRACTION));
	Vect3f outOrientation;
	analyzeTerrain(position, sx/2, outOrientation);
	//h_begin=round(position.z*(1<<VX_FRACTION));
	float A=outOrientation.x;
	float B=outOrientation.y;
	float C=-outOrientation.z;
	float D=(float)h_begin/(float)(1<<VX_FRACTION);//position.z;

	unsigned short* curPreImage=preImage+tb_arraySX*tb_arraySY*tb_keyPoints*XRnd(numPreImage);
	cnt=0;
	////!int kScl=round(1.8f*(float)(1<<16));
	int kScl=round(2.4f*(float)(1<<16));
	float fidx=(float)tb_arraySX/(float)sx;
	float fidy=(float)tb_arraySY/(float)sy;
	if(fidx > fidy){ kScl=round(kScl/fidy);}
	else { kScl=round(kScl/fidx); }
	int i_dx = (tb_arraySX << 16)/sx;
	int i_dy = (tb_arraySY << 16)/sy;
	int fx,fy;
	int m;
	for(m=0; m < tb_keyPoints; m++){
		for(i = 0; i < sy; i++){
			fy=i_dy*i;
			fx=0;
			unsigned short* pi=curPreImage+(m*tb_arraySY*tb_arraySX)+(fy>>16)*tb_arraySX;
			for(j = 0; j < sx; j++) {
				array[cnt]=( (*(pi+(fx>>16)))*kScl )>>16;
				//if(array[cnt]) array[cnt]+=round( (-(A*(j-sx/2) + B*(i-sy/2) + D)/C) * (1<<VX_FRACTION) - h_begin );//
				if(m==0){
					float as=(-(A*(j-sx/2) + B*(i-sy/2) + D)/C);
					substare[cnt]=round( (-(A*(j-sx/2) + B*(i-sy/2) + D)/C ) * (1<<VX_FRACTION) );
				}
				fx+=i_dx;
				cnt++;
			}
		}
	}
}
 
int sTBubble::quant(void)
{
	
	int curKScale=((currentKPFrame+1)<<16)/tb_keyPointsTime[currentKP];
	int prevKScale=(1<<16)-curKScale;
	int xxx=0;//x%d;
	int yyy=0;//y%d;
	int i,j,cnt=0;
	for(i = 0; i < sy; i++){
		for(j = 0; j < sx; j++) {

			int V;
			//if(pCurKP[i*sx+j]==0 && mask[i*sx+j]==0) continue;
			//mask[i*sx+j]=1;
			int offset=vMap.offsetBuf((x+j) & vMap.clip_mask_x, (y+i) & vMap.clip_mask_y);

			int vv;
			if(vMap.VxDBuf[offset]==0) vv= vMap.VxGBuf[offset] <<VX_FRACTION;
			else vv= vMap.VxDBuf[offset] <<VX_FRACTION;
			vv|=vMap.AtrBuf[offset] &VX_FRACTION_MASK;
			if(tmpVx[cnt]!=vv){
				inVx[cnt]+=vv-tmpVx[cnt];
//				if(inVx[cnt]<0)inVx[cnt]=0;
//				if(inVx[cnt]>MAX_VX_HEIGHT)inVx[cnt]=MAX_VX_HEIGHT;
			}

			int Vold=inVx[cnt];
			int prevH;
			if(pPrevKP!=0) 
				prevH = prevKScale*pPrevKP[i*sx+j];
			else 
				prevH = 0;
			int curH=(prevH + pCurKP[i*sx+j]*curKScale) >>(16) ;
			V=curH;
			if(curH==0 && mask[i*sx+j]==0) {tmpVx[cnt]=Vold; cnt++; continue;} //!!!!!!!
			mask[i*sx+j]=1;									  //!!!!!!!

			//V+=h_begin;
			V+=substare[cnt];
			//V=substare[cnt];

			static char fl=0;
			static int k_dh[256];
			if(fl==0){
				fl=1;
				for(int m=0; m<256; m++){
					float x=(float)m/128;//Диапазон от 0 до 2
					k_dh[m]=round((-0.1f+exp(-fabsf((x-1)*(x-1)*(x-1))/(0.4f*0.4f)))*(1<<16));
				}

			}
			if(Vold > V){
/*				//int inx=(Vold-V)>>3;
				int inx=(Vold-V)>>2;//>>3;
				if(inx>255)inx=255;
				//V=Vold + ((V-h_begin)*k_dh[inx]>>(16+1) );
				int dV=V-h_begin; if (dV>(10<<VX_FRACTION))dV=10<<VX_FRACTION;
				V=Vold + ((dV)*k_dh[inx]>>(16) );
				if(V>MAX_VX_HEIGHT)V=MAX_VX_HEIGHT;
				if(V<0) V=0;*/
				V=Vold;
			}
			else {
				//if(vMap.VxDBuf[offset]!=0){
				//	vMap.VxDBuf[offset]=0;
				//	vMap.SurBuf[offset]=vMap.GetGeoType(offset,V);
				//}
			}

			if(V>>VX_FRACTION > maxVx[cnt])maxVx[cnt]=V>>VX_FRACTION;
			const int DELTA_DEEP_GEO=3;//10;
			if(flag_occurrenceGeo && (V>>VX_FRACTION < maxVx[cnt]-DELTA_DEEP_GEO) ){
				if(vMap.VxDBuf[offset]!=0){
					vMap.VxDBuf[offset]=0;
					vMap.SurBuf[offset]=vMap.GetGeoType(offset,V);
				}
			}

			//if(V<0)V=0;
			//if(V>MAX_VX_HEIGHT)V=MAX_VX_HEIGHT;

			//для нормальной границы при  разрушении зеропласта
			if( (vMap.VxDBuf[offset]==0 && vMap.VxGBuf[offset]==(V >>VX_FRACTION)) || (vMap.VxDBuf[offset]==(V >>VX_FRACTION)) ){ 
				vMap.AtrBuf[offset]=(V &VX_FRACTION_MASK) | (vMap.AtrBuf[offset]&=~VX_FRACTION_MASK);
			}
			else {
				if(vMap.VxDBuf[offset]==0) vMap.VxGBuf[offset]=V >>VX_FRACTION;
				else vMap.VxDBuf[offset]=V >>VX_FRACTION;
				//нужно сохранить только признак тени, признак At_ZEROPLASTPRESENT при любом изменении удаляется
				vMap.AtrBuf[offset]=(V &VX_FRACTION_MASK) | (vMap.AtrBuf[offset]&=~(VX_FRACTION_MASK|At_NOTPURESURFACE));
				//if(vMap.VxDBuf[offset]< V >>VX_FRACTION)vMap.VxDBuf[offset]=0;
			}
			tmpVx[cnt]=V;

			cnt++;

		}
	}
	//if(h_begin < vMin) h_begin=h_begin + (1<<VX_FRACTION);
	vMap.recalcArea2Grid(vMap.XCYCL(x-1), vMap.YCYCL(y-1), vMap.XCYCL(x + sx+1), vMap.YCYCL(y + sy+1) );
	vMap.regRender(x, y, x+sx, y+sy);

	frame++; currentKPFrame++;
	if( currentKPFrame >= tb_keyPointsTime[currentKP] ) {
		previsionKP=currentKP;
		currentKP++;
		currentKPFrame=0;
		pPrevKP = pCurKP;
		pCurKP+=sx*sy;
		if(currentKP>=tb_keyPoints) {
			previsionKP=-1; 
			currentKP=0;
			pPrevKP=0; 
			pCurKP=array;
			return 0;
		}
	}
	return 1;
}


const int bubleWSXSY=100;
const int bubleWSXSY05=bubleWSXSY>>1;
typedef int tBubleWormForm[bubleWSXSY][bubleWSXSY];
tBubleWormForm bubleWormForm;

void bubble(int x, int y)
{
	static bool flag_firs=1;
	static int k_dh[256];
	if(flag_firs){
		flag_firs=0;
		int i,j;
		for(i=0; i<bubleWSXSY; i++){
			float y=(float)(i-bubleWSXSY05)/(float)bubleWSXSY05;
			for(j=0; j<bubleWSXSY; j++){
				float x=(float)(j-bubleWSXSY05)/(float)bubleWSXSY05; 
				float f=exp(-(fabsf(x*x)+fabsf(y*y))/(0.4*0.4));
				bubleWormForm[i][j]=round(f*(float)(1<<14)); //15 //(1<<16)+XRnd(1<<14);
			}
		}
		for(int m=0; m<256; m++){
			float x=(float)m/256;//Диапазон от 0 до 1
			k_dh[m]=(0.99+0.1*sin(x*50))* exp(-fabsf((x)*(x)*(x))/(0.4f*0.4f))*(1<<16);//(1.2+0.05*sin(2*tan(x*50)))
		}
	}


	const int MAX_EL_ARR=10000;
	static int num_el_arr=0;
	//static sTBubble bubArrImm[MAX_EL_ARR];
	static sTBubble * bubArr[MAX_EL_ARR];
	if(num_el_arr==0){for(int m=0; m<MAX_EL_ARR; m++) bubArr[m]=0;}

	static int begX=0, begY=0; 
	static float curX=x, curY=y;
	static float begAngle=-M_PI/10.;
	static int step=0;
//	if(begX!=x || begY!=y){
//		begAngle+=M_PI/4;
//		begX=x; begY=y;
//		curX=begX; curY=begY;
//	};

	const float averageSpeed=3.0;//4
	const float deltaSpeed=2.0;
	float SPEED=averageSpeed+frnd(deltaSpeed);
	curX+=cos(begAngle)*SPEED;
	curY+=sin(begAngle)*SPEED;
	int i,j;

//Выпячивание
	//worms(curX-32, curY-32);
	float kOffset=4.;//8
	int curIX=round(curX - cos(begAngle)*SPEED*(float)kOffset);
	int curIY=round(curY - sin(begAngle)*SPEED*(float)kOffset);
	int minV;
	minV=MAX_VX_HEIGHT;
	for(i = curIY-bubleWSXSY05; i < curIY+bubleWSXSY05; i++) {
		for(j = curIX-bubleWSXSY05; j < curIX+bubleWSXSY05; j++) {
			int offset=vMap.offsetBuf(vMap.XCYCL(j), vMap.YCYCL(i));
			//if(minV > vMap.VxGBuf[offset]<<VX_FRACTION)minV=vMap.VxGBuf[offset]<<VX_FRACTION;
			int tvx=GetWholeAlt4G(offset)<<VX_FRACTION;
			if(minV > tvx) minV=tvx;
		}
	}

	float K_FLUCTUATION=1.f;
	////!int curKFluct=round((1.5+frnd(K_FLUCTUATION))*(1<<16));
	int curKFluct=round((3.0+frnd(K_FLUCTUATION))*(1<<16));

	int xx,yy;
	for(i = curIY-bubleWSXSY05, yy=0; i < curIY+bubleWSXSY05; i++, yy++){
		for(j = curIX-bubleWSXSY05, xx=0; j < curIX+bubleWSXSY05; j++, xx++) {
			int offset=vMap.offsetBuf((j) & vMap.clip_mask_x, (i) & vMap.clip_mask_y);
			//int V=vMap.VxGBuf[offset]<<VX_FRACTION;
			//V+=vMap.AtrBuf[offset]&VX_FRACTION_MASK;
			int V, Vold;
			Vold=V=GetAlt4G(offset);
			//V+=FormWorms[i-y][j-x]>>8;

			dh=((bubleWormForm[yy][xx]>>7)*curKFluct)>>16;
			int idx=(V-minV)>>3; if(idx>255)idx=255;
			V+=((dh)/*>>7*/)*k_dh[idx]>>16; //8

			if(V>MAX_VX_HEIGHT)V=MAX_VX_HEIGHT;

			if( (Vold >>VX_FRACTION)==(V >>VX_FRACTION) ){
				vMap.AtrBuf[offset]=(V &VX_FRACTION_MASK) | (vMap.AtrBuf[offset]&=~VX_FRACTION_MASK);
			}
			else {
				//vMap.VxGBuf[offset]=V >>VX_FRACTION;
				//нужно сохранить только признак тени, признак At_ZEROPLASTPRESENT при любом изменении удаляется
				//vMap.AtrBuf[offset]=(V &VX_FRACTION_MASK) | (vMap.AtrBuf[offset]&=~(VX_FRACTION_MASK|At_ZEROPLASTPRESENT));
				PutAlt4G(V, offset, Vold);
			}
		}
	}
	vMap.recalcArea2Grid(vMap.XCYCL(curX-bubleWSXSY05-1), vMap.YCYCL(curY-bubleWSXSY05-1), vMap.XCYCL(curX+bubleWSXSY05+1), vMap.YCYCL(curY+bubleWSXSY05+1) );
	vMap.regRender(vMap.XCYCL(curX-bubleWSXSY05-1), vMap.YCYCL(curY-bubleWSXSY05-1), vMap.XCYCL(curX+bubleWSXSY05+1), vMap.YCYCL(curY+bubleWSXSY05+1) );

//
	const int MAX_QUANT_BUBBLE=10;//12//10//8
	const int MIN_QUANT_BUBBLE=5;//8//5//3
	int num_bubble=XRnd(MAX_QUANT_BUBBLE-MIN_QUANT_BUBBLE)+MIN_QUANT_BUBBLE;
	for(i=0; i<num_bubble; i++){
		if(num_el_arr>=MAX_EL_ARR-1) return;
		float dirAngl=fabsRnd(2*M_PI);
		float MAX_D=50.f;
		float r=abs((sqrt(-log(0.020f+fabsRnd(1.f-0.021f)))-1.f)*MAX_D);
		float addx=cos(dirAngl)*r;
		float addy=sin(dirAngl)*r;
		float MAX_ADDON_R=12.f;
		int addon=XRnd( round(MAX_ADDON_R*(1.0-r/MAX_D)) );//abs((sqrt(-log(0.020+fabsRnd(1.-0.021)))-1)*MAX_ADDON_R);//XRnd(round(MAX_ADDON_R));//XRnd( round(MAX_ADDON_R*(r/MAX_D)) );
		bool flag_occurrenceGeo=0;
		if(r <= 0.5*MAX_D && XRnd(10)>0) flag_occurrenceGeo=1;
		bubArr[num_el_arr]=new sTBubble(round(curX+addx),round(curY+addy),6+addon,6+addon, flag_occurrenceGeo);
		num_el_arr++;
	}

	int k=0;
	float maxQuantTailBubble=3.f;
	const int SIZE_TAIL=20;//30
	float dQuantTailBubble=maxQuantTailBubble/(float)SIZE_TAIL;
	const float BEGIN_TAIL_R=50.f;
	float tailR=BEGIN_TAIL_R;
	float dTailR=BEGIN_TAIL_R/(float)SIZE_TAIL;
	const int TAIL_OFFSET=1*SPEED;
	const float MAX_ADDON_R=8.f;
	float curAddonR=MAX_ADDON_R;
	float dAddonR=MAX_ADDON_R/(float)SIZE_TAIL;
	for(k=TAIL_OFFSET; k< SIZE_TAIL+TAIL_OFFSET; k+=3 ){
		if(step-k<0) break;
		int curTX=round(curX- cos(begAngle)*SPEED*(float)k);
		int curTY=round(curY- sin(begAngle)*SPEED*(float)k);
		num_bubble=round(maxQuantTailBubble);
		for(i=0; i<num_bubble; i++){
			if(num_el_arr>=MAX_EL_ARR-1) return;
			float dirAngl=fabsRnd(2*M_PI);
			float MAX_D=tailR;
			float r=abs((sqrt(-log(0.020f+fabsRnd(1.f-0.021f)))-1)*MAX_D);
			float addx=cos(dirAngl)*r;
			float addy=sin(dirAngl)*r;
			int addon=XRnd( round(MAX_ADDON_R*(1.0-r/MAX_D)) );//abs((sqrt(-log(0.020+fabsRnd(1.-0.021)))-1)*curAddonR); //XRnd(round(MAX_ADDON_R));//XRnd( round(MAX_ADDON_R*(r/MAX_D)) );
			bool flag_occurrenceGeo=0;
			if(r <= 0.5*BEGIN_TAIL_R && XRnd(10)>0) flag_occurrenceGeo=1;
			bubArr[num_el_arr]=new sTBubble(round(curTX+addx), round(curTY+addy),4+addon, 4+addon);
			num_el_arr++;
		}
		maxQuantTailBubble-=dQuantTailBubble;
		tailR-=dTailR;
		curAddonR-=dAddonR;
	}

	static list<sToolzDate> toolzDateLst;
	list<sToolzDate>::iterator p;
	individualToolzer<T2TE_CHANGING_TERRAIN_HEIGHT> toolzerChangeTerHeight ;
	for(p = toolzDateLst.begin(); p != toolzDateLst.end(); p++){
		if(XRnd(2)) toolzerChangeTerHeight.influenceDZ(p->x, p->y, p->r, 0, 0);
	};
	toolzDateLst.erase(toolzDateLst.begin(), toolzDateLst.end());

	int m;
	for(m=0; m<num_el_arr; m++){
		if(bubArr[m]){
			if(bubArr[m]->flag_sleep){
				int k;
				int bx=bubArr[m]->x;
				int by=bubArr[m]->y;
				int ex=bx+bubArr[m]->sx;
				int ey=by+bubArr[m]->sy;
				for(k=0; k<m; k++){
					if(bubArr[k]){
						if( ((bubArr[k]->x < bx && (bubArr[k]->x+bubArr[k]->sx) > bx ) ||
							(bubArr[k]->x > bx && (bubArr[k]->x) < ex )) && 
							((bubArr[k]->y < by && (bubArr[k]->y+bubArr[k]->sy) > by ) ||
							(bubArr[k]->y > by && (bubArr[k]->y) < ey )) ) break;
					}
				}
				if(k!=m) continue;
			}
			if(bubArr[m]->quant()==0 || XRnd(30)==1){
				toolzDateLst.push_back(sToolzDate(bubArr[m]->x+bubArr[m]->sx/2, bubArr[m]->y+bubArr[m]->sy/2, bubArr[m]->sx/2+bubArr[m]->sx/4));
				delete bubArr[m];
				bubArr[m]=0;
			}
		
		}
	}
	step++;
}


sTorpedo::sTorpedo(int x, int y)
{

	float angl=fabsRnd(M_PI*2);
	Vect2f tmp(cos(angl), sin(angl));
	init(x, y, tmp);
}

sTorpedo::sTorpedo(int _x, int _y, Vect2f& _direction)
{
	init(_x, _y, _direction);
}

void sTorpedo::init(int _x, int _y, Vect2f& _direction)
{
	beginX=_x; beginY=_y;
	curX=beginX; curY=beginY;
	for(int m=0; m<CTORPEDO_MAX_EL_ARR; m++) bubArr[m]=0;
	num_el_arr=0;
	//begAngle=_begAngle;//-M_PI/10.;
	direction=_direction;
	direction.normalize(1.);
	step=0;
}

bool sTorpedo::quant(void)
{
	static bool flag_firs=1;
	static int k_dh[256];
	if(flag_firs){
		flag_firs=0;
		int i,j;
		for(i=0; i<bubleWSXSY; i++){
			float y=(float)(i-bubleWSXSY05)/(float)bubleWSXSY05;
			for(j=0; j<bubleWSXSY; j++){
				float x=(float)(j-bubleWSXSY05)/(float)bubleWSXSY05; 
				float f=exp(-(fabsf(x*x)+fabsf(y*y))/(0.4*0.4));
				//bubleWormForm[i][j]=round((f*0.7f)*(float)(1<<14)); //15 //(1<<16)+XRnd(1<<14);
				bubleWormForm[i][j]=round(f*(float)(1<<14)); //15 //(1<<16)+XRnd(1<<14);
			}
		}
		for(int m=0; m<256; m++){
			float x=(float)m/256;//Диапазон от 0 до 1
			k_dh[m]=(0.99+0.1*sin(x*50))* exp(-fabsf((x)*(x)*(x))/(0.4f*0.4f))*(1<<16);//(1.2+0.05*sin(2*tan(x*50)))
		}
	}



	const float averageSpeed=3.0;//4
	const float deltaSpeed=2.0;
	float SPEED=averageSpeed+frnd(deltaSpeed);
	curX+=/*cos(begAngle)*/direction.x*SPEED;
	curY+=/*sin(begAngle)*/direction.y*SPEED;
	const int BORDER=bubleWSXSY;
	if(curX < BORDER || curX+BORDER>=vMap.H_SIZE) return 0;
	if(curY < BORDER || curY+BORDER>=vMap.V_SIZE) return 0;
	int i,j;

//Выпячивание
	float kOffset=4.;//8
	int curIX=round(curX - /*cos(begAngle)*/direction.x*SPEED*(float)kOffset);
	int curIY=round(curY - /*sin(begAngle)*/direction.y*SPEED*(float)kOffset);
////	worms(curX-32, curY-32);
	int minV;
	minV=MAX_VX_HEIGHT;
	int countZERO=0;
	for(i = curIY-bubleWSXSY05; i < curIY+bubleWSXSY05; i++) {
		for(j = curIX-bubleWSXSY05; j < curIX+bubleWSXSY05; j++) {
			int offset=vMap.offsetBuf(vMap.XCYCL(j), vMap.YCYCL(i));
			//if(minV > vMap.VxGBuf[offset]<<VX_FRACTION)minV=vMap.VxGBuf[offset]<<VX_FRACTION;
			int tvx=GetWholeAlt4G(offset)<<VX_FRACTION;
			if(minV > tvx) minV=tvx;
			if(tvx==0) countZERO++;
		}
	}
	const int Maximum_quantity_of_points_of_zero_height_for_permeability=128;
	if(countZERO>Maximum_quantity_of_points_of_zero_height_for_permeability) return 0;

	float K_FLUCTUATION=1.f;
	////!int curKFluct=round((1.5+frnd(K_FLUCTUATION))*(1<<16));
	int curKFluct=round((3.0+frnd(K_FLUCTUATION))*(1<<16));

	int xx,yy;
	for(i = curIY-bubleWSXSY05, yy=0; i < curIY+bubleWSXSY05; i++, yy++){
		int offY=vMap.offsetBuf(0, vMap.YCYCL(i));
		int offGY=vMap.offsetGBuf(0, vMap.YCYCL(i)>>kmGrid);
		for(j = curIX-bubleWSXSY05, xx=0; j < curIX+bubleWSXSY05; j++, xx++) {
			int offset=vMap.XCYCL(j) + offY;
			//int V=vMap.VxGBuf[offset]<<VX_FRACTION;
			//V+=vMap.AtrBuf[offset]&VX_FRACTION_MASK;
			int V, Vold;
			Vold=V=GetAlt4G(offset);
			//V+=FormWorms[i-y][j-x]>>8;

			dh=((bubleWormForm[yy][xx]>>7)*curKFluct)>>16;
			int idx=(V-minV)>>3; if(idx>255)idx=255;
			//dh=((dh)>>7)*k_dh[idx]>>16; //8
			dh=((dh))*k_dh[idx]>>16; //8
			if(dh==0) continue;
			int offsetG=(vMap.XCYCL(j)>>kmGrid) + offGY;
			if( (vMap.GABuf[offsetG]&GRIDAT_BUILDING) != 0){
				vMap.GABuf[offsetG]|=GRIDAT_BASE_OF_BUILDING_CORRUPT;
				///continue;
			}

			V+=dh;
			if(V>MAX_VX_HEIGHT)V=MAX_VX_HEIGHT;

			if( (Vold >>VX_FRACTION)==(V >>VX_FRACTION) ){
				vMap.AtrBuf[offset]=(V &VX_FRACTION_MASK) | (vMap.AtrBuf[offset]&=~VX_FRACTION_MASK);
			}
			else {
				//vMap.VxGBuf[offset]=V >>VX_FRACTION;
				//нужно сохранить только признак тени, признак At_ZEROPLASTPRESENT при любом изменении удаляется
				//vMap.AtrBuf[offset]=(V &VX_FRACTION_MASK) | (vMap.AtrBuf[offset]&=~(VX_FRACTION_MASK|At_ZEROPLASTPRESENT));
				PutAlt4G(V, offset, Vold);
			}
		}
	}
	vMap.recalcArea2Grid(vMap.XCYCL(curX-bubleWSXSY05-1), vMap.YCYCL(curY-bubleWSXSY05-1), vMap.XCYCL(curX+bubleWSXSY05+1), vMap.YCYCL(curY+bubleWSXSY05+1) );
	vMap.regRender(vMap.XCYCL(curX-bubleWSXSY05-1), vMap.YCYCL(curY-bubleWSXSY05-1), vMap.XCYCL(curX+bubleWSXSY05+1), vMap.YCYCL(curY+bubleWSXSY05+1) );

//

	const int MAX_QUANT_BUBBLE=10;//12//10//8
	const int MIN_QUANT_BUBBLE=5;//5;//8//5//3
	int num_bubble=XRnd(MAX_QUANT_BUBBLE-MIN_QUANT_BUBBLE)+MIN_QUANT_BUBBLE;
	for(i=0; i<num_bubble; i++){
		if(num_el_arr>=CTORPEDO_MAX_EL_ARR) break;
		float dirAngl=fabsRnd(2*M_PI);
		float MAX_D=50.f;
		float r=abs((sqrt(-log(0.020f+fabsRnd(1.f-0.021f)))-1)*MAX_D);
		float addx=cos(dirAngl)*r;
		float addy=sin(dirAngl)*r;
		float MAX_ADDON_R=12.f;
		int addon=XRnd( round(MAX_ADDON_R*(1.0-r/MAX_D)) );//abs((sqrt(-log(0.020+fabsRnd(1.-0.021)))-1)*MAX_ADDON_R);//XRnd(round(MAX_ADDON_R));//XRnd( round(MAX_ADDON_R*(r/MAX_D)) );
		bool flag_occurrenceGeo=0;
		if(r <= 0.5*MAX_D && XRnd(10)>0) flag_occurrenceGeo=1;
		if(insert2Arr(round(curX+addx),round(curY+addy),6+addon,6+addon, flag_occurrenceGeo)==0) break;
	}

	int k=0;
	float maxQuantTailBubble=3.f;
	const int SIZE_TAIL=20;//15;//20;//30
	float dQuantTailBubble=maxQuantTailBubble/(float)SIZE_TAIL;
	const float BEGIN_TAIL_R=50.f;
	float tailR=BEGIN_TAIL_R;
	float dTailR=BEGIN_TAIL_R/(float)SIZE_TAIL;
	const int TAIL_OFFSET=1*SPEED;
	const float MAX_ADDON_R=8.f;
	float curAddonR=MAX_ADDON_R;
	float dAddonR=MAX_ADDON_R/(float)SIZE_TAIL;
	for(k=TAIL_OFFSET; k< SIZE_TAIL+TAIL_OFFSET; k+=3 ){
		if(step-k<0) break;
		int curTX=round(curX- direction.x*SPEED*(float)k);
		int curTY=round(curY- direction.y*SPEED*(float)k);
		num_bubble=round(maxQuantTailBubble);
		for(i=0; i<num_bubble; i++){
			if(num_el_arr>=CTORPEDO_MAX_EL_ARR) break;
			float dirAngl=fabsRnd(2*M_PI);
			float MAX_D=tailR;
			float r=abs((sqrt(-log(0.020f+fabsRnd(1.f-0.021f)))-1)*MAX_D);
			float addx=cos(dirAngl)*r;
			float addy=sin(dirAngl)*r;
			int addon=XRnd( round(MAX_ADDON_R*(1.0-r/MAX_D)) );//abs((sqrt(-log(0.020+fabsRnd(1.-0.021)))-1)*curAddonR); //XRnd(round(MAX_ADDON_R));//XRnd( round(MAX_ADDON_R*(r/MAX_D)) );
			bool flag_occurrenceGeo=0;
			if(r <= 0.5*BEGIN_TAIL_R && XRnd(10)>0) flag_occurrenceGeo=1;
			if(insert2Arr(round(curTX+addx), round(curTY+addy),4+addon, 4+addon)==0) break;
		}
		maxQuantTailBubble-=dQuantTailBubble;
		tailR-=dTailR;
		curAddonR-=dAddonR;
	}

	list<sToolzDate>::iterator p;
	for(p = toolzDateLst.begin(); p != toolzDateLst.end(); p++){
		if(XRnd(2)) toolzerChangeTerHeight.influenceDZ(p->x, p->y, p->r, 0, 0);
	};
	toolzDateLst.erase(toolzDateLst.begin(), toolzDateLst.end());

	int m;
	for(m=0; m<num_el_arr; m++){
		if(bubArr[m]){
			if(bubArr[m]->flag_sleep){
				int k;
				int bx=bubArr[m]->x;
				int by=bubArr[m]->y;
				int ex=bx+bubArr[m]->sx;
				int ey=by+bubArr[m]->sy;
				for(k=0; k<m; k++){
					if(bubArr[k]){
						if( ((bubArr[k]->x < bx && (bubArr[k]->x+bubArr[k]->sx) > bx ) ||
							(bubArr[k]->x > bx && (bubArr[k]->x) < ex )) && 
							((bubArr[k]->y < by && (bubArr[k]->y+bubArr[k]->sy) > by ) ||
							(bubArr[k]->y > by && (bubArr[k]->y) < ey )) ) break;
					}
				}
				if(k!=m) continue;
			}
			if(bubArr[m]->quant()==0 || XRnd(30)==1){
				toolzDateLst.push_back(sToolzDate(bubArr[m]->x+bubArr[m]->sx/2, bubArr[m]->y+bubArr[m]->sy/2, bubArr[m]->sx/2+bubArr[m]->sx/4));
				deleteEl(m);
			}
		}
	}

	step++;

	//char buf[20];
	//itoa(num_el_arr, buf, 10);
	//strcat(buf, "\n");
	//::OutputDebugString(buf);
	return 1;
}



void bubbleOld(int x, int y)
{
	static unsigned char* flag_first_start=0;
	const int MAX_EL_ARR=10000;
	static int num_el_arr=0;
	static sTBubble *(bubArr[MAX_EL_ARR]);
	if(num_el_arr==0){for(int m=0; m<MAX_EL_ARR; m++) bubArr[m]=0;}

	if(num_el_arr>=MAX_EL_ARR-1) return;
	int addon=XRnd(16);
	bubArr[num_el_arr]=new sTBubble(x,y,8+addon,8+addon);
	num_el_arr++;
	int m;
	for(m=0; m<num_el_arr; m++){
		if(bubArr[m]){
			if(bubArr[m]->quant()==0){
				delete bubArr[m];
				bubArr[m]=0;
			}
		}
		
	}

/*
	static sTBubble * buble=0;
	if(buble==0) {
		int addon=XRnd(32);
		buble=new sTBubble(x,y,16+addon,16+addon);
	}
	if(buble->quant()==0){
		delete buble;
		buble=0;
	}
	*/
}



////////////////////////////////////////////////////////////////////////////////

sUPoligon::sUPoligon(point4UP* _pArr) 
{
	//int i;
	int i,j;
	for(i=0; i<3; i++) {
		pArr[i]=_pArr[i];
	}
	leftBorder=rightBorder=pArr[0].x;
	upBorder=downBorder=pArr[0].y;
	for(i=1; i<3; i++) {
		if(leftBorder>pArr[i].x) leftBorder=pArr[i].x;
		if(rightBorder<pArr[i].x) rightBorder=pArr[i].x;
		if(upBorder>pArr[i].y)upBorder=pArr[i].y;
		if(downBorder<pArr[i].y)downBorder=pArr[i].y;
	}
	rightBorder++;
	downBorder++;
	sx = rightBorder-leftBorder+1;
	sy = downBorder-upBorder+1;
	arrayVx=new short[sx*sy];
	arrayA=new bool[sx*sy];
	// отсортируем вершины по y
	Vect2s tmpv;
	Vect2s a(pArr[0].x, pArr[0].y);
	Vect2s b(pArr[1].x, pArr[1].y);
	Vect2s c(pArr[2].x, pArr[2].y);
	if(a.y > b.y ) { tmpv=a; a=b; b=tmpv; }
	if(a.y > c.y ) { tmpv=a; a=c; c=tmpv; }
	if(b.y > c.y ) { tmpv=b; b=c; c=tmpv; }

	vMin=MAX_VX_HEIGHT;
	vMax=0;

	int DY=(c.y-a.y);
	if(DY<=0) return; //Прерывание если нет полигона
	int d1,d2;
	int dL,dR;
	int xLp,xRp;
	int xL,xR;
	int cnt=0;
	int yWrk,xWrk;
	bool flag_left_long;
	d1=((c.x-a.x)<<16) / DY;
	int DY1=b.y-a.y;
	int DY2=c.y-b.y;
	if(DY1>0){
		d2=((b.x-a.x)<<16) / DY1;
		if (d1<d2) { dL=d1; dR=d2; flag_left_long=1;}
		else { dL=d2; dR=d1; flag_left_long=0;}
		xLp=xRp=(a.x<<16)+(1<<15);
		for(yWrk=a.y; yWrk<b.y; yWrk++){
			xL=xLp>>16; xR=xRp>>16;
			for(xWrk=leftBorder; xWrk< xL; xWrk++) {arrayA[cnt]=0; arrayVx[cnt++]=vMap.SGetAlt(vMap.XCYCL(xWrk), vMap.YCYCL(yWrk));}
			for(/*xWrk=xL*/; xWrk<=xR; xWrk++) {
				arrayA[cnt]=1;
				short vx=vMap.SGetAlt(vMap.XCYCL(xWrk), vMap.YCYCL(yWrk));
				if(vx<vMin)vMin=vx;
				if(vx>vMax)vMax=vx;
				arrayVx[cnt++]=vx;
			}
			for(/*xWrk=leftBorder*/; xWrk<=rightBorder; xWrk++){arrayA[cnt]=0; arrayVx[cnt++]=vMap.SGetAlt(vMap.XCYCL(xWrk), vMap.YCYCL(yWrk));}
			xLp+=dL; xRp+=dR;
		}
	}
	else { //Полигон не имеет первой части
		if(a.x < b.x) {
			xLp=(a.x<<16)+(1<<15);
			dL=((c.x-a.x)<<16) / DY2;
			xRp=(b.x<<16)+(1<<15);
			dR=((c.x-b.x)<<16) / DY2;
		} 
		else {
			xLp=(b.x<<16)+(1<<15);
			dL=((c.x-b.x)<<16) / DY2;
			xRp=(a.x<<16)+(1<<15);
			dR=((c.x-a.x)<<16) / DY2;
		}
		goto loc_begDraw2Part;
	}
	if(DY2>0) {
		d2=((c.x-b.x)<<16) / DY2;
		if(flag_left_long==1) {
			xRp=(b.x<<16)+(1<<15);
			dR=((c.x-b.x)<<16) / DY2;
		}
		else {
			xLp=(b.x<<16)+(1<<15);
			dL=((c.x-b.x)<<16) / DY2;
		}
loc_begDraw2Part:
		for(yWrk=b.y; yWrk<=c.y; yWrk++){
			xL=xLp>>16; xR=xRp>>16;
			for(xWrk=leftBorder; xWrk< xL; xWrk++){ arrayA[cnt]=0; arrayVx[cnt++]=vMap.SGetAlt(vMap.XCYCL(xWrk), vMap.YCYCL(yWrk));}
			for(/*xWrk=xL*/; xWrk<=xR; xWrk++) {
				arrayA[cnt]=1;
				short vx=vMap.SGetAlt(vMap.XCYCL(xWrk), vMap.YCYCL(yWrk));
				if(vx<vMin)vMin=vx;
				if(vx>vMax)vMax=vx;
				arrayVx[cnt++]=vx;
			}
			for(/*xWrk=leftBorder*/; xWrk<=rightBorder; xWrk++){ arrayA[cnt]=0; arrayVx[cnt++]=vMap.SGetAlt(vMap.XCYCL(xWrk), vMap.YCYCL(yWrk));}
			xLp+=dL; xRp+=dR;
		}
	}


	hBegin=vMin+(vMax-vMin)/3;//+(vmax-vmin)/4;

	Vect3f position(leftBorder+sx/2, upBorder+sy/2, (float)hBegin/(float)(1<<VX_FRACTION));
	Vect3f outOrientation;
	int radius;
	if(sx>sy) radius=sx/2;
	else radius=sy/2;
	analyzeTerrain(position, radius, outOrientation);
	//h_begin=round(position.z*(1<<VX_FRACTION));
	float A=outOrientation.x;
	float B=outOrientation.y;
	float C=-outOrientation.z;
	float D=(float)hBegin/(float)(1<<VX_FRACTION);//position.z;

	int xx,yy;
	for(i=0; i<sy; i++){
		yy=vMap.YCYCL(upBorder+i);
		for(j=0; j<sx; j++){
			xx=vMap.XCYCL(leftBorder+j);
			if(arrayVx[i*sx+j]>0){
				float as=(-(A*(float)(j-sx/2) + B*(float)(i-sy/2) + D)/C);
				arrayVx[i*sx+j]-=round(as*(float)(1<<VX_FRACTION));
			}
		}
	}
	pArr[0].z=round( (-(A*(pArr[0].x-leftBorder-sx/2) + B*(pArr[0].y-upBorder-sy/2) + D)/C) *(1<<VX_FRACTION) );
	pArr[1].z=round( (-(A*(pArr[1].x-leftBorder-sx/2) + B*(pArr[1].y-upBorder-sy/2) + D)/C) *(1<<VX_FRACTION) );
	pArr[2].z=round( (-(A*(pArr[2].x-leftBorder-sx/2) + B*(pArr[2].y-upBorder-sy/2) + D)/C) *(1<<VX_FRACTION) );


	curAngleG=0;
}

sUPoligon::~sUPoligon(void){
	delete [] arrayVx;
	delete [] arrayA;
};


void sUPoligon::smoothPoligon(Vect2f a, Vect2f b, Vect2f c)
{
	const int BORDER_P=2;
	Vect2f tmp;
	if(a.y > b.y ) { tmp=a; a=b; b=tmp; }
	if(a.y > c.y ) { tmp=a; a=c; c=tmp; }
	if(b.y > c.y ) { tmp=b; b=c; c=tmp; }

	int upBorder=a.y;
	int downBorder=c.y;
	int leftBorder=a.x;
	if(leftBorder>b.x)leftBorder=b.x;
	if(leftBorder>c.x)leftBorder=c.x;
	int rightBorder=a.x;
	if(rightBorder<b.x)rightBorder=b.x;
	if(rightBorder<c.x)rightBorder=c.x;

	float DY=(c.y-a.y);
	if(round(DY)<1.f) return; //Прерывание если нет полигона
	float d1,d2;
	float dxL,dxR;
	float xLp,xRp;

	int xL,xR;
	int cnt=0;
	int yWrk,xWrk;
	bool flag_left_long;
	d1=(c.x-a.x) / DY;
	float DY1=b.y-a.y;
	float DY2=c.y-b.y;
	if(round(DY1)>0){
		d2=(b.x-a.x) / DY1;
		if (d1<d2) { 
			dxL=d1; dxR=d2; 
			flag_left_long=1;
		}
		else {
			dxL=d2; dxR=d1;
			flag_left_long=0;
		}
		xLp=xRp=a.x;
		for(yWrk=round(a.y); yWrk<round(b.y); yWrk++){
			xL=round(xLp-BORDER_P); xR=round(xRp+BORDER_P);
			if(xL!=xR){
				for(xWrk=xL; xWrk<=xR; xWrk++) {
					int dy,dx,v=0;
					for(dy = -1;dy <= 1; dy++){
						for(dx = -1; dx <= 1; dx++){
							v += vMap.SGetAlt(vMap.XCYCL(xWrk+dx), vMap.YCYCL(yWrk +dy));
						}
					}
					v/=9;
					vMap.SPutAlt(xWrk, yWrk, v);
				}
			}
			xLp+=dxL; xRp+=dxR;
		}
	}
	else { //Полигон не имеет первой части
		if(a.x < b.x) {
			xLp=a.x; dxL=(c.x-a.x) / DY2;
			xRp=b.x; dxR=(c.x-b.x) / DY2;
		} 
		else {
			xLp=b.x; dxL=(c.x-b.x) / DY2;
			xRp=a.x; dxR=(c.x-a.x) / DY2;
		}
		goto loc_begDraw2Part;
	}
	if(round(DY2)>0) {
		d2=(c.x-b.x) / DY2;
		if(flag_left_long==1) {
			xRp=b.x; dxR=(c.x-b.x) / DY2;

		}
		else {
			xLp=b.x; dxL=(c.x-b.x) / DY2;
		}
loc_begDraw2Part:
		for(yWrk=round(b.y); yWrk<=round(c.y); yWrk++){
			xL=round(xLp-BORDER_P); xR=round(xRp+BORDER_P);
			if(xL!=xR){
				for(xWrk=xL; xWrk<=xR; xWrk++) {
					int dy,dx,v=0;
					for(dy = -1;dy <= 1; dy++){
						for(dx = -1;dx <= 1; dx++){
							v += vMap.SGetAlt(vMap.XCYCL(xWrk+dx), vMap.YCYCL(yWrk +dy));
						}
					}
					v/=9;
					vMap.SPutAlt(xWrk, yWrk, v);
				}
			}
			xLp+=dxL; xRp+=dxR;
		}
	}
	vMap.recalcArea2Grid(vMap.XCYCL(leftBorder), vMap.YCYCL(upBorder), vMap.XCYCL(leftBorder+sx), vMap.YCYCL(upBorder+sy) );
	vMap.regRender(vMap.XCYCL(leftBorder), vMap.YCYCL(upBorder), vMap.XCYCL(leftBorder+sx), vMap.YCYCL(upBorder+sy) );
}

void sUPoligon::quant(float angle_grad)
{
	int xx,yy;
	unsigned int i,j;
	for(i=0; i<sy; i++){
		yy=vMap.YCYCL(upBorder+i);
		for(j=0; j<sx; j++){
			xx=vMap.XCYCL(leftBorder+j);
			// !Оптимизировать
			if(arrayA[i*sx+j]==1){
				//vMap.SPutAltDam(xx,yy,0);
				//vMap.SPutAltGeo(xx,yy,0);
			}
			else if( (arrayA[i*sx+(j+1)%sx]==1) || ( j>0 && arrayA[i*sx+(j-1)%sx]==1) ){
				//vMap.SPutAltDam(xx,yy,0);
				//vMap.SPutAltGeo(xx,yy,0);
			}
		}
	}

	int o_x=pArr[0].x;
	int o_y=pArr[0].y;
	int o_z=pArr[0].z>>VX_FRACTION;//arrayVx[(o_y-upBorder)*sx+(o_x-leftBorder)]>>VX_FRACTION;
	Vect3f qVect(pArr[2].x-o_x, pArr[2].y-o_y, (pArr[2].z>>VX_FRACTION)-o_z);//0);//
	//Определение справа или слева точка от вектора кватерниона
	Vect3f v_o(pArr[2].x-pArr[0].x, pArr[2].y-pArr[0].y, 0.f);
	Vect3f v_2(pArr[1].x-pArr[0].x, pArr[1].y-pArr[0].y, 0.f);
	v_o.normalize(1.f);
	v_2.normalize(1.f);
	Vect3f result;
	result.cross(v_o, v_2);
	if(result.z<0) angle_grad=-angle_grad;

	//static float planeAngle=0.5f*M_PI/180.f;//0.f;
	//planeAngle-=0.5f*M_PI/180.f;
	float planeAngle=angle_grad*M_PI/180.f;
	QuatF wQuat(planeAngle, qVect);
/*		for(i=0; i<sy; i++){
		yy=vMap.YCYCL(upBorder+i);
		for(j=0; j<sx; j++){
			xx=vMap.XCYCL(leftBorder+j);
			if(arrayVx[i*sx+j]!=SUPOLIGON_NON_POLIGON){
				float curz=(float)arrayVx[i*sx+j]/(1<<VX_FRACTION);
				Vect3f in(xx-o_x, yy-o_y, curz-o_z);
				Vect3f out(0,0,0);
				wQuat.xform(in, out);
				int wx=vMap.XCYCL(round(out.x)+o_x);
				int wy=vMap.YCYCL(round(out.y)+o_y);
				int wz=round(out.z*(1<<VX_FRACTION))+(o_z<<VX_FRACTION);
				wz+=100<<VX_FRACTION;
				//int vv=vMap.SGetAlt(wx,wy);
				//if (vv) vMap.SPutAltDam(wx,wy,(wz+vv)>>1);
				//else vMap.SPutAltDam(wx,wy,wz);
				vMap.SPutAltDam(wx,wy,wz);
				//vMap.SPutAltGeo(wx,wy,wz);
			}
		}
	}
*/
	Vect3f in(0,0,0);
	Vect3f out(0,0,0);
	Vect3f a(0,0,0);
	Vect3f b(0,0,0);
	Vect3f c(0,0,0);
	float curz;
	curz=(float)pArr[0].z/(1<<VX_FRACTION);
	in.set(pArr[0].x-o_x, pArr[0].y-o_y, curz-o_z);
	wQuat.xform(in, out);
	a.x=vMap.XCYCL(round(out.x)+o_x);
	a.y=vMap.YCYCL(round(out.y)+o_y);
	//a.z=round(out.z*(1<<VX_FRACTION))+(o_z<<VX_FRACTION);
	a.z=out.z+o_z;

	curz=(float)pArr[1].z/(1<<VX_FRACTION);
	in.set(pArr[1].x-o_x, pArr[1].y-o_y, curz-o_z);
	wQuat.xform(in, out);
	b.x=vMap.XCYCL(round(out.x)+o_x);
	b.y=vMap.YCYCL(round(out.y)+o_y);
	//b.z=round(out.z*(1<<VX_FRACTION))+(o_z<<VX_FRACTION);
	b.z=out.z+o_z;

	Vect2f buckupB=b;

	curz=(float)pArr[2].z/(1<<VX_FRACTION);
	in.set(pArr[2].x-o_x, pArr[2].y-o_y, curz-o_z);
	wQuat.xform(in, out);
	c.x=vMap.XCYCL(round(out.x)+o_x);
	c.y=vMap.YCYCL(round(out.y)+o_y);
	//c.z=round(out.z*(1<<VX_FRACTION))+(o_z<<VX_FRACTION);
	c.z=out.z+o_z;

	//Построение плоскости по 3-м точкам
	//float cA = a.y*(b.z-c.z) + b.y*(c.z-a.z) + c.y*(a.z-b.z);
	//float cB = a.z*(b.x-c.x) + b.z*(c.x-a.x) + c.z*(a.x-b.x);
	//float cC = a.x*(b.y-c.y) + b.x*(c.y-a.y) + c.x*(a.y-b.y);
	//float cD = -(a.x*(b.y*c.z-c.y*b.z) + b.x*(c.y*a.z-a.y*c.z) + c.x*(a.y*b.z-b.y*a.z) );
	float cA=(b.y-a.y)*(c.z-a.z)-(c.y-a.y)*(b.z-a.z);
	float cB=(b.z-a.z)*(c.x-a.x)-(c.z-a.z)*(b.x-a.x);
	float cC=(b.x-a.x)*(c.y-a.y)-(c.x-a.x)*(b.y-a.y);
	float cD=-cA*a.x-cB*a.y-cC*a.z;


	float a_u=pArr[0].x-leftBorder;
	float a_v=pArr[0].y-upBorder;
	float b_u=pArr[1].x-leftBorder;
	float b_v=pArr[1].y-upBorder;
	float c_u=pArr[2].x-leftBorder;
	float c_v=pArr[2].y-upBorder;
	Vect3f tmp;
	float tmpu, tmpv;
	int k=0;
	if(a.y > b.y ) { tmp=a; a=b; b=tmp; tmpu=a_u; a_u=b_u; b_u=tmpu; tmpv=a_v; a_v=b_v; b_v=tmpv;}
	if(a.y > c.y ) { tmp=a; a=c; c=tmp; tmpu=a_u; a_u=c_u; c_u=tmpu; tmpv=a_v; a_v=c_v; c_v=tmpv;}
	if(b.y > c.y ) { tmp=b; b=c; c=tmp; tmpu=b_u; b_u=c_u; c_u=tmpu; tmpv=b_v; b_v=c_v; c_v=tmpv;}

	float DY=(c.y-a.y);
	if(round(DY)<1.f) return; //Прерывание если нет полигона
	float d1,d2;
	float dxL,dxR;
	float xLp,xRp;
	float zLp,zRp;
	float dzL,dzR;

	float uLp,uRp;
	float duL, duR;
	float vLp,vRp;
	float dvL, dvR;

	int xL,xR;
	int cnt=0;
	int yWrk,xWrk;
	bool flag_left_long;
	d1=(c.x-a.x) / DY;
	float DY1=b.y-a.y;
	float DY2=c.y-b.y;
	if(round(DY1)>0){
		d2=(b.x-a.x) / DY1;
		if (d1<d2) { 
			dxL=d1; dxR=d2; 
			flag_left_long=1;
			dzL=(round(c.z)-round(a.z)) / DY;
			dzR=(round(b.z)-round(a.z)) / DY1;
			duL=(c_u-a_u) / DY;
			duR=(b_u-a_u) / DY1;
			dvL=(c_v-a_v) / DY;
			dvR=(b_v-a_v) / DY1;
		}
		else {
			dxL=d2; dxR=d1;
			flag_left_long=0;
			dzL=(b.z-a.z) / DY1;
			dzR=(c.z-a.z) / DY;
			duL=(b_u-a_u) / DY1;
			duR=(c_u-a_u) / DY;
			dvL=(b_v-a_v) / DY1;
			dvR=(c_v-a_v) / DY;
		}
		xLp=xRp=a.x;
		zLp=zRp=a.z;
		uLp=uRp=a_u;
		vLp=vRp=a_v;
		for(yWrk=round(a.y); yWrk<round(b.y); yWrk++){
			xL=round(xLp); xR=round(xRp);
			if(xL!=xR){
				float zCur=zLp;
				float dzCur=(round(zRp)-round(zLp))/(float)(xR-xL);
				float uCur=uLp;
				float duCur=(uRp-uLp)/(float)(xR-xL);
				float vCur=vLp;
				float dvCur=(vRp-vLp)/(float)(xR-xL);
				for(xWrk=xL; xWrk<=xR; xWrk++) {
					//short addVx=arrayVx[(vMap.XCYCL(xWrk)-leftBorder) + sx*(vMap.YCYCL(yWrk)-upBorder)];
					short addVx;
					float u0=floor(uCur);
					float ddu=uCur-u0;
					int u=round(u0);
					float v0=floor(vCur);
					float ddv=vCur-v0;
					int v=round(v0);
					addVx=round( (arrayVx[u+v*sx]*(1-ddu)+arrayVx[u+1+v*sx]*ddu) *(1-ddv) +
								 (arrayVx[u+(v+1)*sx]*(1-ddu)+arrayVx[u+1+(v+1)*sx]*ddu) *ddv );
					//addVx=arrayVx[round(uCur)+round(vCur)*sx];
					//addVx=0;
					//vMap.SPutAltDam(vMap.XCYCL(xWrk), vMap.YCYCL(yWrk), round(zCur*(1<<VX_FRACTION)+addVx) );
					vMap.SPutAlt(vMap.XCYCL(xWrk), vMap.YCYCL(yWrk), round( (-(cD+cA*xWrk+cB*yWrk)/cC)*(1<<VX_FRACTION)+addVx) );
					//arrayVx[cnt++]=vx;
					zCur+=dzCur;
					uCur+=duCur;
					vCur+=dvCur;
				}
			}
			xLp+=dxL; xRp+=dxR;
			zLp+=dzL; zRp+=dzR;
			uLp+=duL; uRp+=duR;
			vLp+=dvL; vRp+=dvR;
		}
	}
	else { //Полигон не имеет первой части
		if(a.x < b.x) {
			xLp=a.x; dxL=(c.x-a.x) / DY2;
			xRp=b.x; dxR=(c.x-b.x) / DY2;

			zLp=a.z; dzL=(c.z-a.z) / DY2;
			zRp=b.z; dzR=(c.z-b.z) / DY2;

			uLp=a_u; duL=(c_u-a_u) / DY2;
			uRp=b_u; duR=(c_u-b_u) / DY2;

			vLp=a_v; dvL=(c_v-a_v) / DY2;
			vRp=b_v; dvR=(c_v-b_v) / DY2;
		} 
		else {
			xLp=b.x; dxL=(c.x-b.x) / DY2;
			xRp=a.x; dxR=(c.x-a.x) / DY2;

			zLp=b.z; dzL=(c.z-b.z) / DY2;
			zRp=a.z; dzR=(c.z-a.z) / DY2;

			uLp=b_u; duL=(c_u-b_u) / DY2;
			uRp=a_u; duR=(c_u-a_u) / DY2;

			vLp=b_v; dvL=(c_v-b_v) / DY2;
			vRp=a_v; dvR=(c_v-a_v) / DY2;
		}
		goto loc_begDraw2Part;
	}
	if(round(DY2)>0) {
		d2=(c.x-b.x) / DY2;
		if(flag_left_long==1) {
			xRp=b.x; dxR=(c.x-b.x) / DY2;
			zRp=b.z; dzR=(c.z-b.z) / DY2;
			uRp=b_u; duR=(c_u-b_u) / DY2;
			vRp=b_v; dvR=(c_v-b_v) / DY2;

		}
		else {
			xLp=b.x; dxL=(c.x-b.x) / DY2;
			zLp=b.z; dzL=(c.z-b.z) / DY2;
			uLp=b_u; duL=(c_u-b_u) / DY2;
			vLp=b_v; dvL=(c_v-b_v) / DY2;
		}
loc_begDraw2Part:
		for(yWrk=round(b.y); yWrk<=round(c.y); yWrk++){
			xL=round(xLp); xR=round(xRp);
			if(xL!=xR){
				float zCur=zLp;
				float dzCur=(zRp-zLp)/(float)(xR-xL);
				float uCur=uLp;
				float duCur=(uRp-uLp)/(float)(xR-xL);
				float vCur=vLp;
				float dvCur=(vRp-vLp)/(float)(xR-xL);
				for(xWrk=xL; xWrk<=xR; xWrk++) {
					//short addVx=arrayVx[(vMap.XCYCL(xWrk)-leftBorder) + sx*(vMap.YCYCL(yWrk)-upBorder)];
					short addVx;
					float u0=floor(uCur);
					float ddu=uCur-u0;
					int u=round(u0);
					float v0=floor(vCur);
					float ddv=vCur-v0;
					int v=round(v0);
					addVx=round( (arrayVx[u+v*sx]*(1-ddu)+arrayVx[u+1+v*sx]*ddu) *(1-ddv) +
								 (arrayVx[u+(v+1)*sx]*(1-ddu)+arrayVx[u+1+(v+1)*sx]*ddu) *ddv );
					//addVx=arrayVx[round(uCur)+round(vCur)*sx];
					//addVx=0;
					//vMap.SPutAltDam(vMap.XCYCL(xWrk), vMap.YCYCL(yWrk), round(zCur*(1<<VX_FRACTION)+addVx) );
					vMap.SPutAlt(vMap.XCYCL(xWrk), vMap.YCYCL(yWrk), round( (-(cD+cA*xWrk+cB*yWrk)/cC)*(1<<VX_FRACTION)+addVx) );
					//arrayVx[cnt++]=vx;
					zCur+=dzCur;
					uCur+=duCur;
					vCur+=dvCur;
				}
			}
			xLp+=dxL; xRp+=dxR;
			zLp+=dzL; zRp+=dzR;
			uLp+=duL; uRp+=duR;
			vLp+=dvL; vRp+=dvR;
		}
	}

	{
		Vect2f as(pArr[0].x, pArr[0].y);
		Vect2f bs(pArr[1].x, pArr[1].y);
		Vect2f cs(buckupB.x, buckupB.y);
		smoothPoligon(as, bs,cs);
	}

	{
		Vect2f as(pArr[1].x, pArr[1].y);
		Vect2f bs(pArr[2].x, pArr[2].y);
		Vect2f cs(buckupB.x, buckupB.y);
		smoothPoligon(as, bs,cs);
	}

	vMap.recalcArea2Grid(vMap.XCYCL(leftBorder), vMap.YCYCL(upBorder), vMap.XCYCL(leftBorder+sx), vMap.YCYCL(upBorder+sy) );
	vMap.regRender(vMap.XCYCL(leftBorder), vMap.YCYCL(upBorder), vMap.XCYCL(leftBorder+sx), vMap.YCYCL(upBorder+sy) );
};




Vect2f calcCrossPoint(Vect3f& Line1, Vect3f& Line2)//float A1, float B1, float C1, float A2, float B2, float C2)
{
	float A1=Line1.x;
	float B1=Line1.y;
	float C1=Line1.z;
	float A2=Line2.x;
	float B2=Line2.y;
	float C2=Line2.z;
	Vect2f retval;
	retval.x=(C1*B2-C2*B1)/(B1*A2-B2*A1);
	retval.y=(C2*A1-C1*A2)/(B1*A2-B2*A1);
	return retval;
}

bool poiligonUP(int _x, int _y)
{
	int i;

	//static int begX=0, begY=0; 
	//static float curX=x, curY=y;
	static float begAngle=-M_PI/10.;


	const int MAX_CROSS_POINT=20;
	const int MAX_EARTH_POLIGON=MAX_CROSS_POINT-2;
	static bool flag_init=0;
	static vector<Vect2f> pointArr;
	static list<sUPoligon*> poligonArr;
	//vector<Vect2f>::iterator p;
	if(flag_init==0) {
		pointArr.reserve(MAX_CROSS_POINT);
		Vect3f prevABC(0,0,0);
		Vect2f prevPoint(0,0);
		bool flag_right=1;
		const float AVERAGE_SPEED=50.0;//4
		const float DELTA_SPEED=40.0;
		const float MIN_ANGLE_NORMAL=5.f*(M_PI/180.f);
		const float MAX_ANGLE_NORMAL=50.f*(M_PI/180.f);
		Vect2f curPoint(_x,_y);
		for(i=0; i<=MAX_CROSS_POINT; i++){
			float angleNorm=MIN_ANGLE_NORMAL+fabsRnd(MAX_ANGLE_NORMAL-MIN_ANGLE_NORMAL);
			if(!flag_right)angleNorm=-angleNorm;
			float angl=begAngle+angleNorm;
			Vect2f normal;
			float normal_x=1.0*cos(angl);
			float normal_y=1.0*sin(angl);
			float C=-(normal_x*curPoint.x+normal_y*curPoint.y);
			Vect3f curABC(normal_x, normal_y, C);

			if(i!=0) {
				Vect2f crossPoint=calcCrossPoint(prevABC, curABC);
				const int BORDER_WORD=20;
				if(crossPoint.x < BORDER_WORD || crossPoint.x+BORDER_WORD>vMap.H_SIZE || 
					crossPoint.y < BORDER_WORD || crossPoint.y+BORDER_WORD>vMap.V_SIZE) break;
				pointArr.push_back(crossPoint);
			}
			prevABC=curABC;
			prevPoint=curPoint;
			flag_right^=1;
			float SPEED=AVERAGE_SPEED+frnd(DELTA_SPEED);
 			curPoint.x+=cos(begAngle)*SPEED;
			curPoint.y+=sin(begAngle)*SPEED;
		}
		flag_init=1;
	}

	static int step=1;
	if(pointArr.size()>=3){
		//for(; i<pointArr.size()-1; step++) 
		if(step<pointArr.size()-1) { //от второго до предпоследнего
			point4UP arr[3];
			float distance1=pointArr[step-1].distance(pointArr[step]);
			Vect2f v1=pointArr[step-1] - pointArr[step];
			v1.normalize(1.f);
			v1*=distance1*(1.f/2.f)+fabsRnd(distance1*(1.f/2.f)); // 1/2   2/3
			v1+=pointArr[step];

			float distance2=pointArr[step+1].distance(pointArr[step]);
			Vect2f v2=pointArr[step+1] - pointArr[step];
			v2.normalize(1.f);
			v2*=distance1/2.f+fabsRnd(distance2*(1.f/2.f)); //  2/3  2/3
			v2+=pointArr[step];


			//arr[0].x= round(pointArr[step-1].x);
			//arr[0].y= round(pointArr[step-1].y);
			arr[0].x= round(v1.x);
			arr[0].y= round(v1.y);

			arr[1].x= round(pointArr[step].x);
			arr[1].y= round(pointArr[step].y);

			//arr[2].x= round(pointArr[step+1].x);
			//arr[2].y= round(pointArr[step+1].y);
			arr[2].x= round(v2.x);
			arr[2].y= round(v2.y);

			sUPoligon* tmp=new sUPoligon(arr);
			poligonArr.push_back(tmp);
			
		}
	}
	step++;
	list<sUPoligon*>::iterator p;
	for(p=poligonArr.begin(); p!=poligonArr.end(); /*p++*/){
		if( (*p)->quant()==0) {
			delete (*p);
			p=poligonArr.erase(p);
		}
		else p++;
	}
	if(poligonArr.empty()) return 0;
	else return 1;
}

sGeoFault::sGeoFault(const Vect2f& begPoint, float begAngle, float aLenght)//int _x, int _y
{
	//const float begAngle=fabsRnd(2*M_PI);//-M_PI/10.;

	const int MAX_CROSS_POINT=20;
	const int MAX_EARTH_POLIGON=MAX_CROSS_POINT-2;

	pointArr.reserve(MAX_CROSS_POINT);
	Vect3f prevABC(0,0,0);
	Vect2f prevPoint(0,0);
	bool flag_right=1;
	const float AVERAGE_SPEED=70.0;//4
	const float DELTA_SPEED=30.0;
	const float MIN_ANGLE_NORMAL=15.f*(M_PI/180.f);
	const float MAX_ANGLE_NORMAL=40.f*(M_PI/180.f);
	Vect2f curPoint(begPoint);
	for(int i=0; i<=MAX_CROSS_POINT; i++){
		float angleNorm=MIN_ANGLE_NORMAL+fabsRnd(MAX_ANGLE_NORMAL-MIN_ANGLE_NORMAL);
		if(!flag_right)angleNorm=-angleNorm;
		float angl=begAngle+angleNorm;
		Vect2f normal;
		float normal_x=1.0*cos(angl);
		float normal_y=1.0*sin(angl);
		float C=-(normal_x*curPoint.x+normal_y*curPoint.y);
		Vect3f curABC(normal_x, normal_y, C);

		if(i!=0) {
			Vect2f crossPoint=calcCrossPoint(prevABC, curABC);
			const int BORDER_WORD=20;
			if(crossPoint.x < BORDER_WORD || crossPoint.x+BORDER_WORD>vMap.H_SIZE || 
				crossPoint.y < BORDER_WORD || crossPoint.y+BORDER_WORD>vMap.V_SIZE) break;
			pointArr.push_back(crossPoint);
		}
		prevABC=curABC;
		prevPoint=curPoint;
		flag_right^=1;
		float SPEED=AVERAGE_SPEED+frnd(DELTA_SPEED);
 		curPoint.x+=cos(begAngle)*SPEED;
		curPoint.y+=sin(begAngle)*SPEED;
		if(curPoint.distance(begPoint) > aLenght) break;
	}
	fstep=1.f;
	step=0;
	//step=1;
}

bool sGeoFault::quant()
{
	if(pointArr.size()>=3){
		//for(; i<pointArr.size()-1; step++) 
		if( step!=round(fstep) ){
			step=round(fstep);

			if(step<pointArr.size()-1) { //от второго до предпоследнего
				point4UP arr[3];
				float distance1=pointArr[step-1].distance(pointArr[step]);
				Vect2f v1=pointArr[step-1] - pointArr[step];
				v1.normalize(1.f);
				v1*=distance1*(4.f/6.f)+fabsRnd(distance1*(2.f/6.f)); // 1/2   2/3
				v1+=pointArr[step];

				const float MAX_DISTANCE=100.f;

				if(distance1>MAX_DISTANCE)distance1=MAX_DISTANCE;
				Vect2f v11=pointArr[step-1] - pointArr[step];
				v11.normalize(1.f);
				v11*=distance1*(0.f/3.f)+fabsRnd(distance1*(3.f/5.f)); // 1/2   2/3
				v11+=pointArr[step];


				float distance2=pointArr[step+1].distance(pointArr[step]);
				Vect2f v2=pointArr[step+1] - pointArr[step];
				v2.normalize(1.f);
				v2*=distance2*4.f/6.f+fabsRnd(distance2*(2.f/6.f)); //  2/3  2/3
				v2+=pointArr[step];

				if(distance2>MAX_DISTANCE)distance2=MAX_DISTANCE;
				Vect2f v22=pointArr[step+1] - pointArr[step];
				v22.normalize(1.f);
				v22*=distance2*(0.f/3.f)+fabsRnd(distance2*(3.f/5.f)); //  2/3  2/3
				v22+=pointArr[step];

				point4UP addArr[2];
				addArr[0].x= round(v11.x);
				addArr[0].y= round(v11.y);
				addArr[1].x= round(v22.x);
				addArr[1].y= round(v22.y);


				//arr[0].x= round(pointArr[step-1].x);
				//arr[0].y= round(pointArr[step-1].y);
				arr[0].x= round(v1.x);
				arr[0].y= round(v1.y);

				arr[1].x= round(pointArr[step].x);
				arr[1].y= round(pointArr[step].y);

				//arr[2].x= round(pointArr[step+1].x);
				//arr[2].y= round(pointArr[step+1].y);
				arr[2].x= round(v2.x);
				arr[2].y= round(v2.y);

				sUPoligonN* tmp=new sUPoligonN(arr, 2, addArr); // 
				poligonArr.push_back(tmp);
				
			}
		}
	}
//	step++;
	fstep+=0.33f + frnd(0.17f);

	list<sUPoligonN*>::iterator p;
	for(p=poligonArr.begin(); p!=poligonArr.end(); /*p++*/){
		if( (*p)->quant()==0) {
			delete (*p);
			p=poligonArr.erase(p);
		}
		else p++;
	}
	if(poligonArr.empty()) return 0;
	else return 1;
}



void poiligonUPOld(int x0, int y0)
{
/*	int x2=x0+130;
	int y2=y0-120;
	int x1=x0+150;
	int y1=y0+120;*/
	int x1, x2, y1, y2;
	x0=1110; y0=1640;
	x1=1143; y1=1550;
	x2=1194; y2=1601;

	static sUPoligon* plg=0;
	if(plg==0){
		point4UP pa[3];
		pa[0].x=x0; pa[0].y=y0;
		pa[1].x=x1; pa[1].y=y1;
		pa[2].x=x2; pa[2].y=y2;
		plg=new sUPoligon(pa);
	}
	static float angle=0;
	plg->quant(angle);
	angle+=10.5;


};

///////////////////////////////////////////////////////////////////
//Круговое вспучивание
#define GRAD2RADF(a)  ((a)*M_PI/180.f)
sGeoSwelling::sGeoSwelling(int xCentre, int yCentre)
{
	const int MIN_BEG_RADIUS_GEO_POLIGONS=10;
	const int MAX_BEG_RADIUS_GEO_POLIGONS=15;
	const int MIN_END_RADIUS_GEO_POLIGONS=40;
	const int MAX_END_RADIUS_GEO_POLIGONS=70;

	const int MIN_GEO_POLIGONS=5;
	const int MAX_GEO_POLIGONS=9;

	//const float MAX_ANGLE_GEO_POLIGON=70.f*(M_PI/180.f);

	int numPoligons=MIN_GEO_POLIGONS + XRnd(MAX_GEO_POLIGONS-MIN_GEO_POLIGONS);

	int i;
	float angleArr[MAX_GEO_POLIGONS];
	float sumNorm=0.f;

	const float MIN_ANGLE_GEO_POLIGON=0.4f;
	const float DELTA_ANGLE_GEO_POLIGON=1.f;
	for(i=0; i<numPoligons; i++){
		angleArr[i]=MIN_ANGLE_GEO_POLIGON+fabsRnd(DELTA_ANGLE_GEO_POLIGON);
		sumNorm+=angleArr[i];
	}
	sumNorm=2.f*M_PI/sumNorm;
	for(i=0; i<numPoligons; i++){
		angleArr[i]*=sumNorm;
	}

	float begAngle=(float)XRnd(360)*M_PI/180.f;

	Vect2f pnt1, pnt2, pnt3, pnt4;
	point4UP arr[3];
	point4UP addArr[2];

	float curAngle=begAngle;
	float curRad;
	int curXC=xCentre;
	int curYC=yCentre;
	for(i=0; i<numPoligons; i++){

		curRad=MIN_BEG_RADIUS_GEO_POLIGONS+XRnd(MAX_BEG_RADIUS_GEO_POLIGONS-MIN_BEG_RADIUS_GEO_POLIGONS);
		addArr[0].x= curXC + round(curRad*cos(curAngle));
		addArr[0].y= curYC + round(curRad*sin(curAngle));

		curRad=MIN_BEG_RADIUS_GEO_POLIGONS+XRnd(MAX_BEG_RADIUS_GEO_POLIGONS-MIN_BEG_RADIUS_GEO_POLIGONS);
		addArr[1].x= curXC + round(curRad*cos(curAngle+angleArr[i]));
		addArr[1].y= curYC + round(curRad*sin(curAngle+angleArr[i]));

		curRad=MIN_END_RADIUS_GEO_POLIGONS+XRnd(MAX_END_RADIUS_GEO_POLIGONS-MIN_END_RADIUS_GEO_POLIGONS);
		arr[0].x= curXC + round(curRad*cos(curAngle));
		arr[0].y= curYC + round(curRad*sin(curAngle));

		arr[1].x= curXC;
		arr[1].y= curYC;

		curRad=MIN_END_RADIUS_GEO_POLIGONS+XRnd(MAX_END_RADIUS_GEO_POLIGONS-MIN_END_RADIUS_GEO_POLIGONS);
		arr[2].x= curXC + round(curRad*cos(curAngle+angleArr[i]));
		arr[2].y= curYC + round(curRad*sin(curAngle+angleArr[i]));

		sUPoligonNSpec* tmp=new sUPoligonNSpec(arr, 2, addArr);
		poligonArr.push_back(tmp);
		curAngle+=angleArr[i];
	}

	//random_shuffle(angleArr, angleArr + numPoligons);

}
//sUPoligonNSpec::sUPoligonNSpec(point4UP* _basePntArr, int maxAddPnt, point4UP* _addPntArr)
//	:sUPoligonN(_basePntArr, maxAddPnt, _addPntArr)
//{
//	flagBegin=0;
//}

bool sGeoSwelling::quant()
{
	list<sUPoligonNSpec*>::iterator p;
	for(p=poligonArr.begin(); p!=poligonArr.end(); /*p++*/){
		if(fabsRnd(1.f)>0.4f){
			(*p)->flagBegin |=true;
		}
		if( (*p)->flagBegin) {
			if( (*p)->quant()==0) {
				delete (*p);
				p=poligonArr.erase(p);
			}
			else p++;
		}
		else p++;
	}
	if(poligonArr.empty()) return 0;
	else return 1;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

sUPoligonN::sUPoligonN(point4UP* _basePntArr, int maxAddPnt, point4UP* _addPntArr)
{
	//int i;
	int i,j;
	for(i=0; i<3; i++) {
		basePntArr[i]=_basePntArr[i];
	}

	xassert(maxAddPnt<=SUPOLIGONN_MAX_ADDING_POINT);
	nElAddPntArr=maxAddPnt;
	for(i=0; i<maxAddPnt; i++) {
		addPntArr[i]=_addPntArr[i];
	}

	leftBorder=rightBorder=basePntArr[0].x;
	upBorder=downBorder=basePntArr[0].y;
	for(i=1; i<3; i++) {
		if(leftBorder>basePntArr[i].x) leftBorder=basePntArr[i].x;
		if(rightBorder<basePntArr[i].x) rightBorder=basePntArr[i].x;
		if(upBorder>basePntArr[i].y)upBorder=basePntArr[i].y;
		if(downBorder<basePntArr[i].y)downBorder=basePntArr[i].y;
	}
	rightBorder++;
	downBorder++;
	sx = rightBorder-leftBorder+1;
	sy = downBorder-upBorder+1;
	arrayVx=new short[sx*sy];
	arrayA=new bool[sx*sy];
	// отсортируем вершины по y
	Vect2s tmpv;
	Vect2s a(basePntArr[0].x, basePntArr[0].y);
	Vect2s b(basePntArr[1].x, basePntArr[1].y);
	Vect2s c(basePntArr[2].x, basePntArr[2].y);
	if(a.y > b.y ) { tmpv=a; a=b; b=tmpv; }
	if(a.y > c.y ) { tmpv=a; a=c; c=tmpv; }
	if(b.y > c.y ) { tmpv=b; b=c; c=tmpv; }

	vMin=MAX_VX_HEIGHT;
	vMax=0;

	int DY=(c.y-a.y);
	if(DY<=0) return; //Прерывание если нет полигона
	int d1,d2;
	int dL,dR;
	int xLp,xRp;
	int xL,xR;
	int cnt=0;
	int yWrk,xWrk;
	bool flag_left_long;
	d1=((c.x-a.x)<<16) / DY;
	int DY1=b.y-a.y;
	int DY2=c.y-b.y;
	if(DY1>0){
		d2=((b.x-a.x)<<16) / DY1;
		if (d1<d2) { dL=d1; dR=d2; flag_left_long=1;}
		else { dL=d2; dR=d1; flag_left_long=0;}
		xLp=xRp=(a.x<<16)+(1<<15);
		for(yWrk=a.y; yWrk<b.y; yWrk++){
			xL=xLp>>16; xR=xRp>>16;
			for(xWrk=leftBorder; xWrk< xL; xWrk++) {arrayA[cnt]=0; arrayVx[cnt++]=vMap.SGetAlt(vMap.XCYCL(xWrk), vMap.YCYCL(yWrk));}
			for(/*xWrk=xL*/; xWrk<=xR; xWrk++) {
				arrayA[cnt]=1;
				short vx=vMap.SGetAlt(vMap.XCYCL(xWrk), vMap.YCYCL(yWrk));
				if(vx<vMin)vMin=vx;
				if(vx>vMax)vMax=vx;
				arrayVx[cnt++]=vx;
			}
			for(/*xWrk=leftBorder*/; xWrk<=rightBorder; xWrk++){arrayA[cnt]=0; arrayVx[cnt++]=vMap.SGetAlt(vMap.XCYCL(xWrk), vMap.YCYCL(yWrk));}
			xLp+=dL; xRp+=dR;
		}
	}
	else { //Полигон не имеет первой части
		if(a.x < b.x) {
			xLp=(a.x<<16)+(1<<15);
			dL=((c.x-a.x)<<16) / DY2;
			xRp=(b.x<<16)+(1<<15);
			dR=((c.x-b.x)<<16) / DY2;
		} 
		else {
			xLp=(b.x<<16)+(1<<15);
			dL=((c.x-b.x)<<16) / DY2;
			xRp=(a.x<<16)+(1<<15);
			dR=((c.x-a.x)<<16) / DY2;
		}
		goto loc_begDraw2Part;
	}
	if(DY2>0) {
		d2=((c.x-b.x)<<16) / DY2;
		if(flag_left_long==1) {
			xRp=(b.x<<16)+(1<<15);
			dR=((c.x-b.x)<<16) / DY2;
		}
		else {
			xLp=(b.x<<16)+(1<<15);
			dL=((c.x-b.x)<<16) / DY2;
		}
loc_begDraw2Part:
		for(yWrk=b.y; yWrk<=c.y; yWrk++){
			xL=xLp>>16; xR=xRp>>16;
			for(xWrk=leftBorder; xWrk< xL; xWrk++){ arrayA[cnt]=0; arrayVx[cnt++]=vMap.SGetAlt(vMap.XCYCL(xWrk), vMap.YCYCL(yWrk));}
			for(/*xWrk=xL*/; xWrk<=xR; xWrk++) {
				arrayA[cnt]=1;
				short vx=vMap.SGetAlt(vMap.XCYCL(xWrk), vMap.YCYCL(yWrk));
				if(vx<vMin)vMin=vx;
				if(vx>vMax)vMax=vx;
				arrayVx[cnt++]=vx;
			}
			for(/*xWrk=leftBorder*/; xWrk<=rightBorder; xWrk++){ arrayA[cnt]=0; arrayVx[cnt++]=vMap.SGetAlt(vMap.XCYCL(xWrk), vMap.YCYCL(yWrk));}
			xLp+=dL; xRp+=dR;
		}
	}


	hBegin=vMin+(vMax-vMin)/3;//+(vmax-vmin)/4;

	Vect3f position(leftBorder+sx/2, upBorder+sy/2, (float)hBegin/(float)(1<<VX_FRACTION));
	Vect3f outOrientation;
	int radius;
	if(sx>sy) radius=sx/2;
	else radius=sy/2;
	analyzeTerrain(position, radius, outOrientation);
	//h_begin=round(position.z*(1<<VX_FRACTION));
	float A=outOrientation.x;
	float B=outOrientation.y;
	float C=-outOrientation.z;
	float D=(float)hBegin/(float)(1<<VX_FRACTION);//position.z;

	int xx,yy;
	for(i=0; i<sy; i++){
		yy=vMap.YCYCL(upBorder+i);
		for(j=0; j<sx; j++){
			xx=vMap.XCYCL(leftBorder+j);
			if(arrayVx[i*sx+j]>0){
				float as=(-(A*(float)(j-sx/2) + B*(float)(i-sy/2) + D)/C);
				arrayVx[i*sx+j]-=round(as*(float)(1<<VX_FRACTION));
			}
		}
	}
	basePntArr[0].z=round( (-(A*(basePntArr[0].x-leftBorder-sx/2) + B*(basePntArr[0].y-upBorder-sy/2) + D)/C) *(1<<VX_FRACTION) );
	basePntArr[1].z=round( (-(A*(basePntArr[1].x-leftBorder-sx/2) + B*(basePntArr[1].y-upBorder-sy/2) + D)/C) *(1<<VX_FRACTION) );
	basePntArr[2].z=round( (-(A*(basePntArr[2].x-leftBorder-sx/2) + B*(basePntArr[2].y-upBorder-sy/2) + D)/C) *(1<<VX_FRACTION) );
	for(i=0; i<nElAddPntArr; i++){
		addPntArr[i].z=round( (-(A*(addPntArr[i].x-leftBorder-sx/2) + B*(addPntArr[i].y-upBorder-sy/2) + D)/C) *(1<<VX_FRACTION) );
	}
	if(nElAddPntArr==0){
		oldPoint1=Vect2f(basePntArr[1].x, basePntArr[1].y);
	}
	else if(nElAddPntArr==2){
		oldPoint1=Vect2f(addPntArr[0].x, addPntArr[0].y);
		oldPoint2=Vect2f(addPntArr[1].x, addPntArr[1].y);
	}

	curAngleG=0;
}

sUPoligonN::~sUPoligonN(void)
{
	delete [] arrayVx;
	delete [] arrayA;
}


void sUPoligonN::smoothPoligon(Vect2f a, Vect2f b, Vect2f c)
{
	const int BORDER_P=2;
	Vect2f tmp;
	if(a.y > b.y ) { tmp=a; a=b; b=tmp; }
	if(a.y > c.y ) { tmp=a; a=c; c=tmp; }
	if(b.y > c.y ) { tmp=b; b=c; c=tmp; }

	int upBorder=a.y;
	int downBorder=c.y;
	int leftBorder=a.x;
	if(leftBorder>b.x)leftBorder=b.x;
	if(leftBorder>c.x)leftBorder=c.x;
	int rightBorder=a.x;
	if(rightBorder<b.x)rightBorder=b.x;
	if(rightBorder<c.x)rightBorder=c.x;

	float DY=(c.y-a.y);
	if(round(DY)<1.f) return; //Прерывание если нет полигона
	float d1,d2;
	float dxL,dxR;
	float xLp,xRp;

	int xL,xR;
	int cnt=0;
	int yWrk,xWrk;
	bool flag_left_long;
	d1=(c.x-a.x) / DY;
	float DY1=b.y-a.y;
	float DY2=c.y-b.y;
	if(round(DY1)>0){
		d2=(b.x-a.x) / DY1;
		if (d1<d2) { 
			dxL=d1; dxR=d2; 
			flag_left_long=1;
		}
		else {
			dxL=d2; dxR=d1;
			flag_left_long=0;
		}
		xLp=xRp=a.x;
		for(yWrk=round(a.y); yWrk<round(b.y); yWrk++){
			xL=round(xLp-BORDER_P); xR=round(xRp+BORDER_P);
			if(xL!=xR){
				for(xWrk=xL; xWrk<=xR; xWrk++) {
					int dy,dx,v=0;
					for(dy = -1;dy <= 1; dy++){
						for(dx = -1;dx <= 1; dx++){
							v += vMap.SGetAlt(vMap.XCYCL(xWrk+dx), vMap.YCYCL(yWrk +dy));
						}
					}
					v/=9;
					vMap.SPutAlt(xWrk, yWrk, v);
				}
			}
			xLp+=dxL; xRp+=dxR;
		}
	}
	else { //Полигон не имеет первой части
		if(a.x < b.x) {
			xLp=a.x; dxL=(c.x-a.x) / DY2;
			xRp=b.x; dxR=(c.x-b.x) / DY2;
		} 
		else {
			xLp=b.x; dxL=(c.x-b.x) / DY2;
			xRp=a.x; dxR=(c.x-a.x) / DY2;
		}
		goto loc_begDraw2Part;
	}
	if(round(DY2)>0) {
		d2=(c.x-b.x) / DY2;
		if(flag_left_long==1) {
			xRp=b.x; dxR=(c.x-b.x) / DY2;

		}
		else {
			xLp=b.x; dxL=(c.x-b.x) / DY2;
		}
loc_begDraw2Part:
		for(yWrk=round(b.y); yWrk<=round(c.y); yWrk++){
			xL=round(xLp-BORDER_P); xR=round(xRp+BORDER_P);
			if(xL!=xR){
				for(xWrk=xL; xWrk<=xR; xWrk++) {
					int dy,dx,v=0;
					for(dy = -1;dy <= 1; dy++){
						for(dx = -1;dx <= 1; dx++){
							v += vMap.SGetAlt(vMap.XCYCL(xWrk+dx), vMap.YCYCL(yWrk +dy));
						}
					}
					v/=9;
					vMap.SPutAlt(xWrk, yWrk, v);
				}
			}
			xLp+=dxL; xRp+=dxR;
		}
	}
	vMap.recalcArea2Grid(vMap.XCYCL(leftBorder), vMap.YCYCL(upBorder), vMap.XCYCL(leftBorder+sx), vMap.YCYCL(upBorder+sy) );
	vMap.regRender(vMap.XCYCL(leftBorder), vMap.YCYCL(upBorder), vMap.XCYCL(leftBorder+sx), vMap.YCYCL(upBorder+sy) );
}

void sUPoligonN::buldCPArr(void)
{
	nElCritPntArr=0;
	int iPrev, iCur, iNext;
	for(iPrev=nElAllPntArr-1, iCur=0, iNext=1; iCur<nElAllPntArr;  iCur++, iPrev++, iNext++){
		if(iNext>=nElAllPntArr) iNext=0;
		if(iPrev>=nElAllPntArr) iPrev=0;
		if(allPntArr[iCur].y < allPntArr[iPrev].y && allPntArr[iCur].y < allPntArr[iNext].y){
			critPntArr[nElCritPntArr++]=iCur;
		}
	}
	int i, j;
	for(i=0; i<nElCritPntArr; i++){
		for(j=i+1; j<nElCritPntArr; j++){
			if(allPntArr[critPntArr[i]].y > allPntArr[critPntArr[j]].y) {
				int t;
				t=critPntArr[i];
				critPntArr[i]=critPntArr[j];
				critPntArr[j]=t;
			}
		}
	}
}

bool sUPoligonN::quant(void){
	curAngleG+=3;
	if(!quant(curAngleG)) return 0;
	if(curAngleG>35) return 0;
	else return 1;
}

bool sUPoligonN::quant(float angle_grad)
{
	bool flag_abort=0;
	int xx,yy;
	unsigned int i,j;
	for(i=0; i<sy; i++){
		yy=vMap.YCYCL(upBorder+i);
		for(j=0; j<sx; j++){
			xx=vMap.XCYCL(leftBorder+j);
			// !Оптимизировать
			if(arrayA[i*sx+j]==1){
				//vMap.SPutAltDam(xx,yy,0);
				//vMap.SPutAltGeo(xx,yy,0);
			}
			else if( (arrayA[i*sx+(j+1)%sx]==1) || ( j>0 && arrayA[i*sx+(j-1)%sx]==1) ){
				//vMap.SPutAltDam(xx,yy,0);
				//vMap.SPutAltGeo(xx,yy,0);
			}
		}
	}

	int o_x=basePntArr[0].x;
	int o_y=basePntArr[0].y;
	int o_z=basePntArr[0].z>>VX_FRACTION;//arrayVx[(o_y-upBorder)*sx+(o_x-leftBorder)]>>VX_FRACTION;
	Vect3f qVect(basePntArr[2].x-o_x, basePntArr[2].y-o_y, 0);//(basePntArr[1].z>>VX_FRACTION)-o_z);
	//Определение справа или слева точка от вектора кватерниона
	Vect3f v_o(basePntArr[2].x-basePntArr[0].x, basePntArr[2].y-basePntArr[0].y, 0.f);
	Vect3f v_2(basePntArr[1].x-basePntArr[0].x, basePntArr[1].y-basePntArr[0].y, 0.f);
	v_o.normalize(1.f);
	v_2.normalize(1.f);
	Vect3f result;
	result.cross(v_o, v_2);
	if(result.z<0) angle_grad=-angle_grad;

	//static float planeAngle=0.5f*M_PI/180.f;//0.f;
	//planeAngle-=0.5f*M_PI/180.f;
	float planeAngle=angle_grad*M_PI/180.f;
	QuatF wQuat(planeAngle, qVect);

/*//Проверка правильности поворота полигона - кватернионом
	for(i=0; i<sy; i++){
		yy=vMap.YCYCL(upBorder+i);
		for(j=0; j<sx; j++){
			xx=vMap.XCYCL(leftBorder+j);
			if(arrayVx[i*sx+j]!=SUPOLIGON_NON_POLIGON){
				float curz=(float)arrayVx[i*sx+j]/(1<<VX_FRACTION);
				Vect3f in(xx-o_x, yy-o_y, curz-o_z);
				Vect3f out(0,0,0);
				wQuat.xform(in, out);
				int wx=vMap.XCYCL(round(out.x)+o_x);
				int wy=vMap.YCYCL(round(out.y)+o_y);
				int wz=round(out.z*(1<<VX_FRACTION))+(o_z<<VX_FRACTION);
				wz+=100<<VX_FRACTION;
				//int vv=vMap.SGetAlt(wx,wy);
				//if (vv) vMap.SPutAltDam(wx,wy,(wz+vv)>>1);
				//else vMap.SPutAltDam(wx,wy,wz);
				vMap.SPutAltDam(wx,wy,wz);
				//vMap.SPutAltGeo(wx,wy,wz);
			}
		}
	}
*/
	Vect3f in(0,0,0);
	Vect3f out(0,0,0);
	Vect3f a(0,0,0);
	Vect3f b(0,0,0);
	Vect3f c(0,0,0);
	float curz;
	curz=(float)basePntArr[0].z/(1<<VX_FRACTION);
	in.set(basePntArr[0].x-o_x, basePntArr[0].y-o_y, curz-o_z);
	wQuat.xform(in, out);
	a.x=vMap.XCYCL(round(out.x)+o_x);
	a.y=vMap.YCYCL(round(out.y)+o_y);
	//a.z=round(out.z*(1<<VX_FRACTION))+(o_z<<VX_FRACTION);
	a.z=out.z+o_z;

	curz=(float)basePntArr[1].z/(1<<VX_FRACTION);
	in.set(basePntArr[1].x-o_x, basePntArr[1].y-o_y, curz-o_z);
	wQuat.xform(in, out);
	b.x=vMap.XCYCL(round(out.x)+o_x);
	b.y=vMap.YCYCL(round(out.y)+o_y);
	//b.z=round(out.z*(1<<VX_FRACTION))+(o_z<<VX_FRACTION);
	b.z=out.z+o_z;

	Vect2f buckupB=b;

	curz=(float)basePntArr[2].z/(1<<VX_FRACTION);
	in.set(basePntArr[2].x-o_x, basePntArr[2].y-o_y, curz-o_z);
	wQuat.xform(in, out);
	c.x=vMap.XCYCL(round(out.x)+o_x);
	c.y=vMap.YCYCL(round(out.y)+o_y);
	//c.z=round(out.z*(1<<VX_FRACTION))+(o_z<<VX_FRACTION);
	c.z=out.z+o_z;

	allPntArr[0]=a;
	for(i=0; i<nElAddPntArr; i++){
		curz=(float)addPntArr[i].z/(1<<VX_FRACTION);
		in.set(addPntArr[i].x-o_x, addPntArr[i].y-o_y, curz-o_z);
		wQuat.xform(in, out);
		allPntArr[i+1].x=vMap.XCYCL(round(out.x)+o_x);
		allPntArr[i+1].y=vMap.YCYCL(round(out.y)+o_y);
		//allPntArr[i+1].z=round(out.z*(1<<VX_FRACTION))+(o_z<<VX_FRACTION);
		allPntArr[i+1].z=out.z+o_z;
	}
	allPntArr[nElAddPntArr+1]=c; //последний элемент
	nElAllPntArr=nElAddPntArr+2;
	buldCPArr();
	clearAET();
	
	//Построение плоскости по 3-м точкам
	//float cA = a.y*(b.z-c.z) + b.y*(c.z-a.z) + c.y*(a.z-b.z);
	//float cB = a.z*(b.x-c.x) + b.z*(c.x-a.x) + c.z*(a.x-b.x);
	//float cC = a.x*(b.y-c.y) + b.x*(c.y-a.y) + c.x*(a.y-b.y);
	//float cD = -(a.x*(b.y*c.z-c.y*b.z) + b.x*(c.y*a.z-a.y*c.z) + c.x*(a.y*b.z-b.y*a.z) );
	float cA=(b.y-a.y)*(c.z-a.z)-(c.y-a.y)*(b.z-a.z);
	float cB=(b.z-a.z)*(c.x-a.x)-(c.z-a.z)*(b.x-a.x);
	float cC=(b.x-a.x)*(c.y-a.y)-(c.x-a.x)*(b.y-a.y);
	float cD=-cA*a.x-cB*a.y-cC*a.z;

	float a_u=basePntArr[0].x-leftBorder;
	float a_v=basePntArr[0].y-upBorder;
	float b_u=basePntArr[1].x-leftBorder;
	float b_v=basePntArr[1].y-upBorder;
	float c_u=basePntArr[2].x-leftBorder;
	float c_v=basePntArr[2].y-upBorder;
	Vect3f tmp;
	float tmpu, tmpv;
	int k=0;
	if(a.y > b.y ) { tmp=a; a=b; b=tmp; tmpu=a_u; a_u=b_u; b_u=tmpu; tmpv=a_v; a_v=b_v; b_v=tmpv;}
	if(a.y > c.y ) { tmp=a; a=c; c=tmp; tmpu=a_u; a_u=c_u; c_u=tmpu; tmpv=a_v; a_v=c_v; c_v=tmpv;}
	if(b.y > c.y ) { tmp=b; b=c; c=tmp; tmpu=b_u; b_u=c_u; c_u=tmpu; tmpv=b_v; b_v=c_v; c_v=tmpv;}

	float DY=(c.y-a.y);
	if(round(DY)<1.f) return 0; //Прерывание если нет полигона
	float d1,d2;
	float dxL,dxR;
	float xLp,xRp;
	float zLp,zRp;
	float dzL,dzR;

	float uLp,uRp;
	float duL, duR;
	float vLp,vRp;
	float dvL, dvR;

	int xL,xR;
	int cnt=0;
	int yWrk,xWrk;
	bool flag_left_long;
	d1=(c.x-a.x) / DY;
	float DY1=b.y-a.y;
	float DY2=c.y-b.y;
	int curIdxCPArr=0;
	if(round(DY1)>0){
		d2=(b.x-a.x) / DY1;
		if (d1<d2) { 
			dxL=d1; dxR=d2; 
			flag_left_long=1;
			dzL=(round(c.z)-round(a.z)) / DY;
			dzR=(round(b.z)-round(a.z)) / DY1;
			duL=(c_u-a_u) / DY;
			duR=(b_u-a_u) / DY1;
			dvL=(c_v-a_v) / DY;
			dvR=(b_v-a_v) / DY1;
		}
		else {
			dxL=d2; dxR=d1;
			flag_left_long=0;
			dzL=(b.z-a.z) / DY1;
			dzR=(c.z-a.z) / DY;
			duL=(b_u-a_u) / DY1;
			duR=(c_u-a_u) / DY;
			dvL=(b_v-a_v) / DY1;
			dvR=(c_v-a_v) / DY;
		}
		xLp=xRp=a.x;
		zLp=zRp=a.z;
		uLp=uRp=a_u;
		vLp=vRp=a_v;
		for(yWrk=round(a.y); yWrk<round(b.y); yWrk++){
			for(;curIdxCPArr<nElCritPntArr && yWrk>=allPntArr[critPntArr[curIdxCPArr]].y; curIdxCPArr++){
				char idxTo;
				idxTo=critPntArr[curIdxCPArr]-1;
				if( idxTo<0) idxTo=nElAllPntArr-1;
				addAETRecord(critPntArr[curIdxCPArr], idxTo, -1);
				idxTo=critPntArr[curIdxCPArr]+1;
				if( idxTo>=nElAllPntArr) idxTo=0;
				addAETRecord(critPntArr[curIdxCPArr], idxTo, +1);
			}
			xL=round(xLp); xR=round(xRp);
			int offY=vMap.offsetBuf(0, vMap.YCYCL(yWrk));
			int offGY=vMap.offsetGBuf(0, vMap.YCYCL(yWrk)>>kmGrid);
			if(xL!=xR){
				float zCur=zLp;
				float dzCur=(round(zRp)-round(zLp))/(float)(xR-xL);
				float uCur=uLp;
				float duCur=(uRp-uLp)/(float)(xR-xL);
				float vCur=vLp;
				float dvCur=(vRp-vLp)/(float)(xR-xL);
				xWrk=xL;
				for(i=0; i<nElAET; i+=2){
					for(; xWrk<(AET[i].x>>16); xWrk++){
						zCur+=dzCur;
						uCur+=duCur;
						vCur+=dvCur;
					}
					//for(xWrk=xL; xWrk<=xR; xWrk++)
					for(; xWrk<=(AET[i+1].x>>16); xWrk++) {
						int offsetG=(vMap.XCYCL(xWrk)>>kmGrid) + offGY;
						if( (vMap.GABuf[offsetG]&GRIDAT_BUILDING) != 0){
							vMap.GABuf[offsetG]|=GRIDAT_BASE_OF_BUILDING_CORRUPT;
						}
						/*else*/ {
							//short addVx=arrayVx[(vMap.XCYCL(xWrk)-leftBorder) + sx*(vMap.YCYCL(yWrk)-upBorder)];
							short addVx;
							float u0=floor(uCur);
							float ddu=uCur-u0;
							int u=round(u0);
							float v0=floor(vCur);
							float ddv=vCur-v0;
							int v=round(v0);
							addVx=round( (arrayVx[u+v*sx]*(1-ddu)+arrayVx[u+1+v*sx]*ddu) *(1-ddv) +
										 (arrayVx[u+(v+1)*sx]*(1-ddu)+arrayVx[u+1+(v+1)*sx]*ddu) *ddv );
							//addVx=arrayVx[round(uCur)+round(vCur)*sx];
							//addVx=0;
							//vMap.SPutAltDam(vMap.XCYCL(xWrk), vMap.YCYCL(yWrk), round(zCur*(1<<VX_FRACTION)+addVx) );
							unsigned short VVV=round( (-(cD+cA*xWrk+cB*yWrk)/cC)*(1<<VX_FRACTION)+addVx);
							if(VVV>MAX_VX_HEIGHT){ VVV=MAX_VX_HEIGHT; flag_abort=1; }
							//vMap.SPutAlt(vMap.XCYCL(xWrk), vMap.YCYCL(yWrk), VVV );
							SPutAlt4G(offY+vMap.XCYCL(xWrk), VVV);
							//arrayVx[cnt++]=vx;
						}
						//zCur+=dzCur;
						uCur+=duCur;
						vCur+=dvCur;
					}
				}
			}
			for(i=0; i<nElAET; i++){
				AET[i].x+=AET[i].dx;
				if(yWrk>=round(allPntArr[AET[i].idxTo].y)){
					char idx=AET[i].idxTo+AET[i].dir;
					if(idx<0)idx=nElAllPntArr-1;
					if(idx>=nElAllPntArr)idx=0;
					if(round(allPntArr[idx].y)>yWrk) { //удаляем и добавляем ребро
						char idxFrom=AET[i].idxTo;
						char dir=AET[i].dir;
						//Соптимизировать
						delAETRecord(i);
						addAETRecord(idxFrom, idx, dir);

					}
					else{ //только удаляем ребро
						delAETRecord(i);
						i--;//компенсация удаления
					}
				}
			}
			xLp+=dxL; xRp+=dxR;
			zLp+=dzL; zRp+=dzR;
			uLp+=duL; uRp+=duR;
			vLp+=dvL; vRp+=dvR;
		}
	}
	else { //Полигон не имеет первой части
		if(a.x < b.x) {
			xLp=a.x; dxL=(c.x-a.x) / DY2;
			xRp=b.x; dxR=(c.x-b.x) / DY2;

			zLp=a.z; dzL=(c.z-a.z) / DY2;
			zRp=b.z; dzR=(c.z-b.z) / DY2;

			uLp=a_u; duL=(c_u-a_u) / DY2;
			uRp=b_u; duR=(c_u-b_u) / DY2;

			vLp=a_v; dvL=(c_v-a_v) / DY2;
			vRp=b_v; dvR=(c_v-b_v) / DY2;
		} 
		else {
			xLp=b.x; dxL=(c.x-b.x) / DY2;
			xRp=a.x; dxR=(c.x-a.x) / DY2;

			zLp=b.z; dzL=(c.z-b.z) / DY2;
			zRp=a.z; dzR=(c.z-a.z) / DY2;

			uLp=b_u; duL=(c_u-b_u) / DY2;
			uRp=a_u; duR=(c_u-a_u) / DY2;

			vLp=b_v; dvL=(c_v-b_v) / DY2;
			vRp=a_v; dvR=(c_v-a_v) / DY2;
		}
		goto loc_begDraw2Part;
	}
	if(round(DY2)>0) {
		d2=(c.x-b.x) / DY2;
		if(flag_left_long==1) {
			xRp=b.x; dxR=(c.x-b.x) / DY2;
			zRp=b.z; dzR=(c.z-b.z) / DY2;
			uRp=b_u; duR=(c_u-b_u) / DY2;
			vRp=b_v; dvR=(c_v-b_v) / DY2;

		}
		else {
			xLp=b.x; dxL=(c.x-b.x) / DY2;
			zLp=b.z; dzL=(c.z-b.z) / DY2;
			uLp=b_u; duL=(c_u-b_u) / DY2;
			vLp=b_v; dvL=(c_v-b_v) / DY2;
		}
loc_begDraw2Part:
		for(yWrk=round(b.y); yWrk<=round(c.y); yWrk++){
			for(;curIdxCPArr<nElCritPntArr && yWrk>=allPntArr[critPntArr[curIdxCPArr]].y; curIdxCPArr++){
				char idxTo;
				idxTo=critPntArr[curIdxCPArr]-1;
				if( idxTo<0) idxTo=nElAllPntArr-1;
				addAETRecord(critPntArr[curIdxCPArr], idxTo, -1);
				idxTo=critPntArr[curIdxCPArr]+1;
				if( idxTo>=nElAllPntArr) idxTo=0;
				addAETRecord(critPntArr[curIdxCPArr], idxTo, +1);
			}
			xL=round(xLp); xR=round(xRp);
			int offY=vMap.offsetBuf(0, vMap.YCYCL(yWrk));
			int offGY=vMap.offsetGBuf(0, vMap.YCYCL(yWrk)>>kmGrid);
			if(xL!=xR){
				float zCur=zLp;
				float dzCur=(zRp-zLp)/(float)(xR-xL);
				float uCur=uLp;
				float duCur=(uRp-uLp)/(float)(xR-xL);
				float vCur=vLp;
				float dvCur=(vRp-vLp)/(float)(xR-xL);
				xWrk=xL;
				for(i=0; i<nElAET; i+=2){
					for(; xWrk<(AET[i].x>>16); xWrk++){
						zCur+=dzCur;
						uCur+=duCur;
						vCur+=dvCur;
					}
					//for(xWrk=xL; xWrk<=xR; xWrk++)
					for(; xWrk<=(AET[i+1].x>>16); xWrk++) {
						int offsetG=(vMap.XCYCL(xWrk)>>kmGrid) + offGY;
						if( (vMap.GABuf[offsetG]&GRIDAT_BUILDING) != 0){
							vMap.GABuf[offsetG]|=GRIDAT_BASE_OF_BUILDING_CORRUPT;
						}
						/*else*/ {
							//short addVx=arrayVx[(vMap.XCYCL(xWrk)-leftBorder) + sx*(vMap.YCYCL(yWrk)-upBorder)];
							short addVx;
							float u0=floor(uCur);
							float ddu=uCur-u0;
							int u=round(u0);
							float v0=floor(vCur);
							float ddv=vCur-v0;
							int v=round(v0);
							addVx=round( (arrayVx[u+v*sx]*(1-ddu)+arrayVx[u+1+v*sx]*ddu) *(1-ddv) +
										 (arrayVx[u+(v+1)*sx]*(1-ddu)+arrayVx[u+1+(v+1)*sx]*ddu) *ddv );
							//addVx=arrayVx[round(uCur)+round(vCur)*sx];
							//addVx=0;
							//vMap.SPutAltDam(vMap.XCYCL(xWrk), vMap.YCYCL(yWrk), round(zCur*(1<<VX_FRACTION)+addVx) );
							///////vMap.SPutAlt(vMap.XCYCL(xWrk), vMap.YCYCL(yWrk), round( (-(cD+cA*xWrk+cB*yWrk)/cC)*(1<<VX_FRACTION)+addVx) );
							unsigned short VVV=round( (-(cD+cA*xWrk+cB*yWrk)/cC)*(1<<VX_FRACTION)+addVx);
							if(VVV>MAX_VX_HEIGHT){ VVV=MAX_VX_HEIGHT; flag_abort=1; }
							//vMap.SPutAlt(vMap.XCYCL(xWrk), vMap.YCYCL(yWrk), VVV );
							SPutAlt4G(offY+vMap.XCYCL(xWrk), VVV);
							//arrayVx[cnt++]=vx;
						}
						//zCur+=dzCur;
						uCur+=duCur;
						vCur+=dvCur;
					}
				}
			}
			for(i=0; i<nElAET; i++){
				AET[i].x+=AET[i].dx;
				if(yWrk>=round(allPntArr[AET[i].idxTo].y)){
					char idx=AET[i].idxTo+AET[i].dir;
					if(idx<0)idx=nElAllPntArr-1;
					if(idx>=nElAllPntArr)idx=0;
					if(round(allPntArr[idx].y)>yWrk) { //удаляем и добавляем ребро
						char idxFrom=AET[i].idxTo;
						char dir=AET[i].dir;
						//Соптимизировать
						delAETRecord(i);
						addAETRecord(idxFrom, idx, dir);

					}
					else{ //только удаляем ребро
						delAETRecord(i);
						i--;//компенсация удаления
					}
				}
			}

			xLp+=dxL; xRp+=dxR;
			zLp+=dzL; zRp+=dzR;
			uLp+=duL; uRp+=duR;
			vLp+=dvL; vRp+=dvR;
		}
	}

	if(nElAddPntArr==0){
		{
			Vect2f as(basePntArr[0].x, basePntArr[0].y);
			//Vect2f bs(basePntArr[1].x, basePntArr[1].y);
			Vect2f cs(buckupB.x, buckupB.y);
			smoothPoligon(as, oldPoint1, cs);
		}
		{
			//Vect2f as(basePntArr[1].x, basePntArr[1].y);
			Vect2f bs(basePntArr[2].x, basePntArr[2].y);
			Vect2f cs(buckupB.x, buckupB.y);
			smoothPoligon(oldPoint1, bs,cs);
		}
	}
	else {
		if(nElAddPntArr==2){
			{
				Vect2f as(basePntArr[0].x, basePntArr[0].y);
				//Vect2f bs(addPntArr[0].x, addPntArr[0].y);
				Vect2f cs(allPntArr[1].x, allPntArr[1].y);
				//smoothPoligon(as, bs,cs);
				smoothPoligon(as, oldPoint1, cs);
			}
			{
				Vect2f as(basePntArr[2].x, basePntArr[2].y);
				Vect2f bs(allPntArr[2].x, allPntArr[2].y);
				//Vect2f cs(addPntArr[1].x, addPntArr[1].y);
				smoothPoligon(as, bs, oldPoint2);
			}
			{
				//Vect2f as(addPntArr[0].x, addPntArr[0].y);
				Vect2f bs(allPntArr[1].x, allPntArr[1].y);
				//Vect2f cs(addPntArr[1].x, addPntArr[1].y);
				smoothPoligon(oldPoint1, bs, oldPoint2);
			}
			{
				Vect2f as(allPntArr[1].x, allPntArr[1].y);
				//Vect2f bs(addPntArr[1].x, addPntArr[1].y);
				Vect2f cs(allPntArr[2].x, allPntArr[2].y);
				smoothPoligon(as, oldPoint2, cs);
			}
		}
	}
	if(nElAddPntArr==0){
		oldPoint1=buckupB;//Vect2f(allPntArr[1].x, basePntArr[1].y);
	}
	else if(nElAddPntArr==2){
		oldPoint1=Vect2f(allPntArr[1].x, allPntArr[1].y);
		oldPoint2=Vect2f(allPntArr[2].x, allPntArr[2].y);
	}

	vMap.recalcArea2Grid(vMap.XCYCL(leftBorder), vMap.YCYCL(upBorder), vMap.XCYCL(leftBorder+sx), vMap.YCYCL(upBorder+sy) );
	vMap.regRender(vMap.XCYCL(leftBorder), vMap.YCYCL(upBorder), vMap.XCYCL(leftBorder+sx), vMap.YCYCL(upBorder+sy) );
	if(flag_abort) return 0;
	else return 1;
}




///////////////////////////////////////////////////////////////////////////////////////////////
//                   Появление моделей из земли
///////////////////////////////////////////////////////////////////////////////////////////////

void meshM2VM::releaseBuf(void)
{
	if(vrtx!=0){
		delete [] vrtx;
		vrtx=0;
	}
	numVrtx=0;
	if(face!=0){
		delete [] face;
		face=0;
	}
	numFace=0;

	if(fname3DS!=0) free(fname3DS);
}

bool meshM2VM::load(const char* fname)//, int numMesh)
{
	releaseBuf();

#ifdef _PERIMETER_

/*	char txtBuf[MAX_PATH];
	strncpy(txtBuf, fname, MAX_PATH);
	int  pos=strlen(txtBuf);
	txtBuf[pos-3]='M'; txtBuf[pos-2]='3'; txtBuf[pos-1]='D';
	pos-=4;
	while(pos){
		if( (txtBuf[pos]!='\\') && (txtBuf[pos]!='/') ) pos--;
		else break;
	}
*/

	vector<sPolygon> poligonArr;
	vector<Vect3f> pointArr;
	GetAllTriangle(fname, pointArr, poligonArr);

	fname3DS=strdup(fname);

	numVrtx=pointArr.size();
	vrtx=new vrtxM2VM[numVrtx];
	numFace=poligonArr.size();
	face=new faceM2VM[numFace];

	int indx;
	for(indx=0; indx< numFace; indx++){
		face[indx].v1=poligonArr[indx].p1;
		face[indx].v2=poligonArr[indx].p2;
		face[indx].v3=poligonArr[indx].p3;
	}
	for(indx=0; indx< numVrtx; indx++){
		vrtx[indx].xyz.x=pointArr[indx].x;
		vrtx[indx].xyz.y=-pointArr[indx].y;
		vrtx[indx].xyz.z=pointArr[indx].z;
	}

/*
	const ErrRec3ds *ErrList=ReturnErrorList3ds();
	file3ds *inFile=NULL;
	inFile=OpenFile3ds(fname, "rb");

	if(inFile==0) { 
		CLEAR_ERROR; CloseAllFiles3ds(); 
		XBuffer msg; msg < "3ds file not found " < fname; 
		ErrH.Abort(msg); return 0; 
	}

	fname3DS=strdup(fname);

	database3ds	*inDB=NULL;
	InitDatabase3ds(&inDB);
	CreateDatabase3ds(inFile, inDB);

	mesh3ds *inMesh=NULL;
	unsigned int numMeshs=GetMeshCount3ds(inDB);
	unsigned int meshID;
	for(meshID=0; meshID<1; meshID++){//numMeshs//Пока читаем только 1-й меш
		GetMeshByIndex3ds(inDB, meshID, &inMesh);

		strcpy(name, inMesh->name);
		numVrtx=inMesh->nvertices;
		vrtx=new vrtxM2VM[numVrtx];
		numFace=inMesh->nfaces;
		face=new faceM2VM[numFace];

		int indx;
		for(indx=0; indx< numFace; indx++){
			face[indx].v1=inMesh->facearray[indx].v1;
			//face[indx].v2=inMesh->facearray[indx].v2;
			//face[indx].v3=inMesh->facearray[indx].v3;
			face[indx].v2=inMesh->facearray[indx].v3;
			face[indx].v3=inMesh->facearray[indx].v2;
		}
		for(indx=0; indx< numVrtx; indx++){
			vrtx[indx].xyz.x=inMesh->vertexarray[indx].x;
			vrtx[indx].xyz.y=-inMesh->vertexarray[indx].y;
			vrtx[indx].xyz.z=inMesh->vertexarray[indx].z;
		}
	}
	RelMeshObj3ds(&inMesh);

	ReleaseDatabase3ds(&inDB);
	CloseFile3ds(inFile);

	//calcNormals();
*/
#endif _PERIMETER_

	return 1;
}

void meshM2VM::rotateAndScaling(int XA, int YA, int ZA, float kX, float kY, float kZ)
{
	Mat3f A_shape;
	//A_shape.set(Vect3f(0.5,-0.5,-0.5), Vect3f::ZERO);
	A_shape.set(Vect3f(kX, kY, -kZ), Vect3f::ZERO);

	A_shape = Mat3f((float)XA*(M_PI/180.0),X_AXIS)*A_shape;
	A_shape = Mat3f((float)-YA*(M_PI/180.0),Y_AXIS)*A_shape;
	A_shape = Mat3f((float)ZA*(M_PI/180.0),Z_AXIS)*A_shape;

	matrix=A_shape;
/*	matrix.xx=A_shape[0][0];
	matrix.xy=A_shape[0][1];
	matrix.xz=A_shape[0][2];

	matrix.yx=A_shape[1][0];
	matrix.yy=A_shape[1][1];
	matrix.yz=A_shape[1][2];

	matrix.zx=A_shape[2][0];
	matrix.zy=A_shape[2][1];
	matrix.zz=A_shape[2][2];
*/
}
void meshM2VM::rotateAndScaling(Vect3f& orientation, Vect3f& scaling)
{
	Mat3f A_shape;
	//A_shape.set(Vect3f(0.5,-0.5,-0.5), Vect3f::ZERO);
	A_shape.set(Vect3f(scaling.x, scaling.y, -scaling.z), Vect3f::ZERO);

	A_shape = Mat3f(orientation.x,X_AXIS)*A_shape;
	A_shape = Mat3f(-orientation.y,Y_AXIS)*A_shape;
	A_shape = Mat3f(orientation.z,Z_AXIS)*A_shape;

	matrix=A_shape;
}

Vect2s meshM2VM::calcSizeXY(void)
{
	const int MIN_BORDER=4;
	int i;
	int minX=vMap.H_SIZE;
	int minY=vMap.V_SIZE;
	int maxX=0;
	int maxY=0;
	for (i = 0; i < numVrtx; i++) {

		Vect3f mm;
		matrix.xform(vrtx[i].xyz, mm);

		//орто проекция
		//vrtx[i].sx=mm.x;
		//vrtx[i].sy=mm.y;
		//vrtx[i].z1=mm.z;
		if(mm.x < minX) minX=mm.x; if(mm.x > maxX) maxX=mm.x;
		if(mm.y < minY) minY=mm.y; if(mm.y > maxY) maxY=mm.y;
	}

////////////////////////////////////////////////
	int sizeX=maxX-minX + MIN_BORDER;
	int sizeY=maxY-minY + MIN_BORDER;
	if(sizeX < 0) sizeX=0;
	if(sizeX > SHRT_MAX) sizeX=SHRT_MAX;
	if(sizeY < 0) sizeY=0;
	if(sizeY > SHRT_MAX) sizeY=SHRT_MAX;
	
	return Vect2s(sizeX, sizeY);
}


//#define roundFInt(a) (((a)+(1<<15))>>16)
//#define wholeFInt(a) ((a)>>16)
#define roundFIntF0(a) ((a)>=0 ? (a)>>16 : (-((-a)>>16)) )
#define cvrtFIntF8(a) ((a+(1<<7))>>8)
#define ceilFIntF16(a) ((a)>=0 ? (((a)+0xffFF)&0xFFff0000) : (-((-(a)+0xffFF)&0xFFff0000)) )
#define ceilFIntF0(a) ((a)>=0 ? (((a)+0xffFF)>>16) : (-((-(a)+0xffFF)>>16)) )
bool meshM2VM::put2KF(int quality, short * KFArr, int sxKF, int syKF, bool flag_inverse, short addH)
{
    memset(KFArr, 0, sxKF*syKF* sizeof(KFArr[0]));
	//return 1;

	const int SCALING4PUT2EARCH=quality;
	int i;
	int minX=vMap.H_SIZE;
	int minY=vMap.V_SIZE;
	int maxX=0;
	int maxY=0;
	int minZ=0x7fFFffFF;// максимальное положительное число
	int maxZ=0x80000001;// самое маленькое отрицательное число
	for (i = 0; i < numVrtx; i++) {
		Vect3f mm;
		matrix.xform(vrtx[i].xyz, mm);

		mm.x*=SCALING4PUT2EARCH;
		mm.y*=SCALING4PUT2EARCH;

		//орто проекция
		vrtx[i].sx=mm.x;
		vrtx[i].sy=mm.y;
		vrtx[i].z1=mm.z;
		if(mm.x < minX) minX=mm.x; if(mm.x > maxX) maxX=mm.x;
		if(mm.y < minY) minY=mm.y; if(mm.y > maxY) maxY=mm.y;
		if(mm.z < minZ) minZ=mm.z; if(mm.z > maxZ) maxZ=mm.z;
	}
	for (i = 0; i < numVrtx; i++){
		vrtx[i].z1=(maxZ-minZ)-(vrtx[i].z1-minZ);
		vrtx[i].x=round((vrtx[i].sx-minX)*(1<<16));
		vrtx[i].y=round((vrtx[i].sy-minY)*(1<<16));
		vrtx[i].z=round(vrtx[i].z1*(1<<16));
	}

	if( ((maxX-minX)<=0) || ((maxY-minY)<=0) ) return 0;
////////////////////////////////////////////////
	int sizeX=maxX-minX;
	int sizeY=maxY-minY;
	xassert( (sizeX < 1024) && (sizeY < 1024) );


	int * zBuffer;
	zBuffer=new int[sizeX*sizeY];
    memset(zBuffer, 0, sizeX*sizeY * sizeof(int));

	for (i = 0; i < numFace; i++) {
		vrtxM2VM *a, *b, *c;
		a=&vrtx[face[i].v1];
		b=&vrtx[face[i].v2];
		c=&vrtx[face[i].v3];

		// отсортируем вершины грани по sy
		if (a->y > b->y) swap(a,b);
		if (a->y > c->y) swap(a,c);
		if (b->y > c->y) swap(b,c);


		int current_sx, current_sy;

		//float
		int tmp, k, x_start, x_end;
		int dx_start, dx_end, dz1_start, dz1_end;
		int z1_start, z1_end;
		int x, z1, dz1;

		int length;


		// посчитаем du/dsx, dv/dsx, d(1/z)/dsx
		// считаем по самой длинной линии (т.е. проходящей через вершину B)
		int divisor;
		divisor=(c->y - a->y);
		if(roundFIntF0(divisor)) k = ((__int64)(b->y - a->y)<<8) / divisor;// F8
		else k=0;
		x_start = a->x + (cvrtFIntF8(c->x - a->x))*(k);
		z1_start = a->z + (cvrtFIntF8(c->z - a->z))*(k);
		x_end = b->x;
		z1_end = b->z;
		divisor= x_start - x_end;
		if(roundFIntF0(divisor)) dz1 = (((__int64)(z1_start - z1_end)<<16)/divisor);
		else dz1=0;
		int tdivisor=divisor;
		int tz1_start=z1_start;
		int tz1_end=z1_end;

		x_start = a->x;
		z1_start = a->z;
		divisor=(c->y - a->y);
		if(roundFIntF0(divisor)){
			dx_start = ((__int64)(c->x - a->x)<<16) / divisor; 
			dz1_start = ((__int64)(c->z - a->z)<<16) / divisor; 
		}
		else { dx_start=0; dz1_start=0;}
#ifdef SUBPIXEL
		tmp = ceilFIntF16(a->y) - a->y;
		x_start += (dx_start>>8) * (tmp>>8);
		z1_start += (dz1_start>>8) * (tmp>>8);
#endif

		if (ceilFIntF16(b->y) > ceilFIntF16(a->y)) {
			tmp = ceilFIntF16(a->y) - (a->y);
			x_end = a->x;
			z1_end = a->z;
			divisor=b->y - a->y;
			if(roundFIntF0(divisor)){ 
				dx_end = ((__int64)(b->x - a->x)<<16) / divisor;
                dz1_end = ((__int64)(b->z - a->z)<<16) / divisor;
			}
			else {dx_end =0; dz1_end =0; }
		} else {
			tmp = ceilFIntF16(b->y) - b->y;
			x_end = b->x;
			z1_end = b->z;
			divisor=c->y - b->y;
			if(roundFIntF0(divisor)){
				dx_end = ((__int64)(c->x - b->x)<<16) / divisor;
                dz1_end = ((__int64)(c->z - b->z)<<16) / divisor;
			}
			else{ dx_end=0; dz1_end=0; }
		}
#ifdef SUBPIXEL
		x_end += (dx_end>>8) * (tmp>>8); //Норма
		z1_end += (dz1_end>>8) * (tmp>>8); //Норма
#endif

////////////////////////////////
		// построчная отрисовка грани
		for (current_sy = ceilFIntF0(a->y); current_sy <= ceilFIntF0(c->y); current_sy++) {
			if((current_sy) >= sizeY ) break;
			if (current_sy == ceilFIntF0(b->y)) {
				x_end = b->x;
				z1_end = b->z;
				divisor=(c->y - b->y);
				if(roundFIntF0(divisor)){
					dx_end=((__int64)(c->x - b->x)<<16)/divisor;
					dz1_end=((__int64)(c->z - b->z)<<16)/divisor;
				}
				else { dx_end=0; dz1_end=0; }
#ifdef SUBPIXEL
				tmp = ceilFIntF16(b->y) - b->y;
				x_end += (dx_end>>8) * (tmp>>8);
				z1_end += (dz1_end>>8) * (tmp>>8);
#endif
			}

			// x_start должен находиться левее x_end
			if (x_start > x_end) {
			  x = x_end;
			  z1 = z1_end;
			  length = ceilFIntF0(x_start) - ceilFIntF0(x_end);
			} else {
			  x = x_start;
			  z1 = z1_start;
			  length = ceilFIntF0(x_end) - ceilFIntF0(x_start);
			}

			// текстурируем строку
			current_sx = ceilFIntF0(x);
	
			if((current_sy) >= 0 ) if (length) {
		#ifdef SUBTEXEL
		      tmp = ceilFIntF16(x) - x;
			  z1 += (dz1>>8)* (tmp>>8);
		#endif
				while (length--) {
				// используем z-буфер для определения видимости текущей точки
					if( (current_sx<sizeX) && (current_sx >= 0)) {
						if (zBuffer[current_sy*sizeX + current_sx] <= z1) {
							zBuffer[current_sy*sizeX + current_sx] = z1;
						}
					}
					z1 += dz1;
					current_sx++;
				}
			}

			// сдвигаем начальные и конечные значения x,(1/z)
			x_start += dx_start;
			z1_start += dz1_start;
			x_end += dx_end;
			z1_end += dz1_end;
		}

////////////
	}
	int sSizeY=sizeY/SCALING4PUT2EARCH;
	int sSizeX=sizeX/SCALING4PUT2EARCH;

	int j,k,m;
	const int sxKFBC=(sxKF>>1)*SCALING4PUT2EARCH;
	const int syKFBC=(syKF>>1)*SCALING4PUT2EARCH;
	const int maxXBound=(maxX-minX)-(SCALING4PUT2EARCH-1);
	const int maxYBound=(maxY-minY)-(SCALING4PUT2EARCH-1);
	const float preFraction=(1.f/((float)SCALING4PUT2EARCH*SCALING4PUT2EARCH))*(1<<VX_FRACTION);
	for(i=0; i<syKF; i++){
		int offY=i*sxKF;
		for(j=0; j<sxKF; j++){
			int h=0;
			int xZB=j*SCALING4PUT2EARCH-sxKFBC-minX;
			int yZB=i*SCALING4PUT2EARCH-syKFBC-minY;
			if(xZB < 0 || yZB <0 || xZB>=maxXBound || yZB>=maxYBound) continue;
			for(k=0; k<SCALING4PUT2EARCH; k++){
				int offP=(yZB+k)*sizeX+xZB;
				for(m=0; m<SCALING4PUT2EARCH; m++){
					//if(zBuffer[offP+m]!=0){
						h+=zBuffer[offP+m];
					//}
				}
			}
			if(h!=0) {
				//KFArr[offY+j]=round(h*preFraction) + addH;
				if(flag_inverse==0) KFArr[offY+j]= ((h/(SCALING4PUT2EARCH*SCALING4PUT2EARCH))>>(16-VX_FRACTION)) + addH;
				else KFArr[offY+j]= -((h/(SCALING4PUT2EARCH*SCALING4PUT2EARCH))>>(16-VX_FRACTION)) + addH;
			}
		}
	}

	delete [] zBuffer;

/*

	float * zBuffer;
	zBuffer=new float[sizeX*sizeY];
    memset(zBuffer, 0, sizeX*sizeY * sizeof(float));

	for (i = 0; i < numFace; i++) {
		vrtxM2VM *a, *b, *c, *tmpv;
		a=&vrtx[face[i].v1];
		b=&vrtx[face[i].v2];
		c=&vrtx[face[i].v3];

		// отсортируем вершины грани по sy
		if (a->sy > b->sy) { tmpv = a; a = b; b = tmpv; }
		if (a->sy > c->sy) { tmpv = a; a = c; c = tmpv; }
		if (b->sy > c->sy) { tmpv = b; b = c; c = tmpv; }

		// грань нулевой высоты рисовать не будем
		if (round(c->sy) <= round(a->sy)) continue;

		int current_sx, current_sy;
		float tmp, k, x_start, x_end;
		float dx_start, dx_end, dz1_start, dz1_end;
		float z1_start, z1_end;
		float x, z1, dz1;
		int length;
		//unsigned short *dest;

		// посчитаем du/dsx, dv/dsx, d(1/z)/dsx
		// считаем по самой длинной линии (т.е. проходящей через вершину B)
		k = (b->sy - a->sy) / (c->sy - a->sy);
		x_start = a->sx + (c->sx - a->sx) * k;
		z1_start = a->z1 + (c->z1 - a->z1) * k;
		x_end = b->sx;
		z1_end = b->z1;
		dz1 = (z1_start - z1_end) / (x_start - x_end);

		x_start = a->sx;
		z1_start = a->z1;
		dx_start = (c->sx - a->sx) / (c->sy - a->sy);
		dz1_start = (c->z1 - a->z1) / (c->sy - a->sy);
//#ifdef SUBPIXEL
		tmp = ceil(a->sy) - a->sy;
		x_start += dx_start * tmp;
		z1_start += dz1_start * tmp;
//#endif

		if (ceil(b->sy) > ceil(a->sy)) {
			tmp = ceil(a->sy) - a->sy;
			x_end = a->sx;
			z1_end = a->z1;
			dx_end = (b->sx - a->sx) / (b->sy - a->sy);
			dz1_end = (b->z1 - a->z1) / (b->sy - a->sy);
		} else {
			tmp = ceil(b->sy) - b->sy;
			x_end = b->sx;
			z1_end = b->z1;
			dx_end = (c->sx - b->sx) / (c->sy - b->sy);
			dz1_end = (c->z1 - b->z1) / (c->sy - b->sy);
		}
//#ifdef SUBPIXEL
		x_end += dx_end * tmp;
		z1_end += dz1_end * tmp;
//#endif

////////////////////////////////

		// построчная отрисовка грани
		for (current_sy = ceil(a->sy); current_sy < ceil(c->sy); current_sy++) {
			if((current_sy-minY) >= sizeY ) break;
			//if((current_sy-minY) < 0 ) break;//continue;
			if (current_sy == ceil(b->sy)) {
				x_end = b->sx;
				z1_end = b->z1;
				dx_end = (c->sx - b->sx) / (c->sy - b->sy);
				dz1_end = (c->z1 - b->z1) / (c->sy - b->sy);
//#ifdef SUBPIXEL
				tmp = ceil(b->sy) - b->sy;
				x_end += dx_end * tmp;
				z1_end += dz1_end * tmp;
//#endif
			}

			// x_start должен находиться левее x_end
			if (x_start > x_end) {
			  x = x_end;
			  z1 = z1_end;
			  length = ceil(x_start) - ceil(x_end);
			} else {
			  x = x_start;
			  z1 = z1_start;
			  length = ceil(x_end) - ceil(x_start);
			}

			// считаем адрес начала строки в видеопамяти
			//dest = GB;
			//dest += current_sy * sizeX05*2 + (int)ceil(x);

			// текстурируем строку
			current_sx = (int)ceil(x)-minX;
	
			if((current_sy-minY) >= 0 ) if (length) {
		//#ifdef SUBTEXEL
		      tmp = ceil(x) - x;
			  z1 += dz1* tmp;
		//#endif
				while (length--) {
				// используем z-буфер для определения видимости текущей точки
					if( (current_sx<sizeX) && (current_sx >= 0)) {
						if (zBuffer[(current_sy-minY)*sizeX + current_sx] <= z1) {
							zBuffer[(current_sy-minY)*sizeX + current_sx] = z1;
						}
					}
					z1 += dz1;
					//dest++;
					current_sx++;
				}
			}

			// сдвигаем начальные и конечные значения x/u/v/(1/z)
			x_start += dx_start;
			z1_start += dz1_start;
			x_end += dx_end;
			z1_end += dz1_end;
		}

////////////
	}
	int sSizeY=sizeY/SCALING4PUT2EARCH;
	int sSizeX=sizeX/SCALING4PUT2EARCH;

	//short* KFArr, int sxKF, int syKF
	
	int j,k,m;//,i;
	const int sxKFBC=(sxKF>>1)*SCALING4PUT2EARCH;
	const int syKFBC=(syKF>>1)*SCALING4PUT2EARCH;
	const int maxXBound=(maxX-minX)-(SCALING4PUT2EARCH-1);
	const int maxYBound=(maxY-minY)-(SCALING4PUT2EARCH-1);
	const float preFraction=(1.f/((float)SCALING4PUT2EARCH*SCALING4PUT2EARCH))*(1<<VX_FRACTION);
	for(i=0; i<syKF; i++){
		int offY=i*sxKF;
		for(j=0; j<sxKF; j++){
			float h=0;
			int xZB=j*SCALING4PUT2EARCH-sxKFBC-minX;
			int yZB=i*SCALING4PUT2EARCH-syKFBC-minY;
			if(xZB < 0 || yZB <0 || xZB>maxXBound || yZB>maxYBound) continue;
			for(k=0; k<SCALING4PUT2EARCH; k++){
				int offP=(yZB+k)*sizeX+xZB;
				for(m=0; m<SCALING4PUT2EARCH; m++){
					//if(zBuffer[offP+m]!=0){
						h+=zBuffer[offP+m];
					//}
				}
			}
			if(h!=0) {
				KFArr[offY+j]=round(h*preFraction) + addH;
			}
		}
	}

	delete [] zBuffer;
*/
	return 1;
}

///////////////

void s_Mesh2VMapDate::init(int _amountKF, short* _timeKFArr, int sx, int sy)
{
	sizeX=sx;
	sizeY=sy;
	amountKF=_amountKF;
	KFAnimationArr=new short[(amountKF-1)*sizeX*sizeY];
	timeKFArr=new short[amountKF]; //0 frame - тайминг
	int i;
	for(i=0; i<(amountKF); i++) timeKFArr[i]=_timeKFArr[i];

}

void s_Mesh2VMapDate::setKFDate(int numKF, const char* names3DS)
{
	meshM2VM mesh;

	if(names3DS && strcmp(names3DS, "")!=0 ){
		char cb[MAX_PATH];
		GetCurrentDirectory(MAX_PATH, cb);
		strcat(cb, "\\"); strcat(cb, Path2TTerraResource);//"\\RESOURCE\\Tools\\"
		strcat(cb, names3DS);

		mesh.load(cb);
		mesh.rotateAndScaling(0,0,0, 0.05f, 0.05f, 0.1f);//1.5f, 1.5f, 1.5f);
	}
	mesh.put2KF(2, KFAnimationArr+numKF*sizeX*sizeY, sizeX, sizeY);//, -10<<VX_FRACTION); //*sizeof(short)
}

void s_Mesh2VMapDate::resetCommandList()
{
	for(CLIt=commandList_immediately.begin(); CLIt!=commandList_immediately.end(); CLIt++){
		delete *CLIt;
	}
	commandList_immediately.clear();
}

void s_Mesh2VMapDate::command_setKeyFrame(const int numKF, const char* names3DS, const int time, const Vect3f& orientation, const Vect3f& scale, bool flag_inverse, short addH, int KF2Loop, int loopCount)
{
	s_commandInformation* tmp=new s_commandInformation();
	tmp->numKF=numKF;
	tmp->time=time;
	tmp->names3DS=names3DS;
	tmp->flag_inverse=flag_inverse;
	tmp->addH=addH;
	tmp->KF2Loop=KF2Loop;
	tmp->loopCount=loopCount;
	tmp->orientation=orientation;
	tmp->scale=scale;
	commandList_immediately.push_back(tmp);
}

void s_Mesh2VMapDate::compilingCommandList()
{
	const int QUALITY=2;
	sizeX=0;
	sizeY=0;
	int countRD=0;
	for(CLIt=commandList_immediately.begin(); CLIt!=commandList_immediately.end(); CLIt++) {
		if( !(*CLIt)->names3DS.empty() ){
			(*CLIt)->pMesh=mesh2VMapDispather.getMeshFrom3DS((*CLIt)->names3DS.c_str());
			(*CLIt)->pMesh->rotateAndScaling( (*CLIt)->orientation, (*CLIt)->scale);
			Vect2s curMaxXY=(*CLIt)->pMesh->calcSizeXY();
			if(curMaxXY.x > sizeX) sizeX=curMaxXY.x;
			if(curMaxXY.y > sizeY) sizeY=curMaxXY.y;
			countRD++;
		}
	}
	xassert(sizeX <= 256 );
	xassert(sizeY <= 256 );

	KFAnimationArr=new short[sizeX*sizeY*countRD];
	short* curPntArr=KFAnimationArr;
	for(CLIt=commandList_immediately.begin(); CLIt!=commandList_immediately.end(); CLIt++) {
		if( !(*CLIt)->names3DS.empty() ){
			(*CLIt)->RasterData=curPntArr;
			(*CLIt)->pMesh->rotateAndScaling( (*CLIt)->orientation, (*CLIt)->scale);
			(*CLIt)->pMesh->put2KF(QUALITY, (*CLIt)->RasterData, sizeX, sizeY, (*CLIt)->flag_inverse, (*CLIt)->addH);
			curPntArr+=sizeX*sizeY;
		}
		else {
			(*CLIt)->RasterData=0;
		}
	}
}

///////////////
s_Mesh2VMapDispather mesh2VMapDispather;
s_Mesh2VMapDispather::s_Mesh2VMapDispather(void)
{
	const int MAX_MODEL_4_M2VM=8;
	const int MAX_M2VM=12;
	M2VMDateArr.reserve(MAX_MODEL_4_M2VM);
	EUArr.reserve(MAX_M2VM);

/*	s_Mesh2VMapDate* pCurM2VMDate;

	pCurM2VMDate=new s_Mesh2VMapDate();
	const int FACE_AMOUNT_KF1=4;
	short timeArr1[FACE_AMOUNT_KF1]={1, 4, 4, 4 };//{2,4,2};
	pCurM2VMDate->init(FACE_AMOUNT_KF1, timeArr1, 32, 32);
	pCurM2VMDate->setKFDate(0, "face_01.3DS");
	pCurM2VMDate->setKFDate(1, "face_02.3DS");
	pCurM2VMDate->setKFDate(2, "");
	M2VMDateArr.push_back(pCurM2VMDate);

	//////////////////////////////////////////

	pCurM2VMDate=new s_Mesh2VMapDate();
	const int FACE_AMOUNT_KF2=3;
	short timeArr2[FACE_AMOUNT_KF2]={1, 4, 1};//{2,4,2};
	pCurM2VMDate->init(FACE_AMOUNT_KF2, timeArr2, 32, 32);
	pCurM2VMDate->setKFDate(0, "Krab.3DS");
	pCurM2VMDate->setKFDate(1, "");
	M2VMDateArr.push_back(pCurM2VMDate);

	////////////////////////////////////////////

	pCurM2VMDate=new s_Mesh2VMapDate();
	const int FACE_AMOUNT_KF3=2;
	short timeArr3[FACE_AMOUNT_KF3]={4, 4 };//{2,4,2};
	pCurM2VMDate->init(FACE_AMOUNT_KF3, timeArr3, 32, 32);
	pCurM2VMDate->setKFDate(0, "Krab.3DS");
	M2VMDateArr.push_back(pCurM2VMDate);
*/

	/////getMeshFrom3DS("ant.3DS"); // PreLoad off
}

s_Mesh2VMapDispather::~s_Mesh2VMapDispather(void)
{
	vector<s_EarthUnit*>::iterator ei;
	for(ei=EUArr.begin(); ei!=EUArr.end(); ei++){
		delete *ei;
	}
	EUArr.clear();
	vector<s_Mesh2VMapDate*>::iterator mdi;
	for(mdi=M2VMDateArr.begin(); mdi!=M2VMDateArr.end(); mdi++){
		delete *mdi;
	}
	M2VMDateArr.clear();

	list<meshM2VM*>::iterator mi;
	for(mi=meshList.begin(); mi!=meshList.end(); mi++){
		delete *mi;
	}
	meshList.clear();

}
/*
s_EarthUnit* s_Mesh2VMapDispather::getEarthUnit(const int ID)
{
	if(ID>=M2VMDateArr.size()) {
		xassert(0 && "Invalid earth unit ID");
		return 0;
	}
	s_EarthUnit* pCurEU;
	pCurEU=new s_EarthUnit(M2VMDateArr[ID]);
	EUArr.push_back(pCurEU);

	return pCurEU;
}
*/

s_EarthUnit* s_Mesh2VMapDispather::getEarthUnit(s_Mesh2VMapDate * mmDate)
{
	s_Mesh2VMapDate* pCurM2VMDate;
	if(mmDate==0){
		pCurM2VMDate=new s_Mesh2VMapDate();
		M2VMDateArr.push_back(pCurM2VMDate);
	}
	else {
		pCurM2VMDate=mmDate;
	}

	s_EarthUnit* pCurEU;
	pCurEU=new s_EarthUnit(pCurM2VMDate);
	EUArr.push_back(pCurEU);

	return pCurEU;
}

void s_Mesh2VMapDispather::deleteEarthUnit(s_EarthUnit* eu, bool autoDeleteMVMDate)
{
	s_Mesh2VMapDate* pCurM2VMDate=0;
	vector<s_EarthUnit*>::iterator ei;
	for(ei=EUArr.begin(); ei!=EUArr.end(); ei++){
		if(eu==*ei) {
			pCurM2VMDate=(*ei)->meshDate;
			delete *ei;
			EUArr.erase(ei);
			break;
		}
	}

	if(autoDeleteMVMDate){
		//удаление s_Mesh2VMapDate !!!
		int cntPresent=0;
		for(ei=EUArr.begin(); ei!=EUArr.end(); ei++){
			if(pCurM2VMDate==(*ei)->meshDate) {
				cntPresent++;
			}
		}

		if(cntPresent==0){
			vector<s_Mesh2VMapDate*>::iterator mdi;
			for(mdi=M2VMDateArr.begin(); mdi!=M2VMDateArr.end(); mdi++){
				if(pCurM2VMDate==*mdi){
					delete *mdi;
					M2VMDateArr.erase(mdi);
					break;
				}
			}
		}
	}

}

meshM2VM* s_Mesh2VMapDispather::getMeshFrom3DS(const char * name3DS)
{
	char cb[MAX_PATH];
	strcpy(cb, Path2TTerraResource); //"\\RESOURCE\\Tools\\"
	strcat(cb, name3DS);

	list<meshM2VM*>::iterator mi;
	for(mi=meshList.begin(); mi!=meshList.end(); mi++){
		if(stricmp(cb, (*mi)->fname3DS)==0) break;
	}
	if(mi!=meshList.end()){
		return *mi;
	}
	else{
		meshM2VM* pMesh=new meshM2VM();


		pMesh->load(cb);
		meshList.push_back(pMesh);
		return pMesh;
	}
}

/////////////////////

void s_EarthUnit::init(int xMap, int yMap)
{
	xassert(meshDate->commandList_immediately.size() >= 2); //Минимум должно быть 2 KF
	nextKFIt=curKFIt=meshDate->commandList_immediately.begin();
	nextKFIt++;
	


	xL=xMap-(meshDate->sizeX>>1);
	yL=yMap-(meshDate->sizeY>>1);


	mask=new unsigned char[meshDate->sizeX*meshDate->sizeY];

	currentBetweenFrame=0;
	//previsionKF=-1;
	//currentKF=0;
	frame=0;

	pCurKFRD=(*curKFIt)->RasterData;
	pNextKFRD=(*nextKFIt)->RasterData;

	inVx=new short[meshDate->sizeX*meshDate->sizeY];
	tmpVx=new short[meshDate->sizeX*meshDate->sizeY];
	substare=new short[meshDate->sizeX*meshDate->sizeY];

	unsigned char* geo=vMap.VxGBuf;
	unsigned char* dam=vMap.VxDBuf;
	unsigned char* atr=vMap.AtrBuf;
	int i,j,cnt=0;
	int vmin=MAX_VX_HEIGHT;
	int vmax=0;
	for(i=0; i<meshDate->sizeY; i++){
		int offy=vMap.offsetBuf(0, vMap.YCYCL(yL+i));
		for(j=0; j<meshDate->sizeX; j++){
			int off=offy+vMap.XCYCL(xL+j);
			int v;
			if(*(dam+off)==0) v= *(geo+off) <<VX_FRACTION;
			else v= *(dam+off) <<VX_FRACTION;
			v|=*(atr+off) &VX_FRACTION_MASK;
			*(inVx+cnt)=v;
			*(tmpVx+cnt)=v;
			if( v < vmin) vmin=v;
			if( v > vmax) vmax=v;
			mask[cnt]=0;
			cnt++;
		}
	}
	//if((vmax-vmin)>DELTA_BUBBLE_NOT_DEEP) h_begin=vmin;
	//else h_begin=vmin-BEGIN_BUBBLE_DEEP;
	h_begin=vmin+(vmax-vmin)/3;//+(vmax-vmin)/4;
	vMin=vmin;
	h_begin=(vmin+vmax)/2;
	
	Vect3f position(xL+(meshDate->sizeX>>1), yL+(meshDate->sizeY>>1), (float)h_begin/(float)(1<<VX_FRACTION));
	Vect3f outOrientation;
	int radiusAnalization;
	if(meshDate->sizeX > meshDate->sizeX) radiusAnalization=meshDate->sizeX/2;
	else radiusAnalization=meshDate->sizeY/2;
	radiusAnalization=round((float)radiusAnalization*(2.f/3.f));
	
	analyzeTerrain(position, radiusAnalization, outOrientation);
	//h_begin=round(position.z*(1<<VX_FRACTION));
	float A=outOrientation.x;
	float B=outOrientation.y;
	float C=-outOrientation.z;
	float D=(float)h_begin/(float)(1<<VX_FRACTION);//position.z;

	cnt=0;
	int sx05=meshDate->sizeX>>1;
	int sy05=meshDate->sizeY>>1;
	for(i = 0; i < meshDate->sizeY; i++){
		for(j = 0; j < meshDate->sizeX; j++) {
			float as=(-(A*(j-sx05) + B*(i-sy05) + D)/C);
			substare[cnt]=round( (-(A*(j-sx05) + B*(i-sy05) + D)/C ) * (1<<VX_FRACTION) );
			cnt++;
		}
	}
	flag_move=0;
}

void s_EarthUnit::relativeMove(int dx, int dy)
{
	if(dx!=0 || dy!=0){
		nxL=xL+dx;
		nyL=yL+dy;
		flag_move=1;
	}
}

void s_EarthUnit::absolutMove(int x, int y)
{
	x-=(meshDate->sizeX>>1);
	y-=(meshDate->sizeY>>1);
	if(x!=xL || y!=yL){
		nxL=x;
		nyL=y;
		flag_move=1;
	}
}


bool s_EarthUnit::quant()
{

	//int curKScale=((currentBetweenFrame+1)<<16)/meshDate->timeKFArr[currentKF];
	int curKScale=((currentBetweenFrame+1)<<16)/(*curKFIt)->time;
	int prevKScale=(1<<16)-curKScale;
	int xxx=0;//x%d;
	int yyy=0;//y%d;
	int i,j,cnt=0;
	unsigned short sx=meshDate->sizeX;
	unsigned short sy=meshDate->sizeY;

	if(flag_move){
		unsigned char* geo=vMap.VxGBuf;
		unsigned char* dam=vMap.VxDBuf;
		unsigned char* atr=vMap.AtrBuf;
		int i,j,cnt=0;
		for(i=0; i<meshDate->sizeY; i++){
			int offy=vMap.offsetBuf(0, vMap.YCYCL(yL+i));
			for(j=0; j<meshDate->sizeX; j++){
				int off=offy+vMap.XCYCL(xL+j);

				short vv;
				short restoreV;
				if(vMap.VxDBuf[off]==0){
					vv= vMap.VxGBuf[off] <<VX_FRACTION;
					vv|=vMap.AtrBuf[off] &VX_FRACTION_MASK;
					restoreV=inVx[cnt]+vv-tmpVx[cnt];
					vMap.VxGBuf[off]=restoreV>>VX_FRACTION;
					vMap.AtrBuf[off]= (restoreV&VX_FRACTION_MASK) | (vMap.AtrBuf[off]&(~VX_FRACTION_MASK));
				}
				else {
					vv= vMap.VxDBuf[off] <<VX_FRACTION;
					vv|=vMap.AtrBuf[off] &VX_FRACTION_MASK;
					restoreV=inVx[cnt]+vv-tmpVx[cnt];
					vMap.VxDBuf[off]=restoreV>>VX_FRACTION;
					vMap.AtrBuf[off]= (restoreV&VX_FRACTION_MASK) | (vMap.AtrBuf[off]&(~VX_FRACTION_MASK));
				}
				cnt++;
			}
		}

		cnt=0;
		int vmin=MAX_VX_HEIGHT;
		int vmax=0;
		for(i=0; i<meshDate->sizeY; i++){
			int offy=vMap.offsetBuf(0, vMap.YCYCL(nyL+i));
			for(j=0; j<meshDate->sizeX; j++){
				int off=offy+vMap.XCYCL(nxL+j);
				int v;
				if(*(dam+off)==0) v= *(geo+off) <<VX_FRACTION;
				else v= *(dam+off) <<VX_FRACTION;
				v|=*(atr+off) &VX_FRACTION_MASK;
				*(inVx+cnt)=v;
				*(tmpVx+cnt)=v;
				if( v < vmin) vmin=v;
				if( v > vmax) vmax=v;
				mask[cnt]=0;
				cnt++;
			}
		}
		h_begin=vmin+(vmax-vmin)/3;//+(vmax-vmin)/4;
		vMin=vmin;
		h_begin=(vmin+vmax)/2;
		
		Vect3f position(nxL+(meshDate->sizeX>>1), nyL+(meshDate->sizeY>>1), (float)h_begin/(float)(1<<VX_FRACTION));
		Vect3f outOrientation;
		analyzeTerrain(position, meshDate->sizeX/2, outOrientation);
		float A=outOrientation.x;
		float B=outOrientation.y;
		float C=-outOrientation.z;
		float D=(float)h_begin/(float)(1<<VX_FRACTION);//position.z;

		cnt=0;
		int sx05=meshDate->sizeX>>1;
		int sy05=meshDate->sizeY>>1;
		for(i = 0; i < meshDate->sizeY; i++){
			for(j = 0; j < meshDate->sizeX; j++) {
				float as=(-(A*(j-sx05) + B*(i-sy05) + D)/C);
				substare[cnt]=round( (-(A*(j-sx05) + B*(i-sy05) + D)/C ) * (1<<VX_FRACTION) );
				cnt++;
			}
		}
		short tmp;
		tmp=xL; xL=nxL; nxL=tmp;
		tmp=yL; yL=nyL; nyL=tmp;
	}

	for(i = 0; i < sy; i++){
		for(j = 0; j < sx; j++) {

			int V;
			//if(pCurKP[i*sx+j]==0 && mask[i*sx+j]==0) continue;
			//mask[i*sx+j]=1;
			int offset=vMap.offsetBuf((xL+j) & vMap.clip_mask_x, (yL+i) & vMap.clip_mask_y);

			int vv;
			if(vMap.VxDBuf[offset]==0) vv= vMap.VxGBuf[offset] <<VX_FRACTION;
			else vv= vMap.VxDBuf[offset] <<VX_FRACTION;
			vv|=vMap.AtrBuf[offset] &VX_FRACTION_MASK;
			if(tmpVx[cnt]!=vv){
				inVx[cnt]+=vv-tmpVx[cnt];
//				if(inVx[cnt]<0)inVx[cnt]=0;
//				if(inVx[cnt]>MAX_VX_HEIGHT)inVx[cnt]=MAX_VX_HEIGHT;
			}

			int Vold=inVx[cnt];
			int prevH;
			if(pCurKFRD!=0) prevH=prevKScale*pCurKFRD[i*sx+j];
			else prevH=0;
			int curH;
			if(pNextKFRD!=0) curH=(prevH + pNextKFRD[i*sx+j]*curKScale) >>(16);
			else curH=prevH >>(16);
			V=curH;
			if( curH==0 && prevH==0 && mask[i*sx+j]==0) {tmpVx[cnt]=Vold; cnt++; continue;} //!!!!!!!
			mask[i*sx+j]=1;									  //!!!!!!!

			//V+=h_begin;
			V+=substare[cnt];
			//V=substare[cnt];

			static char fl=0;
			static int k_dh[256];
			if(fl==0){
				fl=1;
				for(int m=0; m<256; m++){
					float x=(float)m/128;//Диапазон от 0 до 2
					k_dh[m]=round((-0.1f+exp(-fabsf((x-1)*(x-1)*(x-1))/(0.4f*0.4f)))*(1<<16));
				}

			}
			if(Vold > V){
/*				//int inx=(Vold-V)>>3;
				int inx=(Vold-V)>>2;//>>3;
				if(inx>255)inx=255;
				//V=Vold + ((V-h_begin)*k_dh[inx]>>(16+1) );
				int dV=V-h_begin; if (dV>(10<<VX_FRACTION))dV=10<<VX_FRACTION;
				V=Vold + ((dV)*k_dh[inx]>>(16) );
				if(V>MAX_VX_HEIGHT)V=MAX_VX_HEIGHT;
				if(V<0) V=0;*/
			//	V=Vold;
			}
			else {
				//if(vMap.VxDBuf[offset]!=0){
				//	vMap.VxDBuf[offset]=0;
				//	vMap.SurBuf[offset]=vMap.GetGeoType(offset,V);
				//}
			}


			if(V<0)V=0;
			if(V>MAX_VX_HEIGHT)V=MAX_VX_HEIGHT;

			//для нормальной границы при  разрушении зеропласта
			if( (vMap.VxDBuf[offset]==0 && vMap.VxGBuf[offset]==(V >>VX_FRACTION)) || (vMap.VxDBuf[offset]==(V >>VX_FRACTION)) ){ 
				vMap.AtrBuf[offset]=(V &VX_FRACTION_MASK) | (vMap.AtrBuf[offset]&=~VX_FRACTION_MASK);
			}
			else {
				if(vMap.VxDBuf[offset]==0) vMap.VxGBuf[offset]=V >>VX_FRACTION;
				else vMap.VxDBuf[offset]=V >>VX_FRACTION;
				//нужно сохранить только признак тени, признак At_ZEROPLASTPRESENT при любом изменении удаляется
				vMap.AtrBuf[offset]=(V &VX_FRACTION_MASK) | (vMap.AtrBuf[offset]&=~(VX_FRACTION_MASK|At_NOTPURESURFACE));
				//if(vMap.VxDBuf[offset]< V >>VX_FRACTION)vMap.VxDBuf[offset]=0;
			}
			tmpVx[cnt]=V;

			cnt++;

		}
	}
	//if(h_begin < vMin) h_begin=h_begin + (1<<VX_FRACTION);

	vMap.recalcArea2Grid(vMap.XCYCL(xL-1), vMap.YCYCL(yL-1), vMap.XCYCL(xL + sx+1), vMap.YCYCL(yL + sy+1) );
	vMap.regRender(xL, yL, xL+sx, yL+sy);

	if(flag_move){
		vMap.recalcArea2Grid(vMap.XCYCL(nxL-1), vMap.YCYCL(nyL-1), vMap.XCYCL(nxL + sx+1), vMap.YCYCL(nyL + sy+1) );
		vMap.regRender(nxL, nyL, nxL+sx, nyL+sy);
		nxL=xL; nyL=nyL;
	}
	flag_move=0;


	frame++; currentBetweenFrame++;
	if( currentBetweenFrame >= (*curKFIt)->time /*meshDate->timeKFArr[currentKF]*/ ) {
		/*previsionKF=currentKF;
		currentKF++;
		currentBetweenFrame=0;
		pPrevKF=pCurKF;
		pCurKF+=sx*sy;
		if(pCurKF >= meshDate->KFAnimationArr+sx*sy*(meshDate->amountKF-1)){
			pCurKF=meshDate->KFAnimationArr; //Зацикливание на начало
			return 0;
		}
		if(currentKF>=(meshDate->amountKF)) {
			currentKF=1; //Зацикливание на начало
		}
		*/

		currentBetweenFrame=0;

		curKFIt=nextKFIt;
		nextKFIt++;
		if( (flag_loop==1) && ((*curKFIt)->loopCount > 0) ){
			(*curKFIt)->loopCount-=1;
			list<s_commandInformation *>::iterator p;
			for(p=meshDate->commandList_immediately.begin(); p!=meshDate->commandList_immediately.end(); p++){
				if((*p)->numKF==(*curKFIt)->KF2Loop){
					nextKFIt=p;
					break;
				}
			}
		}
		if(nextKFIt==meshDate->commandList_immediately.end()) {
			//В конце выполняется начальная инициализация на случай если квант будет вызываться дальше
			nextKFIt=curKFIt=meshDate->commandList_immediately.begin();
			nextKFIt++;
			pCurKFRD=(*curKFIt)->RasterData;
			pNextKFRD=(*nextKFIt)->RasterData;
			return 0;
		}
		else {
			pCurKFRD=(*curKFIt)->RasterData;
			pNextKFRD=(*nextKFIt)->RasterData;
		}
	}
	return 1;

}


c3DSGeoActionCreator::c3DSGeoActionCreator()
{
}

c3DSGeoActionCreator::~c3DSGeoActionCreator()
{
}

c3DSGeoAction* c3DSGeoActionCreator::Build(short xc, short yc, float orientation,
	s3DSGeoParameter* command)
{
	return instance()->BuildD(xc,yc,orientation,command);
}

c3DSGeoAction* c3DSGeoActionCreator::BuildD(short xc, short yc, float orientation,s3DSGeoParameter* command)
{
	//find
	Cache* cache=NULL;
	for(int i=0;i<array_cache.size();i++)
	{
		if(array_cache[i].command==command)
		{
			cache=&array_cache[i];
			break;
		}
	}

	if(!cache)
	{
		Cache c;
		c.command=command;
		array_cache.push_back(c);
		cache=&array_cache.back();
	}

	if(cache->MMDateArr.empty()){
		cache->MMDateArr.resize(ANT_MAX_MESH2VMAPDATE);
		for(int i=0; i<cache->MMDateArr.size(); i++){
			cache->MMDateArr[i]=0;
		}
	}

	s_EarthUnit* pEarthUnit=NULL;
	float tmp=fmod(orientation, 2*M_PI);
	if(tmp<0)tmp+=2*M_PI;

	float ANT_RAD_IN_CELL_ARR=(2*M_PI)/cache->MMDateArr.size();

	int granulateOrientation=round(tmp/ANT_RAD_IN_CELL_ARR);
	if(granulateOrientation>=cache->MMDateArr.size())
		granulateOrientation=cache->MMDateArr.size()-1;

	if(cache->MMDateArr[granulateOrientation]==0)
	{
		pEarthUnit=mesh2VMapDispather.getEarthUnit(0);
		pEarthUnit->meshDate->resetCommandList();

		Vect3f orient(0,0,granulateOrientation*ANT_RAD_IN_CELL_ARR+0.5f*ANT_RAD_IN_CELL_ARR);

		for(int i=0;i<command->command.size();i++)
		{
			s3DSGeoParameter::CommandSet& c=command->command[i];
			pEarthUnit->meshDate->command_setKeyFrame(c.numKF, c.names3DS.c_str(), c.time, orient, c.scale, c.flag_inverse, c.addH,c.KF2Loop,c.loopCount);
		}
		pEarthUnit->meshDate->compilingCommandList();
		cache->MMDateArr[granulateOrientation]=pEarthUnit->meshDate;
	}
	else {
		pEarthUnit=mesh2VMapDispather.getEarthUnit(cache->MMDateArr[granulateOrientation]);
	}

	pEarthUnit->init(xc, yc);
	return new c3DSGeoAction(pEarthUnit);
}

c3DSGeoActionCreator* c3DSGeoActionCreator::instance()
{
	static c3DSGeoActionCreator generator;
	return &generator;
}

/*
//static
s_Mesh2VMapDate* s_AntBirthGeoAction::MMDateArr[ANT_MAX_MESH2VMAPDATE];
s_Mesh2VMapDate* s_AntDeathGeoAction::MMDateArr[ANT_MAX_MESH2VMAPDATE];

s_AntBirthGeoAction::s_AntBirthGeoAction(short xc, short yc, float orientation)
{
	static bool flag_first=0;
	if(flag_first==0){
		flag_first=1;
		for(int i=0; i<ANT_MAX_MESH2VMAPDATE; i++){
			MMDateArr[i]=0;
		}
	}

	//pEarthUnit=mesh2VMapDispather.getEarthUnit(1);
	//pEarthUnit->init(xc, yc);
	float tmp=fmod(orientation, 2*M_PI);
	if(tmp<0)tmp+=2*M_PI;
	int granulateOrientation=round(tmp/ANT_RAD_IN_CELL_ARR);
	if(granulateOrientation>=ANT_MAX_MESH2VMAPDATE)granulateOrientation=ANT_MAX_MESH2VMAPDATE-1;
	if(MMDateArr[granulateOrientation]==0){
		pEarthUnit=mesh2VMapDispather.getEarthUnit(0);
		pEarthUnit->meshDate->resetCommandList();
		//Vect3f scaling(.05f,.05f,.02f);
		Vect3f scaling(.035f,.035f,.015f);
		Vect3f orient(0,0,granulateOrientation*ANT_RAD_IN_CELL_ARR+0.5f*ANT_RAD_IN_CELL_ARR);
		pEarthUnit->meshDate->command_setKeyFrame(0, "", 6, orient, scaling, 0, 0);
		pEarthUnit->meshDate->command_setKeyFrame(1, "ant.3DS", 6, orient, scaling, 0, 0);
		pEarthUnit->meshDate->command_setKeyFrame(2, "ant.3DS", 4, orient, scaling, 1, 0);
		pEarthUnit->meshDate->compilingCommandList();
		MMDateArr[granulateOrientation]=pEarthUnit->meshDate;
	}
	else {
		pEarthUnit=mesh2VMapDispather.getEarthUnit(MMDateArr[granulateOrientation]);
	}
	pEarthUnit->init(xc, yc);
}

s_AntDeathGeoAction::s_AntDeathGeoAction(short xc, short yc, float orientation)
{
	static bool flag_first=0;
	if(flag_first==0){
		flag_first=1;
		for(int i=0; i<ANT_MAX_MESH2VMAPDATE; i++){
			MMDateArr[i]=0;
		}
	}
	//pEarthUnit=mesh2VMapDispather.getEarthUnit(2);
	//pEarthUnit->init(xc, yc);

	float tmp=fmod(orientation, 2*M_PI);
	if(tmp<0)tmp+=2*M_PI;
	int granulateOrientation=round(tmp/ANT_RAD_IN_CELL_ARR);
	if(granulateOrientation>=ANT_MAX_MESH2VMAPDATE)granulateOrientation=ANT_MAX_MESH2VMAPDATE-1;
	if(MMDateArr[granulateOrientation]==0){
		pEarthUnit=mesh2VMapDispather.getEarthUnit(0);
		pEarthUnit->meshDate->resetCommandList();
		//Vect3f scaling(.05f,.05f,.02f);
		Vect3f scaling(.035f,.035f,.015f);
		Vect3f orient(0,0,granulateOrientation*ANT_RAD_IN_CELL_ARR+0.5f*ANT_RAD_IN_CELL_ARR);
		pEarthUnit->meshDate->command_setKeyFrame(0, "", 6, orient, scaling, 0, 0);
		pEarthUnit->meshDate->command_setKeyFrame(1, "ant.3DS", 4, orient, scaling, 0, 0);
		pEarthUnit->meshDate->compilingCommandList();
		MMDateArr[granulateOrientation]=pEarthUnit->meshDate;
	}
	else {
		pEarthUnit=mesh2VMapDispather.getEarthUnit(MMDateArr[granulateOrientation]);
	}
	pEarthUnit->init(xc, yc);

}
*/

//s_HeadGeoAction headGeoAction;
s_HeadGeoAction::s_HeadGeoAction(void)
{
	static bool flag_first=0;
	if(flag_first==0){
		flag_first=1;
	}

	pEarthUnit=mesh2VMapDispather.getEarthUnit(0);
	pEarthUnit->meshDate->resetCommandList();
	/*pCurM2VMDate->setKFDate(0, "Krab.3DS");*/
	Vect3f scaling(2.f,2.f,2.f);
	Vect3f orient(0,0,0);
	pEarthUnit->meshDate->command_setKeyFrame(0, "", 20, orient, scaling, 0, 0);
	pEarthUnit->meshDate->command_setKeyFrame(1, "head.M3D", 2, orient, scaling, 0, 0);
	orient.x= 5*(M_PI/180.f);
	pEarthUnit->meshDate->command_setKeyFrame(2, "head.M3D", 2, orient, scaling, 0, 0);
	orient.x= 5*(M_PI/180.f); orient.y= 5*(M_PI/180.f);
	pEarthUnit->meshDate->command_setKeyFrame(3, "head.M3D", 2, orient, scaling, 0, 0);
	orient.x= 5*(M_PI/180.f); orient.y= 5*(M_PI/180.f); orient.z= 5*(M_PI/180.f);
	pEarthUnit->meshDate->command_setKeyFrame(4, "head.M3D", 2, orient, scaling, 0, 0);
	orient.x= 3*(M_PI/180.f); orient.y= 2*(M_PI/180.f); orient.z= 2*(M_PI/180.f);
	pEarthUnit->meshDate->command_setKeyFrame(5, "head.M3D", 2, orient, scaling, 0, 0);
	orient.x= 0*(M_PI/180.f); orient.y= 0*(M_PI/180.f); orient.z= 0*(M_PI/180.f);
	pEarthUnit->meshDate->command_setKeyFrame(6, "head.M3D", 2, orient, scaling, 0, 0);
	orient.x= -2*(M_PI/180.f); orient.y= -2*(M_PI/180.f); orient.z= -3*(M_PI/180.f);
	pEarthUnit->meshDate->command_setKeyFrame(7, "head.M3D", 2, orient, scaling, 0, 0);
	orient.x= 0*(M_PI/180.f); orient.y= -5*(M_PI/180.f); orient.z= -5*(M_PI/180.f);
	pEarthUnit->meshDate->command_setKeyFrame(8, "head.M3D", 2, orient, scaling, 0, 0);
	orient.x= 0*(M_PI/180.f); orient.y= 0*(M_PI/180.f); orient.z= -5*(M_PI/180.f);
	pEarthUnit->meshDate->command_setKeyFrame(9, "head.M3D", 2, orient, scaling, 0, 0, 1, INT_MAX);

	orient.x= 0*(M_PI/180.f); orient.y= 0*(M_PI/180.f); orient.z= 0*(M_PI/180.f);
	pEarthUnit->meshDate->command_setKeyFrame(9, "head.M3D", 10, orient, scaling, 0, 0);
	scaling.x=scaling.y=2.f;
	scaling.z=0.5f;
	pEarthUnit->meshDate->command_setKeyFrame(0, "head.M3D", 20, orient, scaling, 1, 0);

	pEarthUnit->meshDate->compilingCommandList();
	//MMDateArr[granulateOrientation]=pEarthUnit->meshDate;

}

void s_HeadGeoAction::init(int x, int y,float radius)
{
	if(pEarthUnit) {
		pEarthUnit->init(x, y);
		pEarthUnit->setLoop();
	}
}

void s_HeadGeoAction::finit(void)
{
	if(pEarthUnit) {
		pEarthUnit->resetLoop();
	}
}


/////////////////////////////////////////////////////////////////////////////////////////////



//const long	SGEOWAVE_FRONT_WIDTH=10;
//const long	SGEOWAVE_DELTA_FRONT_WIDTH=2;

const short SGEOWAVE_LONG_WAVE=18;
const float	SGEOWAVE_SPEED_WAVE=2.;
const float	SGEOWAVE_MAX_WAVE_AMPLITUDE=6<<VX_FRACTION;

const long SGEOWAVE_MAX_TOTAL_RADIUS=100;
const long SGEOWAVE_MAX_RADIUS=SGEOWAVE_MAX_TOTAL_RADIUS-SGEOWAVE_LONG_WAVE;

sGeoWave::sGeoWave(short _x, short _y, short _maxRadius)
{
	if(_maxRadius > MAX_RADIUS_CIRCLEARR){
		xassert(0&&"exceeding max radius in sGeoWave ");
		_maxRadius=MAX_RADIUS_CIRCLEARR;
	}

	x=_x; y=_y; 
	maxRadius=_maxRadius-SGEOWAVE_LONG_WAVE;
	if(maxRadius < 0) maxRadius=0;
	if(maxRadius >= SGEOWAVE_MAX_RADIUS) maxRadius=SGEOWAVE_MAX_RADIUS-1; //Не обязательно (есть проверка ниже)
	step=0;
}


float sGeoWave::calcMaxAmpWave(short curBegRad)
{
	float amp=1.f;
	if(curBegRad > (maxRadius*1/2) ){
		amp=(float)((maxRadius*1/2)-(curBegRad-maxRadius*1/2)) / (maxRadius*1/2);
	}
	return (1.f+noise1(3.4f+(float)curBegRad/10))*amp*SGEOWAVE_MAX_WAVE_AMPLITUDE;

}
bool sGeoWave::quant(void)
{

	int i, j;
	if(step>0){
		short curBegRadWave=1 + round(SGEOWAVE_SPEED_WAVE*(float)(step-1));
		float curMaxAmpWave=calcMaxAmpWave(curBegRadWave);
		for(i = 0; i < SGEOWAVE_LONG_WAVE; i++){
			long curAmp=round(curMaxAmpWave * sin( (float)i*((2*M_PI)/SGEOWAVE_LONG_WAVE) ) );
			int curRad=curBegRadWave+i;
			int max = maxRad[curRad];
			int* xx = xRad[curRad];
			int* yy = yRad[curRad];
			for(j = 0;j < max;j++) {
				int offB=vMap.offsetBuf(vMap.XCYCL(x + xx[j]), vMap.YCYCL(y + yy[j]));
				long V=vMap.SGetAlt(offB);
				V-=curAmp;
				if(V<0)V=0;
				vMap.SPutAlt(offB, V);
			}
		}
	}

	short curBegRadWave=1 + round(SGEOWAVE_SPEED_WAVE*(float)(step));
	if( (curBegRadWave >= maxRadius) || (curBegRadWave >= SGEOWAVE_MAX_RADIUS) ) {
		short renderRad=curBegRadWave+SGEOWAVE_LONG_WAVE;
		damagingBuildingsTolzer(x, y, maxRadius+SGEOWAVE_LONG_WAVE);
		vMap.recalcArea2Grid(vMap.XCYCL(x-renderRad), vMap.YCYCL(y-curBegRadWave), vMap.XCYCL(x + renderRad+1), vMap.YCYCL(y + renderRad+1) );
		vMap.regRender(x-renderRad, y-renderRad, x+renderRad, y+renderRad);
		return 0;
	}

	float curMaxAmpWave=calcMaxAmpWave(curBegRadWave);
	for(i = 0; i < SGEOWAVE_LONG_WAVE; i++){
		long curAmp=round(curMaxAmpWave * sin( (float)i*((2*M_PI)/SGEOWAVE_LONG_WAVE) ) );
		int curRad=curBegRadWave+i;
		int max = maxRad[curRad];
		int* xx = xRad[curRad];
		int* yy = yRad[curRad];
		for(j = 0;j < max;j++) {
			int offB=vMap.offsetBuf(vMap.XCYCL(x + xx[j]), vMap.YCYCL(y + yy[j]));
			long V=vMap.SGetAlt(offB);
			V+=curAmp+XRnd(1<<4);
			//vMap.SPutAlt(offB, V);
			if(V<0)V=0;
			vMap.SPutAltAndClearZL(offB, V);
		}
	}
	short renderRad=curBegRadWave+SGEOWAVE_LONG_WAVE;
	vMap.recalcArea2Grid(vMap.XCYCL(x-renderRad), vMap.YCYCL(y-curBegRadWave), vMap.XCYCL(x + renderRad+1), vMap.YCYCL(y + renderRad+1) );
	vMap.regRender(x-renderRad, y-renderRad, x+renderRad, y+renderRad);

	step++;	
	return 1;
}

#ifdef _SURMAP_
struct KKK{
	int m,n;
	KKK(int _m, int _n){
		m=_m;
		n=_n;
	};
};

int fooL(char ar[])
{
	int lenght=sizeof(ar)/sizeof(ar[0]);
	return lenght;
}

KKK MyArrKKK[]={KKK(2,3), KKK(4,4), KKK(7,3)};
void foo(void){
	float arr123[1024];
	for(int i=0; i<1024; i++){
		arr123[i]=noise1((float)i/100);
	}
	int sz=sizeof(MyArrKKK);
	char k[5];
	fooL(k);
}

struct myRun {
	myRun(void){
		foo();
	}
};
myRun mr;
#endif
