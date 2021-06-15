#include "stdafxTr.h"
#include "fastmath.h"

#include <algorithm>

#define DEBUG_SHOW

#pragma warning( disable : 4554 )  


const int _COL1 = 224 + 15;
const int _COL2 = 224 + 10;

/*-------------------------------RENDER SECTION______________________________*/
//unsigned char* lightCLR[TERRAIN_MAX];
//unsigned char palCLR[TERRAIN_MAX][2*256];
unsigned char light_G[2][512*3];
unsigned char light_GM[2][16][512*3];
unsigned int sin_GM[2][512*3];

//s_terra terra;

//int h_l=160;
int h_l=160;
//const  h_l=120; //интенсивность 255 // 1/2 - 180
int d_x=4;
int d_xw=8;
int delta_s;
int l_ras=32;
int RENDER_BORDER_ZONE;
/* ----------------------------- EXTERN SECTION ---------------------------- */
/* --------------------------- PROTOTYPE SECTION --------------------------- */
/* --------------------------- DEFINITION SECTION -------------------------- */


//static 
int* xRad[MAX_RADIUS_CIRCLEARR + 1];
//static 
int* yRad[MAX_RADIUS_CIRCLEARR + 1];
//static 
int maxRad[MAX_RADIUS_CIRCLEARR + 1];


//const int MATERIAL_MAX = 1;
//const int TERRAIN_MATERIAL[TERRAIN_MAX] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
//const double TERRAIN_DXKOEF[MATERIAL_MAX] = { 1.0 };
//const double TERRAIN_SDKOEF[MATERIAL_MAX] = { 1.0 };
//const double TERRAIN_JJKOEF[MATERIAL_MAX] = { 1.0 };

//unsigned char lightCLRmaterial[MATERIAL_MAX][CLR_MAX];


unsigned char* shadowParent;

//#ifndef TERRAIN16
//static unsigned char* waterBuf[3];
//#endif

int VisiRegR = 0;

///////////////// For SurMAP II //////////////////////////
static int* static_xheap=0;
static int* static_yheap=0;
void vrtMap::landPrepare(void)
{
	const int SIDE = 2*MAX_RADIUS_CIRCLEARR + 1;

	short* rad = new short[SIDE*SIDE];
	int max = 0;

	int calc = 1;
	int i,j,r,ind;
	short* p = rad;
	if(calc){
		for(j=-MAX_RADIUS_CIRCLEARR; j<=MAX_RADIUS_CIRCLEARR; j++)
			for(i=-MAX_RADIUS_CIRCLEARR; i<=MAX_RADIUS_CIRCLEARR; i++, p++){
				r = (int)sqrt(i*(double)i + j*(double)j);
				if(r > MAX_RADIUS_CIRCLEARR) *p = -1;
				else {
					*p = r;
					maxRad[r]++;
					max++;
					}
				}
		}

	int* xheap = static_xheap = new int[max];
	int* yheap = static_yheap = new int[max];
	for(ind = 0;ind <= MAX_RADIUS_CIRCLEARR;ind++){
		xRad[ind] = xheap;
		yRad[ind] = yheap;
		for(p = rad, r = 0, j=-MAX_RADIUS_CIRCLEARR; j<=MAX_RADIUS_CIRCLEARR; j++)
			for(i=-MAX_RADIUS_CIRCLEARR; i<=MAX_RADIUS_CIRCLEARR; i++, p++)
				if(*p == ind){
					xheap[r] = i;
					yheap[r] = j;
					r++;
					}
		xheap += maxRad[ind];
		yheap += maxRad[ind];
		}
	delete rad;
}
void vrtMap::landRelease(void)
{
	if(static_xheap) delete [] static_xheap;
	if(static_yheap) delete [] static_yheap;
}

