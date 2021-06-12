#ifndef _SLOT_H
#define _SLOT_H

#include <Compound3D.hpp>
#include <Location3D.hpp>

using namespace tx3d;

class Slot : public SharedPointer {

	public :

		Slot(Texture3D* texture, Location3D* location, cScene* scene);
		Slot(Component* component, cScene* scene);
		~Slot();

		void setTexture(Texture3D* texture);
		void setLocation(Location3D* location, cScene* scene);

		Texture3D* getTexture() {
			return texture;
		}

		Location3D* getLocation() {
			return location;
		}

		cObjectNodeRoot* getLocationObject() {
			return locationObject;
		}

	protected:

		void updateLocationObject(cScene* scene);

		Texture3D* texture;
		Location3D* location;
		cObjectNodeRoot* locationObject;

};

#endif //_SLOT_H

