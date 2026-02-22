#ifndef _LOCATIONUPDATER_H
#define _LOCATIONUPDATER_H

class LocationUpdater {
	public:
		typedef enum {
			SHIFT_XY,
			SHIFT_ZY,
			SCALE
		} ConstraintType;

		virtual void update(
			const Vect3f& point,
			const Vect3f& direction,
			const Vect3f& direction2,
			ConstraintType type) = 0;

		virtual void setLocationsVisible(bool visible) = 0;
};

#endif // _LOCATIONUPDATER_H