void vrtMap::deltaZone(sToolzerPMO& var) //(int x,int y,int rad,int smth,int dh,int smode,int eql)
{
	int x=var.x; int y=var.y; int rad=var.rad; int smth=var.smth; int dh=var.dh; int smode=var.smode; int eql=var.eql;
	if(dh>0) { FilterMinHeight=0; FilterMaxHeight=var.filterHeigh; }
	else if(dh<0) { FilterMinHeight=var.filterHeigh; FilterMaxHeight=MAX_VX_HEIGHT; }
	else {FilterMinHeight=0; FilterMaxHeight=MAX_VX_HEIGHT; }

	if(rad > MAX_RADIUS_CIRCLEARR){
		xassert(0&&"exceeding max radius in deltaZone ");
		rad=MAX_RADIUS_CIRCLEARR;
	}

	if(flag_record_operation) {
		//ElementPMO p; p.putToolzerParam(10);
		//containerPMO.push_back( ElementPMO(WrapPMO<sToolzerPMO>(x,y,rad,smth,dh,smode,eql)) );
		containerPMO.push_back( ElementPMO(var) );
	}
	UndoDispatcher_PutPreChangedArea(x - rad,y - rad,x + rad +1,y + rad+1);

	static int locp;

	int i,j;
	int max;
	int* xx,*yy;

	int r = rad - rad*smth/10;
	double d = 1.0/(rad - r + 1),dd,ds,s;
	int v,h,k,mean;

	if(dh){
		for(i = 0;i <= r;i++){
			max = maxRad[i];
			xx = xRad[i];
			yy = yRad[i];
			for(j = 0;j < max;j++) voxSet((x + xx[j]) & clip_mask_x,(y + yy[j]) & clip_mask_y,dh);
		}
		for(i=r+1, dd=1.0-d; i <= rad; i++,dd -=d){
			max = maxRad[i];
			xx = xRad[i];
			yy = yRad[i];
			h = (int)(dd*dh);
			if(!h) h = dh > 0 ? 1 : -1;
			switch(smode){
				case 0:
					v = (int)(dd*max);
					ds = (double)v/(double)max;
					for(s = ds,k = 0,j = locp%max;k < max;j = (j + 1 == max) ? 0 : j + 1,k++,s += ds)
						if(s >= 1.0){
							voxSet((x + xx[j]) & clip_mask_x,(y + yy[j]) & clip_mask_y,h);
							s -= 1.0;
							}
					break;
				case 1:
					v = (int)(dd*1000000.0);
					for(j = 0;j < max;j++)
						if((int)XRnd(1000000) < v) voxSet((x + xx[j]) & clip_mask_x,(y + yy[j]) & clip_mask_y,h);
					break;
				case 2:
					v = (int)(dd*max);
					for(k = 0,j = locp%max;k < v;j = (j + 1 == max) ? 0 : j + 1,k++)
						voxSet((x + xx[j]) & clip_mask_x,(y + yy[j]) & clip_mask_y,h);
					locp += max;
					break;
				}
		}
		locp++;
	}
	else {
		int cx,h,cy,cx_;
		if(eql){
			mean = k = 0;
			for(i=0; i <= r; i++){
				max = maxRad[i];
				xx = xRad[i];
				yy = yRad[i];
				for(j = 0;j < max;j++){
					cx = XCYCL(x + xx[j]);
					mean += GetAlt(cx,YCYCL(y+yy[j]));
				}
				k += max;
			}
			mean /= k;
			for(i=0; i <= r; i++){
				max = maxRad[i];
				xx = xRad[i];
				yy = yRad[i];
				for(j = 0;j < max;j++){
					cy=YCYCL(y+yy[j]);
					cx = XCYCL(x + xx[j]);
					h = GetAlt(cx,cy);
					if(abs(h - mean) < eql) {
                        if (h > mean) voxSet(cx, cy, -1);
                        else if (h < mean) voxSet(cx, cy, 1);
                    }
				}
			}
			for(i=r+1, dd=1.0-d; i <= rad; i++,dd -= d){
				max = maxRad[i];
				xx = xRad[i];
				yy = yRad[i];
				h = (int)(dd*dh);
				if(!h) h = dh > 0 ? 1 : -1;

				v = (int)(dd*max);
				ds = (double)v/(double)max;
				for(s = ds,k = 0,j = locp%max;k < max;j = (j + 1 == max) ? 0 : j + 1,k++,s += ds)
					if(s >= 1.0){
						cy=YCYCL(y+yy[j]);
						cx = XCYCL(x + xx[j]);
						h = GetAlt(cx,cy);
						if(abs(h - mean) < eql) {
                            if (h > mean) voxSet(cx, cy, -1);
                            else if (h < mean) voxSet(cx, cy, 1);
                        }
						s -= 1.0;
						}
			}
		}
		else {
			int dx,dy;
			for(i = 0; i <= r; i++){
				max = maxRad[i];
				xx = xRad[i];
				yy = yRad[i];
				for(j = 0;j < max;j++){
					cy=YCYCL(y+yy[j]);
					cx = XCYCL(x + xx[j]);
					h = GetAlt(cx,cy);
					v = 0;
					switch(smode){
						case 0:
							for(dy = -1; dy <= 1; dy++)
								for(dx = -1;dx <= 1;dx++){
									cx_ = XCYCL(cx+dx);
									v += GetAlt(cx_,YCYCL(cy +dy));
								}
							v -= h;
							v >>= 3;
							break;
						case 1:
						case 2:
							for(dy = -1; dy <= 1; dy++){
								for(dx = -1;dx <= 1;dx++){
									cx_ = XCYCL(cx+dx);
									if(abs(dx) + abs(dy) == 2)
										v += GetAlt(cx_,YCYCL(cy +dy));
								}
							}
							v >>= 2;
							break;
					}
					voxSet(cx,cy,v - h);
				}
			}
			for(i=r+1,dd = 1.0-d; i <= rad; i++,dd -= d){
				max = maxRad[i];
				xx = xRad[i];
				yy = yRad[i];
				h = (int)(dd*dh);
				if(!h) h = dh > 0 ? 1 : -1;

				v = (int)(dd*max);
				ds = (double)v/(double)max;
				for(s = ds,k = 0,j = locp%max; k < max; j = (j + 1 == max) ? 0 : j + 1,k++,s += ds){
					if(s >= 1.0){
						cy=YCYCL(y+yy[j]);
						cx = XCYCL(x + xx[j]);
						h = GetAlt(cx,cy);
						v = 0;
						switch(smode){
							case 0:
								for(dy = -1;dy <= 1;dy++){
									for(dx = -1;dx <= 1;dx++){
										cx_ = XCYCL(cx+dx);
										v += GetAlt(cx_,YCYCL(cy +dy));
									}
								}
								v -= h;
								v >>= 3;
								break;
							case 1:
							case 2:
								for(dy = -1;dy <= 1;dy++){
									for(dx = -1;dx <= 1;dx++){
										cx_ = XCYCL(cx+dx);
										if(abs(dx) + abs(dy) == 2)
											v += GetAlt(cx_,YCYCL(cy +dy));
									}
								}
								v >>= 2;
								break;
						}
						voxSet(cx,cy,v - h);
						s -= 1.0;
					}
				}
			}
		}
	}

	regRender(x - rad,y - rad,x + rad+1,y + rad+1);
}
///////////////////////////////////////////////
void vrtMap::squareDeltaZone(sSquareToolzerPMO& var)//(int x,int y,int rad,int smth,int dh,int smode,int eql)
{
	int x=var.x; int y=var.y; int rad=var.rad; int smth=var.smth; int dh=var.dh; int smode=var.smode; int eql=var.eql;
	if(rad > MAX_RADIUS_CIRCLEARR){
		xassert(0&&"exceeding max radius in squaredeltaZone ");
		rad=MAX_RADIUS_CIRCLEARR;
	}

	if(flag_record_operation) {
		containerPMO.push_back( ElementPMO(var) );
	}
	UndoDispatcher_PutPreChangedArea(x - rad,y - rad,x + rad +1,y + rad+1);

	static int locp;

	int i,j;
	int max;
	int* xx,*yy;

	int r = rad - rad*smth/10;
	double d = 1.0/(rad - r + 1),dd,ds,s;
	int v,h,k;

	if(dh){

		voxSet((x) & clip_mask_x, (y ) & clip_mask_y, dh);
		for(i = 1;i <= r;i++){
			//max = maxRad[i];
			//xx = xRad[i];
			//yy = yRad[i];
			//for(j = 0;j < max;j++) voxSet((x + xx[j]) & clip_mask_x,(y + yy[j]) & clip_mask_y,dh);
			for(j=-i; j<=i; j++){
				voxSet((x + j) & clip_mask_x,(y + -i) & clip_mask_y, dh);
				voxSet((x + j) & clip_mask_x,(y + +i) & clip_mask_y, dh);
			}
			for(j=-i+1; j<i; j++){
				voxSet((x + +i) & clip_mask_x,(y + j) & clip_mask_y, dh);
				voxSet((x + -i) & clip_mask_x,(y + j) & clip_mask_y, dh);
			}
		}
		for(i = r + 1,dd = 1.0 - d;i <= rad;i++,dd -= d){
			//max = maxRad[i];
			//xx = xRad[i];
			//yy = yRad[i];
			max=8*r;
			h = (int)(dd*dh);
			if(!h) h = dh > 0 ? 1 : -1;
			v = (int)(dd*1000000.0);
			//for(j = 0;j < max;j++) {
			//	if((int)XRnd(1000000) < v) voxSet((x + xx[j]) & clip_mask_x,(y + yy[j]) & clip_mask_y,h);
			//}
			for(j=-i; j<=i; j++){
				if((int)XRnd(1000000) < v) voxSet((x + j) & clip_mask_x,(y + -i) & clip_mask_y, h);
				if((int)XRnd(1000000) < v) voxSet((x + j) & clip_mask_x,(y + +i) & clip_mask_y, h);
			}
			for(j=-i+1; j<i; j++){
				if((int)XRnd(1000000) < v) voxSet((x + +i) & clip_mask_x,(y + j) & clip_mask_y, h);
				if((int)XRnd(1000000) < v) voxSet((x + -i) & clip_mask_x,(y + j) & clip_mask_y, h);
			}

		}
		locp++;
	}
	else {
		int cx,h,cy,cx_;
		int dx,dy;
		for(i = 0;i <= r;i++){
			max = maxRad[i];
			xx = xRad[i];
			yy = yRad[i];
			for(j = 0;j < max;j++){
				cy=YCYCL(y+yy[j]);
				cx = XCYCL(x + xx[j]);
				h = GetAlt(cx,cy);
				v = 0;
				switch(smode){
					case 0:
						for(dy = -1;dy <= 1;dy++)
							for(dx = -1;dx <= 1;dx++){
								cx_ = XCYCL(cx+dx);
								v += GetAlt(cx_,YCYCL(cy +dy));
							}
						v -= h;
						v >>= 3;
						break;
					case 1:
					case 2:
						for(dy = -1;dy <= 1;dy++){
							for(dx = -1;dx <= 1;dx++){
								cx_ = XCYCL(cx+dx);
								if(abs(dx) + abs(dy) == 2)
									v += GetAlt(cx_,YCYCL(cy +dy));
							}
						}
						v >>= 2;
						break;
				}
				voxSet(cx,cy,v - h);
			}
		}
		for(i = r + 1,dd = 1.0 - d;i <= rad;i++,dd -= d){
			max = maxRad[i];
			xx = xRad[i];
			yy = yRad[i];
			h = (int)(dd*dh);
			if(!h) h = dh > 0 ? 1 : -1;

			v = (int)(dd*max);
			ds = (double)v/(double)max;
			for(s = ds,k = 0,j = locp%max;k < max;j = (j + 1 == max) ? 0 : j + 1,k++,s += ds){
				if(s >= 1.0){
					cy=YCYCL(y+yy[j]);
					cx = XCYCL(x + xx[j]);
					h = GetAlt(cx,cy);
					v = 0;
					switch(smode){
						case 0:
							for(dy = -1;dy <= 1;dy++){
								for(dx = -1;dx <= 1;dx++){
									cx_ = XCYCL(cx+dx);
									v += GetAlt(cx_,YCYCL(cy +dy));
								}
							}
							v -= h;
							v >>= 3;
							break;
						case 1:
						case 2:
							for(dy = -1;dy <= 1;dy++){
								for(dx = -1;dx <= 1;dx++){
									cx_ = XCYCL(cx+dx);
									if(abs(dx) + abs(dy) == 2)
										v += GetAlt(cx_,YCYCL(cy +dy));
								}
							}
							v >>= 2;
							break;
					}
					voxSet(cx,cy,v - h);
					s -= 1.0;
				}
			}
		}
	}

	regRender(x - rad,y - rad,x + rad+1,y + rad+1);
}

