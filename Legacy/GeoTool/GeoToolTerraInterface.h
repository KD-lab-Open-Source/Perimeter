#pragma once

#ifndef _GEO_TOOL_TERRA_INTERFACE_H
#define _GEO_TOOL_TERRA_INTERFACE_H

#include "TerraInterface.inl"
#include "ColorMapManager.h"

class GeoToolTerraInterface : public StandartTerraInterface {

	public:

		GeoToolTerraInterface(ColorMapManager* colorMapManager)
			: colorMapManager(colorMapManager) {
			showDamLayer = true;
		}

		void GetTileColor(
				char* Texture,
				DWORD pitch,
				int xstart,
				int ystart,
				int xend,
				int yend,
				int step ) {

			for(int y = ystart; y < yend; y += step)
			{
				DWORD* tx = (DWORD*) Texture;
				int yy = min(max(0, y), vMap.clip_mask_y);
				for (int x = xstart; x < xend; x += step) {
					int xx = min(max(0, x), vMap.clip_mask_x);
					DWORD color = 
						  colorMapManager->getColor(xx + vMap.H_SIZE * yy, showDamLayer)
						| 0xFF000000;
					*tx = color;
					tx++;
				}
				Texture += pitch;
			}
			
		}

		int GetZ(int x, int y) {
			int ofs = x + vMap.H_SIZE * y;
			return ((showDamLayer && vMap.VxDBuf[ofs] != 0) ? vMap.VxDBuf[ofs] : vMap.VxGBuf[ofs]);
		}

		bool showDamLayer;

	protected:

		ColorMapManager* colorMapManager;
};

#endif //_GEO_TOOL_TERRA_INTERFACE_H