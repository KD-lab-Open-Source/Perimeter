#include "stdafxTr.h"


static const int MAX_SIZE_UNDO_REDO_BUFFER=4096*4096;
int UNDO_REDO_BUFFER_SIZE=0;
void vrtMap::UndoDispatcher_PutPreChangedArea(int xL, int yT, int xR, int yB)
{
#ifdef _SURMAP_
	//preCAs.erase(preCAs.begin(), preCAs.end());
	//curPreCA=preCAs.begin();
	xL=XCYCL(xL);
	xR=XCYCL(xR);
	yT=YCYCL(yT);
	yB=YCYCL(yB);
	int sx, sy;
	if(xL==xR){ sx=H_SIZE; xL=0;}
	else sx=XCYCL(xR-xL);
	if(yT==yB){ sy=V_SIZE; yT=0;}
	else sy=XCYCL(yB-yT);
	if(sx > H_SIZE) sx=H_SIZE;
	if(sy > V_SIZE) sy=V_SIZE;
	int size=sx*sy;

	//list<sPreChangedArea>::iterator pp;
	if(curPreCA!=preCAs.end()) preCAs.erase(curPreCA, preCAs.end()); //Удаление не нужных элементов
	//Проверка на переполнение буфера и удаление 1-х элементов
	while((UNDO_REDO_BUFFER_SIZE+size) > MAX_SIZE_UNDO_REDO_BUFFER){
		if(preCAs.begin()==preCAs.end())ErrH.Abort("Список пустой а UNDO_REDO_BUFFER_SIZE==", XERR_USER, UNDO_REDO_BUFFER_SIZE);
		preCAs.erase(preCAs.begin());
	}
	preCAs.push_back();
	curPreCA=preCAs.end();
	curPreCA--;//Теперь указывет на последний элемент
	curPreCA->setSize(sx, sy);
	int i, j, cnt=0;
	for(i=0; i<sy; i++){
		for(j=0; j<sx; j++){
			//unsigned char g,d,a,s;
			int off=offsetBuf(XCYCL(xL+j), YCYCL(yT+i));
			(curPreCA->geo)[cnt]=VxGBuf[off];
			(curPreCA->dam)[cnt]=VxDBuf[off];
			(curPreCA->atr)[cnt]=AtrBuf[off];
			(curPreCA->sur)[cnt]=SurBuf[off];
			cnt++;
		}
	}
	curPreCA->x=xL;
	curPreCA->y=yT;
	curPreCA++;// теперь указывает на окончание
#endif
}

void vrtMap::UndoDispatcher_Undo(void)
{
	if( preCAs.begin() ==preCAs.end()) return; //если пустой список то возврат
	if(curPreCA==preCAs.begin()) return;//если уже нет изменений

	curPreCA--;//Теперь указывет на последний элемент
	int xL=curPreCA->x;
	int yT=curPreCA->y;
	int sx=curPreCA->sx;
	int sy=curPreCA->sy;
	int i, j, cnt=0;
	for(i=0; i<sy; i++){
		for(j=0; j<sx; j++){
			unsigned char t;
			int off=offsetBuf(XCYCL(xL+j), YCYCL(yT+i));
			t=VxGBuf[off]; VxGBuf[off]=(curPreCA->geo)[cnt]; (curPreCA->geo)[cnt]=t;
			t=VxDBuf[off]; VxDBuf[off]=(curPreCA->dam)[cnt]; (curPreCA->dam)[cnt]=t;
			t=AtrBuf[off]; AtrBuf[off]=(curPreCA->atr)[cnt]; (curPreCA->atr)[cnt]=t;
			t=SurBuf[off]; SurBuf[off]=(curPreCA->sur)[cnt]; (curPreCA->sur)[cnt]=t;
			cnt++;
		}
	}
	//curPreCA++;// теперь указывает на окончание
	regRender(xL, yT, XCYCL(xL+sx), YCYCL(yT+sy) );
}

void vrtMap::UndoDispatcher_Redo(void)
{
	if( preCAs.begin() ==preCAs.end()) return; //если пустой список то возврат
	if(curPreCA==preCAs.end()) return;//если уже нет изменений

	int xL=curPreCA->x;
	int yT=curPreCA->y;
	int sx=curPreCA->sx;
	int sy=curPreCA->sy;
	int i, j, cnt=0;
	for(i=0; i<sy; i++){
		for(j=0; j<sx; j++){
			unsigned char t;
			int off=offsetBuf(XCYCL(xL+j), YCYCL(yT+i));
			t=VxGBuf[off]; VxGBuf[off]=(curPreCA->geo)[cnt]; (curPreCA->geo)[cnt]=t;
			t=VxDBuf[off]; VxDBuf[off]=(curPreCA->dam)[cnt]; (curPreCA->dam)[cnt]=t;
			t=AtrBuf[off]; AtrBuf[off]=(curPreCA->atr)[cnt]; (curPreCA->atr)[cnt]=t;
			t=SurBuf[off]; SurBuf[off]=(curPreCA->sur)[cnt]; (curPreCA->sur)[cnt]=t;
			cnt++;
		}
	}
	regRender(xL, yT, XCYCL(xL+sx), YCYCL(yT+sy) );
	curPreCA++;
}

void vrtMap::UndoDispatcher_KillAllUndo(void)
{
	preCAs.erase(preCAs.begin(), preCAs.end());
	curPreCA=preCAs.begin();
}

bool vrtMap::UndoDispatcher_IsUndoExist(void)
{
	if(curPreCA!=preCAs.begin()) return true;
	else return false;
}

bool vrtMap::UndoDispatcher_IsRedoExist(void)
{
	if(curPreCA!=preCAs.end()) return true;
	else return false;
}
