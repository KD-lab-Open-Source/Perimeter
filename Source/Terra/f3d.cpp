#include "stdafxTr.h"
#include <fstream>

#include "pnint.h"
#ifdef _TX3D_LIBRARY_
	#include <Texture3D.hpp>
#endif
s_f3d f3d;

char * txt_empty ="not Texture";
char * txt_MarbleTexture = "Marble Texture";
char * txt_TreeTexture   = "Tree Texture";
s_f3d::s_f3d(void)
{
	int i;
	for(i=0; i<MAX_3D_FUNCTION; i++) {
		tableF[i].nameFunction=txt_empty;
		tableF[i].function=&s_f3d::calcMarble;
	}
	tableF[0].nameFunction=txt_MarbleTexture;
	tableF[0].function=&s_f3d::calcMarble;
	tableF[1].nameFunction=txt_TreeTexture;
	tableF[1].function=&s_f3d::calcTree;

	setFunction(1);

	kmx=200.; kmy=200.; kmz=200;
	#ifdef _TX3D_LIBRARY_
		indexedTexture = 0;
	#endif
}

#ifdef _TX3D_LIBRARY_
s_f3d::~s_f3d() {
	if (indexedTexture) {
		delete indexedTexture;
		indexedTexture = 0;
	}
}
#endif

char* s_f3d::getNameFuction(int num)
{
	if(num < MAX_3D_FUNCTION) return tableF[num].nameFunction;
	else return txt_empty;
}

void s_f3d::setFunction(int num)
{
	if(num < MAX_3D_FUNCTION) {
		calcFunc=tableF[num].function; currentFunction=num;
	}
	else {
		calcFunc=tableF[0].function; currentFunction=0;
	}
}

//extern float noise3(float vec[3]);
extern float turbulence(float point[3], float lofreq, float hifreq); 
//extern float turbulence01_08(float point[3]); 

int s_f3d::calcMarble(int x, int y, int z)
{
	///float VAR[3];
	///VAR[0]=(float)x/kmx;
	///VAR[1]=(float)y/kmy;
	///VAR[2]=(float)z/kmz;
	int VAR[3];
	VAR[0]=x*dr_kmx;//(x<<NOISE_FRACTION)/kmx;
	VAR[1]=y*dr_kmy;//(y<<NOISE_FRACTION)/kmy;
	VAR[2]=z*dr_kmz;//(z<<NOISE_FRACTION)/(kmz*(1<<VX_FRACTION));
	//int tmp=((float)sin(VAR[2]*VAR[0]*VAR[1]+turbulence(VAR,0.1f,1))+1)*127.5;
	///float var=VAR[2]*VAR[0]*VAR[1];
	float var=(VAR[2]*NOISE_DIVIDER)*(VAR[0]*NOISE_DIVIDER)*(VAR[1]*NOISE_DIVIDER);
	int tmp=round(((float)sin(var+turbulence01_08(VAR))+1)*127.5); 
//	float tt=turbulence(VAR,0.5,1)*20;
//	int tmp=127.5+(tt-(int)tt)*127.5; 

	//if(tmp==254)tmp=253;
	//if(tmp==255)tmp=0;
	return tmp;//cos(VAR[0]+noise3(VAR))*100;//noise3(VAR)*100;
}

/*
int s_f3d::calcTree(int x, int y, int z)
{
	float VAR[3];
	VAR[0]=(float)x/kmx;//(vMap.H_SIZE>>5);
	VAR[1]=(float)y/kmy;//(vMap.V_SIZE>>5);
	VAR[2]=(float)z/(kmz*(1<<VX_FRACTION));//(float)200/40.;////
	///int VAR[3];
	///VAR[0]=(x<<NOISE_FRACTION)/kmx;
	///VAR[1]=(y<<NOISE_FRACTION)/kmy;
	///VAR[2]=(z<<NOISE_FRACTION)/kmz;
//		int tmp=(sin(VAR[2]*VAR[0]*VAR[1]+turbulence(VAR,0.1,1))+1)*127.5; 
	float tt=turbulence(VAR,0.5,1)*20;
	//int tmp=127.5+(tt-(int)tt)*127.5; 
	int tmp=256.f*tt;

	//if(tmp==254)tmp=253;
	//if(tmp==255)tmp=0;
	return tmp;//cos(VAR[0]+noise3(VAR))*100;//noise3(VAR)*100;
}
*/

