#include "stdafxTr.h"


/**/

/*
	PointInPolygon
	Находится ли точка внутри полигона.
	Построчный алгоритм:
	  Если линия перпендикулярная оси X пересекает до точки point
	  полигон нечётное количество раз, то она находится внутри полигона.
	  Паралельные отрезки не учитываем.
	  
*/
/*template<class T>
bool PointInPolygon (POINT p,T* pg,int N)
{
    int c = 0;
    for (int i = 0, j = N-1; i < N; j = i++)
    {
        if ((pg[i].y<=p.y && p.y<pg[j].y && 
				(pg[j].y-pg[i].y)*(p.x-pg[i].x)<(pg[j].x-pg[i].x)*(p.y-pg[i].y))
        ||  (pg[j].y<=p.y && p.y<pg[i].y && 
				(pg[j].y-pg[i].y)*(p.x-pg[i].x)>(pg[j].x-pg[i].x)*(p.y-pg[i].y)))
        {
            c ^= 1;
        }
    }
    return c?true:false;
}*/






/////////////////////////////////////////////////////////////////////////////////////////////

void vrtMap::initGrid(void)
{
	int fullhZeroPlast=hZeroPlast<<VX_FRACTION;
	int i,j,kx,ky;
	for(i=0; i<(V_SIZE>>kmGrid); i++){
		for(j=0; j<(H_SIZE>>kmGrid); j++){
			int of=offsetBuf(j<<kmGrid,i<<kmGrid); //Так как сетка кратна размеру -переход через границу карты исключен
			//if( (j<<kmGrid)==596 && (i<<kmGrid)==464){ //Для брэкпоинта по координатам
			//	int test=0;
			//}
			int Sum=0;
			//unsigned char prZ=0;
			unsigned char ZP=0, LVD=0;
			bool flag_taller_h_ZL=0;
			for(ky=0; ky<sizeCellGrid; ky++){
				for(kx=0; kx<sizeCellGrid; kx++){
					if(AtrBuf[of+kx]&At_LEVELED) {
						LVD++;
						if( (AtrBuf[of+kx]&At_ZPMASK) == At_ZEROPLAST) ZP++;
					}
					unsigned short v=VxDBuf[of+kx];
//					if(v) Sum+=v;
//					else {
//						v=VxGBuf[of+kx];
//						Sum+=v;
//						if((v==hZeroPlast) && ((AtrBuf[of+kx]&VX_FRACTION_MASK)==0)) prZ++; //Если высота подготовлена для зеропласта увеличиваем счетчик
//					}
					if(!v)v=VxGBuf[of+kx];
					Sum+=v;
					//if((v==hZeroPlast) && ((AtrBuf[of+kx]&VX_FRACTION_MASK)==0)) prZ++; 

					
					v=(v<<VX_FRACTION)+(AtrBuf[of+kx]&VX_FRACTION_MASK);
					if(v > fullhZeroPlast) flag_taller_h_ZL=1;
				}
				of+=H_SIZE; //Переход на следующую строку
			}
			int ofG=offsetGBuf(j,i);
			GVBuf[ofG]=(Sum+1) >>(kmGrid+kmGrid); //Округление и деление на 16 (4x4)
			//ОБнуляем атрибуты и выставляем признак подготовленности для зеропласта
			GABuf[ofG]=GRIDAT_MASK_HARDNESS; //0;

			//if(prZ==(sizeCellGrid*sizeCellGrid)) GABuf[ofG]|=GRIDAT_LEVELED;
			if(LVD==(sizeCellGrid*sizeCellGrid)) GABuf[ofG]|=GRIDAT_LEVELED;

			if(flag_taller_h_ZL) GABuf[ofG]|=GRIDAT_TALLER_HZEROPLAST;
			//Зеропласта всегда в начале нет 
			if(ZP==(sizeCellGrid*sizeCellGrid)) GABuf[ofG]|=GRIDAT_ZEROPLAST;
			//if(ZPE==(sizeCellGrid*sizeCellGrid)) GABuf[ofG]|=GRIDAT_ZEROPLASTEMPTY;
			if(/*(ZPE) ||*/ (ZP)) GABuf[ofG]|=GRIDAT_ZEROPLAST_IS_PRESENT;
			//ДЛя ОТЛАДКИ
			//if(prZ==(sizeCellGrid*sizeCellGrid)) GABuf[ofG]|=GRIDAT_ZEROPLAST;

		}
	}

/*	//Сетка проходимости
	//очистка сетки проходимости
	int cnt=0;
	for(i=0; i<(V_SIZE>>kmGridK); i++){
		for(j=0; j<(H_SIZE>>kmGridK); j++){
			GKABuf[cnt]=0;
			cnt++;
		}
	}
	//пересчет
	int dPower=kmGridK-kmGrid;
	for(i=0; i<(V_SIZE>>kmGrid); i++){
		int h1=GVBuf[offsetGBuf(GH_SIZE-1,i)];
		int offOldK=((GH_SIZE-1)>>dPower)+(i>>dPower)*GKH_SIZE;
		for(j=0; j<(H_SIZE>>kmGrid); j++){
			int h2=GVBuf[offsetGBuf(j,i)];
			int dh=h1-h2;
			int offK=(j>>dPower)+(i>>dPower)*GKH_SIZE;
			if( (dh > KronCritical_dh) || (-dh > KronCritical_dh) ){
				GKABuf[offK]=1;
				GKABuf[offOldK]=1;
			}
			h1=h2;
			offOldK=offK;
		}
	}
	for(i=0; i<(H_SIZE>>kmGrid); i++){
		int h1=GVBuf[offsetGBuf(i,GV_SIZE-1)];
		int offOldK=(i>>dPower)+((GV_SIZE-1)>>dPower)*GKH_SIZE;
		for(j=0; j<(V_SIZE>>kmGrid); j++){
			int h2=GVBuf[offsetGBuf(i,j)];
			int dh=h1-h2;
			int offK=(j>>dPower)*GKH_SIZE+(i>>dPower);
			if( (dh > KronCritical_dh) || (-dh > KronCritical_dh) ){
				GKABuf[offK]=1;
				GKABuf[offOldK]=1;
			}
			h1=h2;
			offOldK=offK;
		}
	}
*/
}

