#include <stdafx.h>
#include <Location3DFactory.hpp>
#include <StratumLocation3D.hpp>
#include <SphereLocation3D.hpp>
#include "GeoTool.h"
#include "LocationNode.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const string LocationNode::LOCATION = "location";

LocationNode::LocationNode(const string& paramString) : Node(paramString) {
	
	location = loadXML(paramString, LOCATION);

	tx3d::Location3D* master = createLocation();
	if (master) {
		master->referenced();
		typeName = master->getType();
		master->released();
	}

}

bool LocationNode::isLocationFromNode(tx3d::Location3D* loc) {
	if (loc->getType() == typeName) {
		return true;
	} else {
		tx3d::StratumLocation3D* stratumLoc = dynamic_cast<tx3d::StratumLocation3D*>(loc);
		if (stratumLoc) {
			return typeName.substr(
								0,
								typeName.length() - 1)
				== stratumLoc->getType().substr(
								0,
								stratumLoc->getType().length() - 1);
		}
	}
	return false;
}

tx3d::Location3D* LocationNode::createLocation() {
	return tx3d::Location3DFactory::createLocation3D(location);
}

void LocationNode::updateObject(tx3d::Location3D* location, cObjectNodeRoot* object) {
	if (object) {
		tx3d::SphereLocation3D* sphere = dynamic_cast<tx3d::SphereLocation3D*>(location);
		if (sphere) {
			updateSphereObject(sphere, object);
			return;
		}
		tx3d::StratumLocation3D* stratum = dynamic_cast<tx3d::StratumLocation3D*>(location);
		if (stratum) {
			updateStratumObject(stratum, object);
			return;
		}
	}
}

void LocationNode::updateSphereObject(tx3d::SphereLocation3D* sphere, cObjectNodeRoot* object) {
	MatXf m(Mat3f::ID, Vect3f(sphere->center.x, sphere->center.y, sphere->center.z));
	float r = sphere->radius;
	Scale(m, Vect3f(r, r, r));
	object->SetPosition(m);
}

void LocationNode::updateStratumObject(tx3d::StratumLocation3D* stratum, cObjectNodeRoot* object) {
	char end = stratum->getType()[stratum->getType().length() - 1];
	switch (end) {
		case 'X':
			{
				MatXf m(
					Mat3f(M_PI / 2, Y_AXIS),
					Vect3f(stratum->nearBound, vMap.V_SIZE / 2, 0) );
				float w = stratum->farBound - stratum->nearBound;
				Scale(m, Vect3f(600, vMap.V_SIZE, w));
				object->SetPosition(m);
			}
			break;
		case 'Y':
			{
				MatXf m(
					Mat3f(- M_PI / 2, X_AXIS),
					Vect3f(vMap.H_SIZE / 2, stratum->nearBound, 0) );
				float w = stratum->farBound - stratum->nearBound;
				Scale(m, Vect3f(vMap.H_SIZE, 600, w));
				object->SetPosition(m);
			}
			break;
		case 'Z':
			{
				MatXf m( Mat3f::ID, Vect3f(vMap.H_SIZE / 2, vMap.V_SIZE / 2, stratum->nearBound) );
				float w = stratum->farBound - stratum->nearBound;
				Scale(m, Vect3f(vMap.H_SIZE, vMap.V_SIZE, w));
				object->SetPosition(m);
			}
			break;
	}
}

bool LocationNode::update(
			tx3d::Location3D* location,
			cObjectNodeRoot* object,
			const Vect3f& point,
			const Vect3f& direction,
			const Vect3f& direction2,
			LocationUpdater::ConstraintType type ) {

	if (object) {
		tx3d::SphereLocation3D* sphere = dynamic_cast<tx3d::SphereLocation3D*>(location);
		if (sphere) {
			return updateSphere(sphere, object, point, direction, direction2, type);
		}
		tx3d::StratumLocation3D* stratum = dynamic_cast<tx3d::StratumLocation3D*>(location);
		if (stratum) {
			return updateStratum(stratum, object, point, direction, direction2, type);
		}
	}
	return false;
}

