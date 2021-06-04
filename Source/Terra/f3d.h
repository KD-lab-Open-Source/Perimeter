
#include "pnint.h"

#ifdef _TX3D_LIBRARY_
	#include <tx3d.hpp>
#endif

#define MAX_3D_FUNCTION 2


struct s_f3d {
	s_f3d(void);
	char* getNameFuction(int num);
	void setFunction(int num);
	
	typedef int (s_f3d::*PF)(int x, int y, int z);
	PF calcFunc;

#ifdef _TX3D_LIBRARY_
	unsigned char indexLattice[65536];
	tx3d::IndexedTexture3D *indexedTexture;
	tx3d::Vector3D calcPoint;
	~s_f3d(void);
#endif

	inline int calc(int x, int y, int z){
#if defined(_TX3D_LIBRARY_)
		calcPoint.x = x;
		calcPoint.y = y;
		calcPoint.z = (float)z * 0.03125f;//  /32.0f;
		return indexedTexture->getColorIndex(calcPoint);
#elif defined(_GEOTOOL_)
		return 0;
#else
//		//return ((*this).*calcFunc)(x,y,z);
		return (this ->* calcFunc)(x,y,z);
#endif
	};

	int calcMarble(int x, int y, int z);
	int calcTree(int x, int y, int z);
	void recalcWorld(void);
#ifdef _SURMAP_
	void calcSpecBuf(void);
	void calcSpecBufStr(int Y);
#endif
	void saveVariable(void);
	void loadVariable(void);

	//Таблица соделжащая названия и адреса функций 3D текстуры
	struct s_table {
		char * nameFunction;
		PF function;
	};
	s_table tableF[MAX_3D_FUNCTION];
	int currentFunction; //Номер текущей функции 3D текстуры

	void setKM(int _kmx, int _kmy, int _kmz){
		kmx=_kmx; kmy=_kmy; kmz=_kmz;
		dr_kmx=(1<<NOISE_FRACTION)/kmx;
		dr_kmy=(1<<NOISE_FRACTION)/kmy;
		dr_kmz=(1<<NOISE_FRACTION)/(kmz*(1<<VX_FRACTION));
	};
	//float kmx, kmy, kmz; //Коэф. масштабирования значений по X,Y,Z
	int kmx, kmy, kmz; //Коэф. масштабирования значений по X,Y,Z
	int dr_kmx, dr_kmy, dr_kmz;
	
};
extern s_f3d f3d;

#define MAX_KEY_GEO_PAL 255
struct sKeyGeoPal {
	struct sColor{
		unsigned char r;
		unsigned char g;
		unsigned char b;
	};
	sColor color[MAX_KEY_GEO_PAL+1];
	int key[MAX_KEY_GEO_PAL];
	int numbers;

	sKeyGeoPal(void){
		numbers=0;
		color[0].r=0; color[0].g=0; color[0].b=0;
	};
	int loadKeyGeoPal(void);
	void saveKeyGeoPal(void);
	void copyKeyGeoPal(sKeyGeoPal keyGeoPal);

};
extern struct sKeyGeoPal keyGeoPal;
