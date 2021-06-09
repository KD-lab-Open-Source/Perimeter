/**
 *	"Procedural 3D Texture Library"
 *	
 *	Class contains general utility methods.
 *
 *	@version $Id: Texture3DUtils.h,v 1.8 2002/11/05 17:55:27 ash Exp $
 *
 *	@author Alexander Porechnov (scm)
 */

#pragma once

#ifndef _TX3D_TEXTURE3DUTILS_H
#define _TX3D_TEXTURE3DUTILS_H

#include "Vector3D.hpp"

#include <cmath>

namespace tx3d {

	class Texture3DUtils {
		public :
			static float lerp(float h1, float h2, float shiftMinusOne, float shift) {
				return h1 * shiftMinusOne + h2 * shift;
			}

			static long round(float f) {
                return (long) std::round(f);
                /*
				long res;

				_asm {
					fld		f
					fistp	[dword ptr res]	
				};

				return res;
                */
			}

			static void round(float f, long* res) {
                *res = (long) std::round(f);
                /*
				_asm {
					fld		f
					fistp	[res]	
				};
                */
			}

			static long floor(float f) {
                return (long) std::floor(f);
                /*
				long res;
				float h = 0.5f - 1.e-6f;
				_asm {
					fld		h

					fld		f
					fsub	st(0), st(1)
					fistp	[dword ptr res]	

					fstp	st(0)
				};

				return res;
                */
			}

			/* TODO unused?
			static float ffloor(float f, float h = 0.5f) {
				long xi;
				float res;

				_asm {
					lea		esi, f
						
					fld		[dword ptr h]

					fld		[dword ptr esi]
					fsub	st(0), st(1)
					fistp	[dword ptr xi]	
					fild	[dword ptr xi]	
					fstp	[dword ptr res]	

					fstp	st(0)
				};

				return res;
			}
			*/

			static float reminder(float x, float y) {
                //TODO convert this to C
#ifdef _MSC_VER
                __asm {
                    fld y
                    fld x
                    fprem
                    fstp st(1)
                }
#else
                asm(R"(
                    fld y
                    fld x
                    fprem
                    fstp st(1)
                )");
#endif
			}


			static Vector3D convertHSBtoRGB(const Vector3D& hsbColor);

			static Vector3D convertRGBtoHSB(const Vector3D& rgbColor);

		private:
			Texture3DUtils();
	};

}

#endif