#ifndef __VMAP_H__
#define __VMAP_H__

#include "undoDispatcher.h"

#include "crc.h"

#include "list"
#include "vector"

#include "xmath.h"

#include "procedurMap.h"

extern int NOISE_AMPL;

//Структура определяющая границу кластера
struct pointB {
	short x,y;
};

struct sBorderDim {
	pointB* pp;
	int numbers;
	sBorderDim(void){
		numbers=0;
	}
	~sBorderDim(void){
		reInit(0);
	}
	void reInit(int num){
		if(numbers!=0){
			delete [] pp;
		}
		if(num!=0){
			pp=new pointB [num];
			numbers=num;
		}
	}
	void copy(sBorderDim & in){
		reInit(in.numbers);
		int i;
		for(i=0; i<in.numbers; i++){
			(pp+i)->x =(in.pp+i)->x;
			(pp+i)->y =(in.pp+i)->y;
		}
	}
};

struct sRect {
	short x, y;
	short dx, dy;

	sRect(){ x = y = dx = dy = 0; }
	sRect(short _x, short _y, short _dx, short _dy){ x = _x; y = _y; dx = _dx; dy = _dy; }
	void addBound(const Vect2i& v) { 
		if(dx){ 
			if(x > v.x) 
				x = v.x; 
			if(y > v.y) 
				y = v.y; 
			if(dx < v.x - x) 
				dx = v.x - x; 
			if(dy < v.y - y)
				dy = v.y - y;
		}
		else{
			x = v.x; 
			y = v.y;
			dx = 1;
			dy = 1;
		}
	}
	short x1() const { return x + dx; }
	short y1() const { return y + dy; }
};

//для работы с цветом
struct tColor {
	unsigned char r,g,b;
	unsigned char alpha;
};

//#define offsetAt (XS_Buf * YS_Buf*0)
//#define offsetClTr (XS_Buf * YS_Buf*1)
//#define offsetVx (XS_Buf * YS_Buf*2)
//#define offsetSpec (XS_Buf * YS_Buf*3)

#define kmGrid (2) // Это 2^2 - сетка 4x4
#define sizeCellGrid (1<<kmGrid)
#define GRIDAT_TALLER_HZEROPLAST (0x4) //Выше высоты зеропласта
#define GRIDAT_LEVELED (0x8)

//GRIDAT_ZEROPLAST - Во всех точках есть зеропласт
#define GRIDAT_ZEROPLAST (0x10)
//#define GRIDAT_ZEROPLASTEMPTY 0x20
#define GRIDAT_BUILDING (0x20)

//GRIDAT_ZEROPLAST_IS_PRESENT - Хотя-бы в одной точке есть зеропласт
#define GRIDAT_ZEROPLAST_IS_PRESENT (0x40)
//#define GRIDAT_TEMP_BIT 0x80
#define GRIDAT_BASE_OF_BUILDING_CORRUPT (0x80)
#define GRIDAT_MASK_CLUSTERID (0xFF00) // 8-мь старших бит
#define GRIDAT_STRIP_MASK_CLUSTERID (0x7F00) // 8-мь старших бит
#define GRIDAT_SHIFT_CLUSTERID (8)
#define GRIDAT_MASK_HARDNESS (0x3)

#define GRIDTST_ZEROPLAST(V) (V&GRIDAT_ZEROPLAST)
//#define GRIDTST_ZEROPLASTEMPTY(V) (V&GRIDAT_ZEROPLASTEMPTY)
#define GRIDTST_BUILDING(V) (V&GRIDAT_BUILDING)
#define GRIDTST_LEVELED(V) (V&GRIDAT_LEVELED)
#define GRIDTST_TALLER_HZEROPLAST(V) (V&GRIDAT_TALLER_HZEROPLAST)
#define GRIDTST_CLUSTERID(V) ((V&GRIDAT_MASK_CLUSTERID)>>8)

//Дополнительная сетка
//#define kmGridK 3 // Это 2^3 - сетка 8x8
//#define sizeCellGridK (1<<kmGridK)
//const int KronCritical_dh=3;//Разница высот для не проходимости

// Сетка отражения изменений на воксельной поверхности
#define kmGridChA (6) // 2^6 -сетка 64x64
#define sizeCellGridCA (1<<kmGridChA)

//Воксельное окно
const int MAX_XSIZE_VX_WINDOW=640; //Максимальный размер воксельного окна
//Расстояние от камеры
const int MIN_CAMERADZ_VX_WINDOW=-500;
const int MAX_CAMERADZ_VX_WINDOW=1500;


//Основная структура карты
struct vrtMap {

	enum { SVMPHEADER_ID_SIZE = 4 };

	struct sVmpHeader{
		char id[SVMPHEADER_ID_SIZE];
		int XS;
		int YS;
		int NX;
		int NY;
		sVmpHeader() {
			//id[0]='S'; id[1]='M'; id[2]='W'; id[3]='0';
		}
		void setID(const char* vmpID){
			for(int i=0; i< SVMPHEADER_ID_SIZE; i++){
				if(*vmpID!=0) {
					id[i]=*vmpID;
					vmpID++;
				}
				else id[i]=0;
			}
		}
		bool cmpID(const char* vmpID){
			for(int i=0; i< SVMPHEADER_ID_SIZE; i++){
				if(*vmpID!=id[i]) return false;
				if(*vmpID!=0) vmpID++;
				else return false;
			}
			return true;
		}
		
	};