///////////////////////////////////////////////
void vrtMap::gaussFilter(int _x,int _y,int _rad, double _filter_scaling)
//(int * alt_buff, double filter_scaling, int x_size, int y_size)
{
	const int Diameter=2*_rad+1;
	const int xBeg=XCYCL(_x-_rad);
	const int yBeg=YCYCL(_y-_rad);

	if(flag_record_operation) {
        WrapPMO pmo = WrapPMO<sBlurPMO>(_x,_y,_rad,_filter_scaling);
		containerPMO.push_back( ElementPMO(pmo) );
	}
	UndoDispatcher_PutPreChangedArea(xBeg, yBeg, XCYCL(xBeg + Diameter), YCYCL(yBeg + Diameter));
	//Блок обработки радиуса
	static int store_radius=0;
	static short *y2x=0;
	if(store_radius!=_rad){
		if(y2x) delete [] y2x;
		y2x=new short [_rad+1];
		store_radius=_rad;
		int m;
		for(m=0; m<=_rad; m++){
			y2x[m]=round(sqrtf(_rad*_rad - m*m));
		}
	}
	//int border_mask_x=x_size-1;
	//int border_mask_y=y_size-1;
	const int H = 4;//4
	double filter_array[2*H][2*H];
	int x,y;
	double f,norma = 0;
	double filter_scaling_inv_2 = sqr(1/_filter_scaling);
	for(y = -H;y < H;y++)
		for(x = -H;x < H;x++){
			f = exp(-(sqr((double)x) + sqr((double)y))*filter_scaling_inv_2);
			norma += f;
			filter_array[H + y][H + x] = f;
			}
	double norma_inv = 1/norma;
	//cout << "Gaussian filter, factor: " << filter_scaling <<endl;

	int* new_alt_buff = new int[Diameter*Diameter];
	memset(new_alt_buff, 0, Diameter*Diameter*sizeof(int));

	int xx, yy;
	for(yy = 0; yy < Diameter; yy++){
		int dx=y2x[abs(_rad-yy)]*2;
		int bx=_rad - y2x[abs(_rad-yy)];
		for(xx = bx; xx < dx+bx; xx++){
			f = 0;
			for(y = -H;y < H;y++)
				for(x = -H;x < H;x++){
					unsigned short alt=GetAlt(XCYCL(xBeg + xx + x), YCYCL(yBeg+yy+y));
					f += filter_array[H + y][H + x]*double(alt);
					}
			unsigned char c = round(f*norma_inv);
			new_alt_buff[((yy)*Diameter) + (xx)] = round(f*norma_inv)-GetAlt(XCYCL(xBeg+xx), YCYCL(yBeg+yy));
			}
	}
	//cout <<endl<<endl;
	//memcpy(alt_buff,new_alt_buff,x_size*y_size*sizeof(int));
	for(yy = 0;yy < Diameter; yy++){
		for(xx = 0;xx < Diameter; xx++){
			if(new_alt_buff[((yy)*Diameter) + (xx)])voxSet(XCYCL(xBeg+xx), YCYCL(yBeg+yy), new_alt_buff[((yy)*Diameter) + (xx)]);
			}
	}
	delete new_alt_buff;
	regRender(xBeg, yBeg, XCYCL(xBeg + Diameter), YCYCL(yBeg + Diameter));
}
////////////////////////////////
void vrtMap::squareGaussFilter(int _x,int _y,int _rad, double _filter_scaling)
//(int * alt_buff, double filter_scaling, int x_size, int y_size)
{
	const int Diameter=2*_rad+1;
	const int xBeg=XCYCL(_x-_rad);
	const int yBeg=YCYCL(_y-_rad);

	UndoDispatcher_PutPreChangedArea(xBeg, yBeg, XCYCL(xBeg + Diameter), YCYCL(yBeg + Diameter));
	//int border_mask_x=x_size-1;
	//int border_mask_y=y_size-1;
	const int H = 4;//4
	double filter_array[2*H][2*H];
	int x,y;
	double f,norma = 0;
	double filter_scaling_inv_2 = sqr(1/_filter_scaling);
	for(y = -H;y < H;y++)
		for(x = -H;x < H;x++){
			f = exp(-(sqr((double)x) + sqr((double)y))*filter_scaling_inv_2);
			norma += f;
			filter_array[H + y][H + x] = f;
			}
	double norma_inv = 1/norma;
	//cout << "Gaussian filter, factor: " << filter_scaling <<endl;

	int* new_alt_buff = new int[Diameter*Diameter];
	memset(new_alt_buff, 0, Diameter*Diameter*sizeof(int));

	int xx, yy;
	for(yy = 0; yy < Diameter; yy++){
		for(xx = 0; xx < Diameter; xx++){
			f = 0;
			for(y = -H;y < H;y++)
				for(x = -H;x < H;x++){
					unsigned short alt=GetAlt(XCYCL(xBeg + xx + x), YCYCL(yBeg+yy+y));
					f += filter_array[H + y][H + x]*double(alt);
					}
			unsigned char c = round(f*norma_inv);
			new_alt_buff[((yy)*Diameter) + (xx)] = round(f*norma_inv)-GetAlt(XCYCL(xBeg+xx), YCYCL(yBeg+yy));
			}
	}
	//cout <<endl<<endl;
	//memcpy(alt_buff,new_alt_buff,x_size*y_size*sizeof(int));
	for(yy = 0;yy < Diameter; yy++){
		for(xx = 0;xx < Diameter; xx++){
			if(new_alt_buff[((yy)*Diameter) + (xx)])voxSet(XCYCL(xBeg+xx), YCYCL(yBeg+yy), new_alt_buff[((yy)*Diameter) + (xx)]);
		}
	}
	delete new_alt_buff;
	regRender(xBeg, yBeg, XCYCL(xBeg + Diameter), YCYCL(yBeg + Diameter));
}


void vrtMap::AllworldGaussFilter(double _filter_scaling)
{
	UndoDispatcher_PutPreChangedArea(0, 0, H_SIZE-1, V_SIZE-1);

	const int H = 4;//4
	double filter_array[2*H][2*H];
	int x,y;
	double f,norma = 0;
	double filter_scaling_inv_2 = sqr(1/_filter_scaling);
	for(y = -H;y < H;y++)
		for(x = -H;x < H;x++){
			f = exp(-(sqr((double)x) + sqr((double)y))*filter_scaling_inv_2);
			norma += f;
			filter_array[H + y][H + x] = f;
			}
	double norma_inv = 1/norma;

	int* new_alt_buff = new int[H_SIZE*V_SIZE];
	memset(new_alt_buff, 0, H_SIZE*V_SIZE*sizeof(int));

	int xx, yy;
	for(yy = 0; yy < V_SIZE; yy++){
		for(xx = 0; xx < H_SIZE; xx++){
			f = 0;
			for(y = -H;y < H;y++){
				short yV=yy+y; if(yV>=V_SIZE)yV=V_SIZE-1; if(yV<0) yV=0;
				for(x = -H;x < H;x++){
					short xV=xx+x; if(xV>=H_SIZE)xV=H_SIZE-1; if(xV<0)xV=0;
					//unsigned short alt=GetAlt(XCYCL(xx + x), YCYCL(yy+y));
					unsigned short alt=GetAlt(xV, yV);
					f += filter_array[H + y][H + x]*double(alt);
				}
			}
			unsigned char c = round(f*norma_inv);
			new_alt_buff[((yy)*H_SIZE) + (xx)] = round(f*norma_inv)-GetAlt(XCYCL(xx), YCYCL(yy));
		}
	}
	for(yy = 0;yy < V_SIZE; yy++){
		for(xx = 0;xx < H_SIZE; xx++){
			voxSet(XCYCL(xx), YCYCL(yy), new_alt_buff[((yy)*H_SIZE) + (xx)]);
			}
	}
	delete new_alt_buff;
	regRender(0, 0, H_SIZE-1, V_SIZE-1);
}