int s_f3d::calcTree(int x, int y, int z)
{
	int VAR[3];
	//VAR[0]=x*dr_kmx;
	//VAR[1]=y*dr_kmy;
	//VAR[2]=z*dr_kmz;
	VAR[0]=(x<<NOISE_FRACTION)/kmx;
	VAR[1]=(y<<NOISE_FRACTION)/kmy;
	VAR[2]=(z<<NOISE_FRACTION)/(kmz*(1<<VX_FRACTION));
	
	float tt=turbulence05_10(VAR)*20.;
	//float tt=turbulence01_08(VAR)*20.;
	int tmp=127.5+(tt-(int)tt)*127.5; 

	//int tt=turbulence05_10i(VAR);
	//int tmp=127.5+(tt-(int)tt)*127.5; 

	return tmp;
}

void s_f3d::recalcWorld(void)
{
	int i,j;
	for(i=0; i<vMap.V_SIZE; i++){
		for(j=0; j<vMap.H_SIZE; j++){
			int of=vMap.offsetBuf(j,i);
			if(vMap.VxDBuf[of]==0) {
				short v=(vMap.VxGBuf[of]<<VX_FRACTION)|(vMap.AtrBuf[of]&VX_FRACTION_MASK);
#ifdef _PERIMETER_
				if(v) 
#endif
					vMap.SurBuf[of]=calc(j,i, v);
			}
		}
	}
}

#ifdef _SURMAP_
void s_f3d::calcSpecBuf(void)
{
	int i,j;
	for(i=0; i<vMap.V_SIZE; i++){
		for(j=0; j<vMap.H_SIZE; j++){
			int of=vMap.offsetBuf(j,i);
			if(vMap.VxDBuf[of]!=0) vMap.SpecSurBuf[of]=calc(j,i, (vMap.VxGBuf[of]<<VX_FRACTION));
		}
	}
}
void s_f3d::calcSpecBufStr(int Y)
{
	int j;
	for(j=0; j<vMap.H_SIZE; j++){
		int of=vMap.offsetBuf(j, Y);
		if(vMap.VxDBuf[of]!=0) vMap.SpecSurBuf[of]=calc(j, Y, (vMap.VxGBuf[of]<<VX_FRACTION));
	}
}
#endif

//////////////// Fuction load-save f3d variable
const char * Section3DFParametrs="3DF Parameters";
void s_f3d::saveVariable(void)
{
	XBuffer tbuf;
	tbuf.init();
	tbuf <= kmx < " " <= kmy < " " <= kmz; 
	SaveINIstringV(GetTargetName(vrtMap::worldIniFile),Section3DFParametrs,"ScalingXYZ",tbuf) ;
	tbuf.init();
	tbuf <= currentFunction; 
	SaveINIstringV(GetTargetName(vrtMap::worldIniFile),Section3DFParametrs,"Function",tbuf) ;
}

