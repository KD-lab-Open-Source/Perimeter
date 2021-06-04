/**
 *	"Procedural 3D Texture Library"
 *	
 *	Implementation of texture primitive.
 *
 *	@version $Id: Primitive3D.h,v 1.10 2002/11/07 13:50:53 ash Exp $
 *
 *	@author Alexander Porechnov (scm)
 */

#pragma once

#ifndef _TX3D_PRIMITIVE3D_H
#define _TX3D_PRIMITIVE3D_H

#include "Texture3D.hpp"
#include "Colorizer3D.hpp"
#include "Turbulator3D.hpp"
#include "Interpolator3D.hpp"

namespace tx3d {

	class Primitive3D : public Texture3D {

		public :

			Primitive3D(const string& paramString);

			Primitive3D(
				float persistence,
				int octaveCount,
				const Vector3D& shift,
				const Vector3D& scale,
				Colorizer3D *colorizer,
				Interpolator3D *interpolator,
				Turbulator3D *turbulator = 0
			);

			~Primitive3D();

			void getColor(Vector3D* destClr, const Vector3D &v) {
				shiftedV = v;
				shiftedV &= scale;
				shiftedV += shift;

				colorizer->computeColor( destClr,
										 shiftedV,
										 turbulator->turbulate3D( shiftedV,
																  persistence,
																  octaveCount,
																  interpolator )  );
			}

			void getColor(Vector3D* destClr, const Vector3D &v, float gPersistence, int gOctaveCount) {
				shiftedV = v;
				shiftedV &= scale;
				shiftedV += shift;

				colorizer->computeColor( destClr,
										 shiftedV,
										 turbulator->turbulate3D( shiftedV,
																  gPersistence,
																  gOctaveCount,
																  interpolator )  );
			}

			void zoom(float degree) {
				shift *= degree;
				scale /= degree;
			}

			void zoomColor(float degree) {
				colorizer->zoomColor(degree);
			}

			Colorizer3D* getColorizer() const {
				return colorizer;
			}

			void setColorizer(Colorizer3D *c);

			Turbulator3D* getTurbulator() const {
				return turbulator;
			}

			void setTurbulator(Turbulator3D *t);

			const string& getType() const {
				return TYPE_NAME;
			}

			string paramString() const;

			static const string TYPE_NAME;
			static const string PERSISTENCE;
			static const string OCTAVE_COUNT;
			static const string INTERPOLATOR;
			static const string COLORIZER;
			static const string SHIFT;
			static const string SCALE;

			float persistence;
			int octaveCount;

			Vector3D shift;
			Vector3D scale;

			Interpolator3D *interpolator;


		protected :
			Colorizer3D *colorizer;
			Turbulator3D *turbulator;

			Vector3D shiftedV;
	};

}

#endif