	struct vrtWorld {
		char* name;
		char* dir;
		
		vrtWorld(void){ name = dir = 0; }
		~vrtWorld(void){
			if(name) free(name);
			if(dir) free(dir);
		}
	};

	struct PrmFile {
		char* buf;
		int len;
		int index;

		void init(const char* name);
		char* getAtom(void);
		void finit(void){ delete buf; }
	};

	static char* worldDataFileLinear;
	static char* worldDataFile;
	static char* worldDataFileSection;
	static char* worldIniFile;
	//static char* worldParamZPIniFile;
	static char* worldNetDataFile;
	static char* worldBuildScenarioFile;
	static char* worldGeoPalFile;
	static char* worldDamPalFile;
	static char* worldLeveledTextureFile;
	static char* worldHardnessFile;

	static char* worldRGBCache;


	unsigned char* changedT;

	list<sRect> changedAreas;
	unsigned char* gridChAreas;
	unsigned char* gridChAreas2;

	list<sRect> renderAreas;

///////////////////////////////////////////////////////////////////
	list<sPreChangedArea> preCAs;
	list<sPreChangedArea>::iterator curPreCA;
	void UndoDispatcher_PutPreChangedArea(int xL, int yT, int xR, int yB);
	void UndoDispatcher_Undo(void);
	void UndoDispatcher_Redo(void);
	void UndoDispatcher_KillAllUndo(void);
	bool UndoDispatcher_IsUndoExist(void);
	bool UndoDispatcher_IsRedoExist(void);
///////////////////////////////////////////////////////////////////
	//Procedural map
	bool flag_record_operation;
	vector<ElementPMO> containerPMO;

	void playPMOperation(void);
	template<class Archive>
	void serialize(Archive& ar) {
		ar & WRAP_OBJECT(containerPMO);
		ar & WRAP_OBJECT(H_SIZE_POWER);
		ar & WRAP_OBJECT(V_SIZE_POWER);
		ar & WRAP_OBJECT(GEONET_POWER);
		ar & WRAP_OBJECT(WPART_POWER);
		ar & WRAP_OBJECT(MINSQUARE_POWER);
		ar & WRAP_OBJECT(hZeroPlast);

		unsigned char R_ZL=colZeroPlast.r;
		unsigned char G_ZL=colZeroPlast.g;
		unsigned char B_ZL=colZeroPlast.b;
		unsigned char A_ZL=colZeroPlast.alpha;
		ar & WRAP_OBJECT(R_ZL);
		ar & WRAP_OBJECT(G_ZL);
		ar & WRAP_OBJECT(B_ZL);
		ar & WRAP_OBJECT(A_ZL);
		colZeroPlast.r=R_ZL;
		colZeroPlast.g=G_ZL;
		colZeroPlast.b=B_ZL;
		colZeroPlast.alpha=A_ZL;

		ar & WRAP_OBJECT(GeonetMESH);
		ar & WRAP_OBJECT(NOISE_AMPL);

	}
///////////////////////////////////////////////////////////////////

	int XS_Buf;
	int YS_Buf;

	unsigned int H_SIZE_POWER;
	unsigned int H_SIZE;
	unsigned int clip_mask_x;

	unsigned int V_SIZE_POWER;
	unsigned int V_SIZE;
	unsigned int clip_mask_y;

	int hCamera,fCamera;

	unsigned char* Buf; 

	unsigned char* VxGBuf; //гео VxBuf
	unsigned char* VxDBuf; // насыпной(Dam) VxBuf
	unsigned char* WVxBuf; // текущий VxBuf (или VxGBuf или VxDBuf)
	unsigned char* AtrBuf;
	unsigned char* SurBuf;
	unsigned char* RnrBuf;

	unsigned long* SupBuf;


	unsigned char* LvdTex;
	unsigned int LvdTex_clip_mask_x;
	unsigned int LvdTex_clip_mask_y;
	unsigned char LvdTex_X_SIZE_POWER;
	unsigned char LvdTex_Y_SIZE_POWER;
	unsigned int LvdTex_spec_clip_mask_y;
	unsigned char LvdTex_SPEC_SHIFT_X;
	unsigned char LvdTex_SPEC_SHIFT_Y;

	unsigned char* GVBuf; //Сетка вокселей
	unsigned short* GABuf; //сетка атрибутов
	unsigned int GH_SIZE;
	unsigned int clip_mask_x_g;
	unsigned int GV_SIZE;
	unsigned int clip_mask_y_g;
	unsigned char hZeroPlast;
	tColor colZeroPlast;

	void initGrid(void);
	void loadGrid(XStream & ff);
	void saveGrid(XStream & ff);
	void recalcArea2Grid(int xl, int yt, int xr, int yb );
	void loadHardness2Grid(void);
	void loadHardness(void);
	void saveHardness(void);
	unsigned char getHardness(int xg, int yg){
		return GABuf[offsetGBuf(xg, yg)]&GRIDAT_MASK_HARDNESS;
	}
	void setHardness(int xg, int yg, unsigned char hrd){
		GABuf[offsetGBuf(xg, yg)]=(hrd&GRIDAT_MASK_HARDNESS) | (GABuf[offsetGBuf(xg, yg)]&(~GRIDAT_MASK_HARDNESS)) ;
	}

