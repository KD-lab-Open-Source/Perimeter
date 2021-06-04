/**
 *	"Procedural 3D Texture Library"
 *	
 *	
 *
 *	@version $Id: Texture3DFactory.cpp,v 1.3 2002/11/11 15:37:33 ash Exp $
 *
 *	@author Alexander Porechnov (scm)
 */

#include "stdafxTX3D.h"
#include "Texture3DFactory.hpp"
#include "XMLUtils.hpp"
#include "Compound3D.hpp"
#include "PerTexture3D.hpp"
#include "Laying3D.hpp"
#include "Clear3D.hpp"
#include "FlatModelXTexture3D.hpp"
#include "FlatModelYTexture3D.hpp"
#include "FlatModelZTexture3D.hpp"

using namespace tx3d;

Texture3D* Texture3DFactory::createTexture3D(const string& xml) {
	string type = XMLUtils::extractPropertyFromTag("type", xml);
	string content = XMLUtils::extractContentFromTag(xml);

	//
	if (type == Primitive3D::TYPE_NAME) {
		return
			new Primitive3D(content);
	} else if (type == Clear3D::TYPE_NAME) {
		return
			new Clear3D(content);
	} else if (type == Laying3D::TYPE_NAME) {
		return
			new Laying3D(content);
	} else if (type == Compound3D::TYPE_NAME) {
		return
			new Compound3D(content);
	} else if (type == FlatModelXTexture3D::TYPE_NAME) {
		return
			new FlatModelXTexture3D(content);
	} else if (type == FlatModelYTexture3D::TYPE_NAME) {
		return
			new FlatModelYTexture3D(content);
	} else if (type == FlatModelZTexture3D::TYPE_NAME) {
		return
			new FlatModelZTexture3D(content);
	} else if (type == PerTexture3D::TYPE_NAME) {
		return
			new PerTexture3D(content);
	}
	
	return 0;
}