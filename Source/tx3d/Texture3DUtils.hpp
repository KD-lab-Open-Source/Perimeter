/**
 *	"Procedural 3D Texture Library"
 *	
 *	Class contains general utility methods.
 *
 *	@version $Id: Texture3DUtils.h,v 1.8 2002/11/05 17:55:27 ash Exp $
 *
 *	@author Alexander Porechnov (scm)
 */

#ifndef _TX3D_TEXTURE3DUTILS_H
#define _TX3D_TEXTURE3DUTILS_H

#include "xmath.h"
#include "Vector3D.hpp"

namespace tx3d {

	class Texture3DUtils {
		public :
			static float lerp(float h1, float h2, float shiftMinusOne, float shift) {
				return h1 * shiftMinusOne + h2 * shift;
			}

			static long round(float f) {
                return static_cast<long>(xm::round(f));
			}

			static void round(float f, long* res) {
                *res = static_cast<long>(xm::round(f));
			}

			static long floor(float f) {
                return static_cast<long>(xm::floor(f));
			}

			static Vector3D convertHSBtoRGB(const Vector3D& hsbColor);

			static Vector3D convertRGBtoHSB(const Vector3D& rgbColor);

		private:
			Texture3DUtils();
	};

}

#endif