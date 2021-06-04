#ifndef _LOCATION_NODE_H
#define _LOCATION_NODE_H

#include <SphereLocation3D.hpp>
#include <StratumLocation3D.hpp>
#include <Location3D.hpp>
#include <Texture3D.hpp>
#include <XMLUtils.hpp>
#include "LocationUpdater.h"
#include "Node.h"

class LocationNode : public Node {

	public :

		LocationNode(const string& paramString);
	
		bool isLocationFromNode(tx3d::Location3D* location);
		tx3d::Location3D* createLocation();

		static void updateObject(tx3d::Location3D* location, cObjectNodeRoot* object);
		static bool update(
			tx3d::Location3D* location,
			cObjectNodeRoot* object,
			const Vect3f& point,
			const Vect3f& direction,
			const Vect3f& direction2,
			LocationUpdater::ConstraintType type );
		static cObjectNodeRoot* createLocationObject(tx3d::Location3D* location, cScene* scene);
		static void setObjectSelected(cObjectNodeRoot* obj, bool selected);

	protected :

		static void updateSphereObject(tx3d::SphereLocation3D* sphere, cObjectNodeRoot* object);
		static void updateStratumObject(tx3d::StratumLocation3D* stratum, cObjectNodeRoot* object);

		static bool updateSphere(
				tx3d::SphereLocation3D* sphere,
				cObjectNodeRoot* object,
				const Vect3f& point,
				const Vect3f& direction,
				const Vect3f& direction2,
				LocationUpdater::ConstraintType type );
		static bool updateStratum(
				tx3d::StratumLocation3D* stratum,
				cObjectNodeRoot* object,
				const Vect3f& point,
				const Vect3f& direction,
				const Vect3f& direction2,
				LocationUpdater::ConstraintType type );

		static Vect3f getCrossCenter(const Vect3f& point, const Vect3f& direction);

		static Vect3f cross(sPlane4f& plane, const Vect3f& pointA, const Vect3f& pointB, const Vect3f& direction) {
			return pointA - plane.GetCross(pointA, pointB) * direction;
		}

		static const string LOCATION;

		string typeName;
		string location;
};

#endif // _LOCATION_NODE_H