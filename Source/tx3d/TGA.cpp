#include "stdafxTX3D.h"
#include "TGA.hpp"
/*
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <io.h>
#include <stdio.h>
#include <string.h>
*/
#include "xutil.h"


#include <Windows.H>
#include <StdIo.H>
#include <FStream>

#ifndef _SURMAP_
#include "..\PluginMAX\ZIPStream.h"
#endif

using namespace tx3d;

bool TGA::enable_load_assert=true;

TGA::TGA() : pixels(0), bpp(0), width(0), height(0) {
}

TGA::~TGA() {
	clean();
}

void TGA::clean() {
	if (pixels) {
		delete[] pixels;
		pixels = 0;
	}
}

Vector3D TGA::getPixel(int x, int y) const {
	int index = (x + y * width) * bpp;
	if (index >= 0 && (index + 2) < (width * height * bpp)) {

		return Vector3D(
				pixels[index] / 256.0f,
				pixels[index + 1] / 256.0f,
				pixels[index + 2] / 256.0f
			);

	} else {
		xassert(0 && "Overrun when getPixel from GeoTx TGA");
		return Vector3D();
	}
}

bool TGA::load(const char *fileName) {
	clean();
/*
//---check------
	loadTest(fileName);
	unsigned char* tmpPixels = new unsigned char[width * height * bpp];
	memcpy(tmpPixels, pixels, width * height * bpp);
//---------
*/
#ifndef _SURMAP_
	ZIPStream file(fileName);
#else
	XStream file(fileName, XS_IN);
#endif
	if (!!file) {
		string errMsg("Error reading GeoTx TGA: ");
		errMsg += fileName;
		if(enable_load_assert)
			xxassert(0, errMsg.c_str());
		return false;
	}

	TGAHeader hdr;

	file.read(&hdr, 18);

	bpp = hdr.bitsPerPixel / 8;

	file.seek(18, SEEK_SET);
	width = hdr.width;
	height = hdr.height;

	unsigned long numBytes = width * height * bpp;

	pixels = new unsigned char[numBytes];
	file.read(pixels, numBytes);

	file.close();

//	bool updown = (hdr.flags & 0x20) ? false : true;

	if (hdr.flags & 0x20 == 0) {
		int size = width * bpp;
		unsigned char* tmp = new unsigned char[size];

		for (int y = 0, s = height / 2; y < s; y++) {
			unsigned char* p1 = pixels + y * size;
			unsigned char* p2 = pixels + (height - 1 - y) * size;
			memcpy(tmp, p1, size);
			memcpy(p1, p2, size);
			memcpy(p2, tmp, size);
		}

		delete tmp;
	}

	if (bpp >= 3) {
		for (int i = 0; i < numBytes; i += bpp) {
			unsigned char temp = pixels[i];
			pixels[i] = pixels[i + 2];
			pixels[i + 2] = temp;
		}
	}
/*
//--check------
	bool res = true;
	for (int i = 0; i < numBytes; i++) {
		if (tmpPixels[i] != pixels[i]) {
			res = false;
		}
	}
	delete[] tmpPixels;
//--------
*/
	return true;
}


bool TGA::loadTest(const char *fileName) {
	clean();

	// Для наглядности
	#define TGA_TYPE_COLOR 2
	#define TGA_TYPE_GRAY 3

	unsigned char GrayMask[12] = {0, 0, TGA_TYPE_GRAY, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	unsigned char RGBMask[12]={0, 0, TGA_TYPE_COLOR, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	
	// Заголовок TGA фала
	unsigned char Header[18];

	FILE *File = fopen(fileName, "rb");
	
    if (File == NULL) 
	{
		return false;
	}

	// Читаем заголовок TGA
//	if (fseek(File, 0, SEEK_SET) || fread(Header, sizeof(Header), 1, File) != true)
	if (fseek(File, 0, SEEK_SET) || (fread(Header, sizeof(Header), 1, File) == 0) )
	{
		return false;
	} 
	
	if (memcmp(RGBMask, Header, sizeof(RGBMask))&&memcmp(GrayMask, Header, sizeof(GrayMask))!=0)
	{
		return false;
	}

	bpp= Header[16];

	bpp>>=3;
 	width = (Header[13]<< 8) |Header[12];
	height = (Header[15]<< 8) |Header[14];

	
	pixels = (unsigned char*) malloc(width * height * bpp);

	if(fread(pixels, bpp, width*height,  File)!=size_t(width*height))
	{
		return false;
	}
	
	if(bpp>=3)

	for (int Index=0; Index<width * height * bpp; Index+=bpp)
	{
		unsigned char Temp=pixels[Index];
		pixels[Index]=pixels[Index + 2];
		pixels[Index+2]= Temp;
	}

	fclose(File);
	return true;
}

void TGA::setAssertLoad(bool enable)
{
	enable_load_assert=enable;
}