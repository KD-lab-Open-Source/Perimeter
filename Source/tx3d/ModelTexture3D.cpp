/**
 *	"Procedural 3D Texture Library"
 *	
 *
 *	@version $Id: ModelTexture3D.h,v 1.5 2002/11/07 13:50:53 ash Exp $
 *
 *	@author Alexander Porechnov (scm)
 */

#include "stdafxTX3D.h"
#include "ModelTexture3D.hpp"
#include "Texture3DUtils.hpp"
#include "XMLUtils.hpp"
#include "TGA.hpp"

using namespace tx3d;

const string ModelTexture3D::TYPE_NAME = "Model";

const float HALF = 0.5;

ModelTexture3D::ModelTexture3D(const string& paramString) {
}

ModelTexture3D::ModelTexture3D() {
	model = new Vector3D[17 * 17 * 17];
	for (int i = 0; i < 17 * 17 * 17; i++) {
		model[i].x = 0.0f;
		model[i].y = 0.0f;
		model[i].z = 0.0f;
	}

	TGA image;
	for (int h = 0; h < 16; h++) {
		if (image.load(
			(string("color") + XMLUtils::intToString(h) + string(".tga")).c_str()
					)) {
			for (int u = 0; u < 16; u++) {
				for (int k = 0; k < 16; k++) {
					model[u + k * 17 + h * 17 * 17] = image.getPixel(u, k);

//					model[u + k * 17 + h * 17 * 17].x = image.cMap[u * 3 + k * 16 * 3] / 256.0f;
//					model[u + k * 17 + h * 17 * 17].y = image.cMap[u * 3 + k * 16 * 3 + 1] / 256.0f;
//					model[u + k * 17 + h * 17 * 17].z = image.cMap[u * 3 + k * 16 * 3 + 2] / 256.0f;
				}
			}
		} else {
			image.load("color.tga");
			for (int u = 0; u < 16; u++) {
				for (int k = 0; k < 16; k++) {
					model[u + k * 17 + h * 17 * 17] = image.getPixel(u, k);

//					model[u + k * 17 + h * 17 * 17].x = image.cMap[u * 3 + k * 16 * 3] / 256.0f;
//					model[u + k * 17 + h * 17 * 17].y = image.cMap[u * 3 + k * 16 * 3 + 1] / 256.0f;
//					model[u + k * 17 + h * 17 * 17].z = image.cMap[u * 3 + k * 16 * 3 + 2] / 256.0f;
				}
			}
		}
	}
	int x;
	for (x = 0; x < 16; x++) {
		for (int y = 0; y < 16; y++) {
			model[x + 16 * 17 + y * 17 * 17] = model[x + y * 17 * 17];
			model[16 + y * 17 + x * 17 * 17] = model[y * 17 + x * 17 * 17];
			model[x + y * 17 + 16 * 17 * 17] = model[x + y * 17];
		}
	}
	for (x = 0; x < 16; x++) {
		model[x + 16 * 17 + 16 * 17 * 17] = model[x];
		model[16 + x * 17 + 16 * 17 * 17] = model[x * 17];
		model[16 + 16 * 17 + x * 17 * 17] = model[x * 17 * 17];
	}
	model[16 + 16 * 17 + 16 * 17 * 17] = model[0];
}

ModelTexture3D::~ModelTexture3D() {
	delete[] model;
}

