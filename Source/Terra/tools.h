#ifndef __TOOLS_H__
#define __TOOLS_H__

#ifdef _PERIMETER_
#include "..\Render\inc\Umath.h"
#include "..\UTIL\DebugUtil.h"
#include "..\Units\UnitAttribute.h"
#endif


extern int* xRad[MAX_RADIUS_CIRCLEARR + 1];
extern int* yRad[MAX_RADIUS_CIRCLEARR + 1];
extern int maxRad[MAX_RADIUS_CIRCLEARR + 1];

extern int MosaicTypes[8];

struct BitMap {
	unsigned short sx,sy;
	int sz;
	unsigned char* data;
	unsigned char* palette;
	int force,mode,border,copt;
	int mosaic;
	int size,level,Kmod,modeC2H,alpha;
	int xoffset,yoffset;
	double A11, A12, A21, A22;
	double X, Y;

		BitMap(int _mosaic = 0);

	void load(const char* name);
	void convert(void);
	void place(char* name,int x, int y,int _force,int _mode,int _border = 0,int _level = 0,int _size = 0,int Kmod=0, int _modeC2H = 0,int alpha=90, int ter_type=-1);
	inline int getDelta(int x,int y,int delta){
		x += xoffset;
		y += yoffset;
		x+=vMap.H_SIZE;
		y+=vMap.V_SIZE;
		int x1,y1;
		x1 = round(A11*x + A12*y + X);
		y1 = round(A21*x + A22*y + Y);
		while (x<0){ x+=sx;}
		while (y<0){ y+=sy;}
		if(!mode) return delta*(64 - data[(y1%sy)*sx + (x1%sx)])/64; 
		return delta + force*(64 - data[(y1%sy)*sx + (x1%sx)])/64;
		//if(!mode) return delta*(64 - data[(y%sy)*sx + (x%sx)])/64;
		//return delta + force*(64 - data[(y%sy)*sx + (x%sx)])/64;
	}
	inline int getType(int x,int y){
		x += xoffset;
		y += yoffset;
		x+=vMap.H_SIZE;
		y+=vMap.V_SIZE;
		int x1,y1;
		x1 = round(A11*x + A12*y + X);
		y1 = round(A21*x + A22*y + Y);
		while (x<0){ x+=sx;}
		while (y<0){ y+=sy;}
		return MosaicTypes[data[(y1%sy)*sx + (x1%sx)]%8]; 
		//return MosaicTypes[data[(y%sy)*sx + (x%sx)]%8]; 
	}
	inline int getColor(int x, int y){
		x += xoffset;
		y += yoffset;
		x+=vMap.H_SIZE;
		y+=vMap.V_SIZE;
		int x1,y1;
		x1 = round(A11*x + A12*y + X);
		y1 = round(A21*x + A22*y + Y);
		while (x<0){ x+=sx;}
		while (y<0){ y+=sy;}
		return data[(y1%sy)*sx + (x1%sx)]; 
	};

	void set_alpha(double alpha, int X0, int Y0){
		X0+=vMap.H_SIZE;
		Y0+=vMap.V_SIZE;
		alpha-=90;
		A11 = A22 = cos(alpha*3.1415926535/180);
		A21 = -(A12 = sin(alpha*3.1415926535/180));
		X = X0 - A11*X0 - A12*Y0;
		Y = Y0 - A21*X0 - A22*Y0;
	};

};

extern BitMap placeBMP;
extern BitMap mosaicBMP;
extern int curBmpIndex;

extern void PutTrackPoints(int num_track);


//возвращает в указателе на слово следующее слово, а указатель на буфер передвигает
inline int get_world_in_buf(char*& buf, char* world)
{
	int counter=0;
	while( ((isspace(*buf) ) || (*buf=='=')) && (*buf) ) buf++;
	while( (!(isspace(*buf)) && (*buf!='=')) && (*buf) ){
		*world=*buf; world++, buf++, counter ++;
	}
	*world=0;
	return counter;
}


inline void damagingBuildingsTolzer(int _x, int _y, int _r)
{
	int x = _x >> kmGrid;
	int y = _y >> kmGrid;
	int r = _r >> kmGrid;
	if((r << kmGrid) != _r ) 
		r++;
	if(r > MAX_RADIUS_CIRCLEARR){
		xassert(0&&"exceeding max radius in damaginBuildingsTolzer");
		r = MAX_RADIUS_CIRCLEARR;
	}
	for(int i = 0;i <= r; i++){
		int max = maxRad[i];
		int* xx = xRad[i];
		int* yy = yRad[i];
		for(int j = 0;j < max;j++) {
			int offG = vMap.offsetGBuf(vMap.XCYCLG(x + xx[j]), vMap.YCYCLG(y + yy[j]));
			if(vMap.GABuf[offG]&GRIDAT_BUILDING){
				vMap.GABuf[offG] |= GRIDAT_BASE_OF_BUILDING_CORRUPT;
			}
		}
	}
}

inline void damagingBuildingsTolzerS(int _xL, int _yU, int _xR, int _yD)
{
	int xL=vMap.w2mClampX(_xL);
	int xR=vMap.w2mClampX(_xR);
	int yU=vMap.w2mClampY(_yU);
	int yD=vMap.w2mClampY(_yD);
	int i,j;
	for(i=yU; i<=yD; i++){
		int offGY=vMap.offsetGBuf(0,i);
		for(j=xL; j<=xR; j++){
			if(vMap.GABuf[offGY+j]&GRIDAT_BUILDING){
				vMap.GABuf[offGY+j]|=GRIDAT_BASE_OF_BUILDING_CORRUPT;
			}
		}
	}
}

inline void damagingBuildingsTolzerS(int _x, int _y, int _r)
{
	damagingBuildingsTolzerS(_x-_r, _y-_r, _x+_r, _y+_r);
}


enum toolzer2TerrainEffect {
	T2TE_PLOTTING_ZEROPLAST,
	T2TE_PLOTTING_ZEROPLAST_EMPTY,
	T2TE_ERASE_ZEROPLAST,
	T2TE_PLOTTING_SOOT,
	T2TE_PLOTTING_VERY_LIGHT_DAM,
	T2TE_ALIGNMENT_TERRAIN_4ZP,
	T2TE_ALIGNMENT_TERRAIN_VARIABLE_H,
	T2TE_CHANGING_TERRAIN_HEIGHT,
	T2TE_CHANGING_TERRAIN_HEIGHT_IFNOTZP,
};
enum toolzerMovingTerrainType{
	TMTT_NOT_SUPPORT,
	TMTT_SUPPORT
};

