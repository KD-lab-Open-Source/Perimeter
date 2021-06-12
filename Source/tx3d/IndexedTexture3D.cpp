/**
 *	"Procedural 3D Texture Library"
 *	
 *	
 *
 *	@version $Id: Test3D.h,v 1.10 2002/11/07 13:50:53 ash Exp $
 *
 *	@author Alexander Porechnov (scm)
 */

#include "stdafxTX3D.h"
#include "IndexedTexture3D.hpp"
#include "XMLUtils.hpp"
#include <cmath>
#include <algorithm>

using namespace tx3d;

IndexedTexture3D::IndexedTexture3D(Texture3D* texture, unsigned char* indexLattice)
											: texture(texture), indexLattice(indexLattice) {
	if (texture) {
		texture->referenced();
	}
}

IndexedTexture3D::~IndexedTexture3D() {
	if (texture) {
		texture->released();
	}
}

void IndexedTexture3D::setTexture(Texture3D* t) {
	if (texture) {
		texture->released();
	}
	if (t) {
		t->referenced();
	}
	texture = t;
}

void IndexedTexture3D::fillIndexLattice(Vector3D* colorTable, unsigned char* indexLattice) {
	Vector3D color;
	int x, y, z, i;
	int indexInLattice = 0;
	for (x = 0; x < 32; x++) {
		for (y = 0; y < 64; y++) {
			for (z = 0; z < 32; z++) {
				color.x = x / 31.0f;
				color.y = y / 63.0f;
				color.z = z / 31.0f;

				int nearestIndex = 0;
				Vector3D diff = color - colorTable[0];
//				diff.abs();
				float min = measureColor(diff);
				for (i = 1; i < 256; i++) {
					diff = color - colorTable[i];
//					diff.abs();
					float dist = measureColor(diff);
					if (dist < min) {
						min = dist;
						nearestIndex = i;
					}
				}
				indexLattice[indexInLattice] = nearestIndex;
				indexInLattice++;
			}
		}
	}
}

void IndexedTexture3D::fillIndexLatticeHSB(Vector3D* colorTable, unsigned char* indexLattice) {
	Vector3D color;
	int x, y, z, i;
	int indexInLattice = 0;
	for (x = 0; x < 128; x++) {
		for (y = 0; y < 32; y++) {
			for (z = 0; z < 16; z++) {
				color.x = x / 127.0f;
				color.y = y / 31.0f;
				color.z = z / 15.0f;

				int nearestIndex = 0;
				Vector3D diff = color - Texture3DUtils::convertRGBtoHSB(colorTable[0]);
				float min = measureColor(diff, false);
				for (i = 1; i < 256; i++) {
					diff = color - Texture3DUtils::convertRGBtoHSB(colorTable[i]);
					float dist = measureColor(diff, false);
					if (dist < min) {
						min = dist;
						nearestIndex = i;
					}
				}
				indexLattice[indexInLattice] = nearestIndex;
				indexInLattice++;
			}
		}
	}
}

struct Color {
	Vector3D clr;
	long index;
	bool operator < (const Color& c) const {
		return index > c.index;
	}
};

void IndexedTexture3D::fillColorTable(
								Texture3D* texture,
								Vector3D* colorTable,
								const Vector3D& lookUpStep,
								const Vector3D& startPoint,
								const Vector3D& endPoint,
								float treshold,
								bool rgb
											) {

	Color* pool = new Color[65536];

	int clrCount = 0;
	int i;
	Vector3D clr;
	Vector3D point;

	Vector3D diff;

	for (point.x = startPoint.x; point.x < endPoint.x; point.x += lookUpStep.x) {
		for (point.y = startPoint.y; point.y < endPoint.y; point.y += lookUpStep.y) {
			for (point.z = startPoint.z; point.z < endPoint.z; point.z += lookUpStep.z) {
				texture->getColor(&clr, point);
//===
				clr.x = Texture3DUtils::floor(clr.x * 31.0);
				clr.y = Texture3DUtils::floor(clr.y * 63.0);
				clr.z = Texture3DUtils::floor(clr.z * 31.0);
				clr.x /= 31.0f;
				clr.y /= 63.0f;
				clr.z /= 31.0f;

				if (!rgb) {
					clr = Texture3DUtils::convertRGBtoHSB(clr);
				}

				for (i = 0; i < clrCount; i++) {
					diff = clr - pool[i].clr;
//===
						
					if ( measureColor(diff, rgb) < treshold ) {
						pool[i].index++;
						i = clrCount + 2;
					}
				}
				if (i == clrCount) {
					pool[clrCount].clr = clr;
					pool[clrCount].index = 1;
					clrCount++;
					if (clrCount >= 65536) {
						delete[] pool;
						return;
					}
				}
			}
		}
	}
	sort(&pool[0], &pool[clrCount]);
	for (i = 0; i < 256; i++) {
		colorTable[i] = rgb ? pool[i].clr : Texture3DUtils::convertHSBtoRGB(pool[i].clr);
//		colorTable[i] = Texture3DUtils::convertHSBtoRGB(Texture3DUtils::convertRGBtoHSB(pool[i].clr));
	}

	delete[] pool;
}

string IndexedTexture3D::colorTableToXML(const string& tagName, const Vector3D* colorTable) {
	string res;
	for (int i = 0; i < 256; i++) {
		res += XMLUtils::vectorToTag("color", colorTable[i]);
	}
	return XMLUtils::createTag("colorTable", "", "", res);
}

void IndexedTexture3D::extractColorTableFromXML(const string& xml, Vector3D* colorTable) {
	string content = XMLUtils::extractContentFromTag(xml);
	for (int i = 0; i < 256; i++) {
		colorTable[i] = XMLUtils::vectorFromTag( XMLUtils::extractTagFromXML("color", i, content) );
	}
}
