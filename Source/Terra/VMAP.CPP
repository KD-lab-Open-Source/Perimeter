#include "stdafxTr.h"

#include <fstream>
#include <cstdio>

#include "../Util/SystemUtil.h"

#pragma warning( disable : 4554 )  

/* ----------------------------- EXTERN SECTION ---------------------------- */

extern int NOISE_AMPL;
extern int DEFAULT_TERRAIN;

/* --------------------------- DEFINITION SECTION -------------------------- */

vrtMap vMap;

char* vrtMap::worldDataFileLinear = "output.vmp";
char* vrtMap::worldIniFile        = "world.ini";
//char* vrtMap::worldParamZPIniFile = "paramzp.ini";
char* vrtMap::worldNetDataFile    = "output.vpr";
char* vrtMap::worldBuildScenarioFile = "output.vsc";
char* vrtMap::worldGeoPalFile     = "inGeo.act";
char* vrtMap::worldDamPalFile     = "inDam.act";
char* vrtMap::worldLeveledTextureFile= "leveledSurfaceTexture.tga";
char* vrtMap::worldHardnessFile= "hardness.bin";

char* vrtMap::worldDataFile= "world.cls";
char* vrtMap::worldDataFileSection= "WorldData";

char* vrtMap::worldRGBCache  = "cache.tga";


unsigned char vrtMap::GetGeoType(int offset, int h) 
{
//	For performance testing
/*
	long lTime = timeGetTime();
	for (int ii = 0; ii < 1024; ii++) {
		for (int j = 0; j < 1024; j++) {
			f3d.calc(j,ii,145);
		}
	}
	lTime = timeGetTime() - lTime;
	char intBuffer[30 + 1];
	sprintf(intBuffer, "%d", lTime);
	ErrH.Abort(string(intBuffer).c_str());
*/

	int j=offset & clip_mask_x;
	int i=offset>>H_SIZE_POWER;
	return f3d.calc(j,i,h);//>>VX_FRACTION
}


char* GetINIstringV(const char* iniFile,const char* section,const char* key)
{
	static char buffer[255+1];
	static char fullPath[_MAX_PATH];
	if(_fullpath(fullPath, iniFile, _MAX_PATH) == NULL) ErrH.Abort("Invalid path..." );
	if(!GetPrivateProfileString(section,key,"",buffer,255,fullPath)){//fullPath
		buffer[0]=0;
	}
	return buffer;
}
void SaveINIstringV(const char* iniFile,const char* section,const char* key,const char* var)
{
	//WritePrivateProfileString(section,key,var,iniFile);
	static char fullPath[_MAX_PATH];
	if(_fullpath(fullPath, iniFile, _MAX_PATH) == NULL) ErrH.Abort("Invalid path..." );
	WritePrivateProfileString(section,key,var,fullPath);//fullPath
}

const char* GetTargetName(const char* name)
{
	static char buf[MAX_PATH];

	strcpy(buf, vMap.wTable[vMap.cWorld].dir);
	if(buf[0]) strcat(buf, "\\");
	strcat(buf, name);
	return &buf[0];
}

bool isWorldIDValid(int worldID) {
	return (worldID < vMap.maxWorld && worldID >= 0);
}

const char* GetTargetName(int numWorld, const char* name)
{
	if ( !isWorldIDValid(numWorld) ) ErrH.Abort("World Index out of range");

	static char* buf = new char[256];

	strcpy(buf,vMap.wTable[numWorld].dir);
	if(*buf) strcat(buf,"\\");
	strcat(buf,name);
	return buf;
}

const char* safeGetTargetName(int numWorld, const char* name)
{
	if ( !isWorldIDValid(numWorld) ) return 0;

	static char* buf = new char[256];

	strcpy(buf,vMap.wTable[numWorld].dir);
	if(*buf) strcat(buf,"\\");
	strcat(buf,name);
	return buf;
}

vrtMap::vrtMap(void)
: fmap(0)//, kmap(0)
{
	GeonetMESH=200;
	cWorld = -1;

	wTable = 0;
	Buf = NULL;

	changedT = NULL;

	gridChAreas = NULL;
	gridChAreas2 = NULL;

	pTempArray=NULL;

	worldChanged=0;

	//ClTrBuf=0; AtBuf=0; VxBuf=0;

	VxGBuf=0; VxDBuf=0; SurBuf=0; AtrBuf=0; RnrBuf=0;
	GVBuf=0; GABuf=0;
	SupBuf=0;
	hZeroPlast=30; //Начальная высота зеропласта
//	GKABuf=0;

	LvdTex=0;

	hCamera=512, fCamera=512;

	VxBufWorkMode=GEOiDAM;//GEO;

	veryLightDam=0;

	//Поворот плоскости
	turnAngleAroundZ=0;//-3.141592654f/2;
	//Фокус
	focusC=600;
	//Расстояние от камеры
	cameraDZ=100;
	//Угол наклона камеры
	cameraAngleLean=140;

	flag_ShowHardness=0;
	flag_ShowDbgInfo=0;

	FilterMinHeight=0;
	FilterMaxHeight=MAX_VX_HEIGHT;
#ifdef _SURMAP_
	currentDamTerrain=0;
	int i;
	for(i=0; i<MAX_GEO_SURFACE_TYPE; i++) FilterGeoTerrain[i]=1;
	for(i=0; i<MAX_DAM_SURFACE_TYPE; i++) FilterDamTerrain[i]=1;

	status_ShowHideGeo=0;
	SpecSurBuf=0;
	ShadowingOnEdges=0;
	clipXLeft=clipXRight=clipYTop=clipYBottom=-1;
	//clipXLeft=1500; clipXRight=2047; clipYTop=100; clipYBottom=1000;
#endif
//Очистка списка для Undo  и установка итератора на начальный элемент
	//preCAs.erase(preCAs.begin(), preCAs.end());
	//curPreCA=preCAs.begin();
	UndoDispatcher_KillAllUndo();
	shadow_control=true;
	//PreocedureMap
	flag_record_operation=true;
	containerPMO.clear();
}

vrtMap::~vrtMap(void)
{
	free(dirWorldPrm);

	cWorld = -1;

	if(changedT) { delete changedT; changedT = NULL; }
	if(gridChAreas) { delete [] gridChAreas; gridChAreas = NULL; }
	if(gridChAreas2) { delete [] gridChAreas2; gridChAreas2 = NULL;}

	delLeveledTexture();//Необходимо вызывать до удаления VxDBuf !
	if(VxGBuf!=0) releaseMem4Buf();
	if(wTable!=0){
		delete [] wTable;
	}
	landRelease();

	worldFree();
}

void vrtMap::allocMem4Buf(void)
{
	//Buf = (unsigned char*)malloc(YS_Buf*XS_Buf*5);//+XS_Buf];
	//if(Buf == 0) {ErrH.Abort("MEMORY for VMAP is not allocation");}

	//VxBuf  = new unsigned short [YS_Buf*XS_Buf];//(unsigned short(*)[YS_Buf][XS_Buf])(Buf+offsetVx);
	//AtBuf  = new unsigned char [YS_Buf*XS_Buf];//(unsigned char(*)[YS_Buf][XS_Buf])(Buf+offsetAt);
	//ClTrBuf= new unsigned char [YS_Buf*XS_Buf];//(unsigned char(*)[YS_Buf][XS_Buf])(Buf+offsetClTr);

	VxGBuf = new unsigned char [YS_Buf*XS_Buf];
	WVxBuf = VxGBuf; //Текущие редактируемые воксели - гео-слой
	VxDBuf = new unsigned char [YS_Buf*XS_Buf];
	SurBuf = new unsigned char [YS_Buf*XS_Buf];
	AtrBuf = new unsigned char [YS_Buf*XS_Buf];
	RnrBuf = new unsigned char [YS_Buf*XS_Buf];
	SupBuf = new unsigned long [YS_Buf*XS_Buf];

#ifdef _SURMAP_
	SpecSurBuf = new unsigned char [YS_Buf*XS_Buf];
#endif
//	TgaBuf = new unsigned char [YS_Buf*XS_Buf]; //По идее только для SurMapa

	GVBuf  = new unsigned char [(YS_Buf>>kmGrid)*(XS_Buf>>kmGrid)];
	GABuf  = new unsigned short [(YS_Buf>>kmGrid)*(XS_Buf>>kmGrid)];
	//GKABuf  = new unsigned char [(YS_Buf>>kmGridK)*(XS_Buf>>kmGridK)];

}
void vrtMap::releaseMem4Buf(void)
{
//	delete [] GKABuf;
	delete [] GVBuf;
	delete [] GABuf;

//	delete [] TgaBuf;
#ifdef _SURMAP_
	delete [] SpecSurBuf;
#endif
	delete [] SupBuf;
	delete [] RnrBuf;
	delete [] AtrBuf;
	delete [] SurBuf;
	delete [] VxDBuf;
	delete [] VxGBuf;
	VxGBuf=0;
	//free(Buf);
	//delete [] ClTrBuf;
	//delete [] AtBuf;
	//delete [] VxBuf;
}


void ConvertProcedure(char* name)
{
	XStream fin(name,XS_IN);
	char* outname = strdup(name);
	memcpy(outname + strlen(name) - 3,"CNV",3);

	XStream fout(outname,XS_OUT);

	fin.seek(512,XS_BEG);

	const int xsize = vMap.H_SIZE;
	const int ysize = vMap.V_SIZE;

	unsigned char* buf = new unsigned char[xsize];

//	cout <<endl;
	int i;
	for(i = 0;i < ysize;i++){
		fin.read(buf,xsize);
		fout.write(buf,xsize);
		fin.seek(xsize,XS_CUR);
		fin.read(buf,xsize);
		fout.write(buf,xsize);
//		cout << i << " ";
		}
//	cout <<endl;

	delete buf;
	fin.close();
	fout.close();
}

// world ini special function
static const char* version_ = "1.4";
static const char* secGlobal = "Global Parameters";
static const char* secStorage = "Storage";
static const char* secCreation = "Creation Parameters";

static const char* STR_MAP_POWER_X="Map Power X";
static const char* STR_MAP_POWER_Y="Map Power Y";

int vrtMap::getWorld_H_SIZE(int idxWorld)
{
	int H_sizePower=atoi(GetINIstringV(GetTargetName(idxWorld, worldIniFile), secGlobal, STR_MAP_POWER_X));
	xassert(H_sizePower <= MAX_H_SIZE_POWER);
	int H_size=1<<H_sizePower;
	return H_size;
}

int vrtMap::getWorld_V_SIZE(int idxWorld)
{
	int V_sizePower=atoi(GetINIstringV(GetTargetName(idxWorld, worldIniFile), secGlobal, STR_MAP_POWER_Y));
	xassert(V_sizePower <= MAX_V_SIZE_POWER);
	int V_size=1<<V_sizePower;
	return V_size;
}


void vrtMap::analyzeINI(const char* name)
{

	H_SIZE_POWER = atoi(GetINIstringV(name,secGlobal,STR_MAP_POWER_X));
	V_SIZE_POWER = atoi(GetINIstringV(name,secGlobal,STR_MAP_POWER_Y));
	GEONET_POWER = atoi(GetINIstringV(name,secGlobal,"GeoNet Power"));
	WPART_POWER = atoi(GetINIstringV(name,secGlobal,"Section Size Power"));
	MINSQUARE = 1 << atoi(GetINIstringV(name,secGlobal,"Minimal Square Power"));

	hZeroPlast=atoi(GetINIstringV(name, secGlobal, "hZeroPlast"));

	XBuffer tbuf;
	tbuf.init();
	//tbuf < (GetINIstringV(name, secGlobal, "Color ZeroPlast")) ;
	//tbuf.set(0,XB_BEG);
	//tbuf>=colZeroPlast.r; tbuf>=colZeroPlast.g; tbuf>=colZeroPlast.b; tbuf>=colZeroPlast.alpha;
	colZeroPlast.r=1; colZeroPlast.g=11; colZeroPlast.b=51; colZeroPlast.alpha=70;
	IniManager im("Perimeter.ini", false);
	const char* s=im.get("TD", "Color ZeroLayer");
	if(strlen(s)){
		tbuf < s;
		tbuf.set(0,XB_BEG);
		tbuf>=colZeroPlast.r; tbuf>=colZeroPlast.g; tbuf>=colZeroPlast.b; tbuf>=colZeroPlast.alpha;
	}
	

	if(strcmp(version_,GetINIstringV(name,secStorage,"Version"))) ErrH.Abort("Incorrect Storage Version");


	GeonetMESH = atoi(GetINIstringV(name,secCreation,"Mesh Value"));
	NOISE_AMPL = atoi(GetINIstringV(name,secCreation,"Noise Amplitude"));
	DEFAULT_TERRAIN = atoi(GetINIstringV(name,secCreation,"Default Terrain Type"));// << TERRAIN_OFFSET;


	//Подгрузка параметров зеропласта из другого файла
//	XStream fTst(0);
//	if(fTst.open(GetTargetName(worldParamZPIniFile), XS_IN)){
//		XBuffer tbuf;
//		
//		tbuf < (GetINIstringV(GetTargetName(worldParamZPIniFile), secGlobal, "Color ZeroPlast")) ;
//		tbuf.set(0,XB_BEG);
//		tbuf>=colZeroPlast.r; tbuf>=colZeroPlast.g; tbuf>=colZeroPlast.b; tbuf>=colZeroPlast.alpha;
//  
//		hZeroPlast=atoi(GetINIstringV(GetTargetName(worldParamZPIniFile), secGlobal, "hZeroPlast"));
//	}
}

#ifdef _SURMAP_
//Для SurMap3
void vrtMap::prepare()
{
	//Подготовка круглых инструментов
	landPrepare();
	//Инициализация графического режима
	hCamera=500;
	fCamera=500;

	//Копирование только имени каталога в dirWorldPrm 
	dirWorldPrm = strdup("");//пустая строка

	maxWorld = 1;
	wTable = new vrtWorld[maxWorld];
	wTable[0].name = strdup("Empty world");
	wTable[0].dir = strdup("");

}
//Для SurMap3
void vrtMap::selectUsedWorld(char* _patch2World)
{
	UndoDispatcher_KillAllUndo(); //Очистка всего буфера Undo-Redo

	free(wTable[0].name);
	free(wTable[0].dir);

	wTable[0].name = strdup("SurMap_world");
	wTable[0].dir = strdup(_patch2World);

	cWorld = 0;

	worldFree();

	analyzeINI(GetTargetName(worldIniFile));
	setupGeneralVariable();

	worldPrepare();

	LoadPP();

	releaseChAreaBuf();
	allocChAreaBuf();

	int exist = 1;
	if(!fmap.open(GetTargetName(worldDataFileLinear),XS_IN)) exist = 0;
	else fmap.close();

	if(!exist) {
		if(buildWorld()) exist=1;
	}

	if(!exist) ErrH.Abort("Can't load world",XERR_USER,-1,GetTargetName(worldDataFileLinear));

	LoadVPR();
	RenderPrepare1();

	changedAreas.erase(changedAreas.begin(), changedAreas.end());
	renderAreas.clear();
}

#else //если Периметр
//Для Периметра
void vrtMap::prepare(char* name)
{
	//Подготовка круглых инструментов
	landPrepare();
	//Инициализация графического режима
	hCamera=500;
	fCamera=500;

	//Копирование только имени каталога в dirWorldPrm 
	char* dir,*dirtemp;
	char *dirend=dirtemp=dir=strdup(name);
	while(*dirtemp) {
		if (*dirtemp=='\\') dirend = dirtemp;
		dirtemp++;
	}
	*dirend = '\0';
	dirWorldPrm = strdup(dir);
	free(dir);

	// Заполнение таблицы миров
	prmFile.init(name);
	maxWorld = atoi(prmFile.getAtom());
	if(maxWorld < 1) ErrH.Abort("Empty world list");
	int i;
	char* atom;
	char tString[256];
	wTable = new vrtWorld[maxWorld];
	for(i = 0;i < maxWorld;i++){
		atom = prmFile.getAtom();
		if(!atom) ErrH.Abort("Null world name");
		wTable[i].name = strdup(atom);
		atom = prmFile.getAtom();
		if(!atom) ErrH.Abort("Null world path");

		strcpy(tString,dirWorldPrm);//копирование в строку пути до каталога с worlds.prm
		if(*dirWorldPrm) strcat(tString,"\\");		
		strcat(tString,atom);		//добавление в строку пути до каталога с миром
		wTable[i].dir = strdup(tString);
		}
	prmFile.finit();
}

//Для Периметра
void vrtMap::selectUsedWorld(int nWorld)
{
	XRndSet(1);

	UndoDispatcher_KillAllUndo(); //Очистка всего буфера Undo-Redo

// reload
	if(nWorld >= maxWorld || nWorld < 0) ErrH.Abort("World Index out of range");

	cWorld = nWorld;


	worldFree();


	analyzeINI(GetTargetName(worldIniFile));
	setupGeneralVariable();

	worldPrepare();


	LoadPP();

	releaseChAreaBuf();
	allocChAreaBuf();

	int exist = 1;
	if(!fmap.open(GetTargetName(worldDataFileLinear),XS_IN)) exist = 0;
	else fmap.close();

	if(!exist) {
		if(buildWorld()) exist=1;
	}

	if(!exist) ErrH.Abort("Can't load world",XERR_USER,-1,GetTargetName(worldDataFileLinear));

	LoadVPR();
	RenderPrepare1();

	changedAreas.clear();
	renderAreas.clear();
}

#endif

