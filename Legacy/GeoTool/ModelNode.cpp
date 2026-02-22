#include <stdafx.h>
#include "ModelNode.h"
#include <Texture3DFactory.hpp>
#include <FlatModelXTexture3D.hpp>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

ModelNode::ModelNode(const string& paramString) : TextureNode(paramString) {

	//load texture
	string source = tx3d::XMLUtils::extractPropertyFromTag
						(
							"src",
							tx3d::XMLUtils::extractTagFromXML("texture", 0, paramString)
						 );
	texture = new tx3d::FlatModelXTexture3D(tx3d::Vector3D(0, 0, 0), tx3d::Vector3D(0.25f, 0.25f, 0.25f), source);
	texture->referenced();
	//load image
	loadImage(texture->getSrc().c_str());
}

ModelNode::ModelNode(const string& titleStr, const string& source) {
	init();
	title = titleStr;
	texture = new tx3d::FlatModelXTexture3D(tx3d::Vector3D(0, 0, 0), tx3d::Vector3D(0.25f, 0.25f, 0.25f), source);
	texture->referenced();
	//load image
	loadImage(texture->getSrc().c_str());
}

ModelNode::~ModelNode() {
	texture->released();
}

bool ModelNode::isTexureFromNode(tx3d::Texture3D* tx) {
	tx3d::FlatModelTexture3D* flat = dynamic_cast<tx3d::FlatModelTexture3D*>(tx);
	return flat && flat->getSrc() == texture->getSrc();
}

tx3d::Texture3D* ModelNode::createTexture() {
	return new tx3d::FlatModelXTexture3D(texture);
}
