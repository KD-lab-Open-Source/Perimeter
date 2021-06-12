/**
 *	"Procedural 3D Texture Library"
 *	
 *	Implementation of a vector in 3D space of reals.
 *
 *	@version $Id: Vector3D.h,v 1.9 2002/11/05 17:47:50 ash Exp $
 *
 *	@author Alexander Porechnov (scm)
 */

#pragma once

#ifndef _TX3D_VECTORC3D_H
#define _TX3D_VECTORC3D_H

#include <my_STL.h>
#include <cmath>

namespace tx3d {

	class Vector3D {

		public :
			Vector3D(float x = 0.0, float y = 0.0, float z = 0.0) : x(x), y(y), z(z) {}

			float scalar() const {
				return
					(float) sqrt(x * x + y * y + z * z);
			}

			void abs() {
				x = (float) fabs(x);
				y = (float) fabs(y);
				z = (float) fabs(z);
			}

			void operator &= (const Vector3D& v) {
				x *= v.x;
				y *= v.y;
				z *= v.z;
			}

			void operator *= (float d) {
				x *= d;
				y *= d;
				z *= d;
			}

			void operator /= (float d) {
				x /= d;
				y /= d;
				z /= d;
			}

			void operator -= (float d) {
				x -= d;
				y -= d;
				z -= d;
			}

			void operator += (const Vector3D& v) {
				x += v.x;
				y += v.y;
				z += v.z;
			}

			void operator -= (const Vector3D& v) {
				x -= v.x;
				y -= v.y;
				z -= v.z;
			}

			void operator = (const Vector3D& v) {
				x = v.x;
				y = v.y;
				z = v.z;
			}

			Vector3D operator - ( const Vector3D& v) const {
				return
					Vector3D(x - v.x, y - v.y, z - v.z);
			}

			Vector3D operator + ( const Vector3D& v) const {
				return
					Vector3D(x + v.x, y + v.y, z + v.z);
			}

			Vector3D operator * (float d) const {
				return
					Vector3D(x * d, y * d, z * d);
			}

			bool operator == ( const Vector3D& v) const {
				return (x == v.x && y == v.y && z == v.z);

			}

			bool operator != ( const Vector3D& v) const {
				return (x != v.x || y != v.y || z != v.z);
			}

			float x;
			float y;
			float z;
	};

}

#endif