void vrtMap::setupGeneralVariable()
{
	if(H_SIZE_POWER  > MAX_H_SIZE_POWER) ErrH.Abort("X-Size > MAX");
	if(V_SIZE_POWER  > MAX_V_SIZE_POWER) ErrH.Abort("Y-Size > MAX");

	H_SIZE = 1 << H_SIZE_POWER;
	clip_mask_x = H_SIZE - 1;
	GH_SIZE = H_SIZE >> kmGrid;
	clip_mask_x_g = GH_SIZE - 1;

	V_SIZE = 1 << V_SIZE_POWER;
	clip_mask_y = V_SIZE - 1;
	GV_SIZE = V_SIZE >> kmGrid;
	clip_mask_y_g = GV_SIZE - 1;

	XS_Buf=H_SIZE; //Пока размер буфера будет равен размеру карты
	YS_Buf=V_SIZE;

	MINSQUARE = 1 << MINSQUARE_POWER;

	QUANT = 1 << GEONET_POWER;
	part_map_size_y = 1 << WPART_POWER;
	part_map_size = H_SIZE << WPART_POWER;
	PART_MAX = V_SIZE >> WPART_POWER;
	net_size = H_SIZE*V_SIZE/QUANT/QUANT;
}

void vrtMap::releaseChAreaBuf()
{
	if(changedT){ delete changedT; changedT = 0; }

	if(gridChAreas) { delete [] gridChAreas; gridChAreas=0; }
	if(gridChAreas2) { delete [] gridChAreas2; gridChAreas2=0; }
}

void vrtMap::allocChAreaBuf()
{
	changedT = new unsigned char[YS_Buf];
	memset(changedT,0,YS_Buf);

	int sizeGCA=(V_SIZE>>kmGridChA)*(H_SIZE>>kmGridChA);
	gridChAreas= new unsigned char[sizeGCA];
	gridChAreas2= new unsigned char[sizeGCA];
	clearGridChangedAreas();
}


void vrtMap::createWorld(int hSizePower, int vSizePower)
{
	H_SIZE_POWER=hSizePower;
	V_SIZE_POWER=vSizePower;
	GEONET_POWER=6;
	WPART_POWER=7;
	MINSQUARE_POWER=4;
	hZeroPlast=30;

	GeonetMESH=450;
	NOISE_AMPL=1;


	setupGeneralVariable();
	buildWorld();
}

void vrtMap::newLoad(const char* dirName)
{
	UndoDispatcher_KillAllUndo(); //Очистка всего буфера Undo-Redo

	free(wTable[0].name);
	free(wTable[0].dir);

	wTable[0].name = strdup("SurMap_world");
	wTable[0].dir = strdup(dirName);

	cWorld = 0;

	worldFree();

	////analyzeINI(GetTargetName(worldIniFile));
	if(XStream(0).open(GetTargetName(worldDataFile), XS_IN)){
		XPrmIArchive(GetTargetName(worldDataFile)) >> WRAP_NAME(*this, worldDataFileSection);
	}

	setupGeneralVariable();

	worldPrepare();

	LoadPP();

	releaseChAreaBuf();
	allocChAreaBuf();

	int exist = 1;
	if(!fmap.open(GetTargetName(worldDataFileLinear),XS_IN)) exist = 0;
	else fmap.close();

	if(!exist) {
		if(buildWorld()) exist=1;
	}

	if(!exist) ErrH.Abort("Can't load world",XERR_USER,-1,GetTargetName(worldDataFileLinear));

	LoadVPR();
	RenderPrepare1();

	changedAreas.erase(changedAreas.begin(), changedAreas.end());
	renderAreas.clear();
}

void vrtMap::newSave(const char* dirName)
{
	XPrmOArchive oa(GetTargetName(worldDataFile));
	oa << WRAP_NAME(*this, worldDataFileSection);

}

void vrtMap::delAllZL()
{
	unsigned char* ch = changedT;
	int x,y;
	for(y=0; y<V_SIZE; y++){
		for(x=0; x<H_SIZE; x++){
			int offB=offsetBuf(x,y);
			if((vMap.AtrBuf[offB]&At_ZPMASK)==At_ZEROPLAST) vMap.AtrBuf[offB]=(vMap.AtrBuf[offB]&(~At_NOTPURESURFACE))|At_LEVELED;
			//AtrBuf[offsetBuf(x,y)]&=0xDF; //Очистка тени
			//AtrBuf[offsetBuf(x,y)]&=~(At_NOTPURESURFACE); //Очистка зеропласта
			ch[y] = 1;
		}
	}
}

void vrtMap::checkAndRecover()
{
	unsigned char* ch = changedT;
	int x,y;
	for(y=0; y<V_SIZE; y++){
		for(x=0; x<H_SIZE; x++){
#ifdef _PERIMETER_
			int off=offsetBuf(x,y);
			AtrBuf[off]&=0xDF; //Очистка тени
			//if( (VxGBuf[off]!=hZeroPlast) && (VxDBuf[off]!=hZeroPlast) ){
			if(SGetAlt(off)!=hZeroPlast<<VX_FRACTION ){
				AtrBuf[off]&=~(At_NOTPURESURFACE);
				ch[y]=1;
			}
			if((AtrBuf[off]&At_ZPMASK)==At_ZEROPLAST ){
				AtrBuf[off]=(AtrBuf[off]&(~At_NOTPURESURFACE))|At_LEVELED;
				ch[y]=1;
			}

#else //Surmap
			AtrBuf[offsetBuf(x,y)]&=0xDF; //Очистка тени
//			AtrBuf[offsetBuf(x,y)]&=~(At_NOTPURESURFACE); //Очистка зеропласта
//			if( SGetAlt(offsetBuf(x,y))==85<<VX_FRACTION ){
//				AtrBuf[offsetBuf(x,y)] = (AtrBuf[offsetBuf(x,y)]&(~At_NOTPURESURFACE)) | At_LEVELED;
//				ch[y]=1;
//			}
			//Опускание мира на 30
/*			int V;
			const modifDelta=-((35<<VX_FRACTION)+20);
			if( VxDBuf[offsetBuf(x,y)]!=0 ){
				V=GetAltDam(x, y)+modifDelta;
				if(V<0) V=0; if(V>MAX_VX_HEIGHT)V=MAX_VX_HEIGHT;
				int Vg=GetAltGeo(x, y);
				if(Vg>=V) PutAltGeo(x, y, V);
				PutAltDam(x, y, V);
			}
			else {
				V=GetAltGeo(x, y)+modifDelta;
				if(V<0) V=0; if(V>MAX_VX_HEIGHT)V=MAX_VX_HEIGHT;
				PutAltGeo(x, y, V);
			}
			ch[y]=1;*/
#endif
//			AtrBuf[offsetBuf(x,y)] = (AtrBuf[offsetBuf(x,y)]&(~At_NOTPURESURFACE)) | At_LEVELED;
		}
	}

/*	for(y=0; y<1024; y++){ //
		for(x=0; x<1024; x++){
			AtrBuf[offsetBuf(x,y)]=0x0; //Очистка тени
			VxGBuf[offsetBuf(x,y)]=100;//hZeroPlast;
			VxDBuf[offsetBuf(x,y)]=0;
		}
	}*/

}

void vrtMap::loadHardness2Grid(void)
{
	TGAHEAD tgahead;
	if(!tgahead.loadHeader(GetTargetName("hardness.tga"))) return;
	if((tgahead.PixelDepth!=8) || (tgahead.ImageType!=3)) {
		ErrH.Abort("Wrong type HARDNESS.BIN for the world");//"Не поддерживаемый тип TGA (необходим монохромный не компрессованный) для HARDNESS");
		return;
	}
	if(tgahead.Width!=H_SIZE || tgahead.Height!=V_SIZE){
		ErrH.Abort("Wrong size HARDNESS.BIN for the world");// "Не правильный размер TGA для HARDNESS");
		return;
	}
	unsigned char * HDNBuf;
	HDNBuf=new unsigned char[H_SIZE*V_SIZE];
	tgahead.load2buf(HDNBuf);

	int i,j,kx,ky;
	for(i=0; i<(V_SIZE>>kmGrid); i++){
		for(j=0; j<(H_SIZE>>kmGrid); j++){
			int of=offsetBuf(j<<kmGrid,i<<kmGrid); //Так как сетка кратна размеру -переход через границу карты исключен
			int Sum=0;
			for(ky=0; ky<sizeCellGrid; ky++){
				for(kx=0; kx<sizeCellGrid; kx++){
					Sum+=HDNBuf[of+kx];
				}
				of+=H_SIZE; //Переход на следующую строку
			}
			int ofG=offsetGBuf(j,i);
			//ОБнуляем атрибуты и выставляем признак подготовленности для зеропласта
			Sum=(Sum+1) >>(kmGrid+kmGrid); //Округление и деление на 16 (4x4)
			GABuf[ofG]=((Sum>>6)&GRIDAT_MASK_HARDNESS) | (GABuf[ofG]&(~GRIDAT_MASK_HARDNESS));
			//Sum>>6 приведение от 0-255 к диапазону 0-3
		}
	}

	delete [] HDNBuf;
}

void vrtMap::loadHardness(void)
{

	XStream ff(0);
	if(ff.open(GetTargetName(worldHardnessFile), XS_IN) ){
		unsigned short width, height;
		ff.read(&width, sizeof(width));
		ff.read(&height, sizeof(height));
		const unsigned int KPack=4;
		if( (width*KPack!=GH_SIZE) || (height!=GV_SIZE) ){
			ErrH.Abort("Wrong size HARDNESS.BIN for the world");
			return;
		}

		unsigned char * HDNBuf;
		HDNBuf=new unsigned char[width*height];
		ff.read(HDNBuf, width*height*sizeof(unsigned char));

		int i,j;
		int yOffsetHBuf=0;
		for(i=0; i<height; i++){
			for(j=0; j<width; j++){
				//KPack=4!
				int offG=offsetGBuf(j*KPack, i);
				GABuf[offG]=( HDNBuf[yOffsetHBuf+j] &GRIDAT_MASK_HARDNESS) | (GABuf[offG]&(~GRIDAT_MASK_HARDNESS));
				GABuf[offG+1]=( (HDNBuf[yOffsetHBuf+j]>>2) &GRIDAT_MASK_HARDNESS) | (GABuf[offG+1]&(~GRIDAT_MASK_HARDNESS));
				GABuf[offG+2]=( (HDNBuf[yOffsetHBuf+j]>>4) &GRIDAT_MASK_HARDNESS) | (GABuf[offG+2]&(~GRIDAT_MASK_HARDNESS));
				GABuf[offG+3]=( (HDNBuf[yOffsetHBuf+j]>>6) &GRIDAT_MASK_HARDNESS) | (GABuf[offG+3]&(~GRIDAT_MASK_HARDNESS));
			}
			yOffsetHBuf+=width;
		}

		delete [] HDNBuf;
		ff.close();
	}
}

void vrtMap::saveHardness(void)
{

	XStream ff(GetTargetName(worldHardnessFile), XS_OUT);
	const unsigned int KPack=4;
	unsigned short width, height;
	width=GH_SIZE/KPack;
	height=GV_SIZE;
	ff.write(&width, sizeof(width));
	ff.write(&height, sizeof(height));

	unsigned char * HDNBuf;
	HDNBuf=new unsigned char[width*height];

	int i,j;
	int yOffsetHBuf=0;
	for(i=0; i<height; i++){
		for(j=0; j<width; j++){
			//KPack=4!
			int offG=offsetGBuf(j*KPack, i);
			unsigned char itg=0;
			itg=GABuf[offG]&GRIDAT_MASK_HARDNESS;
			itg|=(GABuf[offG+1]&GRIDAT_MASK_HARDNESS)<<2;
			itg|=(GABuf[offG+2]&GRIDAT_MASK_HARDNESS)<<4;
			itg|=(GABuf[offG+3]&GRIDAT_MASK_HARDNESS)<<6;
			HDNBuf[yOffsetHBuf+j]=itg;
		}
		yOffsetHBuf+=width;
	}
	ff.write(HDNBuf, width*height*sizeof(unsigned char));

	delete [] HDNBuf;
}


void vrtMap::clearGridChangedAreas(void)
{
	int sizeGCA=(V_SIZE>>kmGridChA)*(H_SIZE>>kmGridChA);
	//for(int i=0; i<sizeGCA; i++) gridChAreas[i]=0; //обнуление
	memset(gridChAreas, 0, sizeGCA*sizeof(*gridChAreas));
	memset(gridChAreas2, 0, sizeGCA*sizeof(*gridChAreas));
}

void vrtMap::updateGridChangedAreas2(void)
{
	int sizeGCA=(V_SIZE>>kmGridChA)*(H_SIZE>>kmGridChA);

	int vSizeGCA=(V_SIZE>>kmGridChA);
	int hSizeGCA=(H_SIZE>>kmGridChA);
	int i, j, cnt=0;
	for(i=0; i<vSizeGCA; i++){
		for(j=0; j<hSizeGCA; j++){
			if(gridChAreas[cnt]==1) gridChAreas2[cnt]|=1;
			cnt++;
		}
	}
//	memset(gridChAreas, 0, sizeGCA*sizeof(*gridChAreas));
}


void vrtMap::fullLoad(bool flag_fastLoad)
{
	sVmpHeader VmpHeader;

	delLeveledTexture();//Необходимо вызывать до удаления VxDBuf !
	if(VxGBuf!=0) releaseMem4Buf();
	allocMem4Buf();

	fmap.open(GetTargetName(worldDataFileLinear), XS_IN);

	//const char id[4]={'S','2','T','0'};
	fmap.seek(0,XS_BEG);
	fmap.read(&VmpHeader,sizeof(VmpHeader));
	if (VmpHeader.cmpID("S2T0")) { //(*(int*)VmpHeader.id == *(int*)id )
		fmap.read(&VxGBuf[0],XS_Buf*YS_Buf);
		fmap.read(&VxDBuf[0],XS_Buf*YS_Buf);
		fmap.read(&AtrBuf[0],XS_Buf*YS_Buf);
		fmap.read(&SurBuf[0],XS_Buf*YS_Buf);

		loadGeoDamPal();

		checkAndRecover();
		//Для эксперимента - убирать нужно после своих экскриментов
		//extern void pn(void);
		//pn();
		f3d.loadVariable();
#ifdef _PERIMETER_
		//if(!flag_fastLoad) f3d.recalcWorld();
#endif
#ifdef _SURMAP_
		///f3d.recalcWorld();
		///f3d.calcSpecBuf();
		keyGeoPal.loadKeyGeoPal();
#endif
		// WORLD RENDER 
		//for(i=0; i<YS_Buf; i++){
		//	changedT[i]=0;
		//	RenderStr(i);
		//}
	}
	else ErrH.Abort("VMP file is not TRUE version");

	fmap.close();

	loadLeveledTexture(); //необходимо вызывать после загрузки VxDBuf и палитры

	TGAHEAD tgahead;
	tgahead.loadHeader(GetTargetName(worldRGBCache));
	if((tgahead.PixelDepth!=24) || (tgahead.ImageType!=2)) {
		///AfxMessageBox("Не поддерживаемый тип TGA (необходим 24bit не компрессованный)");
		xassert(0&&"Не поддерживаемый тип TGA (необходим 24bit не компрессованный)");
		return;
	}
	tgahead.load2RGBL(XS_Buf, YS_Buf, SupBuf);

	initGrid();

	clearGridChangedAreas();
	//loadHardness2Grid();
	loadHardness();
	worldChanged=0;
}

bool vrtMap::loadGameMap(const char* fname, bool flag_FastLoad)
{
	XStream ff(fname, XS_IN);
	//Чтение номера мира
	int nW;
	ff.read(&nW, sizeof(cWorld));
//	if(cWorld!=nW) return 0;
	//Чтение таблицы измененных областей
	int sizeGCA=(V_SIZE>>kmGridChA)*(H_SIZE>>kmGridChA);
	ff.read(gridChAreas, sizeGCA*sizeof(unsigned char));
	//Чтение записанных измененных тайлов
	int vSizeGCA=(V_SIZE>>kmGridChA);
	int hSizeGCA=(H_SIZE>>kmGridChA);
	unsigned char buffer[sizeCellGridCA*sizeCellGridCA];
	int i, j, cnt=0;
	for(i=0; i<vSizeGCA; i++){
		for(j=0; j<hSizeGCA; j++){
			if(gridChAreas[cnt]==1){
				int k, m, cnt2=0;
				ff.read(buffer, sizeof(buffer));
				for(k=0; k<sizeCellGridCA; k++){
					int offB=offsetBuf( (j<<kmGridChA), k+(i<<kmGridChA) );
					for(m=0; m<sizeCellGridCA; m++){
						VxGBuf[offB+m]=buffer[cnt2];
						cnt2++;
					}
				}
				cnt2=0;
				ff.read(buffer, sizeof(buffer));
				for(k=0; k<sizeCellGridCA; k++){
					int offB=offsetBuf( (j<<kmGridChA), k+(i<<kmGridChA) );
					for(m=0; m<sizeCellGridCA; m++){
						VxDBuf[offB+m]=buffer[cnt2];
						cnt2++;
					}
				}
				cnt2=0;
				ff.read(buffer, sizeof(buffer));
				for(k=0; k<sizeCellGridCA; k++){
					int offB=offsetBuf( (j<<kmGridChA), k+(i<<kmGridChA) );
					for(m=0; m<sizeCellGridCA; m++){
						AtrBuf[offB+m]=buffer[cnt2];
						cnt2++;
					}
				}
				cnt2=0;
				ff.read(buffer, sizeof(buffer));
				for(k=0; k<sizeCellGridCA; k++){
					int offB=offsetBuf( (j<<kmGridChA), k+(i<<kmGridChA) );
					for(m=0; m<sizeCellGridCA; m++){
						SurBuf[offB+m]=buffer[cnt2];
						cnt2++;
					}
				}

			}
			cnt++;
		}
	}

	loadGrid(ff);
	ff.close();

	checkAndRecover();
#ifdef _PERIMETER_
	//if(!IniManager("Perimeter.ini").getInt("TD","FastLoad"))	delAllZL();
	//if(!IniManager("Perimeter.ini").getInt("TD","FastLoad"))	loadHardness2Grid();
	if(!flag_FastLoad) delAllZL();
	//if(!flag_FastLoad) loadHardness2Grid();
	if(!flag_FastLoad) loadHardness();
	if(!flag_FastLoad) f3d.recalcWorld();
#endif
	//WorldRender();
	worldChanged=0;
	return 1;
}

