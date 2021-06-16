/**
 *	"Procedural 3D Texture Library"
 *	
 *	Implementation of compound texture.
 *
 *	@version $Id: Compound3D.h,v 1.10 2002/11/07 13:50:53 ash Exp $
 *
 *	@author Alexander Porechnov (scm)
 */

#pragma once

#ifndef _TX3D_COMPOUND3D_H
#define _TX3D_COMPOUND3D_H

#include "tweaks.h"
#include <vector>
using std::vector;

#include "Primitive3D.hpp"
#include "Location3D.hpp"
#include "CachingTurbulator3D.hpp"
#include "Texture3DFactory.hpp"
#include "Location3DFactory.hpp"
#include "XMLUtils.hpp"

namespace tx3d {

	class Component : public Serializable, public SharedPointer {
		public:
			Component(const Component& component);

			Component(const string& paramString);

			Component(Texture3D *texture, bool shouldDegradate,	Location3D *location);

			~Component();

			Texture3D* getTexture() const {
				return texture;
			}

			void setTexture(Texture3D* t);

			Primitive3D* getPrimitive() const {
				return primitive;
			}

			Location3D* getLocation() const {
				return location;
			}

			void setLocation(Location3D* l);

			void zoom(float degree);
			void zoomColor(float degree);

			const string& getType() const {
				return TYPE_NAME;
			}

			string paramString() const;

			bool shouldDegradate;

			static const string TYPE_NAME;
			static const string LOCATION;
			static const string TEXTURE;
			static const string SHOULD_DEGRADATE;

		protected:
			Texture3D *texture;
			Location3D *location;
			Primitive3D *primitive;
	};

	class Compound3D : public Texture3D {
		public :

			Compound3D(const string& paramString);

			Compound3D();

			~Compound3D();

			void addTexture3D(Texture3D* texture, bool shouldDegradate, Location3D* location);

			void getColor(Vector3D* destClr, const Vector3D &v);

			void addComponent(const string& component);

			void addComponent(Component* component);

			void remove(Component* component);

			Component* getComponent(int index);

			int getComponentCount();

			const string& getType() const {
				return TYPE_NAME;
			}

			void zoom(float degree);
			void zoomColor(float degree);

			string paramString() const;

			static const string TYPE_NAME;
			static const string COMPONENT;

		protected :
			int getIndexToInsert(const Primitive3D *primitive);
			int getComponentIndex(Component* component);

			vector<Component*> components;
			vector<float> distances;

			CachingTurbulator3D *turbulator;

			//for-performance-purporses
			float t;
			int o;
			Vector3D clr;
	};

}

#endif