void vrtMap::GeoSetZone(int x, int y, int rad, int level, int delta)
{
	if(rad > MAX_RADIUS_CIRCLEARR){
		xassert(0&&"exceeding max radius in GeoSetZone ");
		rad=MAX_RADIUS_CIRCLEARR;
	}

	int i,j;
	int max;
	int* xx,*yy;

	GeoPoint(0,0,0,0,1);

	for(i = 0;i <= rad;i++){
		max = maxRad[i];
		xx = xRad[i];
		yy = yRad[i];
		for(j = 0;j < max;j++)
			GeoPoint((x + xx[j]) & vMap.clip_mask_x,(y + yy[j]) & vMap.clip_mask_y,level,delta,0);
		}

	GeoPoint(0,0,level,delta,2);//здесь дельта и левел нобходимы для правильного масштабирования по измененной поверхности
}





void vrtMap::WorldRender(void)
{
	for(unsigned int i=0; i<V_SIZE; i++){
		changedT[i] = 1;
		RenderStr(i);
	}
//	RenderShadovM3DAll();

}
void vrtMap::RenderRegStr(int Yh,int Yd)
{
	int i=Yh;
	int j=YCYCL(Yd+1);
	while (i!=j)
	{
		RenderStr(i);
		i=YCYCL(++i); // РАЗНИЦА МЕЖДУ i++ ++i тут ОГРОМНАЯ (2048 не зацикливается)
	}
}

extern void UpdateRegionMap(int x1,int y1,int x2,int y2);
int vrtMap::renderBox(int LowX,int LowY,int HiX,int HiY, int changed)//int sizeX,int sizeY
{
	//int sizeX=HiX-LowX;
	//int sizeY=HiY-LowY;
	//int sizeY = (LowY == HiY) ? (V_SIZE-1) : (HiY - LowY);
	//int sizeX = (0 == (HiX - LowX)) ? (H_SIZE-1) : (HiX - LowX);
	int sizeY,sizeX; 
	if(LowY == HiY){ sizeY=V_SIZE-1; LowY=0; HiY=V_SIZE-1;}
	else { sizeY= HiY - LowY;}
	if(LowX == HiX){ sizeX=H_SIZE-1; LowX=0; HiX=H_SIZE-1;}
	else { sizeX= HiX - LowX;}
	int minX=LowX;
#ifdef _PERIMETER_
//#ifdef KRON_CRITICAL_SECTION
	sRect ChRrAr(LowX, LowY, sizeX, sizeY);
	changedAreas.push_back(ChRrAr);
	renderAreas.push_back(ChRrAr);

	int minXG=LowX>>kmGridChA;
	int minYG=LowY>>kmGridChA;
	int maxXG=(LowX+sizeX)>>kmGridChA;
	int maxYG=(LowY+sizeY)>>kmGridChA;
	int hSizeGCA=(H_SIZE>>kmGridChA);
	int i,j;
	for(i=minYG; i<=maxYG; i++){
		for(j=minXG; j<=maxXG; j++){
			gridChAreas[j+i*hSizeGCA]=1;
		}
	}

#else
	int j;
	for(j = 0;j <= sizeY;j++){
		const int y = YCYCL(j + LowY);
		if(changed) changedT[y] = 1;
		int resultx=RenderStr(LowX, y, sizeX);
		if(minX>resultx)minX=resultx;
	}
	int minXG=minX>>kmGridChA;
	int minYG=LowY>>kmGridChA;
	int maxXG=(LowX + sizeX)>>kmGridChA;
	int maxYG=(LowY + sizeY)>>kmGridChA;
	int hSizeGCA=(H_SIZE>>kmGridChA);
	int i;
	for(i=minYG; i<=maxYG; i++){
		for(j=minXG; j<=maxXG; j++){
			gridChAreas[j+i*hSizeGCA]=1;
		}
	}
	UpdateRegionMap(LowX, LowY, LowX+sizeX, LowY+sizeY);
#endif

	return minX;
}

#ifdef _PERIMETER_
//#include "Statistics.h"
#endif

static bool operator< (const sRect& lo, const sRect& ro){
	return lo.y < ro.y;
};

struct xSort {
	sRect* pRA;
	bool operator< (const xSort& ro)const{
		return (pRA->x) < (ro.pRA->x);
	};
};
struct optRA {
	unsigned short begx, endx;
	unsigned short minx;
};

void vrtMap::renderQuant()
{
#ifdef _PERIMETER_
#ifndef _GEOTOOL_
//	start_timer_auto(TerrainRender, STATISTICS_GROUP_TOTAL);
#endif //_GEOTOOL_

	int nReg=0, optNReg=0;
	double sReg=0., optSReg=0.;

	int listSize=renderAreas.size();
	if(!listSize) return;
	vector<sRect> RAVec(listSize);
	list<sRect>::iterator q;
	int i;
	int j,k;
	for(i=0, q=renderAreas.begin(); i<listSize; i++, q++){
		int x=(*q).x;
		RAVec[i].x=x;
		int y=(*q).y;
		RAVec[i].y=y;
		int dx=(*q).dx;
		RAVec[i].dx=dx;
		int dy=(*q).dy;
		RAVec[i].dy=dy;
	}
	sort(RAVec.begin(), RAVec.end());
	//for(i=listSize-1; i>=0; i--){}

	vector<xSort> XSVec;
	XSVec.reserve(100);
	vector<optRA> optimizeRAS;
	vector<optRA>::iterator op;
	optimizeRAS.reserve(10);
	int begRectCS=0, endRectCS=0;
	int curY=RAVec[0].y;
	unsigned char flag_change_RectArea=1;
	int begY, endY;
	for(begY=endY=curY=RAVec[0].y; curY<vMap.V_SIZE; curY++){
		while( ((endRectCS+1)<listSize) && (RAVec[endRectCS+1].y<=curY) ) {
			endRectCS=endRectCS+1; flag_change_RectArea=1;
		}
		while( ((RAVec[begRectCS].y+RAVec[begRectCS].dy) < curY) && (begRectCS<listSize) ) {
			begRectCS++; 
			if(begRectCS>=listSize) goto loc_endRnrCycl; 
			flag_change_RectArea=1;
		}
		if(curY<RAVec[begRectCS].y) curY=RAVec[begRectCS].y;
		if(flag_change_RectArea){
			for(op=optimizeRAS.begin(); op!=optimizeRAS.end(); op++){
				UpdateRegionMap(op->minx, begY, op->endx, endY );
				//optSReg+=(op->endx-op->minx)* (endY-begY);
				optSReg+=(op->endx-op->begx)* (endY-begY);
				optNReg++;
			}
			XSVec.clear();
			optimizeRAS.clear();
			for(j=begRectCS,k=0; j<=endRectCS; j++, k++) {
				XSVec.push_back(xSort());//вставка пустого элемента
				XSVec[k].pRA=&RAVec[j];
			}
			sort(XSVec.begin(), XSVec.end());
			int countRA=k;
			int idxBeginX, idxEndX;
			for(j=0,k=0, idxBeginX=idxEndX=0; j<countRA; j++, k++){
				int maxy=XSVec[idxBeginX].pRA->x + XSVec[idxBeginX].pRA->dx;
				while( ((j+1)<countRA) && (XSVec[j].pRA->x+XSVec[j].pRA->dx+RENDER_BORDER_ZONE) > XSVec[j+1].pRA->x ) { 
					j++; idxEndX++;
					if(maxy<XSVec[idxEndX].pRA->x + XSVec[idxEndX].pRA->dx) maxy=XSVec[idxEndX].pRA->x + XSVec[idxEndX].pRA->dx;
				}
				optimizeRAS.push_back(optRA()); //вставка пустого элемента
				optimizeRAS[k].begx=XSVec[idxBeginX].pRA->x;
				optimizeRAS[k].endx=maxy;
				optimizeRAS[k].minx=optimizeRAS[k].begx;
				idxBeginX=idxEndX=idxEndX+1;
			}
			begY=curY;
		}
		for(op=optimizeRAS.begin(); op!=optimizeRAS.end(); op++){
			int resultx=RenderStr(op->begx, curY, (op->endx - op->begx) );
			if(op->minx>resultx)op->minx=resultx;
		}
		endY=curY;

		flag_change_RectArea=0;
	}
loc_endRnrCycl:
	for(op=optimizeRAS.begin(); op!=optimizeRAS.end(); op++){
		UpdateRegionMap(op->minx, begY, op->endx, endY );
		//optSReg+=(op->endx-op->minx)* (endY-begY);
		optSReg+=(op->endx-op->begx)* (endY-begY);
		optNReg++;
	}

/*
	list<sRect>::iterator ra;
	FOR_EACH(renderAreas,ra){
		int minX=ra->x;
		int j;
		//for(j = 0; j<= ra->dy; j++){
		//	const int y = YCYCL(j + ra->y);
		//	int resultx=RenderStr(ra->x, y, ra->dx);
		//	if(minX>resultx)minX=resultx;
		//}
		//int minXG=minX>>kmGridChA;
		int minXG=ra->x>>kmGridChA;
		int minYG=ra->y>>kmGridChA;
		int maxXG=(ra->x+ra->dx)>>kmGridChA;
		int maxYG=(ra->y+ra->dy)>>kmGridChA;
		int hSizeGCA=(H_SIZE>>kmGridChA);
		int i;
		for(i=minYG; i<=maxYG; i++){
			for(j=minXG; j<=maxXG; j++){
				gridChAreas[j+i*hSizeGCA]=1;
			}
		}
		//extern void UpdateRegionMap(int x1,int y1,int x2,int y2);
		//UpdateRegionMap(minX, ra->y, (ra->x+ra->dx), (ra->y+ra->dy) );
		nReg++;
		sReg+=ra->dx*ra->dy;
	}
*/
	renderAreas.clear();
//	statistics_add(NRegions, 10, nReg);
//	statistics_add(SRegions, 10, sReg);
//	statistics_add(OptimizeNReg, 10, optNReg);
//	statistics_add(OptimizeSReg, 10, optSReg);
#endif
}

