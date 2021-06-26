#include "stdafxTr.h"

void TGAHEAD::save3layers(const char* fname,int sizeX,int sizeY,unsigned char* Ra,unsigned char* Ga,unsigned char* Ba)
{//GetTargetName("outputnh.tga")
	init();
	XStream ff(fname, XS_OUT);
	Width=(short)sizeX;
	Height=(short)sizeY;
	ff.write(this,sizeof(TGAHEAD));
	unsigned char *line = new unsigned char[sizeX*3],*p;
	unsigned int i,j;
	for(j = 0; j<sizeY; j++){
		p = line;
		for(i = 0; i<sizeX; i++){
			//*p++ = (unsigned char)((((*vidBuf)[j][i])&0x1F)<<3) ;
			//*p++ = (unsigned char)((((*vidBuf)[j][i])&0x7E0)>>3) ;
			//*p++ = (unsigned char)((((*vidBuf)[j][i])&0x0F800)>>8) ;
			*p++ = Ba[j*sizeX+i];
			*p++ = Ga[j*sizeX+i];
			*p++ = Ra[j*sizeX+i];
		}
		ff.write(line,sizeX*3);
	}
	ff.close();
	delete[] line;
}

void TGAHEAD::load3layers(const char* fname,int sizeX,int sizeY,unsigned char* Ra,unsigned char* Ga,unsigned char* Ba)
{//GetTargetName("outputnh.tga")
	init();
	XStream ff(fname, XS_IN);
	ff.read(this,sizeof(TGAHEAD));
	if( (Width!=sizeX) || (Height!=sizeY) ) return;
	unsigned char *line = new unsigned char[sizeX*3],*p;
	unsigned int i,j;
/*	for(j = 0; j<sizeY; j++){
		p = line;
		ff.read(line,sizeX*3);
		for(i = 0; i<sizeX; i++){
			//*p++ = (unsigned char)((((*vidBuf)[j][i])&0x1F)<<3) ;
			//*p++ = (unsigned char)((((*vidBuf)[j][i])&0x7E0)>>3) ;
			//*p++ = (unsigned char)((((*vidBuf)[j][i])&0x0F800)>>8) ;
			Ba[j*sizeX+i] = *p++;
			Ga[j*sizeX+i] = *p++;
			Ra[j*sizeX+i] = *p++;
		}
	}*/
	int ibeg,jbeg,iend,jend,ik,jk;
	if(ImageDescriptor&0x20) { jbeg=0; jend=vMap.V_SIZE; jk=1;}
	else { jbeg=vMap.V_SIZE-1; jend=-1; jk=-1;}
	if((ImageDescriptor&0x10)==0) { ibeg=0; iend=vMap.H_SIZE; ik=1;}
	else { ibeg=vMap.H_SIZE-1; iend=-1; ik=-1;}
	for(j=jbeg; j!=jend; j+=jk){
		p = line;
		ff.read(line,vMap.H_SIZE*3);
		for(i = ibeg; i!=iend; i+=ik){
			Ba[j*sizeX+i] = *p++;
			Ga[j*sizeX+i] = *p++;
			Ra[j*sizeX+i] = *p++;
		}
	}

	ff.close();
	delete[] line;
}


static XStream tgaFile(0);
bool TGAHEAD::loadHeader(const char* fname)
{
	init();
	if(!tgaFile.open(fname, XS_IN)) return 0;
	tgaFile.read(this,sizeof(TGAHEAD));
	return true;
}

bool TGAHEAD::load2buf(unsigned char* buf)
{
	int byteInPixel;
	if(PixelDepth==8) byteInPixel=1;
	else if(PixelDepth==16) byteInPixel=2;
	else if(PixelDepth==24) byteInPixel=3;
	else return 0;

	unsigned char *line = new unsigned char[Width*byteInPixel],*p;
	unsigned int i,j,k;
	int ibeg,jbeg,iend,jend,ik,jk;
	if(ImageDescriptor&0x20) { jbeg=0; jend=Height; jk=1;}
	else { jbeg=Height-1; jend=-1; jk=-1;}
	if((ImageDescriptor&0x10)==0) { ibeg=0; iend=Width; ik=1;}
	else { ibeg=Width-1; iend=-1; ik=-1;}
	for(j=jbeg; j!=jend; j+=jk){
		p = line;
		tgaFile.read(line, Width*byteInPixel);
		for(i = ibeg; i!=iend; i+=ik){
			for(k=0; k<byteInPixel; k++) {
				//*buf++=*p++;
				buf[j*Width+i]=*p++;
			}
		}
	}

	tgaFile.close();
	delete[] line;
	return true;
}

void TGAHEAD::load2RGBL(int sizeX,int sizeY, unsigned long* RGBLBuf)
{
	if( (Width!=sizeX) || (Height!=sizeY) ) return;
	unsigned char *line = new unsigned char[sizeX*3],*p;
	unsigned int i,j;
	int ibeg,jbeg,iend,jend,ik,jk;
	if(ImageDescriptor&0x20) { jbeg=0; jend=sizeY; jk=1;}
	else { jbeg=sizeY-1; jend=-1; jk=-1;}
	if((ImageDescriptor&0x10)==0) { ibeg=0; iend=sizeX; ik=1;}
	else { ibeg=sizeX-1; iend=-1; ik=-1;}
	for(j=jbeg; j!=jend; j+=jk){
		p = line;
		tgaFile.read(line,vMap.H_SIZE*3);
		for(i = ibeg; i!=iend; i+=ik){
			unsigned long c= (*p++&0xFF) <<16;
			c|= (*p++&0xFF)<<8;
			c|= (*p++&0xFF);
			RGBLBuf[j*sizeX+i]=c;
		}
	}

	tgaFile.close();
	delete[] line;
}

/*void TGAHEAD::load216(char* fname,unsigned short *ClBuf)
{
	init();
	XStream ff(fname, XS_IN);
	ff.read(this,sizeof(TGAHEAD));
	if( (Width!=2048) || (Height!=2048) ) return;

	if(!ColorMapType) return;
	if(CMapDepth!=24) return;

	//CMapStart=0;CMapLenght=0;CMapDepth=0;
	unsigned char palRGB[256*3];
	ff.read(&palRGB[CMapStart*3],CMapDepth*CMapLenght>>3);

	unsigned char* line = new unsigned char[vMap.H_SIZE],*p;
	int ibeg,jbeg,iend,jend,ik,jk,i,j;
	if(ImageDescriptor&0x20) { jbeg=0; jend=vMap.V_SIZE; jk=1;}
	else { jbeg=vMap.V_SIZE-1; jend=-1; jk=-1;}
	if((ImageDescriptor&0x10)==0) { ibeg=0; iend=vMap.H_SIZE; ik=1;}
	else { ibeg=vMap.H_SIZE-1; iend=-1; ik=-1;}
	for(j=jbeg; j!=jend; j+=jk){
		p = line;
		ff.read(line,vMap.H_SIZE);
		for(i = ibeg; i!=iend; i+=ik){
			short col;
			col =(palRGB[*p*3 + 0] >>3)&0x1F;
			col+=(palRGB[*p*3 + 1] <<3)&0x7E0;
			col+=(palRGB[*p*3 + 2] <<8)&0x0F800;
			ClBuf[j*XS_Buf+i]=col;
			p++;
		}
	}
	ff.close();
	delete[] line;
}*/