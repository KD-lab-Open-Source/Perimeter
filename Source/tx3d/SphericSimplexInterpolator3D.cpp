/**
 *	"Procedural 3D Texture Library"
 *	
 *  
 *	
 *	@version $Id: CosInterpolator3D.cpp,v 1.5 2002/11/05 17:55:26 ash Exp $
 *
 *	@author Alexander Porechnov (scm)
 */

#include "stdafxTX3D.h"
#include "SphericSimplexInterpolator3D.hpp"
#include "Texture3DUtils.hpp"

using namespace tx3d;

const string SphericSimplexInterpolator3D::TYPE_NAME = "SphericSimplex";

float SphericSimplexInterpolator3D::interpolate(const Vector3D &v) {
	//TODO: optimization needed
	float skewLength = (v.x + v.y + v.z) / 3.0f - 0.5f;
	long floorX = Texture3DUtils::round(v.x + skewLength);
	long floorY = Texture3DUtils::round(v.y + skewLength);
	long floorZ = Texture3DUtils::round(v.z + skewLength);
	skewLength = (floorX + floorY + floorZ) / 6.0;
/*
	float reminderX;
	float reminderY;
	float reminderZ;
/*
	_asm {
		fild	[dword ptr floorX]
		fstp	[dword ptr reminderX]

		fild	[dword ptr floorY]
		fstp	[dword ptr reminderY]

		fild	[dword ptr floorZ]
		fstp	[dword ptr reminderZ]
	};
/*
	reminderX = v.x + skewLength - reminderX;
	reminderY = v.y + skewLength - reminderY;
	reminderZ = v.z + skewLength - reminderZ;
*/

/*
	float reminderX = v.x + skewLength;
	float reminderY = v.y + skewLength;
	float reminderZ = v.z + skewLength;

    _asm{
		fld		[dword ptr reminderX]
		fild	[dword ptr floorX]
		fsubp	st(1), st(0)
		fstp	[dword ptr reminderX]

		fld		[dword ptr reminderY]
		fild	[dword ptr floorY]
		fsubp	st(1), st(0)
		fstp	[dword ptr reminderY]

		fld		[dword ptr reminderZ]
		fild	[dword ptr floorZ]
		fsubp	st(1), st(0)
		fstp	[dword ptr reminderZ]
	}
*/

    //TODO Check if this is correct version of above commented block
	float reminderX = v.x + skewLength - (float)floorX;
	float reminderY = v.y + skewLength - (float)floorY;
	float reminderZ = v.z + skewLength - (float)floorZ;

/*
	float reminderX = v.x + skewLength;
	float reminderY = v.y + skewLength;
	float reminderZ = v.z + skewLength;
*/
	simplex[0] = 0;
	simplex[1] = 0;
	simplex[2] = 0;

	char highIndex;
	char lowIndex;
	if (reminderX >= reminderZ) {
		highIndex = reminderX >= reminderY ? 0 : 1;
		lowIndex = reminderY < reminderZ ? 1 : 2;
	} else {
		highIndex = reminderY >= reminderZ ? 1 : 2;
		lowIndex = reminderX < reminderY ? 0 : 1;
	}

//----------1
	skewLength = (simplex[0] + simplex[1] + simplex[2]) / 6.0;
/*	float simplexX = reminderX + skewLength;
	float simplexY = reminderY + skewLength;
	float simplexZ = reminderZ + skewLength;
*/	float simplexX = reminderX + skewLength - simplex[0];
	float simplexY = reminderY + skewLength - simplex[1];
	float simplexZ = reminderZ + skewLength - simplex[2];

	float sph1 = 0.6 - simplexX * simplexX - simplexY * simplexY - simplexZ * simplexZ; 

	long x = floorX + simplex[0];
	long y = floorY + simplex[1];
	long z = floorZ + simplex[2];

	int sixBits = 
		  bitPatterns[
				  ((x >> 0) & 1) << 2
				| ((y >> 0) & 1) << 1
				| ((z >> 0) & 1)
		  ]
		+ bitPatterns[
				  ((y >> 1) & 1) << 2
				| ((z >> 1) & 1) << 1
				| ((x >> 1) & 1)
		  ]
		+ bitPatterns[
				  ((z >> 2) & 1) << 2
				| ((x >> 2) & 1) << 1
				| ((y >> 2) & 1)
		  ]
		+ bitPatterns[
				  ((x >> 3) & 1) << 2
				| ((y >> 3) & 1) << 1
				| ((z >> 3) & 1)
		  ]
		+ bitPatterns[
				  ((y >> 4) & 1) << 2
				| ((z >> 4) & 1) << 1
				| ((x >> 4) & 1)
		  ]
		+ bitPatterns[
				  ((z >> 5) & 1) << 2
				| ((x >> 5) & 1) << 1
				| ((y >> 5) & 1)
		  ]
		+ bitPatterns[
				  ((x >> 6) & 1) << 2
				| ((y >> 6) & 1) << 1
				| ((z >> 6) & 1)
		  ]
		+ bitPatterns[
				  ((y >> 7) & 1) << 2
				| ((z >> 7) & 1) << 1
				| ((x >> 7) & 1)
		  ];

	simplex[highIndex]++;

	float gradientX;
	float gradientY;
	float gradientZ;

	char bit5;
	char bit4;
	char bit3;
	char bit2;
	char low2bits;
	if (sph1 >= 0) {
		bit5 = (sixBits >> 5) & 1;
		bit4 = (sixBits >> 4) & 1;
		bit3 = (sixBits >> 3) & 1;
		bit2 = (sixBits >> 2) & 1;
		low2bits = sixBits & 3;

		switch(low2bits) {
			case 2:
				gradientX = simplexZ;
				gradientY = simplexX;
				gradientZ = simplexY;
				break;
			case 1:
				gradientX = simplexY;
				gradientY = simplexZ;
				gradientZ = simplexX;
				break;
			default:
				gradientX = simplexX;
				gradientY = simplexY;
				gradientZ = simplexZ;
				break;
		}

		gradientX = (bit5 == bit3) ? -gradientX : gradientX;
		if (low2bits == 0) {
			gradientY = (bit5 == bit4) ? -gradientY : gradientY;
			gradientZ = (bit5 != (bit3 ^ bit3)) ? -gradientZ : gradientZ;
		} else {
			if (bit2 == 0) {
				gradientY = (bit5 == bit4) ? -gradientY : gradientY;
				gradientZ = 0;
			} else {
				gradientY = 0;
				gradientZ = (bit5 != (bit3 ^ bit3)) ? -gradientZ : gradientZ;
			}
		}

		sph1 *= sph1;

		sph1 = sph1 * sph1 * (gradientX + gradientY + gradientZ);

	} else {
		sph1 = 0.0f;
	}


//----------2
	skewLength = (simplex[0] + simplex[1] + simplex[2]) / 6.0;
/*	float simplexX = reminderX + skewLength;
	float simplexY = reminderY + skewLength;
	float simplexZ = reminderZ + skewLength;
*/	simplexX = reminderX + skewLength - simplex[0];
	simplexY = reminderY + skewLength - simplex[1];
	simplexZ = reminderZ + skewLength - simplex[2];

	float sph2 = 0.6 - simplexX * simplexX - simplexY * simplexY - simplexZ * simplexZ; 

	x = floorX + simplex[0];
	y = floorY + simplex[1];
	z = floorZ + simplex[2];

	sixBits = 
		  bitPatterns[
				  ((x >> 0) & 1) << 2
				| ((y >> 0) & 1) << 1
				| ((z >> 0) & 1)
		  ]
		+ bitPatterns[
				  ((y >> 1) & 1) << 2
				| ((z >> 1) & 1) << 1
				| ((x >> 1) & 1)
		  ]
		+ bitPatterns[
				  ((z >> 2) & 1) << 2
				| ((x >> 2) & 1) << 1
				| ((y >> 2) & 1)
		  ]
		+ bitPatterns[
				  ((x >> 3) & 1) << 2
				| ((y >> 3) & 1) << 1
				| ((z >> 3) & 1)
		  ]
		+ bitPatterns[
				  ((y >> 4) & 1) << 2
				| ((z >> 4) & 1) << 1
				| ((x >> 4) & 1)
		  ]
		+ bitPatterns[
				  ((z >> 5) & 1) << 2
				| ((x >> 5) & 1) << 1
				| ((y >> 5) & 1)
		  ]
		+ bitPatterns[
				  ((x >> 6) & 1) << 2
				| ((y >> 6) & 1) << 1
				| ((z >> 6) & 1)
		  ]
		+ bitPatterns[
				  ((y >> 7) & 1) << 2
				| ((z >> 7) & 1) << 1
				| ((x >> 7) & 1)
		  ];

	simplex[3 - highIndex - lowIndex]++;
	if (sph2 >= 0) {
		bit5 = (sixBits >> 5) & 1;
		bit4 = (sixBits >> 4) & 1;
		bit3 = (sixBits >> 3) & 1;
		bit2 = (sixBits >> 2) & 1;
		low2bits = sixBits & 3;

		switch(low2bits) {
			case 2:
				gradientX = simplexZ;
				gradientY = simplexX;
				gradientZ = simplexY;
				break;
			case 1:
				gradientX = simplexY;
				gradientY = simplexZ;
				gradientZ = simplexX;
				break;
			default:
				gradientX = simplexX;
				gradientY = simplexY;
				gradientZ = simplexZ;
				break;
		}

		gradientX = (bit5 == bit3) ? -gradientX : gradientX;
		if (low2bits == 0) {
			gradientY = (bit5 == bit4) ? -gradientY : gradientY;
			gradientZ = (bit5 != (bit3 ^ bit3)) ? -gradientZ : gradientZ;
		} else {
			if (bit2 == 0) {
				gradientY = (bit5 == bit4) ? -gradientY : gradientY;
				gradientZ = 0;
			} else {
				gradientY = 0;
				gradientZ = (bit5 != (bit3 ^ bit3)) ? -gradientZ : gradientZ;
			}
		}

		sph2 *= sph2;

		sph2 = sph2 * sph2 * (gradientX + gradientY + gradientZ);

	} else {
		sph2 = 0.0f;
	}

//----------3
	skewLength = (simplex[0] + simplex[1] + simplex[2]) / 6.0;
/*	float simplexX = reminderX + skewLength;
	float simplexY = reminderY + skewLength;
	float simplexZ = reminderZ + skewLength;
*/	simplexX = reminderX + skewLength - simplex[0];
	simplexY = reminderY + skewLength - simplex[1];
	simplexZ = reminderZ + skewLength - simplex[2];


	float sph3 = 0.6 - simplexX * simplexX - simplexY * simplexY - simplexZ * simplexZ; 

	x = floorX + simplex[0];
	y = floorY + simplex[1];
	z = floorZ + simplex[2];

	sixBits = 
		  bitPatterns[
				  ((x >> 0) & 1) << 2
				| ((y >> 0) & 1) << 1
				| ((z >> 0) & 1)
		  ]
		+ bitPatterns[
				  ((y >> 1) & 1) << 2
				| ((z >> 1) & 1) << 1
				| ((x >> 1) & 1)
		  ]
		+ bitPatterns[
				  ((z >> 2) & 1) << 2
				| ((x >> 2) & 1) << 1
				| ((y >> 2) & 1)
		  ]
		+ bitPatterns[
				  ((x >> 3) & 1) << 2
				| ((y >> 3) & 1) << 1
				| ((z >> 3) & 1)
		  ]
		+ bitPatterns[
				  ((y >> 4) & 1) << 2
				| ((z >> 4) & 1) << 1
				| ((x >> 4) & 1)
		  ]
		+ bitPatterns[
				  ((z >> 5) & 1) << 2
				| ((x >> 5) & 1) << 1
				| ((y >> 5) & 1)
		  ]
		+ bitPatterns[
				  ((x >> 6) & 1) << 2
				| ((y >> 6) & 1) << 1
				| ((z >> 6) & 1)
		  ]
		+ bitPatterns[
				  ((y >> 7) & 1) << 2
				| ((z >> 7) & 1) << 1
				| ((x >> 7) & 1)
		  ];

	simplex[lowIndex]++;
	if (sph3 >= 0) {
		bit5 = (sixBits >> 5) & 1;
		bit4 = (sixBits >> 4) & 1;
		bit3 = (sixBits >> 3) & 1;
		bit2 = (sixBits >> 2) & 1;
		low2bits = sixBits & 3;

		switch(low2bits) {
			case 2:
				gradientX = simplexZ;
				gradientY = simplexX;
				gradientZ = simplexY;
				break;
			case 1:
				gradientX = simplexY;
				gradientY = simplexZ;
				gradientZ = simplexX;
				break;
			default:
				gradientX = simplexX;
				gradientY = simplexY;
				gradientZ = simplexZ;
				break;
		}

		gradientX = (bit5 == bit3) ? -gradientX : gradientX;
		if (low2bits == 0) {
			gradientY = (bit5 == bit4) ? -gradientY : gradientY;
			gradientZ = (bit5 != (bit3 ^ bit3)) ? -gradientZ : gradientZ;
		} else {
			if (bit2 == 0) {
				gradientY = (bit5 == bit4) ? -gradientY : gradientY;
				gradientZ = 0;
			} else {
				gradientY = 0;
				gradientZ = (bit5 != (bit3 ^ bit3)) ? -gradientZ : gradientZ;
			}
		}

		sph3 *= sph3;

		sph3 = sph3 * sph3 * (gradientX + gradientY + gradientZ);

	} else {
		sph3 = 0.0f;
	}

//----------4
	skewLength = (simplex[0] + simplex[1] + simplex[2]) / 6.0;
/*	float simplexX = reminderX + skewLength;
	float simplexY = reminderY + skewLength;
	float simplexZ = reminderZ + skewLength;
*/	simplexX = reminderX + skewLength - simplex[0];
	simplexY = reminderY + skewLength - simplex[1];
	simplexZ = reminderZ + skewLength - simplex[2];


	float sph4 = 0.6 - simplexX * simplexX - simplexY * simplexY - simplexZ * simplexZ; 

	x = floorX + simplex[0];
	y = floorY + simplex[1];
	z = floorZ + simplex[2];

	sixBits = 
		  bitPatterns[
				  ((x >> 0) & 1) << 2
				| ((y >> 0) & 1) << 1
				| ((z >> 0) & 1)
		  ]
		+ bitPatterns[
				  ((y >> 1) & 1) << 2
				| ((z >> 1) & 1) << 1
				| ((x >> 1) & 1)
		  ]
		+ bitPatterns[
				  ((z >> 2) & 1) << 2
				| ((x >> 2) & 1) << 1
				| ((y >> 2) & 1)
		  ]
		+ bitPatterns[
				  ((x >> 3) & 1) << 2
				| ((y >> 3) & 1) << 1
				| ((z >> 3) & 1)
		  ]
		+ bitPatterns[
				  ((y >> 4) & 1) << 2
				| ((z >> 4) & 1) << 1
				| ((x >> 4) & 1)
		  ]
		+ bitPatterns[
				  ((z >> 5) & 1) << 2
				| ((x >> 5) & 1) << 1
				| ((y >> 5) & 1)
		  ]
		+ bitPatterns[
				  ((x >> 6) & 1) << 2
				| ((y >> 6) & 1) << 1
				| ((z >> 6) & 1)
		  ]
		+ bitPatterns[
				  ((y >> 7) & 1) << 2
				| ((z >> 7) & 1) << 1
				| ((x >> 7) & 1)
		  ];

	simplex[0]++;
	if (sph4 >= 0) {
		bit5 = (sixBits >> 5) & 1;
		bit4 = (sixBits >> 4) & 1;
		bit3 = (sixBits >> 3) & 1;
		bit2 = (sixBits >> 2) & 1;
		low2bits = sixBits & 3;

		switch(low2bits) {
			case 2:
				gradientX = simplexZ;
				gradientY = simplexX;
				gradientZ = simplexY;
				break;
			case 1:
				gradientX = simplexY;
				gradientY = simplexZ;
				gradientZ = simplexX;
				break;
			default:
				gradientX = simplexX;
				gradientY = simplexY;
				gradientZ = simplexZ;
				break;
		}

		gradientX = (bit5 == bit3) ? -gradientX : gradientX;
		if (low2bits == 0) {
			gradientY = (bit5 == bit4) ? -gradientY : gradientY;
			gradientZ = (bit5 != (bit3 ^ bit3)) ? -gradientZ : gradientZ;
		} else {
			if (bit2 == 0) {
				gradientY = (bit5 == bit4) ? -gradientY : gradientY;
				gradientZ = 0;
			} else {
				gradientY = 0;
				gradientZ = (bit5 != (bit3 ^ bit3)) ? -gradientZ : gradientZ;
			}
		}

		sph4 *= sph4;

		sph4 = sph4 * sph4 * (gradientX + gradientY + gradientZ);

	} else {
		sph4 = 0.0f;
	}

	return 8.0 * (sph1 + sph2 + sph3 + sph4);
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
	return 0.0;
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

/*	return
		  bitPatterns[
				  ((x >> 0) & 1) << 2
				| ((y >> 0) & 1) << 1
				| ((z >> 0) & 1)
		  ]
		+ bitPatterns[
				  ((y >> 1) & 1) << 2
				| ((z >> 1) & 1) << 1
				| ((x >> 1) & 1)
		  ]
		+ bitPatterns[
				  ((z >> 2) & 1) << 2
				| ((x >> 2) & 1) << 1
				| ((y >> 2) & 1)
		  ]
		+ bitPatterns[
				  ((x >> 3) & 1) << 2
				| ((y >> 3) & 1) << 1
				| ((z >> 3) & 1)
		  ]
		+ bitPatterns[
				  ((y >> 4) & 1) << 2
				| ((z >> 4) & 1) << 1
				| ((x >> 4) & 1)
		  ]
		+ bitPatterns[
				  ((z >> 5) & 1) << 2
				| ((x >> 5) & 1) << 1
				| ((y >> 5) & 1)
		  ]
		+ bitPatterns[
				  ((x >> 6) & 1) << 2
				| ((y >> 6) & 1) << 1
				| ((z >> 6) & 1)
		  ]
		+ bitPatterns[
				  ((y >> 7) & 1) << 2
				| ((z >> 7) & 1) << 1
				| ((x >> 7) & 1)
		  ];
*/
}

int SphericSimplexInterpolator3D::getBitPattern(int x, int y, int z, char bitNumber) {
	return bitPatterns[
				  extractBit(x,bitNumber) << 2
				| extractBit(y,bitNumber) << 1
				| extractBit(z,bitNumber)
			];
/*	return bitPatterns[
				  ((x >> bitNumber) & 1) << 2
				| ((y >> bitNumber) & 1) << 1
				| ((z >> bitNumber) & 1)
			];
*/
}
