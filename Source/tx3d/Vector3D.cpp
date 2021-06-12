/**
 *	"Procedural 3D Texture Library"
 *	
 *	Implementation of a vector in 3D space of reals.
 *
 *	@version $Id: Vector3D.h,v 1.9 2002/11/05 17:47:50 ash Exp $
 *
 *	@author Alexander Porechnov (scm)
 */

#include "stdafxTX3D.h"
#include "Vector3D.hpp"
#include <cmath>

using namespace tx3d;
/*
void Vector3D::operator &= (const Vector3D& v) {
	x *= v.x;
	y *= v.y;
	z *= v.z;
}

void Vector3D::operator *= (double d) {
	x *= d;
	y *= d;
	z *= d;
}

void Vector3D::operator += (const Vector3D& v) {
	x += v.x;
	y += v.y;
	z += v.z;
}

void Vector3D::operator -= (const Vector3D& v) {
	x -= v.x;
	y -= v.y;
	z -= v.z;
}

void Vector3D::operator = (const Vector3D& v) {
	x = v.x;
	y = v.y;
	z = v.z;
}

Vector3D Vector3D::operator - ( const Vector3D& v) const {
	return
		Vector3D(x - v.x, y - v.y, z - v.z);
}

Vector3D Vector3D::operator + ( const Vector3D& v) const {
	return
		Vector3D(x + v.x, y + v.y, z + v.z);
}

Vector3D Vector3D::operator * (double d) const {
	return
		Vector3D(x * d, y * d, z * d);
}

bool Vector3D::operator == ( const Vector3D& v) const {
	return (x == v.x && y == v.y && z == v.z);

}

bool Vector3D::operator != ( const Vector3D& v) const {
	return (x != v.x || y != v.y || z != v.z);
}
*/