/**
 *	"Procedural 3D Texture Library"
 *	
 *	
 *
 *	@version $Id: Interpolator3DFactory.h,v 1.1 2002/11/07 13:49:48 ash Exp $
 *
 *	@author Alexander Porechnov (scm)
 */

#pragma once

#ifndef _TX3D_INTERPOLATOR3DFACTORY_H
#define _TX3D_INTERPOLATOR3DFACTORY_H

#include "BackCubicInterpolator3D.hpp"
#include "BackCosInterpolator3D.hpp"
#include "SphericSimplexInterpolator3D.hpp"

namespace tx3d {

	class Interpolator3DFactory {
		public:
			Interpolator3D* getInterpolator3D(const string& xml);
			Interpolator3D* getInterpolator3DByType(const string& type);

			static Interpolator3DFactory& getFactory() {
				if (!factory) {
					factory = new Interpolator3DFactory();
				}
				return *factory;
			}

			~Interpolator3DFactory() {
				if (factory) {
					delete factory;
				}
			}

		protected:
			static Interpolator3DFactory* factory;
			CosInterpolator3D *cos;
			BackCosInterpolator3D *backCos;
			SphericSimplexInterpolator3D *sph;
			CubicInterpolator3D* cub;
			BackCubicInterpolator3D* backCub;
			Interpolator3DFactory() : cos(0), backCos(0), sph(0), cub(0), backCub(0) {}
	};

}

#endif