	void clearGridChangedAreas(void);
	void updateGridChangedAreas2(void);


	unsigned short Sur2Col[MAX_SURFACE_TYPE][MAX_SURFACE_LIGHTING*2]; //2 это два слоя Dam и ZP
	unsigned short SurZP2Col[MAX_SURFACE_LIGHTING*2][MAX_SURFACE_TYPE];
//	unsigned short SurZPE2Col[MAX_SURFACE_LIGHTING*2][MAX_SURFACE_TYPE];
	unsigned short Tex2Col[MAX_SURFACE_LIGHTING*2][MAX_SURFACE_TYPE];
	unsigned short TexZP2Col[MAX_SURFACE_LIGHTING*2][MAX_SURFACE_TYPE];

	unsigned int Sur2Col32[MAX_SURFACE_TYPE][MAX_SURFACE_LIGHTING*2]; //2 это два слоя Dam и ZP
	unsigned int SurZP2Col32[MAX_SURFACE_LIGHTING*2][MAX_SURFACE_TYPE];
	//unsigned int SurZPE2Col32[MAX_SURFACE_LIGHTING*2][MAX_SURFACE_TYPE];
	unsigned int Tex2Col32[MAX_SURFACE_LIGHTING*2][MAX_SURFACE_TYPE];
	unsigned int TexZP2Col32[MAX_SURFACE_LIGHTING*2][MAX_SURFACE_TYPE];

	unsigned char GeoPal[SIZE_GEO_PALETTE];
	unsigned char DamPal[SIZE_DAM_PALETTE];
	unsigned char TexPal[SIZE_DAM_PALETTE];

	int FilterMinHeight;
	int FilterMaxHeight;
#ifdef _SURMAP_
	int currentDamTerrain;
	unsigned char FilterGeoTerrain[MAX_GEO_SURFACE_TYPE];
	unsigned char FilterDamTerrain[MAX_DAM_SURFACE_TYPE];
	int putTgaComplete2AllMap(void);
	void wrldShot(void);
	void superWrldShot(int k);
	void wrldShotOutLine(void);
	void wrldShotMapHeight(void);
	void FlipWorldH(void);
	void FlipWorldV(void);
	void RotateWorldP90(void);
	void RotateWorldM90(void);
	void saveMapWithOtherSize(void);//Удвоенный размер
	//void saveMapWithOtherSize4To2(void);//Уменьшенный вдвое размер
	void saveMapWithOtherSize4To2(const char* _patch2New2x2World);

	void scalingHeighMap(int percent); //Уменьшать увеличивать общий масштаб высоты карты

	unsigned char status_ShowHideGeo;
	unsigned char * SpecSurBuf;
	bool ShadowingOnEdges;

	short clipXLeft, clipXRight, clipYTop, clipYBottom; 
#endif

	PrmFile prmFile;
	int maxWorld,cWorld;
	vrtWorld* wTable;


	XStream fmap,pmap;//,kmap
	char* dirWorldPrm;



	vrtMap(void);
	~vrtMap(void);

	//Функции загрузки 
	void allocMem4Buf(void);
	void releaseMem4Buf(void);

	void allocChAreaBuf();
	void releaseChAreaBuf();

	void loadGeoDamPal(void);
	void saveGeoDamPal(void);

	void save3BufOnly(void);
	void save3Buf(void);
	void restore3Buf(void);
#ifdef _SURMAP_
	bool saveAllWorldAs(const char* _patch2world);
#endif
	bool worldChanged;
	bool loadGameMap(const char* fname, bool flag_FastLoad=1);
	bool saveGameMap(const char* fname);

	void getWorldList(vector<const char*>& world_list) { world_list.clear(); for(int i = 0; i < maxWorld; i++) world_list.push_back(wTable[i].name); }
	int getWorldID(const char* world_name) { for(int i = 0; i < maxWorld; i++) if(stricmp(wTable[i].name, world_name) == 0) return i; return -1; }
	const char* getWorldName(int world_id) { xassert(world_id >= 0 && world_id < maxWorld); return wTable[world_id].name; }

	enum eSurfaceMode{
		Geologic,
		Damming
	};
	unsigned char veryLightDam;
	void convertPal2TableSurCol(unsigned char palBuf[SIZE_GEO_PALETTE],eSurfaceMode SurfaceMode);
	void convertPal2TableTexCol(void);
	void delLeveledTexture(void);
	int loadLeveledTexture(void);

	void scaling16(int cx,int cy,int xc,int yc,int xside,int yside, unsigned short* GRAF_BUF, int GB_MAXX, int GB_MAXY);
	void scaling32(int cx,int cy,int xc,int yc,int xside,int yside, unsigned char* GRAF_BUF, int GB_MAXX, int GB_MAXY);
//	void scaling16HC(int XSrcSize,int cx,int cy,int xc,int yc,int xside,int yside);
	void viewV(int XSrcSize,int cx,int cy,int xc,int yc,int xside,int yside);