#ifndef _SURMAP_
extern int terEnergyBorderCheck(int x,int y);
#endif
///////////////////////////////////////////////////////////////////////////////////////////////////
template<toolzer2TerrainEffect _T2TE_> class individualToolzer{
public:
	int hAppr;
	individualToolzer(){
		hAppr=vMap.hZeroPlast<<VX_FRACTION;
		locp=0;// 4 influenceDZ !
	}
	~individualToolzer(void){};

	void setHAppoximation(int hAppoximation){
		hAppr=hAppoximation<<VX_FRACTION;
	}

	int influenceBM(int x, int y, int sx, int sy, unsigned char * imageArea){

		int begx=vMap.XCYCL(x- (sx>>1));
		int begy=vMap.YCYCL(y- (sy>>1));
		unsigned char * ia=imageArea;
		int i,j;
		int offY=vMap.offsetBuf(0,begy);
		for(i=0; i<sy; i++){
			int offY=vMap.offsetBuf(0,vMap.YCYCL(begy+i));
			for(j=0; j<sx; j++){
				int dV=*(ia);
				switch(_T2TE_){
				case T2TE_PLOTTING_ZEROPLAST:
					{
							int offB=offY+vMap.XCYCL(j+begx);
						//if( dV && ((vMap.VxDBuf[offB]==vMap.hZeroPlast) || (vMap.VxDBuf[offB]==0 && vMap.VxGBuf[offB]==vMap.hZeroPlast)) ){
						//	vMap.AtrBuf[offB]|=At_ZEROPLAST;
						//}
						if( dV && (vMap.SGetAlt(offB)==hAppr) ){
							vMap.AtrBuf[offB]|=At_ZEROPLAST;
						}
					}
					break;
				case T2TE_PLOTTING_ZEROPLAST_EMPTY:
					{
//						int offB=offY+vMap.XCYCL(j+begx);
//						if(dV && (vMap.AtrBuf[offB]&At_ZEROPLAST)) vMap.AtrBuf[offB]|=At_LEVELED;
					}
					break;
				case T2TE_ERASE_ZEROPLAST:
					{
						if(dV) vMap.AtrBuf[offB]&=~At_NOTPURESURFACE;
					}
					break;
				case T2TE_PLOTTING_SOOT:
					{
						int offB=offY+vMap.XCYCL(j+begx);
						unsigned char atr=vMap.AtrBuf[offB];
						if(dV && (atr&At_ZPMASK)!=At_ZEROPLAST ) vMap.AtrBuf[offB]=(atr&(~At_SOOTMASK))|At_SOOT;
					}
					break;
				case T2TE_PLOTTING_VERY_LIGHT_DAM:
					{
						int offB=offY+vMap.XCYCL(j+begx);
						if(dV) {
							if(vMap.VxDBuf[offB]==0) vMap.VxDBuf[offB]=vMap.VxGBuf[offB];
							vMap.SurBuf[offB]=vMap.veryLightDam;
						}
					}
					break;
				case T2TE_ALIGNMENT_TERRAIN_VARIABLE_H:
				case T2TE_ALIGNMENT_TERRAIN_4ZP:
				case T2TE_CHANGING_TERRAIN_HEIGHT:
				case T2TE_CHANGING_TERRAIN_HEIGHT_IFNOTZP:
					{
						int offB=offY+vMap.XCYCL(j+begx);
						tVoxSet(offB, dV);
					}
					break;
				}
				ia++;
			}
		}
		//Эта операция должна быть совмещена с рендером
		vMap.recalcArea2Grid(vMap.XCYCL(begx-1), vMap.YCYCL(begy-1), vMap.XCYCL(begx + sx+1), vMap.YCYCL(begy + sy+1) );
		vMap.regRender(vMap.XCYCL(begx-1), vMap.YCYCL(begy-1), vMap.XCYCL(begx + sx+1), vMap.YCYCL(begy + sy+1) );

		return 0;
	}

//////////////////////////////////////////
	int locp;
	int influenceDZ(int x, int y, int rad, short dh, int smMode, unsigned char picMode=0){

		if(rad > MAX_RADIUS_CIRCLEARR){
			xassert(0&&"exceeding max radius in influenceDZ");
			rad=MAX_RADIUS_CIRCLEARR;
		}

		int begx=vMap.XCYCL(x - rad);
		int begy=vMap.YCYCL(y - rad);

		//void vrtMap::deltaZone(int x,int y,int rad,int smth,int dh,int smode,int eql)
		int eql=0;

		register int i,j;
		int max;
		int* xx,*yy;

		int r = rad - rad*smMode/10;
		float d = 1.0f/(rad - r + 1),dd,ds,s;
		int v, h, k, mean;

		if(dh){
			for(i = 0; i <= r; i++){
				max = maxRad[i];
				xx = xRad[i];
				yy = yRad[i];
				for(j = 0;j < max;j++) {
					tVoxSet( vMap.offsetBuf(vMap.XCYCL(x + xx[j]), vMap.YCYCL(y + yy[j])), dh);
				}
			}

			for(i = r + 1,dd = 1.0f - d; i <= rad; i++,dd -= d){
				max = maxRad[i];
				xx = xRad[i];
				yy = yRad[i];
				h = (int)(dd*dh);
				if(!h) 
					h = dh > 0 ? 1 : -1;
				switch(picMode){
					case 0:
						v = (int)(dd*max);
						ds = (float)v/(float)max;
						for(s = ds,k = 0,j = locp % max; k < max; j = (j + 1 == max) ? 0 : j + 1,k++,s += ds)
							if(s >= 1.0){
								//tVoxSet( vMap.offsetBuf(vMap.XCYCL(x + xx[j]), vMap.YCYCL(y + yy[j])), dh, hZeroPlast, heap);
								tVoxSet( vMap.offsetBuf(vMap.XCYCL(x + xx[j]), vMap.YCYCL(y + yy[j])), dh);
								s -= 1.0;
								}
						break;
					case 1:
						v = (int)(dd*1000000.0);
						for(j = 0;j < max;j++)
							if((int)XRnd(1000000) < v) {
								//tVoxSet( vMap.offsetBuf(vMap.XCYCL(x + xx[j]), vMap.YCYCL(y + yy[j])), dh, hZeroPlast, heap);
								tVoxSet( vMap.offsetBuf(vMap.XCYCL(x + xx[j]), vMap.YCYCL(y + yy[j])), dh);
							}
						break;
					case 2:
						v = (int)(dd*max);
						for(k = 0,j = locp%max;k < v;j = (j + 1 == max) ? 0 : j + 1,k++){
							//tVoxSet( vMap.offsetBuf(vMap.XCYCL(x + xx[j]), vMap.YCYCL(y + yy[j])), dh, hZeroPlast, heap);
							tVoxSet( vMap.offsetBuf(vMap.XCYCL(x + xx[j]), vMap.YCYCL(y + yy[j])), dh);
						}
						locp += max;
						break;
					}
			}
			locp++;
		}
		else {
			const int DH_MEAN = 1; //дельта по которой усредняются высоты
			int cx,h,cy,cx_;
			if(eql){
				mean = k = 0;
				for(i = 0;i <= r;i++){
					max = maxRad[i];
					xx = xRad[i];
					yy = yRad[i];
					for(j = 0;j < max;j++){
						cx = vMap.XCYCL(x + xx[j]);
						mean += vMap.SGetAlt(cx,vMap.YCYCL(y+yy[j]));
					}
					k += max;
				}
				mean /= k;
				for(i = 0;i <= r;i++){
					max = maxRad[i];
					xx = xRad[i];
					yy = yRad[i];
					for(j = 0;j < max;j++){
						cy = vMap.YCYCL(y+yy[j]);
						cx = vMap.XCYCL(x + xx[j]);
						h = vMap.SGetAlt(cx,cy);
						if(abs(h - mean) < eql)
							if(h > mean){ //voxSet(cx,cy,-1);
								//tVoxSet( vMap.offsetBuf(cx, cy), -DH_MEAN);
								tVoxSetAll( vMap.offsetBuf(cx, cy), -DH_MEAN);
							}
							else if(h < mean){ //voxSet(cx,cy,1);
								//tVoxSet( vMap.offsetBuf(cx, cy), DH_MEAN);
								tVoxSetAll( vMap.offsetBuf(cx, cy), DH_MEAN);
							}
					}
				}
				for(i = r + 1,dd = 1.0f - d;i <= rad;i++,dd -= d){
					max = maxRad[i];
					xx = xRad[i];
					yy = yRad[i];
					h = (int)(dd*dh);
					if(!h) h = dh > 0 ? 1 : -1;
					v = (int)(dd*max);
					ds = (float)v/(float)max;
					for(s = ds,k = 0,j = locp%max;k < max;j = (j + 1 == max) ? 0 : j + 1,k++,s += ds)
						if(s >= 1.0){
							cy = vMap.YCYCL(y+yy[j]);
							cx = vMap.XCYCL(x + xx[j]);
							h = vMap.SGetAlt(cx,cy);
							if(abs(h - mean) < eql)
								if(h > mean){ //voxSet(cx,cy,-1);
									//tVoxSet( vMap.offsetBuf(cx, cy), -DH_MEAN);
									tVoxSetAll( vMap.offsetBuf(cx, cy), -DH_MEAN);
								}
								else if(h < mean){ //voxSet(cx,cy,1);
									//tVoxSet( vMap.offsetBuf(cx, cy), DH_MEAN);
									tVoxSetAll( vMap.offsetBuf(cx, cy), DH_MEAN);
								}
							s -= 1.0;
							}
				}
			}
			else {
				int dx,dy;
				for(i = 0;i <= r;i++){
					max = maxRad[i];
					xx = xRad[i];
					yy = yRad[i];
					for(j = 0;j < max;j++){
						cy = vMap.YCYCL(y+yy[j]);
						cx = vMap.XCYCL(x + xx[j]);
						h = vMap.SGetAlt(cx,cy);
						v = 0;
						switch(picMode){
							case 0:
								for(dy = -1;dy <= 1;dy++)
									for(dx = -1;dx <= 1;dx++){
										cx_ = vMap.XCYCL(cx+dx);
										v += vMap.SGetAlt(cx_,vMap.YCYCL(cy +dy));
									}
								v -= h;
								v >>= 3;
								break;
							case 1:
							case 2:
								for(dy = -1;dy <= 1;dy++){
									for(dx = -1;dx <= 1;dx++){
										cx_ = vMap.XCYCL(cx+dx);
										if(abs(dx) + abs(dy) == 2)
											v += vMap.SGetAlt(cx_,vMap.YCYCL(cy +dy));
									}
								}
								v >>= 2;
								break;
						}
						//voxSet(cx,cy,v - h);
						//tVoxSet( vMap.offsetBuf(cx, cy), v-h);
						tVoxSetAll( vMap.offsetBuf(cx, cy), v-h);
					}
				}
				for(i = r + 1,dd = 1.0f - d;i <= rad;i++,dd -= d){
					max = maxRad[i];
					xx = xRad[i];
					yy = yRad[i];
					h = (int)(dd*dh);
					if(!h) h = dh > 0 ? 1 : -1;

					v = (int)(dd*max);
					ds = (float)v/(float)max;
					for(s = ds,k = 0,j = locp%max;k < max;j = (j + 1 == max) ? 0 : j + 1,k++,s += ds){
						if(s >= 1.0){
							cy = vMap.YCYCL(y+yy[j]);
							cx = vMap.XCYCL(x + xx[j]);
							h = vMap.SGetAlt(cx,cy);
							v = 0;
							switch(picMode){
								case 0:
									for(dy = -1;dy <= 1;dy++){
										for(dx = -1;dx <= 1;dx++){
											cx_ = vMap.XCYCL(cx+dx);
											v += vMap.SGetAlt(cx_,vMap.YCYCL(cy +dy));
										}
									}
									v -= h;
									v >>= 3;
									break;
								case 1:
								case 2:
									for(dy = -1;dy <= 1;dy++){
										for(dx = -1;dx <= 1;dx++){
											cx_ = vMap.XCYCL(cx+dx);
											if(abs(dx) + abs(dy) == 2)
												v += vMap.SGetAlt(cx_,vMap.YCYCL(cy +dy));
										}
									}
									v >>= 2;
									break;
							}
							//voxSet(cx,cy,v - h);
							//tVoxSet( vMap.offsetBuf(cx, cy), v-h);
							tVoxSetAll( vMap.offsetBuf(cx, cy), v-h);
							s -= 1.0;
						}
					}
				}
			}
		}
		//regRender(x - rad,y - rad,x + rad,y + rad);

		//Эта операция должна быть совмещена с рендером
		vMap.recalcArea2Grid(vMap.XCYCL(begx-1), vMap.YCYCL(begy-1), vMap.XCYCL(begx + 2*rad+1), vMap.YCYCL(begy + 2*rad+1) );
		vMap.regRender(vMap.XCYCL(begx-1), vMap.YCYCL(begy-1), vMap.XCYCL(begx + 2*rad+1), vMap.YCYCL(begy + 2*rad+1) );


		return 0;
	}
///////////////////////////////////////////////////
	inline int tVoxSet(int offB, int dV){
		///if(vMap.GABuf[vMap.offsetBuf2offsetGBuf(offB)]&GRIDAT_BUILDING) return 0;
		if(dV<0){
			dV=dV*((vMap.GABuf[vMap.offsetBuf2offsetGBuf(offB)]&GRIDAT_MASK_HARDNESS)<<(vMap.GABuf[vMap.offsetBuf2offsetGBuf(offB)]&GRIDAT_MASK_HARDNESS));
			dV=dV/(24);
		}
		return tVoxSetAll(offB, dV);
	};
	inline int tVoxSetAll(int offB, int dV){
		unsigned char fraction=vMap.AtrBuf[offB]&VX_FRACTION_MASK;
		int VD=vMap.VxDBuf[offB];
		int VG=(vMap.VxGBuf[offB]<<VX_FRACTION) + fraction;
		int v;
		if(VD){
			VD=(VD<<VX_FRACTION) + fraction;
			v=VD;
		}
		else v=VG;

		switch (_T2TE_){
		case T2TE_PLOTTING_ZEROPLAST:
			{
				if( v==hAppr ){
					vMap.AtrBuf[offB]|=At_ZEROPLAST;
				}
			}
			break;
		case T2TE_PLOTTING_ZEROPLAST_EMPTY:
			{
//				if(vMap.AtrBuf[offB]&At_ZEROPLAST) vMap.AtrBuf[offB]|=At_LEVELED;
			}
			break;
		case T2TE_ERASE_ZEROPLAST:
			{
#ifndef _SURMAP_
				int x = offB & vMap.clip_mask_x;
				int y = offB >> vMap.H_SIZE_POWER;
				if(!(vMap.GABuf[vMap.offsetBuf2offsetGBuf(offB)] & GRIDAT_MASK_CLUSTERID) && terEnergyBorderCheck(x,y))
#endif
					vMap.AtrBuf[offB]&=~At_NOTPURESURFACE;
			}
			break;
		case T2TE_PLOTTING_SOOT:
			{
				unsigned char atr=vMap.AtrBuf[offB];
				if( (atr&At_ZPMASK)!=At_ZEROPLAST ) vMap.AtrBuf[offB]=(atr&(~At_SOOTMASK))|At_SOOT;
			}
			break;
		case T2TE_PLOTTING_VERY_LIGHT_DAM:
			{
				if(dV) {
					if(vMap.VxDBuf[offB]==0) vMap.VxDBuf[offB]=vMap.VxGBuf[offB];
					vMap.SurBuf[offB]=vMap.veryLightDam;
				}
			}
			break;

		case T2TE_ALIGNMENT_TERRAIN_4ZP:
			if( (vMap.AtrBuf[offB]&At_ZPMASK)==At_ZEROPLAST ) return 0; //break нет специально
			if( dV >0 ){
				if(v < hAppr){//Воздействие если инструмент добавляет и высота меньше hAppr
					v+=dV;
					if(v>hAppr) v=hAppr;
					if(VD==0){ //если был гео слой
						vMap.SurBuf[offB]=vMap.GetGeoType(offB,v);
						vMap.VxGBuf[offB]=v>>VX_FRACTION;
						vMap.AtrBuf[offB]=v&VX_FRACTION_MASK;
						//vMap.AtrBuf[offB]=(v&VX_FRACTION_MASK) | (vMap.AtrBuf[offB]& (~(VX_FRACTION_MASK|At_NOTPURESURFACE)));
					}
					else{ //если был дам слой
						vMap.VxDBuf[offB]=v>>VX_FRACTION;
						vMap.AtrBuf[offB]=v&VX_FRACTION_MASK;
						//vMap.AtrBuf[offB]=(v&VX_FRACTION_MASK) | (vMap.AtrBuf[offB]& (~(VX_FRACTION_MASK|At_NOTPURESURFACE)));
					}
//					return dV;
				}
			}
			else {
				if(v > hAppr){//Воздействие если инструмент убирает и высота больше hAppr
					v+=dV;
					if(v<hAppr) v=hAppr;
					if(v <= VG){ //Если получился гео слой
						//if(v<hAppr) v=hAppr;
						vMap.SurBuf[offB]=vMap.GetGeoType(offB,v);
						vMap.VxGBuf[offB]=v>>VX_FRACTION;
						vMap.AtrBuf[offB]=v&VX_FRACTION_MASK;
						//vMap.AtrBuf[offB]=(v&VX_FRACTION_MASK) | (vMap.AtrBuf[offB]& (~(VX_FRACTION_MASK|At_NOTPURESURFACE)));
						if(VD!=0) vMap.VxDBuf[offB]=0;
					}
					else { //если остался дам слой
						//if(v<hAppr) v=hAppr;
						vMap.VxDBuf[offB]=v>>VX_FRACTION;
						vMap.AtrBuf[offB]=v&VX_FRACTION_MASK;
						//vMap.AtrBuf[offB]=(v&VX_FRACTION_MASK) | (vMap.AtrBuf[offB]& (~(VX_FRACTION_MASK|At_NOTPURESURFACE)));
					}
//					return dV;
				}
			}
			if(v == hAppr) vMap.AtrBuf[offB]=(vMap.AtrBuf[offB]&(~At_ZPMASK))|At_LEVELED;
			return 0;
			break;
		case T2TE_ALIGNMENT_TERRAIN_VARIABLE_H:
			if( (vMap.AtrBuf[offB]&At_ZPMASK)==At_ZEROPLAST ) return 0; //break нет специально
			if( dV >0 ){
				if(v < hAppr){//Воздействие если инструмент добавляет и высота меньше hAppr
					v+=dV;
					if(v>hAppr) v=hAppr;
					if(VD==0){ //если был гео слой
						vMap.SurBuf[offB]=vMap.GetGeoType(offB,v);
						vMap.VxGBuf[offB]=v>>VX_FRACTION;
						vMap.AtrBuf[offB]=v&VX_FRACTION_MASK;
						//vMap.AtrBuf[offB]=(v&VX_FRACTION_MASK) | (vMap.AtrBuf[offB]& (~(VX_FRACTION_MASK|At_NOTPURESURFACE)));
					}
					else{ //если был дам слой
						vMap.VxDBuf[offB]=v>>VX_FRACTION;
						vMap.AtrBuf[offB]=v&VX_FRACTION_MASK;
						//vMap.AtrBuf[offB]=(v&VX_FRACTION_MASK) | (vMap.AtrBuf[offB]& (~(VX_FRACTION_MASK|At_NOTPURESURFACE)));
					}
					return dV;
				}
			}
			else {
				if(v > hAppr){//Воздействие если инструмент убирает и высота больше hAppr
					v+=dV;
					if(v<hAppr) v=hAppr;
					if(v <= VG){ //Если получился гео слой
						//if(v<hAppr) v=hAppr;
						vMap.SurBuf[offB]=vMap.GetGeoType(offB,v);
						vMap.VxGBuf[offB]=v>>VX_FRACTION;
						vMap.AtrBuf[offB]=v&VX_FRACTION_MASK;
						//vMap.AtrBuf[offB]=(v&VX_FRACTION_MASK) | (vMap.AtrBuf[offB]& (~(VX_FRACTION_MASK|At_NOTPURESURFACE)));
						if(VD!=0) vMap.VxDBuf[offB]=0;
					}
					else { //если остался дам слой
						//if(v<hAppr) v=hAppr;
						vMap.VxDBuf[offB]=v>>VX_FRACTION;
						vMap.AtrBuf[offB]=v&VX_FRACTION_MASK;
						//vMap.AtrBuf[offB]=(v&VX_FRACTION_MASK) | (vMap.AtrBuf[offB]& (~(VX_FRACTION_MASK|At_NOTPURESURFACE)));
					}
					return dV;
				}
			}
			return 0;
			break;
		case T2TE_CHANGING_TERRAIN_HEIGHT_IFNOTZP:
			if( (vMap.AtrBuf[offB]&At_ZPMASK)==At_ZEROPLAST ) return 0; //break нет специально
		case T2TE_CHANGING_TERRAIN_HEIGHT:
			if( dV >0 ){ //инструмент добавляет
				v+=dV;
				if(v>MAX_VX_HEIGHT) v=MAX_VX_HEIGHT;
				if(VD==0){ //если был гео слой
					vMap.SurBuf[offB]=vMap.GetGeoType(offB,v);
					vMap.VxGBuf[offB]=v>>VX_FRACTION;
					vMap.AtrBuf[offB]=v&VX_FRACTION_MASK;
					//vMap.AtrBuf[offB]=(v&VX_FRACTION_MASK) | (vMap.AtrBuf[offB]& (~(VX_FRACTION_MASK|At_NOTPURESURFACE)));
				}
				else{ //если был дам слой
					vMap.VxDBuf[offB]=v>>VX_FRACTION;
					vMap.AtrBuf[offB]=v&VX_FRACTION_MASK;
					//vMap.AtrBuf[offB]=(v&VX_FRACTION_MASK) | (vMap.AtrBuf[offB]& (~(VX_FRACTION_MASK|At_NOTPURESURFACE)));
				}
				return dV;
			}
			else { //if(dV <=0)  инструмент убирает 
				v+=dV;
				if(v <= VG){ //Если получился гео слой
					if(v< MIN_VX_HEIGHT) v=MIN_VX_HEIGHT;
					vMap.SurBuf[offB]=vMap.GetGeoType(offB,v);
					vMap.VxGBuf[offB]=v>>VX_FRACTION;
					vMap.AtrBuf[offB]=v&VX_FRACTION_MASK;
					//vMap.AtrBuf[offB]=(v&VX_FRACTION_MASK) | (vMap.AtrBuf[offB]& (~(VX_FRACTION_MASK|At_NOTPURESURFACE)));
					if(VD!=0) vMap.VxDBuf[offB]=0;
				}
				else { //если остался дам слой
					if(v< MIN_VX_HEIGHT) v=MIN_VX_HEIGHT;
					vMap.VxDBuf[offB]=v>>VX_FRACTION;
					vMap.AtrBuf[offB]=v&VX_FRACTION_MASK;
					//vMap.AtrBuf[offB]=(v&VX_FRACTION_MASK) | (vMap.AtrBuf[offB]& (~(VX_FRACTION_MASK|At_NOTPURESURFACE)));
				}
				return dV;
			}
			return 0;
			break;
		}
		return 0;
	};

};

