#include <stdafx.h>
#include <Clear3D.hpp>
#include <Primitive3D.hpp>
#include <Texture3DFactory.hpp>
#include <FlatModelTexture3D.hpp>
#include "SimpleNode.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const string SimpleNode::IMAGE = "image";

SimpleNode::SimpleNode(const string& paramString) : TextureNode(paramString) {

	//load texture
	texture = loadXML(paramString, TEXTURE);

	//load image
	string fileName = tx3d::XMLUtils::extractPropertyFromTag
						(
							tx3d::FlatModelTexture3D::SRC,
							tx3d::XMLUtils::extractTagFromXML(IMAGE, 0, paramString)
						 );
	char imgFile[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, imgFile);
	strcat(imgFile, "\\GeoTool\\res\\Templates\\");
	strcat(imgFile, fileName.c_str());

	loadImage(imgFile);

	//check typeName
	tx3d::Texture3D* master = createTexture();
	if (master) {
		master->referenced();
		if (master->getType() == tx3d::Clear3D::TYPE_NAME) {
			typeName = tx3d::Clear3D::TYPE_NAME;
		} else if (master->getType() == tx3d::Primitive3D::TYPE_NAME) {
			tx3d::Primitive3D* primitive = dynamic_cast<tx3d::Primitive3D*>(master);
			if (primitive->getColorizer()) {
				typeName = primitive->getColorizer()->getType();
			}
		}
		master->released();
	}
}

bool SimpleNode::isTexureFromNode(tx3d::Texture3D* tx) {
	if (tx->getType() == typeName) {
		return true;
	} else if (tx->getType() == tx3d::Primitive3D::TYPE_NAME) {
		tx3d::Primitive3D* primitive = dynamic_cast<tx3d::Primitive3D*>(tx);
		if (primitive->getColorizer()) {
			char end = *(primitive->getColorizer()->getType().end() - 1);
			if (end == 'X' || end == 'Y' || end == 'Z') {
				return typeName.substr(
									0,
									typeName.length() - 1)
					== primitive->getColorizer()->getType().substr(
									0,
									primitive->getColorizer()->getType().length() - 1);
			} else {
				return typeName == primitive->getColorizer()->getType();
			}
		}
	}
	return false;
}

tx3d::Texture3D* SimpleNode::createTexture() {
	return tx3d::Texture3DFactory::createTexture3D(texture);
}