bool vrtMap::saveGameMap(const char* fname)
{
	XStream ff(fname, XS_OUT);
	//Запись номера мира
	ff.write(&cWorld, sizeof(cWorld));
	//Запись таблицы измененных областей
	updateGridChangedAreas2();
	int sizeGCA=(V_SIZE>>kmGridChA)*(H_SIZE>>kmGridChA);
	ff.write(gridChAreas2, sizeGCA*sizeof(unsigned char));
	//запись измененных тайлов
	int vSizeGCA=(V_SIZE>>kmGridChA);
	int hSizeGCA=(H_SIZE>>kmGridChA);
	unsigned char buffer[sizeCellGridCA*sizeCellGridCA];
	int i, j, cnt=0;
	for(i=0; i<vSizeGCA; i++){
		for(j=0; j<hSizeGCA; j++){
			if(gridChAreas2[cnt]==1){
				int k, m, cnt2=0;
				for(k=0; k<sizeCellGridCA; k++){
					int offB=offsetBuf( (j<<kmGridChA), k+(i<<kmGridChA) );
					for(m=0; m<sizeCellGridCA; m++){
						buffer[cnt2]=VxGBuf[offB+m];
						cnt2++;
					}
				}
				ff.write(buffer, sizeof(buffer));
				cnt2=0;
				for(k=0; k<sizeCellGridCA; k++){
					int offB=offsetBuf( (j<<kmGridChA), k+(i<<kmGridChA) );
					for(m=0; m<sizeCellGridCA; m++){
						buffer[cnt2]=VxDBuf[offB+m];
						cnt2++;
					}
				}
				ff.write(buffer, sizeof(buffer));
				cnt2=0;
				for(k=0; k<sizeCellGridCA; k++){
					int offB=offsetBuf( (j<<kmGridChA), k+(i<<kmGridChA) );
					for(m=0; m<sizeCellGridCA; m++){
						buffer[cnt2]=AtrBuf[offB+m];
						cnt2++;
					}
				}
				ff.write(buffer, sizeof(buffer));
				cnt2=0;
				for(k=0; k<sizeCellGridCA; k++){
					int offB=offsetBuf( (j<<kmGridChA), k+(i<<kmGridChA) );
					for(m=0; m<sizeCellGridCA; m++){
						buffer[cnt2]=SurBuf[offB+m];
						cnt2++;
					}
				}
				ff.write(buffer, sizeof(buffer));

			}
			cnt++;
		}
	}
	saveGrid(ff);

	worldChanged=0;
	return !ff.ioError();
}

typedef unsigned int typeAmountCellChAreas;
typedef unsigned short typeCoordinatChAreas;
const int MAX_COORDINAT_CHAREAS=USHRT_MAX;
void vrtMap::generateChAreasInformation(XBuffer& out)
{
	unsigned int save_begin_position=out.tell();
	typeAmountCellChAreas amountCellChAreas=0;
	out.write(&amountCellChAreas, sizeof(amountCellChAreas));

	int vSizeGChA=(V_SIZE>>kmGridChA);
	int hSizeGChA=(H_SIZE>>kmGridChA);


	///unsigned char buffer[sizeCellGridCA*sizeCellGridCA];
	typeCoordinatChAreas i, j;
	unsigned int cnt=0;
	for(i=0; i<vSizeGChA; i++){
		for(j=0; j<hSizeGChA; j++){
			if(gridChAreas[cnt]==1){
				gridChAreas2[cnt]|=1;
				amountCellChAreas++;
				out.write(&j, sizeof(j));//x
				out.write(&i, sizeof(i));//y
				int k;
				unsigned int crc=startCRC32;
				unsigned char bufatr[sizeCellGridCA];
				for(k=0; k<sizeCellGridCA; k++){
					int offB=offsetBuf( (j<<kmGridChA), k+(i<<kmGridChA) );
					crc=crc32(&VxGBuf[offB], sizeCellGridCA, crc);
					crc=crc32(&VxDBuf[offB], sizeCellGridCA, crc);
					for(unsigned int mm=0; mm<sizeCellGridCA; mm++){
						bufatr[mm]=AtrBuf[offB+mm]&(~At_SHADOW);
					}
					crc=crc32(&bufatr[0], sizeCellGridCA, crc);
					crc=crc32(&SurBuf[offB], sizeCellGridCA, crc);
				}

				//for(k=0; k<(sizeCellGridCA/sizeCellGrid); k++){
				//	int offBG=offsetGBuf( (j<<(kmGridChA-kmGrid)), k+(i<<(kmGridChA-kmGrid)) );
				//	crc=crc32((unsigned char*)(&GABuf[offBG]), sizeof(unsigned short)*sizeCellGridCA/sizeCellGrid, crc);
				//	crc=crc32(&GVBuf[offBG], sizeCellGridCA/sizeCellGrid, crc);
				//}

				crc=~crc;
				out.write(&crc, sizeof(crc));//CRC
			}
			cnt++;
		}
	}
	*((typeAmountCellChAreas*)(out.buf+save_begin_position))=amountCellChAreas;
	int sizeGCA=(V_SIZE>>kmGridChA)*(H_SIZE>>kmGridChA);
	memset(gridChAreas, 0, sizeGCA*sizeof(*gridChAreas));
}

unsigned int vrtMap::getChAreasInformationCRC() 
{ 
	XBuffer buf(256, 1);
	generateChAreasInformation(buf);
	return crc32((unsigned char*)buf.address(), buf.tell(), startCRC32); 
}


void vrtMap::compareChAreasInformation(unsigned char* pFirstCAI, unsigned char* pSecondCAI, XBuffer& textOut, XBuffer& binOut)
{
	typeAmountCellChAreas sizeFirst= *((typeAmountCellChAreas*)pFirstCAI);
	pFirstCAI+=sizeof(sizeFirst);
	typeAmountCellChAreas sizeSecond= *((typeAmountCellChAreas*)pSecondCAI);
	pSecondCAI+=sizeof(sizeSecond);

	typeCoordinatChAreas curFtX, curFtY, curSdX, curSdY;
	unsigned int fcrc, scrc;

	bool flag_get_first=1;
	bool flag_get_second=1;
	while(sizeFirst || sizeSecond){
		if(flag_get_first){
			if(sizeFirst){
				curFtX=*((typeCoordinatChAreas*)pFirstCAI);
				pFirstCAI+=sizeof(typeCoordinatChAreas);
				curFtY=*((typeCoordinatChAreas*)pFirstCAI);
				pFirstCAI+=sizeof(typeCoordinatChAreas);
				fcrc=*((unsigned int*)pFirstCAI);
				pFirstCAI+=sizeof(unsigned int);
				sizeFirst--;
			}
			else {
				curFtX=MAX_COORDINAT_CHAREAS;
				curFtY=MAX_COORDINAT_CHAREAS;
			}
			flag_get_first=0;
		}
		if(flag_get_second){
			if(sizeSecond){
				curSdX=*((typeCoordinatChAreas*)pSecondCAI);
				pSecondCAI+=sizeof(typeCoordinatChAreas);
				curSdY=*((typeCoordinatChAreas*)pSecondCAI);
				pSecondCAI+=sizeof(typeCoordinatChAreas);
				scrc=*((unsigned int*)pSecondCAI);
				pSecondCAI+=sizeof(unsigned int);
				sizeSecond--;
			}
			else {
				curSdX=MAX_COORDINAT_CHAREAS;
				curSdY=MAX_COORDINAT_CHAREAS;
			}
			flag_get_second=0;
		}
		int x=-1, y=-1;
		bool flag_crc_notequal=0;
		if(curFtY==curSdY){
			if(curFtX==curSdX){
				if(fcrc!=scrc){
					x=curFtX; y=curFtY;
					flag_crc_notequal=1;
				}
				flag_get_first=1;
				flag_get_second=1;
			}
			else if(curFtX<curSdX){
				x=curFtX; y=curFtY;
				// load next first
				flag_get_first=1;
			}
			else {//curFtX>curSdX
				x=curSdX; y=curSdY;
				// load next second
				flag_get_second=1;
			}
		}
		else if(curFtY<curSdY){
			x=curFtX; y=curFtY;
			// load next first
			flag_get_first=1;
		}
		else {//curFtY>curSdY
			x=curSdX; y=curSdY;
			// load next second
			flag_get_second=1;
		}

		if(x!=-1) {
			if(flag_crc_notequal){
				textOut < "Не совпадает CRC в квадратах ";
			}
			else {
				textOut < "Не совпадают квадраты ";
			}
			textOut < "X=" <= (x<<kmGridChA) <"+" <=(sizeCellGridCA)  < " Y=" <= (y<<kmGridChA) <"+" <=(sizeCellGridCA) < "\r\n";
			binOut < x < y;
		}
	}
}

#ifdef _PERIMETER_
extern void UpdateRegionMap(int x1,int y1,int x2,int y2);

void vrtMap::displayChAreas(unsigned char* pd, unsigned int dsize)
{
	int size=dsize/sizeof(int);
	if( (size&0x01) || (dsize%sizeof(int)) ){
		xassert(0 && "Не корректный размер несовпадающей зоны");
		return;
	}
	size/=2;

	int sizeGCA=(V_SIZE>>kmGridChA)*(H_SIZE>>kmGridChA);
	pTempArray=new bool[sizeGCA];
	memset(pTempArray, 0, sizeGCA);

	int vSizeGChA=(V_SIZE>>kmGridChA);
	int hSizeGChA=(H_SIZE>>kmGridChA);
	int x,y;
	for(; size>0; size--){
		x = *((int*)pd);
		pd+=sizeof(int);
		y = *((int*)pd);
		pd+=sizeof(int);
		pTempArray[x+y*hSizeGChA]=1;
	}
	toShowHardness(true);
	UpdateRegionMap(0, 0, H_SIZE, V_SIZE);

}
#endif

unsigned int vrtMap::getWorldCRC(void)
{
	unsigned int y;
	unsigned int offB=0;
	unsigned int crc=startCRC32;
	unsigned char * pBufTMP=new unsigned char [H_SIZE];
	for(y=0; y<V_SIZE; y++){
		crc=crc32(&VxGBuf[offB], H_SIZE, crc);
		crc=crc32(&VxDBuf[offB], H_SIZE, crc);
		for(unsigned int x=0; x<H_SIZE; x++){
			pBufTMP[x]=AtrBuf[offB+x]&(~At_SHADOW);
		}
		crc=crc32(pBufTMP, H_SIZE, crc);
		crc=crc32(&SurBuf[offB], H_SIZE, crc);
		offB+=H_SIZE;
	}
	delete [] pBufTMP;
	crc=getGridCRC(true, 0, crc);
	crc=~crc;
	return crc;
}

unsigned int vrtMap::getGridCRC(bool fullGrid, int cnt, unsigned int beginCRC)
{
	unsigned int begAdrScan, sizeScan;
	const int sizeGrid=GV_SIZE*GH_SIZE;
	if(!fullGrid){
		const int PARTS=8;
		const int sizePart=sizeGrid/PARTS;
		int curPart=cnt%PARTS;
		begAdrScan=curPart*sizePart;
		sizeScan=sizePart;
	}
	else {
		begAdrScan=0;
		sizeScan=sizeGrid;
	}
	unsigned int crc=beginCRC;
	crc=crc32(&GVBuf[begAdrScan], sizeScan*sizeof(GVBuf[0]), crc);
	crc=crc32((unsigned char*)(&GABuf[begAdrScan]), sizeScan*sizeof(GABuf[0]), crc);
	crc=~crc;
	return crc;
}

void vrtMap::convertPal2TableSurCol(unsigned char palBuf[SIZE_GEO_PALETTE],eSurfaceMode SurfaceMode)
{
	int SurBufOffset=0;
	switch (SurfaceMode){
	case Geologic:
		SurBufOffset=0;
		break;
	case Damming:
		SurBufOffset=128;
		break;
	}
	double Lum=1.5;//1.5;
	//double Lum0=0.2;
	double Lum0=0.05;
	double dLum=(Lum-Lum0)/(double)MAX_SURFACE_LIGHTING;
	unsigned char R,G,B;
	double rZP, gZP, bZP;//, rZPE, gZPE, bZPE;
	rZP=colZeroPlast.r*(255-colZeroPlast.alpha)/256.0;
	gZP=colZeroPlast.g*(255-colZeroPlast.alpha)/256.0;
	bZP=colZeroPlast.b*(255-colZeroPlast.alpha)/256.0;
//	rZPE=colZeroPlastEmpty.r*(255-colZeroPlastEmpty.alpha)/256.0;
//	gZPE=colZeroPlastEmpty.g*(255-colZeroPlastEmpty.alpha)/256.0;
//	bZPE=colZeroPlastEmpty.b*(255-colZeroPlastEmpty.alpha)/256.0;
	float max_lighting=0;
	for(int i=0; i<MAX_SURFACE_TYPE; i++){ //MAX_SURFACE_TYPE =256
		R =palBuf[i*3 + 0];
		G =palBuf[i*3 + 1];
		B =palBuf[i*3 + 2];
		if(SurfaceMode==Damming){
			float lighting=0.3*(float)R + 0.59*(float)G + 0.11*(float)B;
			if(max_lighting <= lighting){
				max_lighting=lighting;
				veryLightDam=i;
			}
		}
		for(int j=0; j<MAX_SURFACE_LIGHTING; j++){ //MAX_SURFACE_LIGHTING =127
			unsigned short col;
			unsigned int col32;
			double cb,cg,cr;
			//cb=B*Lum*j/MAX_SURFACE_LIGHTING; if(cb<0)cb=0; if(cb>255)cb=255;
			//cg=G*Lum*j/MAX_SURFACE_LIGHTING; if(cg<0)cg=0; if(cg>255)cg=255;
			//cr=R*Lum*j/MAX_SURFACE_LIGHTING; if(cr<0)cr=0; if(cr>255)cr=255;
			cb=B*(Lum0+dLum*(double)j); if(cb<0)cb=0; if(cb>255)cb=255;
			cg=G*(Lum0+dLum*(double)j); if(cg<0)cg=0; if(cg>255)cg=255;
			cr=R*(Lum0+dLum*(double)j); if(cr<0)cr=0; if(cr>255)cr=255;
			col =((int)round(cb)  >>3)&0x1F;
			col+=((int)round(cg)  <<3)&0x7E0;
			col+=((int)round(cr)  <<8)&0x0F800;
			Sur2Col[i][j+SurBufOffset]=col;
			col32= (int)round(cb); col32+= (int)round(cg)<<8; col32+= (int)round(cr)<<16;
			Sur2Col32[i][j+SurBufOffset]=col32;
			//ВРЕМЕННО !!!! (для совместимости с отсутствием текстуры выровненной поверхности)
			Tex2Col[j+SurBufOffset][i]=col;
			Tex2Col32[j+SurBufOffset][i]=col32;

			col =((int)round(cb*colZeroPlast.alpha/256.0 + bZP)  >>3)&0x1F;
			col+=((int)round(cg*colZeroPlast.alpha/256.0 + gZP)  <<3)&0x7E0;
			col+=((int)round(cr*colZeroPlast.alpha/256.0 + rZP)  <<8)&0x0F800;
			SurZP2Col[j+SurBufOffset][i]=col;
			col32= (int)round(cb*colZeroPlast.alpha/256.0 + bZP);
			col32+= (int)round(cg*colZeroPlast.alpha/256.0 + gZP)<<8;
			col32+= (int)round(cr*colZeroPlast.alpha/256.0 + rZP)<<16;
			SurZP2Col32[j+SurBufOffset][i]=col32;
			//ВРЕМЕННО !!!! (для совместимости с отсутствием текстуры выровненной поверхности)
			TexZP2Col[j+SurBufOffset][i]=col;
			TexZP2Col32[j+SurBufOffset][i]=col32;

//			col =(round(cb*colZeroPlastEmpty.alpha/256.0 + bZPE)  >>3)&0x1F;
//			col+=(round(cg*colZeroPlastEmpty.alpha/256.0 + gZPE)  <<3)&0x7E0;
//			col+=(round(cr*colZeroPlastEmpty.alpha/256.0 + rZPE)  <<8)&0x0F800;
//			SurZPE2Col[j+SurBufOffset][i]=col;
//			col32=round(cb*colZeroPlast.alpha/256.0 + bZPE);
//			col32+=round(cg*colZeroPlast.alpha/256.0 + gZPE)<<8;
//			col32+=round(cr*colZeroPlast.alpha/256.0 + rZPE)<<16;
//			SurZPE2Col32[j+SurBufOffset][i]=col32;
		}
	}
}

