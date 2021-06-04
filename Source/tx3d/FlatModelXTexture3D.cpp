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
#include "FlatModelXTexture3D.hpp"
//#include "xutil.h"

using namespace tx3d;

const string FlatModelXTexture3D::TYPE_NAME = "FlatModelX";

void FlatModelXTexture3D::getColor(Vector3D* destClr, const Vector3D &v) {

	long yi;
	long zi;
	float yd;
	float zd;

	yd = v.y * scale.y + shift.y;
	zd = v.z * scale.z + shift.z;

	yi = Texture3DUtils::floor(yd);
	zi = Texture3DUtils::floor(zd);

	yd -= yi;
	zd -= zi;

	yi %= (width - 1);
	zi %= (height - 1);

	if (yi < 0) {
		yi += (width - 1);
	}
	if (zi < 0) {
		zi += (height - 1);
	}

	long index = yi + zi * width;

	yd = yd * yd * (3.0f - 2.0f * yd);
	zd = zd * zd * (3.0f - 2.0f * zd);

	float yd1 = 1.0f - yd;
	float zd1 = 1.0f - zd;

	destClr->x =

		Texture3DUtils::lerp
		  (
			Texture3DUtils::lerp(model[index].x, model[index + 1].x, yd1, yd),
			Texture3DUtils::lerp(model[index + width].x, model[index + width + 1].x, yd1, yd),
			zd1, zd
		  );

	destClr->y =

		Texture3DUtils::lerp
		  (
			Texture3DUtils::lerp(model[index].y, model[index + 1].y, yd1, yd),
			Texture3DUtils::lerp(model[index + width].y, model[index + width + 1].y, yd1, yd),
			zd1, zd
		  );

	destClr->z =

		Texture3DUtils::lerp
		  (
			Texture3DUtils::lerp(model[index].z, model[index + 1].z, yd1, yd),
			Texture3DUtils::lerp(model[index + width].z, model[index + width + 1].z, yd1, yd),
			zd1, zd
		  );
}
