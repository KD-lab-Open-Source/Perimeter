#include <stdafx.h>
#include <xmath.h>
#include <Clear3D.hpp>
#include "ColorMapManager.h"
#include "terra.h"


const unsigned int ColorMapManager::EXACT_GEO_COLOR_MAP_BORDER_COLOR = 0x0000FFFF;

ColorMapManager::ColorMapManager() {
	zShift = 0;
	roughGeoColorMapStep = 8;
	roughGeoColorMap = 0;
	exactGeoColorMapRect.set(0, 0, 512, 512);
	exactGeoColorMap = 0;
	vMapWidth = 2096;
	vMapHeight = 2096;
	roughGeoColorMapWidth = vMapWidth / roughGeoColorMapStep;
	roughGeoColorMapHeight = vMapHeight / roughGeoColorMapStep;

	texture = new tx3d::Clear3D();
	if (texture) {
		texture->referenced();
	}
	indexedTexture = new tx3d::IndexedTexture3D(texture, indexLattice);
	shouldShowIndexed = false;
}

ColorMapManager::~ColorMapManager() {
	if (texture) {
		texture->released();
	}
	if (roughGeoColorMap) {
		delete roughGeoColorMap;
	}
	if (exactGeoColorMap) {
		delete exactGeoColorMap;
	}
}

void ColorMapManager::updateMaps() {
	Rect4i wholeWorld(0, 0, vMapWidth, vMapHeight); 
	updateRoughGeoColorMap(wholeWorld);
	updateExactGeoColorMap(wholeWorld);
}

void ColorMapManager::updateRoughGeoColorMap(const Rect4i& rect) {

	Rect4i vMapRect(0, 0, vMapWidth, vMapHeight);
	vMapRect.intersect(rect);
	Rect4i colorMapRect = vMapRect;
	colorMapRect.unZoom(roughGeoColorMapStep);
	updateGeoColorMap(
		roughGeoColorMap,
		roughGeoColorMapWidth,
		roughGeoColorMapHeight,
		colorMapRect,
		roughGeoColorMapStep,
		vMapRect);

}

void ColorMapManager::updateExactGeoColorMap(const Rect4i& rect) {

	Rect4i vMapRect = exactGeoColorMapRect.getIntersection(rect);
	Rect4i colorMapRect = vMapRect;
	colorMapRect.shift(-exactGeoColorMapRect.min);
	updateGeoColorMap(
		exactGeoColorMap,
		exactGeoColorMapRect.getWidth(),
		exactGeoColorMapRect.getHeight(),
		colorMapRect,
		1,
		vMapRect);

}


void ColorMapManager::updateGeoColorMap(
									unsigned int * colorMap,
									int colorMapWidth,
									int colorMapHeight,
									const Rect4i& colorMapRect,
									int vMapStep,
									const Rect4i& vMapRect
									) {

	paintGeoColorMap(colorMap, colorMapWidth, colorMapHeight, colorMapRect, vMapStep, vMapRect);
	lightUpGeoColorMap(colorMap, colorMapWidth, colorMapHeight, colorMapRect, vMapStep, vMapRect);
}

void ColorMapManager::paintGeoColorMap(
									unsigned int * colorMap,
									int colorMapWidth,
									int colorMapHeight,
									const Rect4i& colorMapRect,
									int vMapStep,
									const Rect4i& vMapRect
									) {
	tx3d::Vector3D point;
	tx3d::Vector3D color;
	int colorMapXS = colorMapRect.xMax();
	int colorMapYS = colorMapRect.yMax();
	for (
			int colorMapX = colorMapRect.xMin(), vMapX = vMapRect.xMin();
			colorMapX < colorMapXS;
			colorMapX++, vMapX += vMapStep
		) {

		for (
				int colorMapY = colorMapRect.yMin(), vMapY = vMapRect.yMin();
				colorMapY < colorMapYS;
				colorMapY++, vMapY += vMapStep
			) {

			point.x = vMapX;
			point.y = vMapY;
			point.z = vMap.GetAltGeo(vMapY * vMapWidth + vMapX) / 32.0 + zShift;
			if (shouldShowIndexed) {
				color = pal[indexedTexture->getColorIndex(point)];
			} else {
				texture->getColor(&color, point);
			}
			colorMap[colorMapY * colorMapWidth + colorMapX] = convertColor(color);
		}
	}

}

void ColorMapManager::lightUpGeoColorMap(
									unsigned int * colorMap,
									int colorMapWidth,
									int colorMapHeight,
									const Rect4i& colorMapRect,
									int vMapStep,
									const Rect4i& vMapRect
									) {

	int b = -100;

	float factor;
	float depth = 1.0f;

	int colorMapXS = colorMapRect.xMax();
	int colorMapYS = colorMapRect.yMax();
	for (
			int colorMapY = colorMapRect.yMin(), vMapY = vMapRect.yMin();
			colorMapY < colorMapYS;
			colorMapY++, vMapY += vMapStep
		) {

		for (
				int colorMapX = colorMapRect.xMin(), vMapX = vMapRect.xMin();
				colorMapX < colorMapXS;
				colorMapX++, vMapX += vMapStep
			) {

			int offset = vMapY * vMapWidth + vMapX;
			float h = vMap.GetAltGeo(offset);
			float h1 = vMap.GetAltGeo(offset + 1);
			factor = 1.178f - atan(	(h1 - h) / 160.0f );

			if (factor < 0.52f) {
				depth = 0.33f;
			} else if (factor <= 1.57f) {
				depth = factor / 1.57f;
			} else {
				depth = 2.0f - factor / 1.57f;
				depth = 0.5f * depth + 0.5f;
			}

			offset = colorMapY * colorMapWidth + colorMapX;
			unsigned int color = colorMap[offset];
			float b = (color & 255) / 255.0f * depth;
			float g = ((color >> 8) & 255) / 255.0f * depth;
			float r = ((color >> 16) & 255) / 255.0f * depth;
			colorMap[offset] = convertColor(tx3d::Vector3D(r, g, b));

		}
	}

}

