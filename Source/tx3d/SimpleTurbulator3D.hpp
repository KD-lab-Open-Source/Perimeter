/**
 *	"Procedural 3D Texture Library"
 *	
 *	Clear implementation of "turbulation".
 *	
 *	@version $Id: SimpleTurbulator3D.h,v 1.9 2002/11/07 13:50:54 ash Exp $
 *
 *	@author Alexander Porechnov (scm)
 */

#pragma once

#ifndef _TX3D_SIMPLETURBULATOR3D_H
#define _TX3D_SIMPLETURBULATOR3D_H

#include "Turbulator3D.hpp"
#include <memory> // std::auto_ptr

namespace tx3d {

	class SimpleTurbulator3D : public Turbulator3D {
		public :

			float turbulate3D(const Vector3D &v, float persistence, int octaveCount, Interpolator3D *interpolator);

			static SimpleTurbulator3D* getSharedSimpleTurbulator3D() {
				if (!sharedInstance.get()) {
					sharedInstance.reset(new SimpleTurbulator3D());
				}
				return sharedInstance.get();
			}

		protected:
			static std::unique_ptr<SimpleTurbulator3D> sharedInstance;
			Vector3D freqV;
	};

}

#endif