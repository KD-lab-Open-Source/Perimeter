#ifndef _MODEL_NODE_H
#define _MODEL_NODE_H

#include "TextureNode.h"
#include <FlatModelTexture3D.hpp>

class ModelNode : public TextureNode {

	public :
		ModelNode(const string& paramString);
		ModelNode(const string& title, const string& source);
		~ModelNode();
		
		bool isTexureFromNode(tx3d::Texture3D* tx);
		tx3d::Texture3D* createTexture();
		string toXML() {
			string content = 
					tx3d::XMLUtils::createTag("title", "value", title)
				+	tx3d::XMLUtils::createTag("texture", "src", texture->getSrc());

			return tx3d::XMLUtils::createTag("node", "", "", content);
		}


	protected :

		tx3d::FlatModelTexture3D* texture;
};

#endif // _MODEL_NODE_H