void vrtMap::loadGeoDamPal(void)
{
/*
	for (int i = 0; i < SIZE_GEO_PALETTE; i++) {
		GeoPal[i] = 255;
	}
*/
#ifdef _TX3D_LIBRARY_
	tx3d::Vector3D pal[256];
	ifstream ifsLUT(GetTargetName("geoPal.xml"), ios::in); // | ios::nocreate);
	if (ifsLUT) {
		char szBuffer[258 * 60];
		ifsLUT.get(szBuffer, 258 * 60, '\0');
		tx3d::IndexedTexture3D::extractColorTableFromXML(string(szBuffer), pal);
	}
	for (int i = 0; i < 256; i++) {
		GeoPal[i*3 + 0] = pal[i].x * 255.0;
		GeoPal[i*3 + 1] = pal[i].y * 255.0;
		GeoPal[i*3 + 2] = pal[i].z * 255.0;
	}
#else
	//unsigned char buf[SIZE_GEO_PALETTE];
	XStream ff(GetTargetName(worldGeoPalFile), XS_IN);
	//ff.read(buf,sizeof(buf));
	ff.read(GeoPal, SIZE_GEO_PALETTE);
	ff.close();
#endif

	convertPal2TableSurCol(GeoPal,Geologic);
	//
	XStream ff1(GetTargetName(worldDamPalFile), XS_IN);
	ff1.read(DamPal, SIZE_GEO_PALETTE);
	ff1.close();

	convertPal2TableSurCol(DamPal, Damming);
}

void vrtMap::saveGeoDamPal(void)//unsigned char palBuf[SIZE_GEO_PALETTE] //Bufer в формате BGR
{
	XStream ff(GetTargetName(worldGeoPalFile), XS_OUT);
	ff.write(GeoPal, SIZE_GEO_PALETTE);//palBuf
	ff.close();
	XStream ff1(GetTargetName(worldDamPalFile), XS_OUT);
	ff1.write(DamPal, SIZE_GEO_PALETTE);//palBuf
	ff1.close();
}

void vrtMap::convertPal2TableTexCol()
{
	double Lum=1.5;//1.5;
	//double Lum0=0.2;
	double Lum0=0.05;
	double dLum=(Lum-Lum0)/(double)MAX_SURFACE_LIGHTING;
	unsigned char R,G,B;
	double rZP, gZP, bZP;//, rZPE, gZPE, bZPE;
	rZP=colZeroPlast.r*(255-colZeroPlast.alpha)/256.0;
	gZP=colZeroPlast.g*(255-colZeroPlast.alpha)/256.0;
	bZP=colZeroPlast.b*(255-colZeroPlast.alpha)/256.0;
	for(int i=0; i<MAX_SURFACE_TYPE; i++){ //MAX_SURFACE_TYPE =256
		R =TexPal[i*3 + 0];
		G =TexPal[i*3 + 1];
		B =TexPal[i*3 + 2];
		for(int j=0; j<MAX_SURFACE_LIGHTING; j++){ //MAX_SURFACE_LIGHTING =127
			unsigned short col;
			unsigned int col32;
			double cb,cg,cr;
			cb=B*(Lum0+dLum*(double)j); if(cb<0)cb=0; if(cb>255)cb=255;
			cg=G*(Lum0+dLum*(double)j); if(cg<0)cg=0; if(cg>255)cg=255;
			cr=R*(Lum0+dLum*(double)j); if(cr<0)cr=0; if(cr>255)cr=255;
			col =((int)round(cb)  >>3)&0x1F;
			col+=((int)round(cg)  <<3)&0x7E0;
			col+=((int)round(cr)  <<8)&0x0F800;
			Tex2Col[j][i]=Tex2Col[j+128][i]=col;
			col32= (int)round(cb); col32+= (int)round(cg)<<8; col32+= (int)round(cr)<<16;
			Tex2Col32[j][i]=Tex2Col32[j+128][i]=col32;

			col =((int)round(cb*colZeroPlast.alpha/256.0 + bZP)  >>3)&0x1F;
			col+=((int)round(cg*colZeroPlast.alpha/256.0 + gZP)  <<3)&0x7E0;
			col+=((int)round(cr*colZeroPlast.alpha/256.0 + rZP)  <<8)&0x0F800;
			TexZP2Col[j][i]=TexZP2Col[j+128][i]=col;
			col32= (int)round(cb*colZeroPlast.alpha/256.0 + bZP);
			col32+= (int)round(cg*colZeroPlast.alpha/256.0 + gZP)<<8;
			col32+= (int)round(cr*colZeroPlast.alpha/256.0 + rZP)<<16;
			TexZP2Col32[j][i]=TexZP2Col32[j+128][i]=col32;

		}
	}
}

void vrtMap::save3BufOnly(void)
{
	sVmpHeader VmpHeader;
	fmap.open(GetTargetName(worldDataFileLinear), XS_OUT);
	//const char id[4]={'S','2','T','0'};
	//int i;
	fmap.seek(0,XS_BEG);
	//*(int*)VmpHeader.id = *(int*)id;
	VmpHeader.setID("S2T0");
	VmpHeader.XS=H_SIZE;
	VmpHeader.YS=V_SIZE;
	fmap.write(&VmpHeader,sizeof(VmpHeader));
	fmap.write(&VxGBuf[0],XS_Buf*YS_Buf);
	fmap.write(&VxDBuf[0],XS_Buf*YS_Buf);
	fmap.write(&AtrBuf[0],XS_Buf*YS_Buf);
	fmap.write(&SurBuf[0],XS_Buf*YS_Buf);
	fmap.close();

	for(int i=0;i<YS_Buf;i++){
		changedT[i]=0;
	}
}

void vrtMap::save3Buf(void)
{

	save3BufOnly();

//	char buf[16];
//	saveM3DAll(GetTargetName(worldM3DFile));
//	saveCLSAll(GetTargetName(worldCLSFile));
//	Ch_points.save(GetTargetName(Ch_pointsFileName));

//	PList.save(GetTargetName("points.ins"));
//	fxWaveSave(GetTargetName("fx.dat"));

	saveGeoDamPal();
// Сохранение ключей палитры и функции гео слоя
	f3d.saveVariable();
#ifdef _SURMAP_
	keyGeoPal.saveKeyGeoPal();
#endif

}

#ifdef _SURMAP_
bool vrtMap::saveAllWorldAs(const char* _patch2world)
{
	XBuffer strIn, strOut;
	strIn < wTable[0].dir < "\\" < worldIniFile;
	strOut < _patch2world < "\\" < worldIniFile;
	if(!CopyFile(strIn, strOut, FALSE)){
		return 0;
	}
	free(wTable[0].name);
	free(wTable[0].dir);
	wTable[0].name = strdup("SurMap_world");
	wTable[0].dir = strdup(_patch2world);
	cWorld = 0;

	save3Buf();
	return 1;
}
#endif


void vrtMap::restore3Buf(void)
{
	sVmpHeader VmpHeader;

	UndoDispatcher_KillAllUndo(); //Очистка всего буфера Undo-Redo
	loadGeoDamPal();

	fmap.open(GetTargetName(worldDataFileLinear),XS_IN | XS_OUT);
	//const char id[4]={'S','2','T','0'};
	fmap.seek(0,XS_BEG);
	fmap.read(&VmpHeader,sizeof(VmpHeader));
	if (VmpHeader.cmpID("S2T0")){ //(*(int*)VmpHeader.id == *(int*)id )
		int i;
		for(i=0;i<YS_Buf;i++){
			if(changedT[i]){
				fmap.seek(sizeof(VmpHeader)+i*XS_Buf,XS_BEG);
				fmap.read(&VxGBuf[i*XS_Buf],XS_Buf);
			}
		}
		for(i=0;i<YS_Buf;i++){
			if(changedT[i]){
				fmap.seek(sizeof(VmpHeader)+i*XS_Buf+ XS_Buf*YS_Buf,XS_BEG);
				fmap.read(&VxDBuf[i*XS_Buf],XS_Buf);
			}
		}
		for(i=0;i<YS_Buf;i++){
			if(changedT[i]){
				fmap.seek(sizeof(VmpHeader)+i*XS_Buf+ XS_Buf*YS_Buf*2,XS_BEG);
				fmap.read(&AtrBuf[i*XS_Buf],XS_Buf);
			}
		}
		for(i=0;i<YS_Buf;i++){
			if(changedT[i]){
				if(i>0)changedT[i-1]=1; //Для правильного рендера т.к. при рендере используются 2 строки
				fmap.seek(sizeof(VmpHeader)+i*XS_Buf+ XS_Buf*YS_Buf*3,XS_BEG);
				fmap.read(&SurBuf[i*XS_Buf],XS_Buf);
			}
		}
//		for(i=0;i<YS_Buf;i++){
//			if(changedT[i]){
//				fmap.seek(sizeof(VmpHeader)+i*XS_Buf+XS_Buf*YS_Buf*2,XS_BEG);
//				fmap.read(&SpecBuf[0][i][0],XS_Buf);
//			}
//		}
		// WORLD RENDER 
		for(i=0;i<YS_Buf;i++){
			if(changedT[i]){
				changedT[i]=0;
#ifdef _SURMAP_
				f3d.calcSpecBufStr(i);
#endif
				RenderStr(i);
			}
		}
	}
	else ErrH.Abort("Invalid VMP file ");
	fmap.close();

	loadGeoDamPal();

// Восстановление ключей палитры и функции гео слоя
	f3d.loadVariable();
	//f3d.recalcWorld();
#ifdef _SURMAP_
	//f3d.calcSpecBuf();
	keyGeoPal.loadKeyGeoPal();
#endif

//	delM3DAll();
//	delCLSAll();
//	loadM3DAll(GetTargetName(worldM3DFile));
//	loadCLSAll(GetTargetName(worldCLSFile));
//	RenderShadovM3DAll();
//	Ch_points.load(GetTargetName(Ch_pointsFileName));

}

void vrtMap::scaling16(int cx,int cy,int xc,int yc,int xside,int yside, unsigned short* GRAF_BUF, int GB_MAXX, int GB_MAXY)
{
	int XSrcSize=hCamera*(xside<<1)/fCamera; //(xside<<1)- это xsize
	unsigned short* vp = GRAF_BUF + (yc - yside)*GB_MAXX + (xc - xside);
	int xsize = 2*xside;
	int ysize = 2*yside;
	int XADD = GB_MAXX - xsize;

	cx = XCYCL(cx);
	cy = YCYCL(cy);

	int YSrcSize = ysize*XSrcSize/xsize;

	int k_xscr_x = (XSrcSize << 16)/xsize;
	int k_yscr_y = (YSrcSize << 16)/ysize;

	int tfx = (cx << 16) - (XSrcSize << 15) + (1 << 15);
	int x0 = tfx >> 16;
	int x1 = x0 + XSrcSize;
	int tfy = (cy << 16) - (YSrcSize << 15) + (1 << 15);
	int y0 = tfy >> 16;
	int y1 = y0 + YSrcSize;

//	request(MIN(y0,y1) - MAX_RADIUS/2,MAX(y0,y1) + MAX_RADIUS/2,MIN(x0,x1) - 4,MAX(x0,x1) + 4);

	int i,j,fx,fy;
	unsigned char* lc = &(RnrBuf[0]);//
	unsigned char* ls = &(SurBuf[0]);

	unsigned char* lvd  = &(VxDBuf[0]);
	unsigned char* lvg = &(VxGBuf[0]);
	unsigned char* la = &(AtrBuf[0]);

	unsigned char* dataRnr;
	unsigned char* dataSur;
	unsigned char* dataVxD;
	unsigned char* dataVxG;
	unsigned char* dataAtr;
	unsigned char* dataTex;

#ifdef _SURMAP_
	unsigned char* lss = &(SpecSurBuf[0]);//
	unsigned char* dataSpecSur;
#endif
//	unsigned char* data3;


	for(i = 0;i < ysize;i++){
				fx = tfx;
				fy = tfy;
				dataRnr = lc + YCYCL(fy >> 16)*XS_Buf;
				dataSur= ls + YCYCL(fy >> 16)*XS_Buf;
				dataVxD= lvd + YCYCL(fy >> 16)*XS_Buf;
				dataVxG= lvg + YCYCL(fy >> 16)*XS_Buf;
				dataAtr= la + YCYCL(fy >> 16)*XS_Buf;
				dataTex=&(LvdTex[0]) + (((fy >> 16)&LvdTex_clip_mask_y)<<LvdTex_X_SIZE_POWER);
#ifdef _SURMAP_
				dataSpecSur=lss + YCYCL(fy >> 16)*XS_Buf;
#endif
/*#ifdef MMX
				if((k_xscr_x != 1<<16) ||(xt_mmxUse==0)){
#endif*/
				for(j = 0; j < xsize; j++, vp++){ //
					//if(*(dataAtr + XCYCL(fx >> 16)) &At_ZEROPLAST){
					//	unsigned short col=Sur2Col[*(dataSur + XCYCL(fx >> 16))][*(dataRnr + XCYCL(fx >> 16))];
					//	unsigned short g=((col&0x07E0)>>5)+32; if(g>0x3F) g=0x3F;
					//	*vp = (col&0xF81F)+(g<<5);
					//}
					//else {
					if(*(dataAtr + XCYCL(fx >> 16)) &At_LEVELED){
						if( (*(dataAtr + XCYCL(fx >> 16))&At_ZPMASK)==At_ZEROPLAST)
							*vp = SurZP2Col[*(dataRnr + XCYCL(fx >> 16))][*(dataSur + XCYCL(fx >> 16))];
						else
							*vp = Tex2Col[*(dataRnr + XCYCL(fx >> 16))][*(dataTex + ((fx >> 16)&LvdTex_clip_mask_x) )];
					}
					else{
						*vp = Sur2Col[*(dataSur + XCYCL(fx >> 16))][*(dataRnr + XCYCL(fx >> 16))];
					}
#ifdef _SURMAP_
/*#ifdef _SURMAP_
//	unsigned char* pd=&(VxDBuf[0]);
//	pa = &(AtrBuf[0]);
//	pc = &(SpcRnrBuf[0]);
	int prevVG=VxGBuf[offsetBuf(XL+dx, Y)];
	int VG;
	for (j=dx-1; j>0; j--){
		int off=offsetBuf(XL+j,Y);
		VG=VxGBuf[off];
		if(VxDBuf[off]!=0){
			SpecRnrBuf[off]= SLightTable[SLT_05SIZE+VG-prevVG];
		}
	}
#endif*/
					const unsigned short MASK_R=0xF800;
					const unsigned short MASK_G=0x07E0;
					const unsigned short MASK_B=0x001F;
					const int ALPHA=10;//из 16
					const int ALPHAI=15-ALPHA;
					if(status_ShowHideGeo==1){ //показ скрытого гео + дам
						if(*(dataVxD + XCYCL(fx >> 16))!=0){
							short rnr=SLightTable[SLT_05SIZE+*(dataVxG+XCYCL(fx >> 16)) - *(dataVxG+XCYCL(1+(fx >> 16)))]>>1;
							unsigned int t=Sur2Col[*(dataSpecSur + XCYCL(fx >> 16))][rnr];
							unsigned int told=*vp;
							t= ((((t&MASK_R)*ALPHA + (told&MASK_R)*ALPHAI)>>4)&MASK_R) | ((((t&MASK_G)*ALPHA + (told&MASK_G)*ALPHAI)>>4)&MASK_G) | ((((t&MASK_B)*ALPHA + (told&MASK_B)*ALPHAI)>>4));
							*vp=t;
						}
					}
					else if(status_ShowHideGeo==2){ //показ только скрытого гео
						if(*(dataVxD + XCYCL(fx >> 16))!=0){
							short rnr=SLightTable[SLT_05SIZE+*(dataVxG+XCYCL(fx >> 16)) - *(dataVxG+XCYCL(1+(fx >> 16)))]>>1;
							*vp=Sur2Col[*(dataSpecSur + XCYCL(fx >> 16))][rnr];
						}
					}
#endif
					//}
					//Z-буффер
					//*vz = hCamera-(*(dataVxG + XCYCL(fx >> 16)));
					fx += k_xscr_x;
					//Только для показа зеропласта 
					//if( GRIDAT_ZEROPLAST & GABuf[offsetGBuf( XCYCL(fx>>16)>>kmGrid, YCYCL(fy >> 16) >>kmGrid)] )*vp|=31;
	//				if( (GRIDAT_MASK_CLUSTERID & GABuf[offsetGBuf( XCYCL(fx>>16)>>kmGrid, YCYCL(fy >> 16) >>kmGrid)]) !=0)*vp|=63<<5;//==0x0100
					//if( (GRIDAT_MASK_CLUSTERID & GABuf[offsetGBuf( XCYCL(fx>>16)>>kmGrid, YCYCL(fy >> 16) >>kmGrid)]) ==0x0100)*vp|=31<<11;
////!					
//					if( (GRIDAT_MASK_CLUSTERID & GABuf[offsetGBuf( XCYCL(fx>>16)>>kmGrid, YCYCL(fy >> 16) >>kmGrid)]) ==0x0200)*vp|=31;
//					if( GRIDAT_LEVELED & GABuf[offsetGBuf( XCYCL(fx>>16)>>kmGrid, YCYCL(fy >> 16) >>kmGrid)] )*vp|=63<<5;
					//if( GRIDAT_TALLER_HZEROPLAST  & GABuf[offsetGBuf( XCYCL(fx>>16)>>kmGrid, YCYCL(fy >> 16) >>kmGrid)] )*vp|=31;

					//для показа HARDNESS сетки
					//if( (GRIDAT_MASK_HARDNESS & GABuf[offsetGBuf( XCYCL(fx>>16)>>kmGrid, YCYCL(fy >> 16) >>kmGrid)]) ==0)*vp|=15;
					//if( (GRIDAT_MASK_HARDNESS & GABuf[offsetGBuf( XCYCL(fx>>16)>>kmGrid, YCYCL(fy >> 16) >>kmGrid)]) ==1)*vp|=31<<5;
					//if( (GRIDAT_MASK_HARDNESS & GABuf[offsetGBuf( XCYCL(fx>>16)>>kmGrid, YCYCL(fy >> 16) >>kmGrid)]) ==2)*vp|=15<<11;
				}
				tfy += k_yscr_y;
				vp += XADD;

			
		}
}