class baseToolzer{
public:
	individualToolzer<T2TE_ALIGNMENT_TERRAIN_4ZP> toolzerAligmentTerrain4ZP;
	individualToolzer<T2TE_ALIGNMENT_TERRAIN_VARIABLE_H> toolzerAligmentTerrainVariableH;
	individualToolzer<T2TE_CHANGING_TERRAIN_HEIGHT> toolzerChangeTerHeight;
	individualToolzer<T2TE_CHANGING_TERRAIN_HEIGHT_IFNOTZP> toolzerChangeTerHeightIfNotZP;
	individualToolzer<T2TE_PLOTTING_SOOT> toolzerPlotingSoot;
	individualToolzer<T2TE_PLOTTING_VERY_LIGHT_DAM> toolzerPlottingVeryLightDam;
	individualToolzer<T2TE_ERASE_ZEROPLAST> toolzerEraseZP;
	int phase;
	int x,y;
	baseToolzer(){
		phase=0;
	};
	void start(){ phase=0; }
	void start(int _x, int _y){
		x=vMap.XCYCL(_x); y=vMap.YCYCL(_y);
		start();
	};
	int quant(int _x, int _y){
		x=vMap.XCYCL(_x); y=vMap.YCYCL(_y);
		return quant();
	};
	int quant(){
#ifdef _PERIMETER_
//		log_var(typeid(*this).name());
//		log_var(vMap.getChAreasInformationCRC());
//		Vect2i toolzerPosition(x, y);
//		log_var(toolzerPosition);
#endif
		int result=simpleQuant();
		if(result) phase=phase++;
		else phase=0;
#ifdef _PERIMETER_
//		log_var(phase);
//		log_var(result);
//		log_var(vMap.getChAreasInformationCRC());
#endif
		return result;
	};
	virtual int simpleQuant()=0;