	//Поворот плоскости
	float turnAngleAroundZ;//=0;//-3.141592654f/2;
	//Фокус
	int focusC;//=600;
	//Расстояние от камеры
	int cameraDZ;//=100;
	//Угол наклона камеры
	int cameraAngleLean;//=140;
	void viewV2D(int cx,int cy,int xc,int yc,int xside,int yside, unsigned short* GRAF_BUF, int GB_MAXX, int GB_MAXY);
	void scaling162D(int XSrcSize,int cx,int cy,int xc,int yc,int xside,int yside);

	void checkAndRecover(void);
//	void putTga2Surface(void);

	void delAllZL(void);

	void analyzeINI(const char* name);
	void setupGeneralVariable(void);

	unsigned char GEONET_POWER;
	unsigned char QUANT;
	unsigned int WPART_POWER;
	unsigned int part_map_size_y;
	unsigned int part_map_size;
	int MINSQUARE_POWER;
	int MINSQUARE;
	unsigned int net_size;
	unsigned int PART_MAX;
	int GeonetMESH;

	//WorldGen
	void r_net_init(void);
	void m_net_init(void);
	void rough_init(void);
	void generate_roughness_map(void);
	void map_init(void);
	void generate_alt_map(void);

	void worldPrepare(void);
	void worldClean(void);
	void worldFree(void);
	void worldInit(void);
	void worldRelease(void);
	void generate_noise(void);
	void head_vmpWrite(XStream& fmap);
	void partWrite(XStream& ff,int mode, int Stage);
	void worldPreWrite(XStream& ff);
	void LoadPP(void);
	void SetPP(int n);
	void LoadVPR(int ind = 0);
	void SaveVPR(int ind = 0);
	int buildWorld(void);
	void GeoRecalc(int n, int level, int delta);
	void GeoPoint(int x,int y,int level,int delta,int mode);
	void GeoSetZone(int x,int y,int rad,int level,int delta);

	void landPrepare(void);
	void landRelease(void);


	enum eVxBufWorkMode{
		GEOONLY=0,
		GEO=1,
		DAM=2,
		GEOorDAM=3,
		GEOiDAM=4
	};
	eVxBufWorkMode VxBufWorkMode;


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//	PUBLIC fuction для использования 
	int getWorld_H_SIZE(int idxWorld);
	int getWorld_V_SIZE(int idxWorld);

#ifdef _SURMAP_
	void prepare();
	void selectUsedWorld(char* _patch2WorldIniFile);
#elif _PERIMETER_
	void prepare(char* name);
	void selectUsedWorld(int nWorld);
#endif
	void ShadowControl(bool shadow);
	void fullLoad(bool flag_fastLoad=0);
	void WorldRender(void);
	void renderQuant(void);

	void createWorld(int hSizePower, int vSizePower);
	void newLoad(const char* dirName);
	void newSave(const char* dirName);


	bool IsChanged(void) { return worldChanged;}

	bool IsHardWCC(Vect2i crd){
		unsigned short p = GABuf[offsetGBuf(w2mClampX(crd.x), w2mClampY(crd.y))];
		if((p & GRIDAT_MASK_HARDNESS) == GRIDAT_MASK_HARDNESS) return 0;
		else return 1;
	}

	bool IsFullLoad(void) { return (VxGBuf!=0);}

	bool IsSurfaceLeveled(const Vect2i& leftTop, const Vect2i& rightDown){
		short xL=leftTop.x>>kmGrid;
		short xR=rightDown.x>>kmGrid;
		short yT=leftTop.y>>kmGrid;
		short yD=rightDown.y>>kmGrid;
		short x,y;
		unsigned short a=0xffFF;
		for(y=yT; y<=yD; y++){
			int offGY=offsetGBuf(0, y);
			for(x=xL; x<=xR; x++){
				a&=GABuf[offGY+x];
			}
		}
		if(a&GRIDAT_LEVELED) 
			return true;
		else 
			return false;
	}

	bool IsCyrcleSurfaceLeveled(const Vect2i& center, const int radius);


	void generateChAreasInformation(XBuffer& out);
	unsigned int getWorldCRC(void);
	void compareChAreasInformation(unsigned char* pFirstCAI, unsigned char* pSecondCAI, XBuffer& textOut, XBuffer& binOut);
#ifdef _PERIMETER_
	void displayChAreas(unsigned char* pd, unsigned int dsize);
#endif
	unsigned int getGridCRC(bool fullGrid, int cnt=0, unsigned int beginCRC=startCRC32);
	unsigned int getChAreasInformationCRC();

	int XCYCL(int x)	{ return (x) & clip_mask_x; }
	int YCYCL(int y)	{ return (y) & clip_mask_y; }

	int XCYCLG(int x)	{ return (x) & clip_mask_x_g; }
	int YCYCLG(int y)	{ return (y) & clip_mask_y_g; }

	int w2m(int x) { return x >> kmGrid; } // World to map
	int m2w(int x) { return x << kmGrid; } // Map to world

