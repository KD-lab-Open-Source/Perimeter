/**
 *	"Procedural 3D Texture Library"
 *	
 *	
 *
 *	@version $Id: Colorizer3DFactory.cpp,v 1.2 2002/11/11 15:37:33 ash Exp $
 *
 *	@author Alexander Porechnov (scm)
 */

#include "stdafxTX3D.h"
#include "Colorizer3DFactory.hpp"
#include "XMLUtils.hpp"
#include "Empty.hpp"
#include "Clouds.hpp"
#include "Marbles.hpp"
#include "Spots.hpp"
#include "Woods.hpp"
#include "Sands.hpp"

using namespace tx3d;

Colorizer3D* Colorizer3DFactory::createColorizer3D(const string& xml) {
	string type = XMLUtils::extractPropertyFromTag(XMLUtils::TYPE, xml);
	string content = XMLUtils::extractContentFromTag(xml);
	//
	if (type == Empty::TYPE_NAME) {
		return
			new Empty(content);
	} else if (type == Sand::TYPE_NAME) {
		return
			new Sand(content);
	} else if (type == SandSmooth::TYPE_NAME) {
		return
			new SandSmooth(content);
	} else if (type == Cloud::TYPE_NAME) {
		return
			new Cloud(content);
	} else if (type == CloudAbs::TYPE_NAME) {
		return
			new CloudAbs(content);
	} else if (type == CloudEmboss::TYPE_NAME) {
		return
			new CloudEmboss(content);
	} else if (type == CloudSin::TYPE_NAME) {
		return
			new CloudSin(content);
	} else if (type == MarbleSolidX::TYPE_NAME) {
		return
			new MarbleSolidX(content);
	} else if (type == MarbleChaos::TYPE_NAME) {
		return
			new MarbleChaos(content);
	} else if (type == MarbleChaosX::TYPE_NAME) {
		return
			new MarbleChaosX(content);
	} else if (type == MarbleChaosY::TYPE_NAME) {
		return
			new MarbleChaosY(content);
	} else if (type == MarbleLeopard::TYPE_NAME) {
		return
			new MarbleLeopard(content);
	} else if (type == MarbleRareX::TYPE_NAME) {
		return
			new MarbleRareX(content);
	} else if (type == MarbleRareY::TYPE_NAME) {
		return
			new MarbleRareY(content);
	} else if (type == MarbleSolidY::TYPE_NAME) {
		return
			new MarbleSolidY(content);
	} else if (type == Spots::TYPE_NAME) {
		return
			new Spots(content);
	} else if (type == Wood::TYPE_NAME) {
		return
			new Wood(content);
	} else if (type == WoodBackSin::TYPE_NAME) {
		return
			new WoodBackSin(content);
	} else if (type == WoodCos::TYPE_NAME) {
		return
			new WoodCos(content);
	} else if (type == WoodSin::TYPE_NAME) {
		return
			new WoodSin(content);
	}
	return 0;
}

Colorizer3D* Colorizer3DFactory::createColorizer3DByType(const string& type) {
	//
	if (type == Empty::TYPE_NAME) {
		return
			new Empty();
	} else if (type == Sand::TYPE_NAME) {
		return
			new Sand();
	} else if (type == SandSmooth::TYPE_NAME) {
		return
			new SandSmooth();
	} else if (type == Cloud::TYPE_NAME) {
		return
			new Cloud();
	} else if (type == CloudAbs::TYPE_NAME) {
		return
			new CloudAbs();
	} else if (type == CloudEmboss::TYPE_NAME) {
		return
			new CloudEmboss();
	} else if (type == CloudSin::TYPE_NAME) {
		return
			new CloudSin();
	} else if (type == MarbleSolidX::TYPE_NAME) {
		return
			new MarbleSolidX();
	} else if (type == MarbleChaos::TYPE_NAME) {
		return
			new MarbleChaos();
	} else if (type == MarbleChaosX::TYPE_NAME) {
		return
			new MarbleChaosX();
	} else if (type == MarbleChaosY::TYPE_NAME) {
		return
			new MarbleChaosY();
	} else if (type == MarbleLeopard::TYPE_NAME) {
		return
			new MarbleLeopard();
	} else if (type == MarbleRareX::TYPE_NAME) {
		return
			new MarbleRareX();
	} else if (type == MarbleRareY::TYPE_NAME) {
		return
			new MarbleRareY();
	} else if (type == MarbleSolidY::TYPE_NAME) {
		return
			new MarbleSolidY();
	} else if (type == Spots::TYPE_NAME) {
		return
			new Spots();
	} else if (type == Wood::TYPE_NAME) {
		return
			new Wood();
	} else if (type == WoodBackSin::TYPE_NAME) {
		return
			new WoodBackSin();
	} else if (type == WoodCos::TYPE_NAME) {
		return
			new WoodCos();
	} else if (type == WoodSin::TYPE_NAME) {
		return
			new WoodSin();
	}
	return 0;
}
