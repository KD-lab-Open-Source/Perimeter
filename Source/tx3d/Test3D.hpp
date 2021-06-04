/**
 *	"Procedural 3D Texture Library"
 *	
 *	
 *
 *	@version $Id: Primitive3D.h,v 1.10 2002/11/07 13:50:53 ash Exp $
 *
 *	@author Alexander Porechnov (scm)
 */

#pragma once

#ifndef _TX3D_TEST3D_H
#define _TX3D_TEST3D_H

#include "Texture3D.hpp"

namespace tx3d {

	class Test3D : public Texture3D {

		public :

			Test3D(
				float persistence,
				int octaveCount,
				const Vector3D& shift,
				const Vector3D& scale,
				const Vector3D& bgColor,
				const Vector3D& fgColor
			);

			~Test3D();

			void getColor(Vector3D* destClr, const Vector3D &v);

			const string& getType() const {
				return TYPE_NAME;
			}

			string paramString() const {
				return "";
			}

			void zoom(float degree) {
				shift *= degree;
				scale /= degree;
			}

			void zoomColor(float degree) {
				bgColor *= degree;
				diffColor *= degree;
			}

			static const string TYPE_NAME;

			float persistence;
			int octaveCount;

			Vector3D shift;
			Vector3D scale;

			Vector3D bgColor;
			Vector3D diffColor;

		protected :
			float interpolate(const Vector3D &v);

			float interpolate();

			float cosInterpolate(float h1, float h2, float shift);

			float getNoise(int x, int y, int z);

			float turbulate3D(const Vector3D &v, float persistence, int octaveCount);

			Vector3D shiftedV;
			Vector3D freqV;
			int xi;
			int yi;
			int zi;
			float xd;
			float yd;
			float zd;
			float vert1;
			float vert2;
			float vert3;
			float vert4;
			float vert5;
			float vert6;
			float vert7;
			float vert8;
			float coss;
			float noise;
	};

}

#endif