	int w2mClampX(int x) { return w2m(clamp(x, 0, (int)H_SIZE - 1)); } // Clamped
	int w2mClampY(int y) { return w2m(clamp(y, 0, (int)V_SIZE - 1)); }
	
	int m2wHalf(int x) { return m2w(x) + (1 << (kmGrid - 1)); } // Map to world plus half
		
	Vect2i w2m(const Vect2i& v) { return Vect2i(w2m(v.x), w2m(v.y)); }
	Vect2i m2w(const Vect2i& v) { return Vect2i(m2w(v.x), m2w(v.y)); }

	//function для работы с буфером
	//Базовая функция вычисления смещения в буфферах
	int offsetBuf(int x, int y) {
		return y*XS_Buf+x;
	}
	int offsetBufC(int x, int y) {
		return YCYCL(y)*XS_Buf+XCYCL(x);
	}
	int offsetGBuf(int x, int y) {
		return y*GH_SIZE+x;
	}
	int offsetGBufC(int x, int y) {
		return YCYCLG(y)*GH_SIZE+XCYCLG(x);
	}

	int offsetGBufWorldC(int x, int y) {
		return YCYCLG(y >> kmGrid)*GH_SIZE+XCYCLG(x >> kmGrid);
	}

	int offsetBuf2offsetGBuf(int offBuf){
		return ( (offBuf>>kmGrid)&clip_mask_x_g ) | ( (offBuf>>(2*kmGrid))&(~clip_mask_x_g) );
			//clip_mask_y_g
	}

	Vect3f getVect3f(int x, int y) { return Vect3f((float)x, (float)y, (float)(GetAlt(XCYCL(x), YCYCL(y)) >> VX_FRACTION)); }

	unsigned char getVoxel(int x, int y) { 
		return GVBuf[offsetGBuf(x,y)];
	}
	unsigned char getVoxelC(int x, int y) { 
		return GVBuf[offsetGBufC(x,y)];
	}
	unsigned char getVoxelW(int x, int y) { 
		return getVoxelC(w2m(x), w2m(y));
	}


	bool checkZeroLayer(int x, int y){
		if(GABuf[offsetGBuf(x>>kmGrid, y>>kmGrid)]&GRIDAT_ZEROPLAST) return 1;
		else return 0;
	}

	// Возвращает приблизительную высоту вычисляющуюся по сетке
	int GetApproxAlt(int x, int y){
		return GVBuf[offsetGBuf(x>>kmGrid, y>>kmGrid)];
	}

	void pointDamagingBuildings(int _x, int _y) {
		int x=_x>>kmGrid;
		int y=_y>>kmGrid;
		int offG=offsetGBuf(XCYCLG(x), YCYCLG(y));
		if(GABuf[offG]&GRIDAT_BUILDING) GABuf[offG]|=GRIDAT_BASE_OF_BUILDING_CORRUPT;
	}



	unsigned short getColor(int x, int y){
		return getColor(offsetBuf(x,y));
	}
	unsigned short getColor(int off){
		unsigned short col;
		//unsigned short col=Sur2Col[SurBuf[off]][RnrBuf[off]];
		if(AtrBuf[off] &At_LEVELED) {
			if( (AtrBuf[off]&At_ZPMASK) == At_ZEROPLAST){
				col = SurZP2Col[RnrBuf[off]][SurBuf[off]];
			}
			else {
				unsigned int texOff=off&LvdTex_clip_mask_x;
				texOff|=(off>>LvdTex_SPEC_SHIFT_X)&LvdTex_spec_clip_mask_y;
				col = Tex2Col[RnrBuf[off]][LvdTex[texOff]];//SurZPE2Col[RnrBuf[off]][SurBuf[off]];
			}
		}
		else{
			col=Sur2Col[SurBuf[off]][RnrBuf[off]];
		}

		return col;
	}

