/**
 *	"Procedural 3D Texture Library"
 *	
 *	
 *
 *	@version $Id: Location3DFactory.cpp,v 1.2 2002/11/11 15:37:33 ash Exp $
 *
 *	@author Alexander Porechnov (scm)
 */

#include "stdafxTX3D.h"
#include "SphereLocation3D.hpp"
#include "StratumXLocation3D.hpp"
#include "StratumYLocation3D.hpp"
#include "StratumZLocation3D.hpp"
#include "WorldLocation3D.hpp"
#include "LayingLocation3D.hpp"
#include "Location3DFactory.hpp"
#include "XMLUtils.hpp"

using namespace tx3d;

Location3D* Location3DFactory::createLocation3D(const string& xml) {
	string type = XMLUtils::extractPropertyFromTag(XMLUtils::TYPE, xml);
	string content = XMLUtils::extractContentFromTag(xml);
	//
	if (type == SphereLocation3D::TYPE_NAME) {
		return
			new SphereLocation3D(content);
	} else if (type == WorldLocation3D::TYPE_NAME) {
		return
			new WorldLocation3D(content);
	} else if (type == StratumXLocation3D::TYPE_NAME) {
		return
			new StratumXLocation3D(content);
	} else if (type == StratumYLocation3D::TYPE_NAME) {
		return
			new StratumYLocation3D(content);
	} else if (type == StratumZLocation3D::TYPE_NAME) {
		return
			new StratumZLocation3D(content);
	} else if (type == LayingLocation3D::TYPE_NAME) {
		return
			new LayingLocation3D(content);
	}
	return 0;
}
