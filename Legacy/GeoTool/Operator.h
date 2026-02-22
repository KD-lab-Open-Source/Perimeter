#ifndef _OPERATOR_H
#define _OPERATOR_H

#include "VMapOperator.h"
#include "ColorMapManager.h"
#include <Texture3D.hpp>

class Operator {
	public:
		virtual ~Operator() {
		}

		virtual bool cursorTrace(Vect3f* p, Vect3f* dir) = 0;
		virtual bool cursorTrace(Vect3f* vMapPoint) = 0;
		virtual bool cursorTrace(Vect3f* p, Vect3f* dir, Vect3f* dir2) = 0;

		virtual void shiftExactColorMap(Vect3f* v) = 0;

		virtual VMapOperator* getVMapOperator() = 0;
		virtual ColorMapManager* getColorMapManager() = 0;
		virtual void setTexture(tx3d::Texture3D* tx) = 0;
		virtual bool getViewportPos(Vect2f* pos) = 0;
};

#endif // _UPDATER_H