void vrtMap::scaling32(int cx,int cy,int xc,int yc,int xside,int yside, unsigned char* GRAF_BUF, int GB_MAXX, int GB_MAXY)
{
	int XSrcSize=hCamera*(xside<<1)/fCamera; //(xside<<1)- это xsize
	unsigned char* vp = GRAF_BUF + 4*((yc - yside)*GB_MAXX + (xc - xside));
	int xsize = 2*xside;
	int ysize = 2*yside;
	int XADD = GB_MAXX - xsize;

	cx = XCYCL(cx);
	cy = YCYCL(cy);

	int YSrcSize = ysize*XSrcSize/xsize;

	int k_xscr_x = (XSrcSize << 16)/xsize;
	int k_yscr_y = (YSrcSize << 16)/ysize;

	int tfx = (cx << 16) - (XSrcSize << 15) + (1 << 15);
	int x0 = tfx >> 16;
	int x1 = x0 + XSrcSize;
	int tfy = (cy << 16) - (YSrcSize << 15) + (1 << 15);
	int y0 = tfy >> 16;
	int y1 = y0 + YSrcSize;


	int i,j,fx,fy;
	unsigned char* lc = &(RnrBuf[0]);//
	unsigned char* ls = &(SurBuf[0]);

	unsigned char* lvd  = &(VxDBuf[0]);
	unsigned char* lvg = &(VxGBuf[0]);
	unsigned char* la = &(AtrBuf[0]);

	unsigned char* dataRnr;
	unsigned char* dataSur;
	unsigned char* dataVxD;
	unsigned char* dataVxG;
	unsigned char* dataAtr;
	unsigned char* dataTex;

#ifdef _SURMAP_
	unsigned char* lss = &(SpecSurBuf[0]);//
	unsigned char* dataSpecSur;
#endif


	for(i = 0;i < ysize;i++){
				fx = tfx;
				fy = tfy;
				dataRnr = lc + YCYCL(fy >> 16)*XS_Buf;
				dataSur= ls + YCYCL(fy >> 16)*XS_Buf;
				dataVxD= lvd + YCYCL(fy >> 16)*XS_Buf;
				dataVxG= lvg + YCYCL(fy >> 16)*XS_Buf;
				dataAtr= la + YCYCL(fy >> 16)*XS_Buf;
				dataTex=&(LvdTex[0]) + (((fy >> 16)&LvdTex_clip_mask_y)<<LvdTex_X_SIZE_POWER);
#ifdef _SURMAP_
				dataSpecSur=lss + YCYCL(fy >> 16)*XS_Buf;
#endif
				for(j = 0; j < xsize; j++, vp+=4){ //
					unsigned int tmpC;
					if(*(dataAtr + XCYCL(fx >> 16)) & At_LEVELED){
						if( (*(dataAtr + XCYCL(fx >> 16))&At_ZPMASK)==At_ZEROPLAST)
							tmpC = SurZP2Col32[*(dataRnr + XCYCL(fx >> 16))][*(dataSur + XCYCL(fx >> 16))];
						else
							tmpC = Tex2Col[*(dataRnr + XCYCL(fx >> 16))][*(dataTex + ((fx >> 16)&LvdTex_clip_mask_x) )];//SurZPE2Col32[*(dataRnr + XCYCL(fx >> 16))][*(dataSur + XCYCL(fx >> 16))];
					}
					else{
						tmpC = Sur2Col32[*(dataSur + XCYCL(fx >> 16))][*(dataRnr + XCYCL(fx >> 16))];
					}
					(unsigned int&)(*vp)=tmpC;
#ifdef _SURMAP_
					const unsigned int MASK_R=0xFF0000;
					const unsigned int MASK_G=0x00FF00;
					const unsigned int MASK_B=0x0000FF;
					const int ALPHA=10;//из 16
					const int ALPHAI=15-ALPHA;
					if(status_ShowHideGeo==1){ //показ скрытого гео + дам
						if(*(dataVxD + XCYCL(fx >> 16))!=0){
							short rnr=SLightTable[SLT_05SIZE+*(dataVxG+XCYCL(fx >> 16)) - *(dataVxG+XCYCL(1+(fx >> 16)))]>>1;
							unsigned int t=Sur2Col32[*(dataSpecSur + XCYCL(fx >> 16))][rnr];
							unsigned int told=*vp;
							t= ((((t&MASK_R)*ALPHA + (told&MASK_R)*ALPHAI)>>4)&MASK_R) | ((((t&MASK_G)*ALPHA + (told&MASK_G)*ALPHAI)>>4)&MASK_G) | ((((t&MASK_B)*ALPHA + (told&MASK_B)*ALPHAI)>>4));
							(unsigned int&)(*vp)=t;
						}
					}
					else if(status_ShowHideGeo==2){ //показ только скрытого гео
						if(*(dataVxD + XCYCL(fx >> 16))!=0){
							short rnr=SLightTable[SLT_05SIZE+*(dataVxG+XCYCL(fx >> 16)) - *(dataVxG+XCYCL(1+(fx >> 16)))]>>1;
							(unsigned int&)(*vp)=Sur2Col32[*(dataSpecSur + XCYCL(fx >> 16))][rnr];
						}
					}
#endif
					fx += k_xscr_x;
					//Только для показа зеропласта 
					//if( GRIDAT_ZEROPLAST & GABuf[offsetGBuf( XCYCL(fx>>16)>>kmGrid, YCYCL(fy >> 16) >>kmGrid)] )*vp|=31;
	//				if( (GRIDAT_MASK_CLUSTERID & GABuf[offsetGBuf( XCYCL(fx>>16)>>kmGrid, YCYCL(fy >> 16) >>kmGrid)]) !=0)*vp|=63<<5;//==0x0100
					//if( (GRIDAT_MASK_CLUSTERID & GABuf[offsetGBuf( XCYCL(fx>>16)>>kmGrid, YCYCL(fy >> 16) >>kmGrid)]) ==0x0100)*vp|=63<<5;
	////				if( (GRIDAT_MASK_CLUSTERID & GABuf[offsetGBuf( XCYCL(fx>>16)>>kmGrid, YCYCL(fy >> 16) >>kmGrid)]) ==0x0200)*vp|=31;
					//if( GRIDAT_TALLER_HZEROPLAST  & GABuf[offsetGBuf( XCYCL(fx>>16)>>kmGrid, YCYCL(fy >> 16) >>kmGrid)] )*vp|=31;
					//if( GRIDAT_LEVELED & GABuf[offsetGBuf( XCYCL(fx>>16)>>kmGrid, YCYCL(fy >> 16) >>kmGrid)] )*vp|=31;

					//для показа HARDNESS сетки
					//if( (GRIDAT_MASK_HARDNESS & GABuf[offsetGBuf( XCYCL(fx>>16)>>kmGrid, YCYCL(fy >> 16) >>kmGrid)]) ==0)*vp|=15;
					//if( (GRIDAT_MASK_HARDNESS & GABuf[offsetGBuf( XCYCL(fx>>16)>>kmGrid, YCYCL(fy >> 16) >>kmGrid)]) ==1)*vp|=31<<5;
					//if( (GRIDAT_MASK_HARDNESS & GABuf[offsetGBuf( XCYCL(fx>>16)>>kmGrid, YCYCL(fy >> 16) >>kmGrid)]) ==2)*vp|=15<<11;

					//Для показа сетки проходимости
				}
				tfy += k_yscr_y;
				vp += XADD;

			
		}
}

void vrtMap::viewV(int XSrcSize,int cx,int cy,int xc,int yc,int xside,int yside)
{
/*	//short* vp = (short*)XGR_VIDEOBUF + (yc - yside)*XGR_MAXX + (xc - xside);
	short* vp = (short*)XGR_VIDEOBUF;
	unsigned short* vz = zBuffer + (yc - yside)*XGR_MAXX + (xc - xside);
	int xsize = 2*xside;
	int ysize = 2*yside;
	int XADD = XGR_MAXX - xsize;


//	void View(int x0,int y0,float aa)

	int lasty[640],         // Last pixel drawn on a given column
		lastc[640];         // Color of last pixel on a column
	int d;
	int a,b,hCamera,u0,v0,u1,v1,h0,h1,h2,h3;
	float FOV=3.141592654f/4;   // half of the xy field of view

	//??memset(Video,0,320*200);

	// Initialize last-y and last-color arrays
	for ( d=0; d<XGR_MAXX; d++ ){
		lasty[d]=XGR_MAXY;
		lastc[d]=-1;
	}
	int x0=XCYCL(cx)<<16;
	int y0=YCYCL(cy)<<16;

	// Compute the xy coordinates; a and b will be the position inside the
	// single map cell (0..255). //a,b дробная часть
	u0=(x0>>16)&clip_mask_x;			a=(x0>>8)&255;
	v0=((y0>>16)&clip_mask_y);			b=(y0>>8)&255;
	u1=(u0+1)&clip_mask_x;
	v1=((v0+1)&clip_mask_y);

	// Fetch the height at the four corners of the square the point is in
	unsigned short* lv = &(vMap -> VxBuf[0]);
	v1*=XS_Buf; v0*=XS_Buf;
	h0=lv[u0+v0]; h2=lv[u0+v1];
	h1=lv[u1+v0]; h3=lv[u1+v1];

	// Compute the height using bilinear interpolation
	h0=(h0<<8)+a*(h1-h0);
	h2=(h2<<8)+a*(h3-h2);
	hCamera=((h0<<8)+b*(h2-h0))>>16>>VX_FRACTION;
	hCamera+=30;//Высота камеры над поверхностью

	// Draw the landscape from near to far without overdraw
	for ( d=0; d<1000; d+=1 ){//d+=1+(d>>6)
		//Line(x0+d*65536*cos(aa-FOV),y0+d*65536*sin(aa-FOV),
		//x0+d*65536*cos(aa+FOV),y0+d*65536*sin(aa+FOV),
		//h-30,100*256/(d+1));
		int xx0=x0+d*65536*cos(aa-FOV);
		int yy0=y0+d*65536*sin(aa-FOV);
		int xx1=x0+d*65536*cos(aa+FOV);
		int yy1=y0+d*65536*sin(aa+FOV);

		int scaling=1000*256/(d+1); //scaling=100*256/(d+1);



		//void Line(int x0,int y0,int x1,int y1,int hy,int s)
		int i,sx,sy;

		// Compute xy speed
		sx=(xx1-xx0)/XGR_MAXX; sy=(yy1-yy0)/XGR_MAXX;
		for ( i=0; i<XGR_MAXX; i++ ){
			int c,y,h;

			u0=(xx0>>16)&clip_mask_x;		a=(xx0>>8)&255;
			v0=((yy0>>16)&clip_mask_y);		b=(yy0>>8)&255;
			u1=(u0+1)&clip_mask_x;
			v1=((v0+1)&clip_mask_y);

			v1*=XS_Buf; v0*=XS_Buf;
			h0=lv[u0+v0]; h2=lv[u0+v1];
			h1=lv[u1+v0]; h3=lv[u1+v1];

			// Compute the height using bilinear interpolation
			h0=(h0<<8)+a*(h1-h0);
			h2=(h2<<8)+a*(h3-h2);
			h=((h0<<8)+b*(h2-h0))>>16>>VX_FRACTION;

			// Fetch the color at the four corners of the square the point is in

//			unsigned short* lc = &(vMap -> ClBuf[0]);//
//			h0=lc[u0+v0]; h2=lc[u0+v1];
//			h1=lc[u1+v0]; h3=lc[u1+v1];
			h0=31;

			// Compute the color using bilinear interpolation (in 16.16)
			//h0=(h0<<8)+a*(h1-h0);
			//h2=(h2<<8)+a*(h3-h2);
			//c=((h0<<8)+b*(h2-h0));
			c=h0<<16; //Суммирование цвета пока выключено

			// Compute screen height using the scaling factor
			y=(((hCamera-h)*scaling)>>10)+240;//100

			// Draw the column
			if ( y<(a=lasty[i]) ){
				short *b=vp+a*XGR_MAXX+i;
				int sc,cc;
				if ( lastc[i]==-1 ) lastc[i]=c;
				//sc=(c-lastc[i])/(a-y); // Переход цвета будет сделан потом
				sc=0;
				cc=c;//lastc[i];//Переход цвета будет сделан потом
				if ( a>(XGR_MAXY-1) ) {
					b-=(a-(XGR_MAXY-1))*XGR_MAXX; cc+=(a-(XGR_MAXY-1))*sc; a=(XGR_MAXY-1); 
				}
				if ( y<0 ) y=0;
				while ( y<a ) {
					*b=cc>>16;//>>18 
					cc+=sc; 
					b-=XGR_MAXX; a--;
				}
				lasty[i]=y;
			}
			lastc[i]=c;

			// Advance to next xy position
			xx0+=sx; yy0+=sy;
		}


	
	
	}
	for (int i=0; i<XGR_MAXX; i++ ){
		if ( (lasty[i])>0 ){
		short *b=vp+lasty[i]*XGR_MAXX+i;
			while ( (lasty[i])>0 ) {
				*b=0;
				b-=XGR_MAXX;
				lasty[i]--;
			}
		}
	}

*/
}