	void saveStatus(XBuffer& ff){
		ff.write(&phase, sizeof(phase));
	};
	void loadStatus(XBuffer& ff){
		ff.read(&phase, sizeof(phase));
	};

};

inline void clearAtrBaseOfBuildingCorrupt(int x, int y, int rad)
{
	int i,j;
	const int begxg=(x-rad)>>kmGrid;
	const int begyg=(y-rad)>>kmGrid;
	const int endxg=(x+rad)>>kmGrid;
	const int endyg=(y+rad)>>kmGrid;
	for(j=begyg; j<=endyg; j++){
		int offGB=vMap.offsetGBuf(0,vMap.YCYCLG(j));
		for(i=begxg; i<=endxg; i++){
			int curoff=offGB+vMap.XCYCLG(i);
			if(vMap.GABuf[curoff]&GRIDAT_BUILDING && ((vMap.GABuf[curoff]&GRIDAT_LEVELED)==0) ) continue; //в случае здания только с полностью выровненной поверхноти снимается аттрибут поврежденной
			vMap.GABuf[curoff]&=~GRIDAT_BASE_OF_BUILDING_CORRUPT;
		}
	}
}

//individualToolzer<T2TE_ALIGNMENT_TERRAIN_4ZP> toolzerAligmentTerrain4ZP;
//individualToolzer<T2TE_ALIGNMENT_TERRAIN_VARIABLE_H> toolzerAligmentTerrainVariableH;
//toolzerAligmentTerrainVariableH
class baseDigPutToolzer : public baseToolzer{
public:
	bool flag_leveled_status;
	baseDigPutToolzer(){
		flag_leveled_status=1;
	};
	void setHAppoximation(int wh){
		toolzerAligmentTerrain4ZP.setHAppoximation(wh);
		toolzerAligmentTerrainVariableH.setHAppoximation(wh);

		flag_leveled_status=(wh!=0);
	};
};

#ifdef _PERIMETER_

class digToolzer4K: public baseDigPutToolzer {
public:
	int simpleQuant(){
		//toolzerChangeTerHeight
		//toolzerChangeTerHeightIfNotZP
		//toolzerPlotingSoot
		//toolzerEraseZP
		//toolzerAligmentTerrain4ZP
		//int influenceDZ(int x, int y, int rad, short dz, int smMode)
		//int influenceBM(int x, int y, int sx, int sy, unsigned char * imageArea)
		if(flag_leveled_status) {
			switch(phase){
			case 0:
				//toolzerAligmentTerrain4ZP.influenceDZ(x, y, 5, -3<<VX_FRACTION, 3 );
				//toolzerAligmentTerrain4ZP.influenceDZ(x, y, 16, 0<<VX_FRACTION, 5 );
				toolzerAligmentTerrain4ZP.influenceDZ(x, y, round(10*trucksIntrumentParameter.kRadius4DigZL), round(-3*trucksIntrumentParameter.kHeigh4DigZL*(1<<VX_FRACTION)), 3 );
			case 1:
				toolzerAligmentTerrain4ZP.influenceDZ(x, y, round(20*trucksIntrumentParameter.kRadius4DigZL), 0<<VX_FRACTION, 5 );
				///clearAtrBaseOfBuildingCorrupt(x,y,16);
				break;
			case 2:
				//toolzerAligmentTerrain4ZP.influenceDZ(x, y, 5, -3<<VX_FRACTION, 3 );
				//toolzerAligmentTerrain4ZP.influenceDZ(x, y, 16, 0<<VX_FRACTION, 6 );
				toolzerAligmentTerrain4ZP.influenceDZ(x, y, round(10*trucksIntrumentParameter.kRadius4DigZL), round(-3*trucksIntrumentParameter.kHeigh4DigZL*(1<<VX_FRACTION)), 3 );
			case 3:
				toolzerAligmentTerrain4ZP.influenceDZ(x, y, round(20*trucksIntrumentParameter.kRadius4DigZL), 0<<VX_FRACTION, 6 );
				///clearAtrBaseOfBuildingCorrupt(x,y,16);
				break;
			case 4:
				//toolzerAligmentTerrain4ZP.influenceDZ(x, y, 5, -3<<VX_FRACTION, 3 );
				//toolzerAligmentTerrain4ZP.influenceDZ(x, y, 16, 0<<VX_FRACTION, 7 );
				toolzerAligmentTerrain4ZP.influenceDZ(x, y, round(10*trucksIntrumentParameter.kRadius4DigZL), round(-3*trucksIntrumentParameter.kHeigh4DigZL*(1<<VX_FRACTION)), 3 );
			case 5:
				toolzerAligmentTerrain4ZP.influenceDZ(x, y, round(20*trucksIntrumentParameter.kRadius4DigZL), 0<<VX_FRACTION, 7 );
				clearAtrBaseOfBuildingCorrupt(x,y,16);
				return 0;
				break;
			default:
				return 0;
			};
			return 1;
		}
		else {
			switch(phase){
			case 0:
				toolzerAligmentTerrainVariableH.influenceDZ(x, y, round(5*trucksIntrumentParameter.kRadius4DigZL), round(-3*trucksIntrumentParameter.kHeigh4DigZL*(1<<VX_FRACTION)), 3 );
				toolzerAligmentTerrainVariableH.influenceDZ(x, y, round(16*trucksIntrumentParameter.kRadius4DigZL), 0<<VX_FRACTION, 5 );
				///clearAtrBaseOfBuildingCorrupt(x,y,16);
				break;
			case 1:
				toolzerAligmentTerrainVariableH.influenceDZ(x, y, round(5*trucksIntrumentParameter.kRadius4DigZL), round(-3*trucksIntrumentParameter.kHeigh4DigZL*(1<<VX_FRACTION)), 3 );
				toolzerAligmentTerrainVariableH.influenceDZ(x, y, round(16*trucksIntrumentParameter.kRadius4DigZL), 0<<VX_FRACTION, 6 );
				///clearAtrBaseOfBuildingCorrupt(x,y,16);
				break;
			case 2:
				toolzerAligmentTerrainVariableH.influenceDZ(x, y, round(5*trucksIntrumentParameter.kRadius4DigZL), round(-3*trucksIntrumentParameter.kHeigh4DigZL*(1<<VX_FRACTION)), 3 );
				toolzerAligmentTerrainVariableH.influenceDZ(x, y, round(16*trucksIntrumentParameter.kRadius4DigZL), 0<<VX_FRACTION, 7 );
//				clearAtrBaseOfBuildingCorrupt(x,y,16);
				damagingBuildingsTolzer(x,y,16);
				return 0;
				break;
			default:
				return 0;
			};
			return 1;
		}
	};
};