void vrtMap::regRender(int LowX,int LowY,int HiX,int HiY,int changed)
{
	LowX = XCYCL(LowX);
	HiX = XCYCL(HiX);
	LowY = YCYCL(LowY);
	HiY = YCYCL(HiY);

//	LowX &= ~1;
//	HiX |= 1;

//	int SizeY = (LowY == HiY) ? (V_SIZE-1) : (HiY - LowY);
//	int SizeX = (0 == XCYCL(HiX - LowX)) ? (H_SIZE-1) : (HiX - LowX);

//	int BackScanLen = 0;
/*	int minX=LowX;
	int j;
	for(j = 0;j <= SizeY;j++){
		const int y = YCYCL(j + LowY);
		if(changed) changedT[y] = 1;
		int resultx=RenderStr(LowX, y, SizeX);
		if(minX>resultx)minX=resultx;
	}*/
/*	LowX=minX;*/
	int mx; //СЕЙЧАС НЕ ИСПОЛЬЗУЕТСЯ !
	extern void UpdateRegionMap(int x1,int y1,int x2,int y2);
	if(LowX > HiX){
		if(LowY> HiY){
			mx=renderBox(LowX, LowY, H_SIZE-1, V_SIZE-1, changed);
			///UpdateRegionMap(mx, LowY, H_SIZE-1, V_SIZE-1);
			mx=renderBox(0, 0, HiX, HiY, changed);
			///UpdateRegionMap(0, 0, HiX, HiY);
			mx=renderBox(0, LowY, HiX, V_SIZE-1, changed);
			///UpdateRegionMap(0, LowY, HiX, V_SIZE-1);
			mx=renderBox(LowX, 0, H_SIZE-1, HiY, changed);
			///UpdateRegionMap(mx, 0, H_SIZE-1, HiY);
		}
		else {
			mx=renderBox(LowX, LowY, H_SIZE-1, HiY, changed);
			///UpdateRegionMap(mx, LowY, H_SIZE-1, HiY);
			mx=renderBox(0, LowY, HiX, HiY, changed);
			///UpdateRegionMap(0, LowY, HiX, HiY);
		}
	}
	else { //С X все впорядке
		if(LowY> HiY){
			mx=renderBox(LowX, LowY, HiX, V_SIZE-1, changed);
			///UpdateRegionMap(mx, LowY, HiX, V_SIZE-1);
			mx=renderBox(LowX, 0, HiX, HiY, changed);
			///UpdateRegionMap(mx, 0, HiX, HiY);
		} 
		else {
			mx=renderBox(LowX, LowY, HiX, HiY, changed);
			///UpdateRegionMap(mx, LowY, HiX, HiY);
		}
	}
//#endif
	worldChanged=1;

}


 //old version RenderPrepare1
/*const int gamut=1<<11; // (1<<11)-1 Это середина (2*gamut-размер) таблицы разниц высот
//const int fraction=1<<6; //(1<<6)-1
// begin 16bit version
void RenderPrepare1(void)
{

	int j,i,h_l1;
	float d_x,d_y,dz;
	double k;

	//if (h_l>255) h_l1=511-h_l;
	//else h_l1=h_l; 
	h_l1=h_l; 
	k=h_l1/(sqrt(257*257-h_l1*h_l1)); // k -это tg угла падения света
	delta_s=round(k*(1<<8)*(1<<VX_FRACTION)); 
	//Расчет суши
	for(i=0;i<TERRAIN_MAX;i++){
		h_l1=h_l; 
		d_x=terra.d_x[0][i];
		d_y=terra.d_y[0][i];
		for(j=0; j<gamut*2; j++){
			dz=(float)(j-gamut)/(1<<VX_FRACTION);
			k=( ((dz/sqrt(dz*dz+d_x*d_x)) * sqrt(255*255-h_l1*h_l1)/255 + d_x/sqrt(dz*dz+d_x*d_x) * h_l1/255) );
			if(k<0)k=0;
			terra.light_front[0][i][j]=round((double)(255-terra.ambient_light[0][i])*(sqrt(k*k)));//+l_ras
			terra.light_sideways[0][i][j]= round( (double) ((d_y)/sqrt(dz*dz+d_y*d_y)) *255 );
			terra.light_front[1][i][j]=round((double)(255-terra.ambient_light[0][i])*(sqrt(k*k)));//+l_ras
			terra.light_sideways[1][i][j]= round( (double) ((d_y)/sqrt(dz*dz+d_y*d_y)) *255 );
		}
//		int begzl=terra.light_front[0][i][256*3];
//		for(j=(256*3-1);j>0;j--){
//			terra.light_front[0][i][j]=begzl;
//			terra.light_sideways[0][i][j]=terra.light_sideways[0][i][512*3-j];
//			if(begzl>0)begzl--;
//		}

	}
}*/


void vrtMap::RenderStr(int Y)
{
	RenderStr(0,Y,XS_Buf-1);
}


////////////////////////////////////////////////////////////////////////
static float A_lightS, B_lightS, C_lightS;
static float A_vw, B_vw, C_vw;

static int iA_light, iB_light, iC_light;
static int iA_lightS, iB_lightS, iC_lightS;

static const unsigned char AmbientLight=50;//100;//50;//0;
//static const unsigned char AmbientLtSl=35;
static const unsigned char Light=255-AmbientLight;//180-AmbientLight;
//static const unsigned char LtSl=200;
static const unsigned char AmbientLightShadow=30;
static const unsigned char LightShadow=100-AmbientLightShadow;

static const unsigned char FLAG_SPECULAR_MATERIAL=0x1;
static const unsigned char SHIFT_4TYPE_MATERIALS=1;
static unsigned char geoType2Mater[MAX_GEO_SURFACE_TYPE];
static unsigned char damType2Mater[MAX_DAM_SURFACE_TYPE];
static const int MaxAmountMaterials=2;
static float kDifSM[MaxAmountMaterials]={ 0.7f, 0.5f };
static unsigned char LightDifSM[MaxAmountMaterials];  // /100
static unsigned char AmbientLightDifSM[MaxAmountMaterials];  // /100
static float kSpecSM[MaxAmountMaterials]={ 0.5f, 0.5f };  
static float nSpecSM[MaxAmountMaterials]={15.f, 5.f}; // 
static short COSnG[MaxAmountMaterials][512];
static float shL[32];
#ifdef _SURMAP_
unsigned char SLightTable[SLT_SIZE];
#endif

static float A_light=-234.;
static float B_light=20.;
static float C_light=-180.;//-240.;

