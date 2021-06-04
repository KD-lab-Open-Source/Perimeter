/**
 *	"Procedural 3D Texture Library"
 *	
 *	
 *
 *	@version $Id: Location3DFactory.h,v 1.1 2002/11/07 13:49:48 ash Exp $
 *
 *	@author Alexander Porechnov (scm)
 */

#pragma once

#ifndef _TX3D_LOCATION3DFACTORY_H
#define _TX3D_LOCATION3DFACTORY_H

#include "Location3D.hpp"

namespace tx3d {

	class Location3DFactory {
		public:
			static Location3D* createLocation3D(const string& xml);

		private:
			Location3DFactory();
	};

}

#endif