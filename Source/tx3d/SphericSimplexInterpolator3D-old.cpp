/**
 *	"Procedural 3D Texture Library"
 *	
 *  
 *	
 *	@version $Id: CosInterpolator3D.cpp,v 1.5 2002/11/05 17:55:26 ash Exp $
 *
 *	@author Alexander Porechnov (scm)
 */

#include "stdafx.h"
#include "SphericSimplexInterpolator3D.hpp"

using namespace tx3d;

const string SphericSimplexInterpolator3D::TYPE_NAME = "SphericSimplex";

float SphericSimplexInterpolator3D::interpolate(const Vector3D &v) {
	//TODO: optimization needed
	skewLength = (v.x + v.y + v.z) / 3.0;
	floorX = int(v.x + skewLength);
	floorY = int(v.y + skewLength);
	floorZ = int(v.z + skewLength);
	skewLength = (floorX + floorY + floorZ) / 6.0;
	reminderX = v.x + skewLength - floorX;
	reminderY = v.y + skewLength - floorY;
	reminderZ = v.z + skewLength - floorZ;
	simplex[0] = 0;
	simplex[1] = 0;
	simplex[2] = 0;

	if (reminderX >= reminderZ) {
		highIndex = reminderX >= reminderY ? 0 : 1;
		lowIndex = reminderY < reminderZ ? 1 : 2;
	} else {
		highIndex = reminderY >= reminderZ ? 1 : 2;
		lowIndex = reminderX < reminderY ? 0 : 1;
	}

	return
		  contribution(highIndex)
		+ contribution(3 - highIndex - lowIndex)
		+ contribution(lowIndex)
		+ contribution(0);
}

SphericSimplexInterpolator3D::SphericSimplexInterpolator3D() {
	bitPatterns[0] = 0x15;
	bitPatterns[1] = 0x38;
	bitPatterns[2] = 0x32;
	bitPatterns[3] = 0x2c;
	bitPatterns[4] = 0x0d;
	bitPatterns[5] = 0x13;
	bitPatterns[6] = 0x07;
	bitPatterns[7] = 0x2a;
}

float SphericSimplexInterpolator3D::contribution(char index) {
	skewLength = (simplex[0] + simplex[1] + simplex[2]) / 6.0;
	simplexX = reminderX + skewLength - simplex[0];
	simplexY = reminderY + skewLength - simplex[1];
	simplexZ = reminderZ + skewLength - simplex[2];
	sph = 0.6 - simplexX * simplexX - simplexY * simplexY - simplexZ * simplexZ; 

	sixBits = shuffle(floorX + simplex[0], floorY + simplex[1], floorZ + simplex[2]);
	simplex[index]++;
	if (sph < 0) {
		return 0.0;
	}

	bit5 = extractBit(sixBits, 5);//(sixBits >> 5) & 1;
	bit4 = extractBit(sixBits, 4);//(sixBits >> 4) & 1;
	bit3 = extractBit(sixBits, 3);//(sixBits >> 3) & 1;
	bit2 = extractBit(sixBits, 2);//(sixBits >> 2) & 1;
	low2bits = sixBits & 3;

	switch(low2bits) {
		case 0:
			gradientX = simplexZ;
			gradientY = simplexX;
			gradientZ = simplexY;
			break;
		case 1:
			gradientX = simplexX;
			gradientY = simplexY;
			gradientZ = simplexZ;
			break;
		case 2:
			gradientX = simplexY;
			gradientY = simplexZ;
			gradientZ = simplexX;
			break;
	}

	gradientX = (bit5 == bit3) ? -simplexX : simplexX;
	if (low2bits == 0) {
		gradientY = (bit5 == bit4) ? -simplexY : simplexY;
		gradientZ = (bit5 != (bit3 ^ bit3)) ? -simplexZ : simplexZ;
	} else {
		if (bit2 == 0) {
			gradientY = (bit5 == bit4) ? -simplexY : simplexY;
			gradientZ = 0;
		} else {
			gradientY = 0;
			gradientZ = (bit5 != (bit3 ^ bit3)) ? -simplexZ : simplexZ;
		}
	}

	sph *= sph;

	return 8.0 * sph * sph * (gradientX + gradientY + gradientZ);
}

char SphericSimplexInterpolator3D::extractBit(char source, char bitNumber) {
	return (source >> bitNumber) & 1;
}

int SphericSimplexInterpolator3D::shuffle(int x, int y, int z) {
	return
		  getBitPattern(x,y,z,0) + getBitPattern(y,z,x,1)
		+ getBitPattern(z,x,y,2) + getBitPattern(x,y,z,3)
		+ getBitPattern(y,z,x,4) + getBitPattern(z,x,y,5)
		+ getBitPattern(x,y,z,6) + getBitPattern(y,z,x,7);
}

int SphericSimplexInterpolator3D::getBitPattern(int x, int y, int z, char bitNumber) {
	return bitPatterns[
				  extractBit(x,bitNumber) << 2
				| extractBit(y,bitNumber) << 1
				| extractBit(z,bitNumber)
			];
}