class putToolzer4K: public baseDigPutToolzer {
public:
	int simpleQuant(){
		//toolzerChangeTerHeight
		//toolzerChangeTerHeightIfNotZP
		//toolzerPlotingSoot
		//toolzerEraseZP
		//toolzerAligmentTerrain4ZP
		//int influenceDZ(int x, int y, int rad, short dz, int smMode)
		//int influenceBM(int x, int y, int sx, int sy, unsigned char * imageArea)
		if(flag_leveled_status) {
			switch(phase){
			case 0:
				toolzerAligmentTerrain4ZP.influenceDZ(x, y, round(16*trucksIntrumentParameter.kRadius4PutZL), round(2*trucksIntrumentParameter.kHeigh4PutZL*(1<<VX_FRACTION)), 10 );
				toolzerAligmentTerrain4ZP.influenceDZ(x, y, round(16*trucksIntrumentParameter.kRadius4PutZL), 0<<VX_FRACTION, 3 );
				///clearAtrBaseOfBuildingCorrupt(x,y,16);
				break;
			case 1:
				toolzerAligmentTerrain4ZP.influenceDZ(x, y, round(16*trucksIntrumentParameter.kRadius4PutZL), round(1*trucksIntrumentParameter.kHeigh4PutZL*(1<<VX_FRACTION)), 10 );
				toolzerAligmentTerrain4ZP.influenceDZ(x, y, round(16*trucksIntrumentParameter.kRadius4PutZL), 0<<VX_FRACTION, 3 );
				clearAtrBaseOfBuildingCorrupt(x,y,16);
				return 0;
				break;
			default:
				return 0;
			};
			return 1;
		}
		else {
			switch(phase){
			case 0:
				toolzerAligmentTerrainVariableH.influenceDZ(x, y, round(16*trucksIntrumentParameter.kRadius4PutZL), round(2*trucksIntrumentParameter.kHeigh4PutZL*(1<<VX_FRACTION)), 10 );
				toolzerAligmentTerrainVariableH.influenceDZ(x, y, round(16*trucksIntrumentParameter.kRadius4PutZL), 0<<VX_FRACTION, 3 );
				///clearAtrBaseOfBuildingCorrupt(x,y,16);
				break;
			case 1:
				toolzerAligmentTerrainVariableH.influenceDZ(x, y, round(16*trucksIntrumentParameter.kRadius4PutZL), round(1*trucksIntrumentParameter.kHeigh4PutZL*(1<<VX_FRACTION)), 10 );
				toolzerAligmentTerrainVariableH.influenceDZ(x, y, round(16*trucksIntrumentParameter.kRadius4PutZL), 0<<VX_FRACTION, 3 );
				clearAtrBaseOfBuildingCorrupt(x,y,16);
				return 0;
				break;
			default:
				return 0;
			};
			return 1;
		}
	};
};

class digToolzerGarbage: public baseToolzer{
public:
	digToolzerGarbage(){
		minWHRestriction(0);
	};
	void minWHRestriction(int wh){
		toolzerAligmentTerrainVariableH.setHAppoximation(wh);
	}
	int simpleQuant(){
		//toolzerChangeTerHeight
		//toolzerChangeTerHeightIfNotZP
		//toolzerPlotingSoot
		//toolzerEraseZP
		//toolzerAligmentTerrain4ZP
		//int influenceDZ(int x, int y, int rad, short dz, int smMode)
		//int influenceBM(int x, int y, int sx, int sy, unsigned char * imageArea)
		//steps = {	{ type = TOOLZER_DZ; sx = 10; sy = 10; dh = -4.0; shape = 3;}	};
		//steps = {	{ type = TOOLZER_DZ; sx = 32; sy = 32; dh = 0.0; shape = 10;}	};
		//steps = {	{ type = TOOLZER_DZ; sx = 10; sy = 10; dh = -4.0; shape = 3;}	};
		//steps = {	{ type = TOOLZER_DZ; sx = 32; sy = 32; dh = 0.0; shape = 7;}	};
		//steps = {	{ type = TOOLZER_DZ; sx = 10; sy = 10; dh = -4.0; shape = 3;}	};
		//steps = {	{ type = TOOLZER_DZ; sx = 32; sy = 32; dh = 0.0; shape = 5;}	};
		//steps = {	{ type = TOOLZER_DZ; sx = 10; sy = 10; dh = -4.0; shape = 3;}	};
		//steps = {	{ type = TOOLZER_DZ; sx = 32; sy = 32; dh = 0.0; shape = 3;}	};
		//steps = {	{ type = TOOLZER_DZ; sx = 32; sy = 32; dh = 0.0; shape = 2;}	};

		switch(phase){
		case 0:
			toolzerAligmentTerrainVariableH.influenceDZ(x, y, round(5*trucksIntrumentParameter.kRadius4DigGarbage), round(-0.4f*trucksIntrumentParameter.kHeigh4DigGarbage*(1<<VX_FRACTION)), 5 );//5 //-4
			break;
		case 1:
			toolzerAligmentTerrainVariableH.influenceDZ(x, y, round(10*trucksIntrumentParameter.kRadius4DigGarbage), 0<<VX_FRACTION, 8 ); //16
			break;
		case 2:
			toolzerAligmentTerrainVariableH.influenceDZ(x, y, round(6*trucksIntrumentParameter.kRadius4DigGarbage), round(-0.3f*trucksIntrumentParameter.kHeigh4DigGarbage*(1<<VX_FRACTION)), 8 );//5 //-4
			break;
		case 3:
			toolzerAligmentTerrainVariableH.influenceDZ(x, y, round(10*trucksIntrumentParameter.kRadius4DigGarbage), 0<<VX_FRACTION, 7 ); //16
			break;
//		case 4:
//			toolzerAligmentTerrainVariableH.influenceDZ(x, y, 3, (-0.4f)*(1<<VX_FRACTION), 3 );//5 //-4
//			break;
		case 4:
			toolzerAligmentTerrainVariableH.influenceDZ(x, y, round(16*trucksIntrumentParameter.kRadius4DigGarbage), 0<<VX_FRACTION, 2 ); //16
			return 0;
			break;
		default:
			return 0;
		};
		return 1;
	};
};