/*//Поворот плоскости
float turnAngleAroundZ=0;//-3.141592654f/2;
//Фокус
int focusC=600;
//Расстояние от камеры
int cameraDZ=100;
//Угол наклона камеры
int AAA=140;*/
//float turnAngleAroundZ=-3.141592654f/2;
//void vrtMap::viewV2D(int XSrcSize,int cx,int cy,int xc,int yc,int xside,int yside)
void vrtMap::viewV2D(int cx,int cy,int xc,int yc,int xside,int yside, unsigned short* GRAF_BUF, int GB_MAXX, int GB_MAXY)
{
	//short* vp = (short*)XGR_VIDEOBUF + (yc - yside)*XGR_MAXX + (xc - xside);
	short* vp = (short*)GRAF_BUF;
	//unsigned short* vz = zBuffer + (yc - yside)*GB_MAXX + (xc - xside);
	int xsize = 2*xside;
	int ysize = 2*yside;
	int XADD = GB_MAXX - xsize;


//	void View(int x0,int y0,float turnAngleAroundZ)

	int lasty[MAX_XSIZE_VX_WINDOW],         // Last pixel drawn on a given column
		lastc[MAX_XSIZE_VX_WINDOW];         // Color of last pixel on a column
	int d;
	int a,b,u0,v0,u1,v1,h0,h1,h2,h3;
	float FOV=3.141592654f/4;   // half of the xy field of view

	//??memset(Video,0,320*200);

	// Initialize last-y and last-color arrays
	for ( d=0; d<GB_MAXX; d++ ){
		lasty[d]=GB_MAXY;
		lastc[d]=-1;
	}
	int x0=XCYCL(cx)<<16;
	int y0=YCYCL(cy)<<16;

	//hCamera=600;//Высота камеры над поверхностью

//	unsigned char* lv = &(WVxBuf[0]); 
	unsigned char* lvG = &(VxGBuf[0]); 
	unsigned char* lvD = &(VxDBuf[0]); 

	float tga=tan(cameraAngleLean*3.14/180);
	int dh=round(65536*(sin((cameraAngleLean-90)*3.14/180)*focusC/(focusC+cameraDZ)) );
	int DY,DX;
	for (d=-yside-200; d<yside; d++){
		DY = round(sqrt( pow(d*(focusC+cameraDZ)/(d+tga*focusC),2) + pow(tga*d*(focusC+cameraDZ)/(d+tga*focusC),2) )*65536);
		if(d<0)DY=-DY;
		DX = round( 65536*xside*(focusC+( cameraDZ-(d*(focusC+cameraDZ)/(d+tga*focusC)) ) )/focusC );


	// Draw the landscape from near to far without overdraw
	//for ( d=0; d<1000; d+=1 ){//d+=1+(d>>6)
		//Line(x0+d*65536*cos(aa-FOV),y0+d*65536*sin(aa-FOV),
		//x0+d*65536*cos(aa+FOV),y0+d*65536*sin(aa+FOV),
		//h-30,100*256/(d+1));
		/*int xx0=(x0-DX)*cos(aa)-(y0-DY)*sin(aa);
		int yy0=(y0-DY)*cos(aa)+(x0-DX)*sin(aa);
		int xx1=(x0+DX)*cos(aa)-(y0-DY)*sin(aa);
		int yy1=(y0-DY)*cos(aa)+(x0+DX)*sin(aa);*/

		int xx0=x0+( (-DX)*cos(turnAngleAroundZ)-(-DY)*sin(turnAngleAroundZ) );
		int yy0=y0+( (-DY)*cos(turnAngleAroundZ)+(-DX)*sin(turnAngleAroundZ) );
		int xx1=x0+( (+DX)*cos(turnAngleAroundZ)-(-DY)*sin(turnAngleAroundZ) );
		int yy1=y0+( (-DY)*cos(turnAngleAroundZ)+(+DX)*sin(turnAngleAroundZ) );

		//int scaling=100;///(d+1); //scaling=100*256/(d+1);


		//void Line(int x0,int y0,int x1,int y1,int hy,int s)
		int i,sx,sy;

		// Compute xy speed
		sx=(xx1-xx0)/GB_MAXX; sy=(yy1-yy0)/GB_MAXX;
		for ( i=0; i<GB_MAXX; i++ ){
			int c,y,h;

			u0=(xx0>>16)&clip_mask_x;		a=(xx0>>8)&255;
			v0=((yy0>>16)&clip_mask_y);		b=(yy0>>8)&255;
			u1=(u0+1)&clip_mask_x;
			v1=((v0+1)&clip_mask_y);

			v1*=XS_Buf; v0*=XS_Buf;
			h0=lvD[u0+v0]; h2=lvD[u0+v1];
			h1=lvD[u1+v0]; h3=lvD[u1+v1];
			if(!h0) h0=lvG[u0+v0];
			if(!h1) h1=lvG[u1+v0];
			if(!h2) h2=lvG[u0+v1];
			if(!h3) h3=lvG[u1+v1];

			// Compute the height using bilinear interpolation
			h0=(h0<<8)+a*(h1-h0);
			h2=(h2<<8)+a*(h3-h2);
			h=((h0<<8)+b*(h2-h0))>>16;//>>VX_FRACTION

			// Fetch the color at the four corners of the square the point is in
			//Показ цвета
/*			unsigned short* lc = &(ClBuf[0][0][0]);//
			h0=lc[u0+v0]; h2=lc[u0+v1];
			h1=lc[u1+v0]; h3=lc[u1+v1];

			// Compute the color using bilinear interpolation (in 16.16)
			//h0=(h0<<8)+a*(h1-h0);
			//h2=(h2<<8)+a*(h3-h2);
			//c=((h0<<8)+b*(h2-h0));
			c=h0<<16; //Суммирование цвета пока выключено*/

			//Показ по старой палитре
/*			unsigned char* lc1 = &(ClTrBuf[0]);//
			unsigned char* lc2 = &(AtBuf[0]);//
			//c=TrPal[lc2[u0+v0]& TrW_MASK][lc1[u0+v0]]<<16;
			h0=lc1[u0+v0];
			h1=lc1[u1+v0];
			h2=lc1[u0+v1];
			h3=lc1[u1+v1];

			h0=(h0<<8)+a*(h1-h0);
			h2=(h2<<8)+a*(h3-h2);
			c=((h0<<8)+b*(h2-h0));
			c=TrPal[lc2[u0+v0]& TrW_MASK][c>>16]<<16;*/
			//Показ по новой палитре
			unsigned char* lc1 = &(RnrBuf[0]);//
			unsigned char* lc2 = &(SurBuf[0]);//
			//c=TrPal[lc2[u0+v0]& TrW_MASK][lc1[u0+v0]]<<16;
			h0=lc1[u0+v0];
			h1=lc1[u1+v0];
			h2=lc1[u0+v1];
			h3=lc1[u1+v1];
			unsigned char pr=(h0&0x80) | (h1&0x80) | (h2&0x80) | (h3&0x80);
			h0&=0x7f; h1&=0x7f; h2&=0x7f; h3&=0x7f;

			h0=(h0<<8)+a*(h1-h0);
			h2=(h2<<8)+a*(h3-h2);
			c=((h0<<8)+b*(h2-h0));
			c=Sur2Col[lc2[u0+v0]][pr | (c>>16)]<<16;


			// Compute screen height using the scaling factor
			y=GB_MAXY/2 -d -(dh*h>>16);//100(((hCamera-h)*scaling)>>10)

			// Draw the column
			if ( y<(a=lasty[i]) ){
				short *b=vp+a*GB_MAXX+i;
				int sc,cc;
				if ( lastc[i]==-1 ) lastc[i]=c;
				//sc=(c-lastc[i])/(a-y); // Переход цвета будет сделан потом
				sc=0;
				cc=c;//lastc[i];//Переход цвета будет сделан потом
				if ( a>(GB_MAXY-1) ) {
					b-=(a-(GB_MAXY-1))*GB_MAXX; cc+=(a-(GB_MAXY-1))*sc; a=(GB_MAXY-1); 
				}
				if ( y<0 ) y=0;
				while ( y<a ) {
					*b=cc>>16;//>>18 
					cc+=sc; 
					b-=GB_MAXX; a--;
				}
				lasty[i]=y;
			}
			lastc[i]=c;

			// Advance to next xy position
			xx0+=sx; yy0+=sy;
		}


	
	}
	for (int i=0; i<GB_MAXX; i++ ){
		if ( (lasty[i])>0 ){
		short *b=vp+lasty[i]*GB_MAXX+i;
			while ( (lasty[i])>0 ) {
				*b=0;
				b-=GB_MAXX;
				lasty[i]--;
			}
		}
	}


}

inline int vrtMap::tstMinMaxVox(int h)
{
	if(h < 0) h = 0;
	else if(h > MAX_VX_HEIGHT) h = MAX_VX_HEIGHT;
#ifdef _SURMAP_
	if(h < FilterMinHeight) h = FilterMinHeight ;
	if(h > FilterMaxHeight) h = FilterMaxHeight;
#endif
	return h;
}
/*
void vrtMap::voxSet(int x,int y,int delta,int terrain) //terrain=-1 определено в заголовочнике
{
#ifdef _SURMAP_
	if(clipXLeft!=-1){
		if(x<clipXLeft || x>clipXRight || y<clipYTop || y>clipYBottom) return;
	};
#endif
	x=XCYCL(x);
	y=YCYCL(y);
	int offset=offsetBuf(x,y);
	int h, h2, dg;
#ifdef _SURMAP_ //Мозаика по высоте
	if(VBitMapMosaic.Enable) delta = VBitMapMosaic.getDelta(x,y,delta);
#endif
	switch(VxBufWorkMode){
		case GEOONLY:
//			if(VxDBuf[offset]!=0) return; //если есть Dam слой
#ifdef _SURMAP_
			if(!FilterGeoTerrain[GetTer(offset)]) return;
#endif
			h=GetAltGeo(offset);
#ifdef _SURMAP_
			if(h < FilterMinHeight || h > FilterMaxHeight) return;
#endif
			h=tstMinMaxVox(h+delta);
			if(GetAltDam(offset)<h){
				VxDBuf[offset]=0;
				VxGBuf[offset]=h >>VX_FRACTION;
				AtrBuf[offset]=(h &VX_FRACTION_MASK) | (AtrBuf[offset]&=~VX_FRACTION_MASK);
				SetTer(offset,f3d.calc(x, y, h));//>>VX_FRACTION
			}
			else{
				VxGBuf[offset]=h >>VX_FRACTION;
#ifdef _SURMAP_
				SpecSurBuf[offset]=f3d.calc(x, y, h);
#endif
			}
			break;
		case GEO:
//			if(VxDBuf[offset]!=0) return; //если есть Dam слой
#ifdef _SURMAP_
			if(VxDBuf[offset]!=0) { if(!FilterDamTerrain[GetTer(offset)]) return; }
			else { if(!FilterGeoTerrain[GetTer(offset)]) return; }
#endif
			if(VxDBuf[offset]!=0) h=GetAltDam(offset); //если есть Dam слой
			else h=GetAltGeo(offset);
#ifdef _SURMAP_
			if(h < FilterMinHeight || h > FilterMaxHeight) return;
#endif
			h=tstMinMaxVox(h+delta);

			VxDBuf[offset]=0;
			VxGBuf[offset]=h >>VX_FRACTION;
			AtrBuf[offset]=(h &VX_FRACTION_MASK) | (AtrBuf[offset]&=~VX_FRACTION_MASK);
			SetTer(offset,f3d.calc(x, y, h));//>>VX_FRACTION
			break;
		case DAM:
			if(VxDBuf[offset]==0){ //если нет DAM слоя
#ifdef _SURMAP_
				if(!FilterGeoTerrain[GetTer(offset)]) return;
#endif
				if(delta<=0) return;
				else{ //Если Dam насыпается
					h=GetAltGeo(offset);
#ifdef _SURMAP_
					if(h < FilterMinHeight || h > FilterMaxHeight) return;
#endif
					h=tstMinMaxVox(h+delta);
					VxDBuf[offset]=h >>VX_FRACTION;
					AtrBuf[offset]=(h &VX_FRACTION_MASK) | (AtrBuf[offset]&=~VX_FRACTION_MASK);
				}
			}
			else{ //Если Dam есть
#ifdef _SURMAP_
				if(!FilterDamTerrain[GetTer(offset)]) return;
#endif
				h=GetAltDam(offset);
#ifdef _SURMAP_
				if(h < FilterMinHeight || h > FilterMaxHeight) return;
#endif
				h=tstMinMaxVox(h+delta);
				//delta проверяется для быстроты
				if((delta <= 0) && ((h>>VX_FRACTION) < VxGBuf[offset]) ){//Выступила Geo поверхность
					VxDBuf[offset]=0;
					AtrBuf[offset]=(0 &VX_FRACTION_MASK) | (AtrBuf[offset]&=~VX_FRACTION_MASK);
					SetTer(offset,f3d.calc(x, y, VxGBuf[offset]<<VX_FRACTION));
					break; //т.к. Dam слоя уже нет то прерываем
				}
				else{ //Если остается Dam слой
					VxDBuf[offset]=h >>VX_FRACTION;
					AtrBuf[offset]=(h &VX_FRACTION_MASK) | (AtrBuf[offset]&=~VX_FRACTION_MASK);
				}
			}
#ifdef _SURMAP_
			//простая дам поверхнсть
			if(currentDamTerrain < MAX_SIMPLE_DAM_SURFACE) { SetTer(x,y,currentDamTerrain); }
			//мозаика
			else { }
#endif
			break;
		case GEOorDAM:
			if(VxDBuf[offset]==0) { //если выступает Geo слой
#ifdef _SURMAP_
			if(!FilterGeoTerrain[GetTer(offset)]) return;
#endif
				h=GetAltGeo(offset);
#ifdef _SURMAP_
				if(h < FilterMinHeight || h > FilterMaxHeight) return;
#endif
				h=tstMinMaxVox(h+delta);
				VxGBuf[offset]=h >>VX_FRACTION;
				AtrBuf[offset]=(h &VX_FRACTION_MASK) | (AtrBuf[offset]&=~VX_FRACTION_MASK);
				SetTer(offset,f3d.calc(x, y, h));//>>VX_FRACTION
			}
			else{ //если выступает Dam слой
#ifdef _SURMAP_
				if(!FilterDamTerrain[GetTer(offset)]) return;
#endif
				h=GetAltDam(offset);
#ifdef _SURMAP_
				if(h < FilterMinHeight || h > FilterMaxHeight) return;
#endif
				h=tstMinMaxVox(h+delta);
				if(delta <= 0){ 
					if((h>>VX_FRACTION) < VxGBuf[offset]){//Выступила Geo поверхность
						VxDBuf[offset]=0;
						AtrBuf[offset]=(0 &VX_FRACTION_MASK) | (AtrBuf[offset]&=~VX_FRACTION_MASK);
						SetTer(offset,f3d.calc(x, y, VxGBuf[offset])<<VX_FRACTION);
					}
					else goto continue_work_dam;
				}
				else{ //Если остается Dam слой
continue_work_dam:	VxDBuf[offset]=h >>VX_FRACTION;
					AtrBuf[offset]=(h &VX_FRACTION_MASK) | (AtrBuf[offset]&=~VX_FRACTION_MASK);
				}
			}
			break;
		case GEOiDAM:
			if(VxDBuf[offset]==0) { //если выступает Geo слой
#ifdef _SURMAP_
			if(!FilterGeoTerrain[GetTer(offset)]) return;
#endif
				h=GetAltGeo(offset);
#ifdef _SURMAP_
				if(h < FilterMinHeight || h > FilterMaxHeight) return;
#endif
				h=tstMinMaxVox(h+delta);
				VxGBuf[offset]=h >>VX_FRACTION;
				AtrBuf[offset]=(h &VX_FRACTION_MASK) | (AtrBuf[offset]&=~VX_FRACTION_MASK);
				SetTer(offset,f3d.calc(x, y, h));//>>VX_FRACTION
			}
			else{ //если выступает Dam слой
#ifdef _SURMAP_
				if(!FilterDamTerrain[GetTer(offset)]) return;
#endif
				h=GetAltDam(offset);
#ifdef _SURMAP_
				if(h < FilterMinHeight || h > FilterMaxHeight) return;
#endif
				h2=tstMinMaxVox(h+delta);
				dg=(h2>>VX_FRACTION)-(h>>VX_FRACTION);
				VxDBuf[offset]=h2 >>VX_FRACTION;
				AtrBuf[offset]=(h2 &VX_FRACTION_MASK) | (AtrBuf[offset]&=~VX_FRACTION_MASK);
				h=VxGBuf[offset];
				h+=dg; if(h<0)h=0; if(h>255)h=255;
				VxGBuf[offset]=h;
			}
			break;

	}

	//Удаление зеропласта
	ClearAllZP(x,y);
}
*/
void vrtMap::voxSet(int x,int y,int delta,int terrain) //terrain=-1 определено в заголовочнике
{
	x=XCYCL(x);
	y=YCYCL(y);
	int offset=offsetBuf(x,y);
	int h, h2, dg;


	if(VxDBuf[offset]==0) { //если выступает Geo слой
		h=GetAltGeo(offset);

		if(h < FilterMinHeight || h > FilterMaxHeight) return;

		h=tstMinMaxVox(h+delta);
		VxGBuf[offset]=h >>VX_FRACTION;
		AtrBuf[offset]=(h &VX_FRACTION_MASK) | (AtrBuf[offset]&=~VX_FRACTION_MASK);
		SetTer(offset,f3d.calc(x, y, h));//>>VX_FRACTION
	}
	else{ //если выступает Dam слой
		h=GetAltDam(offset);

		if(h < FilterMinHeight || h > FilterMaxHeight) return;

		h2=tstMinMaxVox(h+delta);
		dg=(h2>>VX_FRACTION)-(h>>VX_FRACTION);
		VxDBuf[offset]=h2 >>VX_FRACTION;
		AtrBuf[offset]=(h2 &VX_FRACTION_MASK) | (AtrBuf[offset]&=~VX_FRACTION_MASK);
		h=VxGBuf[offset];
		h+=dg; if(h<0)h=0; if(h>255)h=255;
		VxGBuf[offset]=h;
	}
	//Удаление зеропласта
	ClearAllZP(x,y);
}

//Необходимо вызывать до удаления VxDBuf!!!
void vrtMap::delLeveledTexture(void)
{
	if(SurBuf!=LvdTex){
		if(LvdTex!=0) delete [] LvdTex;
	}
	LvdTex=0;
}

//необходимо вызывать после загрузки VxDBuf и палитры
int vrtMap::loadLeveledTexture(void)
{
	// По умолчанию для отображения DAM слоя
	LvdTex=SurBuf;
	LvdTex_X_SIZE_POWER=H_SIZE_POWER;
	LvdTex_Y_SIZE_POWER=V_SIZE_POWER;

	LvdTex_clip_mask_x=(1<<LvdTex_X_SIZE_POWER)-1;
	LvdTex_clip_mask_y=(1<<LvdTex_Y_SIZE_POWER)-1;
	LvdTex_spec_clip_mask_y=LvdTex_clip_mask_y<<LvdTex_Y_SIZE_POWER;
	LvdTex_SPEC_SHIFT_X=H_SIZE_POWER-LvdTex_X_SIZE_POWER;
	LvdTex_SPEC_SHIFT_Y=(V_SIZE_POWER-LvdTex_Y_SIZE_POWER) + LvdTex_SPEC_SHIFT_X;


#ifdef _PERIMETER_
	TGAHEAD thead;

	thead.init();
	XStream ff(0);
	if(!ff.open(GetTargetName(worldLeveledTextureFile), XS_IN)) {
		xassert(0 && "loadLeveledTexture: file not found");
		return 0;
	}

	ff.read(&thead,sizeof(TGAHEAD));
	if(!thead.ColorMapType || thead.CMapDepth!=24) {
		xassert(0 && "loadLeveledTexture: texture not 256 color");
		return 0;
	}

	if( (thead.Width > H_SIZE) || (thead.Height > V_SIZE) ) {
		xassert(0 && "loadLeveledTexture: size texture > size map");
		return 0; //Ограничение на размер связано с лишней проверкой при выборке цвета по offset
	}

	int powerWidth=BitSR(thead.Width);
	int powerHeight=BitSR(thead.Height);
	if( (thead.Width!=(1<<powerWidth)) || (thead.Height!=(1<<powerHeight)) ) {
		xassert(0 && "loadLeveledTexture: not multiple to two a degree a size");
		return 0;
	}

	LvdTex_X_SIZE_POWER=powerWidth;
	LvdTex_Y_SIZE_POWER=powerHeight;

	delLeveledTexture();
	LvdTex=new unsigned char[(1<<LvdTex_X_SIZE_POWER)*(1<<LvdTex_Y_SIZE_POWER)];

	LvdTex_clip_mask_x=(1<<LvdTex_X_SIZE_POWER)-1;
	LvdTex_clip_mask_y=(1<<LvdTex_Y_SIZE_POWER)-1;
	LvdTex_spec_clip_mask_y=LvdTex_clip_mask_y<<LvdTex_Y_SIZE_POWER;
	LvdTex_SPEC_SHIFT_X=H_SIZE_POWER-LvdTex_X_SIZE_POWER;
	LvdTex_SPEC_SHIFT_Y=(V_SIZE_POWER-LvdTex_Y_SIZE_POWER) + LvdTex_SPEC_SHIFT_X;

	//CMapStart=0;CMapLenght=0;CMapDepth=0;
	//unsigned char palRGB[256*3];
	unsigned char SurPal[MAX_DAM_SURFACE_TYPE*3];
	ff.read(&SurPal[thead.CMapStart*3],thead.CMapDepth*thead.CMapLenght>>3);

	unsigned char* line = new unsigned char[thead.Width], *p;
	int ibeg,jbeg,iend,jend,ik,jk,i,j;
	if(thead.ImageDescriptor&0x20) { jbeg=0; jend=thead.Height; jk=1;}
	else { jbeg=thead.Height-1; jend=-1; jk=-1;}
	if((thead.ImageDescriptor&0x10)==0) { ibeg=0; iend=thead.Width; ik=1;}
	else { ibeg=thead.Width-1; iend=-1; ik=-1;}
	bool flag_err_ovewrflow=0;
	for(j=jbeg; j!=jend; j+=jk){
		p = line;
		int off=j*thead.Width;
		ff.read(line, thead.Width);
		for(i=ibeg; i!=iend; i+=ik, p++){
			unsigned char sur=(*p);
			LvdTex[off+i]=sur;
		}
	}
	ff.close();
	delete line;


	//Конвертирование палитры из BGR в RGB
	for(i=0; i< MAX_DAM_SURFACE_TYPE; i++){
		TexPal[i*3+0]=SurPal[(i)*3+2];
		TexPal[i*3+1]=SurPal[(i)*3+1];
		TexPal[i*3+2]=SurPal[(i)*3+0];
	}
	convertPal2TableTexCol();
#endif // ifndef _SURMAP_
	return 1;
}

