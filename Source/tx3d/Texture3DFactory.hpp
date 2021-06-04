/**
 *	"Procedural 3D Texture Library"
 *	
 *	
 *
 *	@version $Id: Texture3DFactory.h,v 1.1 2002/11/07 13:49:48 ash Exp $
 *
 *	@author Alexander Porechnov (scm)
 */

#pragma once

#ifndef _TX3D_TEXTURE3DFACTORY_H
#define _TX3D_TEXTURE3DFACTORY_H

#include "Texture3D.hpp"

namespace tx3d {

	class Texture3DFactory {
		public:
			static Texture3D* createTexture3D(const string& xml);
	};

}

#endif