void vrtMap::ShadowControl(bool shadow)
{
	shadow_control=shadow;
	if(shadow){
		double k=-C_light/(-A_light); // k -это tg угла падения света
		delta_s=round(k*(1<<16)*(1<<VX_FRACTION)); 
	}
	else{
		delta_s=MAX_VX_HEIGHT*(1<<16);///
	}
}

void vrtMap::RenderPrepare1(void)
{
	A_vw=0; B_vw=1; C_vw=1;
	float L_light=sqrt(A_light*A_light + B_light*B_light + C_light*C_light);
	A_light/=L_light; B_light/=L_light; C_light/=L_light;
	float L_vw=sqrt(A_vw*A_vw + B_vw*B_vw + C_vw*C_vw);
	A_vw/=L_vw; B_vw/=L_vw; C_vw/=L_vw;

	A_lightS=0;
	B_lightS=0.5f;
	C_lightS=-1;
	float L_lightS=sqrt(A_lightS*A_lightS + B_lightS*B_lightS + C_lightS*C_lightS);
	A_lightS/=L_lightS; B_lightS/=L_lightS; C_lightS/=L_lightS;

	iA_light=round(A_light*(1<<15));
	iB_light=round(B_light*(1<<15));
	iC_light=round(C_light*(1<<15));
	iA_lightS=round(A_lightS*(1<<15));
	iB_lightS=round(B_lightS*(1<<15));
	iC_lightS=round(C_lightS*(1<<15));

	int i,j;

	for(i=0; i<MAX_GEO_SURFACE_TYPE; i++) geoType2Mater[i]=(0<<SHIFT_4TYPE_MATERIALS)|(0&FLAG_SPECULAR_MATERIAL);
	for(i=0; i<MAX_DAM_SURFACE_TYPE; i++) damType2Mater[i]=(0<<SHIFT_4TYPE_MATERIALS)|(0&FLAG_SPECULAR_MATERIAL);

	//for(i=0; i<MAX_DAM_SURFACE_TYPE; i++) damType2Mater[i]=(0<<SHIFT_4TYPE_MATERIALS)|(1&FLAG_SPECULAR_MATERIAL);
	//damType2Mater[1]=(0<<SHIFT_4TYPE_MATERIALS)|(1&FLAG_SPECULAR_MATERIAL);

	for(i=0; i<MaxAmountMaterials; i++){
		LightDifSM[i]=round(kDifSM[i]*(float)Light);
		AmbientLightDifSM[i]=round(kDifSM[i]*(float)AmbientLight);
		for(j=-256; j<256; j++){
			//double m=pow((double)j/256,((double)ref_n[i]/10) )* ((double)ref_k[i]/100);
			double m=pow((double)j/256, (double)nSpecSM[i] )* ((double)kSpecSM[i]);
			COSnG[i][256+j]=round(m*256);
		}
	}

	double k=-C_light/(-A_light); // k -это tg угла падения света
	///delta_s=round(k*(1<<16)*(1<<VX_FRACTION)); 
	///delta_s=MAX_VX_HEIGHT*(1<<16);///
	ShadowControl(shadow_control);
	RENDER_BORDER_ZONE=abs(round(k*64.0)); //64 возможный перепад высот

	for(i=0; i<32; i++){
		shL[i]=(float)(31-i)/31.f;
	}

	extern void build_sqrt_table(void);
	build_sqrt_table();
	init_sqrtTable4IntegerCalculate();

#ifdef _SURMAP_
	float X_LIGHT=1;
	float Y_LIGHT=1;
	float lenght_LIGHT=sqrt(X_LIGHT*X_LIGHT+Y_LIGHT*Y_LIGHT);
	X_LIGHT/=lenght_LIGHT; Y_LIGHT/=lenght_LIGHT; //нормализация
	const unsigned char LUM_LIGHT=255;
	for(i=0; i<SLT_SIZE; i++){
		int dV=i-SLT_05SIZE;
		float x=dV;
		float y=1.0f;
		float l=1/sqrt(x*x + y*y);
		x*=l; y*=l;
		float scalar=x*X_LIGHT+ y*Y_LIGHT;
		if(scalar>0) SLightTable[i]=round(scalar*LUM_LIGHT);
		else SLightTable[i]=0;
	};
#endif

}