void ModelTexture3D::getColor(Vector3D* destClr, const Vector3D &v) {

	long xi;
	long yi;
	long zi;
	long index;
	float xd;
	float yd;
	float zd;

/*	xd = v.x / 63.999999f;
	yd = v.y / 63.999999f;
	zd = v.z / 63.999999f;
*/
	xd = v.x / 255.999999f;
	yd = v.y / 255.999999f;
	zd = v.z / 255.999999f;

	xd -= Texture3DUtils::floor(xd);
	yd -= Texture3DUtils::floor(yd);
	zd -= Texture3DUtils::floor(zd);

	xd *= 15.99999f;
	yd *= 15.99999f;
	zd *= 15.99999f;

/*
	xi = xd;
	yi = yd;
	zi = zd;

	xd = xd - xi;
	yd = yd - yi;
	zd = zd - zi;
*/
	// fast implementation of C code above
#ifdef _MSC_VER
    _asm{
		fld		[dword ptr HALF]

		fld		[dword ptr xd]
		fld		st(0)
		fsub	st(0), st(2)
	fistp	[dword ptr xi]
	fild	[dword ptr xi]
		fsubp	st(1), st(0)
		fstp	[dword ptr xd]

		fld		[dword ptr yd]
		fld		st(0)
		fsub	st(0), st(2)
	fistp	[dword ptr yi]
	fild	[dword ptr yi]
		fsubp	st(1), st(0)
		fstp	[dword ptr yd]

		fld		[dword ptr zd]
		fld		st(0)
		fsub	st(0), st(2)
	fistp	[dword ptr zi]
	fild	[dword ptr zi]
		fsubp	st(1), st(0)
		fstp	[dword ptr zd]

		fstp	st(0)
	}
#else
    //TODO no idea if this is correct
    xi = xd;
    yi = yd;
    zi = zd;

    xd = xd - xi;
    yd = yd - yi;
    zd = zd - zi;
#endif

	index = xi + yi * 17 + zi * 17 * 17;

	xd = xd * xd * (3.0f - 2.0f * xd);
	yd = yd * yd * (3.0f - 2.0f * yd);
	zd = zd * zd * (3.0f - 2.0f * zd);

	float xd1 = 1.0f - xd;
	float yd1 = 1.0f - yd;
	float zd1 = 1.0f - zd;

	destClr->x =

		Texture3DUtils::lerp(
			Texture3DUtils::lerp(
				Texture3DUtils::lerp(model[index].x, model[index + 1].x, xd1, xd),
				Texture3DUtils::lerp(model[index + 17].x, model[index + 17 + 1].x, xd1, xd), yd1, yd),
			Texture3DUtils::lerp(
				Texture3DUtils::lerp(model[index + 17 * 17].x, model[index + 17 * 17 + 1].x, xd1, xd),
				Texture3DUtils::lerp(model[index + 17 * 17 + 17].x, model[index + 17 * 17 + 17 + 1].x, xd1, xd), yd1, yd),
			zd1, zd
		);

	destClr->y =

		Texture3DUtils::lerp(
			Texture3DUtils::lerp(
				Texture3DUtils::lerp(model[index].y, model[index + 1].y, xd1, xd),
				Texture3DUtils::lerp(model[index + 17].y, model[index + 17 + 1].y, xd1, xd), yd1, yd),
			Texture3DUtils::lerp(
				Texture3DUtils::lerp(model[index + 17 * 17].y, model[index + 17 * 17 + 1].y, xd1, xd),
				Texture3DUtils::lerp(model[index + 17 * 17 + 17].y, model[index + 17 * 17 + 17 + 1].y, xd1, xd), yd1, yd),
			zd1, zd
		);

	destClr->z =

		Texture3DUtils::lerp(
			Texture3DUtils::lerp(
				Texture3DUtils::lerp(model[index].z, model[index + 1].z, xd1, xd),
				Texture3DUtils::lerp(model[index + 17].z, model[index + 17 + 1].z, xd1, xd), yd1, yd),
			Texture3DUtils::lerp(
				Texture3DUtils::lerp(model[index + 17 * 17].z, model[index + 17 * 17 + 1].z, xd1, xd),
				Texture3DUtils::lerp(model[index + 17 * 17 + 17].z, model[index + 17 * 17 + 17 + 1].z, xd1, xd), yd1, yd),
			zd1, zd
		);
}

/*inline float ModelTexture3D::lerp(float h1, float h2, float shift1, float shift) {
	return h1 * shift1 + h2 * shift;
}
*/ 

string ModelTexture3D::paramString() const {
	return "";
}


/*	if (v.x == 0.0f) {
		destClr->x = 1.0f;
		destClr->y = 1.0f;
		destClr->z = 1.0f;
	} else {
		destClr->x = 0.0f;
		destClr->y = 0.0f;
		destClr->z = 0.0f;
	}
	if (v.y == 63.0f) {
		destClr->x = 1.0f;
		destClr->y = 1.0f;
		destClr->z = 1.0f;
	} else {
		destClr->x = 0.0f;
		destClr->y = 0.0f;
		destClr->z = 0.0f;
	}
	if (v.z == 0.0f) {
		destClr->x = 1.0f;
		destClr->y = 1.0f;
		destClr->z = 1.0f;
	} else {
		destClr->x = 0.0f;
		destClr->y = 0.0f;
		destClr->z = 0.0f;
	}
*/
