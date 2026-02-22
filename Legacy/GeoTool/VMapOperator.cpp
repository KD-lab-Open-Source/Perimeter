#include <stdafx.h>
#include <Clear3D.hpp>
#include <Texture3DFactory.hpp>

#include "VMapOperator.h"
#include "../Terra/terra.h"
#include "../Render/src/FileImage.h"


VMapOperator::VMapOperator(Updater* updater) : updater(updater) {

	pitBrush32x32 = 0;
	pitBrush256x256 = 0;

	char dir[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, dir);
	strcat(dir, "\\GeoTool\\res\\");

	int width;
	int height;
	int bpp;
	
	//load brushes

	if (
		   !LoadTGA((string(dir) + "pitBrush32x32.tga").c_str(), width, height, pitBrush32x32, bpp)
		|| width != 32
		|| height != 32
		|| bpp != 1

		) {

		ErrH.Abort("pitBrush32x32.tga not found or it isn't grayscale 32 x 32 TGA");
	}

	if (
		   !LoadTGA((string(dir) + "pitBrush256x256.tga").c_str(), width, height, pitBrush256x256, bpp)
		|| width != 256
		|| height != 256
		|| bpp != 1

		) {

		ErrH.Abort("pitBrush256x256.tga not found or it isn't grayscale 256 x 256 TGA");
	}

	brush = pitBrush32x32;
	brushWidth = 32;
	brushHeight = 32;
}

VMapOperator::~VMapOperator() {
	//release brushes
	if (pitBrush32x32) {
		delete [] pitBrush32x32;
	}
	if (pitBrush256x256) {
		delete [] pitBrush256x256;
	}
}

void VMapOperator::setBrush(BrushType type) {
	switch (type) {
		case PIT_BRUSH:
			brush = pitBrush32x32;
			brushWidth = 32;
			brushHeight = 32;
			break;
		case PIT_BRUSH_BIG:
			brush = pitBrush256x256;
			brushWidth = 256;
			brushHeight = 256;
			break;
		default:
			brush = pitBrush32x32;
			brushWidth = 32;
			brushHeight = 32;
			break;
	}
}

void VMapOperator::removeDamLayer(const Rect4i& rect) {
	for (int y = rect.yMin(); y < rect.yMax(); y++) {
		int offset = y * vMap.H_SIZE;
		for (int x = rect.xMin(); x < rect.xMax(); x++) {
			vMap.VxDBuf[offset + x] = 0;
		}
	}
	updater->updateRect(rect);
}

void VMapOperator::planeToZeroplast(const Rect4i& rect) {
	for (int y = rect.yMin(); y < rect.yMax(); y++) {
		int offset = y * vMap.H_SIZE;
		for (int x = rect.xMin(); x < rect.xMax(); x++) {
			vMap.VxGBuf[offset + x] = vMap.hZeroPlast;
			vMap.AtrBuf[offset + x] = 0;
			vMap.VxDBuf[offset + x] = 0;
		}
	}
	updater->updateRect(rect);
}

void VMapOperator::planeDamLayer(int height, const Rect4i& rect) {
	if (height < 0) {
		height = vMap.hZeroPlast;
	}
	for (int y = rect.yMin(); y < rect.yMax(); y++) {
		int offset = y * vMap.H_SIZE;
		for (int x = rect.xMin(); x < rect.xMax(); x++) {
			if (vMap.VxDBuf[offset + x] > height) {
				if (vMap.VxGBuf[offset + x] > height) {
					vMap.VxDBuf[offset + x] = 0;
					vMap.VxGBuf[offset + x] = height;
				} else {
					vMap.VxDBuf[offset + x] = height;
				}
			}
		}
	}
	updater->updateRect(rect);
}

void VMapOperator::planeGeoLayer(int height, const Rect4i& rect) {
	for (int y = rect.yMin(); y < rect.yMax(); y++) {
		int offset = y * vMap.H_SIZE;
		for (int x = rect.xMin(); x < rect.xMax(); x++) {
			vMap.VxGBuf[offset + x] = height;
			vMap.AtrBuf[offset + x] = 0;
			if (vMap.VxDBuf[offset + x] < height) {
				vMap.VxDBuf[offset + x] = 0;
			}
		}
	}
	updater->updateRect(rect);
}

