/**
 *	"Procedural 3D Texture Library"
 *	
 *	
 *
 *	@version $Id: Texture3D.h,v 1.9 2002/11/07 13:50:54 ash Exp $
 *
 *	@author Alexander Porechnov (scm)
 */

#pragma once

#ifndef _TX3D_INDEXEDTEXTURE3D_H
#define _TX3D_INDEXEDTEXTURE3D_H

#include "Texture3D.hpp"
#include "Texture3DUtils.hpp"
//#include "xutil.h"

namespace tx3d {

	class IndexedTexture3D {
		public:
			IndexedTexture3D(Texture3D* texture, unsigned char* indexLattice);

			~IndexedTexture3D();

			static void fillIndexLattice(Vector3D* colorTable, unsigned char* indexLattice);
			static void fillIndexLatticeHSB(Vector3D* colorTable, unsigned char* indexLattice);

			static void fillColorTable(
								Texture3D* texture,
								Vector3D* colorTable,
								const Vector3D& lookUpStep,
								const Vector3D& startPoint = Vector3D(0.0, 0.0, 0.0),
								const Vector3D& endPoint = Vector3D(4096.0, 4096.0, 256.0),
								float treshold = 0.1,
								bool rgb = true
											);


			unsigned char getColorIndex(const Vector3D &v) {
				Vector3D clr;
				texture->getColor(&clr, v);
				int index =	  Texture3DUtils::floor(clr.x * 31.0) * 2048
							+ Texture3DUtils::floor(clr.y * 63.0) * 32
							+ Texture3DUtils::floor(clr.z * 31.0);

				return indexLattice[index & 65535];
			}

			unsigned char getHSBColorIndex(const Vector3D &v) {
				Vector3D clr;
				texture->getColor(&clr, v);
				clr = Texture3DUtils::convertRGBtoHSB(clr);
				int index =	  Texture3DUtils::floor(clr.x * 127.0) * 512
							+ Texture3DUtils::floor(clr.y * 31.0) * 16
							+ Texture3DUtils::floor(clr.z * 15.0);

				return indexLattice[index & 65535];
			}

			Texture3D* getTexture() {
				return texture;
			}

			void setTexture(Texture3D* texture);

			static string colorTableToXML(const string& tagName, const Vector3D* colorTable);

			static void extractColorTableFromXML(const string& xml, Vector3D* colorTable);

			unsigned char* indexLattice;

		protected :

			static float measureColor(const Vector3D& color, bool rgb = true) {
				if (rgb) {
					return (30 * color.x * color.x + 59 * color.y * color.y + 11 * color.z * color.z);
				} else {
					float hue = color.x;
					if (hue > 0.5f) {
						hue -= 0.5f;
					}
					if (hue < -0.5f) {
						hue += 0.5f;
					}
					return (80 * hue * hue + 10 * color.y * color.y + 10 * color.z * color.z);
				}
			}

			Texture3D* texture;
	};

}

#endif