void ColorMapManager::vMapLoaded() {
	vMapWidth = vMap.H_SIZE;
	vMapHeight = vMap.V_SIZE;
	roughGeoColorMapWidth = vMapWidth / roughGeoColorMapStep;
	roughGeoColorMapHeight = vMapHeight / roughGeoColorMapStep;
	exactGeoColorMapRect.set(0, 0, vMapWidth >= 1024 ? 1024 : 0, vMapHeight >= 1024 ? 1024 : 0);
	createRoughMap();
	createExactMap();
	updateMaps();
}

void ColorMapManager::setTexture3D(tx3d::Texture3D* newTexture) {
	if (newTexture) {
		if (texture) {
			texture->released();
		}
		newTexture->referenced();
		texture = newTexture;
		indexedTexture->setTexture(texture);
	}
	updateMaps();
}

void ColorMapManager::setIndexingParams(tx3d::Vector3D* newPal, unsigned char* newIndexLattice) {
	for (int i = 0; i < 256; i++) {
		pal[i] = newPal[i];
	}
	for (i = 0; i < 65536; i++) {
		indexLattice[i] = newIndexLattice[i];
	}
}

void ColorMapManager::updateRect(int x1, int y1, int x2, int y2) {
	updateRect(Rect4i(x1, y1, x2, y2));
}

void ColorMapManager::updateRect(const Rect4i& rect) {
	updateRoughGeoColorMap(rect);
	updateExactGeoColorMap(rect);
}

unsigned int ColorMapManager::getColor(int offset, bool showDamLayer) {
	int x = offset % vMapWidth;
	int y = offset / vMapWidth;

	if ( onExactMapBorder(x, y) ) {
		return EXACT_GEO_COLOR_MAP_BORDER_COLOR;
	} else if (!showDamLayer || vMap.VxDBuf[offset] == 0) {

		if ( exactGeoColorMapRect.inside(x, y) ) {
			return exactGeoColorMap[
				  (y - exactGeoColorMapRect.yMin()) * exactGeoColorMapRect.getWidth()
				+ (x - exactGeoColorMapRect.xMin()) ];
		} else {
			return roughGeoColorMap[
				  Rect4i::unZoom(y, roughGeoColorMapStep) * roughGeoColorMapWidth
				+ Rect4i::unZoom(x, roughGeoColorMapStep) ];
		}

	} else {
		return vMap.getColor32(offset);
	}
}

void ColorMapManager::shiftExactGeoColorMapRect(int x, int y) {
	exactGeoColorMapRect.shift(x, y);

	checkBounds();
	updateExactGeoColorMap(exactGeoColorMapRect);
}

void ColorMapManager::setExactGeoColorMapRect(const Rect4i& rect) {
	bool needRecreateMap = 
		   rect.getWidth() != exactGeoColorMapRect.getWidth()
		|| rect.getHeight() != exactGeoColorMapRect.getHeight();

	exactGeoColorMapRect = rect;

	checkBounds();

	if (needRecreateMap) {
		createExactMap();
	}
	updateExactGeoColorMap(exactGeoColorMapRect);
}

void ColorMapManager::setExactGeoColorMapRect(int x1, int y1, int x2, int y2) {
	bool needRecreateMap = 
		   (x2 - x1) != exactGeoColorMapRect.getWidth()
		|| (y2 - y1) != exactGeoColorMapRect.getHeight();

	exactGeoColorMapRect.set(x1, y1, x2, y2);

	checkBounds();

	if (needRecreateMap) {
		createExactMap();
	}
	updateExactGeoColorMap(exactGeoColorMapRect);
}

void ColorMapManager::createRoughMap() {
	if (roughGeoColorMap) {
		delete roughGeoColorMap;
	}
	roughGeoColorMap = new unsigned int [
			  roughGeoColorMapWidth 
			* roughGeoColorMapHeight ];
}

void ColorMapManager::createExactMap() {
	if (exactGeoColorMap) {
		delete exactGeoColorMap;
	}
	exactGeoColorMap = new unsigned int [
			  exactGeoColorMapRect.getWidth()
			* exactGeoColorMapRect.getHeight() ];
}

void ColorMapManager::checkBounds() {
	if (exactGeoColorMapRect.xMin() < 0) {
		exactGeoColorMapRect.xMax() = exactGeoColorMapRect.getWidth();
		exactGeoColorMapRect.xMin() = 0;
	}
	if (exactGeoColorMapRect.yMin() < 0) {
		exactGeoColorMapRect.yMax() = exactGeoColorMapRect.getHeight();
		exactGeoColorMapRect.yMin() = 0;
	}
	if (exactGeoColorMapRect.xMax() >= vMapWidth) {
		exactGeoColorMapRect.xMin() = vMapWidth - exactGeoColorMapRect.getWidth();
		exactGeoColorMapRect.xMax() = vMapWidth;
	}
	if (exactGeoColorMapRect.yMax() >= vMapWidth) {
		exactGeoColorMapRect.yMin() = vMapHeight - exactGeoColorMapRect.getHeight();
		exactGeoColorMapRect.yMax() = vMapHeight;
	}
}
