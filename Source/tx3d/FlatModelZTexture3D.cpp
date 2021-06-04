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
#include "FlatModelZTexture3D.hpp"

using namespace tx3d;

const string FlatModelZTexture3D::TYPE_NAME = "FlatModelZ";

void FlatModelZTexture3D::getColor(Vector3D* destClr, const Vector3D &v) {

	long xi;
	long yi;
	float xd;
	float yd;

	xd = v.x * scale.x + shift.x;
	yd = v.y * scale.y + shift.y;

	xi = Texture3DUtils::floor(xd);
	yi = Texture3DUtils::floor(yd);

	xd -= xi;
	yd -= yi;

	xi %= (width - 1);
	yi %= (height - 1);

	if (xi < 0) {
		xi += (width - 1);
	}
	if (yi < 0) {
		yi += (height - 1);
	}

	long index = xi + yi * width;

	xd = xd * xd * (3.0f - 2.0f * xd);
	yd = yd * yd * (3.0f - 2.0f * yd);

	float xd1 = 1.0f - xd;
	float yd1 = 1.0f - yd;

	destClr->x =

		Texture3DUtils::lerp
		  (
			Texture3DUtils::lerp(model[index].x, model[index + 1].x, xd1, xd),
			Texture3DUtils::lerp(model[index + width].x, model[index + width + 1].x, xd1, xd),
			yd1, yd
		  );

	destClr->y =

		Texture3DUtils::lerp
		  (
			Texture3DUtils::lerp(model[index].y, model[index + 1].y, xd1, xd),
			Texture3DUtils::lerp(model[index + width].y, model[index + width + 1].y, xd1, xd),
			yd1, yd
		  );

	destClr->z =

		Texture3DUtils::lerp
		  (
			Texture3DUtils::lerp(model[index].z, model[index + 1].z, xd1, xd),
			Texture3DUtils::lerp(model[index + width].z, model[index + width + 1].z, xd1, xd),
			yd1, yd
		  );
}