class putToolzerGarbage: public baseToolzer{
public:
	putToolzerGarbage(){
		maxWHRestriction(250);
	}
	void maxWHRestriction(int wh){
		toolzerAligmentTerrainVariableH.setHAppoximation(wh);
	}
	int simpleQuant(){
		//toolzerChangeTerHeight
		//toolzerChangeTerHeightIfNotZP
		//toolzerPlotingSoot
		//toolzerEraseZP
		//toolzerAligmentTerrain4ZP
		//int influenceDZ(int x, int y, int rad, short dz, int smMode)
		//int influenceBM(int x, int y, int sx, int sy, unsigned char * imageArea)
		//steps = {	{ type = TOOLZER_DZ; sx = 32; sy = 32; dh = 0.5; shape = 10;}	};
		//steps = {	{ type = TOOLZER_DZ; sx = 32; sy = 32; dh = 0.5; shape = 10;}	};
		//steps = {	{ type = TOOLZER_DZ; sx = 32; sy = 32; dh = 0.5; shape = 10;}	};
		//steps = {	{ type = TOOLZER_DZ; sx = 32; sy = 32; dh = 0.5; shape = 10;}	};
		//steps = {	{ type = TOOLZER_DZ; sx = 32; sy = 32; dh = 0.5; shape = 10;}	};
		//steps = {	{ type = TOOLZER_DZ; sx = 32; sy = 32; dh = 0.0; shape = 10;}	};
		//steps = {	{ type = TOOLZER_DZ; sx = 32; sy = 32; dh = 0.0; shape = 8;}	};
		//steps = {	{ type = TOOLZER_DZ; sx = 32; sy = 32; dh = 0.0; shape = 7;}	};
		//steps = {	{ type = TOOLZER_DZ; sx = 32; sy = 32; dh = 0.0; shape = 5;}	};
		//steps = {	{ type = TOOLZER_DZ; sx = 32; sy = 32; dh = 0.0; shape = 3;}	};
		//steps = {	{ type = TOOLZER_DZ; sx = 32; sy = 32; dh = 0.0; shape = 2;}	};
		switch(phase){
		case 0:
			toolzerAligmentTerrainVariableH.influenceDZ(x, y, round(5*trucksIntrumentParameter.kRadius4PutGarbage), round(0.4f*trucksIntrumentParameter.kHeigh4PutGarbage*(1<<VX_FRACTION)), 10 );//16 //0.5f
			break;
		case 1:
			toolzerAligmentTerrainVariableH.influenceDZ(x, y, round(7*trucksIntrumentParameter.kRadius4PutGarbage), round(0.3f*trucksIntrumentParameter.kHeigh4PutGarbage*(1<<VX_FRACTION)), 10 );//16 //0.5f
			break;
//		case 2:
//			toolzerAligmentTerrainVariableH.influenceDZ(x, y, 10, round(0.5f*(1<<VX_FRACTION)), 10 );//16 //0.5f
//			break;
		case 2:
			toolzerAligmentTerrainVariableH.influenceDZ(x, y, round(10*trucksIntrumentParameter.kRadius4PutGarbage), 0, 6 );//16
			break;
		case 3:
			toolzerAligmentTerrainVariableH.influenceDZ(x, y, round(10*trucksIntrumentParameter.kRadius4PutGarbage), 0, 2 );//16
			return 0;
			break;
		default:
			return 0;
		};
		return 1;
	};
};

#endif // _PERIMETER_

class craterToolzerDestroyZP : public baseToolzer{
public:
	float kScale;
	craterToolzerDestroyZP(float _kScale=1.f){
		kScale=_kScale;
	};
	void setKScale(float _kScale=1.f){
		kScale=_kScale;
	}
	int simpleQuant(){
		//toolzerChangeTerHeight
		//toolzerChangeTerHeightIfNotZP
		//toolzerPlotingSoot
		//int influenceDZ(int x, int y, int rad, short dz, int smMode)
		//int influenceBM(int x, int y, int sx, int sy, unsigned char * imageArea)

		switch(phase){
		case 0:
			toolzerChangeTerHeight.influenceDZ(x, y, round(kScale*32), 2*(1<<VX_FRACTION), 3 );
			break;
		case 1:
			toolzerChangeTerHeight.influenceDZ(x, y, round(kScale*24), -4*(1<<VX_FRACTION), 3 );
			break;
		case 2:
			toolzerChangeTerHeight.influenceDZ(x, y, round(kScale*16), -4*(1<<VX_FRACTION), 4 );
			break;
		case 3:
			toolzerChangeTerHeight.influenceDZ(x, y, round(kScale*32), 0, 2 );
			break;
		case 4:
			toolzerPlotingSoot.influenceDZ(x, y, round(kScale*27), 1*(1<<VX_FRACTION), 10);
			break;
		case 5:
			toolzerPlotingSoot.influenceDZ(x, y, round(kScale*38), 1*(1<<VX_FRACTION), 7);
			damagingBuildingsTolzer(x, y, round(kScale*32));
			return 0;
			break;
		default:
			return 0;
		};
		return 1;
	};
};
////////////////
class wormInGroundToolzer: public baseToolzer{
public:
	float angle;
	wormInGroundToolzer(float _angle=0.f){
		setAngle(_angle);
	};
	void setAngle(float _angle){
		angle=_angle + M_PI/2;
	};
	int simpleQuant(){
		//toolzerChangeTerHeight
		//toolzerChangeTerHeightIfNotZP
		//toolzerPlotingSoot
		//int influenceDZ(int x, int y, int rad, short dz, int smMode)
		//int influenceBM(int x, int y, int sx, int sy, unsigned char * imageArea)
		int xu=vMap.XCYCL(round(x+cos(angle)*10.));
		int yu=vMap.YCYCL(round(y+sin(angle)*10.));
		int xd=vMap.XCYCL(round(x-cos(angle)*10.));
		int yd=vMap.YCYCL(round(y-sin(angle)*10.));
		switch(phase){
		case 0:
			toolzerChangeTerHeight.influenceDZ(xu, yu, 14, 4*(1<<VX_FRACTION), 10 );
			toolzerChangeTerHeight.influenceDZ(xd, yd, 14, 4*(1<<VX_FRACTION), 10 );
			toolzerChangeTerHeight.influenceDZ(xu, yu, 16, 0, 3 );
			toolzerChangeTerHeight.influenceDZ(xd, yd, 16, 0, 3 );
			break;
		case 1:
			toolzerChangeTerHeight.influenceDZ(xu, yu, 14, round(0.5*(1<<VX_FRACTION)), 10 );
			toolzerChangeTerHeight.influenceDZ(xd, yd, 14, round(0.5*(1<<VX_FRACTION)), 10 );
			break;
		case 2:
			toolzerChangeTerHeight.influenceDZ(xu, yu, 14, -round(0.5*(1<<VX_FRACTION)), 10 );
			toolzerChangeTerHeight.influenceDZ(xd, yd, 14, -round(0.5*(1<<VX_FRACTION)), 10 );
			break;
		case 3:
			toolzerChangeTerHeight.influenceDZ(xu, yu, 16, 0, 3 );
			toolzerChangeTerHeight.influenceDZ(xd, yd, 16, 0, 3 );
			return 0;
			break;
		default:
			return 0;
		};
		return 1;
	};
};


class wormOutGroundToolzer: public baseToolzer{
public:
	float angle;
	wormOutGroundToolzer(float _angle=0.f){
		setAngle(_angle);
	};
	void setAngle(float _angle){
		angle=_angle + M_PI/2;
	};
	int simpleQuant(){
		//toolzerChangeTerHeight
		//toolzerChangeTerHeightIfNotZP
		//toolzerPlotingSoot
		//int influenceDZ(int x, int y, int rad, short dz, int smMode)
		//int influenceBM(int x, int y, int sx, int sy, unsigned char * imageArea)
		int xu=vMap.XCYCL(round(x+cos(angle)*10.));
		int yu=vMap.YCYCL(round(y+sin(angle)*10.));
		int xd=vMap.XCYCL(round(x-cos(angle)*10.));
		int yd=vMap.YCYCL(round(y-sin(angle)*10.));
		switch(phase){
		case 0:
			toolzerChangeTerHeight.influenceDZ(xu, yu, 20, 2*(1<<VX_FRACTION), 5 );
			toolzerChangeTerHeight.influenceDZ(xd, yd, 20, 2*(1<<VX_FRACTION), 5 );
			toolzerChangeTerHeight.influenceDZ(xu, yu, 24, 0, 3 );
			toolzerChangeTerHeight.influenceDZ(xd, yd, 24, 0, 3 );

			//toolzerChangeTerHeight.influenceDZ(x, y, 28, 2*(1<<VX_FRACTION), 10 );
			break;
		case 1:
			toolzerChangeTerHeight.influenceDZ(xu, yu, 13, -4*(1<<VX_FRACTION), 10 );
			toolzerChangeTerHeight.influenceDZ(x,  y,  13, -4*(1<<VX_FRACTION), 10 );
			toolzerChangeTerHeight.influenceDZ(xd, yd, 13, -4*(1<<VX_FRACTION), 10 );
			toolzerChangeTerHeight.influenceDZ(xu, yu, 16, 0, 5 );
			toolzerChangeTerHeight.influenceDZ(x , y , 16, 0, 5 );
			toolzerChangeTerHeight.influenceDZ(xd, yd, 16, 0, 5 );
			return 0;
			//toolzerChangeTerHeight.influenceDZ(x, y, 32, 0, 3 );
			break;
		case 2:
			//toolzerChangeTerHeight.influenceDZ(x, y, 28, -2*(1<<VX_FRACTION), 10 );
			break;
		case 3:
			//toolzerChangeTerHeight.influenceDZ(x, y, 32, 0, 3 );
			return 0;
			break;
		default:
			return 0;
		};
		return 1;
	};
};