bool LocationNode::updateSphere(
			tx3d::SphereLocation3D* sphere,
			cObjectNodeRoot* object,
			const Vect3f& point,
			const Vect3f& direction,
			const Vect3f& direction2,
			LocationUpdater::ConstraintType type ) {

	switch (type) {
		case LocationUpdater::SCALE :
			{
				Vect3f center = Vect3f(sphere->center.x, sphere->center.y, sphere->center.z);
				Vect3f aLine = point;
				sphere->radius = GetDistFromPointToLine(center, aLine, aLine + direction);
				updateSphereObject(sphere, object);
				return true;
			}
		case LocationUpdater::SHIFT_XY :
			{
				if (direction.z == 0) {
					return false;
				}
				Vect3f center =  
					  point
					- direction * ( (point.z - sphere->center.z) / direction.z );
				sphere->center.x = center.x;
				sphere->center.y = center.y;
				updateSphereObject(sphere, object);
				return true;
			}
		case LocationUpdater::SHIFT_ZY :
			{
				sPlane4f plane(point, point + direction, point + direction2);
				Vect3f a(sphere->center.x, sphere->center.y, 0);
				Vect3f b(sphere->center.x, sphere->center.y, 100);
				sphere->center.z = plane.GetCross(a, b) * 100;
				updateSphereObject(sphere, object);
				return true;
			}
		default:
			return false;
	}
}

bool LocationNode::updateStratum(
			tx3d::StratumLocation3D* stratum,
			cObjectNodeRoot* object,
			const Vect3f& point,
			const Vect3f& direction,
			const Vect3f& direction2,
			LocationUpdater::ConstraintType type ) {

	char end = stratum->getType()[stratum->getType().length() - 1];
	switch (type) {
		case LocationUpdater::SCALE :
			{
				switch (end) {
					case 'X':
						{
							Vect3f center =  
									  point
									- direction * ( point.z / direction.z );
							float middle = (stratum->farBound + stratum->nearBound) / 2;
							if ( middle > center.x ) {
								stratum->nearBound = center.x;
							} else {
								stratum->farBound = center.x;
							}
						}
						break;
					case 'Y':
						{
							Vect3f center =  
									  point
									- direction * ( point.z / direction.z );
							float middle = (stratum->farBound + stratum->nearBound) / 2;
							if ( middle > center.y ) {
								stratum->nearBound = center.y;
							} else {
								stratum->farBound = center.y;
							}
						}
						break;
					case 'Z':
						{
							Vect3f center = getCrossCenter(point, direction);
							float middle = (stratum->farBound + stratum->nearBound) / 2;
							if ( middle > center.z ) {
								stratum->nearBound = center.z;
							} else {
								stratum->farBound = center.z;
							}
						}
						break;
				}
				updateStratumObject(stratum, object);
				return true;
			}
		case LocationUpdater::SHIFT_XY :
			{
				if (end == 'Z' || direction.z == 0) {
					return false;
				}
				Vect3f center =  
					  point
					- direction * ( point.z / direction.z );
				float width = (stratum->farBound - stratum->nearBound) / 2;
				if (end == 'X') {
					stratum->nearBound = center.x - width; 
					stratum->farBound = center.x + width;
				} else {
					stratum->nearBound = center.y - width; 
					stratum->farBound = center.y + width;
				}
				updateStratumObject(stratum, object);
				return true;
			}
		case LocationUpdater::SHIFT_ZY :
			{
				if (end != 'Z') {
					return false;
				}
				Vect3f center = getCrossCenter(point, direction);
				float width = (stratum->farBound - stratum->nearBound) / 2;
				stratum->nearBound = center.z - width; 
				stratum->farBound = center.z + width;
				updateStratumObject(stratum, object);
				return true;
			}
		default:
			return false;
	}	
}