void s_f3d::loadVariable(void)
{
#ifdef _TX3D_LIBRARY_
	if (!indexedTexture) {
		indexedTexture = new tx3d::IndexedTexture3D(
				tx3d::Texture3DFactory::createTexture3D("<texture type='Primitive'><persistence value='0.35'/><octaveCount value='4'/><shift value='50.0,50.0,50.0'/><scale value='0.01,0.01,0.01'/><colorizer type='Cloud'><bgColor value='0.0,0.0,0.0'/><fgColor value='1.0,1.0,1.0'/></colorizer><interpolator type='Cubic'/></texture>"),
				indexLattice
			);
	}

	ifstream ifsTx(GetTargetName("geoTx.xml"));
	char szBuffer[1024 * 10];
	ifsTx.get(szBuffer, 1024 * 10, '\0');

	tx3d::Texture3D* pTx = tx3d::Texture3DFactory::createTexture3D(string(szBuffer));
	if (!pTx) {
		pTx = tx3d::Texture3DFactory::createTexture3D("<texture type='Clear'/>");
	}
	indexedTexture->setTexture(pTx);

	ifstream ifsLattice(GetTargetName("geoLattice.bin"), ios::in | ios::binary);
	if (ifsLattice) {
		ifsLattice.read((char*)indexLattice, 65536);
		//ifsLattice.read((ifstream::char_type*)indexLattice, 65536);
	}
/*
	XStream ff(GetTargetName("geoLattice.bin"), XS_IN);
	ff.read(indexLattice, 65536);
	ff.close();
*/
#endif
	XBuffer tbuf;
	tbuf.init();
	tbuf < GetINIstringV(GetTargetName(vrtMap::worldIniFile),Section3DFParametrs,"ScalingXYZ");
	unsigned int k=tbuf.tell();
	if(k!=0){
		tbuf.set(0,XB_BEG);
		tbuf >= kmx >= kmy >= kmz;
		setKM(kmx, kmy, kmz);
	}
	tbuf.init();
	tbuf < GetINIstringV(GetTargetName(vrtMap::worldIniFile),Section3DFParametrs,"Function");
	int t;
	tbuf.set(0,XB_BEG);
	tbuf >= t;
	setFunction(t);
}



struct sKeyGeoPal keyGeoPal;
char * strGeoPalparams="GeoPal Parameters";
int sKeyGeoPal::loadKeyGeoPal(void)
{
	XBuffer tbuf;
	tbuf.init();
	tbuf < GetINIstringV(GetTargetName(vrtMap::worldIniFile),strGeoPalparams,"NumbersKey");
	unsigned int k=tbuf.tell();
	if(k!=0){
		tbuf.set(0,XB_BEG);
		tbuf >= numbers;

		int i;
		tbuf.init();
		tbuf < (GetINIstringV(GetTargetName(vrtMap::worldIniFile),strGeoPalparams,"Colors"));
		tbuf.set(0,XB_BEG);
		//Цветов на один больше чем keys
		for(i=0; i<(numbers+1); i++) { tbuf >= color[i].r; tbuf >= color[i].g; tbuf >= color[i].b; }

		tbuf.init();
		tbuf < (GetINIstringV(GetTargetName(vrtMap::worldIniFile),strGeoPalparams,"Keys"));
		tbuf.set(0,XB_BEG);
		for(i=0; i<numbers; i++) { tbuf >= key[i]; }

		return 1;
	}
	return 0;
}

void sKeyGeoPal::saveKeyGeoPal(void)
{
	XBuffer tbuf;
	tbuf.init();
	tbuf <= numbers;
	SaveINIstringV(GetTargetName(vrtMap::worldIniFile),strGeoPalparams,"NumbersKey",tbuf) ;

	int i;
	tbuf.init();
	//Цветов на один больше чем keys
	//ВНИМАНИЕ ! Пробела в конце строки для записи данных в INI файл не должно быть !(иначе при каждой записи будут добавляться пробелы в конец строки(баг Windows))
	for(i=0; i<(numbers+1); i++) { tbuf< " " <= color[i].r < " " <= color[i].g < " " <= color[i].b; }
	SaveINIstringV(GetTargetName(vrtMap::worldIniFile),strGeoPalparams,"Colors",tbuf) ;

	tbuf.init();
	//ВНИМАНИЕ ! Пробела в конце строки для записи данных в INI файл не должно быть !(иначе при каждой записи будут добавляться пробелы в конец строки(баг Windows))
	for(i=0; i<numbers; i++) { tbuf < " " <= key[i] ; }
	SaveINIstringV(GetTargetName(vrtMap::worldIniFile),strGeoPalparams,"Keys",tbuf) ;

}


