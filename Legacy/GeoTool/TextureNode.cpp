#include <stdafx.h>
#include <TGA.hpp>
#include <FlatModelTexture3D.hpp>
#include "TextureNode.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const string TextureNode::TEXTURE = "texture";
const int TextureNode::IMAGE_WIDTH = 64;
const int TextureNode::IMAGE_HEIGHT = 256;

TextureNode::TextureNode(const string& paramString) : Node(paramString) {

	init();
}

TextureNode::~TextureNode() {
	if (bits) {
		delete [] bits;
	}
}

void TextureNode::init() {
	bits = new unsigned char[IMAGE_WIDTH * IMAGE_HEIGHT * 4];
}
void TextureNode::updateBitmap(CBitmap* bitmap) {
	bitmap->SetBitmapBits(IMAGE_WIDTH * IMAGE_HEIGHT * 4, bits);
}

void TextureNode::loadImage(const char* fileName) {
	tx3d::TGA image;
	if (image.loadTest(fileName)) {
		for (int y = 0; y < IMAGE_HEIGHT; y++) {
			for (int x = 0; x < IMAGE_WIDTH; x++) {
				int yBits = y * IMAGE_WIDTH;
				int yMap = y % image.getHeight();
				int xMap = x % image.getWidth();
				tx3d::Vector3D v = image.getPixel(xMap, yMap);
				bits[(yBits + x) * 4] = v.z * 255;
				bits[(yBits + x) * 4 + 1] = v.y * 255;
				bits[(yBits + x) * 4 + 2] = v.x * 255;
				bits[(yBits + x) * 4 + 3] = 0 * 255;
			}
		}
	}
}
