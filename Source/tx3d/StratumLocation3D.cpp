/**
 *	"Procedural 3D Texture Library"
 *	
 *	Stratum in plane location of texture.
 *
 *	@version $Id: StratumLocation3D.h,v 1.1 2002/11/05 17:41:59 ash Exp $
 *
 *	@author Alexander Porechnov (scm)
 */

#include "stdafxTX3D.h"
#include "StratumLocation3D.hpp"
#include "XMLUtils.hpp"

using namespace tx3d;

const string StratumLocation3D::NEAR_BOUND = "nearBound";
const string StratumLocation3D::FAR_BOUND = "farBound";

StratumLocation3D::StratumLocation3D(const string& paramString)
 : ExtendedWorldLocation3D(paramString) {
	nearBound = XMLUtils::floatFromTag(XMLUtils::extractTagFromXML(NEAR_BOUND, 0, paramString));
	farBound = XMLUtils::floatFromTag(XMLUtils::extractTagFromXML(FAR_BOUND, 0, paramString));
}

StratumLocation3D::StratumLocation3D(const StratumLocation3D* stratumLocation)
		: ExtendedWorldLocation3D(stratumLocation) {
	nearBound = stratumLocation->nearBound;
	farBound = stratumLocation->farBound;
}

string StratumLocation3D::paramString() const {
	return 
		  ExtendedWorldLocation3D::paramString()
		+ XMLUtils::floatToTag(NEAR_BOUND, nearBound)
		+ XMLUtils::floatToTag(FAR_BOUND, farBound);
}
