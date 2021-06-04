/**
 *	"Procedural 3D Texture Library"
 *	
 *	
 *
 *	@version $Id: Interpolator3DFactory.cpp,v 1.1 2002/11/07 13:49:48 ash Exp $
 *
 *	@author Alexander Porechnov (scm)
 */

#include "stdafxTX3D.h"
#include "Interpolator3DFactory.hpp"
#include "XMLUtils.hpp"

using namespace tx3d;

Interpolator3DFactory *Interpolator3DFactory::factory = 0;

Interpolator3D* Interpolator3DFactory::getInterpolator3D(const string& xml) {
	string type = XMLUtils::extractPropertyFromTag("type", xml);
	return getInterpolator3DByType(type);
}

Interpolator3D* Interpolator3DFactory::getInterpolator3DByType(const string& type) {
	if (type == CubicInterpolator3D::TYPE_NAME) {
		if (!cub) {
			cub = new CubicInterpolator3D();
		}
		return cub;
	}
	if (type == BackCubicInterpolator3D::TYPE_NAME) {
		if (!backCub) {
			backCub = new BackCubicInterpolator3D();
		}
		return backCub;
	}
	if (type == CosInterpolator3D::TYPE_NAME) {
		if (!cos) {
			cos = new CosInterpolator3D();
		}
		return cos;
	}
	if (type == BackCosInterpolator3D::TYPE_NAME) {
		if (!backCos) {
			backCos = new BackCosInterpolator3D();
		}
		return backCos;
	}
	if (type == SphericSimplexInterpolator3D::TYPE_NAME) {
		if (!sph) {
			sph = new SphericSimplexInterpolator3D();
		}
		return sph;
	}
	return 0;
}