class debrisToolzer: public baseToolzer{
public:
	int radius;
	float kScale;
	debrisToolzer(int _radius=32){
		setRadius(_radius);
	};
	void setRadius(int _radius=32){
		radius=_radius;
		kScale=(float)radius/32.f;
	}
	int simpleQuant(){
		//toolzerChangeTerHeight
		//toolzerChangeTerHeightIfNotZP
		//toolzerPlotingSoot
		//int influenceDZ(int x, int y, int rad, short dz, int smMode)
		//int influenceBM(int x, int y, int sx, int sy, unsigned char * imageArea)

		switch(phase){
		case 0:
			toolzerChangeTerHeight.influenceDZ(x, y, round(kScale*32), -2*(1<<VX_FRACTION), 3 );
			break;
		case 1:
			toolzerChangeTerHeight.influenceDZ(x, y, round(kScale*24), +4*(1<<VX_FRACTION), 3 );
			break;
		case 2:
			toolzerChangeTerHeight.influenceDZ(x, y, round(kScale*16), +4*(1<<VX_FRACTION), 4 );
			break;
		case 3:
			toolzerChangeTerHeight.influenceDZ(x, y, round(kScale*32), 0, 2 );
			break;
		case 4:
			toolzerPlotingSoot.influenceDZ(x, y, round(kScale*27), 1*(1<<VX_FRACTION), 10);
			break;
		case 5:
			toolzerPlotingSoot.influenceDZ(x, y, round(kScale*32), 1*(1<<VX_FRACTION), 10);
			return 0;
			break;
		default:
			return 0;
		};
		return 1;
	};
};


class ghostToolzer: public baseToolzer{
public:
	int radius;
	float kScale;
	ghostToolzer(int _radius=32){
		setRadius(_radius);
	};
	void setRadius(int _radius=32){
		radius=_radius;
		kScale=(float)radius/32.f;
	}
	int simpleQuant(){
		//toolzerChangeTerHeight
		//toolzerChangeTerHeightIfNotZP
		//toolzerPlotingSoot
		//int influenceDZ(int x, int y, int rad, short dz, int smMode)
		//int influenceBM(int x, int y, int sx, int sy, unsigned char * imageArea)

		switch(phase){
		case 0:
			toolzerPlottingVeryLightDam.influenceDZ(x, y, round(kScale*27), 1*(1<<VX_FRACTION), 5);
			break;
		case 1:
			toolzerPlottingVeryLightDam.influenceDZ(x, y, round(kScale*32), 1*(1<<VX_FRACTION), 10);
			return 0;
			break;
		default:
			return 0;
		};
		return 1;
	};
};

class demonToolzer: public baseToolzer{
public:
	int radius;
	float kScale;
	demonToolzer(int _radius=32){
		setRadius(_radius);
	};
	void setRadius(int _radius=32){
		radius=_radius;
		kScale=(float)radius/32.f;
	}
	int simpleQuant(){
		//toolzerChangeTerHeight
		//toolzerChangeTerHeightIfNotZP
		//toolzerPlotingSoot
		//int influenceDZ(int x, int y, int rad, short dz, int smMode)
		//int influenceBM(int x, int y, int sx, int sy, unsigned char * imageArea)

		switch(phase){
		case 0:
			toolzerPlotingSoot.influenceDZ(x, y, round(kScale*18), 1*(1<<VX_FRACTION), 10, 0);
			break;
		case 1:
		case 2:
			break;
		case 3:
			toolzerPlotingSoot.influenceDZ(x, y, round(kScale*24), 1*(1<<VX_FRACTION), 10, 2);
			break;
		case 4:
		case 5:
			break;
		case 6:
			toolzerPlotingSoot.influenceDZ(x, y, round(kScale*32), 1*(1<<VX_FRACTION), 10, 2);
			return 0;
			break;
		default:
			return 0;
		};
		return 1;
	};
};


class craterToolzerAbyssMake : public baseToolzer{
public:
	int simpleQuant(){
		//toolzerChangeTerHeight
		//toolzerChangeTerHeightIfNotZP
		//toolzerPlotingSoot
		//int influenceDZ(int x, int y, int rad, short dz, int smMode)
		//int influenceBM(int x, int y, int sx, int sy, unsigned char * imageArea)

		switch(phase){
		case 0:
			toolzerChangeTerHeight.influenceDZ(x, y,5,-5*(1<<VX_FRACTION), 3 );
//			toolzerChangeTerHeight.influenceDZ(x, y,16,0,5);
			return 0;
		default:
			return 0;
		};
		return 1;
	};
};

class craterToolzerWallMake : public baseToolzer{
public:
	int simpleQuant(){
		//toolzerChangeTerHeight
		//toolzerChangeTerHeightIfNotZP
		//toolzerPlotingSoot
		//int influenceDZ(int x, int y, int rad, short dz, int smMode)
		//int influenceBM(int x, int y, int sx, int sy, unsigned char * imageArea)

		switch(phase){
		case 0:
			toolzerChangeTerHeight.influenceDZ(x,y,16,3*(1<<VX_FRACTION),2);
			toolzerChangeTerHeight.influenceDZ(x,y,16,4,10);
			return 0;
		default:
			return 0;
		};
		return 1;
	};
};


class eraseZPToolzer: public baseToolzer{
public:
	int simpleQuant(){
		//toolzerChangeTerHeight
		//toolzerChangeTerHeightIfNotZP
		//toolzerPlotingSoot
		//toolzerEraseZP
		//int influenceDZ(int x, int y, int rad, short dz, int smMode)
		//int influenceBM(int x, int y, int sx, int sy, unsigned char * imageArea)

		switch(phase){
		case 0:
			toolzerEraseZP.influenceDZ(x, y, 4, 1<<VX_FRACTION, 5 );
			break;
		case 1:
			toolzerEraseZP.influenceDZ(x, y, 8, 1<<VX_FRACTION, 5 );
			break;
		case 2:
			toolzerEraseZP.influenceDZ(x, y, 8, 1<<VX_FRACTION, 0 );
			return 0;
			break;
		default:
			return 0;
		};
		return 1;
	};
};


inline void plotCircleZL(int x, int y, int r){
	if(r>MAX_RADIUS_CIRCLEARR){
		xassert(0&&"exceeding max radius in plotCircleZL ");
		r=MAX_RADIUS_CIRCLEARR;
	}
	int i, j;
	for(i = 0;i <= r; i++){
		int max = maxRad[i];
		int* xx = xRad[i];
		int* yy = yRad[i];
		for(j = 0;j < max;j++) {
			int offB=vMap.offsetBuf(vMap.XCYCL(x + xx[j]), vMap.YCYCL(y + yy[j]));
			if( (vMap.AtrBuf[offB]&At_LEVELED)!=0 ) vMap.AtrBuf[offB]|=At_ZEROPLAST;
		}
	}
	vMap.recalcArea2Grid( vMap.XCYCL(x-r), vMap.YCYCL(y-r), vMap.XCYCL(x+r), vMap.YCYCL(y+r) );
	vMap.regRender( vMap.XCYCL(x-r), vMap.YCYCL(y-r), vMap.XCYCL(x+r), vMap.YCYCL(y+r) );
}

inline void eraseCircleZL(int x, int y, int r){
	if(r>MAX_RADIUS_CIRCLEARR){
		xassert(0&&"exceeding max radius in eraseCircleZL ");
		r=MAX_RADIUS_CIRCLEARR;
	}
	int i, j;
	for(i = 0;i <= r; i++){
		int max = maxRad[i];
		int* xx = xRad[i];
		int* yy = yRad[i];
		for(j = 0;j < max;j++) {
			int curX=vMap.XCYCL(x + xx[j]);
			int curY=vMap.YCYCL(y + yy[j]);
			int offB=vMap.offsetBuf(curX, curY);
#ifndef _SURMAP_
			///if(!(vMap.GABuf[vMap.offsetGBuf(curX>>kmGrid, curY>>kmGrid)] & GRIDAT_MASK_CLUSTERID) && terEnergyBorderCheck(curX, curY))
#endif
				if((vMap.AtrBuf[offB]&At_ZPMASK)==At_ZEROPLAST) vMap.AtrBuf[offB]=(vMap.AtrBuf[offB]&(~At_NOTPURESURFACE))|At_LEVELED;
		}
	}
	vMap.recalcArea2Grid( vMap.XCYCL(x-r), vMap.YCYCL(y-r), vMap.XCYCL(x+r), vMap.YCYCL(y+r) );
	vMap.regRender( vMap.XCYCL(x-r), vMap.YCYCL(y-r), vMap.XCYCL(x+r), vMap.YCYCL(y+r) );
}