Vect3f LocationNode::getCrossCenter(const Vect3f& point, const Vect3f& direction) {
	Vect3f zero(0, 0, 0);
	Vect3f x(vMap.H_SIZE, 0, 0);
	Vect3f y(0, vMap.V_SIZE, 0);
	Vect3f xy(vMap.H_SIZE, vMap.V_SIZE, 0);

	float zeroNorm2 = (point - zero).norm2();
	float xNorm2 = (point - x).norm2();
	float yNorm2 = (point - y).norm2();
	float xyNorm2 = (point - xy).norm2();

	float minCorner = min( zeroNorm2, min(xNorm2, min(yNorm2, xyNorm2)) );

	Vect3f center;
	Vect3f pointB = point - direction;
	if (minCorner == zeroNorm2) {
		if (xNorm2 < yNorm2) {
			center = cross(sPlane4f(zero, x, Vect3f(0, 0, 100)), point, pointB, direction);
			if (center.x < 0) {
				center = cross(sPlane4f(zero, y, Vect3f(0, 0, 100)), point, pointB, direction);
			}
		} else {
			center = cross(sPlane4f(zero, y, Vect3f(0, 0, 100)), point, pointB, direction);
			if (center.y < 0) {
				center = cross(sPlane4f(zero, x, Vect3f(0, 0, 100)), point, pointB, direction);
			}
		}
	} else if (minCorner == xNorm2) {
		if (zeroNorm2 > xyNorm2) {
			center = cross(sPlane4f(xy, x, Vect3f(vMap.H_SIZE, 0, 100)), point, pointB, direction);
			if (center.y < 0) {
				center = cross(sPlane4f(zero, x, Vect3f(vMap.H_SIZE, 0, 100)), point, pointB, direction);
			}
		} else {
			center = cross(sPlane4f(zero, x, Vect3f(vMap.H_SIZE, 0, 100)), point, pointB, direction);
			if (center.x > vMap.H_SIZE) {
				center = cross(sPlane4f(xy, x, Vect3f(vMap.H_SIZE, 0, 100)), point, pointB, direction);
			}
		}
	} else if (minCorner == yNorm2) {
		if (zeroNorm2 > xyNorm2) {
			center = cross(sPlane4f(xy, y, Vect3f(0, vMap.V_SIZE, 100)), point, pointB, direction);
			if (center.x < 0) {
				center = cross(sPlane4f(zero, y, Vect3f(0, vMap.V_SIZE, 100)), point, pointB, direction);
			}
		} else {
			center = cross(sPlane4f(zero, y, Vect3f(0, vMap.V_SIZE, 100)), point, pointB, direction);
			if (center.y > vMap.V_SIZE) {
				center = cross(sPlane4f(xy, y, Vect3f(0, vMap.V_SIZE, 100)), point, pointB, direction);
			}
		}
	} else {
		if (xNorm2 > yNorm2) {
			center = cross(sPlane4f(xy, y, Vect3f(vMap.H_SIZE, vMap.V_SIZE, 100)), point, pointB, direction);
			if (center.x > vMap.H_SIZE) {
				center = cross(sPlane4f(xy, x, Vect3f(vMap.H_SIZE, vMap.V_SIZE, 100)), point, pointB, direction);
			}
		} else {
			center = cross(sPlane4f(xy, x, Vect3f(vMap.H_SIZE, vMap.V_SIZE, 100)), point, pointB, direction);
			if (center.y > vMap.V_SIZE) {
				center = cross(sPlane4f(xy, y, Vect3f(vMap.H_SIZE, vMap.V_SIZE, 100)), point, pointB, direction);
			}
		}
	}
	return center;
}

cObjectNodeRoot* LocationNode::createLocationObject(tx3d::Location3D* location, cScene* scene) {
	tx3d::SphereLocation3D* sphere = dynamic_cast<tx3d::SphereLocation3D*>(location);
	if (sphere) {
		cObjectNodeRoot* res = scene->CreateObject("GeoTool\\res\\model\\sphere.M3D", NULL);
		updateSphereObject(sphere, res);
		res->ClearAttr(ATTRUNKOBJ_IGNORE);
//		res->SetAttr(ATTRUNKOBJ_WIREFRAME);
//		res->SetAttr(ATTRUNKOBJ_NOLIGHT);
		setObjectSelected(res, false);
		return res;
	}
	tx3d::StratumLocation3D* stratum = dynamic_cast<tx3d::StratumLocation3D*>(location);
	if (stratum) {
		cObjectNodeRoot* res = scene->CreateObject("GeoTool\\res\\model\\stratum.M3D", NULL);
		updateStratumObject(stratum, res);
		res->ClearAttr(ATTRUNKOBJ_IGNORE);
//		res->SetAttr(ATTRUNKOBJ_WIREFRAME);
//		res->SetAttr(ATTRUNKOBJ_NOLIGHT);
		setObjectSelected(res, false);
		return res;
	}
	return 0;
}

void LocationNode::setObjectSelected(cObjectNodeRoot* obj, bool selected) {
	float r = selected ? 1 : 0;
	float b = selected ? 0 : 1;
	obj->SetColor(
			&sColor4f(r * 0.2f, 0, b * 0.2f, 1.0f),		//ambient
			&sColor4f(r * 1.0f, 0, b * 1.0f, 0.4f),		//diffuse
			&sColor4f(0, 0, 0, 1.0f) );					//specular
}