void sKeyGeoPal::copyKeyGeoPal(sKeyGeoPal keyGeoPal)
{
	numbers=keyGeoPal.numbers;
	int i;
	for(i=0; i<(numbers+1); i++) {
		color[i].r=keyGeoPal.color[i].r;
		color[i].g=keyGeoPal.color[i].g;
		color[i].b=keyGeoPal.color[i].b;
	}
	for(i=0; i<numbers; i++) key[i]=keyGeoPal.key[i];
}





////////////////////////////////////////////////////////////////////////////////////////////////////
// Old Version 

void pn(void)
{
//	loadTga4Noise();

/*	float VAR[3];
	int i,j;
	for(i=0; i<vMap.V_SIZE; i++){//vMap.V_SIZE
		for(j=0; j<vMap.H_SIZE; j++){

			//float km=400;
			float km=200;
			float kmz=200;
			VAR[0]=(float)j/km;//(vMap.H_SIZE>>5);
			VAR[1]=(float)i/km;//(vMap.V_SIZE>>5);
			int of=vMap.offsetBuf(j,i);
			VAR[2]=(float)vMap.VxGBuf[of]/kmz;//(float)200/40.;////
//			int tmp=(sin(VAR[2]*VAR[0]*VAR[1]+turbulence(VAR,0.1,1))+1)*127.5; 
			//float tt=turbulence(VAR,0.2,1);
			//int tmp=127.5+(tt-(int)tt)*127.5 ; //+bufTga4Noise[((i*j)&0xFFff)]

			float tt=turbulence(VAR,0.5,1)*20;
			int tmp=127.5+(tt-(int)tt)*127.5; 


			//float radius, angle;
			//radius = sqrt(VAR[0]+VAR[1]);
			//if (VAR[3]==0) angle = 3.14159265358979323846/2;
			//else angle = atan(VAR[2]);
			//int tmp= round(radius+tt*256) % 256;



			vMap.SurBuf[of]=tmp;//cos(VAR[0]+noise3(VAR))*100;//noise3(VAR)*100;
		}
	}*/


/*
	for(i=0; i<256; i++){//vMap.V_SIZE
		for(j=0; j<vMap.H_SIZE; j++){

			VAR[0]=(float)j/300.;//(vMap.H_SIZE>>5);
			VAR[1]=(float)i/300.;//(vMap.V_SIZE>>5);
			int of=vMap.offsetBuf(j,i);
			VAR[2]=(float)255/300.;//(float)vMap.VxGBuf[of]/300.;////
			int tmp=(sin(VAR[2]*VAR[0]*VAR[1]+turbulence(VAR,0.1,1))+1)*127.5; 

			vMap.SurBuf[of]=tmp;//cos(VAR[0]+noise3(VAR))*100;//noise3(VAR)*100;
		}
	}

	for(i=256; i<512; i++){//vMap.V_SIZE
		for(j=0; j<vMap.H_SIZE; j++){

			VAR[0]=(float)j/300.;//(vMap.H_SIZE>>5);
			VAR[1]=(float)255/300.;//(vMap.V_SIZE>>5);
			int of=vMap.offsetBuf(j,i);
			VAR[2]=(float)(511-i)/300.;//(float)200/40.;////
			int tmp=(sin(VAR[2]*VAR[0]*VAR[1]+turbulence(VAR,0.1,1))+1)*127.5; 

			vMap.SurBuf[of]=tmp;//cos(VAR[0]+noise3(VAR))*100;//noise3(VAR)*100;
		}
	}
*/
/*	//cosine( x + perlin(x,y,z) 
	for(i=0; i<256; i++){//vMap.V_SIZE
		for(j=0; j<vMap.H_SIZE; j++){
			VAR[0]=(float)j;///(vMap.H_SIZE>>0);
			VAR[1]=(float)i;///(vMap.V_SIZE>>0);
			int of=vMap.offsetBuf(j,i);
			//VAR[2]=(float)vMap.VxGBuf[of]/100;
			VAR[2]=(float)256;///100
			//int tmp=(sin(VAR[2]*i*j+turbulence(VAR,0.01,1))+1)*127.5; 
			int tmp=((sin(sqrt(i*i+j*j+3*turbulence(VAR, 0.01,1)))))*127.5; 
			vMap.RnrBuf[of]=tmp;//cos(VAR[0]+noise3(VAR))*100;//noise3(VAR)*100;
		}
	}
	for(i=256; i<512; i++){//vMap.V_SIZE
		for(j=0; j<vMap.H_SIZE; j++){
			VAR[0]=(float)j;///(vMap.H_SIZE>>0);
			VAR[1]=(float)256;///(vMap.V_SIZE>>0);
			int of=vMap.offsetBuf(j,i);
			//VAR[2]=(float)vMap.VxGBuf[of]/100;
			VAR[2]=(float)(512-i);///100
			//int tmp=(sin(VAR[2]*256*j+turbulence(VAR,0.01,1))+1)*127.5; 
			int tmp=((sin(sqrt(256*256+j*j+3*turbulence(VAR, 0.01,1)))))*127.5; 
			vMap.RnrBuf[of]=tmp;//cos(VAR[0]+noise3(VAR))*100;//noise3(VAR)*100;
		}
	}
*/
}