	bool flag_ShowDbgInfo;
	bool* pTempArray;//Массив для показа несовпадающих областе (при игре по сети)
	bool flag_ShowHardness;
	void toShowHardness(bool flag){ flag_ShowHardness=flag; }
	void toShowDbgInfo(bool flag){ flag_ShowDbgInfo=flag; }
	bool IsShowDbgInfo(void){ return flag_ShowDbgInfo; }
	unsigned int getColor32(int x, int y){
		int off=offsetBuf(x,y);
		return getColor32(off);
	}
	unsigned int getColor32(int off){
		unsigned int col32;
		//unsigned short col=Sur2Col[SurBuf[off]][RnrBuf[off]];
		if(AtrBuf[off] &At_LEVELED){
			unsigned int texOff=off&LvdTex_clip_mask_x;
			texOff|=(off>>LvdTex_SPEC_SHIFT_X)&LvdTex_spec_clip_mask_y;
			const unsigned int mask=0x00FEFEFF;
			if( (AtrBuf[off]&At_ZPMASK) == At_ZEROPLAST){
///				col32=Sur2Col32[SurBuf[off]][RnrBuf[off]];
				col32 = SurZP2Col32[RnrBuf[off]][SurBuf[off]];
///				col32=TexZP2Col32[RnrBuf[off]][LvdTex[texOff]]; //col32 = SurZPE2Col32[RnrBuf[off]][SurBuf[off]];
///				col32= ( (col32&mask) + (SurZP2Col32[RnrBuf[off]][SurBuf[off]]&mask) ) >> 1;
			}
			else{
				col32=Tex2Col32[RnrBuf[off]][LvdTex[texOff]]; //col32 = SurZPE2Col32[RnrBuf[off]][SurBuf[off]];
				col32= ( (col32&mask) + (Sur2Col32[SurBuf[off]][RnrBuf[off]]&mask) ) >> 1;
			}
		}
		else{
			col32=Sur2Col32[SurBuf[off]][RnrBuf[off]];
		}
#ifndef _FINAL_VERSION_
		if(flag_ShowDbgInfo){
			if( (GABuf[offsetBuf2offsetGBuf(off)]&GRIDAT_BUILDING) != 0) col32|=0xFF;
			if( (GABuf[offsetBuf2offsetGBuf(off)]&GRIDAT_BASE_OF_BUILDING_CORRUPT) != 0) col32|=0xFF0000;
			//if( (GABuf[offsetBuf2offsetGBuf(off)]&GRIDAT_TALLER_HZEROPLAST) != 0) col32|=0xFF00;
			//if( (GABuf[offsetBuf2offsetGBuf(off)]&GRIDAT_LEVELED ) != 0) col32|=0x00ff00;
			//if( GABuf[offsetBuf2offsetGBuf(off)]&GRIDAT_MASK_HARDNESS == GRIDAT_MASK_HARDNESS ) col32|=0xFF00;
		}
		if(flag_ShowHardness){
			const unsigned int mask=0x00FEFEFF;
			if(!(GABuf[offsetBuf2offsetGBuf(off)] & GRIDAT_MASK_HARDNESS)) col32=((0xFF0000&mask) + (col32&mask))>>1;
			if(pTempArray){
				const unsigned int clmask=(H_SIZE>>kmGridChA)-1;
				unsigned int noff=( (off>>kmGridChA)&clmask ) | ( (off>>(2*kmGridChA))&(~clmask) );
				if(pTempArray[noff]) col32=((0x00FF00&mask) + (col32&mask))>>1;
			}
		}
#endif
		return col32;
	}
	void drawTile(char* Texture, unsigned long pitch,int xstart,int ystart,int xend,int yend,int step);
	bool TstZP(int off){
		if( (AtrBuf[off]&At_ZPMASK)==At_ZEROPLAST ) return 1;
		else return 0;
	}

	bool leveled(int off){
		if(AtrBuf[off]&At_LEVELED) return 1;
		else return 0;
	}

	enum eStatusZP{
		NOT_PRESENT=0,
		PRESENT=1,
		FULL_PRESENT=2
	};
	eStatusZP TestStatusZP(int xL, int yT, int xR, int yD){
		int mask=(1<<kmGrid)-1;
		xL>>=kmGrid; 
		if(xR&mask)xR=(xR>>kmGrid)+1;
		else xR=(xR>>kmGrid);
		xR=XCYCLG(xR);
		yT>>=kmGrid; 
		if(yD&mask)yD=(yD>>kmGrid)+1;
		else yD=(yD>>kmGrid);
		yD=YCYCLG(yD);
		int i,j;
		unsigned char fl_PRESENT=0;
		unsigned char fl_FULL_PRESENT=1;
		for(i=yT; i!=yD; i=YCYCLG(i+1)){
			for(j=xL; j!=xR; j=XCYCLG(j+1)){
				unsigned short atr=GABuf[offsetGBuf(j,i)];
				if(atr&(GRIDAT_ZEROPLAST_IS_PRESENT)) fl_PRESENT |=1;
				if(!(atr&(GRIDAT_ZEROPLAST/*|GRIDAT_ZEROPLASTEMPTY*/))) fl_FULL_PRESENT =0;
			}
		}
		if(fl_FULL_PRESENT==1)return FULL_PRESENT;
		if(fl_PRESENT==1) return PRESENT;
		return NOT_PRESENT;
	}

	////////////////////////////////////////////////////
	unsigned char GetGeoType(int offset, int h);
	unsigned char GetGeoType(int x, int y, int h) {
		return GetGeoType(offsetBuf(x,y), h); // (x+y)%
	}
	//..................................................

	///////////////////////////////////////
	unsigned char GetTer(int offset) {
		return (SurBuf[offset]);
	}
	unsigned char GetTer(int x,int y) {
		return GetTer(offsetBuf(x,y));
	}
	//......................................

	////////////////////////////////////////
	void SetTer(int offset, unsigned char newtype) {
		SurBuf[offset] = newtype;
	}
	void SetTer(int x,int y, unsigned char newtype) {
		SetTer(offsetBuf(x,y),newtype);
	}
	//.......................................

	unsigned char GetAtr(int x,int y) {
		return AtrBuf[y*XS_Buf+x];
	}
	void SetAtr(int x,int y, unsigned char newat) {
		AtrBuf[y*XS_Buf+x] = (AtrBuf[y*XS_Buf+x]) | newat;
	}

	void ClearAllZP(int x, int y){
		AtrBuf[offsetBuf(x,y)] = (AtrBuf[offsetBuf(x,y)]) & (~At_NOTPURESURFACE);
	}