void VMapOperator::dig(const Vect3f& center, float factor) {

	int brWidthHalf = brushWidth / 2;
	int brHeightHalf = brushHeight / 2;

	Rect4i digArea(
		center.x - brWidthHalf,
		center.y - brHeightHalf,
		center.x + brWidthHalf,
		center.y + brHeightHalf);

	Rect4i vMapRect = digArea;
	vMapRect.intersect(Rect4i(0, 0, vMap.H_SIZE - 1, vMap.V_SIZE - 1));
	if (vMapRect.getWidth() > 0 && vMapRect.getHeight() > 0) {
		int brushX = digArea.xMin() - vMapRect.xMin();
		int brushY = digArea.yMin() - vMapRect.yMin();
		for (int x = vMapRect.xMin(), brX = brushX; x < vMapRect.xMax(); x++, brX++) {
			for (int y = vMapRect.yMin(), brY = brushY; y < vMapRect.yMax(); y++, brY++) {
				int offset = y * vMap.H_SIZE + x;
				int depth = brush[brY * brushWidth + brX] * factor;
				int height = vMap.VxDBuf[offset];
				if (height == 0) {
					height = vMap.VxGBuf[offset] - depth;
					if (height < 0) {
						vMap.VxGBuf[offset] = 0;
						vMap.AtrBuf[offset] = 0;
					} else {
						vMap.VxGBuf[offset] = height;
					}
				} else {
					height -= depth;
					if (height < 0) {
						vMap.VxDBuf[offset] = 0;
						vMap.VxGBuf[offset] = 0;
						vMap.AtrBuf[offset] = 0;
					} else if (vMap.VxGBuf[offset] > height) {
						vMap.VxDBuf[offset] = 0;
						vMap.VxGBuf[offset] = height;
					} else {
						vMap.VxDBuf[offset] = height;
					}
				}
			}
		}
		toolzerAligmentTerrainVariableH.influenceDZ(center.x, center.y, max(brWidthHalf, brHeightHalf), 0<<VX_FRACTION, 10 );
	/*
		toolzerAligmentTerrainVariableH.influenceDZ(v.x, v.y, 5, -4<<VX_FRACTION, 3 );
		toolzerAligmentTerrainVariableH.influenceDZ(v.x, v.y, 16, 0<<VX_FRACTION, 10 );
		toolzerAligmentTerrainVariableH.influenceDZ(v.x, v.y, 5, -4<<VX_FRACTION, 3 );
		toolzerAligmentTerrainVariableH.influenceDZ(v.x, v.y, 16, 0<<VX_FRACTION, 7 );
		toolzerAligmentTerrainVariableH.influenceDZ(v.x, v.y, 5, -4<<VX_FRACTION, 3 );
		toolzerAligmentTerrainVariableH.influenceDZ(v.x, v.y, 16, 0<<VX_FRACTION, 5 );
		toolzerAligmentTerrainVariableH.influenceDZ(v.x, v.y, 5, -4<<VX_FRACTION, 3 );
		toolzerAligmentTerrainVariableH.influenceDZ(v.x, v.y, 16, 0<<VX_FRACTION, 3 );
		toolzerAligmentTerrainVariableH.influenceDZ(v.x, v.y, 16, 0<<VX_FRACTION, 2 );
	*/
		vMap.regRender(vMapRect.xMin(), vMapRect.yMin(), vMapRect.xMax(), vMapRect.yMax());
		updater->updateRect(vMapRect);
	}
}

void VMapOperator::prepareVMap() {
	vMap.prepare("RESOURCE\\WORLDS\\worlds.prm", NULL, NULL, 0, 0, 0);
}

void VMapOperator::loadMap(int mapNumber) {
	vMap.selectUsedWorld(mapNumber);
	vMap.ShadowControl(false);
	vMap.fullLoad(false);

	vMap.WorldRender();
}

Rect4i VMapOperator::getAdjustedRect(Vect3f* center, int width, int height) {
	width /= 2;
	height /= 2;
	Rect4i res(center->x - width, center->y - height, center->x + width, center->y + height);
	res.intersect(Rect4i(0, 0, vMap.H_SIZE, vMap.V_SIZE));
	return res;
}