/*
int vrtMap::drawCircleZeroLayer(int _x, int _y, int _r)
{
	int i;
	const r2=_r*_r;
	for(i=0; i<=_r; i++){
		int dx=sqrt(r2-i*i);
		for(j=0; j<=dx; j++){
			if()
		}
	}
}
*/
////////// AUX ///////////////////////////
static int* LineTable = 0;
static int LineTableLenght;
void calcLineTable(int curr_lenght,int k_vu,int base_step,int up_step)
{
	if(!LineTable){
		LineTableLenght = 1024;
		if(!(LineTableLenght & 1))
			LineTableLenght++;
		memset(LineTable = new int[LineTableLenght],0,sizeof(int)*LineTableLenght);
		}
	int fv = (-k_vu)*curr_lenght/2 + (1 << 15);
	int old_v,v;
	int v_diff;

	int* c = LineTable;
	old_v = (fv >> 16)*up_step;
	for(int u = 0;u <= curr_lenght;u++){
		fv += k_vu;
		v = (fv >> 16)*up_step;
		v_diff = v - old_v + base_step;
		old_v = v;
		*c++ = v_diff;
		}
}

void vrtMap::PrmFile::init(const char* name)
{
	XStream ff(name,XS_IN);
	buf = new char[len = ff.size() + 1]; //+1 необходим для вставки в конец файла "0"(иначе при обработке строк будет мусор)
	buf[len-1]='\0'; //Запись "0" в конц файла.
	ff.read(buf,len);
	ff.close();

	int i = 0,mode = 1;
	char c;
	while(i < len){
		c = buf[i];
		if(c == '\"'){
			buf[i++] = '\0';
			while(i < len){
				if(buf[i] == '\"') break;
				i++;
				}
			if(i == len) ErrH.Abort("Wrong PRM format, second <\"> is absent");
			else buf[i] = '\0';
			}
		else
			if(c == ' ' || c == '\t' || c == '\r' || c == ',' || c == '\n') buf[i] = '\0';
		i++;
	}
	i = 0;
	while(i < len){
		c = buf[i];
		if(mode){ if(c == '/' && buf[i + 1] == '*'){ buf[i] = '\0'; mode = 0; } }
		else {
			if(c == '*' && buf[i + 1] == '/'){ buf[i] = buf[i + 1] = '\0'; i++; mode = 1; }
			else if(c != '\n') buf[i] = '\0';
			}
		i++;
	}
	index = 0;
}

char* vrtMap::PrmFile::getAtom(void)
{
	char* p = buf + index;
	while(index < len && !(*p)) p++, index++;
	if(index == len) return NULL;
	char* ret = p;
	while(index < len && *p) p++, index++;
	return ret;
}

unsigned char* convert_vox2vid(int vox, char* buf)
{
	int fraction,cel;
	if(vox>=0){
		fraction=vox & VX_FRACTION_MASK;
		cel=vox>>VX_FRACTION;
		sprintf(buf,"%4hi.%02hu\0",cel,fraction);
	}
	else {
		fraction= (-vox) & VX_FRACTION_MASK; //Дробную часть надо показывать без знака
		cel=(-vox)>>VX_FRACTION;				
		sprintf(buf,"-%04hi.%02hu\0",cel,fraction);
	}
	return (unsigned char*)buf;
}
int convert_vid2vox(char* buf)
{
	char cc[10]={'0','0','\0'};
	int fraction=0;
	short cels=0;
	sscanf(buf,"%hd%*c%s",&cels,cc);
	float Znak=0;
	sscanf(buf,"%f",&Znak);
	if(cc[1]==0) cc[1]='0';
	cc[2]=0;
	fraction=atoi(cc);
	if(fraction>VX_FRACTION_MASK)fraction=VX_FRACTION_MASK;
	int cel=(int)cels; //Необходимо т.к. 
	int vox;
	if (Znak>=0) vox= (cel<<VX_FRACTION) | (fraction);
	else {
		vox= ((-cel)<<VX_FRACTION) | (fraction); //Дробная часть знака не имеет
		vox=-vox;
	}
	return vox;
}

void save2Stl(void)
{

	int x,y;
	int * masS;
	masS=new int[vMap.V_SIZE*vMap.H_SIZE];
	for(y=0; y<vMap.V_SIZE; y++){
		for(x=0; x<vMap.H_SIZE; x++){
			masS[x+y*vMap.H_SIZE]=vMap.GetAlt(x,y);
		}
	}
	XStream f("Stl.vmp",XS_OUT);
	f.write(&masS[0],sizeof(int)*vMap.V_SIZE*vMap.H_SIZE);
	delete [] masS;
}

#ifdef _SURMAP_
int vrtMap::putTgaComplete2AllMap(void)
{
	//XBuffer tb;
	//tb < dirBitMap   < "\\" < fname;
/*	TGAHEAD tgahead;
	tgahead.loadHeader(GetTargetName("allDamMap.tga"));
	if((tgahead.PixelDepth!=8) || (tgahead.ImageType!=1)) {
		//ErrH.Message("Не поддерживаемый тип TGA (необходим 256цветный не компрессованный)");
		return 0;
	}
	VBitMap.create(tgahead.Width, tgahead.Height);
	tgahead.load2buf(VBitMap.data);
	m_XC= OwnerClass->m_SurToolMoveVar.m_XCenterM;
	m_YC= OwnerClass->m_SurToolMoveVar.m_YCenterM;
*/
	TGAHEAD thead;

	thead.init();
	//XStream ff(GetTargetName("allDamMap.tga"), XS_IN);//worldDamSurfaceFile
	XStream ff(0);
	if(!ff.open(GetTargetName("allDamMap.tga"), XS_IN)) return 0;

	ff.read(&thead,sizeof(TGAHEAD));
	if( (thead.Width!=H_SIZE) || (thead.Height!=V_SIZE) ) return 0;

	if(!thead.ColorMapType) return 0;
	if(thead.CMapDepth!=24) return 0;

	//CMapStart=0;CMapLenght=0;CMapDepth=0;
	//unsigned char palRGB[256*3];
	unsigned char SurPal[MAX_DAM_SURFACE_TYPE*3];
	ff.read(&SurPal[thead.CMapStart*3],thead.CMapDepth*thead.CMapLenght>>3);

	unsigned char* line = new unsigned char[XS_Buf],*p;
	int ibeg,jbeg,iend,jend,ik,jk,i,j;
	if(thead.ImageDescriptor&0x20) { jbeg=0; jend=YS_Buf; jk=1;}
	else { jbeg=YS_Buf-1; jend=-1; jk=-1;}
	if((thead.ImageDescriptor&0x10)==0) { ibeg=0; iend=XS_Buf; ik=1;}
	else { ibeg=XS_Buf-1; iend=-1; ik=-1;}
	bool flag_err_ovewrflow=0;
	for(j=jbeg; j!=jend; j+=jk){
		p = line;
		ff.read(line,XS_Buf);
		for(i=ibeg; i!=iend; i+=ik, p++){
			int off=offsetBuf(i, j);
			//if(*p>0){//
				if(VxDBuf[off]==0){
					if(!FilterGeoTerrain[GetTer(off)]) continue;
					int h=GetAltGeo(off);
					if(h < FilterMinHeight || h > FilterMaxHeight) continue;

					unsigned short vvv=VxGBuf[off];//+1;
					//if(vvv>255)vvv=255;
					VxDBuf[off]=vvv;
				}
				else {
					if(!FilterDamTerrain[GetTer(off)]) continue;
					//int h=GetAltDam(off);
					int h=SGetAlt(off);
					if(h < FilterMinHeight || h > FilterMaxHeight) continue;
				}
				int sur=(*p);// + MAX_SIMPLE_DAM_SURFACE;
				//if(sur>255){ 
				//	flag_err_ovewrflow=1; sur=255;
				//}
				SurBuf[off]=sur;
			//}//
		}
	}
	ff.close();
	delete line;

	unsigned char* ch = changedT;
	for(i=0; i<V_SIZE; i++)ch[i]=1;

	//Конвертирование палитры из BGR в RGB
	//for(i=MAX_SIMPLE_DAM_SURFACE; i< MAX_DAM_SURFACE_TYPE; i++)
	for(i=0; i< MAX_DAM_SURFACE_TYPE; i++){
//		DamPal[i*3+0]=SurPal[(i-MAX_SIMPLE_DAM_SURFACE)*3+2];
//		DamPal[i*3+1]=SurPal[(i-MAX_SIMPLE_DAM_SURFACE)*3+1];
//		DamPal[i*3+2]=SurPal[(i-MAX_SIMPLE_DAM_SURFACE)*3+0];
		DamPal[i*3+0]=SurPal[(i)*3+2];
		DamPal[i*3+1]=SurPal[(i)*3+1];
		DamPal[i*3+2]=SurPal[(i)*3+0];
	}
	convertPal2TableSurCol(DamPal,Damming);
	if(flag_err_ovewrflow==0) return 1;
	else return 2;
}

void vrtMap::wrldShot(void)
{
	static int cnt = 0;
	const int SX = H_SIZE;
	const int SY = V_SIZE;
//TGA write
	TGAHEAD thead;
	XBuffer buf;
	//buf < "Ph_A_" <= cnt++ < ".tga";
	buf < wTable[cWorld].dir < "_" <= cnt++ < ".tga";
	XStream ff(buf, XS_OUT);
//		thead.IDLenght=0; thead.ColorMapType=0; thead.ImageType=2;
//		thead.CMapStart=0; thead.CMapLenght=0; thead.CMapDepth=0;
//		thead.XOffset=0; thead.YOffset=0;
//		thead.PixelDepth=24; thead.ImageDescriptor=0x20;
	thead.Width=(short)SX;
	thead.Height=(short)SY;
	ff.write(&thead,sizeof(thead));

	unsigned char* line = new unsigned char[SX*3],*p;

	unsigned int i,j;
	for(j = 0; j<V_SIZE; j++){
		p = line;
		for(i = 0; i<H_SIZE; i++){

			int offb=offsetBuf(i,j);

/*			unsigned short col;
			if( AtrBuf[offb] &At_ZEROPLAST){
				if(AtrBuf[offb] &At_ZEROPLASTEMPTY)
					col = SurZPE2Col[RnrBuf[offb]][SurBuf[offb]];
				else
					col = SurZP2Col[RnrBuf[offb]][SurBuf[offb]];
			}
			else{
				col = Sur2Col[SurBuf[offb]][RnrBuf[offb]];
			}
			*p++ = (unsigned char)((col&0x1F)<<3) ;
			*p++ = (unsigned char)((col&0x7E0)>>3) ;
			*p++ = (unsigned char)((col&0x0F800)>>8) ;
*/
			unsigned int col32;
			if( AtrBuf[offb] & At_LEVELED){
				if(AtrBuf[offb] &At_ZEROPLAST)
					col32 = SurZP2Col32[RnrBuf[offb]][SurBuf[offb]];
				else
					col32 = 0;//SurZPE2Col32[RnrBuf[offb]][SurBuf[offb]];
			}
			else{
				col32 = Sur2Col32[SurBuf[offb]][RnrBuf[offb]];
			}
			*p++ = (unsigned char)(col32&0xFF) ;
			*p++ = (unsigned char)((col32&0xFF00)>>8) ;
			*p++ = (unsigned char)((col32&0xFF0000)>>16) ;
		}
		ff.write(line,SX*3);
	}
	ff.close();
	delete line;

}

void vrtMap::wrldShotOutLine(void)
{
	static int cnt = 0;
	const int SX = H_SIZE;
	const int SY = V_SIZE;
//TGA write
	TGAHEAD thead;
	XBuffer buf;
	//buf < "Ph_A_" <= cnt++ < ".tga";
	buf < wTable[cWorld].dir < "_OL_" <= cnt++ < ".tga";
	XStream ff(buf, XS_OUT);
//		thead.IDLenght=0; thead.ColorMapType=0; thead.ImageType=2;
//		thead.CMapStart=0; thead.CMapLenght=0; thead.CMapDepth=0;
//		thead.XOffset=0; thead.YOffset=0;
//		thead.PixelDepth=24; thead.ImageDescriptor=0x20;
	thead.Width=(short)SX;
	thead.Height=(short)SY;
	ff.write(&thead,sizeof(thead));

	unsigned char* line = new unsigned char[SX*3],*p;

	unsigned int i,j;
	for(j = 0; j<V_SIZE; j++){
		p = line;
		for(i = 0; i<H_SIZE; i++){

			int offb=offsetBuf(i,j);

			unsigned short col;
			if( AtrBuf[offb] &At_LEVELED){
				if(AtrBuf[offb] &At_ZEROPLAST){
					//col = SurZP2Col[RnrBuf[offb]][SurBuf[offb]];
					col = SurZP2Col[(RnrBuf[offb]&0x80)+90][SurBuf[offb]];
				}
				else{
					//col = SurZPE2Col[RnrBuf[offb]][SurBuf[offb]];
					col = 0;//SurZPE2Col[(RnrBuf[offb]&0x80)+90][SurBuf[offb]];
				}
			}
			else{
				//col = Sur2Col[SurBuf[offb]][RnrBuf[offb]];
				col = Sur2Col[SurBuf[offb]][(RnrBuf[offb]&0x80)+90];
			}
			*p++ = (unsigned char)((col&0x1F)<<3) ;
			*p++ = (unsigned char)((col&0x7E0)>>3) ;
			*p++ = (unsigned char)((col&0x0F800)>>8) ;
		}
		ff.write(line,SX*3);
	}
	ff.close();
	delete line;

}

void vrtMap::wrldShotMapHeight(void)
{
	static int cnt = 0;
	const int SX = H_SIZE;
	const int SY = V_SIZE;
//TGA write
	TGAHEAD thead;
	XBuffer buf;
	//buf < "Ph_A_" <= cnt++ < ".tga";
	buf < wTable[cWorld].dir < "_MH_" <= cnt++ < ".tga";
	XStream ff(buf, XS_OUT);
//		thead.IDLenght=0; thead.ColorMapType=0; thead.ImageType=2;
//		thead.CMapStart=0; thead.CMapLenght=0; thead.CMapDepth=0;
//		thead.XOffset=0; thead.YOffset=0;
//		thead.PixelDepth=24; thead.ImageDescriptor=0x20;
	thead.PixelDepth=8;
	thead.ImageType=3;
	thead.Width=(short)SX;
	thead.Height=(short)SY;
	ff.write(&thead,sizeof(thead));

	unsigned char* line = new unsigned char[SX],*p;

	unsigned int i,j;
	for(j = 0; j<V_SIZE; j++){
		p = line;
		for(i = 0; i<H_SIZE; i++){

			int offb=offsetBuf(i,j);
			*p++=SGetAlt(offb)>>VX_FRACTION;//1;//
		}
		ff.write(line,SX);
	}
	ff.close();
	delete line;
}


#ifdef _TX3D_LIBRARY_
	#include <tx3d.hpp>
#endif
void vrtMap::superWrldShot(int k)
{
	static int cnt = 0;
	const int SX = H_SIZE;
	const int SY = V_SIZE;
//TGA write
	TGAHEAD thead;
	//XBuffer buf;
	//buf < "Ph_A_" <= cnt++ < ".tga";
	//buf < wTable[cWorld].dir < "_" <= cnt++ < ".tga";

	char buf[MAX_PATH];
	sprintf(buf, "%s_%04u.tga", wTable[cWorld].dir, cnt++);
	XStream ff(buf, XS_OUT);
//		thead.IDLenght=0; thead.ColorMapType=0; thead.ImageType=2;
//		thead.CMapStart=0; thead.CMapLenght=0; thead.CMapDepth=0;
//		thead.XOffset=0; thead.YOffset=0;
//		thead.PixelDepth=24; thead.ImageDescriptor=0x20;
	thead.Width=(short)SX;
	thead.Height=(short)SY;
	ff.write(&thead,sizeof(thead));

	unsigned char* line = new unsigned char[SX*3],*p;

	unsigned int i,j;
	for(j = 0; j<V_SIZE; j++){
		p = line;
		for(i = 0; i<H_SIZE; i++){

			int offb=offsetBuf(i,j);

			unsigned int col32;
			if(VxDBuf[offb]==0){
#ifdef _TX3D_LIBRARY_
				tx3d::Vector3D calcPoint;
				calcPoint.x = i;
				calcPoint.y = j;
				calcPoint.z = (float)(SGetAlt(offb)+k) * 0.03125f;//  /32.0f;
				tx3d::Vector3D clr;
				f3d.indexedTexture->getTexture()->getColor(&clr, calcPoint);
				float Lum=1.5f;//!!!! константы взяты из кода выше !!!
				float Lum0=0.05f;
				float lght=Lum0+(Lum-Lum0)*((float)RnrBuf[offb]/127.f);
				int r=round(clr.x*lght*255.f); r=clamp(r, 0, 255);
				int g=round(clr.y*lght*255.f); g=clamp(g, 0, 255);
				int b=round(clr.z*lght*255.f); b=clamp(b, 0, 255);
				*p++ = (unsigned char)b;
				*p++ = (unsigned char)g;
				*p++ = (unsigned char)r;
#endif
			}
			else{
				col32 = Sur2Col32[SurBuf[offb]][RnrBuf[offb]];
				*p++ = (unsigned char)(col32&0xFF) ;
				*p++ = (unsigned char)((col32&0xFF00)>>8) ;
				*p++ = (unsigned char)((col32&0xFF0000)>>16) ;
			}
		}
		ff.write(line,SX*3);
	}
	ff.close();
	delete line;
}


