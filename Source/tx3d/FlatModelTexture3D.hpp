/**
 *	"Procedural 3D Texture Library"
 *	
 *
 *	@version $Id: PerTexture3D.h,v 1.5 2002/11/07 13:50:53 ash Exp $
 *
 *	@author Alexander Porechnov (scm)
 */

#pragma once

#ifndef _TX3D_FLATMODELTEXTURE3D_H
#define _TX3D_FLATMODELTEXTURE3D_H

#include "Texture3D.hpp"

namespace tx3d {

	class FlatModelTexture3D : public Texture3D {

		public :
			FlatModelTexture3D(const string& paramString);

			FlatModelTexture3D(const FlatModelTexture3D* origin);

			FlatModelTexture3D
				(
					const Vector3D& shift,
					const Vector3D& scale,
					const string& src

				);

			FlatModelTexture3D
				(
					const Vector3D& shift,
					const Vector3D& scale,
					Vector3D* flatModel,
					int modelWidth,
					int modelHeight
				);

			~FlatModelTexture3D();
			
			void setSrc(const string& src);
			void setModel(Vector3D* flatModel, int modelWidth, int modelHeight);

			void zoom(float degree) {
				shift *= degree;
				scale /= degree;
			}

			string getSrc() const {
				return src;
			}

			string paramString() const;
//			virtual const string& getType() const = 0;

			static const string SHIFT;
			static const string SCALE;
			static const string MODEL;
			static const string WIDTH;
			static const string HEIGHT;
			static const string SRC;

			Vector3D shift;
			Vector3D scale;

		protected :

			string src;

			Vector3D* model;
			int width;
			int height;
	};

}

#endif