	int tstMinMaxVox(int h);
	// НизкоУровневые функции работы с поверхностью GetAltGeo GetAltDam
	//Высота вокселя складывается из целой части(в Vx(G|D)Buf) и дробной (в AtrBuf)
	/////////// FUNCTION GetAltGeo ///////////
	unsigned short GetAltGeo(int offset) {
		if(VxDBuf[offset]==0) return ( (VxGBuf[offset]<<VX_FRACTION)|(AtrBuf[offset]&VX_FRACTION_MASK) );
		else return (VxGBuf[offset]<<VX_FRACTION);
	}
	unsigned short GetAltGeo(int x,int y) {
		return GetAltGeo(offsetBuf(x,y));
	}
	//.........................................
	/////////// FUNCTION GetAltDam ///////////
	unsigned short GetAltDam(int offset) {
		int whole=(VxDBuf[offset]);
		if(whole) return ( (whole<<VX_FRACTION)|(AtrBuf[offset]&VX_FRACTION_MASK) );
		else return 0; //Если нет целой части Dam-а, то тогда возвращается 0
		//else return GetAltGeo(offset); //Если нет целой части Dam-а, то тогда возвращается Geo слой
	}
	unsigned short GetAltDam(int x,int y) {
		return GetAltDam(offsetBuf(x,y));
	}
	//.........................................
	//Высокоуровневая функция работы с поверхностью GetAlt
	/////////// FUNCTION GetAlt ///////////
	unsigned short GetAlt(int offset) {
		unsigned short V;
		switch(VxBufWorkMode){
		case GEOONLY:
			V=GetAltGeo(offset);
			break;
		case GEO:
			//V=GetAltGeo(offset);
			V=SGetAlt(offset);
			break;
		case DAM:
			//V=GetAltDam(offset);
			V=SGetAlt(offset);
			break;
		case GEOorDAM:
		case GEOiDAM:
			//V=GetAltDam(offset);
			///if(!V) V=GetAltGeo(offset);
			V=SGetAlt(offset);
			break;
		}
		return V;
	}
	unsigned short GetAlt(int x,int y) {
		return GetAlt(offsetBuf(x,y));
	}
	//.......................................
	unsigned short GetAltC(int x,int y){ return GetAlt(XCYCL(x), YCYCL(y)); }


	//........................................
	/////////// FUNCTION Simple GetAlt ///////////
	unsigned short SGetAlt(int offset) {
		unsigned short whole=(VxDBuf[offset]);
		unsigned char fraction=(AtrBuf[offset]&VX_FRACTION_MASK);
		if(whole) return ( (whole<<VX_FRACTION)|fraction);
		else return ((unsigned short)(VxGBuf[offset]<<VX_FRACTION)|fraction);//Если нет целой части Dam-а, то тогда возвращается Geo слой
	}
	unsigned short SGetAlt(int x,int y) {
		return SGetAlt(offsetBuf(x,y));
	}
	//........................................


	// НизкоУровневые функции работы с поверхностью PutAltGeo PutAltDam 
	//Высота вокселя складывается из целой части(в Vx(G|D)Buf) и дробной (в AtrBuf)
	/////////// FUNCTION PutAltGeo ///////////
	void PutAltGeo(int offset, int V) {
		VxGBuf[offset]=V >>VX_FRACTION;
		if(VxDBuf[offset] <= V >>VX_FRACTION){ //Если Гео слой достиг Dam слоя
			AtrBuf[offset]=(V &VX_FRACTION_MASK) | (AtrBuf[offset]&=~VX_FRACTION_MASK);
			SetTer(offset,GetGeoType(offset,V));
			VxDBuf[offset]=0; //Dam слой становится равный 0
		}
	}
	void PutAltGeo(int x,int y, int V) {
		PutAltGeo(offsetBuf(x,y),V);
	}
	//.........................................

	// ВНИМАНИЕ PutAltDam устанавливает высоту Dam
	/////////// FUNCTION PutAltDam ///////////
	int PutAltDam(int offset, int V) {
		if(VxGBuf[offset] > V >>VX_FRACTION){ //Если Гео слой достиг Dam слоя //=
			SetTer(offset,GetGeoType(offset,V));
			VxDBuf[offset]=0; //Dam слой становится равный 0
			return 0; //Dam слой не установился
		}
		else {
			VxDBuf[offset]=V >>VX_FRACTION;
			AtrBuf[offset]=(V &VX_FRACTION_MASK) | (AtrBuf[offset]&=~VX_FRACTION_MASK);
			//SetTer(offset, TgaBuf[offset]);
			return 1; //Dam слой успешно установился
		}
	}
	void PutAltDam(int x,int y, int V) {
		PutAltDam(offsetBuf(x,y),V);
	}
	//.........................................
	//Высокоуровневая функция работы с поверхностью PutAlt
	/////////// FUNCTION PutAlt ///////////
	void PutAlt(int offset,int V) {
		switch(VxBufWorkMode){
			case GEO:
				PutAltGeo(offset,V);
				break;
			case DAM:
				//Возможно проверка не нужна т.к. уже должно быть известно на какую высоту насыпать
				//возможна ошибка при 
				//if((GetAltGeo(offset)>>VX_FRACTION) < (V>>VX_FRACTION)) 
				PutAltDam(offset,V);
				break;
			case GEOorDAM:
			case GEOiDAM:
				if((GetAltGeo(offset)>>VX_FRACTION) < (V>>VX_FRACTION)) PutAltDam(offset,V);
				else {
					PutAltDam(offset,0); //если высота ниже гео слоя то насыпь устанавливается в 0
					PutAltGeo(offset,V); 
				}
				break;
		}
	}
	void PutAlt(int x,int y,int V) {
		PutAlt(offsetBuf(x,y),V);
	}


