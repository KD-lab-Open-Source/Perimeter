/**
 *	"Procedural 3D Texture Library"
 *	
 *	
 *
 *	@version $Id: Colorizer3DFactory.h,v 1.1 2002/11/07 13:49:48 ash Exp $
 *
 *	@author Alexander Porechnov (scm)
 */

#pragma once

#ifndef _TX3D_COLORIZER3DFACTORY_H
#define _TX3D_COLORIZER3DFACTORY_H

#include "Colorizer3D.hpp"

namespace tx3d {

	class Colorizer3DFactory {
		public:
			static Colorizer3D* createColorizer3D(const string& xml);
			static Colorizer3D* createColorizer3DByType(const string& type);

		private:
			Colorizer3DFactory();
	};

}

#endif