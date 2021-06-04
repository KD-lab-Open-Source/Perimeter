#ifndef _TEXTURE_NODE_H
#define _TEXTURE_NODE_H

#include <Texture3D.hpp>
#include <XMLUtils.hpp>
#include "Node.h"

class TextureNode : public Node {

	public :
		TextureNode(const string& paramString);
		~TextureNode();

		virtual bool isTexureFromNode(tx3d::Texture3D* tx) = 0;
		virtual tx3d::Texture3D* createTexture() = 0;

		void updateBitmap(CBitmap* bitmap);


	protected :

		TextureNode() : Node() {
		}

		void init();
		
		void loadImage(const char* fileName);

		static const string TEXTURE;
		static const int IMAGE_WIDTH;
		static const int IMAGE_HEIGHT;

		unsigned char* bits;
};

#endif // _TEXTURE_NODE_H