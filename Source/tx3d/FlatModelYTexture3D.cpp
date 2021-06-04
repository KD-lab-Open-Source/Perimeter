/**
 *	"Procedural 3D Texture Library"
 *	
 *
 *	@version $Id: ModelTexture3D.h,v 1.5 2002/11/07 13:50:53 ash Exp $
 *
 *	@author Alexander Porechnov (scm)
 */

#include "stdafxTX3D.h"
#include "Texture3DUtils.hpp"
#include "FlatModelYTexture3D.hpp"

using namespace tx3d;

const string FlatModelYTexture3D::TYPE_NAME = "FlatModelY";

void FlatModelYTexture3D::getColor(Vector3D* destClr, const Vector3D &v) {

	long xi;
	long zi;
	float xd;
	float zd;


	xd = v.x * scale.x + shift.x;
	zd = v.z * scale.z + shift.z;

	xi = Texture3DUtils::floor(xd);
	zi = Texture3DUtils::floor(zd);

	xd -= xi;
	zd -= zi;

	xi %= (width - 1);
	zi %= (height - 1);

	if (xi < 0) {
		xi += (width - 1);
	}
	if (zi < 0) {
		zi += (height - 1);
	}

	long index = xi + zi * width;

	xd = xd * xd * (3.0f - 2.0f * xd);
	zd = zd * zd * (3.0f - 2.0f * zd);

	float xd1 = 1.0f - xd;
	float zd1 = 1.0f - zd;

	destClr->x =

		Texture3DUtils::lerp
		  (
			Texture3DUtils::lerp(model[index].x, model[index + 1].x, xd1, xd),
			Texture3DUtils::lerp(model[index + width].x, model[index + width + 1].x, xd1, xd),
			zd1, zd
		  );

	destClr->y =

		Texture3DUtils::lerp
		  (
			Texture3DUtils::lerp(model[index].y, model[index + 1].y, xd1, xd),
			Texture3DUtils::lerp(model[index + width].y, model[index + width + 1].y, xd1, xd),
			zd1, zd
		  );

	destClr->z =

		Texture3DUtils::lerp
		  (
			Texture3DUtils::lerp(model[index].z, model[index + 1].z, xd1, xd),
			Texture3DUtils::lerp(model[index + width].z, model[index + width + 1].z, xd1, xd),
			zd1, zd
		  );
}
