#ifndef _VMAP_OPERATOR_H
#define _VMAP_OPERATOR_H

#include "Rect4i.h"
#include "Updater.h"
#include "terra.h"

class VMapOperator {

	public :

		VMapOperator(Updater* updater);
		~VMapOperator();

		typedef enum {
			PIT_BRUSH,
			PIT_BRUSH_BIG
		} BrushType;

		void setBrush(BrushType type);

		void removeDamLayer(const Rect4i& rect = Rect4i(0, 0, vMap.H_SIZE, vMap.V_SIZE));
		void planeDamLayer(int height, const Rect4i& rect = Rect4i(0, 0, vMap.H_SIZE, vMap.V_SIZE));
		void planeGeoLayer(int height, const Rect4i& rect = Rect4i(0, 0, vMap.H_SIZE, vMap.V_SIZE));
		void planeToZeroplast(const Rect4i& rect = Rect4i(0, 0, vMap.H_SIZE, vMap.V_SIZE));
		void dig(const Vect3f& center, float factor);

		static Rect4i getAdjustedRect(Vect3f* center, int width, int height);
		static void loadMap(int mapNumber);
		static void prepareVMap();
	
	protected :
		Updater* updater;

		unsigned char* brush;
		int brushWidth;
		int brushHeight;

		unsigned char* pitBrush32x32;
		unsigned char* pitBrush256x256;

		individualToolzer<T2TE_ALIGNMENT_TERRAIN_VARIABLE_H> toolzerAligmentTerrainVariableH;

};

#endif // _VMAP_OPERATOR_H