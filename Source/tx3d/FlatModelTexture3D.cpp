/**
 *	"Procedural 3D Texture Library"
 *	
 *
 *	@version $Id: ModelTexture3D.h,v 1.5 2002/11/07 13:50:53 ash Exp $
 *
 *	@author Alexander Porechnov (scm)
 */

#include "stdafxTX3D.h"
#include "FlatModelTexture3D.hpp"
#include "Texture3DUtils.hpp"
#include "XMLUtils.hpp"
#include "TGA.hpp"

using namespace tx3d;

const string FlatModelTexture3D::SHIFT = "shift";
const string FlatModelTexture3D::SCALE = "scale";
const string FlatModelTexture3D::WIDTH = "width";
const string FlatModelTexture3D::HEIGHT = "height";
const string FlatModelTexture3D::MODEL = "model";
const string FlatModelTexture3D::SRC = "src";


FlatModelTexture3D::FlatModelTexture3D(const string& paramString) {
	shift = 
		XMLUtils::vectorFromTag(
			XMLUtils::extractTagFromXML(SHIFT, 0, paramString)
		);
	scale = 
		XMLUtils::vectorFromTag(
			XMLUtils::extractTagFromXML(SCALE, 0, paramString)
		);
	model = 0;
	setSrc( XMLUtils::extractPropertyFromTag
						(
							SRC,
							XMLUtils::extractTagFromXML(MODEL, 0, paramString)
						 ) );
}

FlatModelTexture3D::FlatModelTexture3D(const FlatModelTexture3D* origin) {
	shift = origin->shift;
	scale = origin->scale;
	src = origin->getSrc();
	width = origin->width;
	height = origin->height;

	model = new Vector3D[width * height];
	for (int i = 0; i < width * height; i++) {
		model[i] = origin->model[i];
	}

//	model = 0;
//	setSrc(origin->getSrc());
}

FlatModelTexture3D::FlatModelTexture3D
				(
					const Vector3D& shift,
					const Vector3D& scale,
					const string& source
				) : 
					shift(shift),
					scale(scale) {
	model = 0;
	setSrc(source);
}

FlatModelTexture3D::FlatModelTexture3D
						(
							const Vector3D& shift,
							const Vector3D& scale,
							Vector3D* flatModel,
							int modelWidth,
							int modelHeight
						) : 
							shift(shift),
							scale(scale) {

	model = 0;
	src.clear();
	setModel(flatModel, modelWidth, modelHeight);
}

FlatModelTexture3D::~FlatModelTexture3D() {
	if (model) {
		delete[] model;
	}
}

void FlatModelTexture3D::setSrc(const string& source) {
	src = source;
	TGA image;
	Vector3D* flatModel;
	if (image.load(src.c_str())) {
		flatModel = new Vector3D[image.getWidth() * image.getHeight()];
		for (int u = 0; u < image.getWidth(); u++) {
			for (int k = 0; k < image.getHeight(); k++) {
				flatModel[u + k * image.getWidth()] = image.getPixel(u, k);

//				flatModel[u + k * image.Width].x = image.cMap[u * 3 + k * image.Width * 3] / 256.0f;
//				flatModel[u + k * image.Width].y = image.cMap[u * 3 + k * image.Width * 3 + 1] / 256.0f;
//				flatModel[u + k * image.Width].z = image.cMap[u * 3 + k * image.Width * 3 + 2] / 256.0f;
			}
		}
		setModel(flatModel, image.getWidth(), image.getHeight());
	} else {
//		src.clear();
		flatModel = new Vector3D[16 * 16];
		setModel(flatModel, 16, 16);
	}
	delete[] flatModel;
}

void FlatModelTexture3D::setModel(Vector3D* flatModel, int modelWidth, int modelHeight) {

	width = modelWidth + 1;
	height = modelHeight + 1;
	if (model) {
		delete[] model;
	}
	model = new Vector3D[width * height];
	int x;
	for (x = 0; x < (width - 1); x++) {
		for (int y = 0; y < (height - 1); y++) {
			model[x + width * y] = flatModel[x + y * (width - 1)];
		}
	}
	for (x = 0; x < width; x++) {
		model[x + width * (height - 1)] = model[x];
	}
	for (x = 0; x < height; x++) {
		model[(width - 1) + x * width] = model[x * width];
	}
}

string FlatModelTexture3D::paramString() const {
	return 
		  XMLUtils::vectorToTag(SHIFT, shift)
		+ XMLUtils::vectorToTag(SCALE, scale)
		+ XMLUtils::createTag(MODEL, SRC, src);
//		+ XMLUtils::intToTag(WIDTH, width - 1)
//		+ XMLUtils::intToTag(HEIGHT, height - 1);
}