/*
inline unsigned char vrtMap::GetGeoType(int offset, int h) 
{
		int j=offset & clip_mask_x;
		int i=offset>>H_SIZE_POWER;

		float VAR[3];
	
		//float km=400;
		//float km=20000;
		//float kmz=400;
		float km=200;
		float kmz=200;
		VAR[0]=(float)j/km;//(vMap.H_SIZE>>5);
		VAR[1]=(float)i/km;//(vMap.V_SIZE>>5);
		int of=vMap.offsetBuf(j,i);
		VAR[2]=(float)vMap.VxGBuf[of]/kmz;//(float)200/40.;////
//		int tmp=(sin(VAR[2]*VAR[0]*VAR[1]+turbulence(VAR,0.1,1))+1)*127.5; 
		float tt=turbulence(VAR,0.5,1)*20;
		int tmp=127.5+(tt-(int)tt)*127.5; 

		return tmp;//cos(VAR[0]+noise3(VAR))*100;//noise3(VAR)*100;
		//(offset+(offset>>H_SIZE_POWER))%MAX_GEOLOGIC_SURFACE_TYPE; // (x+y)%
}


unsigned char bufTga4Noise[256*256];
void loadTga4Noise(void)
{
	TGAHEAD thead;

	thead.init();
	XStream ff(GetTargetName("wn.tga"), XS_IN);
	ff.read(&thead,sizeof(TGAHEAD));
	//if( (thead.Width!=2048) || (thead.Height!=2048) ) return;

	if(!thead.ColorMapType) return;
	if(thead.CMapDepth!=24) return;

	//CMapStart=0;CMapLenght=0;CMapDepth=0;
	//unsigned char palRGB[256*3];
	unsigned char SurPal[MAX_DAM_SURFACE_TYPE*3];
	ff.read(&SurPal[thead.CMapStart*3],thead.CMapDepth*thead.CMapLenght>>3);

	unsigned char* line = new unsigned char[256],*p;
	int ibeg,jbeg,iend,jend,ik,jk,i,j;
	if(thead.ImageDescriptor&0x20) { jbeg=0; jend=256; jk=1;}
	else { jbeg=256-1; jend=-1; jk=-1;}
	if((thead.ImageDescriptor&0x10)==0) { ibeg=0; iend=256; ik=1;}
	else { ibeg=256-1; iend=-1; ik=-1;}
	for(j=jbeg; j!=jend; j+=jk){
		p = line;
		ff.read(line,256);
		for(i = ibeg; i!=iend; i+=ik){
			bufTga4Noise[j*256+i]=*p;
			p++;
		}
	}
	ff.close();
	delete line;
}

*/