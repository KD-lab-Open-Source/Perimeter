#include "stdafxTr.h"


static const int MIN_GG_ALT=0;
//static const int MAX_GG_ALT=148<<VX_FRACTION;
//static const int NOISELEVEL_GG=100<<VX_FRACTION;
static const int MINSQUARE_GG=8;//степень двойки

static int Cmin_alt = MIN_VX_HEIGHT;
static int Cmax_alt = MAX_VX_HEIGHT;

static void gaussFilter(int * alt_buff, double filter_scaling, int x_size, int y_size);

#define MAP_1(x, y) (map1[y*sizeX+x])
#define MAP_2(x, y) (map2[y*sizeX+x])
#define MAP_T(x, y) (mapT[y*sizeX+x])
#define ROUGH_GRID(x, y) (roughGrid[y*netRSizeX+x])
#define WORK_GRID(x, y) (workGrid[y*netSizeX+x])
//BorderForm 0- нет границы;1- на границе сетка падает до 0; 2- граница по exp 4 cтепени; 3- по exp 3 степени; 4- по exp 2 степени
void geoGeneration(sGeoPMO& var)///(int _x, int _y, int sx, int sy, int MAX_GG_ALT, int kPowCellSize, int kPowShiftCS4RG, int GeonetMESH, int NOISELEVEL_GG, int BorderForm, bool _inverse)
{
	if(vMap.flag_record_operation) {
		vMap.containerPMO.push_back( ElementPMO(var) );
	}

	int _x=var.x;
	int _y=var.y;
	int sx=var.sx;
	int sy=var.sy;
	int MAX_GG_ALT=var.maxGGAlt;
	int kPowCellSize=var.powerCellSize;
	int kPowShiftCS4RG=var.powerShift;
	int GeonetMESH=var.geoNetMesh;
	int NOISELEVEL_GG=var.noiseLevel;
	int BorderForm=var.borderForm;
	bool _inverse=var.inverse;

	Cmax_alt=MAX_GG_ALT;///!!!!

	vMap.FilterMinHeight=0; vMap.FilterMaxHeight=MAX_VX_HEIGHT;///!!!

	if(kPowCellSize < 2) kPowCellSize=2;//минимальная ячейка 4x4
	if(kPowShiftCS4RG >= kPowCellSize) kPowShiftCS4RG=kPowCellSize-1;//минимальная ячейка R сетки 2x2

	int cellSize=1<<kPowCellSize;
	int net05SizeX=((sx/2)/cellSize+1); // +1 -запас по границе 
	int net05SizeY=((sy/2)/cellSize+1);
	int netSizeX=net05SizeX*2+1+1;//+1 для центра //+1 добавка по правому краю для правильности
	int netSizeY=net05SizeY*2+1+1;
	int sizeX=netSizeX*cellSize;
	int sizeY=netSizeY*cellSize;
	int xBeg=_x-sx/2;///(net05SizeX*cellSize);
	int yBeg=_y-sy/2;///(net05SizeY*cellSize);

	int DIVIDER=1<<kPowShiftCS4RG;
	int cellSizeR=cellSize/DIVIDER;
	int netR05SizeX=net05SizeX*DIVIDER; // +1 -запас по границе 
	int netR05SizeY=net05SizeY*DIVIDER;
	int netRSizeX=netR05SizeX*2+1+1;//+1 для центра //+1 добавка по правому краю для правильности
	int netRSizeY=netR05SizeY*2+1+1;

	int sizeGrid=netSizeX*netSizeY;
	int sizeGridR=netRSizeX*netRSizeY;
	int * roughGrid=new int[sizeGridR];
	int * workGrid=new int[sizeGrid];

	int sizeMap=sizeX*sizeY;
	int * map1=new int[sizeMap];
	int * map2=new int[sizeMap];
	int * mapT=new int[sizeMap];




	//memset(workGrid, 0, sizeGrid*sizeof(int));
	float sy05=net05SizeY;//netSizeY/2.0f;
	float sx05=net05SizeX;//netSizeX/2.0f;
	int xNet, yNet;
	for(yNet=0; yNet<netSizeY; yNet++){
		float y=(float)(yNet-sy05)/(float)sy05;
		for(xNet=0; xNet<netSizeX; xNet++){
			//ROUGH_GRID(xNet, yNet)=MIN_GG_ALT+XRnd(MAX_GG_ALT-MIN_GG_ALT);
			//WORK_GRID(xNet, yNet)=MIN_GG_ALT+XRnd(MAX_GG_ALT-MIN_GG_ALT);
			if(BorderForm!=0){
				float x=(float)(xNet-sx05)/(float)sx05; 
				float f=exp(-(fabsf(x*x*x*x)+fabsf(y*y*y*y))/(0.4*0.4));
				WORK_GRID(xNet, yNet)=round((MIN_GG_ALT+XRnd(MAX_GG_ALT-MIN_GG_ALT))*f);
			}
			else{ WORK_GRID(xNet, yNet)=MIN_GG_ALT+XRnd(MAX_GG_ALT-MIN_GG_ALT); }

			//ROUGH_GRID(xNet, yNet)=round((MIN_GG_ALT+XRnd(MAX_GG_ALT-MIN_GG_ALT))*f);
			//f=1;
			//WORK_GRID(xNet, yNet)=round((MIN_GG_ALT+XRnd(MAX_GG_ALT-MIN_GG_ALT))*f);
/*			if(WORK_GRID(xNet, yNet)==0){
				int dx=1+XRnd(4);
				int dy=1+XRnd(4);
				int xx,yy;
				int h=MIN_GG_ALT+XRnd(MAX_GG_ALT-MIN_GG_ALT);
				for(yy=0; yy<dy; yy++){
					for(xx=0; xx<dx; xx++){
						int nx=xNet+xx; if(nx>netSizeX)continue;
						int ny=yNet+yy; if(ny>netSizeY)continue;
						if(WORK_GRID(nx, ny)==0) WORK_GRID(nx, ny)=h;
					}
				}
			}*/
		}
	}
	//gaussFilter(workGrid, .7, netSizeX, netSizeY);
/////////
/*	extern int* xRad[MAX_RADIUS_CIRCLEARR + 1];
	extern int* yRad[MAX_RADIUS_CIRCLEARR + 1];
	extern int maxRad[MAX_RADIUS_CIRCLEARR + 1];
	int i,j,rad=sx/2;
	for(i = 0;i <= rad;i++){
		max = maxRad[i];
		xx = xRad[i];
		yy = yRad[i];
		for(j = 0;j < max;j++)
			if( ( (sx/2 + xx[j])%cellSize==0)&& ( (sy/2 + yy[j])%cellSize==0 ) ){
				WORK_GRID( ((sx/2 + xx[j])/cellSize),  ((sy/2 + yy[j])/cellSize) );
			}
		}
*/
/////////
	for(yNet=0; yNet<netRSizeY; yNet++){
		for(xNet=0; xNet<netRSizeX; xNet++){
			ROUGH_GRID(xNet, yNet)=MIN_GG_ALT+XRnd(MAX_GG_ALT-MIN_GG_ALT);
			//WORK_GRID(xNet, yNet)=MIN_GG_ALT+XRnd(MAX_GG_ALT-MIN_GG_ALT);
		}
	}

	int xMap, yMap, i;
	for(i=0; i<sizeMap; i++){
		map1[i]=XRnd(NOISELEVEL_GG);
		map2[i]=XRnd(NOISELEVEL_GG);
	}
	//подготовка R карты
	for(yNet=0, yMap=0; yNet<netRSizeY; yNet++, yMap+=cellSizeR){
		for(xNet=0, xMap=0; xNet<netRSizeX; xNet++, xMap+=cellSizeR){
			MAP_1(xMap, yMap)=ROUGH_GRID(xNet, yNet);
		}
	}


	int which = 0;
	int square_size,x1,y1,x2,y2;
	int p1,p2,p3,p4;
	int random_center, random_range, ind;
	int i1,i2,i3,i4;
	int i1_9,i2_9,i3_9,i4_9;
	int i1_3,i2_3,i3_3,i4_3;

	for(ind=0, square_size=cellSizeR; square_size>MINSQUARE_GG; square_size>>=1, ind++){
		for(y1=0; y1<(sizeY-cellSizeR); y1+=square_size){
			for(x1 = 0; x1< (sizeX-cellSizeR); x1+=square_size){
				x2 = (x1 + square_size);
				y2 = y1 + square_size;
				if(!which){
					i1 = MAP_1(x1, y1);
					i2 = MAP_1(x2, y1);
					i3 = MAP_1(x1, y2);
					i4 = MAP_1(x2, y2);
				}
				else {
					i1 = MAP_2(x1, y1);
					i2 = MAP_2(x2, y1);
					i3 = MAP_2(x1, y2);
					i4 = MAP_2(x2, y2);
				}
				i1_3 = i1*3; i1_9 = i1_3*3;
				i2_3 = i2*3; i2_9 = i2_3*3;
				i3_3 = i3*3; i3_9 = i3_3*3;
				i4_3 = i4*3; i4_9 = i4_3*3;
				p1 = (i1_9 + i2_3 + i3_3 + i4) >> 4;
				p2 = (i1_3 + i2_9 + i3 + i4_3) >> 4;
				p3 = (i1_3 + i2 + i3_9 + i4_3) >> 4;
				p4 = (i1 + i2_3 + i3_3 + i4_9) >> 4;

				random_center = square_size<<(VX_FRACTION); ///?
				random_range = random_center << 1;

				p1 += random(random_range) - random_center;
				p2 += random(random_range) - random_center;
				p3 += random(random_range) - random_center;
				p4 += random(random_range) - random_center;

				if(p1 < Cmin_alt) p1 = Cmin_alt; else if(p1 > Cmax_alt) p1 = Cmax_alt;
				if(p2 < Cmin_alt) p2 = Cmin_alt; else if(p2 > Cmax_alt) p2 = Cmax_alt;
				if(p3 < Cmin_alt) p3 = Cmin_alt; else if(p3 > Cmax_alt) p3 = Cmax_alt;
				if(p4 < Cmin_alt) p4 = Cmin_alt; else if(p4 > Cmax_alt) p4 = Cmax_alt;

				x2 = (x1 + (square_size >> 1)) ;
				y2 = y1 + (square_size >> 1);

				if(!which){
					MAP_2(x1, y1) = (unsigned short)p1;
					MAP_2(x2, y1) = (unsigned short)p2;
					MAP_2(x1, y2) = (unsigned short)p3;
					MAP_2(x2, y2) = (unsigned short)p4;
				}
				else {
					MAP_1(x1, y1) = (unsigned short)p1;
					MAP_1(x2, y1) = (unsigned short)p2;
					MAP_1(x1, y2) = (unsigned short)p3;
					MAP_1(x2, y2) = (unsigned short)p4;
				}
			}
		}
		which = 1 - which;
	}

	if(!which) memcpy(mapT, map1, sizeMap*sizeof(int) );
	else memcpy(mapT, map2, sizeMap*sizeof(int)  );
///////////////////////////////////////////////////////////////////////////////////

	//подготовка M карты
	for(yNet=0, yMap=0; yNet<netSizeY; yNet++, yMap+=cellSize){
		for(xNet=0, xMap=0; xNet<netSizeX; xNet++, xMap+=cellSize){
			MAP_1(xMap, yMap)=WORK_GRID(xNet, yNet);
		}
	}

	which = 0;

	for(ind=0, square_size=cellSize; square_size>1; square_size>>=1, ind++){
		for(y1=0; y1<(sizeY-cellSize); y1+=square_size){
			for(x1 = 0; x1< (sizeX-cellSize); x1+=square_size){
				x2 = (x1 + square_size);
				y2 = y1 + square_size;
				if(!which){
					i1 = MAP_1(x1, y1);
					i2 = MAP_1(x2, y1);
					i3 = MAP_1(x1, y2);
					i4 = MAP_1(x2, y2);
				}
				else {
					i1 = MAP_2(x1, y1);
					i2 = MAP_2(x2, y1);
					i3 = MAP_2(x1, y2);
					i4 = MAP_2(x2, y2);
				}
				i1_3 = i1*3; i1_9 = i1_3*3;
				i2_3 = i2*3; i2_9 = i2_3*3;
				i3_3 = i3*3; i3_9 = i3_3*3;
				i4_3 = i4*3; i4_9 = i4_3*3;
				p1 = (i1_9 + i2_3 + i3_3 + i4) >> 4;
				p2 = (i1_3 + i2_9 + i3 + i4_3) >> 4;
				p3 = (i1_3 + i2 + i3_9 + i4_3) >> 4;
				p4 = (i1 + i2_3 + i3_3 + i4_9) >> 4;

				random_center = square_size*MAP_T(x1,y1)/GeonetMESH;
				random_range = random_center << 1;

				p1 += (random(random_range) - random_center);
				p2 += (random(random_range) - random_center);
				p3 += (random(random_range) - random_center);
				p4 += (random(random_range) - random_center);

				if(p1 < Cmin_alt) p1 = Cmin_alt; else if(p1 > Cmax_alt) p1 = Cmax_alt;
				if(p2 < Cmin_alt) p2 = Cmin_alt; else if(p2 > Cmax_alt) p2 = Cmax_alt;
				if(p3 < Cmin_alt) p3 = Cmin_alt; else if(p3 > Cmax_alt) p3 = Cmax_alt;
				if(p4 < Cmin_alt) p4 = Cmin_alt; else if(p4 > Cmax_alt) p4 = Cmax_alt;

				x2 = (x1 + (square_size >> 1));
				y2 = y1 + (square_size >> 1);

				if(!which){
					MAP_2(x1, y1) = (unsigned short)p1;
					MAP_2(x2, y1) = (unsigned short)p2;
					MAP_2(x1, y2) = (unsigned short)p3;
					MAP_2(x2, y2) = (unsigned short)p4;
				}
				else {
					MAP_1(x1, y1) = (unsigned short)p1;
					MAP_1(x2, y1) = (unsigned short)p2;
					MAP_1(x1, y2) = (unsigned short)p3;
					MAP_1(x2, y2) = (unsigned short)p4;
				}
			}
		}
		which = 1 - which;
	}

	if(!which) memcpy(map2, map1, sizeMap*sizeof(int) );
	////////////////////////////////////////////////////
	//map2 выходная карта
	vMap.UndoDispatcher_PutPreChangedArea(xBeg, yBeg, xBeg+sx/*sizeX*/, yBeg+sy/*sizeY*/);

	sy05=sy/2;///(sizeY-2*cellSize)/2.0f;
	sx05=sx/2;///(sizeX-2*cellSize)/2.0f;
	unsigned char* ch = vMap.changedT;
	for(yMap=0; yMap<(sy/*sizeY-2*cellSize*/); yMap++){
		float y=(float)(yMap-sy05)/(float)sy05;
		for(xMap=0; xMap<(sx/*sizeX-2*cellSize*/); xMap++){
			float x=(float)(xMap-sx05)/(float)sx05; 
			float f;
			switch (BorderForm){
			case 0:
			case 1:
			default:
				f=1;
				break;
			case 2:
				f=exp(-(fabsf(x*x*x*x)+fabsf(y*y*y*y))/(0.4*0.4));
				break;
			case 3:
				f=exp(-(fabsf(x*x*x)+fabsf(y*y*y))/(0.4*0.4));
				break;
			case 4:
				f=exp(-(fabsf(x*x)+fabsf(y*y))/(0.4*0.4));
				break;
			}
			if(_inverse)f=-f;
			//vMap.voxSet(xBeg+xMap, yBeg+yMap, round(MAP_2(xMap, yMap)*f));
			TerrainMetod.put(vMap.XCYCL(xBeg+xMap), vMap.YCYCL(yBeg+yMap), round(MAP_2(xMap, yMap)*f));
		}
		ch[vMap.YCYCL(yBeg+yMap)] = 1;
	}
	vMap.regRender(xBeg, yBeg, xBeg+sizeX, yBeg+sizeY);


	delete [] mapT;
	delete [] map2;
	delete [] map1;

	delete [] workGrid;
	delete [] roughGrid;
}


static void gaussFilter(int * alt_buff, double filter_scaling, int x_size, int y_size)
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

	int* new_alt_buff = new int[x_size*y_size];

	int xx,yy;
	for(yy = 0;yy < (int)y_size;yy++){
		for(xx = 0;xx < (int)x_size;xx++){
			f = 0;
			for(y = -H;y < H;y++)
				for(x = -H;x < H;x++){
					f += filter_array[H + y][H + x]*double(alt_buff[((yy + y)&border_mask_y)*x_size + ((xx + x)&border_mask_x)]);
					}
			unsigned char c = round(f*norma_inv);
			new_alt_buff[((yy)*x_size) + (xx)] = round(f*norma_inv);
			}
	}
	//cout <<endl<<endl;
	memcpy(alt_buff,new_alt_buff,x_size*y_size*sizeof(int));
	delete new_alt_buff;
}