	//........................................
	/////////// FUNCTION Simple PutAltGeo ///////////
	void SPutAltGeo(int offset, int V) {
		VxGBuf[offset]=V >>VX_FRACTION;
		AtrBuf[offset]=(V &VX_FRACTION_MASK) | (AtrBuf[offset]&=~(VX_FRACTION_MASK|At_NOTPURESURFACE));
	}
	void SPutAltGeo(int x,int y, int V) {
		SPutAltGeo(offsetBuf(x,y),V);
	}
	//.........................................
	/////////// FUNCTION Simple PutAltDam ///////////
	void SPutAltDam(int offset, int V) {
		VxDBuf[offset]=V >>VX_FRACTION;
		AtrBuf[offset]=(V &VX_FRACTION_MASK) | (AtrBuf[offset]&=~(VX_FRACTION_MASK|At_NOTPURESURFACE));
	}
	void SPutAltDam(int x,int y, int V) {
		SPutAltDam(offsetBuf(x,y),V);
	}
	//.........................................
	void SPutAlt(int offset, int V) {
		if(VxDBuf[offset]==0) VxGBuf[offset]=V>>VX_FRACTION;
		else VxDBuf[offset]=V>>VX_FRACTION;
		AtrBuf[offset]=(V &VX_FRACTION_MASK) | (AtrBuf[offset]&=~VX_FRACTION_MASK);
	}
	void SPutAlt(int x,int y, int V) {
		SPutAlt(offsetBuf(x,y),V);
	}
	void SPutAltAndClearZL(int offset, int V) {
		if(VxDBuf[offset]==0) VxGBuf[offset]=V>>VX_FRACTION;
		else VxDBuf[offset]=V>>VX_FRACTION;
		AtrBuf[offset]=(V &VX_FRACTION_MASK) | (AtrBuf[offset]&=~(VX_FRACTION_MASK|At_NOTPURESURFACE));
	}

	void SDig(int x, int y, int dv){
		unsigned int off=offsetBuf(x, y);
		int v;
		if(VxDBuf==0){
			v=GetAltGeo(off);
			v-=dv;
			if(v<0)v=0;
			SPutAltGeo(off, v);
		}
		else{
			v=GetAltDam(off);
			int vg=GetAltGeo(off);
			v-=dv;
			if(v<vg){
				if(v<0)v=0;
				SPutAltGeo(off, v);
				VxDBuf[off]=0;
			}
			else{
				if(v<0)v=0;
				SPutAltDam(off,v);
			}
		}
	}


	void voxSet(int x,int y,int delta,int terrain=-1);

	void deltaZone(sToolzerPMO& var);//(int x,int y,int rad,int smth,int dh,int smode,int eql);
	void squareDeltaZone(sSquareToolzerPMO& var);//(int x,int y,int rad,int smth,int dh,int smode,int eql);
	void gaussFilter(int x,int y,int rad, double filter_scaling);
	void squareGaussFilter(int _x,int _y,int _rad, double _filter_scaling);

	void AllworldGaussFilter(double _filter_scaling);

	void RenderRegStr(int Yh,int Yd);
	void regRender(int LowX,int LowY,int HiX,int HiY,int changed = 1);
	void regRender(const sRect& rect, int changed = 1) { regRender(rect.x, rect.y, rect.x1(), rect.y1(), changed); }
	int renderBox(int LowX,int LowY,int HiX,int HiY, int changed);

	void RenderPrepare1(void);
	int RenderStr(int XL, int Y, int dx);
	void RenderStr(int Y);

	void calc_Flood(int x0,int y0,int level,int ttype,int ti = 0);
	void WORLD_Drift(int x0,int y0,int level,int ttype);
protected:
	bool shadow_control;
};

	
const char* GetTargetName(const char* name);
const char* GetTargetName(int numWorld, const char* name);
const char* safeGetTargetName(int numWorld, const char* name);
bool isWorldIDValid(int worldID);
extern void geoGeneration(sGeoPMO& var);


extern vrtMap vMap;



//Работа с INI Файлом
extern char* GetINIstringV(const char* iniFile,const char* section,const char* key);
extern void SaveINIstringV(const char* iniFile,const char* section,const char* key,const char* var);

extern unsigned char* convert_vox2vid(int vox, char* buf);
extern int convert_vid2vox(char* buf);

#ifdef _SURMAP_
const unsigned int SLT_SIZE=512;
const unsigned int SLT_05SIZE=SLT_SIZE/2;
extern unsigned char SLightTable[SLT_SIZE];
#endif

#endif //__VMAP_H__