void vrtMap::FlipWorldH(void)
{
	unsigned int i,j;
	int DY=V_SIZE;
	int DX=H_SIZE>>1;
	int MX=H_SIZE-1;
	for(j = 0; j<DY; j++){
		for(i = 0; i<DX; i++){
			int off1=offsetBuf(i, j);
			int off2=offsetBuf(MX-i, j);
			unsigned char t;
			t=VxGBuf[off1]; VxGBuf[off1]=VxGBuf[off2]; VxGBuf[off2]=t;
			t=VxDBuf[off1]; VxDBuf[off1]=VxDBuf[off2]; VxDBuf[off2]=t;
			t=SurBuf[off1]; SurBuf[off1]=SurBuf[off2]; SurBuf[off2]=t;
			t=AtrBuf[off1]; AtrBuf[off1]=AtrBuf[off2]; AtrBuf[off2]=t;
		}
	}
}

void vrtMap::FlipWorldV(void)
{
	unsigned int i,j;
	int DY=V_SIZE>>1;
	int DX=H_SIZE;
	int MY=V_SIZE-1;
	for(j = 0; j<DY; j++){
		for(i = 0; i<DX; i++){
			int off1=offsetBuf(i, j);
			int off2=offsetBuf(i, MY-j);
			unsigned char t;
			t=VxGBuf[off1]; VxGBuf[off1]=VxGBuf[off2]; VxGBuf[off2]=t;
			t=VxDBuf[off1]; VxDBuf[off1]=VxDBuf[off2]; VxDBuf[off2]=t;
			t=SurBuf[off1]; SurBuf[off1]=SurBuf[off2]; SurBuf[off2]=t;
			t=AtrBuf[off1]; AtrBuf[off1]=AtrBuf[off2]; AtrBuf[off2]=t;
		}
	}
}

void vrtMap::RotateWorldP90(void)
{
	unsigned int i,j;
	int DY=V_SIZE>>1;
	int DX=H_SIZE-1;
	int BEGI=0;
	const int BORDX=H_SIZE-1;
	const int BORDY=V_SIZE-1;
	for(j = 0; j<DY; j++){
		for(i = BEGI; i<DX; i++){
			int off1=offsetBuf(i, j);
			int off2=offsetBuf(BORDX-j, i);
			int off3=offsetBuf(BORDX-i, BORDY-j);
			int off4=offsetBuf(j, BORDY-i);
			unsigned char t;
			t=VxGBuf[off1]; VxGBuf[off1]=VxGBuf[off4]; 
			VxGBuf[off4]=VxGBuf[off3]; VxGBuf[off3]=VxGBuf[off2]; VxGBuf[off2]=t;

			t=VxDBuf[off1]; VxDBuf[off1]=VxDBuf[off4]; 
			VxDBuf[off4]=VxDBuf[off3]; VxDBuf[off3]=VxDBuf[off2]; VxDBuf[off2]=t;

			t=SurBuf[off1]; SurBuf[off1]=SurBuf[off4]; 
			SurBuf[off4]=SurBuf[off3]; SurBuf[off3]=SurBuf[off2]; SurBuf[off2]=t;

			t=AtrBuf[off1]; AtrBuf[off1]=AtrBuf[off4]; 
			AtrBuf[off4]=AtrBuf[off3]; AtrBuf[off3]=AtrBuf[off2]; AtrBuf[off2]=t;

		}
		DX-=1; BEGI+=1;
	}
}

void vrtMap::RotateWorldM90(void)
{
	unsigned int i,j;
	int DY=V_SIZE>>1;
	int DX=H_SIZE-1;
	int BEGI=0;
	const int BORDX=H_SIZE-1;
	const int BORDY=V_SIZE-1;
	for(j = 0; j<DY; j++){
		for(i = BEGI; i<DX; i++){
			int off1=offsetBuf(i, j);
			int off2=offsetBuf(BORDX-j, i);
			int off3=offsetBuf(BORDX-i, BORDY-j);
			int off4=offsetBuf(j, BORDY-i);
			unsigned char t;
			t=VxGBuf[off1]; VxGBuf[off1]=VxGBuf[off2]; 
			VxGBuf[off2]=VxGBuf[off3]; VxGBuf[off3]=VxGBuf[off4]; VxGBuf[off4]=t;

			t=VxDBuf[off1]; VxDBuf[off1]=VxDBuf[off2]; 
			VxDBuf[off2]=VxDBuf[off3]; VxDBuf[off3]=VxDBuf[off4]; VxDBuf[off4]=t;

			t=SurBuf[off1]; SurBuf[off1]=SurBuf[off2]; 
			SurBuf[off2]=SurBuf[off3]; SurBuf[off3]=SurBuf[off4]; SurBuf[off4]=t;

			t=AtrBuf[off1]; AtrBuf[off1]=AtrBuf[off2]; 
			AtrBuf[off2]=AtrBuf[off3]; AtrBuf[off3]=AtrBuf[off4]; AtrBuf[off4]=t;

		}
		DX-=1; BEGI+=1;
	}
}


void vrtMap::saveMapWithOtherSize(void)//Удвоенный размер
{
	sVmpHeader VmpHeader;
	//const char id[4]={'S','2','T','0'};
	//int i;
	XStream fo;
	fo.open("4xWorld", XS_OUT);
	fo.seek(0,XS_BEG);

	//*(int*)VmpHeader.id = *(int*)id;
	VmpHeader.setID("S2T0");
	VmpHeader.XS=4096;
	VmpHeader.YS=4096;


	fo.write(&VmpHeader,sizeof(VmpHeader));

	int i;
	unsigned char dBuf[4096];
	//преобразование гео
	for(i=0; i<4096; i++)dBuf[i]=0;//17 //Высота новог гео слоя
	for(i=0; i<(1024); i++){
		fo.write(&dBuf[0], 4096);
	}
	for(i=0; i<2048; i++){
		fo.write(&dBuf[0], 1024);
		fo.write(&VxGBuf[i*XS_Buf], XS_Buf);
		fo.write(&dBuf[0], 1024);
	}
	for(i=0; i<(1024); i++){
		fo.write(&dBuf[0], 4096);
	}
	//преобразование дам
	for(i=0; i<4096; i++)dBuf[i]=0;//18 //Высота новог дам слоя
	for(i=0; i<(1024); i++){
		fo.write(&dBuf[0], 4096);
	}
	for(i=0; i<2048; i++){
		fo.write(&dBuf[0], 1024);
		fo.write(&VxDBuf[i*XS_Buf], XS_Buf);
		fo.write(&dBuf[0], 1024);
	}
	for(i=0; i<(1024); i++){
		fo.write(&dBuf[0], 4096);
	}
	//преобразование atr
	for(i=0; i<4096; i++)dBuf[i]=0; //17;//Дробная часть высоты
	for(i=0; i<(1024); i++){
		fo.write(&dBuf[0], 4096);
	}
	for(i=0; i<2048; i++){
		fo.write(&dBuf[0], 1024);
		fo.write(&AtrBuf[i*XS_Buf], XS_Buf);
		fo.write(&dBuf[0], 1024);
	}
	for(i=0; i<(1024); i++){
		fo.write(&dBuf[0], 4096);
	}
	//преобразование sur
	for(i=0; i<4096; i++)dBuf[i]=0;
	for(i=0; i<(1024); i++){
		fo.write(&dBuf[0], 4096);
	}
	for(i=0; i<2048; i++){
		fo.write(&dBuf[0], 1024);
		fo.write(&SurBuf[i*XS_Buf], XS_Buf);
		fo.write(&dBuf[0], 1024);
	}
	for(i=0; i<(1024); i++){
		fo.write(&dBuf[0], 4096);
	}
	fo.close();
}

/*

bool vrtMap::saveAllWorldAs(const char* _patch2world)
{
	XBuffer strIn, strOut;
	strIn < wTable[0].dir < "\\" < worldIniFile;
	strOut < _patch2world < "\\" < worldIniFile;
	if(!CopyFile(strIn, strOut, FALSE)){
		return 0;
	}
	free(wTable[0].name);
	free(wTable[0].dir);
	wTable[0].name = strdup("SurMap_world");
	wTable[0].dir = strdup(_patch2world);
	cWorld = 0;

	save3Buf();
	return 1;
}
*/
void vrtMap::saveMapWithOtherSize4To2(const char* _patch2New2x2World)
{
	XBuffer strIn, strOut;
	strIn < wTable[0].dir < "\\" < worldIniFile;
	strOut < _patch2New2x2World < "\\" < worldIniFile;
	if(!CopyFile(strIn, strOut, FALSE)){
		return;// 0;
	}
	XBuffer tbuf;
	tbuf.init();
	tbuf <= 11;//2048
	SaveINIstringV(strOut, "Global Parameters", "Map Power X", tbuf) ;
	tbuf.init();
	tbuf <= 11;//2048
	SaveINIstringV(strOut, "Global Parameters", "Map Power Y", tbuf) ;

	///saveGeoDamPal();
	tbuf.init();
	tbuf < _patch2New2x2World < "\\" < worldGeoPalFile;
	XStream ff(tbuf, XS_OUT);
	ff.write(GeoPal, SIZE_GEO_PALETTE);//palBuf
	ff.close();
	tbuf.init();
	tbuf < _patch2New2x2World < "\\" < worldDamPalFile;
	XStream ff1(tbuf, XS_OUT);
	ff1.write(DamPal, SIZE_GEO_PALETTE);//palBuf
	ff1.close();
// Сохранение ключей палитры и функции гео слоя
	//f3d.saveVariable();
	const char * Section3DFParametrs="3DF Parameters";
	tbuf.init();
	tbuf <= f3d.kmx < " " <= f3d.kmy < " " <= f3d.kmz; 
	SaveINIstringV(strOut , Section3DFParametrs, "ScalingXYZ", tbuf) ;
	tbuf.init();
	tbuf <= f3d.currentFunction; 
	SaveINIstringV(strOut , Section3DFParametrs, "Function", tbuf) ;

#ifdef _SURMAP_
	//keyGeoPal.saveKeyGeoPal();
	char * strGeoPalparams="GeoPal Parameters";
	tbuf.init();
	tbuf <= keyGeoPal.numbers;
	SaveINIstringV(strOut, strGeoPalparams, "NumbersKey", tbuf) ;

	int i;
	tbuf.init();
	//Цветов на один больше чем keys
	//ВНИМАНИЕ ! Пробела в конце строки для записи данных в INI файл не должно быть !(иначе при каждой записи будут добавляться пробелы в конец строки(баг Windows))
	for(i=0; i<(keyGeoPal.numbers+1); i++) { tbuf< " " <= keyGeoPal.color[i].r < " " <= keyGeoPal.color[i].g < " " <= keyGeoPal.color[i].b; }
	SaveINIstringV(strOut, strGeoPalparams, "Colors", tbuf) ;

	tbuf.init();
	//ВНИМАНИЕ ! Пробела в конце строки для записи данных в INI файл не должно быть !(иначе при каждой записи будут добавляться пробелы в конец строки(баг Windows))
	for(i=0; i<keyGeoPal.numbers; i++) { tbuf < " " <= keyGeoPal.key[i] ; }
	SaveINIstringV(strOut, strGeoPalparams, "Keys",tbuf) ;
#endif


	sVmpHeader VmpHeader;
	//const char id[4]={'S','2','T','0'};
	//int i;
	XStream fo;
	tbuf.init();
	tbuf < _patch2New2x2World < "\\" < worldDataFileLinear;
	fo.open(tbuf/*"2xWorld"*/, XS_OUT);
	fo.seek(0,XS_BEG);
	//*(int*)VmpHeader.id = *(int*)id;
	VmpHeader.setID("S2T0");
	VmpHeader.XS=2048;
	VmpHeader.YS=2048;
	fo.write(&VmpHeader,sizeof(VmpHeader));

	int j;
	unsigned char dBuf[2048];
	//преобразование гео
	for(i=0; i<2048; i++){
		for(j=0; j<2048; j++){
			dBuf[j]=VxGBuf[i*2*H_SIZE + j*2];
		}
		fo.write(&dBuf[0], 2048);
	}

	//преобразование дам
	for(i=0; i<2048; i++){
		for(j=0; j<2048; j++){
			dBuf[j]=VxDBuf[i*H_SIZE*2 + j*2];
		}
		fo.write(&dBuf[0], 2048);
	}

	//преобразование atr
	for(i=0; i<2048; i++){
		for(j=0; j<2048; j++){
			dBuf[j]=AtrBuf[i*H_SIZE*2 + j*2];
		}
		fo.write(&dBuf[0], 2048);
	}

	//преобразование sur
	for(i=0; i<2048; i++){
		for(j=0; j<2048; j++){
			dBuf[j]=SurBuf[i*H_SIZE*2 + j*2];
		}
		fo.write(&dBuf[0], 2048);
	}
	fo.close();
}

void vrtMap::scalingHeighMap(int percent)
{
	float k=(float)percent/100.f;
	static int cnt = 0;
	unsigned int i,j;
	for(j = 0; j<V_SIZE; j++){
		for(i = 0; i<H_SIZE; i++){
			int offb=offsetBuf(i,j);
			if(VxDBuf[offb]==0){ //geo
				short v=SGetAlt(offb);
				v=round(v*k);
				if(v>MAX_VX_HEIGHT)v=MAX_VX_HEIGHT; if(v<0)v=0;
				SPutAltGeo(offb, v);
			}
			else { //dam
				short v=round((float)SGetAlt(offb)*k);
				short cv=round((float)VxGBuf[offb]*k);
				if(cv>MAX_VX_HEIGHT_WHOLE)cv=MAX_VX_HEIGHT_WHOLE; if(cv<0)cv=0;
				VxGBuf[offb]=cv;
				VxDBuf[offb]=v>>VX_FRACTION;
				AtrBuf[offb]=(v&VX_FRACTION_MASK) | (AtrBuf[offb]&=~VX_FRACTION_MASK);
			}

		}
	}
}

#endif

int counterDrawTile=0;
double timeDrawTile=0;
double minTimeDrawTile=10000;
double maxTimeDrawTile=0;
void vrtMap::drawTile(char* Texture,unsigned long pitch,int xstart,int ystart,int xend,int yend,int step)
{
	if(step!=1) return;
	double t=clockf();
	for(int y = ystart; y < yend; y += step)
	{
		DWORD* tx=(DWORD*)Texture;
		int yy=max(0,y);
		for (int x = xstart; x < xend; x += step)
		{
			int xx=max(0,x);
			DWORD color=vMap.getColor32(xx,yy)|0xFF000000;
			*tx = color;
			tx++;
		}
		Texture += pitch;
	}
	t=clockf()-t;
	counterDrawTile++;
	timeDrawTile+=t;
	if(t<minTimeDrawTile) minTimeDrawTile=t;
	if(t>maxTimeDrawTile) maxTimeDrawTile=t;
}
/*
struct statistic{
	~statistic(){
		XStream fout("!stat.txt",XS_OUT);
		fout < "AvrgTime=" <= (timeDrawTile/(double)counterDrawTile) < "\r\n";
		fout < "minTime=" <= minTimeDrawTile < "\r\n";
		fout < "maxTime=" <= maxTimeDrawTile < "\r\n";
		fout < "counter=" <= counterDrawTile < "\r\n";
	}
};
statistic rts;
*/

/*
void checkVMapPoint(void)
{
	int xm=1308;
	int ym=2384;
	unsigned short gatr=vMap.GABuf[vMap.offsetGBuf(xm>>kmGrid, ym>>kmGrid)];
	int i,j;
	for(i=0; i<4; i++){
		for(j=0; j<4; j++){
			unsigned char atr=vMap.GetAtr(xm+j, ym+i);
			unsigned char atr1=vMap.SGetAlt(xm+j, ym+i);
		}
	}
}
*/

bool vrtMap::IsCyrcleSurfaceLeveled(const Vect2i& center, const int radius){
	short xC=center.x>>kmGrid;
	short yC=center.y>>kmGrid;
	short rG=radius>>kmGrid;

	if(rG > MAX_RADIUS_CIRCLEARR){
		xassert(0&&"exceeding max radius in IsCyrcleSurfaceLeveled");
		rG=MAX_RADIUS_CIRCLEARR;
	}

	unsigned short a=0xffFF;

	for(int i = 0;i <= rG;i++){
		int maxr = maxRad[i];
		int* xx = xRad[i];
		int* yy = yRad[i];
		for(int j = 0;j < maxr; j++) {
			a&=GABuf[offsetGBufC(xC+xx[j], yC+yy[j])];
		}
	}
	if(a&GRIDAT_LEVELED) 
		return true;
	else 
		return false;
}