inline void flashCircleLeveling(int x, int y, int r){
	if(r>MAX_RADIUS_CIRCLEARR){
		xassert(0&&"exceeding max radius in flashCircleLeveling");
		r=MAX_RADIUS_CIRCLEARR;
	}
	int i, j;
	for(i = 0;i <= r; i++){
		int max = maxRad[i];
		int* xx = xRad[i];
		int* yy = yRad[i];
		for(j = 0;j < max;j++) {
			int offB=vMap.offsetBuf(vMap.XCYCL(x + xx[j]), vMap.YCYCL(y + yy[j]));
			if(vMap.VxDBuf[offB]==0) vMap.VxGBuf[offB]=vMap.hZeroPlast;
			else vMap.VxDBuf[offB]=vMap.hZeroPlast;
			vMap.AtrBuf[offB]=At_LEVELED | (vMap.AtrBuf[offB]&At_SHADOW);
		}
	}
	vMap.recalcArea2Grid( vMap.XCYCL(x-r), vMap.YCYCL(y-r), vMap.XCYCL(x+r), vMap.YCYCL(y+r) );
	vMap.regRender( vMap.XCYCL(x-r), vMap.YCYCL(y-r), vMap.XCYCL(x+r), vMap.YCYCL(y+r) );
}

inline void flashPixelLeveling(int x, int y){
	int offB=vMap.offsetBuf(x, y);
	if(vMap.VxDBuf[offB]==0) vMap.VxGBuf[offB]=vMap.hZeroPlast;
	else vMap.VxDBuf[offB]=vMap.hZeroPlast;
	vMap.AtrBuf[offB]=At_LEVELED | (vMap.AtrBuf[offB]&At_SHADOW);

	vMap.recalcArea2Grid( vMap.XCYCL(x-1), vMap.YCYCL(y-1), vMap.XCYCL(x+1), vMap.YCYCL(y+1) );
	vMap.regRender( vMap.XCYCL(x-1), vMap.YCYCL(y-1), vMap.XCYCL(x+1), vMap.YCYCL(y+1) );
}


//#################################################################################################



///////////////////////////////////////////////////////////////////////
struct sVBitMapMosaic {
	unsigned short sx,sy;
	int sz;
	unsigned char* data;
	int effect;//force,
	int k_effect;
	int xoffset,yoffset;
	double A11, A12, A21, A22;
	double X, Y;
	bool Enable;

	sVBitMapMosaic(void){
		data = NULL;
		//force = 256; 
		effect = 0;
		k_effect = 256;
		xoffset = yoffset = 0;
		Enable=0;
	};

	void create(int _sx, int _sy){
		sx=_sx; sy=_sy;
		data=new unsigned char[sx*sy];
	};
	void release(void){
		if(data != NULL) {
			delete [] data;
			data=NULL;
		}
		Enable=0;
	};
	~sVBitMapMosaic(void){
		release();
	};

	inline int getDelta(int x,int y,int delta){
		x += xoffset;
		y += yoffset;
		x+=vMap.H_SIZE;
		y+=vMap.V_SIZE;
		int x1,y1;
		x1 = round(A11*x + A12*y + X);
		y1 = round(A21*x + A22*y + Y);
		while (x<0){ x+=sx;}
		while (y<0){ y+=sy;}
		if(!effect) return delta*(data[(y1%sy)*sx + (x1%sx)])/256; //-128
		return delta + k_effect*(data[(y1%sy)*sx + (x1%sx)]-128)/256;//force*
	}
/*	inline int getType(int x,int y){ 
		x += xoffset;
		y += yoffset;
		x+=vMap.H_SIZE;
		y+=vMap.V_SIZE;
		int x1,y1;
		x1 = round(A11*x + A12*y + X);
		y1 = round(A21*x + A22*y + Y);
		while (x<0){ x+=sx;}
		while (y<0){ y+=sy;}
		return MosaicTypes[data[(y1%sy)*sx + (x1%sx)]%8]; 
	}*/
	inline int getColor(int x, int y){
		x += xoffset;
		y += yoffset;
		x+=vMap.H_SIZE;
		y+=vMap.V_SIZE;
		int x1,y1;
		x1 = round(A11*x + A12*y + X);
		y1 = round(A21*x + A22*y + Y);
		while (x<0){ x+=sx;}
		while (y<0){ y+=sy;}
		return data[(y1%sy)*sx + (x1%sx)]; 
	};

	void set_alpha(double alpha, int X0, int Y0){
		X0+=vMap.H_SIZE;
		Y0+=vMap.V_SIZE;
		//alpha-=90;
		A11 = A22 = cos(alpha*3.1415926535/180);
		A21 = -(A12 = sin(alpha*3.1415926535/180));
		X = X0 - A11*X0 - A12*Y0;
		Y = Y0 - A21*X0 - A22*Y0;
	};

};
extern sVBitMapMosaic VBitMapMosaic;
////////////////////////////////////////////////////////////////////////////////

struct sRGBBitMap {
	unsigned short sx,sy;
	unsigned char* data;

	sRGBBitMap(void){
		data = 0;
	};

	void create(int _sx, int _sy){
		sx=_sx; sy=_sy;
		data=new unsigned char[sx*sy*3];//RGB
	};
	void release(void){
		if(data != 0) {
			delete [] data;
			data=0;
		}
	};
	~sRGBBitMap(void){
		release();
	};

	inline int getColor(int x, int y){
		return data[((y%sy)*sx + (x%sx))*3]; 
	};

};



////////////////////////////////////////////////////////////////////////////////
struct sVBitMap {
	unsigned short mX, mY;
	unsigned short sx,sy;
	unsigned char* data;
	float A11, A12, A21, A22;
	float X, Y;
	float kX, kY, kZ;

	sVBitMap(void){
		data = NULL;
	};

	void create(int _sx, int _sy){
		sx=_sx; sy=_sy;
		data=new unsigned char[sx*sy];
	};
	void clear(){
		const int size=sx*sy;
		for(int i=0; i<size; i++) data[i]=0;
	}
	void release(void){
		if(data != NULL) {
			delete [] data;
			data=NULL;
		}
	};
	~sVBitMap(void){
		release();
	};


	void put(int VBMmode, int VBMlevel, int VBMnoiseLevel, int VBMnoiseAmp, bool VBMinverse);

	inline int getColor(int x, int y){
		int x1,y1;
		x1 = round((A11*x + A12*y + X)*kX+(sx>>1));
		y1 = round((A21*x + A22*y + Y)*kY+(sy>>1));
		if(x1<0 || y1<0 || x1>=sx || y1>=sy) return -1;
		return data[(y1%sy)*sx + (x1%sx)]; 
	};

	inline int getPreciseColor(int x, int y){
		int x1,y1,xt, yt;
		xt = round( ((A11*x + A12*y + X)*kX+(sx>>1))*256 );
		yt = round( ((A21*x + A22*y + Y)*kY+(sy>>1))*256 );
		x1=xt>>8; y1=yt>>8; xt&=0xFF; yt&=0xFF;
		if(x1<0 || y1<0 || (x1+1)>=sx || (y1+1)>=sy) return 0;
		int h0,h1,h2,h3;
		h0=data[y1*sx + x1];
		h1=data[y1*sx + x1+1];
		h2=data[(y1+1)*sx + x1];
		h3=data[(y1+1)*sx + x1+1];
		h0=(h0<<8)+xt*(h1-h0);
		h2=(h2<<8)+xt*(h3-h2);
		return( (h0<<8)+yt*(h2-h0) )>>16; 
	};

	void Draw(int ViewX, int ViewY, float k_scale, unsigned short* GRAF_BUF, int GB_MAXX, int GB_MAXY){
		int x,y;
		float Ik_scale= 1.0f/k_scale;
		unsigned short * b=GRAF_BUF;
		int addx=ViewX-round((float)(GB_MAXX>>1)*Ik_scale);
		int addy=ViewY-round((float)(GB_MAXY>>1)*Ik_scale);
		for(y=0; y<GB_MAXY; y++){
			for(x=0; x<GB_MAXX; x++){
				int c=getColor(addx+round((float)x*Ik_scale), addy+round((float)y*Ik_scale));
				//if(c!=-1) *b=c;
				if(c>0) *b=c;
				b++;
			}
		}
	};

	void set_alpha(float alpha, int X0, int Y0, float _kX, float _kY, float _kZ){
		A11 = A22 = cosf(alpha*3.1415926535f/180.f);
		A21 = -(A12 = sinf(alpha*3.1415926535f/180.f));
		X = - A11*X0 - A12*Y0 ;
		Y = - A21*X0 - A22*Y0 ;
		kX=_kX; kY=_kY; kZ=_kZ;
		mX=X0; mY=Y0;
	};
	void set_alphaR(float alpha, int X0, int Y0){
		A11 = A22 = cosf(alpha);
		A21 = -(A12 = sinf(alpha));
		X = - A11*X0 - A12*Y0 ;
		Y = - A21*X0 - A22*Y0 ;
		kX=1.f; kY=1.f; kZ=1.f;
	};

};
extern sVBitMap VBitMap;

struct sTerrainMetod {
	int mode;
	int level;
	int noiseLevel;
	int noiseAmp;
	//bool inverse;
	sTerrainMetod(void){
		mode=4; level=0; noiseLevel=0; noiseAmp=1<<VX_FRACTION;
	};
	void update( int _mode, int _level, int _noiseLevel, int _noiseAmp){
		mode=_mode; level=_level; noiseLevel=_noiseLevel; noiseAmp=_noiseAmp;
	};
	void put(int x, int y, int v);
};
extern sTerrainMetod TerrainMetod;

#endif // __TOOLS_H__
