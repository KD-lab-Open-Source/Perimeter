#ifndef _SIMPLE_NODE_H
#define _SIMPLE_NODE_H

#include "TextureNode.h"

class SimpleNode : public TextureNode {

	public :
		SimpleNode(const string& paramString);
		
		bool isTexureFromNode(tx3d::Texture3D* tx);
		tx3d::Texture3D* createTexture();


	protected :

		static const string IMAGE;

		string typeName;
		string texture;
};

#endif // _SIMPLE_NODE_H