void vrtMap::recalcArea2Grid(int xl, int yt, int xr, int yb )
{
	int fullhZeroPlast=hZeroPlast<<VX_FRACTION;
	int xlG=xl>>kmGrid;
	int ytG=yt>>kmGrid;

	//int dxG=XCYCL(xr-xl);
	//dxG=(dxG>>kmGrid)+2;
	int dxG=XCYCLG((xr>>kmGrid)-xlG)+1;
	if(dxG > H_SIZE>>kmGrid)dxG=H_SIZE>>kmGrid;
	//int dyG=YCYCL(yb-yt);
	//dyG=(dyG>>kmGrid)+2;
	int dyG=YCYCLG((yb>>kmGrid)-ytG)+1;
	if(dyG > V_SIZE>>kmGrid)dyG=V_SIZE>>kmGrid;

	int i,j,kx,ky;
	for(i=0; i<dyG; i++){
		for(j=0; j<dxG; j++){
			int of=offsetBuf( XCYCL((xlG+j)<<kmGrid), YCYCL((ytG+i)<<kmGrid) ); 
			int Sum=0;
			//unsigned char prZ=0;
			unsigned char ZP=0, LVD=0;
			char flag_taller_h_ZL=0;
			for(ky=0; ky<sizeCellGrid; ky++){
				for(kx=0; kx<sizeCellGrid; kx++){
					if(AtrBuf[of+kx]&At_LEVELED) {
						LVD++;
						if( (AtrBuf[of+kx]&At_ZPMASK) == At_ZEROPLAST) ZP++;
					}
					unsigned short v=VxDBuf[of+kx];
//					if(v) Sum+=v;
//					else {
//						v=VxGBuf[of+kx];
//						Sum+=v;
//						//if(v==hZeroPlast) prZ++; //Если высота подготовлена для зеропласта увеличиваем счетчик
//						if((v==hZeroPlast) && ((AtrBuf[of+kx]&VX_FRACTION_MASK)==0)) prZ++; //Если высота подготовлена для зеропласта увеличиваем счетчик
//					}
					if(!v)v=VxGBuf[of+kx];
					Sum+=v;
					//if((v==hZeroPlast) && ((AtrBuf[of+kx]&VX_FRACTION_MASK)==0)) prZ++; 

					v=(v<<VX_FRACTION)+(AtrBuf[of+kx]&VX_FRACTION_MASK);
					if(v > fullhZeroPlast) flag_taller_h_ZL=1;
				}
				of+=H_SIZE; //Переход на следующую строку //Зацикливание не нужно т.к. ячейка не переходит границы карты никогда
			}
			int ofG=offsetGBuf(XCYCLG(xlG+j),YCYCLG(ytG+i));
			GVBuf[ofG]=(Sum+1) >>(kmGrid+kmGrid); //Округление и деление на 16 (4x4)
			//ОБнуляем атрибуты и выставляем признак подготовленности для зеропласта
			unsigned short atg=GABuf[ofG];
			atg&= (~GRIDAT_LEVELED) & (~GRIDAT_TALLER_HZEROPLAST) & (~GRIDAT_ZEROPLAST) /*& (~GRIDAT_ZEROPLASTEMPTY)*/ & (~GRIDAT_ZEROPLAST_IS_PRESENT);

			//if(prZ==(sizeCellGrid*sizeCellGrid)) atg|=GRIDAT_LEVELED;
			if(LVD==(sizeCellGrid*sizeCellGrid)) atg|=GRIDAT_LEVELED;

			if(flag_taller_h_ZL) atg|=GRIDAT_TALLER_HZEROPLAST;
			if(ZP==(sizeCellGrid*sizeCellGrid)) atg|=GRIDAT_ZEROPLAST;
			//if(ZPE==(sizeCellGrid*sizeCellGrid)) atg|=GRIDAT_ZEROPLASTEMPTY;
			if(/*(ZPE) ||*/ (ZP)) atg|=GRIDAT_ZEROPLAST_IS_PRESENT;
			GABuf[ofG]=atg;
		}
	}

/*
//	Пересчет сетки проходимости
	int dPower=kmGridK-kmGrid;
	for(i=0; i<dyG; i++){
		int h1=GVBuf[offsetGBuf(XCYCLG(xlG-1),YCYCLG(i+ytG))];
		int offOldK=(XCYCLG(xlG-1)>>dPower)+(YCYCLG(i+ytG)>>dPower)*GKH_SIZE;
		for(j=0; j<dxG; j++){
			int h2=GVBuf[offsetGBuf(XCYCLG(j+xlG),YCYCLG(i+ytG))];
			int dh=h1-h2;
			int offK=(XCYCLG(j+xlG)>>dPower)+(YCYCLG(i+ytG)>>dPower)*GKH_SIZE;
			if( (dh > KronCritical_dh) || (-dh > KronCritical_dh) ){
				GKABuf[offK]=1;
				GKABuf[offOldK]=1;
			}
			//очистка сетки проходимости
			else GKABuf[offK]=0;
			h1=h2;
			offOldK=offK;
		}
	}
	for(i=0; i<dxG; i++){
		int h1=GVBuf[offsetGBuf(XCYCLG(i+xlG),YCYCLG(ytG-1))];
		int offOldK=(XCYCLG(i+xlG)>>dPower)+(YCYCLG(ytG-1)>>dPower)*GKH_SIZE;
		for(j=0; j<dyG; j++){
			int h2=GVBuf[offsetGBuf(XCYCLG(i+xlG),YCYCLG(j+ytG))];
			int dh=h1-h2;
			int offK=(YCYCLG(j+ytG)>>dPower)*GKH_SIZE+(XCYCLG(i+xlG)>>dPower);
			if( (dh > KronCritical_dh) || (-dh > KronCritical_dh) ){
				GKABuf[offK]=1;
				GKABuf[offOldK]=1;
			}
			h1=h2;
			offOldK=offK;
		}
	}
*/
}


void vrtMap::saveGrid(XStream & ff)
{
	int i;//,j,kx,ky;
	char flag_taller_h_zp=0;
	for(i=0; i<(GV_SIZE); i++){
		ff.write(&GVBuf[offsetGBuf(0,i)], (GH_SIZE)*sizeof(GVBuf[0]));
		ff.write(&GABuf[offsetGBuf(0,i)], (GH_SIZE)*sizeof(GABuf[0]));
	}
}

void vrtMap::loadGrid(XStream & ff)
{
	int i;//,j,kx,ky;
	char flag_taller_h_zp=0;
	for(i=0; i<(GV_SIZE); i++){
		ff.read(&GVBuf[offsetGBuf(0,i)], (GH_SIZE)*sizeof(GVBuf[0]));
		ff.read(&GABuf[offsetGBuf(0,i)], (GH_SIZE)*sizeof(GABuf[0]));
		//for(j=0; j<GH_SIZE; j++){
		//	GABuf[offsetGBuf(j,i)] &= ~(GRIDAT_BUILDING|GRIDAT_BASE_OF_BUILDING_CORRUPT);
		//}
	}
}