//static int STATUS_DEBUG_RND=0;
int vrtMap::RenderStr(int XL, int Y, int dx)
{
	if(XL>0) { XL--; dx++; }//для правильного расчета тени
	if((XL+dx) >= H_SIZE) {
		//ErrH.Abort("Incorrect render parameters");
		xassert(0&&"Incorrect render parameters");
		dx=0;
	}

//	eVxBufWorkMode backupVxBufWorkMode=VxBufWorkMode;//сохранение режима доступа к поверхности
//	VxBufWorkMode=GEOiDAM; //Установка режима доступа к самой верхней высоте
	Y=YCYCL(Y);

	//float ty=(float)Y/(V_SIZE>>1)-1.0f;
	//ty=ty*ty; ty=ty*ty; ty=ty*ty; ty=ty*ty;
	const unsigned char shift4Fraction=14;
	const unsigned char firstShift4FractionX=shift4Fraction-H_SIZE_POWER;
	const unsigned char firstShift4FractionY=shift4Fraction-V_SIZE_POWER;
	int tty=(Y*2-V_SIZE)<<firstShift4FractionY;
	tty=tty*tty>>shift4Fraction; tty=tty*tty>>shift4Fraction; tty=tty*tty>>shift4Fraction; //tty=tty*tty>>shift4Fraction;

	unsigned char *pa,*pc,*pah; 
	int offY=offsetBuf(0,Y);
	int offYh=offsetBuf(0,YCYCL(Y-1));
	pa = &(AtrBuf[Y*XS_Buf]);
	pah = &(AtrBuf[YCYCL(Y-1)*XS_Buf]);
	pc = &(RnrBuf[Y*XS_Buf]);

	int j=XL+dx;
	int SumHShadow=0;
	int h_s=0,vv,ob;
	if(j<(H_SIZE-1)){
		do{
			j++;
			ob=offY + XCYCL(j);
			//vv=GetAlt(ob)<<16;
			int whole=(VxDBuf[ob]);
			if(whole) vv= ( (whole<<VX_FRACTION)|(AtrBuf[ob]&VX_FRACTION_MASK) );
			else vv= ( (VxGBuf[ob]<<VX_FRACTION)|(AtrBuf[ob]&VX_FRACTION_MASK) );
			vv<<=16;

			vv-=SumHShadow; if(vv<0) vv=0;
			if(vv>h_s)h_s=vv;
			SumHShadow+=delta_s;
			//*(pa+XCYCL(j))|=At_ZEROPLAST; //For DEBUG
		//Проверка на окончание тени и высота снижения тени не уменьшала высоту максимально возможного вокселя ниже текущей высоты тени
		//Надо решить вопрос со смещением для GetAlt и AtrBuf-pa
		}while( ((*(pa+XCYCL(j))&At_SHADOW) !=0) && (((MAX_VX_HEIGHT<<16)-h_s) > SumHShadow) && j<H_SIZE ); //j<H_SIZE прверка на границу карты
	}
	//подразумевается что dx не выходит за границы мира и left point не выходит за ганицы мира
	j=dx; //+( 256*(1<<VX_FRACTION)*(1<<8)/delta_s )+1;
	unsigned short Vr,V,Vh;
	int i;
	Vr=SGetAlt(XCYCL(XL+dx+1),Y);
	unsigned char prevShadow=0;
	if( (GetAtr(XCYCL(XL+dx+1),Y)&At_SHADOW)!=0 ) prevShadow=1;
	do{
		do{
		//for(j;j>=0;j--){} 
			i=XCYCL(j+XL);

			if (h_s > delta_s) {h_s -=delta_s;}
			else {h_s = 0;}

			//затенение по краям
			//float tx=(float)i/(H_SIZE>>1)-1.0f;
			//tx=tx*tx; tx=tx*tx; tx=tx*tx; tx=tx*tx;
			//float f;//exp(-(tx+ty)/(.7f*.7f));
			//f=1-(tx+ty);
			//затенение по краям
			int ttx=(i*2-H_SIZE)<<firstShift4FractionX;
			ttx=ttx*ttx>>shift4Fraction; ttx=ttx*ttx>>shift4Fraction; ttx=ttx*ttx>>shift4Fraction; //ttx=ttx*ttx>>shift4Fraction;
			int f=(1<<shift4Fraction)-(ttx+tty);

			//Обработка зеропласта
			//if( ( (*(pa+i)) &At_ZEROPLAST)!=0 ) SurBuf[offY+i]=0xFF;//127
			//if( ( (*(pa+i)) &At_ZEROPLASTEMPTY)!=0 ) SurBuf[offY+i]=0xFE;
			//Обнуление необходимо чтоб небыло переполнение адреса в SurZP2Col и SurZPE2Col т.к. они расчитаны только на гео слой
			//if( (*(pa+i) &At_ZEROPLAST)!=0  ||  (*(pa+i) &At_ZEROPLASTEMPTY)!=0 ) VxDBuf[offY+i]=0;

			//h_s = 0;
			unsigned char terrain=GetTer(offY+i);

			//V=GetAlt(offY+i);
			//Vh=GetAlt(offYh+i);
			unsigned char material;
			int whole=(VxDBuf[offY+i]);
			if(whole) { //Dam слой
				V= ( (whole<<VX_FRACTION)|(AtrBuf[offY+i]&VX_FRACTION_MASK) );
				material=damType2Mater[terrain];
			}
			else { //Geo слой
				V= ( (VxGBuf[offY+i]<<VX_FRACTION)|(AtrBuf[offY+i]&VX_FRACTION_MASK) );
				material=geoType2Mater[terrain];
			}
			///////
			whole=(VxDBuf[offYh+i]);
			if(whole) { //Dam слой
				Vh= ( (whole<<VX_FRACTION)|(AtrBuf[offYh+i]&VX_FRACTION_MASK) );
			}
			else { //Geo слой
				Vh= ( (VxGBuf[offYh+i]<<VX_FRACTION)|(AtrBuf[offYh+i]&VX_FRACTION_MASK) );
			}


			*(pa+i)&=~At_SHADOW;

//расчет - full int
			int A=((Vr<<2)-(V<<2)),B=((V<<2)-(Vh<<2)), C=-(1<<8); //C=-1<<7//7дробных! //сдвиг на 8для большей плавности рельефа т.к. точность была потеряна при переходе на VX_FRACTION=5
			//float A=(float)(Vr-V)/(float)(1<<VX_FRACTION);
			//float B=(float)(V-Vh)/(float)(1<<VX_FRACTION);
			//float C=-1;

			//int L=round(sqrt(A*A+B*B+C*C)); 
			///extern inline float fastsqrt(float n);
			///int L=round(fastsqrt(A*A+B*B+C*C));
			int L=fastsqrtI(A*A+B*B+C*C);

			//A=(A<<8)/L; B=(B<<8)/L; C=(C<<8)/L;

			int icosA;//=(A*iA_light + B*iB_light + C*iC_light)/L;
//			float cosA;
//			cosA=(A*A_light + B*B_light + C*C_light)/256.;
			int lght;
			if((material&FLAG_SPECULAR_MATERIAL)==0){//только диффузный цвет
				material>>=SHIFT_4TYPE_MATERIALS;
				if( h_s > ((int)V<<16)) {
					//float cosAS=(A*A_lightS + B*B_lightS + C*C_lightS)/256.;
					int icosAS=(A*iA_lightS + B*iB_lightS + C*iC_lightS)/L;
					int shI=(h_s-((int)V<<16))>>(16+2); 
					//if(shI > 31) shI=31;
					//lght=round(AmbientLight*cosAS*shL[31-shI]) + round((Light*cosA+AmbientLight)*shL[shI]);
					//lght=( (AmbientLight*icosAS*shI) + ((Light*icosA+(AmbientLight<<15))*(31-shI)) )>>(15+5);
					if(shI>=31){
						lght=(LightShadow*icosAS) >>15;
						lght+=AmbientLightShadow;
					}
					else {
						icosA=(A*iA_light + B*iB_light + C*iC_light)/L;
						lght=( ((LightShadow*icosAS+(AmbientLightShadow<<15))*shI) + ((Light*icosA+(AmbientLight<<15))*(31-shI)) )>>(15+5);
					}
					prevShadow=1;
					*(pa+i)|=At_SHADOWV;
				}
				else{
					icosA=(A*iA_light + B*iB_light + C*iC_light)/L;
					//lght=round(Light*cosA);// + round(AmbientLt*cosAS);
					lght=(Light*icosA)>>15;// + round(AmbientLt*cosAS);
					lght+=AmbientLight;
					prevShadow=0;
				}
			}
			else{ //диф+спекулар
			}

//####################################
/*
//расчет full float
			float A=(float)(Vr-V)/(float)(1<<(VX_FRACTION));
			float B=(float)(V-Vh)/(float)(1<<(VX_FRACTION));
			float C=-2; //C=-1 // 2 для большей плавности рельефа т.к. точность была потеряна при переходе на VX_FRACTION=5
			float L=sqrt(A*A+B*B+C*C);
			//extern inline float fastsqrt(float n);
			//float L=fastsqrt(A*A+B*B+C*C);
			A=A/L; B=B/L; C=C/L;

			//int L=round(sqrt(A*A+B*B+C*C)); 
			///extern inline float fastsqrt(float n);
			///int L=round(fastsqrt(A*A+B*B+C*C));

			float cosA;
			cosA=(A*A_light + B*B_light + C*C_light);
			int lght;
			if((material&FLAG_SPECULAR_MATERIAL)==0){//только диффузный цвет
				material>>=SHIFT_4TYPE_MATERIALS;
				if( h_s > ((int)V<<16)) {
					float cosAS=(A*A_lightS + B*B_lightS + C*C_lightS);
					int shI=h_s-((int)V<<16)>>(16+2); if(shI > 31) shI=31;
					lght=round(AmbientLight*cosAS*shL[31-shI]) + round((Light*cosA+AmbientLight)*shL[shI]);
					prevShadow=1;
					*(pa+i)|=At_SHADOWV;
				}
				else{
					lght=round(Light*cosA);// + round(AmbientLt*cosAS);
					lght+=AmbientLight;
					prevShadow=0;
				}
			}
			else{ //диф+спекулар
				material>>=SHIFT_4TYPE_MATERIALS;
				if( h_s > ((int)V<<16)) {
					float cosAS=(A*A_lightS + B*B_lightS + C*C_lightS);
					int shI=h_s-((int)V<<16)>>(16+2); if(shI > 31) shI=31;
					lght=round(AmbientLightDifSM[material]*cosAS*shL[31-shI]) + round((LightDifSM[material]*cosA+AmbientLight)*shL[shI]);
					prevShadow=1;
					*(pa+i)|=At_SHADOWV;
				}
				else{
					float cosLV, cosB;
					int cosG;
					cosLV=A_light*A_vw + B_light*B_vw + C_light*C_vw;
					cosB=( A*A_vw + B*B_vw + C*C_vw);
					cosG=round((cosLV-2*cosA*cosB)*255);
					if (cosG > 255) cosG=255; if(cosG < 0)cosG=0;
					short Lspec=COSnG[material][256+cosG];
					lght=round(LightDifSM[material]*cosA)+ Lspec ;//+ round(AmbientLtSl*cosAS);
					lght+=AmbientLight;
					prevShadow=0;
				}
			}
//####################################
*/

//Buckup от перехода полностью на int
/*
//расчет корня - int остальное float
			int A=((Vr<<2)-(V<<2)),B=((V<<2)-(Vh<<2)), C=-1<<8; //C=-1<<7//7дробных! //сдвиг на 8для большей плавности рельефа т.к. точность была потеряна при переходе на VX_FRACTION=5
			int L=fastsqrtI(A*A+B*B+C*C);
			A=(A<<8)/L; B=(B<<8)/L; C=(C<<8)/L;
			float cosA;
			cosA=(A*A_light + B*B_light + C*C_light)/256.;
			int lght;
			if((material&FLAG_SPECULAR_MATERIAL)==0){//только диффузный цвет
				material>>=SHIFT_4TYPE_MATERIALS;
				if( h_s > ((int)V<<16)) {
					float cosAS=(A*A_lightS + B*B_lightS + C*C_lightS)/256.;
					int shI=h_s-((int)V<<16)>>(16+2); if(shI > 31) shI=31;
					lght=round(AmbientLight*cosAS*shL[31-shI]) + round((Light*cosA+AmbientLight)*shL[shI]);
					prevShadow=1;
					*(pa+i)|=At_SHADOWV;
				}
				else{
					lght=round(Light*cosA);// + round(AmbientLt*cosAS);
					lght+=AmbientLight;
					prevShadow=0;
				}
			}
			else{ //диф+спекулар
				material>>=SHIFT_4TYPE_MATERIALS;
				if( h_s > ((int)V<<16)) {
					float cosAS=(A*A_lightS + B*B_lightS + C*C_lightS)/256.;
					int shI=h_s-((int)V<<16)>>(16+2); if(shI > 31) shI=31;
					lght=round(AmbientLightDifSM[material]*cosAS*shL[31-shI]) + round((LightDifSM[material]*cosA+AmbientLight)*shL[shI]);
					prevShadow=1;
					*(pa+i)|=At_SHADOWV;
				}
				else{
					float cosLV, cosB;
					int cosG;
					cosLV=A_light*A_vw + B_light*B_vw + C_light*C_vw;
					cosB=( (int)A*A_vw +(int)B*B_vw +(int)C*C_vw)/256.;
					cosG=round((cosLV-2*cosA*cosB)*255);
					if (cosG > 255) cosG=255; if(cosG < 0)cosG=0;
					short Lspec=COSnG[material][256+cosG];
					lght=round(LightDifSM[material]*cosA)+ Lspec ;//+ round(AmbientLtSl*cosAS);
					lght+=AmbientLight;
					prevShadow=0;
				}
			}
//##########################
*/

/*
			float cosA, cosAS, cosLV, cosB;
			int cosG;
			short Lspec;

			cosAS=(A*A_lightS + B*B_lightS + C*C_lightS)/256.;
			cosLV=A_light*A_vw + B_light*B_vw + C_light*C_vw;
			cosB=( (int)A*A_vw +(int)B*B_vw +(int)C*C_vw)/256.;
			cosG=round((cosLV-2*cosA*cosB)*255);
			if (cosG > 255) cosG=255; if(cosG < 0)cosG=0;
			Lspec=COSnG[terrain][256+cosG];
			int lght;
			if( h_s > ((int)V<<16)) {
				int shI=h_s-((int)V<<16)>>(16+2); if(shI > 31) shI=31;
				if(terrain>200) lght=round(AmbientLtSl*cosAS*shL[31-shI]) + round(LtSl*cosA*shL[shI]);
				else lght=round(AmbientLt*cosAS*shL[31-shI]) + round(Lt*cosA*shL[shI]);
				prevShadow=1;
				*(pa+i)|=At_SHADOWV;
			}
			else{
				if(terrain>200)lght=round(LtSl*cosA)+ Lspec ;//+ round(AmbientLtSl*cosAS);
				else lght=round(Lt*cosA);// + round(AmbientLt*cosAS);
				prevShadow=0;
			}*/
			//lght+=100;
			//lght-=(255-(V>>VX_FRACTION))>>1;

			//затенение по краям
			//lght=round(f*lght);
			//затенение по краям
#ifdef _SURMAP_
			if(ShadowingOnEdges)lght=f*lght>>shift4Fraction;
#else
			//lght=f*lght>>shift4Fraction;
#endif
			if(lght<0)lght=0; if(lght>255)lght=255;
			*(pc+i)=lght;
			//if(h_s  <= (V<<8)) { h_s=V<<8; }

			if(h_s  <= (V<<16)) { h_s=V<<16; }


/*			float cosA=(A*A_light + B*B_light + C*C_light)/256.;
			float cosLV=A_light*A_vw + B_light*B_vw + C_light*C_vw;
			float cosB=( (int)A*A_vw +(int)B*B_vw +(int)C*C_vw)/256.;
			int cosG=round((cosLV-2*cosA*cosB)*255);
			if (cosG > 255) cosG=255; if(cosG < 0)cosG=0; //if(cosG < -255)cosG=-255;
			short Lspec=COSnG[terrain][256+cosG];

			//int lght=round(255.*cosA)+Lspec; if(lght<0)lght=0; if(lght>255)lght=255;
			int lght;
			//if(cosA<0)cosA=-cosA;
			if(terrain>200)lght=round(200.*cosA)+Lspec;
			else lght=round(255.*cosA);
			//lght+=100;
			//lght-=(255-(V>>VX_FRACTION))>>1;
			if(lght<0)lght=0; if(lght>255)lght=255;
			if( h_s > ((int)V<<8) ) {
				*(pc+i)=lght;//((terra.light_front[0][type][idex_light_front] * terra.light_sideways[0][type][idex_light_sideways] >>8) +terra.ambient_light[0][type]);

				if(*(pah+i)&At_SHADOW) prevShadow+=1;
				if(prevShadow==0) ;
				else { 
					if(prevShadow==1) *(pc+i)=(*(pc+i)>>1) + (*(pc+i)>>2);
					else *(pc+i)=*(pc+i)>>1;
				}
				*(pa+i)|=At_SHADOWV;
				prevShadow=1;
			}
			else {
				*(pc+i)=lght;//((terra.light_front[0][type][idex_light_front] * terra.light_sideways[0][type][idex_light_sideways] >>8) +terra.ambient_light[0][type]);
				if(h_s  <= (V<<8)) { h_s=V<<8; }
				if(*(pah+i)&At_SHADOW) *(pc+i)=(*(pc+i)>>1) + (*(pc+i)>>2);
				//h_s=*pv<<16;
				prevShadow=0;
			}*/
			Vr=V;
			if( (*(pa+i)&At_SOOTMASK)==At_SOOT){
				*(pc+i)=*(pc+i)>>2; //Если сажа
			}
			if(!VxDBuf[offY+i]) *(pc+i)=*(pc+i)>>1; //если выступает Geo
			else *(pc+i)=(0x80|(*(pc+i)>>1));// если выступает Dam
//			if(STATUS_DEBUG_RND)*(pc+i)=0;

			//*(pa+i)|=At_ZEROPLAST; //For DEBUG
			j--;//вместо for-а
		}while( (j>=0 || prevShadow!=0) && i>0 ); //i>0 проверка на конец карты
		//*(pa+XCYCL(j+XL))|=At_ZEROPLAST; //For DEBUG
	}while( (*(pa+XCYCL(j+XL))&At_SHADOW) && i>0);

//	VxBufWorkMode=backupVxBufWorkMode;//восстановление режима доступа к поверхности
	return i;//XCYCL(j+XL);
}


