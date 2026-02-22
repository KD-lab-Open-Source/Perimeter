#ifndef _COLOR_MAP_MANAGER_H
#define _COLOR_MAP_MANAGER_H

#include <tx3d.hpp>
//#include <Texture3D.hpp>
//#include <Texture3DUtils.hpp>
#include "Rect4i.h"

class ColorMapManager {

	public :

		ColorMapManager();
		~ColorMapManager();

		unsigned int getColor(int offset, bool showDamLayer);

		void vMapLoaded();

		void shiftExactGeoColorMapRect(int x, int y);
		void setExactGeoColorMapRect(int x1, int y1, int x2, int y2);
		void setExactGeoColorMapRect(const Rect4i& rect);

		void setTexture3D(tx3d::Texture3D* texture);
		void setIndexingParams(tx3d::Vector3D* pal, unsigned char* indexLattice);
		void showIndexed(bool show) {
			shouldShowIndexed = show;
			updateMaps();
		}
		void toggleShowIndexed() {
			showIndexed(!shouldShowIndexed);
		}

		void updateRect(int x1, int y1, int x2, int y2);
		void updateRect(const Rect4i& rect);

		inline Rect4i getExactGeoColorMapRect() const {
			return exactGeoColorMapRect;
		}

		void setZShift(int shift) {
			zShift = shift;
		}

		int getZShift() const {
			return zShift;
		}

	protected:

		static const unsigned int EXACT_GEO_COLOR_MAP_BORDER_COLOR;

		int zShift;

		int roughGeoColorMapStep;
		unsigned int * roughGeoColorMap;

		int vMapWidth;
		int vMapHeight;
		int roughGeoColorMapWidth;
		int roughGeoColorMapHeight;

		Rect4i exactGeoColorMapRect;
		unsigned int * exactGeoColorMap;

		tx3d::Texture3D* texture;

		tx3d::Vector3D pal[256];
		unsigned char indexLattice[65536];
		tx3d::IndexedTexture3D *indexedTexture;

		bool shouldShowIndexed;

		void updateMaps();
		void updateRoughGeoColorMap(const Rect4i& rect);
		void updateExactGeoColorMap(const Rect4i& rect);
		void updateGeoColorMap(
							unsigned int * colorMap,
							int colorMapWidth,
							int colorMapHeight,
							const Rect4i& colorMapRect,
							int vMapStep,
							const Rect4i& vMapRect
								);
		void paintGeoColorMap(
							unsigned int * colorMap,
							int colorMapWidth,
							int colorMapHeight,
							const Rect4i& colorMapRect,
							int vMapStep,
							const Rect4i& vMapRect
								);

		void lightUpGeoColorMap(
							unsigned int * colorMap,
							int colorMapWidth,
							int colorMapHeight,
							const Rect4i& colorMapRect,
							int vMapStep,
							const Rect4i& vMapRect
								);

		void createRoughMap();
		void createExactMap();
		void checkBounds();

		static unsigned int convertColor(tx3d::Vector3D& color) {
			return
				  (tx3d::Texture3DUtils::round(255 * 255) << 24)
				| (tx3d::Texture3DUtils::round(255 * color.x) << 16) 
				| (tx3d::Texture3DUtils::round(255 * color.y) << 8)
				| (tx3d::Texture3DUtils::round(255 * color.z));
		}

		static unsigned int convertColor(int r, int g, int b) {
			return
				  (255 * 255 << 24)
				| (255 * r << 16) 
				| (255 * g << 8)
				| (255 * b);
		}

		inline bool onExactMapBorder(int x, int y) {
			if ( x > exactGeoColorMapRect.xMin() && x < exactGeoColorMapRect.xMax() ) {
				return
					   y == exactGeoColorMapRect.yMin()
					|| y == exactGeoColorMapRect.yMax();
			} else if (y > exactGeoColorMapRect.yMin() && y < exactGeoColorMapRect.yMax()) {
				return
					   x == exactGeoColorMapRect.xMin()
					|| x == exactGeoColorMapRect.xMax();
			} else {
				return false;
			}
		}

};

#endif //_COLOR_MAP_MANAGER_H