//////////////////////////////////////////////////////////////////////


void RenderShadovM3D(int number, float y)
{
/*	cShadow Sh;
	Sh.number=number;
	float f_dzx=(float)1/((float)delta_s/(256*fraction));
	DrawMeshShadeP(&Sh,10000,round(y),f_dzx);///10000*delta_s>>16);
	short *shadeBuf=Sh.shade;
		for (int j=0;j<Sh.yShade;j++,shadeBuf+=Sh.xShade)  
		{
			int y=YCYCL(Sh.y+j), x=Sh.x,x_cycl=Sh.x;
			float v;
			unsigned short *pv0=&(VxBuf[y<<H_SIZE_POWER]);
			unsigned char *pc0=&(ClTrBuf[y<<H_SIZE_POWER]);
			unsigned char *pa0=&(AtBuf[y*XS_Buf]);
			v=(*(pv0+(x_cycl)))/fraction;
			int xshade=round((Sh.x-x)-(Sh.z-v)*f_dzx);
			while(xshade<Sh.xShade)
			{
				if((xshade>0)&&(shadeBuf[xshade]>=(v=*(pv0+(x_cycl))/fraction ))&&((pa0[x_cycl]&At_SHADOW)==0))
					*(pc0+x_cycl)>>=1;
				x_cycl=XCYCL(--x);
				v=*(pv0+(x_cycl))/fraction;
				xshade=round((Sh.x-x)-(Sh.z-v)*f_dzx);
			}
		}

	editM3Dyh=YCYCL(Sh.y);
	editM3Dyd=YCYCL(Sh.y+Sh.yShade);
*/
}



///End 16bit version




/////////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
